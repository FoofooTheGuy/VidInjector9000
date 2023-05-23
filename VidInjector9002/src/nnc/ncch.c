
#include "nnc/exefs.h"
#include "nnc/romfs.h"
#include "nnc/ncch.h"
#include <string.h>
#include <stdlib.h>
#include "./internal.h"

#define EXHEADER_OFFSET NNC_MEDIA_UNIT
#define EXHEADER_FULL_SIZE (2*EXHEADER_NCCH_SIZE)
#define EXHEADER_NCCH_SIZE 0x400


result nnc_read_ncch_header(rstream *rs, nnc_ncch_header *ncch)
{
	u8 header[0x200];
	result ret;

	TRY(read_at_exact(rs, 0, header, sizeof(header)));
	/* 0x000 */ ncch->keyy = nnc_u128_import_be(&header[0x000]);
	/* 0x000 */ /* signature (and keyY) */
	/* 0x100 */ if(memcmp(&header[0x100], "NCCH", 4) != 0)
	/* 0x100 */ 	return NNC_R_CORRUPT;
	/* 0x104 */ ncch->content_size = LE32P(&header[0x104]);
	/* 0x108 */ ncch->partition_id = LE64P(&header[0x108]);
	/* 0x110 */ ncch->maker_code[0] = header[0x110];
	/* 0x111 */ ncch->maker_code[1] = header[0x111];
	/* 0x111 */ ncch->maker_code[2] = '\0';
	/* 0x112 */ ncch->version = LE16P(&header[0x112]);
	/* 0x114 */ memcpy(ncch->seed_hash, &header[0x114], 4);
	/* 0x118 */ ncch->title_id = LE64P(&header[0x118]);
	/* 0x120 */ /* reserved */
	/* 0x130 */ memcpy(ncch->logo_hash, &header[0x130], sizeof(nnc_sha256_hash));
	/* 0x150 */ memcpy(ncch->product_code, &header[0x150], 0x10);
	/* 0x150 */ ncch->product_code[0x10] = '\0';
	/* 0x160 */ memcpy(ncch->exheader_hash, &header[0x160], sizeof(nnc_sha256_hash));
	/* 0x180 */ ncch->exheader_size = LE32P(&header[0x180]);
	/* 0x184 */ /* reserved */
	/* 0x188 */ /* ncchflags[0] */
	/* 0x189 */ /* ncchflags[1] */
	/* 0x18A */ /* ncchflags[2] */
	/* 0x18B */ ncch->crypt_method = header[0x18B];
	/* 0x18C */ ncch->platform = header[0x18C];
	/* 0x18D */ ncch->type = header[0x18D];
	/* 0x18E */ ncch->content_unit = NNC_MEDIA_UNIT * nnc_pow2(header[0x18E]);
	/* 0x18F */ ncch->flags = header[0x18F];
	/* 0x190 */ ncch->plain_offset = LE32P(&header[0x190]);
	/* 0x194 */ ncch->plain_size = LE32P(&header[0x194]);
	/* 0x198 */ ncch->logo_offset = LE32P(&header[0x198]);
	/* 0x19C */ ncch->logo_size = LE32P(&header[0x19C]);
	/* 0x1A0 */ ncch->exefs_offset = LE32P(&header[0x1A0]);
	/* 0x1A4 */ ncch->exefs_size = LE32P(&header[0x1A4]);
	/* 0x1A8 */ ncch->exefs_hash_size = LE32P(&header[0x1A8]);
	/* 0x1AC */ /* reserved */
	/* 0x1B0 */ ncch->romfs_offset = LE32P(&header[0x1B0]);
	/* 0x1B4 */ ncch->romfs_size = LE32P(&header[0x1B4]);
	/* 0x1B8 */ ncch->romfs_hash_size = LE32P(&header[0x1B8]);
	/* 0x1BC */ /* reserved */
	/* 0x1C0 */ memcpy(ncch->exefs_hash, &header[0x1C0], sizeof(nnc_sha256_hash));
	/* 0x1E0 */ memcpy(ncch->romfs_hash, &header[0x1E0], sizeof(nnc_sha256_hash));
	return NNC_R_OK;
}

