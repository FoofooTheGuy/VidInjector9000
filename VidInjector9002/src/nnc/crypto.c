
#include "mbedtls/mbedtls/version.h"
#include "mbedtls/mbedtls/sha256.h"
#include "mbedtls/mbedtls/sha1.h"
#include "mbedtls/mbedtls/aes.h"
#include "nnc/crypto.h"
#include "nnc/ticket.h"
#include "nnc/ncch.h"
#include <stdlib.h>
#include <string.h>
#include "./internal.h"

/* In MbedTLS version 2 the normal functions were marked deprecated
 * you were supposed to use *_ret, but in mbedTLS version 3+ the
 * *_ret functions had the functions renamed to have the _ret suffix removed */
#if MBEDTLS_VERSION_MAJOR == 2
	#define mbedtls_sha256_starts mbedtls_sha256_starts_ret
	#define mbedtls_sha256_update mbedtls_sha256_update_ret
	#define mbedtls_sha256_finish mbedtls_sha256_finish_ret
	#define mbedtls_sha1_starts mbedtls_sha1_starts_ret
	#define mbedtls_sha1_update mbedtls_sha1_update_ret
	#define mbedtls_sha1_finish mbedtls_sha1_finish_ret
#endif

nnc_result nnc_crypto_sha256_incremental(nnc_sha256_incremental_hash *self)
{
	*self = malloc(sizeof(mbedtls_sha256_context));
	if(!*self) return NNC_R_NOMEM;
	mbedtls_sha256_init(*self);
	mbedtls_sha256_starts(*self, 0);
	return NNC_R_OK;
}

void nnc_crypto_sha256_feed(nnc_sha256_incremental_hash self, u8 *data, u32 length)
{
	mbedtls_sha256_update(self, data, length);
}

void nnc_crypto_sha256_finish(nnc_sha256_incremental_hash self, nnc_sha256_hash digest)
{
	mbedtls_sha256_finish(self, digest);
	mbedtls_sha256_free(self);
}

void nnc_crypto_sha256_reset(nnc_sha256_incremental_hash self)
{
	mbedtls_sha256_init(self);
	mbedtls_sha256_starts(self, 0);
}

void nnc_crypto_sha256_free(nnc_sha256_incremental_hash self)
{
	free(self);
}

static result hasher_writer_write(nnc_hasher_writer *self, u8 *buf, u32 size)
{
	u32 to_hash = self->lim ? MIN(self->lim - self->hashed, size) : size;
	nnc_crypto_sha256_feed(self->hash, buf, to_hash);
	self->hashed += to_hash;
	return self->child->funcs->write(self->child, buf, size);
}

static result hasher_writer_wclose(nnc_hasher_writer *self) { nnc_crypto_sha256_free(self->hash); return NNC_R_OK; }
static result hasher_writer_wtell(nnc_hasher_writer *self)  { return self->child->funcs->tell(self->child); }

static const nnc_wstream_funcs hasher_writer_wfuncs = {
	.write = (nnc_write_func)  hasher_writer_write,
	.close = (nnc_wclose_func) hasher_writer_wclose,
	.tell  = (nnc_wtell_func)  hasher_writer_wtell,
};

nnc_result nnc_open_hasher_writer(nnc_hasher_writer *self, nnc_wstream *child, nnc_u32 limit)
{
	self->funcs  = &hasher_writer_wfuncs;
	self->child  = child;
	self->lim    = limit;
	self->hashed = 0;
	return nnc_crypto_sha256_incremental(&self->hash);
}

void nnc_hasher_writer_digest(nnc_hasher_writer *self, nnc_sha256_hash digest)
{
	nnc_crypto_sha256_finish(self->hash, digest);
	hasher_writer_wclose(self);
}

void nnc_hasher_writer_digest_reset(nnc_hasher_writer *self, nnc_sha256_hash digest)
{
	nnc_crypto_sha256_finish(self->hash, digest);
	nnc_crypto_sha256_reset(self->hash);
}


