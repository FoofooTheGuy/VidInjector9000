
#include "nnc/crypto.h"
#include "nnc/romfs.h"
#include "nnc/ivfc.h"
#include "nnc/utf.h"
#include <string.h>
#include <stdlib.h>
#include "./internal.h"

#define INVAL 0xFFFFFFFF /* aka UINT32_MAX */
#define MAX_PATH 1024    /* no good rationale for this specific limit except it looking nice */


result nnc_read_romfs_header(rstream *rs, nnc_romfs_header *romfs)
{
	/* in reality this is only 0x5C & 0x28 bytes, but we need to align reads */
	u8 ivfc_header[0x60], l3_header[0x30];
	result ret;
	TRY(read_at_exact(rs, 0, ivfc_header, sizeof(ivfc_header)));

	if(memcmp(ivfc_header, "IVFC\x00\x00\x01\x00", 8) != 0)
		return NNC_R_CORRUPT;
	u32 master_hash_size = LE32P(&ivfc_header[0x08]);
	/* u32 block_size = pow(2, LE32P(&ivfc_header[0x4C])) */
	u32 block_size = 1 << LE32P(&ivfc_header[0x4C]);
	/* for some reason the 0x4 padding after the ivfc_header is
	 * not documented on 3dbrew but it's definitely there */
	nnc_u32 l3_offset = 0x60 + master_hash_size;
	l3_offset = ALIGN(l3_offset, block_size);

	/* TODO: More (shallow) ivfc container assertions */

	TRY(read_at_exact(rs, l3_offset, l3_header, sizeof(l3_header)));
	/* header size should always be 0x28 LE */
	if(memcmp(l3_header, "\x28\x00\x00\x00", 4) != 0)
		return NNC_R_CORRUPT;

#define OFLEN_PAIR(st, of) st.offset = ((u64) LE32P(&l3_header[of])) + l3_offset; st.length = LE32P(&l3_header[of + 0x04])
	OFLEN_PAIR(romfs->dir_hash, 0x04);
	OFLEN_PAIR(romfs->dir_meta, 0x0C);
	OFLEN_PAIR(romfs->file_hash, 0x14);
	OFLEN_PAIR(romfs->file_meta, 0x1C);
#undef OFLEN_PAIR
	romfs->data_offset = ((u64) LE32P(&l3_header[0x24])) + l3_offset;

	return NNC_R_OK;
}

static u32 hash_func(const u16 *name, u32 len, u32 parent)
{
	/* ninty tm */
	u32 ret = parent ^ 123456789;
	for(u32 i = 0; i < len; ++i)
	{
		ret = (ret >> 5) | (ret << 27);
		ret ^= name[i];
	}
	return ret;
}

#define DIR_OFF_PARENT     0x00
#define DIR_OFF_SIBLING    0x04
#define DIR_OFF_DCHILDREN  0x08
#define DIR_OFF_FCHILDREN  0x0C
#define DIR_OFF_NEXTBUCKET 0x10
#define DIR_OFF_NAMELEN    0x14 /* bytes! */
#define DIR_OFF_NAME       0x18

#define DIR_PARENT(buf) LE32P(&(buf)[DIR_OFF_PARENT])
#define DIR_SIBLING(buf) LE32P(&(buf)[DIR_OFF_SIBLING])
#define DIR_DCHILDREN(buf) LE32P(&(buf)[DIR_OFF_DCHILDREN])
#define DIR_FCHILDREN(buf) LE32P(&(buf)[DIR_OFF_FCHILDREN])
#define DIR_NEXTBUCKET(buf) LE32P(&(buf)[DIR_OFF_NEXTBUCKET])
#define DIR_NAMELEN(buf) LE32P(&(buf)[DIR_OFF_NAMELEN])
#define DIR_NAME(buf) ((const u16 *) (&(buf)[DIR_OFF_NAME]))
#define DIR_META(ctx, offset) (ctx->dir_meta_data + offset)

