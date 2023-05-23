
#include "nnc/sigcert.h"
#include "nnc/tmd.h"
#include <string.h>
#include <stdlib.h>
#include "./internal.h"

#define CINFO_UNUSED_SIZE ((NNC_CINFO_MAX_SIZE - 1) * 0x24)
#define CINFO_SIZE (NNC_CINFO_MAX_SIZE * 0x24)


result nnc_read_tmd_header(rstream* rs, nnc_tmd_header* tmd)
{
	result ret;
	TRY(NNC_RS_PCALL(rs, seek_abs, 0));
	TRY(nnc_read_sig(rs, &tmd->sig));
	u8 buf[0x84];
	TRY(read_exact(rs, buf, sizeof(buf)));
	/* 0x00 */ tmd->version = buf[0x0];
	/* 0x01 */ tmd->ca_crl_ver = buf[0x1];
	/* 0x02 */ tmd->signer_crl_ver = buf[0x2];
	/* 0x03 */ /* reserved */
	/* 0x04 */ tmd->sys_ver = BE64P(&buf[0x4]);
	/* 0x0C */ tmd->title_id = BE64P(&buf[0xC]);
	/* 0x14 */ tmd->title_type = BE32P(&buf[0x14]);
	/* 0x18 */ tmd->group_id = BE16P(&buf[0x18]);
	/* 0x1A */ tmd->save_size = LE32P(&buf[0x1A]);
	/* 0x1E */ tmd->priv_save_size = LE32P(&buf[0x1E]);
	/* 0x22 */ /* reserved */
	/* 0x26 */ tmd->srl_flag = buf[0x26];
	/* 0x27 */ /* reserved */
	/* 0x58 */ tmd->access_rights = BE32P(&buf[0x58]);
	/* 0x5C */ tmd->title_ver = BE16P(&buf[0x5C]);
	/* 0x5E */ tmd->content_count = BE16P(&buf[0x5E]);
	/* 0x60 */ tmd->boot_content = BE16P(&buf[0x60]);
	/* 0x62 */ /* padding */
	/* 0x64 */ memcpy(tmd->hash, &buf[0x64], sizeof(nnc_sha256_hash));
	return NNC_R_OK;
}

static u32 get_cinfo_pos(nnc_tmd_header* tmd)
{
	u32 ret = nnc_sig_size(tmd->sig.type);
	if (!ret) return 0;
	return ret + 0xC4;
}

static u32 get_crec_pos(nnc_tmd_header* tmd)
{
	u32 ret = nnc_sig_size(tmd->sig.type);
	if (!ret) return 0;
	return ret + 0x9C4;
}

static result parse_info_records(u8* buf, nnc_cinfo_record* records)
{
	for (u8 i = 0; i < NNC_CINFO_MAX_SIZE; ++i)
	{
		u8* blk = buf + (i * 0x24);
		/* 0x00 */ records[i].offset = BE16P(&blk[0x00]);
		/* 0x02 */ records[i].count = BE16P(&blk[0x02]);
		if (records[i].count == 0)
			break; /* end */
		/* 0x04 */ memcpy(records[i].hash, &blk[0x04], 0x20);
	}
	return NNC_R_OK;
}

result nnc_verify_read_tmd_info_records(rstream* rs, nnc_tmd_header* tmd,
	nnc_cinfo_record* records)
{
	u32 pos = get_cinfo_pos(tmd);
	if (!pos) return NNC_R_INVALID_SIG;

	result ret;
	u8 data[CINFO_SIZE];
	nnc_sha256_hash digest;
	TRY(read_at_exact(rs, pos, data, CINFO_SIZE));
	TRY(nnc_crypto_sha256(data, digest, CINFO_SIZE));
	if (memcmp(digest, tmd->hash, sizeof(digest)) != 0)
		return NNC_R_CORRUPT;
	return parse_info_records(data, records);
}

result nnc_read_tmd_info_records(rstream* rs, nnc_tmd_header* tmd,
	nnc_cinfo_record* records)
{
	u32 pos = get_cinfo_pos(tmd);
	if (!pos) return NNC_R_INVALID_SIG;

	result ret;
	u8 data[CINFO_SIZE];
	TRY(read_at_exact(rs, pos, data, CINFO_SIZE));
	return parse_info_records(data, records);
}

bool nnc_verify_tmd_info_records(rstream* rs, nnc_tmd_header* tmd)
{
	u32 pos = get_cinfo_pos(tmd);
	if (!pos) return false;

	nnc_sha256_hash digest;
	TRYB(NNC_RS_PCALL(rs, seek_abs, pos));
	TRYB(nnc_crypto_sha256_part(rs, digest, CINFO_SIZE));
	return memcmp(digest, tmd->hash, sizeof(digest)) == 0;
}

