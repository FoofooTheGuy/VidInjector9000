
/* #if NNC_PLATFORM_UNIX | NNC_PLATFORM_3DS */
#define _LARGEFILE64_SOURCE
#define _DEFAULT_SOURCE
#define _BSD_SOURCE
/* #endif */

#include "./internal.h"

#if NNC_PLATFORM_UNIX || NNC_PLATFORM_3DS
#include <unistd.h>
#endif

#include "nnc/crypto.h"
#include "nnc/stream.h"
#include <stdlib.h>
#include <string.h>

#define FILE_SIZE_NULL ((u32) -1)

enum nnc_file_flags {
	NNC_FILE_KEEP_ALIVE = 1,
};

static result nnc_seek_file_abs(FILE* file, u32 pos, u32* npos)
{
	int res;
	if (pos <= INT32_MAX || sizeof(long) > 4)
		res = fseek(file, pos, SEEK_SET);
#if NNC_PLATFORM_WINDOWS
	else
		res = _fseeki64(file, pos, SEEK_SET);
#elif NNC_PLATFORM_UNIX || NNC_PLATFORM_3DS
	else /* This should be fine, be sure to test */
		res = lseek64(fileno(file), pos, SEEK_SET);
#else
	else
	{
		/* ugly hack */
		res = fseek(file, INT32_MAX, SEEK_SET);
		if (res == 0)
			res = fseek(file, pos - INT32_MAX, SEEK_CUR);
	}
#endif
	if (res == 0 && npos)
		*npos = pos;
	return res == 0 ? NNC_R_OK : NNC_R_SEEK_RANGE;
}

static result file_read(nnc_file* self, u8* buf, u32 max, u32* totalRead)
{
	u32 total = fread(buf, 1, max, self->f);
	if (totalRead) *totalRead = total;
	if (total != max)
		return ferror(self->f) ? NNC_R_FAIL_READ : NNC_R_OK;
	self->off += total;
	return NNC_R_OK;
}

static result file_seek_abs(nnc_file* self, u32 pos)
{
	if (self->size == 0 && pos == 0) return NNC_R_OK;
	if (pos >= self->size) return NNC_R_SEEK_RANGE;
	return nnc_seek_file_abs(self->f, pos, &self->off);
}

static result file_seek_rel(nnc_file* self, u32 pos)
{
	u32 npos = self->off + pos;
	if (npos >= self->size) return NNC_R_SEEK_RANGE;
	return nnc_seek_file_abs(self->f, npos, &self->off);
}

static u32 file_size(nnc_file* self) { return self->size; }
static u32 file_tell(nnc_file* self) { return self->off; }

static void file_close(nnc_file* self)
{
	if (!(self->flags & NNC_FILE_KEEP_ALIVE))
		fclose(self->f);
}

static const nnc_rstream_funcs file_funcs = {
	.read = (nnc_read_func)file_read,
	.seek_abs = (nnc_seek_abs_func)file_seek_abs,
	.seek_rel = (nnc_seek_rel_func)file_seek_rel,
	.size = (nnc_size_func)file_size,
	.close = (nnc_close_func)file_close,
	.tell = (nnc_tell_func)file_tell,
};

static u32 get_file_size(FILE* file, u32 seekback)
{
	fseek(file, 0, SEEK_END);
	u32 size
#if NNC_PLATFORM_WINDOWS
		= _ftelli64(file);
#elif NNC_PLATFORM_UNIX || NNC_PLATFORM_3DS
		= ftello64(file);
#else
		= ftell(file); /* dangerous call */
#endif
	nnc_seek_file_abs(file, seekback, NULL);
	return size;
}

result nnc_file_open(nnc_file* self, const char* name)
{
	self->f = fopen(name, "rb");
	self->flags = 0;
	self->off = 0;
	if (!self->f) return NNC_R_FAIL_OPEN;
	self->size = get_file_size(self->f, 0);
	self->funcs = &file_funcs;
	return self->size == FILE_SIZE_NULL ? NNC_R_FAIL_OPEN : NNC_R_OK;
}

static nnc_result wfile_write(nnc_wfile* self, nnc_u8* buf, nnc_u32 size)
{
	result res = fwrite(buf, 1, size, self->f) == size ? NNC_R_OK : NNC_R_FAIL_WRITE;
	if (res == NNC_R_OK) self->off += size;
	return res;
}

static result wfile_close(nnc_wfile* self)
{
	return fclose(self->f) == 0 ? NNC_R_OK : NNC_R_FAIL_WRITE;
}