result nnc_crypto_sha256_part(nnc_rstream *rs, nnc_sha256_hash digest, u32 size)
{
	mbedtls_sha256_context ctx;
	mbedtls_sha256_init(&ctx);
	mbedtls_sha256_starts(&ctx, 0);
	u8 block[BLOCK_SZ];
	u32 read_left = size, next_read = MIN(size, BLOCK_SZ), read_ret;
	result ret;
	while(read_left != 0)
	{
		ret = NNC_RS_PCALL(rs, read, block, next_read, &read_ret);
		if(ret != NNC_R_OK) goto out;
		if(read_ret != next_read) { ret = NNC_R_TOO_SMALL; goto out; }
		mbedtls_sha256_update(&ctx, block, read_ret);
		read_left -= next_read;
		next_read = MIN(read_left, BLOCK_SZ);
	}
	mbedtls_sha256_finish(&ctx, digest);
	ret = NNC_R_OK;
out:
	mbedtls_sha256_free(&ctx);
	return ret;
}

result nnc_crypto_sha1_part(nnc_rstream *rs, nnc_sha1_hash digest, u32 size)
{
	mbedtls_sha1_context ctx;
	mbedtls_sha1_init(&ctx);
	mbedtls_sha1_starts(&ctx);
	u8 block[BLOCK_SZ];
	u32 read_left = size, next_read = MIN(size, BLOCK_SZ), read_ret;
	result ret;
	while(read_left != 0)
	{
		ret = NNC_RS_PCALL(rs, read, block, next_read, &read_ret);
		if(ret != NNC_R_OK) goto out;
		if(read_ret != next_read) { ret = NNC_R_TOO_SMALL; goto out; }
		mbedtls_sha1_update(&ctx, block, read_ret);
		read_left -= next_read;
		next_read = MIN(read_left, BLOCK_SZ);
	}
	mbedtls_sha1_finish(&ctx, digest);
	ret = NNC_R_OK;
out:
	mbedtls_sha1_free(&ctx);
	return ret;
}


result nnc_crypto_sha256_stream(nnc_rstream *rs, nnc_sha256_hash digest)
{
	return nnc_crypto_sha256_part(rs, digest, NNC_RS_PCALL0(rs, size) - NNC_RS_PCALL0(rs, tell));
}

void nnc_crypto_sha256_buffer(nnc_u8 *data, nnc_u32 size, nnc_sha256_hash digest)
{
	nnc_memory mem;
	nnc_mem_open(&mem, data, size);
	nnc_crypto_sha256_stream(NNC_RSP(&mem), digest);
}

bool nnc_crypto_hasheq(nnc_sha256_hash a, nnc_sha256_hash b)
{
	return memcmp(a, b, sizeof(nnc_sha256_hash)) == 0;
}

result nnc_crypto_sha256(const u8 *buf, nnc_sha256_hash digest, u32 size)
{
	mbedtls_sha256_context ctx;
	mbedtls_sha256_init(&ctx);
	mbedtls_sha256_starts(&ctx, 0);
	mbedtls_sha256_update(&ctx, buf, size);
	mbedtls_sha256_finish(&ctx, digest);
	mbedtls_sha256_free(&ctx);
	return NNC_R_OK;
}

nnc_result nnc_seeds_seeddb(nnc_rstream *rs, nnc_seeddb *seeddb)
{
	u8 buf[0x20];
	result ret;
	seeddb->size = 0;
	u32 expected_size;
	TRY(read_exact(rs, buf, 0x10));
	expected_size = LE32P(&buf[0x00]);
	if(!(seeddb->entries = malloc(expected_size * sizeof(struct nnc_seeddb_entry))))
		return NNC_R_NOMEM;
	for(u32 i = 0; i < expected_size; ++i, ++seeddb->size)
	{
		TRY(read_exact(rs, buf, 0x20));
		memcpy(seeddb->entries[i].seed, &buf[0x08], NNC_SEED_SIZE);
		seeddb->entries[i].title_id = LE64P(&buf[0x00]);
	}
	return NNC_R_OK;
}

result nnc_scan_seeddb(nnc_seeddb *seeddb)
{
	char path[SUP_FILE_NAME_LEN];
	if(!find_support_file("seeddb.bin", path))
		return NNC_R_NOT_FOUND;
	nnc_file f;
	result ret;
	TRY(nnc_file_open(&f, path));
	ret = nnc_seeds_seeddb(NNC_RSP(&f), seeddb);
	NNC_RS_CALL0(f, close);
	return ret;
}

