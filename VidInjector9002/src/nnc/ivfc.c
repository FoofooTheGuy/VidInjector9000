
/* The "Integrity VerFication Container"
 *  this container consists of several "levels" which each contain
 *  hashes of the level above chunked into tiny blocks */

#include "./internal.h"
#include "nnc/ivfc.h"
#include <string.h>

#define IVFC_MAX_HEADER_SIZE_CONST (ALIGN(0x0C + 0x18 * NNC_IVFC_MAX_LEVELS + 0x08, 0x10))

static int nnc_ivfc_read_level_descriptor(u8* data, nnc_ivfc_level_descriptor* ldesc)
{
	static u32 zeros[6] = { 0, 0, 0, 0, 0, 0 }; /* sizeof(zeros) = 0x18 */
	if (memcmp(data, zeros, sizeof(zeros)) == 0)
		return 1; /* end of data */
	ldesc->logical_offset = LE64P(&data[0x00]);
	ldesc->size = LE64P(&data[0x08]);
	ldesc->block_size_log2 = LE32P(&data[0x10]);
	return 0;
}

nnc_result nnc_read_ivfc_header(nnc_rstream* rs, nnc_ivfc* ivfc, nnc_u32 expected_levels)
{
	if (expected_levels > NNC_IVFC_MAX_LEVELS)
		return NNC_R_INVAL;

	u8 data[IVFC_MAX_HEADER_SIZE_CONST];
	result ret;
	TRY(read_at_exact(rs, 0, data, 0x14 + expected_levels * 0x18));

	if (memcmp(data, "IVFC", 4) != 0)
		return NNC_R_CORRUPT;

	ivfc->id = LE32P(&data[0x04]);
	ivfc->l0_size = LE32P(&data[0x08]);

	u32 i;
	for (i = 0; i < NNC_IVFC_MAX_LEVELS; ++i)
		if (nnc_ivfc_read_level_descriptor(&data[0x0C + 0x18 * i], &ivfc->level[i]))
			break;

	ivfc->number_levels = i;

	/* zero levels means there's not even application data.... that sounds like corruption */
	return i == 0 || (expected_levels != 0 && ivfc->number_levels != expected_levels) ? NNC_R_CORRUPT : NNC_R_OK;
}

static result nnc_ivfc_finish_block(nnc_ivfc_writer* self)
{
	/* if there is no space left for another block, we need to allocate another block of hashes */
	if (self->blocks_hashed == self->blocks_allocated)
	{
		u64 real_old_size = self->blocks_allocated * sizeof(nnc_sha256_hash);
		/* (block_size / sizeof(nnc_sha256_hash)) * sizeof(nnc_sha256_hash) = block_size */
		u8* new_hashes = realloc(self->block_hashes, real_old_size + self->block_size);
		if (new_hashes == NULL) return NNC_R_NOMEM;
		/* we need to clear the new area */
		memset(new_hashes + real_old_size, 0x00, self->block_size);
		self->block_hashes = (nnc_sha256_hash*)new_hashes;
		self->blocks_allocated += self->block_size / sizeof(nnc_sha256_hash);
	}

	nnc_crypto_sha256_finish(self->current_hash, self->block_hashes[self->blocks_hashed++]);
	/* when we've extracted the digest we need to prepare it for
	 * the next incremental hash */
	nnc_crypto_sha256_reset(self->current_hash);
	self->current_hashed_size = 0;
	return NNC_R_OK;
}

static result nnc_ivfc_wwrite(nnc_ivfc_writer* self, u8* buf, u32 size)
{
	u32 bufptr = 0, sizeleft = size;
	result ret;

	/* TODO: Check if the new write will fit in the master hash */

	/* if we have some incremental buffer left */
	if (self->current_hashed_size)
	{
		u32 will_hash = self->block_size - self->current_hashed_size;
		will_hash = MIN(size, will_hash);
		nnc_crypto_sha256_feed(self->current_hash, buf, will_hash);
		self->current_hashed_size += will_hash;
		bufptr += will_hash;
		sizeleft -= will_hash;
		if (self->current_hashed_size == self->block_size)
			TRY(nnc_ivfc_finish_block(self));
	}

	if (sizeleft)
	{
		/* we can only write chunks of self->block_size (which is power of 2 aligned) fast */
		u32 will_hash_blocks = ALIGN_DOWN(sizeleft, self->block_size) / self->block_size;
		for (u32 i = 0; i < will_hash_blocks; ++i)
		{
			nnc_crypto_sha256_feed(self->current_hash, buf + bufptr, self->block_size);
			TRY(nnc_ivfc_finish_block(self));
			bufptr += self->block_size;
			sizeleft -= self->block_size;
		}
	}

	/* and the last bit we need to write to the incremental buffer hash */
	if (sizeleft)
	{
		nnc_crypto_sha256_feed(self->current_hash, buf + bufptr, sizeleft);
		self->current_hashed_size = sizeleft;
		/* there's no chance of finishing the block here, since it would've been
		 * done in the above if branch already */
	}

	/* All /we/ do is bookkeeping for when close is called, the actual
	 * child stream should do all the writing itself directly */
	ret = NNC_WS_PCALL(self->child, write, buf, size);
	if (ret == NNC_R_OK) self->final_lv_size += size;

	return ret;
}