#define SUBVIEW_R(mode, offset, size) \
	nnc_subview_open(&section->u. mode .sv, rs, offset, size)
#define SUBVIEW(mode, offset, size) \
	SUBVIEW_R(mode, NNC_MU_TO_BYTE(offset), NNC_MU_TO_BYTE(size))

result nnc_ncch_section_romfs(nnc_ncch_header *ncch, nnc_rstream *rs,
	nnc_keypair *kp, nnc_ncch_section_stream *section)
{
	if(ncch->flags & NNC_NCCH_NO_ROMFS || ncch->romfs_size == 0)
		return NNC_R_NOT_FOUND;
	if(ncch->flags & NNC_NCCH_NO_CRYPTO)
		return SUBVIEW(dec, ncch->romfs_offset, ncch->romfs_size), NNC_R_OK;

	u8 iv[0x10]; result ret;
	TRY(nnc_get_ncch_iv(ncch, NNC_SECTION_ROMFS, iv));
	SUBVIEW(enc, ncch->romfs_offset, ncch->romfs_size);
	return nnc_aes_ctr_open(&section->u.enc.crypt, NNC_RSP(&section->u.enc.sv),
		&kp->secondary, iv);
}

result nnc_ncch_section_exefs_header(nnc_ncch_header *ncch, nnc_rstream *rs,
	nnc_keypair *kp, nnc_ncch_section_stream *section)
{
	if(ncch->exefs_size == 0) return NNC_R_NOT_FOUND;
	/* ExeFS header is only 1 NNC_MEDIA_UNIT large */
	if(ncch->flags & NNC_NCCH_NO_CRYPTO)
		return SUBVIEW(dec, ncch->exefs_offset, 1), NNC_R_OK;

	u8 iv[0x10]; result ret;
	TRY(nnc_get_ncch_iv(ncch, NNC_SECTION_EXEFS, iv));
	SUBVIEW(enc, ncch->exefs_offset, 1);
	return nnc_aes_ctr_open(&section->u.enc.crypt, NNC_RSP(&section->u.enc.sv),
		&kp->primary, iv);
}

nnc_result nnc_ncch_section_exheader(nnc_ncch_header *ncch, nnc_rstream *rs,
	nnc_keypair *kp, nnc_ncch_section_stream *section)
{
	if(ncch->exheader_size == 0) return NNC_R_NOT_FOUND;
	/* for some reason the header says it's 0x400 bytes whilest it really is 0x800 bytes */
	if(ncch->exheader_size != EXHEADER_NCCH_SIZE) return NNC_R_CORRUPT;
	if(ncch->flags & NNC_NCCH_NO_CRYPTO)
		return SUBVIEW_R(dec, EXHEADER_OFFSET, EXHEADER_FULL_SIZE), NNC_R_OK;

	u8 iv[0x10]; result ret;
	TRY(nnc_get_ncch_iv(ncch, NNC_SECTION_EXHEADER, iv));
	SUBVIEW_R(enc, EXHEADER_OFFSET, EXHEADER_FULL_SIZE);
	return nnc_aes_ctr_open(&section->u.enc.crypt, NNC_RSP(&section->u.enc.sv),
		&kp->primary, iv);
}

static result efs_strm_read(nnc_ncch_exefs_stream *self, u8 *buf, u32 max, u32 *totalRead)
{
	u32 willread = MIN(max, self->size - self->pos);
	*totalRead = willread;
	nnc_result ret;
	u32 bufoff = 0;

	while(willread)
	{
		nnc_ncch_exefs_substream *sstream = &self->substreams[self->curstream];
		/* internal error */
		if(sstream->readoff == sstream->size) return NNC_R_INTERNAL;
		u32 left_in_ss = sstream->size - sstream->readoff;
		u32 this_read = MIN(willread, left_in_ss);
		TRY(read_exact(NNC_RSP(&sstream->stream), buf + bufoff, this_read));
		bufoff += this_read;
		willread -= this_read;
		sstream->readoff += this_read;
		self->pos += this_read;
		/* we need to move over to the next substream */
		if(sstream->readoff == sstream->size)
		{
			++self->curstream;
			if(self->curstream != self->streamcount)
			{
				sstream = &self->substreams[self->curstream];
				sstream->readoff = 0;
				TRY(NNC_RS_CALL(sstream->stream, seek_abs, 0));
			}
		}
	}
	return NNC_R_OK;
}

