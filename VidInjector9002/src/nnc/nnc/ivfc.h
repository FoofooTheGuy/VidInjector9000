/** \file   ivfc.h
 *  \brief  Functions relating to the Integrity VeriFication Container, used in RomFS, DISA and DIFF.
 */
#ifndef inc_nnc_ivfc_h
#define inc_nnc_ivfc_h

#include "base.h"
#include "stream.h"
#include "crypto.h"
NNC_BEGIN

/** Maximum of levels possible in an IVFC. */
#define NNC_IVFC_MAX_LEVELS  4

/** \brief Commonly used values for the IVFC header ID field. */
enum nnc_ivfc_id {
	NNC_IVFC_ID_ROMFS = 0x10000, ///< Used in RomFS IVFCs.
	NNC_IVFC_ID_SAVE  = 0x20000, ///< Used in DISA and DIFF partition IVFCs.
};

/** \brief Commonly used values for the level count in an IVFC container. */
enum nnc_ivfc_levels {
	NNC_IVFC_LEVELS_ROMFS = 3, ///< Used in RomFS IVFCs.
	NNC_IVFC_LEVELS_SAVE  = 4, ///< Used in DISA and DIFF partition IVFCs.
};

/** \brief Commonly used values for the block size in an IVFC container. */
enum nnc_ivfc_blocksize {
	NNC_IVFC_BLOCKSIZE_ROMFS = 4096, ///< Used in RomFS IVFCs.
};

typedef struct nnc_ivfc_level_descriptor {
	nnc_u64 logical_offset;  ///< The logical (not real!) offset of this level.
	nnc_u64 size;            ///< The size of level.
	nnc_u32 block_size_log2; ///< The log2 result of the block size (see #nnc_log2 and #nnc_pow2).
} nnc_ivfc_level_descriptor;

typedef struct nnc_ivfc {
	nnc_u32 id;                                           ///< IVFC ID, see #nnc_ivfc_id.
	nnc_u32 l0_size;                                      ///< Size of level 0, or, the master hash size.
	nnc_u32 number_levels;                                ///< Amount of levels in this file, excluding level 0.
	nnc_ivfc_level_descriptor level[NNC_IVFC_MAX_LEVELS]; ///< Level descriptors, excluding level 0.
} nnc_ivfc;

typedef struct nnc_ivfc_writer {
	const nnc_wstream_funcs *funcs;
	nnc_wstream *child;
	nnc_sha256_hash *block_hashes;
	nnc_sha256_incremental_hash current_hash;
	nnc_u64 final_lv_size;
	nnc_u32 current_hashed_size;
	nnc_u32 blocks_allocated;
	nnc_u32 blocks_hashed;
	nnc_u32 id, levels;
	nnc_u32 block_size; /* not log2! */
	nnc_u32 header_pos;
} nnc_ivfc_writer;

/** \brief                  Reads the header of an IVFC.
 *  \param rs               Stream to read from.
 *  \param ivfc             Output IVFC.
 *  \param expected_levels  Levels to load in this IVFC file. If it is 0 then the amount is guessed.
 */
nnc_result nnc_read_ivfc_header(nnc_rstream *rs, nnc_ivfc *ivfc, nnc_u32 expected_levels);

/** \brief  This function opens a write stream object to write an IVFC-wrapped format.\n
 *
 *  This stream automatically manages the IVFC header and all hashes, including the master hash.
 *  When this stream is opened space for the header and master hash are allocated and
 *  filled with zeros. When this stream is written to the data is hashed and cached
 *  for when it is time to write the other levels. All hashes and the IVFC header
 *  are filled when this stream is closed, the substream stays open.
 *  \param self        The output IVFC writer stream.
 *  \param child       The underlaying stream to write to.
 *  \param levels      The amount of levels to write, no more than #NNC_IVFC_MAX_LEVELS, see #nnc_ivfc_levels.
 *  \param id          IVFC ID, see #nnc_ivfc_id.
 *  \param block_size  The (power of 2!) block size to use, see #nnc_ivfc_blocksize.
 *  \note              This stream requires the `seek` function in `child`
 */
nnc_result nnc_open_ivfc_writer(nnc_ivfc_writer *self, nnc_wstream *child, nnc_u32 levels, nnc_u32 id, nnc_u32 block_size);

/** \brief       Frees memory in use by an IVFC writer without writing out the rest of the IVFC file.
 *  \param self  The writer to free.
 */
void nnc_ivfc_abort_write(nnc_ivfc_writer *self);

NNC_END
#endif

