
#include "nnc/ticket.h"
#include "nnc/cia.h"
#include "nnc/tmd.h"
#include <string.h>
#include <stdlib.h>
#include "./internal.h"

#define CALIGN(n) ALIGN(n, 64)
#define HDRSIZE_AL CALIGN(HDRSIZE)
#define HDRSIZE 0x2020

#define BLOCK_SZ 0x10000


nnc_result nnc_read_cia_header(nnc_rstream *rs, nnc_cia_header *cia)
{
	nnc_u8 header[0x2020];
	result ret;

	TRY(read_at_exact(rs, 0, header, sizeof(header)));
	/* 0x00 */ if(LE32P(&header[0x00]) != HDRSIZE) return NNC_R_CORRUPT;
	/* 0x04 */ cia->type = LE16P(&header[0x04]);
	/* 0x06 */ cia->version = LE16P(&header[0x06]);
	/* 0x08 */ cia->cert_chain_size = LE32P(&header[0x08]);
	/* 0x0C */ cia->ticket_size = LE32P(&header[0x0C]);
	/* 0x10 */ cia->tmd_size = LE32P(&header[0x10]);
	/* 0x14 */ cia->meta_size = LE32P(&header[0x14]);
	/* 0x18 */ cia->content_size = LE64P(&header[0x18]);
	/* 0x20 */ memcpy(cia->content_index, &header[0x20], sizeof(cia->content_index));
	return NNC_R_OK;
}

void nnc_cia_open_certchain(nnc_cia_header *cia, nnc_rstream *rs, nnc_subview *sv)
{
	nnc_u32 offset = HDRSIZE_AL;
	nnc_subview_open(sv, rs, offset, cia->cert_chain_size);
}

void nnc_cia_open_ticket(nnc_cia_header *cia, nnc_rstream *rs, nnc_subview *sv)
{
	nnc_u32 offset = HDRSIZE_AL + CALIGN(cia->cert_chain_size);
	nnc_subview_open(sv, rs, offset, cia->ticket_size);
}

void nnc_cia_open_tmd(nnc_cia_header *cia, nnc_rstream *rs, nnc_subview *sv)
{
	nnc_u32 offset = HDRSIZE_AL + CALIGN(cia->cert_chain_size) + CALIGN(cia->ticket_size);
	nnc_subview_open(sv, rs, offset, cia->tmd_size);
}

nnc_result nnc_cia_open_meta(nnc_cia_header *cia, nnc_rstream *rs, nnc_subview *sv)
{
	if(cia->meta_size == 0) return NNC_R_NOT_FOUND;
	nnc_u32 offset = HDRSIZE_AL + CALIGN(cia->cert_chain_size) + CALIGN(cia->ticket_size) + CALIGN(cia->tmd_size) + CALIGN(cia->content_size);
	nnc_subview_open(sv, rs, offset, cia->meta_size);
	return NNC_R_OK;
}

void nnc_cia_get_iv(nnc_u8 iv[0x10], u16 index)
{
	memset(&iv[2], 0, 0x10 - 2);
	u16 *iv16 = (u16 *) iv;
	iv16[0] = BE16(index);
}

nnc_result nnc_cia_make_reader(nnc_cia_header *cia, nnc_rstream *rs,
	nnc_keyset *ks, nnc_cia_content_reader *reader)
{
	result ret;
	reader->cia = cia;
	reader->rs = rs;

	/* first we get the chunk records */
	nnc_tmd_header tmdhdr;
	nnc_subview sv;
	nnc_cia_open_tmd(cia, rs, &sv);
	TRY(nnc_read_tmd_header(NNC_RSP(&sv), &tmdhdr));
	reader->content_count = tmdhdr.content_count;
	reader->chunks = malloc(sizeof(nnc_chunk_record) * reader->content_count);
	if(!reader->chunks) return NNC_R_NOMEM;
	TRYLBL(nnc_read_tmd_chunk_records(NNC_RSP(&sv), &tmdhdr, reader->chunks), free_chunks);

	/* now we derive title key */
	nnc_ticket tik;
	nnc_cia_open_ticket(cia, rs, &sv);
	TRYLBL(nnc_read_ticket(NNC_RSP(&sv), &tik), free_chunks);
	/* obviously corrupt */
	if(tik.title_id != tmdhdr.title_id) { ret = NNC_R_CORRUPT; goto free_chunks; }
	TRYLBL(nnc_decrypt_tkey(&tik, ks, reader->key), free_chunks);

	return NNC_R_OK;
free_chunks:
	reader->chunks = NULL;
	free(reader->chunks);
	return ret;
}

static nnc_result open_content(nnc_cia_content_reader *reader, nnc_chunk_record *chunk, nnc_u32 offset,
	nnc_cia_content_stream *content)
{
	if(chunk->flags & NNC_CHUNKF_ENCRYPTED)
	{
		u16 iv[8] = { BE16(chunk->index), 0, 0, 0, 0, 0, 0, 0 };
		nnc_subview_open(&content->u.enc.sv, reader->rs, offset, chunk->size);
		return nnc_aes_cbc_open(&content->u.enc.crypt, NNC_RSP(&content->u.enc.sv),
			reader->key, (u8 *) iv);
	}
	nnc_subview_open(&content->u.dec.sv, reader->rs, offset, chunk->size);
	return NNC_R_OK;
}