u8 *nnc_get_seed(nnc_seeddb *seeddb, u64 tid)
{
	for(u32 i = 0; i < seeddb->size; ++i)
	{
		if(seeddb->entries[i].title_id == tid)
			return seeddb->entries[i].seed;
	}
	return NULL;
}

void nnc_free_seeddb(nnc_seeddb *seeddb)
{
	free(seeddb->entries);
}

enum keyfield {
	DEFAULT = 0x02,
	DEV     = 0x04,
	RETAIL  = 0x08,

	TYPE_FIELD = DEV | RETAIL,
};

#define TYPE_FLAG(sel) (sel == NNC_KEYSET_RETAIL ? RETAIL : DEV)

static const struct _kstore {
	const u128 kx_ncch0;
	const u128 kx_ncch1;
	const u128 kx_ncchA;
	const u128 kx_ncchB;
	const u128 ky_comy0;
	const u128 ky_comy1;
	const u128 ky_comy2;
	const u128 ky_comy3;
	const u128 ky_comy4;
	const u128 ky_comy5;
} default_keys[2] =
{
	{	/* retail */
		.kx_ncch0 = NNC_HEX128(0xB98E95CECA3E4D17,1F76A94DE934C053),
		.kx_ncch1 = NNC_HEX128(0xCEE7D8AB30C00DAE,850EF5E382AC5AF3),
		.kx_ncchA = NNC_HEX128(0x82E9C9BEBFB8BDB8,75ECC0A07D474374),
		.kx_ncchB = NNC_HEX128(0x45AD04953992C7C8,93724A9A7BCE6182),
		.ky_comy0 = NNC_HEX128(0x64C5FD55DD3AD988,325BAAEC5243DB98),
		.ky_comy1 = NNC_HEX128(0x4AAA3D0E27D4D728,D0B1B433F0F9CBC8),
		.ky_comy2 = NNC_HEX128(0xFBB0EF8CDBB0D8E4,53CD99344371697F),
		.ky_comy3 = NNC_HEX128(0x25959B7AD0409F72,684198BA2ECD7DC6),
		.ky_comy4 = NNC_HEX128(0x7ADA22CAFFC476CC,8297A0C7CEEEEEBE),
		.ky_comy5 = NNC_HEX128(0xA5051CA1B37DCF3A,FBCF8CC1EDD9CE02),
	},
	{	/* dev */
		.kx_ncch0 = NNC_HEX128(0x510207515507CBB1,8E243DCB85E23A1D),
		.kx_ncch1 = NNC_HEX128(0x81907A4B6F1B4732,3A677974CE4AD71B),
		.kx_ncchA = NNC_HEX128(0x304BF1468372EE64,115EBD4093D84276),
		.kx_ncchB = NNC_HEX128(0x6C8B2944A0726035,F941DFC018524FB6),
		.ky_comy0 = NNC_HEX128(0x55A3F872BDC80C55,5A654381139E153B),
		.ky_comy1 = NNC_HEX128(0x4434ED14820CA1EB,AB82C16E7BEF0C25),
		.ky_comy2 = NNC_HEX128(0xF62E3F958E28A21F,289EEC71A86629DC),
		.ky_comy3 = NNC_HEX128(0x2B49CB6F9998D9AD,94F2EDE7B5DA3E27),
		.ky_comy4 = NNC_HEX128(0x750552BFAA1C0407,55C8D59A55F9AD1F),
		.ky_comy5 = NNC_HEX128(0xAADA4CA8F6E5A977,E0A0F9E476CF0D63),
	}
};

nnc_result nnc_keyset_default(nnc_keyset *ks, u8 setsel)
{
	const struct _kstore *s = &default_keys[setsel];
	u8 type = ks->flags & TYPE_FIELD, mtype = TYPE_FLAG(setsel);
	if(type & mtype) return NNC_R_MISMATCH;
	ks->kx_ncch0 = s->kx_ncch0;
	ks->kx_ncch1 = s->kx_ncch1;
	ks->kx_ncchA = s->kx_ncchA;
	ks->kx_ncchB = s->kx_ncchB;
	ks->ky_comy0 = s->ky_comy0;
	ks->ky_comy1 = s->ky_comy1;
	ks->ky_comy2 = s->ky_comy2;
	ks->ky_comy3 = s->ky_comy3;
	ks->ky_comy4 = s->ky_comy4;
	ks->ky_comy5 = s->ky_comy5;
	ks->flags |= DEFAULT | mtype;
	return NNC_R_OK;
}