#define FILE_OFF_PARENT     0x00
#define FILE_OFF_SIBLING    0x04
#define FILE_OFF_OFFSET     0x08
#define FILE_OFF_SIZE       0x10
#define FILE_OFF_NEXTBUCKET 0x18
#define FILE_OFF_NAMELEN    0x1C
#define FILE_OFF_NAME       0x20

#define FILE_PARENT(buf) LE32P(&(buf)[0x0])
#define FILE_SIBLING(buf) LE32P(&(buf)[0x04])
#define FILE_OFFSET(buf) LE64P(&(buf)[0x08])
#define FILE_SIZE(buf) LE64P(&(buf)[0x10])
#define FILE_NEXTBUCKET(buf) LE32P(&(buf)[0x18])
#define FILE_NAMELEN(buf) LE32P(&(buf)[0x1C])
#define FILE_NAME(buf) ((const u16 *) (&(buf)[0x20]))
#define FILE_META(ctx, offset) (ctx->file_meta_data + offset)

static u32 get_dir_single_offset(nnc_romfs_ctx *ctx, const u16 *path, u32 len, u32 parent_offset)
{
	u32 tab_len = ctx->header.dir_hash.length / sizeof(u32);
	u32 i = hash_func(path, len, parent_offset) % tab_len;

	u32 namelen, offset = ctx->dir_hash_tab[i], len2 = len * sizeof(u16);
	u8 *dir;

	do {
		if(offset == INVAL)
			break; /* bucket is unused; fail */
		dir = DIR_META(ctx, offset);
		namelen = DIR_NAMELEN(dir);
		if(namelen != len2 || memcmp(DIR_NAME(dir), path, len2) != 0)
		{
			offset = DIR_NEXTBUCKET(dir);
			continue;
		}
		return offset;
	} while(1);

	/* failed to find correct bucket */
	return INVAL;
}

static u32 get_file_single_offset(nnc_romfs_ctx *ctx, const u16 *path, u32 len, u32 parent_offset)
{
	u32 tab_len = ctx->header.file_hash.length / sizeof(u32);
	u32 i = hash_func(path, len, parent_offset) % tab_len;

	u32 namelen, offset = ctx->file_hash_tab[i], len2 = len * sizeof(u16);
	u8 *file;

	do {
		if(offset == INVAL)
			break; /* bucket is unused; fail */
		file = FILE_META(ctx, offset);
		namelen = FILE_NAMELEN(file);
		if(namelen != len2 || memcmp(FILE_NAME(file), path, len2) != 0)
		{
			offset = FILE_NEXTBUCKET(file);
			continue;
		}
		return offset;
	} while(1);

	/* failed to find correct bucket */
	return INVAL;
}

/* "/" and "\0" encodings are the same for both utf16 and utf8 */
static const u16 *find_next_char(const u16 *str, u32 len, u16 mychar)
{
	for(u32 i = 0; i < len; ++i)
		if(str[i] == mychar)
			return &str[i];
	return NULL;
}

static u32 get_offset_until_semilast_part(nnc_romfs_ctx *ctx, const u16 *utf16_path, u32 len, const u16 **last_part, u32 *last_part_len, int *dir_hint)
{
	static u16 char_slash = '/';
	static u16 char_null  = '\0';

	const u16 *eos = utf16_path + len;

	while(utf16_path[0] == char_slash)
		++utf16_path;

	/* we parsed either "/" or ""; both should refer to the root */
	if(utf16_path[0] == char_null)
	{
		*last_part = NULL;
		*dir_hint = 1;
		return 0;
	}

	u32 off = 0; /* root directory */
	const u16 *testvalid;
	const u16 *slash = utf16_path;
	const u16 *prev = utf16_path;

	*dir_hint = 0;

	while((slash = find_next_char(prev, eos - prev, char_slash)))
	{
		/* consider this input
		 *  test/  <= this loop will put testvalid at null after which will be from the loop */
		testvalid = slash;
		while(*testvalid == char_slash) ++testvalid;
		if(*testvalid == char_null) { *dir_hint = 1; break; }

		if((off = get_dir_single_offset(ctx, prev, slash - prev, off)) == INVAL)
			return INVAL;

		/* this will skip all slashes that go directly after our slash,
		 *  consider this input
		 *   abc///b   <= will put prev at "b", after which it can find no more slashes and break */
		prev = slash;
		while(*prev == char_slash)
			++prev;
	}

	*last_part_len = (slash ? slash : eos) - prev;
	*last_part = prev;

	return off;
}