nnc_result nnc_cia_open_content(nnc_cia_content_reader *reader, nnc_u16 index,
	nnc_cia_content_stream *content, nnc_chunk_record **chunk_output)
{
	nnc_u16 i;
	nnc_u32 offset = HDRSIZE_AL + CALIGN(reader->cia->cert_chain_size) + CALIGN(reader->cia->ticket_size) + CALIGN(reader->cia->tmd_size);
	for(i = 0; i < reader->content_count; ++i)
	{
		if(!NNC_CINDEX_HAS(reader->cia->content_index, reader->chunks[i].index))
			continue; /* why is this even a thing ninty */
		if(reader->chunks[i].index == index) break;
		offset += CALIGN(reader->chunks[i].size);
	}
	if(i == reader->content_count)
		return NNC_R_NOT_FOUND;
	nnc_chunk_record *chunk = &reader->chunks[i];
	if(chunk_output) *chunk_output = chunk;

	return open_content(reader, chunk, offset, content);
}

void nnc_cia_free_reader(nnc_cia_content_reader *reader)
{
	free(reader->chunks);
}

nnc_result nnc_write_cia(
	nnc_u8 wflags,
	nnc_certchain_or_stream certchain,
	nnc_ticket_or_stream ticket,
	nnc_tmd_or_stream tmd,
	nnc_u16 amount_contents,
	nnc_cia_writable_ncch *contents,
	nnc_wstream *ws)
{
	if(!ws->funcs->seek || !ws->funcs->subreadstream)
		return NNC_R_INVAL;

#define DO_VALIDATE_FOR(ptr, opt1, opt2) if( !ptr || (wflags & (opt1 | opt2)) == 0 || (wflags & (opt1 | opt2)) == (opt1 | opt2)) return NNC_R_INVAL
	DO_VALIDATE_FOR(certchain, NNC_CIA_WF_CERTCHAIN_BUILD, NNC_CIA_WF_CERTCHAIN_STREAM);
	DO_VALIDATE_FOR(ticket, NNC_CIA_WF_TICKET_BUILD, NNC_CIA_WF_TICKET_STREAM);
	DO_VALIDATE_FOR(tmd, NNC_CIA_WF_TMD_BUILD, NNC_CIA_WF_TMD_STREAM);
#undef DO_VALIDATE_FOR

	result ret;
	nnc_u32 certchain_size, ticket_size, tmd_size, hdr_off, tmd_off, off, size, chunkcount = 0, startpos, endpos;
	nnc_chunk_record *chunk_records = NULL;
	nnc_wstream *content_writer;
	nnc_hasher_writer hasher = { NULL };
	nnc_u64 content_size = 0;
	u8 header[0x2020];

	u8 *content_index = &header[0x20];
	memset(content_index, 0x00, 0x2000);

	/* reserve some space for the header before we start doing actual work */
	hdr_off = NNC_WS_PCALL0(ws, tell);
	TRY(nnc_write_padding(ws, HDRSIZE_AL));

#define PERFORM_ALIGNMENT(written_size) nnc_write_padding(ws, CALIGN(written_size) - written_size)

	/* certchain */
	if(wflags & NNC_CIA_WF_CERTCHAIN_BUILD)
		return NNC_R_UNSUPPORTED; /* certchain building is unsupported for now */
	else /* wflags & NNC_CIA_WF_CERTCHAIN_STREAM */
		TRY(nnc_copy((nnc_rstream *) certchain, ws, &certchain_size));
	TRY(PERFORM_ALIGNMENT(certchain_size));

	/* ticket */
	if(wflags & NNC_CIA_WF_TICKET_BUILD)
	{
		off = NNC_WS_PCALL0(ws, tell);
		TRY(nnc_write_ticket((nnc_ticket *) ticket, ws));
		ticket_size = NNC_WS_PCALL0(ws, tell) - off;
	}
	else
		TRY(nnc_copy((nnc_rstream *) ticket, ws, &ticket_size));
	TRY(PERFORM_ALIGNMENT(ticket_size));

	if(wflags & NNC_CIA_WF_TMD_BUILD)
	{
		tmd_off = NNC_WS_PCALL0(ws, tell);
		/* If this TMD is built, there is no way to sign the signature so it should always be zero'd out */
		tmd_size = nnc_calculate_tmd_size(amount_contents, NNC_SIG_NONE + NNC_SIG_RSA_2048_SHA256);
		TRY(nnc_write_padding(ws, CALIGN(tmd_size)));
		chunk_records = malloc(sizeof(nnc_chunk_record) * amount_contents);
		if(!chunk_records) return NNC_R_NOMEM;
		TRYLBL(nnc_open_hasher_writer(&hasher, ws, 0), out);
		content_writer = NNC_WSP(&hasher);
	}
	else
	{
		TRY(nnc_copy((nnc_rstream *) tmd, ws, &tmd_size));
		content_writer = ws;
	}

	/* Now we can start writing the contents */
	for(u32 i = 0; i < amount_contents; ++i)
	{
		switch(contents[i].type)
		{
		case NNC_CIA_NCCHBUILD_NONE:
			continue; /* nothing to be done */
		case NNC_CIA_NCCHBUILD_STREAM:
			TRYLBL(nnc_copy((nnc_rstream *) contents[i].ncch, content_writer, &size), out);
			break;
		case NNC_CIA_NCCHBUILD_BUILD:
			off = NNC_WS_PCALL0(ws, tell);
			/* this requires seeking... which our content_writer may not have since it may be a hasher */
			if(wflags & NNC_CIA_WF_TMD_BUILD)
			{
				startpos = NNC_WS_PCALL0(ws, tell);
				TRYLBL(nnc_write_ncch_from_buildable((nnc_buildable_ncch *) contents[i].ncch, ws), out);
				endpos = NNC_WS_PCALL0(ws, tell);
			}
			else
				TRYLBL(nnc_write_ncch_from_buildable((nnc_buildable_ncch *) contents[i].ncch, content_writer), out);
			size = NNC_WS_PCALL0(ws, tell) - off;
			break;
		}
		/* we don't need to hash the alignment bytes, so we can just use `ws` always */
		TRYLBL(PERFORM_ALIGNMENT(size), out);

		if(wflags & NNC_CIA_WF_TMD_BUILD)
		{
			/* now we write the chunk record for this content */
			chunk_records[chunkcount].id = i;
			chunk_records[chunkcount].index = i;
			chunk_records[chunkcount].flags = 0; /* we don't set any flags */
			chunk_records[chunkcount].size = size;
			if(contents[i].type == NNC_CIA_NCCHBUILD_BUILD)
			{
				/* we need to read back and hash */
				nnc_subview sv;
				TRYLBL(NNC_WS_PCALL(ws, subreadstream, &sv, startpos, endpos - startpos), out);
				ret = nnc_crypto_sha256_stream(NNC_RSP(&sv), chunk_records[chunkcount].hash);
				NNC_RS_CALL0(sv, close);
				if(ret != NNC_R_OK)
					goto out;
			}
			else
				nnc_hasher_writer_digest_reset(&hasher, chunk_records[chunkcount].hash);
			++chunkcount;
		}

		content_index[i / 8] |= 1 << (7 - (i % 8));
		content_size += CALIGN(size);
	}

	/* now we can write the TMD, if required */
	if(wflags & NNC_CIA_WF_TMD_BUILD)
	{
		NNC_WS_CALL0(hasher, close);
		hasher.funcs = NULL;

		off = NNC_WS_PCALL0(ws, tell);
		TRYLBL(NNC_WS_PCALL(ws, seek, tmd_off), out);
		nnc_tmd_header *hdr = (nnc_tmd_header *) tmd;
		enum nnc_sigtype old_type = hdr->sig.type;
		char old_issuer[0x40]; strncpy(old_issuer, hdr->sig.issuer, 0x40);
		hdr->sig.type = NNC_SIG_NONE + NNC_SIG_RSA_2048_SHA256; /* just ensure that value is set, and restore it later */
		strncpy(hdr->sig.issuer, "Root-CA00000003-CP0000000b", 0x40);
		TRYLBL(nnc_write_tmd(hdr, chunk_records, chunkcount, ws), out);
		strncpy(hdr->sig.issuer, old_issuer, 0x40);
		hdr->sig.type = old_type;
		free(chunk_records);
		chunk_records = NULL;
		/* alignment not required as it was already done before */
		TRY(NNC_WS_PCALL(ws, seek, off));
	}

	/* We would (optionally) write the meta here, but we don't have support for it yet */

#undef PERFORM_ALIGNMENT

	/* We can finally go back to writing the header */
	U32P(&header[0x00]) = LE32(HDRSIZE);
	U16P(&header[0x04]) = 0; /* type */
	U16P(&header[0x06]) = 0; /* version */
	U32P(&header[0x08]) = LE32(certchain_size);
	U32P(&header[0x0C]) = LE32(ticket_size);
	U32P(&header[0x10]) = LE32(tmd_size);
	U32P(&header[0x14]) = 0; /* no support for the meta section (yet) */
	U64P(&header[0x18]) = LE64(content_size);
	/* content index is already written... */

	off = NNC_WS_PCALL0(ws, tell);
	TRY(NNC_WS_PCALL(ws, seek, hdr_off));
	TRY(NNC_WS_PCALL(ws, write, header, sizeof(header)));
	/* bellow call not needed since we already padded it before */
	/* TRY(nnc_write_padding(ws, HDRSIZE_AL - HDRSIZE)); */
	TRY(NNC_WS_PCALL(ws, seek, off));

out:
	if(hasher.funcs) NNC_WS_CALL0(hasher, close);
	free(chunk_records);
	return ret;
}