static nnc_result wfile_seek(nnc_wfile* self, nnc_u32 pos)
{
	return nnc_seek_file_abs(self->f, pos, &self->off);
}

static nnc_u32 wfile_tell(nnc_wfile* self)
{
	return self->off;
}

static nnc_result wfile_subreadstream(nnc_wfile* self, nnc_subview* out, nnc_u32 start, nnc_u32 len)
{
	nnc_file* substream = malloc(sizeof(nnc_file));
	if (!substream) return NNC_R_NOMEM;
	substream->funcs = &file_funcs;
	substream->flags = NNC_FILE_KEEP_ALIVE;
	substream->f = self->f;
	substream->size = get_file_size(self->f, self->off);
	if (start + len > substream->size)
		return NNC_R_SEEK_RANGE;
	if (substream->size == FILE_SIZE_NULL)
		return NNC_R_FAIL_OPEN;
	nnc_subview_open(out, NNC_RSP(substream), start, len);
	nnc_subview_delete_on_close(out);
	return NNC_R_OK;
}

static const nnc_wstream_funcs wfile_funcs = {
	.write = (nnc_write_func)wfile_write,
	.close = (nnc_wclose_func)wfile_close,
	.seek = (nnc_wseek_func)wfile_seek,
	.tell = (nnc_wtell_func)wfile_tell,
	.subreadstream = (nnc_wsubreadstream_func)wfile_subreadstream,
};

result nnc_wfile_open(nnc_wfile* self, const char* name)
{
	self->f = fopen(name, "wb+");
	if (!self->f) return NNC_R_FAIL_OPEN;
	self->funcs = &wfile_funcs;
	self->off = 0;
	return NNC_R_OK;
}

static result hdrsaver_write(nnc_header_saver* self, nnc_u8* buf, nnc_u32 size)
{
	if (self->pos >= self->start && self->pos < self->start + self->count)
	{
		u32 relpos = self->pos - self->start;
		u32 to_save = MIN(self->count - relpos, size);
		memcpy(&self->buffer[relpos], buf, to_save);
	}
	self->pos += size;
	return self->child->funcs->write(self->child, buf, size);
}

static result hdrsaver_close(nnc_header_saver* self) { free(self->buffer); return NNC_R_OK; }
static nnc_result hdrsaver_seek(nnc_header_saver* self, nnc_u32 pos) { self->pos = pos; return self->child->funcs->seek(self->child, pos); }
static nnc_u32 hdrsaver_tell(nnc_header_saver* self) { return self->child->funcs->tell(self->child); }


static const nnc_wstream_funcs hdrsaver_funcs_seekable = {
	.write = (nnc_write_func)hdrsaver_write,
	.close = (nnc_wclose_func)hdrsaver_close,
	.seek = (nnc_wseek_func)hdrsaver_seek,
	.tell = (nnc_wtell_func)hdrsaver_tell,
};

static const nnc_wstream_funcs hdrsaver_funcs = {
	.write = (nnc_write_func)hdrsaver_write,
	.close = (nnc_wclose_func)hdrsaver_close,
	.tell = (nnc_wtell_func)hdrsaver_tell,
};

nnc_result nnc_open_header_saver(nnc_header_saver* self, nnc_wstream* child, nnc_u32 count)
{
	self->funcs = child->funcs->seek ? &hdrsaver_funcs_seekable : &hdrsaver_funcs;
	self->child = child;
	self->count = count;
	self->start = NNC_WS_PCALL0(child, tell);
	self->pos = self->start;

	self->buffer = malloc(count);
	return self->buffer ? NNC_R_OK : NNC_R_NOMEM;
}

static result mem_read(nnc_memory* self, u8* buf, u32 max, u32* totalRead)
{
	*totalRead = MIN(max, self->size);
	memcpy(buf, ((u8*)self->un.ptr_const) + self->pos, *totalRead);
	self->pos += *totalRead;
	return NNC_R_OK;
}

static result mem_seek_abs(nnc_memory* self, u32 pos)
{
	if (pos >= self->size) return NNC_R_SEEK_RANGE;
	self->pos = pos;
	return NNC_R_OK;
}

static result mem_seek_rel(nnc_memory* self, u32 pos)
{
	u32 npos = self->pos + pos;
	if (npos >= self->size) return NNC_R_SEEK_RANGE;
	self->pos = npos;
	return NNC_R_OK;
}

static u32 mem_size(nnc_memory* self)
{
	return self->size;
}

static void mem_close(nnc_memory* self)
{
	/* nothing to do ... */
	(void)self;
}

static void mem_own_close(nnc_memory* self)
{
	free(self->un.ptr);
}