static void fill_info_file(nnc_romfs_ctx *ctx, nnc_romfs_info *info, u32 offset)
{
	u8 *file = FILE_META(ctx, offset);
	info->type = NNC_ROMFS_FILE;
	info->u.f.sibling = FILE_SIBLING(file);
	info->u.f.parent = FILE_PARENT(file);
	info->u.f.offset = FILE_OFFSET(file);
	info->u.f.size = FILE_SIZE(file);
	info->filename_length = FILE_NAMELEN(file) / sizeof(u16);
	info->filename = FILE_NAME(file);
}

static void fill_info_dir(nnc_romfs_ctx *ctx, nnc_romfs_info *info, u32 offset)
{
	u8 *dir = DIR_META(ctx, offset);
	info->type = NNC_ROMFS_DIR;
	info->u.d.dchildren = DIR_DCHILDREN(dir);
	info->u.d.fchildren = DIR_FCHILDREN(dir);
	info->u.d.sibling = DIR_SIBLING(dir);
	info->u.d.parent = DIR_PARENT(dir);
	info->filename_length = DIR_NAMELEN(dir) / sizeof(u16);
	info->filename = DIR_NAME(dir);
}

nnc_result nnc_get_info(nnc_romfs_ctx *ctx, nnc_romfs_info *info, const char *path)
{
	if(!nnc_cbuf_utf8_to_utf16(&ctx->cbuf, (u8 *) path, strlen(path)))
		return NNC_R_NOT_FOUND;
	const u16 *last_part;
	u32 last_part_len, rof;
	int dir_hint;
	u32 parent_off = get_offset_until_semilast_part(ctx, ctx->cbuf.buffer.utf16, ctx->cbuf.converted_length, &last_part, &last_part_len, &dir_hint);
	if(parent_off == INVAL) return NNC_R_NOT_FOUND;
	/* means the root directory was requested */
	if(last_part == NULL)
	{
		rof = 0;
		goto do_fill_dir;
	}

	/* means a trailing slash was found; always a directory */
	if(!dir_hint)
	{
		/* now we first look for a file, since that is more likely in the no trailing slash case */
		rof = get_file_single_offset(ctx, last_part, last_part_len, parent_off);
		if(rof != INVAL)
		{
			fill_info_file(ctx, info, rof);
			return NNC_R_OK;
		}
		/* it could still be a directory if there was no trailing slash, worth checking for */
	}

	rof = get_dir_single_offset(ctx, last_part, last_part_len, parent_off);
	if(rof != INVAL)
	{
do_fill_dir:
		fill_info_dir(ctx, info, rof);
		return NNC_R_OK;
	}

	info->type = NNC_ROMFS_NONE;
	return NNC_R_NOT_FOUND;
}

const char *nnc_romfs_info_filename(nnc_romfs_ctx *ctx, nnc_romfs_info *info)
{
	return (const char *) nnc_cbuf_utf16_to_utf8(&ctx->cbuf, info->filename, info->filename_length);
}

int nnc_romfs_next(nnc_romfs_iterator *it, nnc_romfs_info *ent)
{
	if(!it->dir || it->next == INVAL) return 0;
	if(it->in_dir)
	{
		fill_info_dir(it->ctx, ent, it->next);
		it->next = ent->u.d.sibling;
		if(it->next == INVAL)
		{
			it->next = it->dir->u.d.fchildren;
			it->in_dir = false;
		}
	}
	else
	{
		fill_info_file(it->ctx, ent, it->next);
		it->next = ent->u.f.sibling;
	}
	return 1;
}