static const u128 C1_b = NNC_HEX128(0x1FF9E9AAC5FE0408,024591DC5D52768A);
static const u128 *C1 = &C1_b;

static const u128 C2_b = NNC_HEX128(0xFFFEFB4E29590258,2A680F5F1A4F3E79);
static const u128 *C2 = &C2_b;

static const u128 system_fixed_key = NNC_HEX128(0x527CE630A9CA305F,3696F3CDE954194B);
static const u128 fixed_key = NNC_HEX128(0x0000000000000000,0000000000000000);

static void hwkgen_3ds(u128 *output, u128 *kx, u128 *ky)
{
	/* NormalKey = (((KeyX ROL 2) XOR KeyY) + C1) ROR 41 */
	*output = *kx;
	nnc_u128_rol(output, 2);
	nnc_u128_xor(output, ky);
	nnc_u128_add(output, C1);
	nnc_u128_ror(output, 41);
}

static void hwkgen_dsi(u128 *output, u128 *kx, u128 *ky)
{
	/* NormalKey = ((KeyX XOR KeyY) + C2) ROL 42 */
	*output = *kx;
	nnc_u128_xor(output, ky);
	nnc_u128_add(output, C2);
	nnc_u128_rol(output, 42);
}

static void hwkgen(nnc_ncch_header *ncch, u128 *output, u128 *kx, u128 *ky)
{
	if(nnc_tid_category(ncch->title_id) & NNC_TID_CAT_TWL)
		hwkgen_dsi(output, kx, ky);
	else
		hwkgen_3ds(output, kx, ky);
}

nnc_result nnc_keyy_seed(nnc_ncch_header *ncch, nnc_u128 *keyy, u8 seed[NNC_SEED_SIZE])
{
	nnc_sha256_hash hashbuf;
	nnc_u8 strbuf[0x20];
	memcpy(strbuf, seed, NNC_SEED_SIZE);
	nnc_u64 title_id_int = LE64(ncch->title_id);
	memcpy(strbuf + NNC_SEED_SIZE, &title_id_int, sizeof(ncch->title_id));
	nnc_crypto_sha256(strbuf, hashbuf, 0x18);
	if(memcmp(hashbuf, ncch->seed_hash, 4) != 0)
		return NNC_R_CORRUPT;
	nnc_u128_bytes_be(&ncch->keyy, strbuf);
	memcpy(strbuf + 0x10, seed, NNC_SEED_SIZE);
	nnc_crypto_sha256(strbuf, hashbuf, 0x20);
	*keyy = nnc_u128_import_be(hashbuf);
	return NNC_R_OK;
}

static result ky_from_ncch(u128 *ky, nnc_ncch_header *ncch, nnc_seeddb *seeddb)
{
	result ret;
	/* We need to decrypt the keyY first */
	if(ncch->flags & NNC_NCCH_USES_SEED)
	{
		if(!seeddb) return NNC_R_SEED_NOT_FOUND;
		u8 *seed;
		if(!(seed = nnc_get_seed(seeddb, ncch->title_id)))
			return NNC_R_SEED_NOT_FOUND;
		TRY(nnc_keyy_seed(ncch, ky, seed));
	}
	/* it's fine to use */
	else *ky = ncch->keyy;
	return NNC_R_OK;
}

static void key_fixed(nnc_ncch_header *ncch, u128 *output)
{
	*output = nnc_tid_category(ncch->title_id) & NNC_TID_CAT_SYSTEM
		? system_fixed_key : fixed_key;
}

result nnc_key_menu_info(u128 *output, nnc_keyset *ks, nnc_ncch_header *ncch)
{
	if(ncch->flags & NNC_NCCH_NO_CRYPTO)
		return NNC_R_INVAL;
	if(ncch->flags & NNC_NCCH_FIXED_KEY)
		return key_fixed(ncch, output), NNC_R_OK;

	if(!(ks->flags & DEFAULT)) return NNC_R_KEY_NOT_FOUND;
	/* "menu info" always uses keyslot 0x2C and the unencrypted
	 * keyy even if NNC_NCCH_USES_SEED is set (!!!) */
	hwkgen(ncch, output, &ks->kx_ncch0, &ncch->keyy);
	return NNC_R_OK;
}

