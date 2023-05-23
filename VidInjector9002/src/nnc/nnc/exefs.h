/** \file  exefs.h
 *  \brief Functions relating to ExeFS.
 *  \see   https://www.3dbrew.org/wiki/ExeFS
 */
#ifndef inc_nnc_exefs_h
#define inc_nnc_exefs_h

#include "stream.h"
#include "crypto.h"
#include "base.h"
NNC_BEGIN

/** Maximum files an exefs has. */
#define NNC_EXEFS_MAX_FILES 10

/** Size of raw ExeFS header. You don't need this. */
#define NNC_EXEFS_HEADER_SIZE 0x200

/** Exefs file header. */
typedef struct nnc_exefs_file_header {
	char name[9];         ///< Filename, NULL-terminated.
	nnc_u32 offset;       ///< Offset in exefs.
	nnc_u32 size;         ///< File size.
	nnc_sha256_hash hash; ///< Hash of the content.
} nnc_exefs_file_header;

/** Returns whether or not a file header is used, the check is strlen(name) == 0. */
bool nnc_exefs_file_in_use(nnc_exefs_file_header *fh);

/** \brief          Read the header of an exefs.
 *  \param rs       Stream to read exefs from.
 *  \param headers  Array of size \ref NNC_EXEFS_MAX_FILES.
 *  \param size     (optional) Amount of used files.
 */
nnc_result nnc_read_exefs_header(nnc_rstream *rs, nnc_exefs_file_header *headers, nnc_u8 *size);

/** \brief          Searches the exefs for \p name.
 *  \return         Returns the appropriate exefs header index of -1 if none was found.
 *  \param name     Name to search for. Maximum length is 8.
 *  \param headers  Array of size \ref NNC_EXEFS_MAX_FILES returned by \ref nnc_read_exefs_header.
 */
nnc_i8 nnc_find_exefs_file_index(const char *name, nnc_exefs_file_header *headers);

/** \brief         Open a subview for a file.
 *  \param rs      Stream to open in.
 *  \param header  File to open.
 *  \param sv      Output stream.
 *  \note          If the ExeFS comes from an NCCH use \ref nnc_ncch_exefs_subview.
 */
void nnc_exefs_subview(nnc_rstream *rs, nnc_subview *sv, nnc_exefs_file_header *header);

/** \brief      Write an ExeFS.
 *  \param vfs  VFS to use as file source.
 *  \param ws   Stream to write to.
 *  \note       The VFS may not contain any directories and there are at the maximum \ref NNC_EXEFS_MAX_FILES files allowed.
 */
nnc_result nnc_write_exefs(nnc_vfs *vfs, nnc_wstream *ws);

NNC_END
#endif