nnc_romfs_iterator nnc_romfs_mkit(nnc_romfs_ctx *ctx, const nnc_romfs_info *dir)
{
	nnc_romfs_iterator ret;
	if(dir->type != NNC_ROMFS_DIR)
	{
		ret.next = INVAL;
		ret.dir = NULL;
		return ret;
	}
	ret.dir = dir;
	if(dir->u.d.dchildren == INVAL)
	{
		ret.next = dir->u.d.fchildren;
		ret.in_dir = false;
	}
	else
	{
		ret.next = dir->u.d.dchildren;
		ret.in_dir = true;
	}
	ret.ctx = ctx;
	return ret;
}

result nnc_romfs_open_subview(nnc_romfs_ctx *ctx, nnc_subview *sv, nnc_romfs_info *info)
{
	if(info->type != NNC_ROMFS_FILE) return NNC_R_NOT_A_FILE;
	nnc_subview_open(sv, ctx->rs, ctx->header.data_offset + info->u.f.offset,
		info->u.f.size);
	return NNC_R_OK;
}

static result nnc_romfs_to_vfs_iterate(nnc_romfs_ctx *ctx, nnc_romfs_info *info, nnc_vfs_directory_node *dir)
{
	nnc_romfs_iterator it = nnc_romfs_mkit(ctx, info);
	nnc_vfs_directory_node *ndir;
	nnc_romfs_info ent;
	nnc_subview sv;
	result ret;

	while(nnc_romfs_next(&it, &ent))
	{
		const char *name = nnc_romfs_info_filename(ctx, &ent);
		if(ent.type == NNC_ROMFS_DIR)
		{
			/* first we have to make a new directory */
			TRY(nnc_vfs_add_directory(dir, name, &ndir));
			/* now we can recurse into it */
			TRY(nnc_romfs_to_vfs_iterate(ctx, &ent, ndir));
		}
		else
		{
			/* first we need to open the subview */
			TRY(nnc_romfs_open_subview(ctx, &sv, &ent));
			/* now we can add a file with reader, which will copy the subview */
			TRY(nnc_vfs_add_file(dir, name, NNC_VFS_SUBVIEW(&sv)));
		}
	}

	return NNC_R_OK;
}

result nnc_romfs_to_vfs(nnc_romfs_ctx *ctx, nnc_vfs_directory_node *dir)
{
	nnc_romfs_info info;
	result ret;
	TRY(nnc_get_info(ctx, &info, "/"));
	return nnc_romfs_to_vfs_iterate(ctx, &info, dir);
}

result nnc_init_romfs(nnc_rstream *rs, nnc_romfs_ctx *ctx)
{
	result ret;
	TRY(nnc_read_romfs_header(rs, &ctx->header));

	ctx->file_meta_data = ctx->dir_meta_data = NULL;
	ctx->file_hash_tab = ctx->dir_hash_tab = NULL;

	TRY(nnc_cbuf_init(&ctx->cbuf, 0));

	ret = NNC_R_NOMEM;
	if(!(ctx->file_hash_tab = malloc(ctx->header.file_hash.length)))
		goto fail;
	if(!(ctx->file_meta_data = malloc(ctx->header.file_meta.length)))
		goto fail;
	if(!(ctx->dir_hash_tab = malloc(ctx->header.dir_hash.length)))
		goto fail;
	if(!(ctx->dir_meta_data = malloc(ctx->header.dir_meta.length)))
		goto fail;

	if((ret = read_at_exact(rs, ctx->header.file_hash.offset, (u8 *) ctx->file_hash_tab,
		ctx->header.file_hash.length)) != NNC_R_OK) goto fail;
	if((ret = read_at_exact(rs, ctx->header.file_meta.offset, (u8 *) ctx->file_meta_data,
		ctx->header.file_meta.length)) != NNC_R_OK) goto fail;
	if((ret = read_at_exact(rs, ctx->header.dir_hash.offset, (u8 *) ctx->dir_hash_tab,
		ctx->header.dir_hash.length)) != NNC_R_OK) goto fail;
	if((ret = read_at_exact(rs, ctx->header.dir_meta.offset, (u8 *) ctx->dir_meta_data,
		ctx->header.dir_meta.length)) != NNC_R_OK) goto fail;

	ctx->rs = rs;
	return NNC_R_OK;
fail:
	/* calls the same functions as we would want to do here */
	nnc_free_romfs(ctx);
	return ret;
}