static result nnc_ivfc_fill_hashbuffer(nnc_ivfc_writer* self, nnc_sha256_hash** output, u8* data_to_hash, u64 datalen)
{
	/* the data must be aligned by the hash size */
	if (datalen & (sizeof(nnc_sha256_hash) - 1))
		return NNC_R_INVAL;

	u64 my_length = ALIGN((ALIGN(datalen, self->block_size) / self->block_size) * sizeof(nnc_sha256_hash), self->block_size);
	nnc_sha256_hash* hashes = malloc(my_length);
	if (!hashes) return NNC_R_NOMEM;

	/* we need to hash each block of the data */
	u32 nhashes = (ALIGN(datalen, self->block_size) / self->block_size);
	for (u32 i = 0; i < nhashes; ++i)
	{
		nnc_crypto_sha256_feed(self->current_hash, data_to_hash + self->block_size * i, self->block_size);
		nnc_crypto_sha256_finish(self->current_hash, hashes[i]);
		nnc_crypto_sha256_reset(self->current_hash);
	}

	/* the other (unused) hashes must be zero-initialized afterwards */
	memset(&hashes[nhashes], 0x00, my_length - nhashes * sizeof(nnc_sha256_hash));

	*output = hashes;
	return NNC_R_OK;
}

static result nnc_ivfc_wclose(nnc_ivfc_writer* self)
{
	/* -1 since the final level doesn't need it's own hash buffer */
	nnc_sha256_hash* hash_buffers[NNC_IVFC_MAX_LEVELS - 1] = { 0 };
	hash_buffers[self->levels - 2] = self->block_hashes;
	nnc_sha256_hash* master_hashes = NULL;

	result ret = NNC_R_OK;
	u64 pad_bytes = ALIGN(self->final_lv_size, self->block_size) - self->final_lv_size;
	/* We may still need to finish the last hash if it wasn't complete yet, let's just do that right now quickly by padding */
	TRYLBL(nnc_write_padding(NNC_WSP(self), pad_bytes), out);

	/* now we'll calculate all sizes of each level */
	u64 level_sizes[NNC_IVFC_MAX_LEVELS];

	level_sizes[self->levels - 1] = self->final_lv_size;
	for (i32 i = self->levels - 2; i != -1; --i)
	{
		/* let ln = levels[i] and lm[i + 1]; ln contains lm_size/block_size hashes, so the size is (lm_size/block_size)*hash_size, the hash used is sha256 (0x20 bytes)
		 * note that it itself is also aligned to the block size of the level above which in our implementation is just the same */
		level_sizes[i] = (ALIGN(level_sizes[i + 1], self->block_size) / self->block_size) * sizeof(nnc_sha256_hash);
	}
	u32 l0_size = (ALIGN(level_sizes[0], self->block_size) / self->block_size) * sizeof(nnc_sha256_hash);

	/* we've padded enough so we can write the hashes now! */
	/* first comes level 1, are hashes of level 2, which are hashes of level 3, which are ...
	 * and then the final (application) level we already have in self->block_hashes */
	 /* we'll store all hashes in a temporary buffer for easyness, now that we've calculated the size it should
	  * be easy enough to do */
	  /* -2 since we already have the hashes for lm (with lm = last_level - 1) */
	for (i32 i = self->levels - 3; i != -1; --i)
		TRYLBL(nnc_ivfc_fill_hashbuffer(self, &hash_buffers[i], (u8*)hash_buffers[i + 1], level_sizes[i + 1]), out);

	/* now we can hash the "master hash" */
	TRYLBL(nnc_ivfc_fill_hashbuffer(self, &master_hashes, (u8*)hash_buffers[0], level_sizes[0]), out);

	/* and now we'll write all the hashes to the file */
	for (u32 i = 0; i < self->levels - 1; ++i)
	{
		u32 aligned_size = ALIGN(level_sizes[i], self->block_size);
		TRYLBL(NNC_WS_PCALL(self->child, write, (u8*)hash_buffers[i], aligned_size), out);
	}

	u32 return_pos = NNC_WS_PCALL0(self->child, tell);
	/* Now we can write the header and level 0, after we seek and seek back to the end */
	TRYLBL(NNC_WS_PCALL(self->child, seek, self->header_pos), out);

	u32 block_size_log2 = nnc_log2(self->block_size);

	/* and finally we can build the header */
	u8 ivfc_header_buf[IVFC_MAX_HEADER_SIZE_CONST];

	/* "Level 0" header */
	memcpy(&ivfc_header_buf[0x00], "IVFC", 4);
	U32P(&ivfc_header_buf[0x04]) = LE32(self->id);
	U32P(&ivfc_header_buf[0x08]) = LE32(l0_size); /* also known as the "master hash", although there may be more than one */

	u64 logical_offset = 0;
	for (u32 i = 0; i < self->levels; ++i)
	{
		U64P(&ivfc_header_buf[0x0C + 0x00 + i * 0x18]) = LE64(logical_offset);   /* logical offset  */
		U64P(&ivfc_header_buf[0x0C + 0x08 + i * 0x18]) = LE64(level_sizes[i]);   /* level size      */
		U32P(&ivfc_header_buf[0x0C + 0x10 + i * 0x18]) = LE32(block_size_log2);  /* block size log2 */
		U32P(&ivfc_header_buf[0x0C + 0x14 + i * 0x18]) = 0;                      /* reserved        */
		logical_offset += ALIGN(level_sizes[i], self->block_size);
	}

	u32 header_size = 0x0C + 0x18 * self->levels + 0x08;
	u32 real_size = ALIGN(header_size, 0x10);
	/* Footer of the header */
	U32P(&ivfc_header_buf[header_size - 0x04]) = 0;
	U32P(&ivfc_header_buf[header_size - 0x08]) = LE32(header_size);  /* here goes the size of the IVFC header (without the alignment), after all the levels */
	/* the rest of the header is aligned to 0x10 (?) */
	memset(&ivfc_header_buf[header_size], 0x00, real_size - header_size);

	/* now we can write that block + lv0 hashes */
	TRYLBL(NNC_WS_PCALL(self->child, write, ivfc_header_buf, real_size), out);
	/* and now we can finally write level 0 aka the "master hash",
	 * we can just use the size of the padding required actually since nnc_ivfc_wwrite ensures the master hashes will
	 * never be bigger than we can actually safely store in one block */
	TRYLBL(NNC_WS_PCALL(self->child, write, (u8*)master_hashes, ALIGN(real_size + l0_size, self->block_size) - real_size), out);

	/* and finally restore the position */
	TRYLBL(NNC_WS_PCALL(self->child, seek, return_pos), out);

out:
	/* And finally we can free up our own resources */
	nnc_crypto_sha256_free(self->current_hash);
	free(master_hashes);
	for (u32 i = 0; i < NNC_IVFC_MAX_LEVELS - 1; ++i)
		free(hash_buffers[i]);
	return ret;
}