static result efs_strm_seek_abs(nnc_ncch_exefs_stream *self, u32 pos)
{
	if(pos > self->size) return NNC_R_SEEK_RANGE;
	/* find the correct bucket */
	for(u8 i = 0; i < self->streamcount; ++i)
		if(self->substreams[i].offset >= pos)
		{
			/* found it; seek to this bucket */
			u32 amount_in = self->substreams[i].offset - pos;
			result res = NNC_RS_CALL(self->substreams[i].stream, seek_abs, amount_in);
			if(res != NNC_R_OK) return res;
			self->substreams[i].readoff = amount_in;
			self->pos = pos;
			self->curstream = i;
			return NNC_R_OK;
		}
	/* shouldn't happen */
	return NNC_R_SEEK_RANGE;
}

static result efs_strm_seek_rel(nnc_ncch_exefs_stream *self, u32 pos)
{
	return efs_strm_seek_abs(self, self->pos + pos);
}

static u32 efs_strm_size(nnc_ncch_exefs_stream *self)
{
	return self->size;
}

static void efs_strm_close(nnc_ncch_exefs_stream *self)
{
	for(u8 i = 0; i < self->streamcount; ++i)
		NNC_RS_CALL0(self->substreams[i].stream, close);
}

static u32 efs_strm_tell(nnc_ncch_exefs_stream *self) { return self->pos; }

static const nnc_rstream_funcs efs_strm_funcs = {
	.read = (nnc_read_func) efs_strm_read,
	.seek_abs = (nnc_seek_abs_func) efs_strm_seek_abs,
	.seek_rel = (nnc_seek_rel_func) efs_strm_seek_rel,
	.size = (nnc_size_func) efs_strm_size,
	.close = (nnc_close_func) efs_strm_close,
	.tell = (nnc_tell_func) efs_strm_tell,
};

nnc_result nnc_ncch_exefs_full_stream(nnc_ncch_exefs_stream *self, nnc_ncch_header *ncch, nnc_rstream *rs, nnc_keypair *kp)
{
	nnc_ncch_section_stream *header_stream = &self->substreams[0].stream.section;
	nnc_exefs_file_header headers[NNC_EXEFS_MAX_FILES];
	u8 filecount;
	result ret;

	self->funcs = &efs_strm_funcs;
	self->streamcount = 0;

	TRY(nnc_ncch_section_exefs_header(ncch, rs, kp, header_stream)); self->streamcount = 1;
	TRYLBL(nnc_read_exefs_header(NNC_RSP(header_stream), headers, &filecount), out);
	NNC_RS_PCALL(header_stream, seek_abs, 0);

	self->substreams[0].offset = 0;
	self->substreams[0].size = NNC_EXEFS_HEADER_SIZE;
	u32 mpos = NNC_EXEFS_HEADER_SIZE;

	u8 sindex = 1;
	u32 tpos, nsiz;
	for(u8 i = 0; i < filecount; ++i)
	{
		tpos = NNC_EXEFS_HEADER_SIZE + headers[i].offset;
		if(mpos < tpos)
		{
			/* we need to add an "in-between" section */
			nsiz = tpos - mpos;
			self->substreams[sindex].offset = mpos;
			self->substreams[sindex].size = nsiz;
			self->substreams[sindex].readoff = 0;
			/* open a substream in the raw exefs */
			nnc_subview_open(&self->substreams[sindex].stream.raw, rs, NNC_MU_TO_BYTE(ncch->exefs_offset) + mpos, nsiz);
			mpos += nsiz;
			++sindex;
			++self->streamcount;
		}
		else if(mpos > tpos)
		{
			/* in this case something is wrong */
			ret = NNC_R_CORRUPT;
			break;
		}
		self->substreams[sindex].offset = mpos;
		self->substreams[sindex].size = headers[i].size;
		self->substreams[sindex].readoff = 0;
		TRYLBL(nnc_ncch_exefs_subview(ncch, rs, kp, &self->substreams[sindex].stream.section, &headers[i]), out);
		++self->streamcount;
		++sindex;
		mpos += headers[i].size;
	}
	/* Add the last section */
	tpos = NNC_MU_TO_BYTE(ncch->exefs_size);
	if(mpos < tpos)
	{
		nsiz = tpos - mpos;
		self->substreams[sindex].offset = mpos;
		self->substreams[sindex].size = nsiz;
		self->substreams[sindex].readoff = 0;
		nnc_subview_open(&self->substreams[sindex].stream.raw, rs, NNC_MU_TO_BYTE(ncch->exefs_offset) + mpos, nsiz);
		++self->streamcount;
		mpos += nsiz;
	}
	else if(mpos > tpos)
		ret = NNC_R_CORRUPT;

	self->size = mpos;
	self->pos = 0;
	self->curstream = 0;

out:
	if(ret != NNC_R_OK)
		efs_strm_close(self);
	return ret;
}