void nnc_free_romfs(nnc_romfs_ctx *ctx)
{
	free(ctx->file_meta_data);
	free(ctx->file_hash_tab);
	free(ctx->dir_meta_data);
	free(ctx->dir_hash_tab);
	nnc_cbuf_free(&ctx->cbuf);
}

static bool nnc_is_composite(u32 x)
{
	/* if x is divisable by a "known" prime, it is a composite,
	 * this algorithm is obviously bad but it's what Nintendo uses so we'll roll with it */
	return
		   x % 2  == 0
		|| x % 3  == 0
		|| x % 5  == 0
		|| x % 7  == 0
		|| x % 11 == 0
		|| x % 13 == 0
		|| x % 17 == 0;
}

static u32 nnc_next_prime(u32 x)
{
	u32 ret = x;
	while(nnc_is_composite(ret))
		++ret;
	return ret;
}

/* N.B.: the algorithm used in AM is slightly different, but this will do */
static u32 nnc_romfs_table_length(u32 entries)
{
	if(entries <= 3)       return 3;
	else if(entries <= 19) return entries | 1; /* quite mysterious, does not return a prime for entries one_of {8,9,14,15} */
	else                   return nnc_next_prime(entries);
}

/* this struct is used for saving the "stack" in the functions for creating the
 * hash table structures */
struct romfs_writer_ctx
{
	u32          *dir_hash, *file_hash;
	struct dynbuf dir_meta,  file_meta;
	nnc_utf_conversion_buffer cbuf;
	u32 dir_hashtab_len;
	u32 file_hashtab_len;
	/* state */
	u64 current_file_data_offset; /* incremented as we go */
};

static result nnc_romfs_convert_to_utf16(struct romfs_writer_ctx *ctx, const char *utf8)
{
	return nnc_cbuf_utf8_to_utf16(&ctx->cbuf, (const u8 *) utf8, strlen(utf8))
		? NNC_R_OK : NNC_R_NOMEM;
}

static inline void nnc_romfs_add_to_hash_to_offset_table(struct romfs_writer_ctx *ctx, u32 parent_offset, u32 offset, u32 *hash_table, u32 hashtablen, u8 *meta_table, u32 next_bucket_offset)
{
	u32 index = hash_func(ctx->cbuf.buffer.utf16, ctx->cbuf.converted_length, parent_offset) % hashtablen;
	u32 initial_offset = hash_table[index];
	if(initial_offset == INVAL)
		hash_table[index] = offset;
	else
	{
		u32 coffset, noffset = initial_offset;
		/* we have to walk the linked list and insert our offset at the back */
		do {
			coffset = noffset;
			noffset = LE32P(meta_table + coffset + next_bucket_offset);
		} while(noffset != INVAL);
		/* we can now write our offset in coffset's NEXTBUCKET field */
		U32P(meta_table + coffset + next_bucket_offset) = LE32(offset);
	}
}

