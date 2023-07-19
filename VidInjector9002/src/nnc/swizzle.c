
#include "nnc/swizzle.h"
#include <assert.h>
#include "./internal.h"

/* This code is total chaos...... */

/* N.B.: `from' must be less than `to'! */
#define SHIFT_BIT_TO(var, to, from) (  ((var >> (from)) & 1) << (to)  )
#define UNPACK_U3_SPREAD(var, offset) \
	  SHIFT_BIT_TO(var, 0, 0 + offset) \
	| SHIFT_BIT_TO(var, 1, 2 + offset) \
	| SHIFT_BIT_TO(var, 2, 4 + offset)

#define SWIZZLE_CUR_INDEX ((y + sy) * xdim + x + sx)

#define DEFINE_UNSWIZZLE_FUNC_EX(format_name, inp_type, outp_type, endian_func, vars, conversion_routine)  \
  void nnc_unswizzle_zorder_##format_name(inp_type *inp, outp_type *outp, u16 xdim, u16 ydim)              \
  {                                                                                                        \
    u16 sx, sy, x, y, block;                                                                               \
    inp_type in_colour;                                                                                    \
    u32 j = 0;                                                                                             \
    vars                                                                                                   \
                                                                                                           \
    /* `dim' must be aligned by 8 due to the blocks being 8x8,                                             \
     * this assert is obviously suboptimal but for the sake                                                \
     * of easier code it's better than returning a result */                                               \
    assert(IS_ALIGNED(xdim, 8));                                                                           \
    assert(IS_ALIGNED(ydim, 8));                                                                           \
                                                                                                           \
    /* Nintendo swizzles their images in block of 8x8, with those blocks                                   \
     *  containing the image data z-order swizzled                                                         \
     * see: https://en.wikipedia.org/wiki/Z-order_curve */                                                 \
    for(y = 0; y < ydim; y += 8)                                                                           \
      for(x = 0; x < xdim; x += 8)                                                                         \
        for(block = 0; block < 8 * 8; ++block)                                                             \
        {                                                                                                  \
          /* The range for these is small enough to make a table                                           \
           * but it doesn't pose a large performance problem (afaik)                                       \
           * so i'll just keep it like this for simplicity */                                              \
          sx = UNPACK_U3_SPREAD(block, 0);                                                                 \
          sy = UNPACK_U3_SPREAD(block, 1);                                                                 \
          in_colour = endian_func(inp[j++]);                                                               \
          conversion_routine                                                                               \
        }                                                                                                  \
  }

#define DEFINE_UNSWIZZLE_FUNC(format_name, inp_type, outp_type, endian_func, vars, conversion_routine) \
  DEFINE_UNSWIZZLE_FUNC_EX(format_name, inp_type, outp_type, endian_func, outp_type out_colour; vars, conversion_routine outp[SWIZZLE_CUR_INDEX] = out_colour;)

#define DEFINE_SWIZZLE_FUNC_EX(format_name, inp_type, outp_type, endian_func, vars, conversion_routine)  \
  void nnc_swizzle_zorder_##format_name(inp_type *inp, outp_type *outp, u16 xdim, u16 ydim)              \
  {                                                                                                      \
    u16 sx, sy, y, x, block;                                                                             \
    outp_type out_colour;                                                                                \
    u32 j = 0;                                                                                           \
    vars                                                                                                 \
                                                                                                         \
    assert(IS_ALIGNED(xdim, 8));                                                                         \
    assert(IS_ALIGNED(ydim, 8));                                                                         \
                                                                                                         \
    for(y = 0; y < ydim; y += 8)                                                                         \
      for(x = 0; x < xdim; x += 8)                                                                       \
        for(block = 0; block < 8 * 8; ++block)                                                           \
        {                                                                                                \
          sx = UNPACK_U3_SPREAD(block, 0);                                                               \
          sy = UNPACK_U3_SPREAD(block, 1);                                                               \
          conversion_routine                                                                             \
          outp[j++] = endian_func(out_colour);                                                           \
        }                                                                                                \
  }

#define DEFINE_SWIZZLE_FUNC(format_name, inp_type, outp_type, endian_func, vars, conversion_routine) \
  DEFINE_SWIZZLE_FUNC_EX(format_name, inp_type, outp_type, endian_func, inp_type in_colour; vars, in_colour = inp[SWIZZLE_CUR_INDEX]; conversion_routine)

#define FIX_COLOUR_DEPTH(clr, in_bits, out_bits) \
	( ( ((float) (clr)) / ( (float) ( (1 << in_bits) - 1) ) ) * ( (1 << out_bits) - 1 ) )

#define ALPHA_DO_NOTHING(x)
#define ALPHA_DO_SOMETHING(x) x

#define ALPHA_DISABLE() ALPHA_DO_NOTHING
#define ALPHA_ENABLE() ALPHA_DO_SOMETHING

#define input_alpha_bitlen ALPHA_BITS