result nnc_key_content(u128 *output, nnc_keyset *ks, nnc_seeddb *seeddb,
	nnc_ncch_header *ncch)
{
	if(ncch->flags & NNC_NCCH_NO_CRYPTO)
		return NNC_R_INVAL;
	if(ncch->flags & NNC_NCCH_FIXED_KEY)
		return key_fixed(ncch, output), NNC_R_OK;

	result ret; u128 ky; TRY(ky_from_ncch(&ky, ncch, seeddb));

	switch(ncch->crypt_method)
	{
#define CASE(val, key, dep) case val: if(!(ks->flags & (dep))) return NNC_R_KEY_NOT_FOUND; hwkgen(ncch, output, &ks->kx_ncch##key, &ky); break;
	CASE(0x00, 0, DEFAULT)
	CASE(0x01, 1, DEFAULT)
	CASE(0x0A, A, DEFAULT)
	CASE(0x0B, B, DEFAULT)
#undef CASE
	default: return NNC_R_NOT_FOUND;
	}

	return NNC_R_OK;
}

nnc_result nnc_fill_keypair(nnc_keypair *output, nnc_keyset *ks, nnc_seeddb *seeddb,
	struct nnc_ncch_header *ncch)
{
	if(ncch->flags & NNC_NCCH_NO_CRYPTO)
		return NNC_R_OK; // no need to do any key generating
	result ret;
	TRY(nnc_key_content(&output->secondary, ks, seeddb, ncch));
	return nnc_key_menu_info(&output->primary, ks, ncch);
}

result nnc_get_ncch_iv(struct nnc_ncch_header *ncch, u8 for_section,
	u8 counter[0x10])
{
	if(ncch->flags & NNC_NCCH_NO_CRYPTO)
		return NNC_R_INVAL;
	if(for_section < NNC_SECTION_EXHEADER || for_section > NNC_SECTION_ROMFS)
		return NNC_R_INVAL;
	u64 *ctr64 = (u64 *) counter;
	u32 *ctr32 = (u32 *) counter;
	switch(ncch->version)
	{
	case 0:
	case 2:
		ctr64[0] = BE64(ncch->partition_id);
		ctr64[1] = 0;
		counter[8] = for_section;
		break;
	case 1:
		ctr64[0] = LE64(ncch->partition_id);
		ctr32[2] = 0;
		switch(for_section)
		{
		case NNC_SECTION_EXHEADER:
			ctr32[3] = BE32(NNC_MEDIA_UNIT);
			break;
		case NNC_SECTION_EXEFS:
			ctr32[3] = BE32(NNC_MU_TO_BYTE(ncch->exefs_offset));
			break;
		case NNC_SECTION_ROMFS:
			ctr32[3] = BE32(NNC_MU_TO_BYTE(ncch->romfs_offset));
			break;
		}
		break;
	default:
		return NNC_R_UNSUPPORTED;
	}
	return NNC_R_OK;
}

struct generic_crypto_obj {
	const void *funcs;
	void *crypto_ctx;
	void *child;
	nnc_u8 last_unaligned_block[0x10];
	/* crypto-method specific data */
	nnc_u8 additional_data[];
};

typedef void   (*crypto_decrypt_func)(struct generic_crypto_obj *self, u32 size, u8 *buf);
typedef result (*crypto_redo_iv_func)(struct generic_crypto_obj *self, u32 pos);

static result do_crypto_seek(struct generic_crypto_obj *self, u32 pos, crypto_redo_iv_func redo_iv)
{
	u32 cpos = NNC_RS_PCALL0(self->child, tell);
	nnc_result ret;
	/* i doubt this will happen but it's here anyway
	 * to save a bit of time. */
	if(cpos == pos) return NNC_R_OK;
	if(pos % 0x10 == 0)
	{
		TRY(redo_iv(self, pos));
	}
	else
	{
		/* we need to do the slightly more complicated version */
		u32 aligned = ALIGN_DOWN(pos, 0x10);
		NNC_RS_PCALL(self->child, seek_abs, aligned);
		TRY(redo_iv(self, aligned));
		u32 totalRead;
		ret = NNC_RS_PCALL(self, read, self->last_unaligned_block, 0x10, &totalRead);
		if(ret != NNC_R_OK || totalRead != 0x10)
			return NNC_R_TOO_SMALL;
	}
	return NNC_RS_PCALL(self->child, seek_abs, pos);
}