static inline void nnc_romfs_add_to_parent_directory(struct romfs_writer_ctx *ctx, u32 parent_offset, u32 offset, u8 *meta_table, u32 children_offset, u32 next_sibling_offset)
{
	/* dir meta is hardcoded since we should always add to the directory metadata obviously */
	u32 parent_child_offset = LE32P(ctx->dir_meta.buffer + parent_offset + children_offset);
	if(parent_child_offset == INVAL)
		U32P(ctx->dir_meta.buffer + parent_offset + children_offset) = LE32(offset);
	else
	{
		/* we need to put ourselves at the end of the siblings list of the first child */
		u32 coffset, noffset = parent_child_offset;
		do {
			coffset = noffset;
			/* however here we should use the meta table, since the sibling pointer
			 * is stored in the appropriate meta table */
			noffset = LE32P(meta_table + coffset + next_sibling_offset);
		} while(noffset != INVAL);
		U32P(meta_table + coffset + next_sibling_offset) = LE32(offset);
	}
}

static result nnc_romfs_write_directory(struct romfs_writer_ctx *ctx, const char *vdirname, u32 parent_offset, u32 *new_parent_offset)
{
	nnc_result ret = nnc_romfs_convert_to_utf16(ctx, vdirname ? vdirname : "");
	if(ret != NNC_R_OK) return ret;
	u32 actual_string_length = ctx->cbuf.converted_length * 2;

	u32 meta_offset = ctx->dir_meta.used;
	nnc_romfs_add_to_hash_to_offset_table(ctx, parent_offset, meta_offset, ctx->dir_hash, ctx->dir_hashtab_len, ctx->dir_meta.buffer, DIR_OFF_NEXTBUCKET);

	u8 mbuf[DIR_OFF_NAMELEN + 4];

	U32P(&mbuf[DIR_OFF_PARENT]) = LE32(parent_offset);
	U32P(&mbuf[DIR_OFF_SIBLING]) = LE32(INVAL); /* initialize to no next sibling */
	U32P(&mbuf[DIR_OFF_DCHILDREN]) = LE32(INVAL);
	U32P(&mbuf[DIR_OFF_FCHILDREN]) = LE32(INVAL);
	U32P(&mbuf[DIR_OFF_NEXTBUCKET]) = LE32(INVAL);
	U32P(&mbuf[DIR_OFF_NAMELEN]) = LE32(actual_string_length);

	TRY(dynbuf_push(&ctx->dir_meta, mbuf, DIR_OFF_NAMELEN + 4));
	TRY(dynbuf_push(&ctx->dir_meta, (u8 *) ctx->cbuf.buffer.utf16, ALIGN(actual_string_length, 4)));

	/* we have to add ourselves to some lists! */
	if(vdirname) /* can't add to the root */
		nnc_romfs_add_to_parent_directory(ctx, parent_offset, meta_offset, ctx->dir_meta.buffer, DIR_OFF_DCHILDREN, DIR_OFF_SIBLING);

	*new_parent_offset = meta_offset;

	return NNC_R_OK;
}

static result nnc_romfs_write_file_meta(struct romfs_writer_ctx *ctx, nnc_vfs_file_node *node, u32 parent_offset)
{
	nnc_result ret = nnc_romfs_convert_to_utf16(ctx, node->vname);
	if(ret != NNC_R_OK) return ret;
	u32 actual_string_length = ctx->cbuf.converted_length * 2;

	u32 meta_offset = ctx->file_meta.used;
	nnc_romfs_add_to_hash_to_offset_table(ctx, parent_offset, meta_offset, ctx->file_hash, ctx->file_hashtab_len, ctx->file_meta.buffer, FILE_OFF_NEXTBUCKET);

	u8 mbuf[FILE_OFF_NAMELEN + 4];

	u64 filesize = nnc_vfs_node_size(node);

	U32P(&mbuf[FILE_OFF_PARENT]) = LE32(parent_offset);
	U32P(&mbuf[FILE_OFF_SIBLING]) = LE32(INVAL); /* initialize to invalid since we do not know this yet */
	U64P(&mbuf[FILE_OFF_OFFSET]) = LE64(ctx->current_file_data_offset);
	U64P(&mbuf[FILE_OFF_SIZE]) = LE64(filesize);
	U32P(&mbuf[FILE_OFF_NEXTBUCKET]) = LE32(INVAL);
	U32P(&mbuf[FILE_OFF_NAMELEN]) = LE32(actual_string_length);

	TRY(dynbuf_push(&ctx->file_meta, mbuf, FILE_OFF_NAMELEN + 4));
	TRY(dynbuf_push(&ctx->file_meta, (u8 *) ctx->cbuf.buffer.utf16, ALIGN(actual_string_length, 4)));

	/* we now need to add ourselves to the directory */
	nnc_romfs_add_to_parent_directory(ctx, parent_offset, meta_offset, ctx->file_meta.buffer, DIR_OFF_FCHILDREN, FILE_OFF_SIBLING);

	ctx->current_file_data_offset += filesize;
	ctx->current_file_data_offset = ALIGN(ctx->current_file_data_offset, 16);

	return NNC_R_OK;
}

