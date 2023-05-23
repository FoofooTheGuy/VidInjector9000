
#include "mbedtls/mbedtls/version.h"
#include "nnc/sigcert.h"
#include "mbedtls/mbedtls/pk.h"
#include <string.h>
#include <stdlib.h>
#include "./internal.h"

/* In MbedTLS version 3 struct members are now accessed with MBEDTLS_PRIVATE */
#if MBEDTLS_VERSION_MAJOR == 3
#define ACCESS_PRIV(name) MBEDTLS_PRIVATE(name)
#else
#define ACCESS_PRIV(name) name
#endif

#define NNC_SIGTYPE_IS_NONE(s) ((s) >= NNC_SIG_NONE && (s) <= NNC_SIG_NONE + NNC_SIG_ECDSA_SHA256)

#define SIGN_MAX 5
#define CERT_MAX 2

static u16 size_lut[0x6] = { 0x200, 0x100, 0x3C, 0x200, 0x100, 0x3C };
static u16 pad_lut[0x6] = { 0x3C,  0x3C,  0x40, 0x3C,  0x3C,  0x40 };
#define SIG_MAX_SIZE 0x240


u16 nnc_sig_size(enum nnc_sigtype sig)
{
	if (NNC_SIGTYPE_IS_NONE(sig))
		sig -= NNC_SIG_NONE;
	if (sig > SIGN_MAX) return 0;
	return size_lut[sig] + pad_lut[sig] + 0x04;
}

u16 nnc_sig_dsize(enum nnc_sigtype sig)
{
	if (sig > SIGN_MAX) return 0;
	return size_lut[sig];
}

result nnc_read_sig(rstream* rs, nnc_signature* sig)
{
	/* this function is a tad bit cursed due to alignment */
	result ret;
	u8 signum[4 + 12];
	TRY(read_exact(rs, signum, sizeof(signum)));
	if (signum[0] != 0x00 || signum[1] != 0x01 || signum[2] != 0x00 || signum[3] > SIGN_MAX)
		return NNC_R_INVALID_SIG;
	sig->type = signum[3];
	nnc_u8 sigdata[0x270];
	u16 total_sig_read_size = size_lut[sig->type] + pad_lut[sig->type] - 12;
	TRY(read_exact(rs, sigdata, total_sig_read_size + 0x40));
	memcpy(sig->data, &signum[4], 12);
	memcpy(&sig->data[12], sigdata, size_lut[sig->type] - 12);
	memcpy(sig->issuer, &sigdata[total_sig_read_size], 0x40);
	sig->issuer[0x40] = '\0';
	return NNC_R_OK;
}

nnc_result nnc_write_sig(nnc_signature* sig, nnc_wstream* ws)
{
	if (NNC_SIGTYPE_IS_NONE(sig->type))
	{
		u8 data[SIG_MAX_SIZE + 0x40 /* + issuer */];
		memset(data, 0x00, sizeof(data));
		data[0x01] = 1; /* absolutely required... */
		data[0x03] = sig->type - NNC_SIG_NONE;
		/* we need to have an issuer or we fail */
		memcpy(&data[nnc_sig_size(sig->type)], sig->issuer, sizeof(sig->issuer));
		return NNC_WS_PCALL(ws, write, data, nnc_sig_size(sig->type) + 0x40);
	}
	if (sig->type > SIGN_MAX)
		return NNC_R_INVALID_SIG;
	u8 data[0x240 + 0x40];
	data[0] = 0x00;
	data[1] = 0x01;
	data[2] = 0x00;
	data[3] = sig->type;
	u16 data_size = size_lut[sig->type];
	u16 pad_size = pad_lut[sig->type];
	memcpy(&data[4], sig->data, data_size);
	memset(&data[4 + data_size], 0, pad_size);
	memcpy(&data[4 + data_size + pad_size], sig->issuer, 0x40);
	return NNC_WS_PCALL(ws, write, data, 4 + data_size + pad_size + 0x40);
}

const char* nnc_sigstr(enum nnc_sigtype sig)
{
	switch (sig)
	{
	case NNC_SIG_RSA_4096_SHA1:
		return "RSA 4096 - SHA1";
	case NNC_SIG_RSA_2048_SHA1:
		return "RSA 2048 - SHA1";
	case NNC_SIG_ECDSA_SHA1:
		return "Elliptic Curve - SHA1";
	case NNC_SIG_RSA_4096_SHA256:
		return "RSA 4096 - SHA256";
	case NNC_SIG_RSA_2048_SHA256:
		return "RSA 2048 - SHA256";
	case NNC_SIG_ECDSA_SHA256:
		return "Elliptic Curve - SHA256";
	case NNC_SIG_NONE:
		return "Signature - NULL";
	}
	return NULL;
}

