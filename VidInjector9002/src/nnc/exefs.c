
#include "nnc/exefs.h"
#include <string.h>
#include <stdlib.h>
#include "./internal.h"

#define NNC_EXEFS_ALIGNMENT 0x200

bool nnc_exefs_file_in_use(nnc_exefs_file_header *fh)
{
	return fh->name[0] != '\0';
}

result nnc_read_exefs_header(nnc_rstream *rs, nnc_exefs_file_header *headers, nnc_u8 *size)
{
	result ret;
	u8 i = 0;

	u8 data[0x200];
	u8 *cur = data;
	TRY(read_at_exact(rs, 0x0, data, sizeof(data)));
	for(; i < NNC_EXEFS_MAX_FILES && cur[0] != '\0'; ++i, cur = &data[0x10 * i])
	{
		/* 0x00 */ memcpy(headers[i].name, cur, 8);
		/* 0x00 */ headers[i].name[8] = '\0';
		/* 0x08 */ headers[i].offset = LE32P(&cur[0x8]);
		/* 0x0C */ headers[i].size = LE32P(&cur[0xC]);
	}
	for(int j = i; j != NNC_EXEFS_MAX_FILES; ++j)
		headers[j].name[0] = '\0';

	for(u8 j = 0; j < i; ++j)
	{
		memcpy(headers[j].hash,
			&data[0xC0 + ((NNC_EXEFS_MAX_FILES - j - 1) * sizeof(nnc_sha256_hash))],
			sizeof(nnc_sha256_hash));
	}

	if(size) *size = i;

	return NNC_R_OK;
}

i8 nnc_find_exefs_file_index(const char *name, nnc_exefs_file_header *headers)
{
	u32 len = strlen(name);
	if(len > 8) return -1;
	/* so that the NULL-terminator is compared as well;
	 *  in the file header our file names are guaranteed to end with a null terminator,
	 *  also comparing this prevents issues like the name "i" matching the file name "icon",
	 *  when it should in reality just not match anything at all (assuming the exefs does
	 *  not contain the file "i") */
	++len;
	for(u8 i = 0; i < NNC_EXEFS_MAX_FILES && nnc_exefs_file_in_use(&headers[i]); ++i)
	{
		if(memcmp(name, headers[i].name, len) == 0)
			return i;
	}
	return -1;
}

void nnc_exefs_subview(nnc_rstream *rs, nnc_subview *sv, nnc_exefs_file_header *header)
{
	nnc_subview_open(sv, rs, NNC_EXEFS_HEADER_SIZE + header->offset, header->size);
}

result nnc_write_exefs(nnc_vfs *vfs, nnc_wstream *ws)
{
	u8 header[0x200];
	u8 *block;
	unsigned i, namelen;
	size_t cumulative_offset = 0, size;
	nnc_vfs_file_node *node;
	nnc_vfs_stream *source;
	result ret;
	nnc_sha256_hash hash;
	u32 copied;

	if(vfs->totalfiles > NNC_EXEFS_MAX_FILES) return NNC_R_TOO_LARGE;
	if(vfs->totaldirs != 1)                   return NNC_R_NOT_A_FILE;

	memset(header, 0x00, sizeof(header));

	for(i = 0; i < vfs->root_directory.filecount; ++i)
	{
		node = &vfs->root_directory.file_children[i];
		namelen = strlen(node->vname);
		if(namelen > 8) return NNC_R_TOO_LARGE;

		/* we may as well use the stream here instead of nnc_vfs_node_size() since we need to hash as well */
		TRY(nnc_vfs_open_node(node, &source));
		size = NNC_RS_PCALL0(source, size);
		ret = nnc_crypto_sha256_stream(source, hash);
		nnc_vfs_close_node(source);
		if(ret != NNC_R_OK)
			return ret;

		block = &header[0x10 * i];
		/* 0x00 */ strncpy((char *) block, node->vname, 8); /* strncpy will pad the rest of the bytes with \0 */
		/* 0x08 */ U32P(&block[0x08]) = LE32(cumulative_offset);
		/* 0x0C */ U32P(&block[0x0C]) = LE32(size);
		block = &header[0xC0 + sizeof(nnc_sha256_hash) * (NNC_EXEFS_MAX_FILES - i - 1)];
		/* 0x00 */ memcpy(block, hash, sizeof(hash));
		cumulative_offset += ALIGN(size, NNC_EXEFS_ALIGNMENT);
	}

	TRY(NNC_WS_PCALL(ws, write, header, sizeof(header)));

	for(i = 0; i < vfs->root_directory.filecount; ++i)
	{
		TRY(nnc_vfs_open_node(&vfs->root_directory.file_children[i], &source));
		ret = nnc_copy(source, ws, &copied);
		nnc_vfs_close_node(source);
		if(ret != NNC_R_OK)
			return ret;
		TRY(nnc_write_padding(ws, ALIGN(copied, NNC_EXEFS_ALIGNMENT) - copied));
	}

	return NNC_R_OK;
}