static u32 mem_tell(nnc_memory* self)
{
	return self->pos;
}

static const nnc_rstream_funcs mem_funcs = {
	.read = (nnc_read_func)mem_read,
	.seek_abs = (nnc_seek_abs_func)mem_seek_abs,
	.seek_rel = (nnc_seek_rel_func)mem_seek_rel,
	.size = (nnc_size_func)mem_size,
	.close = (nnc_close_func)mem_close,
	.tell = (nnc_tell_func)mem_tell,
};

static const nnc_rstream_funcs mem_own_funcs = {
	.read = (nnc_read_func)mem_read,
	.seek_abs = (nnc_seek_abs_func)mem_seek_abs,
	.seek_rel = (nnc_seek_rel_func)mem_seek_rel,
	.size = (nnc_size_func)mem_size,
	.close = (nnc_close_func)mem_own_close,
	.tell = (nnc_tell_func)mem_tell,
};

void nnc_mem_open(nnc_memory* self, const void* ptr, u32 size)
{
	self->funcs = &mem_funcs;
	self->size = size;
	self->un.ptr_const = ptr;
	self->pos = 0;
}

void nnc_mem_own_open(nnc_memory* self, void* ptr, u32 size)
{
	self->funcs = &mem_own_funcs;
	self->size = size;
	self->un.ptr = ptr;
	self->pos = 0;
}

enum nnc_subview_flags {
	NNC_SUBVIEW_DELETE_ON_CLOSE = 1,
};

static result subview_read(nnc_subview* self, u8* buf, u32 max, u32* totalRead)
{
	u32 sizeleft = self->size - self->pos;
	max = MIN(max, sizeleft);
	result ret;
	/* seek to correct offset in child */
	TRY(NNC_RS_PCALL(self->child, seek_abs, self->off + self->pos));
	ret = NNC_RS_PCALL(self->child, read, buf, max, totalRead);
	self->pos += *totalRead;
	return ret;
}

static result subview_seek_abs(nnc_subview* self, u32 pos)
{
	if (pos >= self->size) return NNC_R_SEEK_RANGE;
	self->pos = pos;
	return NNC_R_OK;
}

static result subview_seek_rel(nnc_subview* self, u32 pos)
{
	u32 npos = self->pos + pos;
	if (npos >= self->size) return NNC_R_SEEK_RANGE;
	self->pos = npos;
	return NNC_R_OK;
}

static u32 subview_size(nnc_subview* self)
{
	return self->size;
}

static void subview_close(nnc_subview* self)
{
	if (self->flags & NNC_SUBVIEW_DELETE_ON_CLOSE)
	{
		NNC_RS_PCALL0(self->child, close);
		free(self->child);
	}
}

static nnc_u32 subview_tell(nnc_subview* self)
{
	return self->pos;
}

static const nnc_rstream_funcs subview_funcs = {
	.read = (nnc_read_func)subview_read,
	.seek_abs = (nnc_seek_abs_func)subview_seek_abs,
	.seek_rel = (nnc_seek_rel_func)subview_seek_rel,
	.size = (nnc_size_func)subview_size,
	.close = (nnc_close_func)subview_close,
	.tell = (nnc_tell_func)subview_tell,
};

void nnc_subview_open(nnc_subview* self, nnc_rstream* child, nnc_u32 off, nnc_u32 len)
{
	self->funcs = &subview_funcs;
	self->flags = 0;
	self->child = child;
	self->size = len;
	self->off = off;
	self->pos = 0;
}

void nnc_subview_delete_on_close(nnc_subview* self)
{
	self->flags |= NNC_SUBVIEW_DELETE_ON_CLOSE;
}

/* ... vfs code ... */

#define DEFAULT_FILE_CHILDREN_ALLOC 8
#define DEFAULT_DIR_CHILDREN_ALLOC  8

static result nnc_vfs_initialize_directory_node(nnc_vfs_directory_node* dir, const char* vname, nnc_vfs* vfs)
{
	dir->directory_children = malloc(DEFAULT_DIR_CHILDREN_ALLOC * sizeof(nnc_vfs_directory_node));
	dir->file_children = malloc(DEFAULT_FILE_CHILDREN_ALLOC * sizeof(nnc_vfs_file_node));
	dir->associated_vfs = vfs;
	dir->vname = strdup(vname);
	if (!dir->directory_children || !dir->file_children || (vname && !dir->vname))
	{
		free(dir->directory_children);
		free(dir->file_children);
		free(dir->vname);
		return NNC_R_NOMEM;
	}
	dir->dircount = 0;
	dir->filecount = 0;
	dir->diralloc = DEFAULT_DIR_CHILDREN_ALLOC;
	dir->filealloc = DEFAULT_FILE_CHILDREN_ALLOC;
	return NNC_R_OK;
}