static result do_crypto_read(struct generic_crypto_obj *self, u8 *buf, u32 max, u32 *totalRead, crypto_decrypt_func decrypt)
{
	u32 offset = NNC_RS_PCALL0(self->child, tell);
	u32 real_read = 0;
	/* if the starting offset is not aligned we need to do a little more fuckery */
	if(offset % 0x10 != 0)
	{
		/* TODO: This does not account for case if the last read was at the end */
		u8 skip = offset & (0x10 - 1);
		u8 need = 0x10 - skip;
		/* for the exceptional case that need > max */
		need = MIN(need, max);
		memcpy(buf, self->last_unaligned_block + skip, need);
		real_read += need;
		buf += need;
		max -= need;
		/* seek to the next block so we don't read data twice */
		NNC_RS_PCALL(self->child, seek_rel, need);
	}

	u32 aligned_max = ALIGN_DOWN(max, 0x10);

	result ret;
	bool full_read;
	if(aligned_max)
	{
		TRY(NNC_RS_PCALL(self->child, read, buf, aligned_max, totalRead));
		full_read = *totalRead == aligned_max;
		if(*totalRead % 0x10 != 0)
		{
			/* this could theorically be handled but i don't think it matters much */
			return NNC_R_BAD_ALIGN;
		}
		decrypt(self, *totalRead, buf);
		real_read += *totalRead;
		buf += *totalRead;
		max -= *totalRead;
	} else full_read = true;

	/* if we still need to read some unaligned data: */
	if(full_read && max)
	{
		TRY(NNC_RS_PCALL(self->child, read, self->last_unaligned_block, 0x10, totalRead));
		if(*totalRead != 0x10)
			memset(self->last_unaligned_block + *totalRead, 0x00, 0x10 - *totalRead);
		decrypt(self, 0x10, self->last_unaligned_block);
		u8 applicable_read = MIN(*totalRead, max);
		/* "unread" the extra bytes we read so we get the correct offset */
		TRY(NNC_RS_PCALL(self->child, seek_abs, NNC_RS_PCALL0(self->child, tell) - (0x10 - applicable_read)));
		memcpy(buf, self->last_unaligned_block, applicable_read);
		real_read += applicable_read;
	}
	*totalRead = real_read;
	return NNC_R_OK;
}

/* nnc_aes_ctr */

static result redo_ctr_iv(nnc_aes_ctr *ac, u32 offset)
{
	u128 ctr = NNC_PROMOTE128(offset / 0x10);
	nnc_u128_add(&ctr, &ac->iv);
	nnc_u128_bytes_be(&ctr, ac->ctr);
	return NNC_R_OK;
}

static void aes_ctr_decrypt(nnc_aes_ctr *self, u32 size, u8 *buf)
{
	size_t of = 0;
	u8 block[0x10];
	mbedtls_aes_crypt_ctr(self->crypto_ctx, size, &of, self->ctr, block, buf, buf);
}

static result aes_ctr_read(nnc_aes_ctr *self, u8 *buf, u32 max, u32 *totalRead)
{
	return do_crypto_read((struct generic_crypto_obj *) self, buf, max, totalRead, (crypto_decrypt_func) aes_ctr_decrypt);
}

static result aes_ctr_seek_abs(nnc_aes_ctr *self, u32 pos)
{
	return do_crypto_seek((struct generic_crypto_obj *) self, pos, (crypto_redo_iv_func) redo_ctr_iv);
}

static result aes_ctr_seek_rel(nnc_aes_ctr *self, u32 pos)
{
	return aes_ctr_seek_abs(self, NNC_RS_PCALL0(self->child, tell) + pos);
}

static u32 aes_ctr_size(nnc_aes_ctr *self)
{
	return NNC_RS_PCALL0(self->child, size);
}

static u32 aes_ctr_tell(nnc_aes_ctr *self)
{
	return NNC_RS_PCALL0(self->child, tell);
}

static void aes_ctr_close(nnc_aes_ctr *self)
{
	mbedtls_aes_free(self->crypto_ctx);
	free(self->crypto_ctx);
}

