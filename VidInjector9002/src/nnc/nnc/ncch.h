/** \file   ncch.h
 *  \brief  Functions relating to NCCH.
 *  \see    https://www.3dbrew.org/wiki/NCCH
 */
#ifndef inc_nnc_ncch_h
#define inc_nnc_ncch_h

#include "exheader.h"
#include "crypto.h"
#include "exefs.h"
#include "u128.h"
#include "base.h"
NNC_BEGIN

/** To be used with \ref nnc_ncch_header::crypt_method */
enum nnc_ncch_crypt_methods {
	NNC_CRYPT_INITIAL = 0x00, ///< Used from the initial system version.
	NNC_CRYPT_700     = 0x01, ///< Used from system version 7.0.0-X.
	NNC_CRYPT_930     = 0x0A, ///< Used from system version 9.3.0-X.
	NNC_CRYPT_960     = 0x0B, ///< Used from system version 9.6.0-X.
};

/** To be used with \ref nnc_ncch_header::platform */
enum nnc_ncch_platform {
	NNC_NCCH_O3DS = 0x01, ///< Old 3ds/2ds (XL/LL).
	NNC_NCCH_N3DS = 0x02, ///< New 3ds/2ds (XL/LL).
};

/** To be used with \ref nnc_ncch_header::type */
enum nnc_ncch_type {
	NNC_NCCH_DATA       = 0x01, ///< Set in CFAs.
	NNC_NCCH_EXE        = 0x02, ///< Set in CXIs.
	NNC_NCCH_SYS_UPDATE = 0x04, ///< System update.
	NNC_NCCH_MANUAL     = 0x08, ///< Manual.
	NNC_NCCH_TRIAL      = 0x10, ///< Trial.
};

/** To be used with \ref nnc_ncch_header::flags */
enum nnc_ncch_flags {
	NNC_NCCH_FIXED_KEY = 0x01, ///< Encryption uses fixed key.
	NNC_NCCH_NO_ROMFS  = 0x02, ///< NCCH doesn't have a RomFS.
	NNC_NCCH_NO_CRYPTO = 0x04, ///< NCCH isn't encrypted.
	NNC_NCCH_USES_SEED = 0x20, ///< Encryption uses seed.
};

typedef struct nnc_ncch_header {
	nnc_u128 keyy;                  ///< KeyY, note that if \ref NNC_NCCH_USES_SEED is set this is not the actual keyY in use, in that case use \ref nnc_keyy_seed.
	nnc_u32 content_size;           ///< Content size in media units, see \ref NNC_MU_TO_BYTE.
	nnc_u64 partition_id;           ///< Partition ID.
	char maker_code[3];             ///< Maker code, NULL-terminated but length is always 2.
	nnc_u16 version;                ///< NCCH format version.
	nnc_u8 seed_hash[4];            ///< First u32 of SHA256 hash of seed if used.
	nnc_u64 title_id;               ///< Also known as a "program id".
	nnc_sha256_hash logo_hash;      ///< Hash of the logo region.
	char product_code[17];          ///< Product code, NULL-terminated but length is always 16.
	nnc_sha256_hash exheader_hash;  ///< Hash of the extended header region.
	nnc_u32 exheader_size;          ///< Size of the extended header region in bytes (!!).
	nnc_u8 crypt_method;            ///< Determines the keys to use, see \ref nnc_ncch_crypt_methods.
	nnc_u8 platform;                ///< Content platform, see \ref nnc_ncch_platform.
	nnc_u8 type;                    ///< Content type field, see \ref nnc_ncch_type.
	nnc_u32 content_unit;           ///< Content unit size for this NCCH.
	nnc_u8 flags;                   ///< Other flags, see \ref nnc_ncch_flags.
	nnc_u32 plain_offset;           ///< Plain region offset in media units, see \ref NNC_MU_TO_BYTE.
	nnc_u32 plain_size;             ///< Plain region size in media units, see \ref NNC_MU_TO_BYTE.
	nnc_u32 logo_offset;            ///< Logo region offset in media units, see \ref NNC_MU_TO_BYTE.
	nnc_u32 logo_size;              ///< Logo region size in media units, see \ref NNC_MU_TO_BYTE.
	nnc_u32 exefs_offset;           ///< ExeFS region offset in media units, see \ref NNC_MU_TO_BYTE.
	nnc_u32 exefs_size;             ///< ExeFS region size in media units, see \ref NNC_MU_TO_BYTE.
	nnc_u32 exefs_hash_size;        ///< ExeFS hash region size in media units, see \ref NNC_MU_TO_BYTE.
	nnc_u32 romfs_offset;           ///< RomFS region offset in media units, see \ref NNC_MU_TO_BYTE.
	nnc_u32 romfs_size;             ///< RomFS size in media units, see \ref NNC_MU_TO_BYTE.
	nnc_u32 romfs_hash_size;        ///< RomFS hash region size in media units, see \ref NNC_MU_TO_BYTE.
	nnc_sha256_hash exefs_hash;     ///< Hash of the first \p exefs_hash_size bytes of the ExeFS region.
	nnc_sha256_hash romfs_hash;     ///< Hash of the first \p romfs_hash_size bytes of the RomFS region.
} nnc_ncch_header;

