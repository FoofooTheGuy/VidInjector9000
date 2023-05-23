
#include "nnc/u128.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "./internal.h"


u128 *nnc_u128_xor(u128 *a, const u128 *b)
{
	a->hi ^= b->hi;
	a->lo ^= b->lo;
	return a;
}

u128 *nnc_u128_rol(u128 *a, u8 n)
{
	if(n < 64)
	{
		/* save highest n as low to OR back in later */
		u64 saved_hi = (a->hi & ~(UINT64_MAX >> n)) >> (64 - n);
		u64 saved_lo = (a->lo & ~(UINT64_MAX >> n)) >> (64 - n);
		a->hi <<= n;
		a->lo <<= n;
		a->hi |= saved_lo;
		a->lo |= saved_hi;
	}
	else
	{
		printf("EINVAL: rol: n >= 64 (n=%i)\n", n);
		abort();
	}
	return a;
}

u128 *nnc_u128_ror(u128 *a, u8 n)
{
	if(n < 64)
	{
		/* save lowest n as high to OR back in later */
		u64 saved_hi = (a->hi & ~(UINT64_MAX << n)) << (64 - n);
		u64 saved_lo = (a->lo & ~(UINT64_MAX << n)) << (64 - n);
		a->hi >>= n;
		a->lo >>= n;
		a->hi |= saved_lo;
		a->lo |= saved_hi;
	}
	else
	{
		printf("EINVAL: ror: n >= 64 (n=%i)\n", n);
		abort();
	}
	return a;
}

u128 *nnc_u128_add(u128 *a, const u128 *b)
{
	u64 add = a->lo + b->lo;
	if(add < a->lo)
		++a->hi;
	a->hi += b->hi;
	a->lo = add;
	return a;
}

#define HALF (NNC_U128_LEN / 2)
u128 nnc_u128_from_hex(const char *s)
{
	if(strncmp(s, "0x", 2) == 0)
		s += 2;
	int end = strlen(s);
	if(end > NNC_U128_LEN)
		return (u128) { 0, 0 };
	u128 ret;
	if(end > HALF)
	{
		int cutoff = end - HALF;
		char buf[HALF + 1];
		buf[HALF] = '\0';
		memcpy(buf, s, cutoff);
		ret.lo = strtoull(s + cutoff, NULL, 16);
		ret.hi = strtoull(buf, NULL, 16);
	}
	else
	{
		ret.lo = strtoull(s, NULL, 16);
		ret.hi = 0;
	}
	return ret;
}

void nnc_u128_bytes_be(const u128 *a, u8 bytes[0x10])
{
	u64 *bytes64 = (u64 *) bytes;
	bytes64[0] = BE64(a->hi);
	bytes64[1] = BE64(a->lo);
}

u128 nnc_u128_import_be(nnc_u8 bytes[0x10])
{
	u64 *cast = (u64 *) bytes;
	return (u128) { .hi = BE64(cast[0]), .lo = BE64(cast[1]) };
}