result nnc_vfs_init(nnc_vfs* vfs)
{
	vfs->totalfiles = 0;
	vfs->totaldirs = 1;
	return nnc_vfs_initialize_directory_node(&vfs->root_directory, NULL, vfs);
}

static void nnc_vfs_free_file_node(nnc_vfs_file_node* file)
{
	file->generator->delete_data(file->data);
	free(file->vname);
}

static void nnc_vfs_free_directory_node(nnc_vfs_directory_node* dir)
{
	for (unsigned i = 0; i < dir->dircount; ++i)
		nnc_vfs_free_directory_node(&dir->directory_children[i]);
	dir->associated_vfs->totaldirs -= dir->dircount;

	for (unsigned i = 0; i < dir->filecount; ++i)
		nnc_vfs_free_file_node(&dir->file_children[i]);
	dir->associated_vfs->totalfiles -= dir->filecount;

	free(dir->directory_children);
	free(dir->file_children);
	free(dir->vname);
}

void nnc_vfs_free(nnc_vfs* vfs)
{
	if (vfs->root_directory.directory_children && vfs->root_directory.file_children)
	{
		nnc_vfs_free_directory_node(&vfs->root_directory);
		vfs->root_directory.directory_children = NULL;
		vfs->root_directory.file_children = NULL;
	}
}

void nnc_vfs_free_files(nnc_vfs_directory_node* dir)
{
	for (unsigned i = 0; i < dir->filecount; ++i)
		nnc_vfs_free_file_node(&dir->file_children[i]);
	dir->associated_vfs->totalfiles -= dir->filecount;
	dir->filecount = 0;
}

void nnc_vfs_free_directories(nnc_vfs_directory_node* dir)
{
	for (unsigned i = 0; i < dir->dircount; ++i)
		nnc_vfs_free_directory_node(&dir->directory_children[i]);
	dir->associated_vfs->totaldirs -= dir->dircount;
	dir->dircount = 0;
}

nnc_result nnc_vfs_add_file(nnc_vfs_directory_node* dir, const char* vname, const nnc_vfs_reader_generator* generator, ... /* generator parameters */)
{
	/* we need to allocate more */
	if (dir->filealloc == dir->filecount)
	{
		unsigned newalloc = dir->filealloc * 4;
		nnc_vfs_file_node* newdirs = realloc(dir->file_children, newalloc * sizeof(nnc_vfs_file_node));
		if (!newdirs) return NNC_R_NOMEM;
		dir->file_children = newdirs;
		dir->filealloc = newalloc;
	}

	nnc_vfs_file_node* newfile = &dir->file_children[dir->filecount];

	va_list va;
	va_start(va, generator);
	nnc_result res = generator->initialize(&newfile->data, va);
	va_end(va);

	if (res != NNC_R_OK)
		return res;

	newfile->generator = generator;
	newfile->vname = strdup(vname);
	if (!newfile->vname)
		return NNC_R_NOMEM;

	++dir->associated_vfs->totalfiles;
	++dir->filecount;
	return NNC_R_OK;
}

nnc_result nnc_vfs_add_directory(nnc_vfs_directory_node* dir, const char* vname, nnc_vfs_directory_node** out_new_dir)
{
	/* we need to allocate more */
	if (dir->diralloc == dir->dircount)
	{
		unsigned newalloc = dir->diralloc * 4;
		nnc_vfs_directory_node* newdirs = realloc(dir->directory_children, newalloc * sizeof(nnc_vfs_directory_node));
		if (!newdirs) return NNC_R_NOMEM;
		dir->directory_children = newdirs;
		dir->diralloc = newalloc;
	}
	nnc_vfs_directory_node* newdir = &dir->directory_children[dir->dircount++];
	nnc_result res = nnc_vfs_initialize_directory_node(newdir, vname, dir->associated_vfs);
	if (res != NNC_R_OK)
	{
		--dir->dircount;
		return res;
	}
	if (out_new_dir) *out_new_dir = newdir;
	++dir->associated_vfs->totaldirs;
	return NNC_R_OK;
}

#if NNC_PLATFORM_UNIX || NNC_PLATFORM_3DS
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#define DIRENT_API 1
#elif NNC_PLATFORM_WINDOWS
#include <windows.h>
#define WINDOWS_API 1
#endif

