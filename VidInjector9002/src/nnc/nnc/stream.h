/** \file   stream.h
 *  \brief  Functions relating to stream io.
 */
#ifndef inc_nnc_stream_h
#define inc_nnc_stream_h

#include "base.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
NNC_BEGIN

/** \{
 *  \anchor read
 *  \name   Read Stream
 */

/** \brief Call \ref nnc_rstream function on pointer. */
#define NNC_RS_PCALL(obj, func, ...) ((nnc_rstream *) obj)->funcs->func((nnc_rstream *) (obj), __VA_ARGS__)
/** \brief Call \ref nnc_rstream function. */
#define NNC_RS_CALL(obj, func, ...) NNC_RS_PCALL(&obj, func, __VA_ARGS__)
/** \brief Cast stream-like type to \ref nnc_rstream pointer for passing to other functions. */
#define NNC_RSP(obj) ((nnc_rstream *) (obj))
/** \brief Call a \ref nnc_rstream function without arguments on a pointer. */
#define NNC_RS_PCALL0(obj, func) ((nnc_rstream *) obj)->funcs->func((nnc_rstream *) (obj))
/** \brief Call a \ref nnc_rstream function without arguments. */
#define NNC_RS_CALL0(obj, func) NNC_RS_PCALL0(&obj, func)

struct nnc_rstream;
/** Read from stream. */
typedef nnc_result (*nnc_read_func)(struct nnc_rstream *self, nnc_u8 *buf, nnc_u32 max,
		nnc_u32 *totalRead);
/** Seek to absolute position in stream. */
typedef nnc_result (*nnc_seek_abs_func)(struct nnc_rstream *self, nnc_u32 pos);
/** Seek to relative to current position in stream. */
typedef nnc_result (*nnc_seek_rel_func)(struct nnc_rstream *self, nnc_u32 pos);
/** Get total size of stream. */
typedef nnc_u32 (*nnc_size_func)(struct nnc_rstream *self);
/** Close/free the stream */
typedef void (*nnc_close_func)(struct nnc_rstream *self);
/** Get current position in stream */
typedef nnc_u32 (*nnc_tell_func)(struct nnc_rstream *self);

/** All functions a stream should have */
typedef struct nnc_rstream_funcs {
	nnc_read_func read;
	nnc_seek_abs_func seek_abs;
	nnc_seek_rel_func seek_rel;
	nnc_size_func size;
	nnc_close_func close;
	nnc_tell_func tell;
} nnc_rstream_funcs;

/** Struct containing just a func table which should be
 *  used as parameter when taking a stream. */
typedef struct nnc_rstream {
	const nnc_rstream_funcs *funcs;
	/* user-data */
} nnc_rstream;

/** Stream for a file using the standard FILE. */
typedef struct nnc_file {
	const nnc_rstream_funcs *funcs;
	nnc_u32 size;
	FILE *f;
	nnc_u8 flags;
} nnc_file;

/** Stream for memory buffer. */
typedef struct nnc_memory {
	const nnc_rstream_funcs *funcs;
	nnc_u32 size;
	nnc_u32 pos;
	union nnc_memory_un {
		const void *ptr_const;
		void *ptr;
	} un;
} nnc_memory;

/** Stream for reading a specific part of another stream */
typedef struct nnc_subview {
	const nnc_rstream_funcs *funcs;
	nnc_rstream *child;
	nnc_u32 size;
	nnc_u32 off;
	nnc_u32 pos;
	nnc_u8 flags;
} nnc_subview;

/** \brief       Create a new file stream.
 *  \param self  Output stream.
 *  \param name  Filename to open. */
nnc_result nnc_file_open(nnc_file *self, const char *name);

/** \brief       Create a new memory stream.
 *  \param self  Output stream.
 *  \param ptr   Pointer to memory.
 *  \param size  Size of memory. */
void nnc_mem_open(nnc_memory *self, const void *ptr, nnc_u32 size);

/** \brief       Create a new memory stream that free()s the pointer when closed.
 *  \param self  Output stream.
 *  \param ptr   Pointer to memory.
 *  \param size  Size of memory. */
void nnc_mem_own_open(nnc_memory *self, void *ptr, nnc_u32 size);

/** \brief        Create a new subview stream.
 *  \param self   Output stream.
 *  \param child  Child stream.
 *  \param off    Starting offset in \p child.
 *  \param len    Length of data in \p child.
 *  \note         Closing this stream has no effect; the child stream is not closed, that is, unless #nnc_subview_delete_on_close is called.
 */
void nnc_subview_open(nnc_subview *self, nnc_rstream *child, nnc_u32 off, nnc_u32 len);

/** \brief       This function makes the substream close and free its child stream when it is closed.
 *  \param self  The stream to enable this functionality on.
 *  \warning     A subview marked with this function MUST be closed!
 */
