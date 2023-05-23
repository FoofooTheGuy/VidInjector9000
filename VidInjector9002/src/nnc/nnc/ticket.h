/** \file   ticket.h
 *  \brief  Functions relating to tickets.
 *  \see    https://www.3dbrew.org/wiki/Ticket
 */
#ifndef inc_nnc_ticket_h
#define inc_nnc_ticket_h

#include "sigcert.h"
#include "stream.h"
#include "crypto.h"
#include "base.h"
NNC_BEGIN

enum nnc_ticket_license {
	NNC_TICKET_LIC_PERMANENT    = 0, ///< Permanent license, this is the usual type.
	NNC_TICKET_LIC_DEMO         = 1, ///< Demo license.
	NNC_TICKET_LIC_TRIAL        = 2, ///< Trial license.
	NNC_TICKET_LIC_RENTAL       = 3, ///< Rental license.
	NNC_TICKET_LIC_SUBSCRIPTION = 4, ///< Subscription license.
	NNC_TICKET_LIC_SERVICE      = 5, ///< Service license.
};

typedef struct nnc_ticket {
	nnc_signature sig;        ///< Signature.
	nnc_u8 ecc_pubkey[0x3C];  ///< ECC public key.
	nnc_u8 version;           ///< Ticket format version.
	nnc_u8 cacrlversion;      ///< CA CRL version.
	nnc_u8 signercrlversion;  ///< Signer CRL version.
	nnc_u8 title_key[0x10];   ///< Title key, encrypted, see \ref nnc_decrypt_tkey.
	nnc_u64 ticket_id;        ///< Ticket ID.
	nnc_u32 console_id;       ///< Console ID.
	nnc_u64 title_id;         ///< Title ID, also known as a program ID.
	nnc_u16 title_version;    ///< Title version, see \ref nnc_parse_version.
	nnc_u8 license_type;      ///< License type, see \ref nnc_ticket_license.
	nnc_u8 common_keyy;       ///< Common keyY index used for title key encryption.
	nnc_u32 eshop_account_id; ///< eShop account ID, filled with 0's in many cases.
	nnc_u8 audit;             ///< Audit.
	nnc_u8 limits[0x40];      ///< Limits.
} nnc_ticket;

/** \brief      Reads a ticket.
 *  \param rs   Stream to read from.
 *  \param tik  Output ticket.
 *  \returns
 *  Anything \ref nnc_read_sig can return. \n
 *  Anything rstream read can return.
 */
nnc_result nnc_read_ticket(nnc_rstream *rs, nnc_ticket *tik);

/** \brief      Writes a ticket.
 *  \param tik  Ticket to write.
 *  \param ws   Output write stream.
 */
nnc_result nnc_write_ticket(nnc_ticket *tik, nnc_wstream *ws);

/** \brief         Hashes part of the ticket required for \ref nnc_verify_signature.
 *  \param rs      Stream to hash from.
 *  \param tik     Ticket data.
 *  \param digest  Output hash.
 *  \returns
 *  Anything \ref nnc_sighash can return.\n
 *  \p NNC_R_INVALID_SIG => Invalid signature.
 */
nnc_result nnc_ticket_signature_hash(nnc_rstream *rs, nnc_ticket *tik, nnc_sha_hash digest);

NNC_END
#endif

