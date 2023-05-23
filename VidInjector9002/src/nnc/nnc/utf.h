/** \file  utf.h
 *  \brief UTF conversion.
 */
#ifndef inc_nnc_utf_h
#define inc_nnc_utf_h

#include "base.h"
NNC_BEGIN

typedef struct nnc_utf_conversion_buffer {
	size_t converted_length, buflen;
	union {
		nnc_u8  *utf8;
		nnc_u16 *utf16;
		void    *voidp;
	} buffer;
} nnc_utf_conversion_buffer;

/** \brief         Converts a UTF16 string to a UTF8 one.
 *  \note          The NULL terminator is never placed.
 *  \param out     Output UTF8 string. Something like (\p inlen * 4) will always fit the entire output.
 *  \param outlen  Length of the `out` array.
 *  \param in      Input UTF16 string.
 *  \param inlen   Length of the `in` array.
 *  \returns       Total size of the converted string, even if buffer was too small.
 */
size_t nnc_utf16_to_utf8(nnc_u8 *out, size_t outlen, const nnc_u16 *in, size_t inlen);

/** \brief         Converts a UTF8 string to a UTF16 one.
 *  \note          The NULL terminator is never placed.
 *  \param out     Output UTF16 string. Something like (\p inlen * 4) will always fit the entire output.
 *  \param outlen  Length of the `out` array.
 *  \param in      Input UTF8 string.
 *  \param inlen   Length of the `in` array.
 *  \returns       Total size of the converted string, even if buffer was too small.
 */
size_t nnc_utf8_to_utf16(nnc_u16 *out, size_t outlen, const nnc_u8 *in, size_t inlen);

/** \brief                 Initializes a UTF conversion buffer.
 *  \param buf             Output buffer.
 *  \param initial_length  Initial length of the buffer, may be 0 to pick the default.
 *  \note                  The output buffer must be freed by #nnc_cbuf_free.
 */
nnc_result nnc_cbuf_init(nnc_utf_conversion_buffer *buf, size_t initial_length);

/** \brief      Frees a UTF conversion buffer.
 *  \param buf  The buffer to free.
 */
void nnc_cbuf_free(nnc_utf_conversion_buffer *buf);

/** \brief       Convert UTF8 to UTF16.
 *  \param buf   Buffer to operate in.
 *  \param utf8  Input UTF8 string.
 *  \param len   Size of the `utf8` array.
 *  \note        The returned pointer will be changed by subsequent calls to #nnc_cbuf_utf8_to_utf16 and #nnc_cbuf_utf16_to_utf8.
 *  \returns     NULL if the allocation failed.
 */
nnc_u16 *nnc_cbuf_utf8_to_utf16(nnc_utf_conversion_buffer *buf, const nnc_u8 *utf8, size_t len);

/** \brief        Convert UTF16 to UTF8.
 *  \param buf    Buffer to operate in.
 *  \param utf16  Input UTF16 string.
 *  \param len    Size of the `utf16` array.
 *  \note         The returned pointer will be changed by subsequent calls to #nnc_cbuf_utf8_to_utf16 and #nnc_cbuf_utf16_to_utf8.
 *  \returns      NULL if the allocation failed.
 */
nnc_u8 *nnc_cbuf_utf16_to_utf8(nnc_utf_conversion_buffer *buf, const nnc_u16 *utf16, size_t len);

NNC_END
#endif