/** Contains all information that is used during the writing of an NCCH. */
typedef struct nnc_condensed_ncch_header {
	nnc_u64 partition_id;
	nnc_u64 title_id;
	nnc_u8 platform;
	nnc_u8 type;
	char product_code[17];
	char maker_code[3];
} nnc_condensed_ncch_header;

typedef void* nnc_vfs_or_stream;
typedef void* nnc_exheader_or_stream;

/** Contains all information required to build an NCCH, except the write stream. */
typedef struct nnc_buildable_ncch {
	nnc_condensed_ncch_header chdr;
	nnc_exheader_or_stream exheader;
	nnc_rstream *logo;
	nnc_rstream *plain;
	nnc_vfs_or_stream exefs;
	nnc_vfs_or_stream romfs;
	nnc_u8 wflags;
} nnc_buildable_ncch;

/** Flags for writing an ncch. */
enum nnc_ncch_wflags {
	NNC_NCCH_WF_ROMFS_VFS        = 1,   ///< Use the VFS pointer to write the RomFS.
	NNC_NCCH_WF_ROMFS_STREAM     = 2,   ///< Use the rstream pointer to copy the RomFS.
	NNC_NCCH_WF_ROMFS_OMIT       = 0,   ///< Omit the RomFS from the NCCH, pass NULL for the `romfs` parameter.
	NNC_NCCH_WF_EXEFS_VFS        = 4,   ///< Use the VFS pointer to write the ExeFS.
	NNC_NCCH_WF_EXEFS_STREAM     = 8,   ///< Use the rstream pointer to copy the ExeFS.
	NNC_NCCH_WF_EXEFS_OMIT       = 0,   ///< Omit the ExeFS from the NCCH, pass NULL for the `exefs` parameter.
	NNC_NCCH_WF_EXHEADER_BUILD   = 16,  ///< Write a new exheader based on the exheader field in the union.
	NNC_NCCH_WF_EXHEADER_STREAM  = 32,  ///< Copy the exheader from a stream.
	NNC_NCCH_WF_EXHEADER_OMIT    = 0,   ///< Omit the exheader from the NCCH, pass NULL for the `exheader` parameter.
};

/** A pseudo-stream to hold all possible required streams, yet still
 *  usable like all other streams with \ref NNC_RSP */
typedef struct nnc_ncch_section_stream {
	union nnc_ncch_section_stream_union {
		struct nnc_ncch_section_stream_enc {
			nnc_aes_ctr crypt;
			nnc_subview sv;
		} enc; ///< Used when the NCCH is encrypted.
		struct nnc_ncch_section_stream_dec {
			nnc_subview sv;
		} dec; ///< Used when the NCCH is decrypted.
	} u;
} nnc_ncch_section_stream;

typedef struct nnc_ncch_exefs_substream {
	union nnc_ncch_exefs_substream_stype {
		nnc_ncch_section_stream section;
		nnc_subview raw;
	} stream;
	nnc_u32 offset;
	nnc_u32 size;
	nnc_u32 readoff;
} nnc_ncch_exefs_substream;

typedef struct nnc_ncch_exefs_stream {
	const nnc_rstream_funcs *funcs;
	nnc_ncch_exefs_substream substreams[(1 + NNC_EXEFS_MAX_FILES) * 2];
	nnc_u32 pos, size;
	nnc_u8 filecount, streamcount, curstream;
} nnc_ncch_exefs_stream;


/** \brief       Reads the header of an NCCH.
 *  \param rs    Stream to read NCCH header from.
 *  \param ncch  Output NCCH header.
 *  \returns
 *  \p NNC_R_CORRUPT => Invalid magic.
 */
nnc_result nnc_read_ncch_header(nnc_rstream *rs, nnc_ncch_header *ncch);

/** \brief          Open a stream for the RomFS.
 *  \param ncch     NCCH to open from.
 *  \param rs       Stream associated with NCCH.
 *  \param kp       Keypair from \ref nnc_fill_keypair.
 *  \param section  Output stream.
 *  \returns
 *  Anything \ref nnc_aes_ctr_open can return.\n
 *  Anything \ref nnc_get_ncch_iv can return.\n
 *  \p NNC_R_NOT_FOUND => No RomFS is present in this NCCH.
 */
nnc_result nnc_ncch_section_romfs(nnc_ncch_header *ncch, nnc_rstream *rs,
	nnc_keypair *kp, nnc_ncch_section_stream *section);

/** \brief          Open a stream for the ExeFS header
 *  \param ncch     NCCH to open from.
 *  \param rs       Stream associated with NCCH.
 *  \param kp       Keypair from \ref nnc_fill_keypair.
 *  \param section  Output stream.
 *  \note           You have to open files with \ref nnc_ncch_exefs_subview instead
 *                  of \ref nnc_exefs_subview.
 *  \returns
 *  Anything \ref nnc_aes_ctr_open can return.\n
 *  Anything \ref nnc_get_ncch_iv can return.\n
 *  \p NNC_R_NOT_FOUND => No ExeFS is present in this NCCH.
 */