static const nnc_rstream_funcs aes_ctr_funcs = {
	.read = (nnc_read_func) aes_ctr_read,
	.seek_abs = (nnc_seek_abs_func) aes_ctr_seek_abs,
	.seek_rel = (nnc_seek_rel_func) aes_ctr_seek_rel,
	.size = (nnc_size_func) aes_ctr_size,
	.close = (nnc_close_func) aes_ctr_close,
	.tell = (nnc_tell_func) aes_ctr_tell,
};

nnc_result nnc_aes_ctr_open(nnc_aes_ctr *self, nnc_rstream *child, u128 *key, u8 iv[0x10])
{
	self->funcs = &aes_ctr_funcs;
	if(!(self->crypto_ctx = malloc(sizeof(mbedtls_aes_context))))
		return NNC_R_NOMEM;
	self->iv = nnc_u128_import_be(iv);
	self->child = child;

	u8 buf[0x10];
	nnc_u128_bytes_be(key, buf);
	mbedtls_aes_setkey_enc(self->crypto_ctx, buf, 128);

	redo_ctr_iv(self, 0);
	return NNC_R_OK;
}

static nnc_result redo_cbc_iv(nnc_aes_cbc *self, u32 offset)
{
	if(offset == 0) memcpy(self->iv, self->init_iv, 0x10);
	else
	{
		result ret;
		/* the new IV is the previous encrypted block */
		TRY(NNC_RS_PCALL(self->child, seek_abs, offset - 16));
		u32 read;
		ret = NNC_RS_PCALL(self->child, read, self->iv, 16, &read);
		if(ret != NNC_R_OK || read != 0x10)
			return NNC_R_TOO_SMALL;
	}
	return NNC_R_OK;
}

static void aes_cbc_decrypt(nnc_aes_cbc *self, u32 size, u8 *buf)
{
	mbedtls_aes_crypt_cbc(self->crypto_ctx, MBEDTLS_AES_DECRYPT, size, self->iv, buf, buf);
}

static result aes_cbc_read(nnc_aes_cbc *self, u8 *buf, u32 max, u32 *totalRead)
{
	return do_crypto_read((struct generic_crypto_obj *) self, buf, max, totalRead, (crypto_decrypt_func) aes_cbc_decrypt);
}

static result aes_cbc_seek_abs(nnc_aes_cbc *self, u32 pos)
{
	return do_crypto_seek((struct generic_crypto_obj *) self, pos, (crypto_redo_iv_func) redo_cbc_iv);
}

static result aes_cbc_seek_rel(nnc_aes_cbc *self, u32 pos)
{
	return aes_cbc_seek_abs(self, NNC_RS_PCALL0(self->child, tell) + pos);
}

static u32 aes_cbc_size(nnc_aes_cbc *self)
{
	return NNC_RS_PCALL0(self->child, size);
}

static u32 aes_cbc_tell(nnc_aes_cbc *self)
{
	return NNC_RS_PCALL0(self->child, tell);
}

static void aes_cbc_close(nnc_aes_cbc *self)
{
	mbedtls_aes_free(self->crypto_ctx);
	free(self->crypto_ctx);
}

static const nnc_rstream_funcs aes_cbc_funcs = {
	.read = (nnc_read_func) aes_cbc_read,
	.seek_abs = (nnc_seek_abs_func) aes_cbc_seek_abs,
	.seek_rel = (nnc_seek_rel_func) aes_cbc_seek_rel,
	.size = (nnc_size_func) aes_cbc_size,
	.close = (nnc_close_func) aes_cbc_close,
	.tell = (nnc_tell_func) aes_cbc_tell,
};

static result init_aes_cbc(nnc_aes_cbc *self, void *child, u8 key[0x10], u8 iv[0x10], bool set_deckey)
{
	if(!(self->crypto_ctx = malloc(sizeof(mbedtls_aes_context))))
		return NNC_R_NOMEM;
	memcpy(self->init_iv, iv, 0x10);
	memcpy(self->iv, iv, 0x10);
	self->child = child;

	if(set_deckey) mbedtls_aes_setkey_dec(self->crypto_ctx, key, 128);
	else           mbedtls_aes_setkey_enc(self->crypto_ctx, key, 128);
	return NNC_R_OK;
}

