
/** \file  base.h
 *  \brief Basic type definitions & functions.
 */
#ifndef inc_nnc_base_h
#define inc_nnc_base_h

#ifdef __cplusplus
	#define NNC_BEGIN extern "C" {
	#define NNC_END   }
#else
	#define NNC_BEGIN
	#define NNC_END
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
NNC_BEGIN

/** Media units to bytes. */
#define NNC_MU_TO_BYTE(a) ((a) * NNC_MEDIA_UNIT)
/** Amount of bytes in a media unit. */
#define NNC_MEDIA_UNIT 0x200

typedef uint8_t nnc_u8;   ///< 8-bit unsigned integer.
typedef uint16_t nnc_u16; ///< 16-bit unsigned integer.
typedef uint32_t nnc_u32; ///< 32-bit unsigned integer.
typedef uint64_t nnc_u64; ///< 64-bit unsigned integer.

typedef int8_t nnc_i8;   ///< 8-bit signed integer.
typedef int16_t nnc_i16; ///< 16-bit signed integer.
typedef int32_t nnc_i32; ///< 32-bit signed integer.
typedef int64_t nnc_i64; ///< 64-bit signed integer.

typedef float nnc_f32;  ///< 32-bit signed float.
typedef double nnc_f64; ///< 64-bit signed float.

typedef nnc_u32 nnc_result; ///< Result type.

enum nnc_result_codes {
	NNC_R_OK,              ///< Everything went fine.
	NNC_R_FAIL_OPEN,       ///< Failed to open.
	NNC_R_SEEK_RANGE,      ///< Invalid seek range.
	NNC_R_FAIL_READ,       ///< Failed to read.
	NNC_R_TOO_SMALL,       ///< Reading yielded too little data.
	NNC_R_TOO_LARGE,       ///< Input data too large.
	NNC_R_INVALID_SIG,     ///< Invalid signature type.
	NNC_R_CORRUPT,         ///< Used if hash or magic doesn't match up.
	NNC_R_NOMEM,           ///< Out of memory.
	NNC_R_NOT_FOUND,       ///< Not found.
	NNC_R_NOT_A_FILE,      ///< Not a file.
	NNC_R_KEY_NOT_FOUND,   ///< Key not found.
	NNC_R_MISMATCH,        ///< Mismatch.
	NNC_R_SEED_NOT_FOUND,  ///< Seed not found.
	NNC_R_UNSUPPORTED,     ///< Unsupported.
	NNC_R_INVAL,           ///< Invalid parameter(s).
	NNC_R_BAD_ALIGN,       ///< Bad alignment.
	NNC_R_BAD_SIG,         ///< Signature failed verification.
	NNC_R_CERT_NOT_FOUND,  ///< Certificate not found.
	NNC_R_INVALID_CERT,    ///< Invalid certificate type.
	NNC_R_FAIL_WRITE,      ///< Failed to write.
	NNC_R_NOT_OPEN,        ///< File not open.
	NNC_R_OPEN,            ///< File open.
	NNC_R_OS,              ///< Generic operating system error.
	NNC_R_INTERNAL,        ///< Internal library error, this should not happen.
};

enum nnc_tid_category {
	NNC_TID_CAT_NORMAL    = 0x0,     ///< Exact normal (= games/homebrew) category.
	NNC_TID_CAT_DLP_CHILD = 0x1,     ///< DLP Child.
	NNC_TID_CAT_DEMO      = 0x2,     ///< Exact demo category.
	NNC_TID_CAT_AOC       = 0x4,     ///< Add-On Content.
	NNC_TID_CAT_NO_EXE    = 0x8,     ///< Can't execute.
	NNC_TID_CAT_UPDATE    = 0xE,     ///< Exact update category (NNC_TID_CAT_DEMO | NNC_TID_CAT_AOC | NNC_TID_CAT_NO_EXE).
	NNC_TID_CAT_SYSTEM    = 0x10,    ///< System.
	NNC_TID_CAT_NOT_MOUNT = 0x80,    ///< Not require right for mount.
	NNC_TID_CAT_DLC       = 0x8C,    ///< Exact DLC category (NNC_TID_CAT_DEMO | NNC_TID_CAT_AOC | NNC_TID_CAT_NO_EXE | NNC_TID_CAT_NOT_MOUNT).
	NNC_TID_CAT_TWL       = 0x8000,  ///< TWL (DSi).
};


/** \brief        Parse a version int into the components major.minor.patch
 *  \param ver    Input version int.
 *  \param major  (optional) Output major version.
 *  \param minor  (optional) Output minor version.
 *  \param patch  (optional) Output patch version.
 */
void nnc_parse_version(nnc_u16 ver, nnc_u8 *major, nnc_u8 *minor, nnc_u8 *patch);

/** \brief      Gets an result string from an nnc_result.
 *  \param res  Result to get string from.
 *  \returns    NULL if \p res is invalid.
 */
const char *nnc_strerror(nnc_result res);

/** \brief    Computes the log2 result of x.
 *  \param x  Value to calculate the log2 of.
 *  \note     Returns 0 if `x` is not a power of 2.
 */
nnc_u32 nnc_log2(nnc_u32 x);

/** \brief      Calculates the `exp`th power of 2.
 *  \param exp  The power to raise to.
 * */
nnc_u32 nnc_pow2(nnc_u32 exp);

/** \{
 *  \anchor tid
 *  \name   Title IDs
 */

/** Empty 3ds title ID. */
#define NNC_BASE_TID 0x0004000000000000

/** Gets the category of a title ID, to be used with \ref nnc_tid_category. */
nnc_u16 nnc_tid_category(nnc_u64 tid);
/** Gets the unique ID of a title ID. */
nnc_u32 nnc_tid_unique_id(nnc_u64 tid);
/** Gets the variation of a title ID. */
nnc_u8 nnc_tid_variation(nnc_u64 tid);
/** Sets the category of a title ID, use values from \ref nnc_tid_category. */
void nnc_tid_set_category(nnc_u64 *tid, nnc_u16 category);
/** Sets the unique ID of a title ID. */
void nnc_tid_set_unique_id(nnc_u64 *tid, nnc_u32 uniqid);
/** Sets the variation of a title ID. */
void nnc_tid_set_variation(nnc_u64 *tid, nnc_u8 variation);

/** \} */

NNC_END
#endif