nnc_result nnc_ncch_section_exefs_header(nnc_ncch_header *ncch, nnc_rstream *rs,
	nnc_keypair *kp, nnc_ncch_section_stream *section);

/** \brief         Opens an ExeFS file for an ExeFS that belongs to an NCCH.
 *  \param ncch    NCCH to open ExeFS file of.
 *  \param rs      Stream associated with NCCH.
 *  \param kp      Keypair from \ref nnc_fill_keypair.
 *  \param file    Output file stream.
 *  \param header  File header from \ref nnc_read_exefs_header.
 *  \returns
 *  Anything \ref nnc_aes_ctr_open can return.\n
 *  Anything \ref nnc_get_ncch_iv can return.
 */
nnc_result nnc_ncch_exefs_subview(nnc_ncch_header *ncch, nnc_rstream *rs,
	nnc_keypair *kp, nnc_ncch_section_stream *file, nnc_exefs_file_header *header);

/** \brief       This stream provides transparent sequential access to the decrypted ExeFS of an NCCH.
 *  \param self  Output stream.
 *  \param ncch  NCCH to open ExeFS file of.
 *  \param rs    Stream associated with NCCH.
 *  \param kp    Keypair from \ref nnc_fill_keypair.
 */
nnc_result nnc_ncch_exefs_full_stream(nnc_ncch_exefs_stream *self, nnc_ncch_header *ncch, nnc_rstream *rs, nnc_keypair *kp);

/** \brief          Open a stream for the extended header.
 *  \param ncch     NCCH to open from.
 *  \param rs       Stream associated with NCCH.
 *  \param kp       Keypair from \ref nnc_fill_keypair.
 *  \param section  Output stream.
 *  \returns
 *  Anything \ref nnc_aes_ctr_open can return.\n
 *  Anything \ref nnc_get_ncch_iv can return.\n
 *  \p NNC_R_NOT_FOUND => No extended header is present in this NCCH.
 *  \p NNC_R_CORRUPT => Extended header is not the expected size.
 *  \warning         Hashing the full stream will not match `ncch->exheader_hash`,
 *                   please use #nnc_crypto_sha256_part with the size parameter
 *                   of `ncch->exheader_size`.
 */
nnc_result nnc_ncch_section_exheader(nnc_ncch_header *ncch, nnc_rstream *rs,
	nnc_keypair *kp, nnc_ncch_section_stream *section);

/** \brief          Opens the plain section in an NCCH used for SDK strings.
 *  \param ncch     NCCH to open plain section of.
 *  \param rs       Stream associated with the NCCH.
 *  \param section  Output section.
 *  \returns
 *  \p NNC_R_NOT_FOUND => No plain section is present in this file.
 */
nnc_result nnc_ncch_section_plain(nnc_ncch_header *ncch, nnc_rstream *rs,
	nnc_subview *section);

/** \brief          Opens the logo section in an NCCH.
 *  \param ncch     NCCH to open logo of.
 *  \param rs       Stream associated with the NCCH.
 *  \param section  Output section.
 *  \returns
 *  \p NNC_R_NOT_FOUND => No logo is present in this file.
 */
nnc_result nnc_ncch_section_logo(nnc_ncch_header *ncch, nnc_rstream *rs,
	nnc_subview *section);

/** \brief      Condense an NCCH header to contain only user-writable values.
 *  \param cnd  Output condensed header.
 *  \param hdr  Input header.
 */
void nnc_condense_ncch(nnc_condensed_ncch_header *cnd, nnc_ncch_header *hdr);

/** \brief           Write an NCCH image.
 *  \param header    The header to write, for conversion from a normal NCCH header see #nnc_condense_ncch.
 *  \param wflags    Write flags, see #nnc_ncch_wflags.
 *  \param exheader  Exheader section, for possible types see #nnc_ncch_wflags.
 *  \param logo      Logo section read stream, NULL if you want none.
 *  \param plain     Plain section read stream, NULL if you want none.
 *  \param exefs     ExeFS section, for possible types see #nnc_ncch_wflags.
 *  \param romfs     RomFS section, for possible types see #nnc_ncch_wflags.
 *  \param ws        The output write stream.
 *  \note            The write stream *must* support seeking.
 */
nnc_result nnc_write_ncch(
	nnc_condensed_ncch_header *header,
	nnc_u8 wflags,
	nnc_exheader_or_stream exheader,
	nnc_rstream *logo,
	nnc_rstream *plain,
	nnc_vfs_or_stream exefs,
	nnc_vfs_or_stream romfs,
	nnc_wstream *ws
);

/** \brief        Write an NCCH from a buildable NCCH struct.
 *  \param bncch  "Buildable NCCH" which contains all information required to build in one struct.
 *  \param ws     Output stream.
 */
static inline nnc_result nnc_write_ncch_from_buildable(nnc_buildable_ncch *bncch, nnc_wstream *ws)
{
	return nnc_write_ncch(&bncch->chdr, bncch->wflags, bncch->exheader, bncch->logo, bncch->plain, bncch->exefs, bncch->romfs, ws);
}

NNC_END
#endif

