/** \file  sigcert.h
 *  \brief Signature & certificate chain parsing code that is re-used throughout several 3ds formats.
 */
#ifndef inc_nnc_sigcert_h
#define inc_nnc_sigcert_h

#include "stream.h"
#include "crypto.h"
#include "base.h"
NNC_BEGIN

enum nnc_sigtype {
	NNC_SIG_RSA_4096_SHA1   = 0,   ///< RSA 4096 with SHA1 (unused for 3DS).
	NNC_SIG_RSA_2048_SHA1   = 1,   ///< RSA 2048 with SHA1 (unused for 3DS).
	NNC_SIG_ECDSA_SHA1      = 2,   ///< Elliptic Curve with SHA1 (unused for 3DS).
	NNC_SIG_RSA_4096_SHA256 = 3,   ///< RSA 4096 with SHA256.
	NNC_SIG_RSA_2048_SHA256 = 4,   ///< RSA 2048 with SHA256.
	NNC_SIG_ECDSA_SHA256    = 5,   ///< Elliptic Curve with SHA256.
	NNC_SIG_NONE            = 200, ///< Value used (only) by nnc to write an empty signature. Add to this constant the signature type.
};

typedef struct nnc_signature {
	enum nnc_sigtype type; ///< Signature type.
	nnc_u8 data[0x200];    ///< Signature data. May not always be used entirely.
	char issuer[0x41];     ///< Signature issuer (NULL-terminated).
} nnc_signature;

typedef struct nnc_certificate {
	nnc_signature sig;  ///< Signature for certificate.
	enum nnc_certificate_type {
		NNC_CERT_RSA_4096 = 0,
		NNC_CERT_RSA_2048 = 1,
		NNC_CERT_ECDSA    = 2,
	} type;             ///< Certificate type.
	char name[0x41];    ///< Certificate issuer (NULL-terminated).
	union nnc_certificate_data {
		struct nnc_certificate_rsa2048 {
			nnc_u8 modulus[0x100]; ///< Modulus.
			nnc_u8 exp[0x4];       ///< Exponent.
		} rsa2048;               ///< Used for type = NNC_CERT_RSA_2048
		struct nnc_certificate_rsa4096 {
			nnc_u8 modulus[0x200]; ///< Modulus.
			nnc_u8 exp[0x4];       ///< Exponent.
		} rsa4096;               ///< Used for type = NNC_CERT_RSA_4096
		struct nnc_certificate_ecdsa {
			nnc_u8 pubkey[0x3C];   ///< Public key.
		} ecdsa;                 ///< Used for type = NNC_CERT_ECDSA.
		nnc_u8 raw[0x204];       ///< Raw bytes.
	} data;             ///< Certificate data.
	nnc_u32 expiration; ///<
} nnc_certificate;

typedef struct nnc_certchain {
	nnc_certificate *certs; ///< Certificate array of size \p len.
	int len;                ///< Amount of certificates.
} nnc_certchain;

/** \brief      Gets the signature size from the type.
 *  \return     Returns total signature size (<u>including</u> the 4 identifying bytes), <u>not</u> including the issuer (size=0x40) or 0 if invalid.
 *  \param sig  Signature type.
 *  \note       If `sig` is NNC_SIG_NONE This function returns the amount of bytes that nulled signature will take up (still without the issuer).
 */
nnc_u16 nnc_sig_size(enum nnc_sigtype sig);

/** \brief      Gets the size of the signature data field for \p type.
 *  \return     Size of \ref nnc_signature::data or 0 if invalid.
 *  \param sig  Signature type
 *  \note       This function returns 0 with NNC_SIG_NONE as there is no data.
 */
nnc_u16 nnc_sig_dsize(enum nnc_sigtype sig);

/** \brief      Reads entire signature and seeks to end.
 *  \param rs   Stream to read signature from.
 *  \param sig  Output signature.
 *  \note       Does not seek to position 0, but reads from the current offset.
 */