struct filename_builder {
	char* buf;
	unsigned alloc, basepos;
};

int fnbuild_setbase(struct filename_builder* builder, const char* base)
{
	int len = strlen(base);
	builder->alloc = (len + 1) * 2;
	builder->basepos = len + 1;
	builder->buf = malloc(builder->alloc);
	if (!builder->buf) return 0;
	memcpy(builder->buf, base, len);
#if NNC_PLATFORM_WINDOWS
	builder->buf[len] = '\\';
#else
	builder->buf[len] = '/';
#endif
	return 1;
}

void fnbuild_free(struct filename_builder* builder)
{
	free(builder->buf);
}

int fnbuild(struct filename_builder* builder, const char* newbit)
{
	int len = strlen(newbit) + 1;
	unsigned required = builder->basepos + len;
	if (required > builder->alloc)
	{
		required *= 2;
		char* newbuf = realloc(builder->buf, required);
		if (!newbuf) return 0;
		builder->buf = newbuf;
		builder->alloc = required;
	}
	/* also copies the NULL terminator */
	memcpy(builder->buf + builder->basepos, newbit, len);
	return 1;
}

nnc_result nnc_vfs_link_directory(nnc_vfs_directory_node* dir, const char* dirname, char* (*transform)(const char*, void*), void* udata)
{
	nnc_vfs_directory_node* deeper_dir;
	nnc_result ret = NNC_R_OK;
	const char* final_name;
	char* transformed = NULL;

	struct filename_builder fnb;
	if (!fnbuild_setbase(&fnb, dirname))
		return NNC_R_NOMEM;

#if DIRENT_API
	DIR* d = opendir(dirname);
	if (!d)
	{
		fnbuild_free(&fnb);
		return NNC_R_FAIL_OPEN;
	}

	struct dirent* ent;
	while ((ent = readdir(d)))
	{
		if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
			continue; /* these need to be skipped */

		if (transform)
		{
			transformed = transform(ent->d_name, udata);
			final_name = transformed;
			/* no need to free later on if the pointer is the same */
			if (transformed == ent->d_name)
				transformed = NULL;
		}
		else final_name = ent->d_name;

		/* transform() returning NULL means to skip this file */
		if (!final_name) continue;

		if (!fnbuild(&fnb, ent->d_name))
		{
			ret = NNC_R_NOMEM;
			goto out;
		}

		/* TODO: ent->d_type is an extension which not all libcs support! */
		unsigned char type = ent->d_type;
		if (type == DT_UNKNOWN || type == DT_LNK)
		{
			struct stat st;
			/* we want to resolve the link */
			if (stat(fnb.buf, &st) != 0)
			{
				ret = NNC_R_OS;
				goto out;
			}
			if (S_ISDIR(st.st_mode))      type = DT_DIR;
			else if (S_ISREG(st.st_mode)) type = DT_REG;
		}

		switch (type)
		{
		case DT_DIR:
			/* we need to add this directory and then recurse into it */
			TRYLBL(nnc_vfs_add_directory(dir, final_name, &deeper_dir), out);
			TRYLBL(nnc_vfs_link_directory(deeper_dir, fnb.buf, transform, udata), out);
			break;
		case DT_REG:
			TRYLBL(nnc_vfs_add_file(dir, final_name, NNC_VFS_FILE(fnb.buf)), out);
			break;
			/* anything that's not a file or directory we can safely ignore */
		default:
			break;
		}

		free(transformed);
		transformed = NULL;
	}
#elif WINDOWS_API
	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	/* just windows things */
	if (!fnbuild(&fnb, "*"))
	{
		fnbuild_free(&fnb);
		return NNC_R_NOMEM;
	}

	hFind = FindFirstFileA(fnb.buf, &ffd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		fnbuild_free(&fnb);
		return NNC_R_FAIL_OPEN;
	}

	do {
		if (strcmp(ffd.cFileName, ".") == 0 || strcmp(ffd.cFileName, "..") == 0)
			continue; /* these need to be skipped */

		if (transform)
		{
			transformed = transform(ffd.cFileName, udata);
			final_name = transformed;
			/* no need to free later on if the pointer is the same */
			if (transformed == ffd.cFileName)
				transformed = NULL;
		}
		else final_name = ffd.cFileName;

		/* transform() returning NULL means to skip this file */
		if (!final_name) continue;

		if (!fnbuild(&fnb, ffd.cFileName))
		{
			ret = NNC_R_NOMEM;
			goto out;
		}

		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			TRYLBL(nnc_vfs_add_directory(dir, final_name, &deeper_dir), out);
			TRYLBL(nnc_vfs_link_directory(deeper_dir, fnb.buf, transform, udata), out);
		}
		else /* file */
		{
			TRYLBL(nnc_vfs_add_file(dir, final_name, NNC_VFS_FILE(fnb.buf)), out);
		}
	} while (FindNextFileA(hFind, &ffd) != 0);

	if (GetLastError() != ERROR_NO_MORE_FILES)
		ret = NNC_R_FAIL_READ;