static u32 nnc_ivfc_wtell(nnc_ivfc_writer* self)
{
	return self->child->funcs->tell(self->child);
}

static nnc_wstream_funcs nnc_ivfc_wfuncs = {
	.write = (nnc_write_func)nnc_ivfc_wwrite,
	.close = (nnc_wclose_func)nnc_ivfc_wclose,
	.tell = (nnc_wtell_func)nnc_ivfc_wtell,
};

nnc_result nnc_open_ivfc_writer(nnc_ivfc_writer* self, nnc_wstream* child, nnc_u32 levels, nnc_u32 id, nnc_u32 block_size)
{
	self->funcs = &nnc_ivfc_wfuncs;
	self->child = child;
	self->levels = levels;
	self->id = id;
	self->block_size = block_size;
	self->final_lv_size = 0;
	self->header_pos = child->funcs->tell(child);

	if (!child->funcs->seek || block_size == 0 || block_size & (block_size - 1) || levels > NNC_IVFC_MAX_LEVELS)
		return NNC_R_INVAL;

	/* Write IVFC header dummy + l0 which is filled on close */
	nnc_result res = nnc_write_padding(child, ALIGN(0x14 + 0x18 * levels, block_size));
	if (res != NNC_R_OK) return res;

	self->current_hashed_size = 0;
	self->blocks_allocated = block_size / sizeof(nnc_sha256_hash);
	self->blocks_hashed = 0;
	self->block_hashes = malloc(block_size); /* (block_size / sizeof(nnc_sha256_hash)) * sizeof(nnc_sha256_hash) = block_size */
	if (!self->block_hashes) return NNC_R_NOMEM;
	memset(self->block_hashes, 0x00, block_size); /* we need to zero-initialize */

	res = nnc_crypto_sha256_incremental(&self->current_hash);
	if (res != NNC_R_OK)
	{
		free(self->block_hashes);
		return res;
	}

	return NNC_R_OK;
}

void nnc_ivfc_abort_write(nnc_ivfc_writer* self)
{
	nnc_crypto_sha256_free(self->current_hash);
}