/** \file  tmd.h
 *  \brief Functions relating to TMD (Title metadata).
 *  \see   https://www.3dbrew.org/wiki/Title_metadata
 */
#ifndef inc_nnc_tmd_h
#define inc_nnc_tmd_h

#include "sigcert.h"
#include "stream.h"
#include "crypto.h"
#include "base.h"
NNC_BEGIN

/** Returns if the info record is the last record. */
#define NNC_CINFO_IS_LAST(record) ((record).count == 0)
/** Maximum info records in a TMD file. */
#define NNC_CINFO_MAX_SIZE 64

enum nnc_chunk_flags {
	NNC_CHUNKF_ENCRYPTED  = 0x1,    ///< Content is encrypted.
	NNC_CHUNKF_DISC       = 0x2,    ///< Disc.
	NNC_CHUNKF_CFM        = 0x4,    ///< CFM.
	NNC_CHUNKF_OPTIONAL   = 0x4000, ///< Optional.
	NNC_CHUNKF_SHARED     = 0x8000, ///< Shared.
};

typedef struct nnc_tmd_header {
	nnc_signature sig;      ///< Signature.
	nnc_u8 version;         ///< TMD Version.
	nnc_u8 ca_crl_ver;      ///< CA CRL version.
	nnc_u8 signer_crl_ver;  ///< Signer CRL version.
	nnc_u64 sys_ver;        ///< System version.
	nnc_u64 title_id;       ///< Title ID (\ref tid).
	nnc_u32 title_type;     ///< Title type.
	nnc_u32 save_size;      ///< Save data size.
	nnc_u32 priv_save_size; ///< SRL private save data size.
	nnc_u8 srl_flag;        ///< SRL flag.
	nnc_u32 access_rights;  ///< Access rights.
	nnc_u16 title_ver;      ///< Title version. See: \ref nnc_parse_version
	nnc_u16 group_id;       ///< Group ID.
	nnc_u16 content_count;  ///< Content count.
	nnc_u16 boot_content;   ///< Boot content.
	nnc_sha256_hash hash;   ///< Hash of the content info records.
} nnc_tmd_header;

/** Struct to hold hashes for nnc_chunk_record. */
typedef struct nnc_cinfo_record {
	nnc_u16 offset;        ///< Index from where to hash.
	nnc_u16 count;         ///< Amount of records hash is for.
	nnc_sha256_hash hash;  ///< Hash for next index_count records.
} nnc_cinfo_record;

/** Struct to hold information about contents in a title. */
typedef struct nnc_chunk_record {
	nnc_u32 id;           ///< Content ID.
	nnc_u16 index;        ///< Content index.
	nnc_u16 flags;        ///< Flags set, see \ref nnc_chunk_flags.
	nnc_u64 size;         ///< Size of the content.
	nnc_sha256_hash hash; ///< Hash of the content.
} nnc_chunk_record;


/** \brief      Read a TMD header from a stream.
 *  \param rs   Stream to read TMD from.
 *  \param tmd  Output TMD header.
 */
nnc_result nnc_read_tmd_header(nnc_rstream *rs, nnc_tmd_header *tmd);

/** \brief      Verify correctness of the content info records.
 *  \param rs   Stream to read TMD from.
 *  \param tmd  TMD returned from \ref nnc_read_tmd_header.
 */
bool nnc_verify_tmd_info_records(nnc_rstream *rs, nnc_tmd_header *tmd);

/** \brief          Verify correctness of the content info records and read if good.
 *  \param rs       Stream to read TMD from.
 *  \param tmd      TMD returned from \ref nnc_read_tmd_header.
 *  \param records  Array of records of size \ref NNC_CINFO_MAX_SIZE.
 */
nnc_result nnc_verify_read_tmd_info_records(nnc_rstream *rs, nnc_tmd_header *tmd,
	nnc_cinfo_record *records);

/** \brief          Read content info records from TMD.
 *  \param rs       Stream to read TMD from.
 *  \param tmd      TMD returned from \ref nnc_read_tmd_header.
 *  \param records  Array of records of size \ref NNC_CINFO_MAX_SIZE.
 */
nnc_result nnc_read_tmd_info_records(nnc_rstream *rs, nnc_tmd_header *tmd,
	nnc_cinfo_record *records);

/** \brief          Verifies if content chunk records are correct.
 *  \param rs       Stream to read TMD from.
 *  \param tmd      TMD header gotten from \ref nnc_read_tmd_header.
 *  \param records  Array of records gotten from one of the read functions.
 */
bool nnc_verify_tmd_chunk_records(nnc_rstream *rs, nnc_tmd_header *tmd, nnc_cinfo_record *records);

/** \brief          Reads chunks records from TMD.
 *  \param rs       Stream to read TMD from.
 *  \param tmd      TMD header gotten from \ref nnc_read_tmd_header.
 *  \param records  Array of records with a size of at least tmd->content_count. As a reference,
 *                  Godmode9 statically allocates 383 or 1024 for large TMDs records.
 */
nnc_result nnc_read_tmd_chunk_records(nnc_rstream *rs, nnc_tmd_header *tmd, nnc_chunk_record *records);

/** \brief         Hashes part of the TMD required for \ref nnc_verify_signature.
 *  \param rs      Stream to hash from.
 *  \param tmd     TMD header.
 *  \param digest  Output hash.
 *  \returns
 *  Anything \ref nnc_sighash can return.\n
 *  \p NNC_R_INVALID_SIG => Invalid signature.
 *  \warning       Just verifying this hash doesn't mean the TMD is free from tampering.
 *                 You must verify the info records & chunks records.
 */
nnc_result nnc_tmd_signature_hash(nnc_rstream *rs, nnc_tmd_header *tmd, nnc_sha_hash digest);

/** \brief             Write a TMD.
 *  \param tmd         TMD header to write.
 *  \param records     Records to write.
 *  \param numrecords  Amount of records.
 *  \param ws          Output write stream.
 */
nnc_result nnc_write_tmd(nnc_tmd_header *tmd, nnc_chunk_record *records, nnc_u16 numrecords, nnc_wstream *ws);

/** \brief                Calculates the size of a TMD, without building it.
 *  \param content_count  The amount of contents this TMD will have.
 *  \param sig_type       The signature type this TMD will have.
 */
nnc_u32 nnc_calculate_tmd_size(nnc_u16 content_count, enum nnc_sigtype sig_type);

NNC_END
#endif