#define DEFINE_SWIZZLE_FUNC_PAIRS(input_format_name, input_total_bitlen, input_red_bitlen, input_green_bitlen, input_blue_bitlen, do_with_alpha, unswizzle_colours, swizzle_colours) \
	DEFINE_UNSWIZZLE_FUNC(le_##input_format_name##_to_rgba8, u##input_total_bitlen, u32, LE##input_total_bitlen, u8 r; u8 g; u8 b; u8 a = 0xFF;,                              \
		unswizzle_colours                                                                                                                                                       \
		out_colour = (r << 24) | (g << 16) | (b << 8) | (a << 0);                                                                                                               \
	)                                                                                                                                                                         \
	DEFINE_SWIZZLE_FUNC(rgba8_to_le_##input_format_name, u32, u##input_total_bitlen, LE##input_total_bitlen, u8 r; u8 g; u8 b; do_with_alpha(u8 a;),                          \
		r = FIX_COLOUR_DEPTH(in_colour >> 24 & 0xFF, 8, input_red_bitlen);                                                                                                      \
		g = FIX_COLOUR_DEPTH(in_colour >> 16 & 0xFF, 8, input_green_bitlen);                                                                                                    \
		b = FIX_COLOUR_DEPTH(in_colour >> 8  & 0xFF, 8, input_blue_bitlen);                                                                                                     \
		do_with_alpha(a = FIX_COLOUR_DEPTH(in_colour >> 0 & 0xFF, 8, input_alpha_bitlen);)                                                                                      \
		swizzle_colours                                                                                                                                                         \
	)                                                                                                                                                                         \
	DEFINE_UNSWIZZLE_FUNC(le_##input_format_name##_to_be_rgba8, u##input_total_bitlen, u32, LE##input_total_bitlen, u8 r; u8 g; u8 b; u8 a = 0xFF;,                              \
		unswizzle_colours                                                                                                                                                       \
		out_colour = BE32((r << 24) | (g << 16) | (b << 8) | (a << 0));                                                                                                         \
	)                                                                                                                                                                         \
	DEFINE_SWIZZLE_FUNC(be_rgba8_to_le_##input_format_name, u32, u##input_total_bitlen, LE##input_total_bitlen, u8 r; u8 g; u8 b; do_with_alpha(u8 a;),                       \
		in_colour = BE32(in_colour);                                                                                                                                          \
		r = FIX_COLOUR_DEPTH(in_colour >> 24 & 0xFF, 8, input_red_bitlen);                                                                                                      \
		g = FIX_COLOUR_DEPTH(in_colour >> 16 & 0xFF, 8, input_green_bitlen);                                                                                                    \
		b = FIX_COLOUR_DEPTH(in_colour >> 8  & 0xFF, 8, input_blue_bitlen);                                                                                                     \
		do_with_alpha(a = FIX_COLOUR_DEPTH(in_colour >> 0 & 0xFF, 8, input_alpha_bitlen);)                                                                                      \
		swizzle_colours                                                                                                                                                         \
	)                                                                                                                                                                         \
	DEFINE_UNSWIZZLE_FUNC_EX(le_##input_format_name##_to_be_rgb8, u##input_total_bitlen, u8, LE##input_total_bitlen,                                                          \
		u8 r; u8 g; u8 b; u8 a = 0xFF;                                                                                                                                          \
		u8 *current;                                                                                                                                                            \
	,                                                                                                                                                                         \
		unswizzle_colours                                                                                                                                                       \
		current = &outp[3 * SWIZZLE_CUR_INDEX];                                                                                                                                 \
		current[0] = r;                                                                                                                                                         \
		current[1] = g;                                                                                                                                                         \
		current[2] = b;                                                                                                                                                         \
		(void) a; /* a is stripped in this format */                                                                                                                            \
	)                                                                                                                                                                         \
	DEFINE_SWIZZLE_FUNC_EX(be_rgb8_to_le_##input_format_name, u8, u##input_total_bitlen, LE##input_total_bitlen, u8 r; u8 g; u8 b; do_with_alpha(u8 a = 0xFF;) u8 *current;,  \
		current = &inp[3 * SWIZZLE_CUR_INDEX];                                                                                                                                  \
		r = FIX_COLOUR_DEPTH(current[0], 8, input_red_bitlen);                                                                                                                  \
		g = FIX_COLOUR_DEPTH(current[1], 8, input_green_bitlen);                                                                                                                \
		b = FIX_COLOUR_DEPTH(current[2], 8, input_blue_bitlen);                                                                                                                 \
		/* we don't support alpha in this format, hardcode to 0xFF */                                                                                                           \
		swizzle_colours                                                                                                                                                         \
	)

DEFINE_SWIZZLE_FUNC_PAIRS(rgb565, /* total bits: */ 16, /* red bits: */ 5, /* green bits: */ 6, /* blue bits: */ 5, /* alpha component: */ ALPHA_DISABLE()
	, /* unswizzle colours: */ r = FIX_COLOUR_DEPTH(in_colour >> 11 & 0x1F, 5, 8);
	                           g = FIX_COLOUR_DEPTH(in_colour >> 5  & 0x3F, 6, 8);
	                           b = FIX_COLOUR_DEPTH(in_colour >> 0  & 0x1F, 5, 8);
 	, /* swizzle colours:   */ out_colour = (r << 11) | (g << 5) | (b << 0);
)

#define ALPHA_BITS 1
DEFINE_SWIZZLE_FUNC_PAIRS(rgb5551, /* total bits: */ 16, /* red bits: */ 5, /* green bits: */ 6, /* blue bits: */ 5, /* alpha component: */ ALPHA_ENABLE()
	, /* unswizzle colours: */ r = FIX_COLOUR_DEPTH(in_colour >> 11 & 0x1F, 5, 8);
	                           g = FIX_COLOUR_DEPTH(in_colour >> 6  & 0x3F, 6, 8);
	                           b = FIX_COLOUR_DEPTH(in_colour >> 1  & 0x1F, 5, 8);
	                           a = FIX_COLOUR_DEPTH(in_colour >> 0  & 0x01, 1, 8);
 	, /* swizzle colours:   */ out_colour = (r << 11) | (g << 6) | (b << 1) | (a << 0);
)
#undef ALPHA_BITS