void nnc_subview_delete_on_close(nnc_subview *self);

/** \} */

/** \{
 *  \anchor write
 *  \name   Write Stream
 */

/** \brief Call \ref nnc_wstream function on pointer. */
#define NNC_WS_PCALL(obj, func, ...) ((nnc_wstream *) obj)->funcs->func((nnc_wstream *) (obj), __VA_ARGS__)
/** \brief Call \ref nnc_wstream function. */
#define NNC_WS_CALL(obj, func, ...) NNC_WS_PCALL(&obj, func, __VA_ARGS__)
/** \brief Cast stream-like type to \ref nnc_wstream pointer for passing to other functions. */
#define NNC_WSP(obj) ((nnc_wstream *) (obj))
/** \brief Call a \ref nnc_wstream function without arguments on a pointer. */
#define NNC_WS_PCALL0(obj, func) ((nnc_wstream *) obj)->funcs->func((nnc_wstream *) (obj))
/** \brief Call a \ref nnc_wstream function without arguments. */
#define NNC_WS_CALL0(obj, func) NNC_WS_PCALL0(&obj, func)

struct nnc_wstream;
typedef nnc_result (*nnc_write_func)(struct nnc_wstream *self, nnc_u8 *buf, nnc_u32 size);
typedef nnc_result (*nnc_wclose_func)(struct nnc_wstream *self);
typedef nnc_result (*nnc_wseek_func)(struct nnc_wstream *self, nnc_u32 abspos);
typedef nnc_u32    (*nnc_wtell_func)(struct nnc_wstream *self);
typedef nnc_result (*nnc_wsubreadstream_func)(struct nnc_wstream *self, nnc_subview *out, nnc_u32 start, nnc_u32 amount);

typedef struct nnc_wstream_funcs {
	nnc_write_func write;
	nnc_wclose_func close;
	nnc_wseek_func seek; ///< Note that this may be NULL in streams that do not support seeking.
	nnc_wtell_func tell;
	nnc_wsubreadstream_func subreadstream; ///< Note that this may be NULL in streams that do not support readback.
} nnc_wstream_funcs;

typedef struct nnc_wstream {
	const nnc_wstream_funcs *funcs;
	/* user-data */
} nnc_wstream;

typedef struct nnc_wfile {
	const nnc_wstream_funcs *funcs;
	nnc_u32 off;
	FILE *f;
} nnc_wfile;

typedef struct nnc_header_saver {
	const nnc_wstream_funcs *funcs;
	nnc_wstream *child;
	nnc_u32 count, pos, start;
	nnc_u8 *buffer;
} nnc_header_saver;

/** \brief       Opens a file for writing.
 *  \param self  Output write stream.
 *  \param name  Filename to open.
 */
nnc_result nnc_wfile_open(nnc_wfile *self, const char *name);

/** \brief        This stream saves the first few bytes of a write stream.
 *  \param self   Output header saver.
 *  \param child  Child stream that when written to the first `count` bytes are saved of.
 *  \param count  Amount of header bytes to save.
 *  \note         The buffered data is freed when this stream is closed.
 *  \note         This stream only supports seeking if the child stream does too.
 *  \note         The start of the header is determined by the current position in the write stream.
 */
nnc_result nnc_open_header_saver(nnc_header_saver *self, nnc_wstream *child, nnc_u32 count);

/** \} */

/** \{
 *  \anchor vfs
 *  \name   Virtual FileSystem
 */

/** \brief VFS vfile adding parameters for adding a real file */
#define NNC_VFS_FILE(filename) &nnc__internal_vfs_generator_file, (filename)
/** \brief VFS vfile parameters for adding a read stream */
#define NNC_VFS_READER(rs) &nnc__internal_vfs_generator_reader, (rs)
/** \brief VFS vfile parameters for adding a subview stream copy */
#define NNC_VFS_SUBVIEW(sv) &nnc__internal_vfs_generator_subview, (sv)
/** \brief Identitiy map in a directory link, that is, preserve the names from the filesystem */
#define nnc_vfs_identity_transform  NULL /* magic from the function itself */
/** \brief An alternative name for nnc_rstream used in the VFS functions */
#define nnc_vfs_stream nnc_rstream

typedef void *nnc_vfs_generator_data;

struct nnc_vfs;

typedef struct nnc_vfs_reader_generator {
	nnc_result (*initialize)(nnc_vfs_generator_data *udata, va_list va);
	nnc_result (*make_reader)(nnc_vfs_generator_data udata, nnc_vfs_stream **out);
	nnc_u64 (*node_size)(nnc_vfs_generator_data udata);
	void (*delete_data)(nnc_vfs_generator_data udata);
} nnc_vfs_reader_generator;