nnc_result nnc_read_sig(nnc_rstream *rs, nnc_signature *sig);

/** \brief      Write a signature.
 *  \param sig  The signature to write.
 *  \param ws   Output stream.
 */
nnc_result nnc_write_sig(nnc_signature *sig, nnc_wstream *ws);

/** \brief      Signature type to string
 *  \return     String version of signature type or NULL if invalid.
 *  \param sig  Signature type.
 */
const char *nnc_sigstr(enum nnc_sigtype sig);

/** \brief        Verifies a signature.
 *  \param chain  Chain to select certificate from.
 *  \param sig    Signature to verify with.
 *  \param hash   Hash to verify.
 *  \return
 *  \p NNC_R_OK => Signature passed verification.\n
 *  \p NNC_R_BAD_SIG => Signature failed verification.\n
 *  \p NNC_R_CERT_NOT_FOUND => Certificate not found in \p chain.
 *  \p NNC_R_INVALID_SIG => Invalid signature.
 */
nnc_result nnc_verify_signature(nnc_certchain *chain, nnc_signature *sig, nnc_sha_hash hash);

/** \brief         Selects either sha1 or sha256 based on \p sig.
 *  \param rs      Stream to read data from.
 *  \param sig     Signature type.
 *  \param digest  Output digest.
 *  \param size    Amount of data to hash.
 *  \return
 *  Anything \ref nnc_crypto_sha256_part can return.\n
 *  Anything \ref nnc_crypto_sha1_part can return.\n
 *  \p NNC_R_INVALID_SIG => \p sig is invalid.
 */
nnc_result nnc_sighash(nnc_rstream *rs, enum nnc_sigtype sig, nnc_sha_hash digest, nnc_u32 size);

/** \brief         Reads a certificate chain found in TMDs and tickets acquired from the CDN and CIAs.
 *  \param rs      Output certificate chain.
 *  \param chain   Output certificate chain. If extend is true this may be a chain written to by an earlier call to \ref nnc_read_certchain or \ref nnc_scan_certchains.
 *  \param extend  Use realloc() for first allocation instead of malloc().
 *  \note          You should always call \ref nnc_free_certchain after you're done with \p chain.
 *  \note          If `chain` is NULL, this function performs some checks to see if this is a certificate chain and returns an error if not.
 *  \return
 *  Anything rs->read() can return.\n
 *  Anything \ref nnc_read_sig can return.\n
 *  \p NNC_R_INVALID_CERT => Invalid certificate type.\n
 *  \p NNC_R_TOO_SMALL => Most likely a misaligned/invalid certificate chain.\n
 *  \p NNC_R_NOMEM => Failed to allocate memory for certificate chain.
 */
nnc_result nnc_read_certchain(nnc_rstream *rs, nnc_certchain *chain, bool extend);

/** \brief        Scans support directories for specific certificate chains in this order:\n
 *                 - CA00000003-CP0000000b.bin: Usually contains certificates for TMDs.\n
 *                 - CA00000003-XS0000000c.bin: Usually contains certificates for tickets.\n
 *                 - CA00000004-XS00000009.bin: Usually contains certificates for TMDs (developer).\n
 *                 - CA00000004-CP0000000a.bin: Usually contains certificates for tickets (developer).\n
 *                 - cert_bundle.bin: (nnc-specific) Container for all certificates.
 *  \param chain  Output certificate chain.
 *  \see          \ref nnc_scan_seeddb for the support directories scanned.
 *  \note         This function does not return anything. Any missing/bad chains are ignored.
 *  \note         You should always call \ref nnc_free_certchain after you're done with \p chain.
 */
void nnc_scan_certchains(nnc_certchain *chain);

/** \brief        free()s dynamic memory in use by certificate chain.
 *  \param chain  Certificate chain to free.
 */
void nnc_free_certchain(nnc_certchain *chain);

NNC_END
#endif