#else
	ret = NNC_R_UNSUPPORTED;
#endif

out:
	free(transformed);
	fnbuild_free(&fnb);
#if DIRENT_API
	closedir(d);
#elif WINDOWS_API
	FindClose(hFind);
#endif
	return ret;
}

static nnc_vfs_node* nnc_vfs_search_array(nnc_vfs_node* nodes, unsigned nodeslen, const char* name, size_t namelen)
{
	for (unsigned i = 0; i < nodeslen; ++i)
		if (strlen(nodes[i].vname) == namelen && memcmp(nodes[i].vname, name, namelen) == 0)
			return &nodes[i];
	return NULL;
}

nnc_vfs_directory_node* nnc_vfs_search_dirname(nnc_vfs_directory_node* node, const char* path, const char** last_component, size_t* last_component_len)
{
	while (*path == '/')
		++path;
	const char* last_comp = strrchr(path, '/'), * next_slash;
	/* TODO: Test for paths like /hello/my-dir/dir/; last_comp should be at the / before dir, not the actual last one */
	/* there is only one component; we are already in the dirname:
	 *   /hello  -> dirname is /; root
	 *    hello  -> dirname is /; root */
	if (!last_comp)
	{
		last_comp = path;
		goto out;
	}
	++last_comp;

	size_t namelen;
	while (path != last_comp)
	{
		next_slash = strchr(path, '/');
		/* this situation should be impossible, as we do not process the last slash;
		 * let's just return NULL to indicate failure */
		if (!next_slash) return NULL;
		namelen = next_slash - path;
		node = (nnc_vfs_directory_node*)nnc_vfs_search_array((nnc_vfs_node*)node->directory_children, node->dircount, path, namelen);
		if (!node) return NULL;
		path = next_slash;
		while (*path == '/')
			++path;
	}

out:
	if (last_component && last_component_len)
	{
		*last_component = last_comp;
		*last_component_len = strlen(last_comp);
	}
	return node;
}

nnc_vfs_file_node* nnc_vfs_file_by_name(nnc_vfs_directory_node* root_dir, const char* name)
{
	const char* last_component; size_t last_component_len;
	nnc_vfs_directory_node* last_node = nnc_vfs_search_dirname(root_dir, name, &last_component, &last_component_len);
	if (!last_node) return NULL;
	return (nnc_vfs_file_node*)nnc_vfs_search_array((nnc_vfs_node*)last_node->file_children, last_node->filecount, last_component, last_component_len);
}

nnc_vfs_directory_node* nnc_vfs_directory_by_name(nnc_vfs_directory_node* root_dir, const char* name)
{
	const char* last_component; size_t last_component_len;
	nnc_vfs_directory_node* last_node = nnc_vfs_search_dirname(root_dir, name, &last_component, &last_component_len);
	if (!last_node) return NULL;
	return (nnc_vfs_directory_node*)nnc_vfs_search_array((nnc_vfs_node*)last_node->directory_children, last_node->dircount, last_component, last_component_len);
}

static result vfs_stream_read(nnc_vfs_stream* self, u8* buf, u32 max, u32* totalRead) { return self->substream->funcs->read(self->substream, buf, max, totalRead); }
static result vfs_stream_seek_abs(nnc_vfs_stream* self, u32 pos) { return self->substream->funcs->seek_abs(self->substream, pos); }
static result vfs_stream_seek_rel(nnc_vfs_stream* self, u32 pos) { return self->substream->funcs->seek_rel(self->substream, pos); }
static u32 vfs_stream_size(nnc_vfs_stream* self) { return self->substream->funcs->size(self->substream); }
static u32 vfs_stream_tell(nnc_vfs_stream* self) { return self->substream->funcs->tell(self->substream); }
static void vfs_stream_close(nnc_vfs_stream* self)
{
	if (self->flags & NNC_VFS_STREAM_RECURSIVE_CLOSE)
		self->substream->funcs->close(self->substream);
	if (self->flags & NNC_VFS_STREAM_FREE_ON_CLOSE)
		free(self->substream);
}

