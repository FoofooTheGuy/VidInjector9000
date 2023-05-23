/** \file   u128.h
 *  \brief  An simple 128-bit unsigned integer implementation.
 */
#ifndef inc_nnc_u128_h
#define inc_nnc_u128_h

#include "base.h"
#include <inttypes.h>
NNC_BEGIN

typedef struct nnc_u128 {
	nnc_u64 hi;
	nnc_u64 lo;
} nnc_u128;

/** \brief Put a constant in an nnc_u128, Usage:
 *  \code
 *  NNC_HEX128(0x1234567887654321,1234567887654321)
 *  \endcode
 */
#define NNC_HEX128(hi, lo) \
	{ hi, 0x##lo }

/** \brief Promote a u8, u16, u32 or u64 to an u128
 *  \code
 *  nnc_u64 small = 0x424242;
 *  nnc_u128 promoted = NNC_PROMOTE128(smalll);
 *  \endcode
 */
#define NNC_PROMOTE128(lo) \
	{ 0, lo }

/** Printf format for lowercase u128 hex. */
#define NNC_FMT128_LOWER "%016" PRIx64 "%016" PRIx64
/** Printf format for uppercase u128 hex. */
#define NNC_FMT128 "%016" PRIX64 "%016" PRIX64
/** Printf arguments for \ref NNC_FMT128 and \ref NNC_FMT128_LOWER. */
#define NNC_ARG128(u) (u).hi, (u).lo
/** Length of a u128 in hex without the "0x" prefix. */
#define NNC_U128_LEN 0x20

/** Xor \p a with \p b saving the result in \p a and returning the pointer. */
nnc_u128 *nnc_u128_xor(nnc_u128 *a, const nnc_u128 *b);
/** Add \p a with \p b saving the result in \p a and returning the pointer. */
nnc_u128 *nnc_u128_add(nnc_u128 *a, const nnc_u128 *b);
/** Rotate \p a \p n bits to the right saving the result in \p a and returning the pointer, \p n is at max 63. */
nnc_u128 *nnc_u128_rol(nnc_u128 *a, nnc_u8 n);
/** Rotate \p a \p n bits to the left saving the result in \p a and returning the pointer, \p n is at max 63. */
nnc_u128 *nnc_u128_ror(nnc_u128 *a, nnc_u8 n);
/** Export the u128 as big-endian bytes */
void nnc_u128_bytes_be(const nnc_u128 *a, nnc_u8 bytes[0x10]);
/** Parse a u128 from a hex string, optionally prefixed with "0x". */
nnc_u128 nnc_u128_from_hex(const char *s);
/** Create a u128 from big-endian bytes */
nnc_u128 nnc_u128_import_be(nnc_u8 bytes[0x10]);

NNC_END
#endif