static result nnc_romfs_write_meta(struct romfs_writer_ctx *ctx, nnc_vfs_directory_node *dir, u32 parent_offset)
{
	nnc_vfs_directory_node *ndir;
	u32 new_parent_offset;
	result ret;
	for(unsigned i = 0; i < dir->filecount; ++i)
		TRY(nnc_romfs_write_file_meta(ctx, &dir->file_children[i], parent_offset));
	for(unsigned i = 0; i < dir->dircount; ++i)
	{
		ndir = &dir->directory_children[i];
		/* first write this directory */
		TRY(nnc_romfs_write_directory(ctx, ndir->vname, parent_offset, &new_parent_offset));
		/* and then recurse further into this directory */
		TRY(nnc_romfs_write_meta(ctx, ndir, new_parent_offset));
	}
	return NNC_R_OK;
}

static result nnc_romfs_write_file_data(nnc_wstream *ws, nnc_vfs_directory_node *dir)
{
	u32 copied, padding;
	result ret;

	/* write all files... */
	for(unsigned i = 0; i < dir->filecount; ++i)
	{
		nnc_vfs_stream *stream;
		TRY(nnc_vfs_open_node(&dir->file_children[i], &stream));
		ret = nnc_copy(stream, ws, &copied);
		nnc_vfs_close_node(stream);
		if(ret != NNC_R_OK)
			return ret;
		padding = ALIGN(copied, 16) - copied;
		TRY(nnc_write_padding(ws, padding));
	}
	/* and recurse into directories */
	for(unsigned i = 0; i < dir->dircount; ++i)
		TRY(nnc_romfs_write_file_data(ws, &dir->directory_children[i]));

	return NNC_R_OK;
}