nnc_result nnc_aes_cbc_open(nnc_aes_cbc *self, nnc_rstream *child, u8 key[0x10], u8 iv[0x10])
{
	self->funcs = &aes_cbc_funcs;
	return init_aes_cbc(self, child, key, iv, true);
}

static result aes_cbc_write(nnc_aes_cbc *self, u8 *buf, u32 size)
{
	if(size % 0x10 != 0) return NNC_R_BAD_ALIGN;
	result ret;
	u8 block[BLOCK_SZ];
	u32 next_read, pos = 0;
	while(size != 0)
	{
		next_read = MIN(BLOCK_SZ, size);
		mbedtls_aes_crypt_cbc(self->crypto_ctx, MBEDTLS_AES_ENCRYPT, next_read, self->iv,
			&buf[pos], block);
		TRY(NNC_WS_PCALL(self->child, write, block, next_read));
		pos += next_read;
		size -= next_read;
	}
	return NNC_R_OK;
}

static result aes_cbc_wclose(nnc_aes_cbc *self)
{
	aes_cbc_close(self);
	return NNC_R_OK;
}

static u32 aes_cbc_wtell(nnc_aes_cbc *self)
{
	return self->child->funcs->tell(self->child);
}

static const nnc_wstream_funcs aes_cbc_wfuncs = {
	.write = (nnc_write_func) aes_cbc_write,
	.close = (nnc_wclose_func) aes_cbc_wclose,
	.tell  = (nnc_wtell_func) aes_cbc_wtell,
};

nnc_result nnc_aes_cbc_open_w(nnc_aes_cbc *self, nnc_wstream *child, u8 key[0x10], u8 iv[0x10])
{
	self->funcs = &aes_cbc_wfuncs;
	return init_aes_cbc(self, child, key, iv, false);
}

result nnc_decrypt_tkey(nnc_ticket *tik, nnc_keyset *ks, nnc_u8 decrypted[0x10])
{
	u128 *used_keyy;
	switch(tik->common_keyy)
	{
	case 0: used_keyy = &ks->ky_comy0; break;
	case 1: used_keyy = &ks->ky_comy1; break;
	case 2: used_keyy = &ks->ky_comy2; break;
	case 3: used_keyy = &ks->ky_comy3; break;
	case 4: used_keyy = &ks->ky_comy4; break;
	case 5: used_keyy = &ks->ky_comy5; break;
	default: return NNC_R_CORRUPT; /* invalid key selected */
	}
	u64 iv[2] = { BE64(tik->title_id), 0 };
	mbedtls_aes_context ctx;
	u8 buf[0x10];

	nnc_u128_bytes_be(used_keyy, buf);
	mbedtls_aes_setkey_dec(&ctx, buf, 128);
	mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_DECRYPT, 0x10, (u8 *) iv,
		tik->title_key, decrypted);
	mbedtls_aes_free(&ctx);
	return NNC_R_OK;
}

static nnc_seeddb nnc_empty_seeddb = {
	.size    = 0,
	.entries = NULL,
};
static nnc_seeddb *nnc_default_seeddb = &nnc_empty_seeddb;

nnc_seeddb *nnc_set_default_seeddb(nnc_seeddb *sdb)
{
	if(!sdb) sdb = &nnc_empty_seeddb;
	nnc_seeddb *ret = nnc_default_seeddb;
	nnc_default_seeddb = sdb;
	return ret;
}

nnc_seeddb *nnc_get_default_seeddb(void) { return nnc_default_seeddb; }

static nnc_keyset nnc_gkset = {
	.flags = 0,
};
static nnc_keyset *nnc_default_kset = &nnc_gkset;

nnc_keyset *nnc_set_default_keyset(nnc_keyset *kset)
{
	if(!kset) kset = &nnc_gkset;
	nnc_keyset *ret = nnc_default_kset;
	nnc_default_kset = kset;
	return ret;
}

nnc_keyset *nnc_get_default_keyset(void)
{
	/* aka not yet initialized; this check is done here because nnc_set_default_keyset() is
	 * not done for nnc_gkset */
	if(!(nnc_default_kset->flags & TYPE_FIELD))
		nnc_keyset_default(nnc_default_kset, false);
	return nnc_default_kset;
}