static const nnc_rstream_funcs vfs_stream_funcs = {
	.read = (nnc_read_func)vfs_stream_read,
	.seek_abs = (nnc_seek_abs_func)vfs_stream_seek_abs,
	.seek_rel = (nnc_seek_rel_func)vfs_stream_seek_rel,
	.size = (nnc_size_func)vfs_stream_size,
	.close = (nnc_close_func)vfs_stream_close,
	.tell = (nnc_tell_func)vfs_stream_tell,
};

void nnc_vfs_open_stream(nnc_vfs_stream* self, nnc_rstream* substream, int flags)
{
	self->funcs = &vfs_stream_funcs;
	self->substream = substream;
	self->flags = flags;
}

nnc_result nnc_vfs_open_node(nnc_vfs_file_node* node, nnc_vfs_stream* res)
{
	return node->generator->make_reader(node->data, res);
}

u64 nnc_vfs_node_size(nnc_vfs_file_node* node)
{
	return node->generator->node_size(node->data);
}

struct nnc_filegen_data {
	/* GCC complains if this does not have a size, but in reality it's dynamically sized */
	char path[1];
};

static nnc_result nnc_filegen_initialize(nnc_vfs_generator_data* udata, va_list va)
{
	*udata = strdup(va_arg(va, const char*));
	return *udata ? NNC_R_OK : NNC_R_NOMEM;
}

static nnc_result nnc_filegen_make_reader(nnc_vfs_generator_data udata, nnc_vfs_stream* out)
{
	struct nnc_filegen_data* data = (struct nnc_filegen_data*)udata;
	nnc_file* reader = malloc(sizeof(nnc_file));
	if (!reader) return NNC_R_NOMEM;
	nnc_result res = nnc_file_open(reader, data->path);
	if (res != NNC_R_OK) free(reader);
	nnc_vfs_open_stream(out, (nnc_rstream*)reader, NNC_VFS_STREAM_FULL_CLOSE);
	return res;
}

static nnc_u64 nnc_filegen_node_size(nnc_vfs_generator_data udata)
{
	struct nnc_filegen_data* data = (struct nnc_filegen_data*)udata;
#if NNC_PLATFORM_UNIX || NNC_PLATFORM_3DS
	/* We can use stat here, which is a bit more efficient */
	struct stat st;
	return lstat(data->path, &st) == 0 ? st.st_size : 0;
#else
	/* We can use the C FILE api as a generic fallback */
	FILE* f = fopen(data->path, "rb");
	if (!f) return 0;
	u64 ret = get_file_size(f, 0);
	fclose(f);
	return ret;
#endif
}

static void nnc_filegen_delete_data(nnc_vfs_generator_data udata)
{
	free(udata);
}

const nnc_vfs_reader_generator nnc__internal_vfs_generator_file = {
	.initialize = nnc_filegen_initialize,
	.make_reader = nnc_filegen_make_reader,
	.node_size = nnc_filegen_node_size,
	.delete_data = nnc_filegen_delete_data,
};

struct rgen_data {
	nnc_rstream* substream;
	int flags;
};

static nnc_result nnc_rgen_initialize(nnc_vfs_generator_data* out_udata, va_list params)
{
	struct rgen_data* data = malloc(sizeof(struct rgen_data));
	if (!data) return NNC_R_NOMEM;
	data->substream = va_arg(params, nnc_rstream*);
	data->flags = va_arg(params, int);
	*out_udata = data;
	return NNC_R_OK;
}

static nnc_result nnc_rgen_make_reader(nnc_vfs_generator_data udata, nnc_vfs_stream* out_stream)
{
	struct rgen_data* data = (struct rgen_data*)udata;
	nnc_vfs_open_stream(out_stream, data->substream, NNC_VFS_STREAM_NONE);
	return nnc_rs_seek_abs(data->substream, 0);
}

static size_t nnc_rgen_node_size(nnc_vfs_generator_data udata)
{
	struct rgen_data* data = (struct rgen_data*)udata;
	return nnc_rs_size(data->substream);
}

static void nnc_rgen_delete_data(nnc_vfs_generator_data udata)
{
	struct rgen_data* data = (struct rgen_data*)udata;
	if (data->flags & NNC_VFS_STREAM_RECURSIVE_CLOSE)
		nnc_rs_close(data->substream);
	if (data->flags & NNC_VFS_STREAM_FREE_ON_CLOSE)
		free(data->substream);
	free(data);
}

const nnc_vfs_reader_generator nnc__internal_vfs_generator_reader = {
	.initialize = nnc_rgen_initialize,
	.make_reader = nnc_rgen_make_reader,
	.node_size = nnc_rgen_node_size,
	.delete_data = nnc_rgen_delete_data,
};

