
#ifndef inc_internal_h
#define inc_internal_h

#include "nnc/base.h"

#define BLOCK_SZ 0x10000

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

#define u8 nnc_u8
#define u16 nnc_u16
#define u32 nnc_u32
#define u64 nnc_u64
#define u128 nnc_u128

#define i8 nnc_i8
#define i16 nnc_i16
#define i32 nnc_i32
#define i64 nnc_i64

#define result nnc_result

#ifdef __GNUC__
	#define BSWAP_BUILTIN
	#define bswap16 __builtin_bswap16
	#define bswap32 __builtin_bswap32
	#define bswap64 __builtin_bswap64
#else
	#define bswap16 nnc_bswap16
	#define bswap32 nnc_bswap32
	#define bswap64 nnc_bswap64
#endif

#define rstream nnc_rstream

#define BE16(a) (bswap16(a))
#define BE32(a) (bswap32(a))
#define BE64(a) (bswap64(a))

#define LE16(a) ((u16) (a))
#define LE32(a) ((u32) (a))
#define LE64(a) ((u64) (a))

#define U16P(a) (* (u16 *) (a))
#define U32P(a) (* (u32 *) (a))
#define U64P(a) (* (u64 *) (a))

#define BE16P(a) BE16(U16P(a))
#define BE32P(a) BE32(U32P(a))
#define BE64P(a) BE64(U64P(a))

#define LE16P(a) LE16(U16P(a))
#define LE32P(a) LE32(U32P(a))
#define LE64P(a) LE64(U64P(a))

#define TRYB(expr) if(( expr ) != NNC_R_OK) return false
#define TRYV(expr) if(( expr ) != NNC_R_OK) return
#define TRY(expr) if((ret = ( expr )) != NNC_R_OK) return ret
#define TRYLBL(expr, label) if((ret = ( expr )) != NNC_R_OK) goto label

#define ALIGN(a, n)        (((a) + ((n) - 1)) & ~((n) - 1)) /* N.B.: `n' must be a power of 2! */
#define ALIGN_DOWN(a, n)   (((a) & ~((n) - 1)))             /* N.B.: `n' must be a power of 2! */
#define IS_UNALIGNED(a, n) ((a) & ((n) - 1))                /* N.B.: `n' must be a power of 2! */

#define NNC_BYTE_TO_MU(a) (ALIGN(a, NNC_MEDIA_UNIT) / NNC_MEDIA_UNIT)

#ifndef BSWAP_BUILTIN
nnc_u16 nnc_bswap16(nnc_u16 a);
nnc_u32 nnc_bswap32(nnc_u32 a);
nnc_u64 nnc_bswap64(nnc_u64 a);
#endif

#if defined(_WIN32) || defined(__MINGW32__) || defined(__CYGWIN__)
	#define NNC_PLATFORM_WINDOWS 1
#elif defined(__unix__) || defined(__linux__) || defined(__APPLE__)
	#define NNC_PLATFORM_UNIX 1
#elif defined(_3DS) || defined(__3DS__)
	#define NNC_PLATFORM_3DS 1
#endif

/* forward declaration from stream.h */
struct nnc_rstream;
#define read_at_exact nnc_read_at_exact
result nnc_read_at_exact(struct nnc_rstream *rs, u32 offset, u8 *data, u32 dsize);
#define read_exact nnc_read_exact
result nnc_read_exact(struct nnc_rstream *rs, u8 *data, u32 dsize);
#define dumpmem nnc_dumpmem
/* for debugging */
void nnc_dumpmem(void *mem, u32 len);
#define find_support_file nnc_find_support_file
#define SUP_FILE_NAME_LEN (1024 + 1)
bool nnc_find_support_file(const char *name, char *output);
#define strdup nnc_strdup
char *nnc_strdup(const char *s);

struct dynbuf {
	u8 *buffer;
	u32 alloc, used;
};

#define dynbuf_new nnc_dynbuf_new
nnc_result nnc_dynbuf_new(struct dynbuf *db, u32 initial_size);
#define dynbuf_push nnc_dynbuf_push
nnc_result nnc_dynbuf_push(struct dynbuf *db, u8 *data, u32 len);
#define dynbuf_free nnc_dynbuf_free
void nnc_dynbuf_free(struct dynbuf *db);

#endif