bool nnc_verify_tmd_chunk_records(rstream* rs, nnc_tmd_header* tmd, nnc_cinfo_record* records)
{
	u32 pos = get_crec_pos(tmd);
	if (!pos) return false;
	TRYB(NNC_RS_PCALL(rs, seek_abs, pos));
	u32 to_hash = tmd->content_count;
	for (u32 i = 0; records[i].count != 0 && to_hash != 0; ++i)
	{
		to_hash -= records[i].count;
		/* sizeof(chunk_record) = 0x30 */
		u32 hash_size = records[i].count * 0x30;
		nnc_sha256_hash digest;
		TRYB(nnc_crypto_sha256_part(rs, digest, hash_size));
		if (memcmp(digest, records[i].hash, sizeof(digest)) != 0)
			return false;
	}
	return to_hash == 0;
}

result nnc_read_tmd_chunk_records(rstream* rs, nnc_tmd_header* tmd, nnc_chunk_record* records)
{
	u32 pos = get_crec_pos(tmd);
	if (!pos) return NNC_R_INVALID_SIG;
	result ret;
	TRY(NNC_RS_PCALL(rs, seek_abs, pos));
	for (u16 i = 0; i < tmd->content_count; ++i)
	{
		nnc_chunk_record* rec = &records[i];
		u8 blk[0x30];
		TRY(read_exact(rs, blk, sizeof(blk)));
		/* 0x00 */ rec->id = BE32P(&blk[0x00]);
		/* 0x04 */ rec->index = BE16P(&blk[0x04]);
		/* 0x06 */ rec->flags = BE16P(&blk[0x06]);
		/* 0x08 */ rec->size = BE64P(&blk[0x08]);
		/* 0x10 */ memcpy(rec->hash, &blk[0x10], sizeof(nnc_sha256_hash));
	}
	return NNC_R_OK;
}

result nnc_tmd_signature_hash(nnc_rstream* rs, nnc_tmd_header* tmd, nnc_sha_hash digest)
{
	u32 pos = nnc_sig_size(tmd->sig.type);
	if (!pos) return NNC_R_INVALID_SIG;
	NNC_RS_PCALL(rs, seek_abs, pos);
	/* 0xC4 = sizeof(tmd_header) */
	return nnc_sighash(rs, tmd->sig.type, digest, 0xC4);
}

result nnc_write_tmd(nnc_tmd_header* tmd, nnc_chunk_record* records, u16 numrecords, nnc_wstream* ws)
{
	/* We write version 1, so that should be in the struct as well */
	if (numrecords != tmd->content_count || tmd->version != 1)
		return NNC_R_INVAL;

	result ret;
	TRY(nnc_write_sig(&tmd->sig, ws));

	/* generate the records data... */
	typedef u8 record[0x30];
	size_t reclen = sizeof(record) * numrecords;
	record* recdata = malloc(reclen);
	for (int i = 0; i < numrecords; ++i)
	{
		U32P(&recdata[i][0x00]) = BE32(records[i].id);
		U16P(&recdata[i][0x04]) = BE16(records[i].index);
		U16P(&recdata[i][0x06]) = BE16(records[i].flags);
		U64P(&recdata[i][0x08]) = BE64(records[i].size);
		memcpy(&recdata[i][0x10], records[i].hash, 0x20);
	}

	/* generate content info record */
	u8 header[0x84 + CINFO_SIZE];
	u8* cinfo = &header[0x84];
	memset(&cinfo[0x24], 0, CINFO_UNUSED_SIZE);

	U16P(&cinfo[0x00]) = 0;
	U16P(&cinfo[0x02]) = BE16(numrecords);
	/* has all records and put them in */
	nnc_crypto_sha256((u8*)recdata, &cinfo[0x04], reclen);

	/* and finally we generate the header */
	header[0x00] = tmd->version;
	header[0x01] = tmd->ca_crl_ver;
	header[0x02] = tmd->signer_crl_ver;
	header[0x03] = 0; /* reserved */
	U64P(&header[0x04]) = BE64(tmd->sys_ver);
	U64P(&header[0x0C]) = BE64(tmd->title_id);
	U32P(&header[0x14]) = BE32(tmd->title_type);
	U16P(&header[0x18]) = BE16(tmd->group_id);
	U32P(&header[0x1A]) = LE32(tmd->save_size);
	U32P(&header[0x1E]) = LE32(tmd->priv_save_size);
	memset(&header[0x22], 0, 0x4);
	header[0x26] = tmd->srl_flag;
	memset(&header[0x27], 0, 0x31);
	U32P(&header[0x58]) = BE32(tmd->access_rights);
	U16P(&header[0x5C]) = BE16(tmd->title_ver);
	U16P(&header[0x5E]) = BE16(tmd->content_count);
	U16P(&header[0x60]) = BE16(tmd->boot_content);
	U16P(&header[0x62]) = 0; /* padding */
	nnc_crypto_sha256(cinfo, &header[0x64], CINFO_SIZE);

	TRYLBL(NNC_WS_PCALL(ws, write, header, sizeof(header)), out);
	TRYLBL(NNC_WS_PCALL(ws, write, (u8*)recdata, reclen), out);

out:
	free(recdata);
	return ret;
}

nnc_u32 nnc_calculate_tmd_size(u16 content_count, enum nnc_sigtype sig_type)
{
	/* signature, issuer, header, content info records, content chunk records */
	return nnc_sig_size(sig_type) + 0x40 + 0x84 + 0x24 * 64 + 0x30 * content_count;
}
