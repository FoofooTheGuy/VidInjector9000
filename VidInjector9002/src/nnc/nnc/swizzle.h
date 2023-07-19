/** \file  swizzle.h
 *  \brief Functions relating to image swizzling.
 */
#ifndef inc_nnc_swizzle_h
#define inc_nnc_swizzle_h

#include "base.h"
NNC_BEGIN

/** \brief       Unswizzles a z-order curve rgb565 (little endian) image to an rgba8 (native endian) image.
 *  \param inp   Input array, should be at least of size `dim*dim*sizeof(nnc_u16)`.
 *  \param outp  Output array, should be at least of size `dim*dim*sizeof(nnc_u32)`.
 *  \param x     Image width, must be aligned by 8.
 *  \param y     Image height, must be aligned by 8.
 */
	void nnc_unswizzle_zorder_le_rgb565_to_rgba8(nnc_u16* inp, nnc_u32* outp, nnc_u16 x, nnc_u16 y);

/** \brief       Swizzles an rgba8 (native endian) image to a z-order curve rgb565 (little endian) image.
 *  \param inp   Input array, should be at least of size `dim*dim*sizeof(nnc_u32)`.
 *  \param outp  Output array, should be at least of size `dim*dim*sizeof(nnc_u16)`.
 *  \param x     Image width, must be aligned by 8.
 *  \param y     Image height, must be aligned by 8.
 */
void nnc_swizzle_zorder_rgba8_to_le_rgb565(nnc_u32* inp, nnc_u16* outp, nnc_u16 x, nnc_u16 y);

/** \brief       Unswizzles a z-order curve rgb565 (little endian) image to an rgba8 (big endian) image.
 *  \param inp   Input array, should be at least of size `dim*dim*sizeof(nnc_u16)`.
 *  \param outp  Output array, should be at least of size `dim*dim*sizeof(nnc_u32)`.
 *  \param x     Image width, must be aligned by 8.
 *  \param y     Image height, must be aligned by 8.
 */
void nnc_unswizzle_zorder_le_rgb565_to_be_rgba8(nnc_u16* inp, nnc_u32* outp, nnc_u16 x, nnc_u16 y);

/** \brief       Swizzles an rgba8 (big endian) image to a z-order curve rgb565 (little endian) image.
 *  \param inp   Input array, should be at least of size `dim*dim*sizeof(nnc_u32)`.
 *  \param outp  Output array, should be at least of size `dim*dim*sizeof(nnc_u16)`.
 *  \param x     Image width, must be aligned by 8.
 *  \param y     Image height, must be aligned by 8.
 */
void nnc_swizzle_zorder_be_rgba8_to_le_rgb565(nnc_u32* inp, nnc_u16* outp, nnc_u16 x, nnc_u16 y);

/** \brief       Unswizzles a z-order curver rgb565 (little endian) image to an rgb8 (big endian) image.
 *  \param inp   Input array, should be at least of size `dim*dim*sizeof(nnc_u16)`.
 *  \param outp  Output array, should be at least of size `dim*dim*3*sizeof(nnc_u8)`.
 *  \param x     Image width, must be aligned by 8.
 *  \param y     Image height, must be aligned by 8.
 *  \note        The output image data can be represented by the following struct:
 *               \code
 *               struct rgb8 {
 *                 u8 r;
 *                 u8 g;
 *                 u8 b;
 *               } __attribute__((packed));
 *               \endcode
 *  \warning     Due to alignment issues this format is not recommended.
 */
void nnc_unswizzle_zorder_le_rgb565_to_be_rgb8(nnc_u16* inp, nnc_u8* outp, nnc_u16 x, nnc_u16 y);

/** \brief       Swizzles an rgb8 (big endian) image to a z-order curve rgb565 (little endian) image.
 *  \param inp   Input array, should be at least of size `dim*dim*sizeof(nnc_u16)`.
 *  \param outp  Output array, should be at least of size `dim*dim*3*sizeof(nnc_u8)`.
 *  \param x     Image width, must be aligned by 8.
 *  \param y     Image height, must be aligned by 8.
 *  \note        The input image data can be represented by the following struct:
 *               \code
 *               struct rgb8 {
 *                 u8 r;
 *                 u8 g;
 *                 u8 b;
 *               } __attribute__((packed));
 *               \endcode
 *  \warning     Due to alignment issues this format is not recommended.
 */