typedef struct nnc_vfs_file_node {
	char *vname;
	const nnc_vfs_reader_generator *generator;
	nnc_vfs_generator_data data;
} nnc_vfs_file_node;

typedef struct nnc_vfs_directory_node {
	char *vname;
	struct nnc_vfs_directory_node *directory_children;
	nnc_vfs_file_node *file_children;
	struct nnc_vfs *associated_vfs;
	unsigned dircount, filecount;
	unsigned diralloc, filealloc;
} nnc_vfs_directory_node;

typedef struct nnc_vfs_node {
	char *vname;
} nnc_vfs_node;

typedef struct nnc_vfs {
	nnc_vfs_directory_node root_directory;
	/* speeds up romfs processing by removing a tree walk requirement */
	unsigned totaldirs;  /* including the root directory */
	unsigned totalfiles;
} nnc_vfs;

/** \brief               Creates a new VFS.
 *  \param vfs           Output VFS.
 *  \note                You must free the memory allocated by this function by a matching call to \ref nnc_vfs_free. */
nnc_result nnc_vfs_init(nnc_vfs *vfs);

/** \brief      free()s memory in use by a VFS.
 *  \param vfs  The VFS to free
 */
void nnc_vfs_free(nnc_vfs* vfs);

/** \brief            Adds a new virtual file to a VFS directory.
 *  \param dir        Directory to add to.
 *  \param vname      The filename the file should have *in the VFS directory*.
 *  \param generator  Usually filled out by \ref NNC_VFS_FILE or \ref NNC_VFS_READER, it is actually the generator that generates a reader (see \ref nnc_vfs_reader_generator).
 *  \param ...        Parameters for the generator.
 */
nnc_result nnc_vfs_add_file(nnc_vfs_directory_node *dir, const char *vname, const nnc_vfs_reader_generator *generator, ... /* generator parameters */);

/** \brief          Adds a new virtual directory to another VFS directory.
 *  \param dir      Directory to add to.
 *  \param vname    Virtual directory name, without a trailing slash.
 *  \param new_dir  An optional pointer that, if passed, will be filled with another pionter to the newly created directory.
 *  \note           To link an entire real directory tree use \ref nnc_vfs_link_directory.
 */
nnc_result nnc_vfs_add_directory(nnc_vfs_directory_node *dir, const char *vname, nnc_vfs_directory_node **new_dir);

/** \brief            Add all files in a real directory tree to a directory in the VFS.
 *  \param dir        The directory to link into.
 *  \param dirname    The real directory path to link.
 *  \param transform  This function is responsible for transforming the real names of files and directories in the linked directory.\n
 *                    This function must return a new heap allocated string, the original input string, or, to not add the file, NULL.\n
 *                    To do no transformations at all (identity map) pass \ref nnc_vfs_identity_transform instead.\n
 *                    The prototype of this callback is `char *transform(const char *original_name, void *udata);`
 *  \param udata      This pointer will be passed to all calls of `transform` as the `udata` parameter.
 *  \note             This feature is currently unavailable on windows.
 */
nnc_result nnc_vfs_link_directory(nnc_vfs_directory_node *dir, const char *dirname, char *(*transform)(const char *, void *), void *udata);

/** \brief       Open a node in the VFS for reading.
 *  \param node  The node to open
 *  \param res   The output read stream.
 *  \note        The output read stream must be freed up with #nnc_vfs_close_node.
 */
nnc_result nnc_vfs_open_node(nnc_vfs_file_node *node, nnc_vfs_stream **res);

/** \brief         Close the read stream that is opened by #nnc_vfs_open_node.
 *  \param reader  The stream to close.
 */
void nnc_vfs_close_node(nnc_vfs_stream *reader);

/** \brief       Get the file size of a node in the VFS.
 *  \param node  Node to get size of.
 */
nnc_u64 nnc_vfs_node_size(nnc_vfs_file_node *node);

/* \cond INTERNAL */
extern const nnc_vfs_reader_generator nnc__internal_vfs_generator_subview;
extern const nnc_vfs_reader_generator nnc__internal_vfs_generator_reader;
extern const nnc_vfs_reader_generator nnc__internal_vfs_generator_file;
/* \endcond */

/** \} */

/** \brief         Copy the contents of a stream to another.
 *  \param from    Source read stream.
 *  \param to      Destination write stream.
 *  \param copied  (Optional) Output for the amount of copied bytes.
 */
nnc_result nnc_copy(nnc_rstream *from, nnc_wstream *to, nnc_u32 *copied);

/** \brief        Writes `count` 0x00 bytes as padding.
 *  \param ws     The stream to write padding to.
 *  \param count  The amount of 0x00 bytes to write.
 */
nnc_result nnc_write_padding(nnc_wstream *ws, nnc_u32 count);

NNC_END
#endif