static nnc_result nnc_rdcpygen_initialize(nnc_vfs_generator_data* out_udata, va_list params)
{
	struct rgen_data* data = malloc(sizeof(struct rgen_data));
	if (!data) return NNC_R_NOMEM;

	nnc_rstream* substream_local = va_arg(params, nnc_rstream*);
	size_t objsize = va_arg(params, size_t);
	data->flags = va_arg(params, int);

	data->substream = malloc(objsize);
	if (!data->substream) return NNC_R_NOMEM;
	memcpy(data->substream, substream_local, objsize);

	*out_udata = data;
	return NNC_R_OK;
}

static void nnc_rdcpygen_delete_data(nnc_vfs_generator_data udata)
{
	struct rgen_data* data = (struct rgen_data*)udata;
	if (data->flags & NNC_VFS_STREAM_RECURSIVE_CLOSE)
		data->substream->funcs->close(data->substream);
	free(data->substream);
	free(data);
}


const nnc_vfs_reader_generator nnc__internal_vfs_generator_reader_copy = {
	.initialize = nnc_rdcpygen_initialize,
	.make_reader = nnc_rgen_make_reader,
	.node_size = nnc_rgen_node_size,
	.delete_data = nnc_rdcpygen_delete_data,
};

//

nnc_result nnc_copy(nnc_rstream* from, nnc_wstream* to, u32* copied)
{
	u8 block[BLOCK_SZ];
	u32 left = NNC_RS_PCALL0(from, size), next, actual;
	result ret;
	TRY(NNC_RS_PCALL(from, seek_abs, 0));

	if (copied) *copied = left;
	while (left != 0)
	{
		next = MIN(left, BLOCK_SZ);
		TRY(NNC_RS_PCALL(from, read, block, next, &actual));
		if (actual != next) return NNC_R_TOO_SMALL;
		TRY(NNC_WS_PCALL(to, write, block, next));
		left -= next;
	}

	return NNC_R_OK;
}

nnc_result nnc_write_padding(nnc_wstream* self, nnc_u32 count)
{
	u8 buffer[4096];
	/* if count < sizeof(buffer) it makes no sense to completely fill it with 0s */
	memset(buffer, 0x00, MIN(count, sizeof(buffer)));

	nnc_u32 left = count, to_do;
	nnc_result ret;

	while (left)
	{
		to_do = MIN(left, sizeof(buffer));
		TRY(self->funcs->write(self, buffer, to_do));
		left -= to_do;
	}

	return NNC_R_OK;
}

/* wrapper funcs */

nnc_result nnc_rs_read_(nnc_rstream* rs, nnc_u8* buf, nnc_u32 max, nnc_u32* totalRead)
{
	if (!rs->funcs) return NNC_R_NOT_OPEN;
	u32 nread;
	nnc_result res = rs->funcs->read(rs, buf, max, &nread);
	if (res != NNC_R_OK) return res;
	if (totalRead) *totalRead = nread;
	else if (/* !totalRead && */ nread != max) return NNC_R_TOO_SMALL;
	return NNC_R_OK;
}

nnc_result nnc_rs_read_at_(nnc_rstream* rs, nnc_u32 pos, nnc_u8* buf, nnc_u32 max, nnc_u32* totalRead)
{
	nnc_result res = nnc_rs_seek_abs(rs, pos);
	if (res != NNC_R_OK) return res;
	return nnc_rs_read(rs, buf, max, totalRead);
}

nnc_result nnc_rs_seek_abs_(nnc_rstream* rs, nnc_u32 pos)
{
	if (!rs->funcs) return NNC_R_NOT_OPEN;
	if (nnc_rs_tell(rs) == pos) return NNC_R_OK;
	return rs->funcs->seek_abs(rs, pos);
}

nnc_result nnc_rs_seek_rel_(nnc_rstream* rs, nnc_u32 pos)
{
	if (!rs->funcs) return NNC_R_NOT_OPEN;
	if (pos == 0) return NNC_R_OK;
	return rs->funcs->seek_rel(rs, pos);
}

nnc_u32 nnc_rs_size_(nnc_rstream* rs) { return rs->funcs ? rs->funcs->size(rs) : 0; }
nnc_u32 nnc_rs_tell_(nnc_rstream* rs) { return rs->funcs ? rs->funcs->tell(rs) : 0; }

void nnc_rs_close_(nnc_rstream* rs)
{
	if (rs->funcs)
	{
		rs->funcs->close(rs);
		rs->funcs = NULL;
	}
}