static void import_rsa(mbedtls_rsa_context* ctx, u8* mod, u16 mod_size, u8* exp)
{
	mbedtls_mpi_read_binary(&ctx->ACCESS_PRIV(N), mod, mod_size);
	mbedtls_mpi_read_binary(&ctx->ACCESS_PRIV(E), exp, 0x4);
	ctx->ACCESS_PRIV(len) = mod_size;
}

static bool setup_pk(nnc_certchain* chain, nnc_signature* sig, mbedtls_pk_context* ctx)
{
	nnc_certificate* cert;
	/* (usually?) in the form (issuer user)-(certificate used to verify certificate)-(certificate name) */
	char* signame = strrchr(sig->issuer, '-');
	if (signame) ++signame;
	else        signame = sig->issuer; /* fall back to full issuer */
	for (int i = 0; i < chain->len; ++i)
	{
		cert = &chain->certs[i];
		if (strcmp(cert->name, signame) == 0)
		{
			/* found cert we want to import */
			switch (cert->type)
			{
			case NNC_CERT_RSA_2048:
				if (!(sig->type == NNC_SIG_RSA_2048_SHA1 || sig->type == NNC_SIG_RSA_2048_SHA256))
					continue; /* invalid cert/sig pair */
				import_rsa(mbedtls_pk_rsa(*ctx), cert->data.rsa2048.modulus,
					0x100, cert->data.rsa2048.exp);
				return true;
			case NNC_CERT_RSA_4096:
				if (!(sig->type == NNC_SIG_RSA_4096_SHA1 || sig->type == NNC_SIG_RSA_4096_SHA256))
					continue; /* invalid cert/sig pair */
				import_rsa(mbedtls_pk_rsa(*ctx), cert->data.rsa4096.modulus,
					0x200, cert->data.rsa4096.exp);
				return true;
			case NNC_CERT_ECDSA:
				if (!(sig->type == NNC_SIG_ECDSA_SHA1 || sig->type == NNC_SIG_ECDSA_SHA256))
					continue; /* invalid cert/sig pair */
				/* TODO: implement ECDSA certificates */
				continue;
			}
		}
	}
	return false;
}

result nnc_verify_signature(nnc_certchain* chain, nnc_signature* sig, nnc_sha_hash hash)
{
	const mbedtls_pk_info_t* pkinfo;
	switch (sig->type)
	{
	case NNC_SIG_RSA_4096_SHA1:
	case NNC_SIG_RSA_2048_SHA1:
	case NNC_SIG_RSA_4096_SHA256:
	case NNC_SIG_RSA_2048_SHA256:
		pkinfo = mbedtls_pk_info_from_type(MBEDTLS_PK_RSA);
		break;
	case NNC_SIG_ECDSA_SHA1:
	case NNC_SIG_ECDSA_SHA256:
		pkinfo = mbedtls_pk_info_from_type(MBEDTLS_PK_ECDSA);
		break;
	default:
		pkinfo = NULL;
		break;
	}
	if (!pkinfo) return NNC_R_INVALID_SIG;

	mbedtls_pk_context ctx;
	mbedtls_pk_init(&ctx);
	mbedtls_pk_setup(&ctx, pkinfo);
	if (!setup_pk(chain, sig, &ctx))
	{
		mbedtls_pk_free(&ctx);
		return NNC_R_CERT_NOT_FOUND;
	}

	bool ret;

	switch (sig->type)
	{
	case NNC_SIG_RSA_4096_SHA1:
	case NNC_SIG_RSA_2048_SHA1:
	case NNC_SIG_ECDSA_SHA1:
		ret = mbedtls_pk_verify(&ctx, MBEDTLS_MD_SHA1, hash, sizeof(nnc_sha1_hash), sig->data, nnc_sig_dsize(sig->type)) == 0;
		break;
	case NNC_SIG_RSA_4096_SHA256:
	case NNC_SIG_RSA_2048_SHA256:
	case NNC_SIG_ECDSA_SHA256:
		ret = mbedtls_pk_verify(&ctx, MBEDTLS_MD_SHA256, hash, sizeof(nnc_sha256_hash), sig->data, nnc_sig_dsize(sig->type)) == 0;
		break;
	default:
		ret = false;
	}

	mbedtls_pk_free(&ctx);
	return ret ? NNC_R_OK : NNC_R_BAD_SIG;
}

