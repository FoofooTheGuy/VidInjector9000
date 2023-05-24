
#include "nnc/ticket.h"
#include <string.h>
#include "./internal.h"


nnc_result nnc_read_ticket(nnc_rstream *rs, nnc_ticket *tik)
{
	result ret;
	TRY(NNC_RS_PCALL(rs, seek_abs, 0));
	TRY(nnc_read_sig(rs, &tik->sig));
	u8 buf[0x124];
	TRY(read_exact(rs, buf, sizeof(buf)));
	/* 0x00 */ memcpy(tik->ecc_pubkey, &buf[0x00], 0x3C);
	/* 0x3C */ tik->version = buf[0x3C];
	/* 0x3D */ tik->cacrlversion = buf[0x3D];
	/* 0x3E */ tik->signercrlversion = buf[0x3E];
	/* 0x3F */ memcpy(tik->title_key, &buf[0x3F], 0x10);
	/* 0x4F */ /* reserved */
	/* 0x50 */ tik->ticket_id = BE64P(&buf[0x50]);
	/* 0x58 */ tik->console_id = BE32P(&buf[0x58]);
	/* 0x5C */ tik->title_id = BE64P(&buf[0x5C]);
	/* 0x64 */ /* reserved */ /* sys_access */
	/* 0x66 */ tik->title_version = BE16P(&buf[0x66]);
	/* 0x68 */ /* reserved */ /* u32 time_mask + u32 permit_mask ? */
	/* 0x70 */ tik->license_type = buf[0x70];
	/* 0x71 */ tik->common_keyy = buf[0x71];
	/* 0x72 */ /* reserved */
	/* 0x9C */ tik->eshop_account_id = BE32P(&buf[0x9C]);
	/* 0xA0 */ /* reserved */
	/* 0xA1 */ tik->audit = buf[0xA1];
	/* 0xA2 */ /* reserved */ /* content_permissions + u8[3] reserved */
	/* 0xE4 */ memcpy(tik->limits, &buf[0xE4], 0x40);
	return NNC_R_OK;
}

nnc_result nnc_write_ticket(nnc_ticket *tik, nnc_wstream *ws)
{
	result ret;
	/* write signature */
	TRY(nnc_write_sig(&tik->sig, ws));
	nnc_u8 data[0x124];
	/* 0x00 */ memcpy(&data[0x00], tik->ecc_pubkey, 0x3C);
	/* 0x3C */ data[0x3C] = tik->version;
	/* 0x3D */ data[0x3D] = tik->cacrlversion;
	/* 0x3E */ data[0x3E] = tik->signercrlversion;
	/* 0x3F */ memcpy(&data[0x3F], tik->title_key, 0x10);
	/* 0x4F */ data[0x4F] = 0x00;
	/* 0x50 */ U64P(&data[0x50]) = BE64(tik->ticket_id);
	/* 0x58 */ U32P(&data[0x58]) = BE32(tik->console_id);
	/* 0x5C */ U64P(&data[0x5C]) = BE64(tik->title_id);
	/* 0x64 */ U16P(&data[0x64]) = 0x0000;
	/* 0x66 */ U16P(&data[0x66]) = BE16(tik->title_version);
	/* 0x68 */ U64P(&data[0x68]) = 0x0000000000000000;
	/* 0x70 */ data[0x70] = tik->license_type;
	/* 0x71 */ data[0x71] = tik->common_keyy;
	/* 0x72 */ memset(&data[0x72], 0x00, 0x2A);
	/* 0x9C */ U32P(&data[0x9C]) = BE32(tik->eshop_account_id);
	/* 0xA0 */ data[0xA0] = 0x00;
	/* 0xA1 */ data[0xA1] = tik->audit;
	/* 0xA2 */ memset(&data[0xA2], 0x00, 0x42);
	/* 0xE4 */ memcpy(&data[0xE4], tik->limits, 0x40);
	/* write header */
	TRY(NNC_WS_PCALL(ws, write, data, sizeof(data)));
	/* write index (TODO) */
	return NNC_R_OK;
}

result nnc_ticket_signature_hash(nnc_rstream *rs, nnc_ticket *tik, nnc_sha_hash digest)
{
	u32 pos = nnc_sig_size(tik->sig.type);
	if(!pos) return NNC_R_INVALID_SIG;
	NNC_RS_PCALL(rs, seek_abs, pos);
	/* we need to hash the entire thing except the actual signature itself */
	return nnc_sighash(rs, tik->sig.type, digest, NNC_RS_PCALL0(rs, size) - pos);
}

