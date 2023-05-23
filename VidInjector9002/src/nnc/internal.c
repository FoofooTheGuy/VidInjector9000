
#include "nnc/stream.h"
#include "nnc/base.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "./internal.h"


#ifndef BSWAP_BUILTIN
static void bswap(u8* r, u8* n, u8 len)
{
	for (u8 i = 0; i < len; ++i)
		r[i] = n[len - i - 1];
}

#define MKBSWAP(n) \
	u##n nnc_bswap##n(u##n a) \
	{ u##n r; bswap((u8 *) &r, (u8 *) &a, n/8); return r; }

MKBSWAP(16)
MKBSWAP(32)
MKBSWAP(64)
#endif

result nnc_read_at_exact(nnc_rstream* rs, u32 offset, u8* data, u32 dsize)
{
	result ret;
	u32 size;
	TRY(NNC_RS_PCALL(rs, seek_abs, offset));
	TRY(NNC_RS_PCALL(rs, read, data, dsize, &size));
	return size == dsize ? NNC_R_OK : NNC_R_TOO_SMALL;
}

result nnc_read_exact(nnc_rstream* rs, u8* data, u32 dsize)
{
	result ret;
	u32 size;
	TRY(NNC_RS_PCALL(rs, read, data, dsize, &size));
	return size == dsize ? NNC_R_OK : NNC_R_TOO_SMALL;
}

/* also contains implementations from in base.h */

void nnc_parse_version(u16 ver, u8* major, u8* minor, u8* patch)
{
	if (major) *major = (ver >> 10) & 0x3F;
	if (minor) *minor = (ver >> 4) & 0x3F;
	if (patch) *patch = (ver) & 0xF;
}

u16 nnc_tid_category(u64 tid)
{
	return (tid >> 32) & 0xFFFF;
}

u32 nnc_tid_unique_id(u64 tid)
{
	return (tid >> 8) & 0xFFFFFF;
}

u8 nnc_tid_variation(u64 tid)
{
	return (tid) & 0xFF;
}

void nnc_tid_set_category(u64* tid, u16 category)
{
	*tid = (*tid & 0xFFFF0000FFFFFFFF) | ((u64)(category & 0xFFFF) << 32);
}

void nnc_tid_set_unique_id(u64* tid, u32 uniqid)
{
	*tid = (*tid & 0xFFFFFFFF000000FF) | ((u64)(uniqid & 0xFFFFFF) << 8);
}

void nnc_tid_set_variation(u64* tid, u8 variation)
{
	*tid = (*tid & 0xFFFFFFFFFFFFFF00) | ((u64)(variation & 0xFF));
}

void nnc_dumpmem(void* mem_, u32 len)
{
	u8* mem = mem_;
	const char* atfmt = "0x%02X: ";
	/* a little ugly but it works */
	if (len > 0xFFFFFFFF) atfmt = "0x%016X: ";
	else if (len > 0xFFFF) atfmt = "0x%08X: ";
	else if (len > 0xFF) atfmt = "0x%04X: ";
	if (len <= 0x10) atfmt = "";
	for (u32 i = 0; i < len; i += 0x10)
	{
		int row = MIN(0x10, len - i);
		printf(atfmt, i);
		for (int j = 0; j < row; ++j)
			printf("%02X ", mem[i + j]);
		printf("%*s", (0x10 - row) * 3, "");
		printf("| ");
		for (int j = 0; j < row; ++j)
			printf("%c", isgraph(mem[i + j]) ? mem[i + j] : '.');
		puts("");
	}
}

const char* nnc_strerror(nnc_result res)
{
	switch (res)
	{
	case NNC_R_OK: return "OK";
	case NNC_R_FAIL_OPEN: return "failed to open";
	case NNC_R_SEEK_RANGE: return "invalid seek range";
	case NNC_R_FAIL_READ: return "failed to read";
	case NNC_R_TOO_SMALL: return "data too small";
	case NNC_R_TOO_LARGE: return "data too large";
	case NNC_R_INVALID_SIG: return "invalid signature type";
	case NNC_R_CORRUPT: return "corrupt";
	case NNC_R_NOMEM: return "out of memory";
	case NNC_R_NOT_FOUND: return "not found";
	case NNC_R_NOT_A_FILE: return "not a file";
	case NNC_R_KEY_NOT_FOUND: return "key not found";
	case NNC_R_MISMATCH: return "mismatch";
	case NNC_R_SEED_NOT_FOUND: return "seed not found";
	case NNC_R_UNSUPPORTED: return "not supported";
	case NNC_R_INVAL: return "invalid parameter";
	case NNC_R_BAD_ALIGN: return "bad alignment";
	case NNC_R_BAD_SIG: return "signature failed verification";
	case NNC_R_CERT_NOT_FOUND: return "certificate not found";
	case NNC_R_INVALID_CERT: return "invalid certificate type";
	case NNC_R_FAIL_WRITE: return "failed to write";
	case NNC_R_NOT_OPEN: return "not open";
	case NNC_R_OPEN: return "already open";
	case NNC_R_OS: return "OS error";
	case NNC_R_INTERNAL: return "internal nnc error";
	}
	return NULL;
}

u32 nnc_log2(u32 x)
{
	/* x is not a power of 2 */
	if (x == 0 || x & (x - 1))
		return 0;
#ifdef __GNUC__
	return __builtin_ctz(x);
#else
	u32 i;
	for (i = 0; i < sizeof(u32) * 8 - 1; ++i)
		if (x & (1 << i)) break;
	return i;
#endif
}

u32 nnc_pow2(u32 exp)
{
	return 1 << exp;
}

#if NNC_PLATFORM_UNIX || defined(__CYGWIN__)
#include <unistd.h>
#define UNIX_LIKE
#define can_read(f) access(f, R_OK) == 0
#elif NNC_PLATFORM_WINDOWS
#include <io.h>
#define can_read(f) _access_s(f, 4) == 0
#endif

bool nnc_find_support_file(const char* name, char* output)
{
#define CHECK_BASE(...) do { \
		if(snprintf(output, SUP_FILE_NAME_LEN, __VA_ARGS__) > SUP_FILE_NAME_LEN) \
		  break; \
		if(can_read(output)) \
			return true; \
	} while(0)
	char* envptr;
#define CHECKE(path) CHECK_BASE("%s/%s/%s", envptr, path, name)
#define CHECK(path) CHECK_BASE("%s/%s", path, name)

#ifdef NNC_PLATFORM_UNIX
	if ((envptr = getenv("HOME")))
	{
		CHECKE(".config/3ds");
		CHECKE("3ds");
		CHECKE(".3ds");
	}
	CHECK("/usr/share/3ds");
#elif NNC_PLATFORM_WINDOWS
	if ((envptr = getenv("USERPROFILE")))
	{
		CHECKE("3ds");
		CHECKE(".3ds");
	}
	if ((envptr = getenv("APPDATA")))
		CHECKE("3ds");
#else
	/* no clue where to look */
	(void)envptr;
	(void)output;
	(void)name;
#endif

	/* nothing found */
	return false;
#undef CHECK_BASE
#undef CHECKE
#undef CHECK
}

char* nnc_strdup(const char* s)
{
	if (!s) return NULL;
	size_t len = strlen(s) + 1;
	char* ret = malloc(len);
	if (!ret) return NULL;
	memcpy(ret, s, len);
	return ret;
}

nnc_result nnc_dynbuf_new(struct dynbuf* db, u32 initial_size)
{
	db->buffer = malloc(initial_size);
	if (!db->buffer)
		return NNC_R_NOMEM;
	db->used = 0;
	db->alloc = initial_size;
	return NNC_R_OK;
}

nnc_result nnc_dynbuf_push(struct dynbuf* db, u8* data, u32 len)
{
	u32 new_used = db->used + len;
	if (new_used >= db->alloc)
	{
		u32 new_alloc = db->alloc * 2;
		u8* new_buf = realloc(db->buffer, new_alloc);
		if (!new_buf) return NNC_R_NOMEM;
		db->buffer = new_buf;
		db->alloc = new_alloc;
	}
	memcpy(&db->buffer[db->used], data, len);
	db->used = new_used;
	return NNC_R_OK;
}

void nnc_dynbuf_free(struct dynbuf* db)
{
	free(db->buffer);
}