void nnc_swizzle_zorder_be_rgb8_to_le_rgb565(nnc_u8* inp, nnc_u16* outp, nnc_u16 x, nnc_u16 y);
/** \brief       Unswizzles a z-order curve rgb5551 (little endian) image to an rgba8 (native endian) image.
 *  \param inp   Input array, should be at least of size `dim*dim*sizeof(nnc_u16)`.
 *  \param outp  Output array, should be at least of size `dim*dim*sizeof(nnc_u32)`.
 *  \param x     Image width, must be aligned by 8.
 *  \param y     Image height, must be aligned by 8.
 */
void nnc_unswizzle_zorder_le_rgb5551_to_rgba8(nnc_u16* inp, nnc_u32* outp, nnc_u16 x, nnc_u16 y);

/** \brief       Swizzles an rgba8 (native endian) image to a z-order curve rgb5551 (little endian) image.
 *  \param inp   Input array, should be at least of size `dim*dim*sizeof(nnc_u32)`.
 *  \param outp  Output array, should be at least of size `dim*dim*sizeof(nnc_u16)`.
 *  \param x     Image width, must be aligned by 8.
 *  \param y     Image height, must be aligned by 8.
 */
void nnc_swizzle_zorder_rgba8_to_le_rgb5551(nnc_u32* inp, nnc_u16* outp, nnc_u16 x, nnc_u16 y);

/** \brief       Unswizzles a z-order curve rgb5551 (little endian) image to an rgba8 (big endian) image.
 *  \param inp   Input array, should be at least of size `dim*dim*sizeof(nnc_u16)`.
 *  \param outp  Output array, should be at least of size `dim*dim*sizeof(nnc_u32)`.
 *  \param x     Image width, must be aligned by 8.
 *  \param y     Image height, must be aligned by 8.
 */
void nnc_unswizzle_zorder_le_rgb5551_to_be_rgba8(nnc_u16* inp, nnc_u32* outp, nnc_u16 x, nnc_u16 y);

/** \brief       Swizzles an rgba8 (big endian) image to a z-order curve rgb5551 (little endian) image.
 *  \param inp   Input array, should be at least of size `dim*dim*sizeof(nnc_u32)`.
 *  \param outp  Output array, should be at least of size `dim*dim*sizeof(nnc_u16)`.
 *  \param x     Image width, must be aligned by 8.
 *  \param y     Image height, must be aligned by 8.
 */
void nnc_swizzle_zorder_be_rgba8_to_le_rgb5551(nnc_u32* inp, nnc_u16* outp, nnc_u16 x, nnc_u16 y);

/** \brief       Unswizzles a z-order curver rgb5551 (little endian) image to an rgb8 (big endian) image.
 *  \param inp   Input array, should be at least of size `dim*dim*sizeof(nnc_u16)`.
 *  \param outp  Output array, should be at least of size `dim*dim*3*sizeof(nnc_u8)`.
 *  \param x     Image width, must be aligned by 8.
 *  \param y     Image height, must be aligned by 8.
 *  \note        The output image data can be represented by the following struct:
 *               \code
 *               struct rgb8 {
 *                 u8 r;
 *                 u8 g;
 *                 u8 b;
 *               } __attribute__((packed));
 *               \endcode
 *  \warning     Due to alignment issues this format is not recommended.
 */
void nnc_unswizzle_zorder_le_rgb5551_to_be_rgb8(nnc_u16* inp, nnc_u8* outp, nnc_u16 x, nnc_u16 y);

/** \brief       Swizzles an rgb8 (big endian) image to a z-order curve rgb5551 (little endian) image.
 *  \param inp   Input array, should be at least of size `dim*dim*sizeof(nnc_u16)`.
 *  \param outp  Output array, should be at least of size `dim*dim*3*sizeof(nnc_u8)`.
 *  \param x     Image width, must be aligned by 8.
 *  \param y     Image height, must be aligned by 8.
 *  \note        The input image data can be represented by the following struct:
 *               \code
 *               struct rgb8 {
 *                 u8 r;
 *                 u8 g;
 *                 u8 b;
 *               } __attribute__((packed));
 *               \endcode
 *  \warning     Due to alignment issues this format is not recommended.
 */
void nnc_swizzle_zorder_be_rgb8_to_le_rgb5551(nnc_u8* inp, nnc_u16* outp, nnc_u16 x, nnc_u16 y);

NNC_END
#endif