nnc_result nnc_ncch_exefs_subview(nnc_ncch_header *ncch, nnc_rstream *rs,
	nnc_keypair *kp, nnc_ncch_section_stream *section, nnc_exefs_file_header *header)
{
	if(ncch->exefs_size == 0) return NNC_R_NOT_FOUND;
	if(ncch->flags & NNC_NCCH_NO_CRYPTO)
		return SUBVIEW_R(dec,
			NNC_MU_TO_BYTE(ncch->exefs_offset) + NNC_EXEFS_HEADER_SIZE + header->offset,
			header->size), NNC_R_OK;

	nnc_u128 *key; u8 iv[0x10]; result ret;
	TRY(nnc_get_ncch_iv(ncch, NNC_SECTION_EXEFS, iv));
	nnc_u128 ctr = nnc_u128_import_be(iv);
	/* we need to advance the IV to the correct section, split the header so the
	 * compiler hopefully has an easier job optimizing that */
	nnc_u128 addition = NNC_PROMOTE128((header->offset / 0x10) + (NNC_EXEFS_HEADER_SIZE / 0x10));
	nnc_u128_add(&ctr, &addition);
	nnc_u128_bytes_be(&ctr, iv);

	/* these belong to the "info menu" group */
	if(strcmp(header->name, "icon") == 0 || strcmp(header->name, "banner") == 0)
		key = &kp->primary;
	/* the rest belongs to the "content" group */
	else
		key = &kp->secondary;

	SUBVIEW_R(enc,
		NNC_MU_TO_BYTE(ncch->exefs_offset) + NNC_EXEFS_HEADER_SIZE + header->offset,
		header->size);
	return nnc_aes_ctr_open(&section->u.enc.crypt, NNC_RSP(&section->u.enc.sv), key, iv);
}

nnc_result nnc_ncch_section_plain(nnc_ncch_header *ncch, nnc_rstream *rs,
	nnc_subview *section)
{
	if(ncch->plain_size == 0) return NNC_R_NOT_FOUND;
	nnc_subview_open(section, rs, NNC_MU_TO_BYTE(ncch->plain_offset),
		NNC_MU_TO_BYTE(ncch->plain_size));
	return 0;
}

nnc_result nnc_ncch_section_logo(nnc_ncch_header *ncch, nnc_rstream *rs,
	nnc_subview *section)
{
	if(ncch->logo_size == 0) return NNC_R_NOT_FOUND;
	nnc_subview_open(section, rs, NNC_MU_TO_BYTE(ncch->logo_offset),
		NNC_MU_TO_BYTE(ncch->logo_size));
	return 0;
}

void nnc_condense_ncch(nnc_condensed_ncch_header *cnd, nnc_ncch_header *hdr)
{
	cnd->partition_id = hdr->partition_id;
	cnd->title_id = hdr->title_id;
	cnd->platform = hdr->platform;
	cnd->type = hdr->type;
	strncpy(cnd->product_code, hdr->product_code, sizeof(cnd->product_code));
	strncpy(cnd->maker_code, hdr->maker_code, sizeof(hdr->maker_code));
}

