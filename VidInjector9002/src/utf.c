
#include "./internal.h"
#include "nnc/utf.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// https://en.wikipedia.org/wiki/UTF-8
// https://en.wikipedia.org/wiki/UTF-16
//  i hate utf

static void write_utf8(u8 *out, size_t outlen, size_t *outptr, u32 cp)
{
	if(cp < 0x80)
	{
		size_t n = *outptr + 1;
		if(n < outlen)
			out[*outptr] = cp;
		*outptr = n;
	}
	else if(cp < 0x800)
	{
		size_t n = *outptr + 2;
		if(n < outlen)
		{
			out[*outptr + 0] = (cp >> 6)   | 0xC0;
			out[*outptr + 1] = (cp & 0x3F) | 0x80;
		}
		*outptr = n;
	}
	else if(cp < 0x10000)
	{
		size_t n = *outptr + 3;
		if(n < outlen)
		{
			out[*outptr + 0] = (cp >> 12)         | 0xE0;
			out[*outptr + 1] = ((cp >> 6) & 0x3F) | 0x80;
			out[*outptr + 2] = (cp & 0x3F)        | 0x80;
		}
		*outptr = n;
	}
	else if(cp < 0x110000)
	{
		size_t n = *outptr + 4;
		if(n < outlen)
		{
			out[*outptr + 0] = (cp >> 18)          | 0xF0;
			out[*outptr + 1] = ((cp >> 12) & 0x3F) | 0x80;
			out[*outptr + 2] = ((cp >> 6) & 0x3F)  | 0x80;
			out[*outptr + 3] = (cp & 0x3F)         | 0x80;
		}
		*outptr = n;
	}
	else { } /* invalid codepoint */
}

static void write_utf16(u16 *out, size_t outlen, size_t *outptr, u32 cp)
{
	/* contains invalid codepoints as well, we'll just ignore them */
	if(cp < 0x10000)
	{
		size_t n = *outptr + 1;
		if(n < outlen)
			out[*outptr] = cp;
		*outptr = n;
	}
	else if(cp < 0x11000)
	{
		cp &= ~0x10000;
		size_t n = *outptr + 2;
		if(n < outlen)
		{
			out[*outptr + 0] = (cp >> 10)   | 0xD800;
			out[*outptr + 1] = (cp & 0x3FF) | 0xDC00;
		}
		*outptr = n;
	}
	else { } /* invalid codepoint */
}

/* should there be a BE version of this? */
size_t nnc_utf16_to_utf8(u8 *out, size_t outlen, const u16 *in, size_t inlen)
{
	size_t outptr = 0;
	for(size_t i = 0; i < inlen; ++i)
	{
		u16 p1 = LE16(in[i]);
		if(p1 == '\0')
			break; /* finished */
		else if(p1 < 0xD800 || p1 > 0xE000)
			write_utf8(out, outlen, &outptr, p1);
		/* surrogate pair */
		else
		{
			u16 p2 = LE16(in[i + 1]);
			u16 w1 = p1 & 0x3FF;
			u16 w2 = p2 & 0x3FF;
			u32 cp = 0x10000 | (w1 << 10) | w2;
			write_utf8(out, outlen, &outptr, cp);
			++i; /* since we moved ahead 2 for the pair */
		}
	}
	return outptr;
}

size_t nnc_utf8_to_utf16(u16 *out, size_t outlen, const u8 *in, size_t inlen)
{
	size_t outptr = 0;
	for(size_t i = 0; i < inlen; ++i)
	{
		u8 p1 = in[i];
		if(p1 == '\0')
			break; /* finished */
		else if(p1 < 0x80)
		{
			write_utf16(out, outlen, &outptr, in[i]);
			continue;
		}
#define INCCHK(n) if(!((i + n) < inlen)) break
		INCCHK(1);
		if(p1 < 0xE0)
		{
			u32 cp = ((p1 & 0x1F) << 6)
			       | (in[i + 1] & 0x3F);
			write_utf16(out, outlen, &outptr, cp);
			i += 1;
			continue;
		}
		INCCHK(2);
		if(p1 < 0xF0)
		{
			u32 cp = ((p1 & 0xF) << 12)
			       | ((in[i + 1] & 0x3F) << 6)
			       | (in[i + 2] & 0x3F);
			write_utf16(out, outlen, &outptr, cp);
			i += 2;
			continue;
		}
		INCCHK(3);
		if(p1 < 0xF5)
		{
			u32 cp = ((p1 & 0x7) << 18)
			       | ((in[i + 1] & 0x3F) << 12)
			       | ((in[i + 2] & 0x3F) << 6)
			       | (in[i + 3] & 0x3F);
			write_utf16(out, outlen, &outptr, cp);
			i += 3;
			continue;
		}
#undef INCCHK
		/* invalid... */
	}
	return outptr;
}

nnc_result nnc_cbuf_init(nnc_utf_conversion_buffer *buf, size_t initial_length)
{
	if(!initial_length) initial_length = 128;
	buf->buffer.voidp = malloc(initial_length);
	buf->buflen = initial_length;
	buf->converted_length = 0;
	return buf->buffer.voidp ? NNC_R_OK : NNC_R_NOMEM;
}

void nnc_cbuf_free(nnc_utf_conversion_buffer *buf)
{
	free(buf->buffer.voidp);
}

static int nnc_cbuf_perform_alloc(nnc_utf_conversion_buffer *buf, size_t minlen)
{
	minlen += 4; /* for the null termination/alignment */
	minlen = ALIGN(minlen, 4);
	if(buf->buflen < minlen)
	{
		void *nbuf = realloc(buf->buffer.voidp, minlen);
		if(!nbuf) return 0;
		buf->buffer.voidp = nbuf;
		buf->buflen = minlen;
	}
	return 1;
}

u16 *nnc_cbuf_utf8_to_utf16(nnc_utf_conversion_buffer *buf, const u8 *utf8, size_t len)
{
	if(!nnc_cbuf_perform_alloc(buf, len * 4)) return NULL;
	size_t ret = nnc_utf8_to_utf16(buf->buffer.utf16, buf->buflen, utf8, len);
	/* -2 for the null termination, although this should never happen */
	if(ret * sizeof(u16) > buf->buflen - 2) return NULL;
	memset(&buf->buffer.utf16[ret], 0x00, 4);
	buf->converted_length = ret;
	return buf->buffer.utf16;
}

u8 *nnc_cbuf_utf16_to_utf8(nnc_utf_conversion_buffer *buf, const u16 *utf16, size_t len)
{
	if(!nnc_cbuf_perform_alloc(buf, len * 4)) return NULL;
	size_t ret = nnc_utf16_to_utf8(buf->buffer.utf8, buf->buflen, utf16, len);
	/* -1 for the null termination, although this should never happen */
	if(ret > buf->buflen - 1) return NULL;
	memset(&buf->buffer.utf8[ret], 0x00, 4);
	buf->converted_length = ret;
	return buf->buffer.utf8;
}