nnc_result nnc_sighash(nnc_rstream* rs, enum nnc_sigtype sig, nnc_sha_hash digest, u32 size)
{
	switch (sig)
	{
	case NNC_SIG_RSA_4096_SHA1:
	case NNC_SIG_RSA_2048_SHA1:
	case NNC_SIG_ECDSA_SHA1:
		return nnc_crypto_sha1_part(rs, digest, size);
	case NNC_SIG_RSA_4096_SHA256:
	case NNC_SIG_RSA_2048_SHA256:
	case NNC_SIG_ECDSA_SHA256:
		return nnc_crypto_sha256_part(rs, digest, size);
	case NNC_SIG_NONE:
		break;
	}
	return NNC_R_INVALID_SIG;
}

nnc_result nnc_read_certchain(nnc_rstream* rs, nnc_certchain* chain, bool extend)
{
	NNC_RS_PCALL(rs, seek_abs, 0);
	u32 size = NNC_RS_PCALL0(rs, size);
	/* typical certificate chains only have 3 certificates at most */
	u32 left = 3;
	result res;

	u32 len = 0;
	if (chain)
	{
		if (extend) chain->certs = realloc(chain->certs, sizeof(nnc_certificate) * (chain->len + 3));
		else { chain->certs = malloc(sizeof(nnc_certificate) * 3); chain->len = 0; }
		if (!chain->certs) return NNC_R_NOMEM;
		len = chain->len;
	}

	nnc_certificate dummy;
	nnc_certificate* cert = &dummy;
	while (NNC_RS_PCALL0(rs, tell) != size)
	{
		/* we need to allocate more */
		if (chain && !left)
		{
			chain->certs = realloc(chain->certs, sizeof(nnc_certificate) * (len + 3));
			if (!chain->certs) return NNC_R_NOMEM;
			left = 3;
		}
		--left;

		if (chain) cert = &chain->certs[len];
		if ((res = nnc_read_sig(rs, &cert->sig)) != NNC_R_OK)
			goto err;
		u8 first_blocks[0x48 + 8];
		if ((res = read_exact(rs, first_blocks, sizeof(first_blocks))) != NNC_R_OK)
			goto err;
		cert->type = BE32P(&first_blocks[0x00]);
		memcpy(cert->name, &first_blocks[0x04], 0x40);
		cert->name[0x40] = '\0';
		cert->expiration = LE32P(&first_blocks[0x44]);
		memcpy(cert->data.raw, &first_blocks[0x48], 8);
		u32 padding_size, cert_size;
		nnc_u8 rest_data[0x230];
		switch (cert->type)
		{
		case NNC_CERT_RSA_2048:
			cert_size = 0x104 - 8;
			padding_size = 0x34;
			break;
		case NNC_CERT_RSA_4096:
			cert_size = 0x204 - 8;
			padding_size = 0x34;
			break;
		case NNC_CERT_ECDSA:
			cert_size = 0x3C - 8;
			padding_size = 0x3C;
			break;
		default:
			res = NNC_R_INVALID_CERT;
			goto err;
		}
		if ((res = read_exact(rs, rest_data, cert_size + padding_size)) != NNC_R_OK)
			goto err;
		memcpy(&cert->data.raw[8], rest_data, cert_size);
		++len;
	}

	if (chain) chain->len = len;
	return NNC_R_OK;
err:
	if (chain && !extend)
		free(chain->certs);
	return res;
}

void nnc_scan_certchains(nnc_certchain* chain)
{
	chain->len = 0;
	bool extend = false;
	char path[SUP_FILE_NAME_LEN];
	nnc_file file;
#define DOFILE(name) \
	if(find_support_file(name, path)) \
		if(nnc_file_open(&file, path) == NNC_R_OK) { \
			nnc_read_certchain(NNC_RSP(&file), chain, extend); \
			extend = true; \
			NNC_RS_CALL0(file, close); \
		} \
	/* Certificate usually used for TMDs */
	DOFILE("CA00000003-CP0000000b.bin");
	/* Certificate usually used for tickets */
	DOFILE("CA00000003-XS0000000c.bin");
	/* Certificate usually used for TMDs (developer) */
	DOFILE("CA00000004-CP00000009.bin");
	/* Certificate usually used for tickets (developer) */
	DOFILE("CA00000004-XS0000000a.bin");
	/* Certificate used as a combination of all certificates */
	DOFILE("cert_bundle.bin");
#undef DOFILE
}

void nnc_free_certchain(nnc_certchain* chain)
{
	if (chain->len) free(chain->certs);
}