result nnc_write_romfs(nnc_vfs *vfs, nnc_wstream *ws)
{
	nnc_result ret = NNC_R_OK;

	/* first we start building the metadata & offset by hash lookup tables for both files and directories */

	/* dir count starts at one due to the root dir / */
	struct romfs_writer_ctx ctx = { NULL, NULL, {NULL}, {NULL}, {0,0,{NULL}},  0, 0, 0 };
	nnc_ivfc_writer writer = { NULL };

	ctx.dir_hashtab_len = nnc_romfs_table_length(vfs->totaldirs);
	ctx.file_hashtab_len = nnc_romfs_table_length(vfs->totalfiles);

	u32 file_hashtab_size = ctx.file_hashtab_len * sizeof(u32);
	u32 dir_hashtab_size = ctx.dir_hashtab_len * sizeof(u32);

	TRY(dynbuf_new(&ctx.dir_meta, 8192));
	TRYLBL(dynbuf_new(&ctx.file_meta, 8192), out);

	ctx.file_hash = malloc(file_hashtab_size);
	ctx.dir_hash = malloc(dir_hashtab_size);
	if(!ctx.file_hash || !ctx.dir_hash)
		goto out;

	memset(ctx.file_hash, 0xFF, file_hashtab_size);
	memset(ctx.dir_hash, 0xFF, dir_hashtab_size);

	TRYLBL(nnc_cbuf_init(&ctx.cbuf, 0), out);

	/* first we have to write the root directory */
	u32 root_directory_offset;
	TRYLBL(nnc_romfs_write_directory(&ctx, NULL, 0, &root_directory_offset), out);

	/* first walk to add all metadata, and later we walk again but to add all file data */
	TRYLBL(nnc_romfs_write_meta(&ctx, &vfs->root_directory, root_directory_offset), out);

	TRYLBL(nnc_open_ivfc_writer(&writer, ws, NNC_IVFC_LEVELS_ROMFS, NNC_IVFC_ID_ROMFS, NNC_IVFC_BLOCKSIZE_ROMFS), out);

	u8 romfs_header_buf[0x28];

	U32P(&romfs_header_buf[0x00]) = LE32(sizeof(romfs_header_buf)); /* header size */
	U32P(&romfs_header_buf[0x04]) = LE32(sizeof(romfs_header_buf)); /* offset/size pairs no w*/
	U32P(&romfs_header_buf[0x08]) = LE32(dir_hashtab_size);
	U32P(&romfs_header_buf[0x0C]) = LE32(sizeof(romfs_header_buf) + dir_hashtab_size);
	U32P(&romfs_header_buf[0x10]) = LE32(ctx.dir_meta.used);
	U32P(&romfs_header_buf[0x14]) = LE32(sizeof(romfs_header_buf) + dir_hashtab_size + ctx.dir_meta.used);
	U32P(&romfs_header_buf[0x18]) = LE32(file_hashtab_size);
	U32P(&romfs_header_buf[0x1C]) = LE32(sizeof(romfs_header_buf) + dir_hashtab_size + ctx.dir_meta.used + file_hashtab_size);
	U32P(&romfs_header_buf[0x20]) = LE32(ctx.file_meta.used);
	U32P(&romfs_header_buf[0x24]) = ALIGN(LE32(sizeof(romfs_header_buf) + dir_hashtab_size + ctx.dir_meta.used + file_hashtab_size + ctx.file_meta.used), 0x10);

	TRYLBL(NNC_WS_CALL(writer, write, romfs_header_buf, sizeof(romfs_header_buf)), out);

	/* now we can dump our tables and afterwards ... */
	TRYLBL(NNC_WS_CALL(writer, write, (u8 *) ctx.dir_hash, dir_hashtab_size), out);
	TRYLBL(NNC_WS_CALL(writer, write, (u8 *) ctx.dir_meta.buffer, ctx.dir_meta.used), out);
	TRYLBL(NNC_WS_CALL(writer, write, (u8 *) ctx.file_hash, file_hashtab_size), out);
	TRYLBL(NNC_WS_CALL(writer, write, (u8 *) ctx.file_meta.buffer, ctx.file_meta.used), out);

	/* and now the long-awaited files, which we first need to put at an aligned offset obviously */
	u32 now_off = NNC_WS_CALL0(writer, tell);
	TRYLBL(nnc_write_padding(NNC_WSP(&writer), ALIGN(now_off, 0x10) - now_off), out);
	TRYLBL(nnc_romfs_write_file_data(NNC_WSP(&writer), &vfs->root_directory), out);

	/* and this close writes the IVFC hashes and headers and such */
	ret = NNC_WS_CALL0(writer, close);
	writer.funcs = NULL; /* just so we don't abort twice */

out:
	if(writer.funcs && ret != NNC_R_OK)
		nnc_ivfc_abort_write(&writer);

	nnc_dynbuf_free(&ctx.file_meta);
	nnc_dynbuf_free(&ctx.dir_meta);
	nnc_cbuf_free(&ctx.cbuf);
	free(ctx.file_hash);
	free(ctx.dir_hash);

	return ret;
}