nnc_result nnc_write_ncch(
	nnc_condensed_ncch_header *ncch_header,
	nnc_u8 wflags,
	nnc_exheader_or_stream exheader,
	nnc_rstream *logo,
	nnc_rstream *plain,
	nnc_vfs_or_stream exefs,
	nnc_vfs_or_stream romfs,
	nnc_wstream *ws)
{
	result ret;
	u32 header_off, end_off, logo_off = 0, plain_off = 0, exefs_off = 0, romfs_off = 0, logo_size = 0, plain_size = 0, exefs_size = 0, romfs_size = 0;
	nnc_sha256_hash exheader_hash, logo_hash, exefs_super_hash, romfs_super_hash;
	nnc_hasher_writer hwrite;
	nnc_header_saver hsaver;
	u8 header[0x200], exheader_in_use = 0;

	if(!ws->funcs->seek)
		return NNC_R_INVAL;

#define DO_VALIDATE_FOR(ptr, opt1, opt2) if( (!ptr && (wflags & (opt1 | opt2))) || (ptr && !(wflags & (opt1 | opt2))) || (wflags & (opt1 | opt2)) == (opt1 | opt2)) return NNC_R_INVAL
	DO_VALIDATE_FOR(exheader, NNC_NCCH_WF_EXHEADER_BUILD, NNC_NCCH_WF_EXHEADER_STREAM);
	DO_VALIDATE_FOR(romfs, NNC_NCCH_WF_ROMFS_VFS, NNC_NCCH_WF_ROMFS_STREAM);
	DO_VALIDATE_FOR(exefs, NNC_NCCH_WF_EXEFS_VFS, NNC_NCCH_WF_EXEFS_STREAM);
#undef DO_VALIDATE_FOR

	memset(&exheader_hash, 0x00, sizeof(exheader_hash));
	memset(&logo_hash, 0x00, sizeof(logo_hash));
	memset(&exefs_super_hash, 0x00, sizeof(exefs_super_hash));
	memset(&romfs_super_hash, 0x00, sizeof(romfs_super_hash));

	/* we'll reserve space for the header as we'll write it last */
	header_off = NNC_WS_PCALL0(ws, tell);
	/* the exheader resides directly after the header */
	TRY(nnc_write_padding(ws, EXHEADER_OFFSET));

	if(exheader)
	{
		if(wflags & NNC_NCCH_WF_EXHEADER_BUILD)
			return NNC_R_UNSUPPORTED; /* unsupported for now */
		else
		{
			if(NNC_RS_PCALL0((nnc_rstream *) exheader, size) != EXHEADER_FULL_SIZE)
				return NNC_R_INVAL;
			TRY(nnc_open_hasher_writer(&hwrite, ws, EXHEADER_NCCH_SIZE));
			ret = nnc_copy((nnc_rstream *) exheader, NNC_WSP(&hwrite), NULL);
			nnc_hasher_writer_digest(&hwrite, exheader_hash);
			if(ret != NNC_R_OK) return ret;
		}
		exheader_in_use = 1;
	}

	if(logo)
	{
		logo_off = NNC_WS_PCALL0(ws, tell);
		TRY(nnc_open_hasher_writer(&hwrite, ws, 0));
		ret = nnc_copy(logo, NNC_WSP(&hwrite), &logo_size);
		if(ret == NNC_R_OK)
			ret = nnc_write_padding(NNC_WSP(&hwrite), ALIGN(logo_size, NNC_MEDIA_UNIT) - logo_size);
		nnc_hasher_writer_digest(&hwrite, logo_hash);
		if(ret != NNC_R_OK) return ret;
		if(logo_size == 0) logo_off = 0;
	}

	if(plain)
	{
		plain_off = NNC_WS_PCALL0(ws, tell);
		TRY(nnc_copy(plain, ws, &plain_size));
		TRY(nnc_write_padding(ws, ALIGN(plain_size, NNC_MEDIA_UNIT) - plain_size));
		if(plain_size == 0) plain_off = 0;
	}

	if(exefs)
	{
		exefs_off = NNC_WS_PCALL0(ws, tell);
		if(wflags & NNC_NCCH_WF_EXEFS_VFS)
		{
			TRY(nnc_open_header_saver(&hsaver, ws, NNC_MEDIA_UNIT));
			ret = nnc_write_exefs((nnc_vfs *) exefs, NNC_WSP(&hsaver));
			exefs_size = NNC_WS_PCALL0(ws, tell) - exefs_off;
			if(exefs_size >= NNC_MEDIA_UNIT)
				nnc_crypto_sha256_buffer(hsaver.buffer, NNC_MEDIA_UNIT, exefs_super_hash);
			NNC_WS_CALL0(hsaver, close);
			if(ret == NNC_R_OK && exefs_size < NNC_MEDIA_UNIT)
				ret = NNC_R_INVAL; /* shouldn't happen afaik */
			if(ret != NNC_R_OK) return ret;
		}
		else
		{
			if((exefs_size = NNC_RS_PCALL0((nnc_rstream *) exefs, size)) < NNC_MEDIA_UNIT)
				return NNC_R_INVAL; /* a valid ExeFS has at least NNC_MEDIA_UNIT bytes */
			TRY(nnc_open_hasher_writer(&hwrite, ws, NNC_MEDIA_UNIT));
			ret = nnc_copy((nnc_rstream *) exefs, NNC_WSP(&hwrite), NULL);
			nnc_hasher_writer_digest(&hwrite, exefs_super_hash);
			if(ret != NNC_R_OK) return ret;
		}
		TRY(nnc_write_padding(ws, ALIGN(exefs_size, NNC_MEDIA_UNIT) - exefs_size));
	}

	if(romfs)
	{
		romfs_off = NNC_WS_PCALL0(ws, tell);
		if(wflags & NNC_NCCH_WF_ROMFS_VFS)
		{
			TRY(nnc_open_header_saver(&hsaver, ws, NNC_MEDIA_UNIT));
			ret = nnc_write_romfs((nnc_vfs *) romfs, NNC_WSP(&hsaver));
			romfs_size = NNC_WS_PCALL0(ws, tell) - romfs_off;
			if(romfs_size >= NNC_MEDIA_UNIT)
				nnc_crypto_sha256_buffer(hsaver.buffer, NNC_MEDIA_UNIT, romfs_super_hash);
			NNC_WS_CALL0(hsaver, close);
			if(ret == NNC_R_OK && romfs_size < NNC_MEDIA_UNIT)
				ret = NNC_R_INVAL; /* shouldn't happen afaik */
			if(ret != NNC_R_OK) return ret;
		}
		else
		{
			if((romfs_size = NNC_RS_PCALL0((nnc_rstream *) romfs, size)) < NNC_MEDIA_UNIT)
				return NNC_R_INVAL; /* a valid RomFS has at least NNC_MEDIA_UNIT bytes */
			TRY(nnc_open_hasher_writer(&hwrite, ws, NNC_MEDIA_UNIT));
			ret = nnc_copy((nnc_rstream *) romfs, NNC_WSP(&hwrite), NULL);
			nnc_hasher_writer_digest(&hwrite, romfs_super_hash);
			if(ret != NNC_R_OK) return ret;
		}
		TRY(nnc_write_padding(ws, ALIGN(romfs_size, NNC_MEDIA_UNIT) - romfs_size));
		if(romfs_size == 0) romfs_off = 0;
	}

	/* now comes the header */
	end_off = NNC_WS_PCALL0(ws, tell);
	TRY(NNC_WS_PCALL(ws, seek, header_off));

	/* convert everything to media units... */
	/* not before normalizing offsets to be inside the ncch of course */
	logo_off   = NNC_BYTE_TO_MU(logo_off - header_off);
	plain_off  = NNC_BYTE_TO_MU(plain_off - header_off);
	exefs_off  = NNC_BYTE_TO_MU(exefs_off - header_off);
	romfs_off  = NNC_BYTE_TO_MU(romfs_off - header_off);
	logo_size  = NNC_BYTE_TO_MU(logo_size);
	plain_size = NNC_BYTE_TO_MU(plain_size);
	exefs_size = NNC_BYTE_TO_MU(exefs_size);
	romfs_size = NNC_BYTE_TO_MU(romfs_size);

	char product_code[0x10 + 1];
	memset(product_code, 0x00, sizeof(product_code));
	strncpy(product_code, ncch_header->product_code, sizeof(product_code));

	/* 0x000 */ memset(&header[0x000], 0x00, 0x100);
	/* 0x100 */ memcpy(&header[0x100], "NCCH", 4);
	/* 0x104 */ U32P(&header[0x104]) = LE32(romfs_off + romfs_size); /* content size */
	/* 0x108 */ U64P(&header[0x108]) = LE64(ncch_header->partition_id);
	/* 0x110 */ memcpy(&header[0x110], ncch_header->maker_code, 2);
	/* 0x112 */ U16P(&header[0x112]) = LE16(2);
	/* 0x114 */ memset(&header[0x114], 0x00, 4); /* seed hash, crypto not supported yet */
	/* 0x118 */ U64P(&header[0x118]) = LE64(ncch_header->title_id);
	/* 0x120 */ memset(&header[0x120], 0x00, 0x10); /* reserved */
	/* 0x130 */ memcpy(&header[0x130], logo_hash, 0x20); /* logo region hash */
	/* 0x150 */ memcpy(&header[0x150], product_code, 0x10);
	/* 0x160 */ memcpy(&header[0x160], exheader_hash, 0x20); /* exheader region hash (initial 0x400) */
	/* 0x180 */ U32P(&header[0x180]) = exheader_in_use ? LE32(EXHEADER_NCCH_SIZE) : 0; /* "exheader size," but better named "exheader hash size" */
	/* 0x184 */ U32P(&header[0x184]) = 0; /* reserved */
	/* 0x188 */ header[0x188] = 0; /* ncchflags[0] */
	/* 0x189 */ header[0x189] = 0; /* ncchflags[1] */
	/* 0x18A */ header[0x18A] = 0; /* ncchflags[2] */
	/* 0x18B */ header[0x18B] = NNC_CRYPT_INITIAL; /* crypto method, unsupported for now */
	/* 0x18C */ header[0x18C] = ncch_header->platform;
	/* 0x18D */ header[0x18D] = ncch_header->type;
	/* 0x18E */ header[0x18E] = 0; /* content unit size; 0x200*2^0=0x200 (=NNC_MEDIA_UNIT) */
	/* 0x18F */ header[0x18F] = NNC_NCCH_NO_CRYPTO; /* flags */
	/* 0x18F */ if(!romfs_size) header[0x18F] |= NNC_NCCH_NO_ROMFS;
	/* 0x190 */ U32P(&header[0x190]) = LE32(plain_off);
	/* 0x194 */ U32P(&header[0x194]) = LE32(plain_size);
	/* 0x198 */ U32P(&header[0x198]) = LE32(logo_off);
	/* 0x19C */ U32P(&header[0x19C]) = LE32(logo_size);
	/* 0x1A0 */ U32P(&header[0x1A0]) = LE32(exefs_off);
	/* 0x1A4 */ U32P(&header[0x1A4]) = LE32(exefs_size);
	/* 0x1A8 */ U32P(&header[0x1A8]) = LE32(1); /* ExeFS hash region size (1 MU (0x200) suffices for the hashed header) */
	/* 0x1AC */ U32P(&header[0x1AC]) = 0; /* reserved */
	/* 0x1B0 */ U32P(&header[0x1B0]) = LE32(romfs_off);
	/* 0x1B4 */ U32P(&header[0x1B4]) = LE32(romfs_size);
	/* 0x1B8 */ U32P(&header[0x1B8]) = LE32(1); /* RomFS hash region size (1 MU (0x200) suffices for the IVFC master hash) */
	/* 0x1BC */ U32P(&header[0x1BC]) = 0; /* reserved */
	/* 0x1C0 */ memcpy(&header[0x1C0], exefs_super_hash, 0x20); /* ExeFS superblock hash */
	/* 0x1E0 */ memcpy(&header[0x1E0], romfs_super_hash, 0x20); /* RomFS superblock hash */

	TRY(NNC_WS_PCALL(ws, write, header, sizeof(header)));
	TRY(NNC_WS_PCALL(ws, seek, end_off));

	return NNC_R_OK;
}

