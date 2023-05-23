
#include "nnc/exheader.h"
#include <string.h>
#include "./internal.h"

#include "./exheader_syscall_tab.inc" /* forgive me for my sins */


static result do_kernel_caps(nnc_exheader *exh, nnc_u8 *buf)
{
	memset(exh->allowed_syscalls, 0, sizeof(exh->allowed_syscalls));
	exh->kernel_release_minor = exh->kernel_release_major =
		exh->max_handles = exh->kflags = 0;

	int mmap_count = 0;

	for(int i = 0; i < 28; ++i)
	{
		u32 desc = LE32P(&buf[0x370 + 0x4 * i]);
		/* interrupt */
		if((desc & 0xF0000000) == 0xe0000000)
		{
			return NNC_R_UNSUPPORTED;
		}
		/* syscall */
		else if((desc & 0xF8000000) == 0xF0000000)
		{
			u32 i = (desc >> 24) & 0x7;
			if(i > 8) return NNC_R_CORRUPT;
			exh->allowed_syscalls[i] = desc & 0x00FFFFFF;
		}
		/* kernel release */
		else if((desc & 0xFE000000) == 0xFC000000)
		{
			exh->kernel_release_minor = (desc >> 0) & 0xFF;
			exh->kernel_release_major = (desc >> 8) & 0xFF;
		}
		/* handle table size */
		else if((desc & 0xFF000000) == 0xFE000000)
			exh->max_handles = desc & 0x3FF;
		else if((desc & 0xFF800000) == 0xFF000000)
			exh->kflags = desc & 0xFFFF; /* in reality only 13 bits are in use */
#define DO_MMAP(base_type, f1, f2, f3) \
			else if((desc & f1) == f2) \
			{ \
				if(mmap_count == NNC_MAX_MMAPS) return NNC_R_CORRUPT; \
				nnc_u32 is_rw = desc & (1 << 20); \
				exh->memory_mappings[mmap_count].type = is_rw \
					? base_type##_RO : base_type##_RW; \
				exh->memory_mappings[mmap_count].range_start = (desc & f3) << 12; \
				if(++i == 28) return NNC_R_CORRUPT; \
				desc = LE32P(&buf[0x370 + 0x4 * i]); \
				if((desc & f1) != f2 || (desc & (1 << 20)) != is_rw) \
					return NNC_R_CORRUPT; \
				exh->memory_mappings[mmap_count].range_end = (desc & f3) << 12; \
				++mmap_count; \
			}
		DO_MMAP(NNC_EXHDR_MMAP_STATIC, 0xFFE00000, 0xFF800000, 0xFFFFF)
		DO_MMAP(NNC_EXHDR_MMAP_IO, 0xFFF00000, 0xFFE00000, 0x1FFFFF)
#undef DO_MMAP
		else if(desc != 0xFFFFFFFF)
			return NNC_R_CORRUPT; /* / not supported */
	}
	exh->memory_mappings[mmap_count].type = NNC_EXHDR_MMAP_END_ARRAY;
	return NNC_R_OK;
}

result nnc_read_exheader(nnc_rstream *rs, nnc_exheader *exh)
{
	u8 buf[0x400];
	result ret;
	TRY(read_at_exact(rs, 0, buf, sizeof(buf)));
#define SEG(out, base) \
	(out).address = LE32P(&buf[base + 0x00]); \
	(out).max_pages = LE32P(&buf[base + 0x04]); \
	(out).size = LE32P(&buf[base + 0x08])
	/* 0x000 */ memcpy(exh->application_title, &buf[0x000], 8);
	/* 0x000 */ exh->application_title[8] = '\0';
	/* 0x008 */ /* reserved */
	/* 0x00D */ exh->sci_flags = buf[0x00D];
	/* 0x00E */ exh->remaster_version = LE16P(&buf[0x00E]);
	/* 0x010 */ SEG(exh->text, 0x010);
	/* 0x01C */ exh->stack_size = LE32P(&buf[0x01C]);
	/* 0x020 */ SEG(exh->ro, 0x020);
	/* 0x02C */ /* reserved */
	/* 0x030 */ SEG(exh->data, 0x030);
	/* 0x03C */ exh->bss_size = LE32P(&buf[0x03C]);
	/* 0x040 */ for(int i = 0; i < 48; ++i)
	/* 0x040 */ {
	/* 0x040 */		if((exh->dependencies[i] = LE64P(&buf[0x040 + 0x08 * i])) == 0)
	/* 0x040 */			break; /* end of list */
	/* 0x040 */ }
	/* 0x1C0 */ exh->savedata_size = LE64P(&buf[0x1C0]);
	/* 0x1C8 */ exh->jump_id = LE64P(&buf[0x1C8]);
	/* 0x1D0 */ /* reserved */
	/* 0x200 */ exh->title_id = LE64P(&buf[0x200]);
	/* 0x208 */ exh->core_version = LE32P(&buf[0x208]);
	/* 0x20C */ exh->aci_flags = buf[0x20C]; // flag1
	/* 0x20D */ exh->n3ds_mode = buf[0x20D] & 0x7; // flag2
	/* 0x20E */ u8 flag0 = buf[0x20E];
	/* 0x20E */ exh->ideal_processor = flag0 & 0x3;
	/* 0x20E */ exh->affinity_mask = (flag0 >> 2) & 0x3;
	/* 0x20E */ exh->o3ds_mode = (flag0 >> 4) & 0xF;
	/* 0x20F */ exh->priority = buf[0x20F];
	/* 0x210 */ for(int i = 0; i < 16; ++i)
	/* 0x210 */ {
	/* 0x210 */ 	exh->resource_limit_category = LE16P(&buf[0x210 + 0x2 * i]);
	/* 0x210 */ }
	/* 0x230 */ exh->extdata_id = LE64P(&buf[0x230]);
	/* 0x234 */ exh->system_savedata_ids[0] = LE32P(&buf[0x234]);
	/* 0x238 */ exh->system_savedata_ids[1] = LE32P(&buf[0x238]);
	/* 0x240 */ exh->storage_accessible_unique_ids[0] = LE32P(&buf[0x240]);
	/* 0x244 */ exh->storage_accessible_unique_ids[1] = LE32P(&buf[0x244]);
	/* 0x248 */ exh->access_info = LE64(U64P(&buf[0x248]) & ~0xFF00000000000000ULL);
	/* 0x24F */ exh->other_attribs = buf[0x24F];
	/* 0x250 */ for(int i = 0; i < 34; ++i)
	/* 0x250 */ {
	/* 0x250 */ 	memcpy(exh->service_access_control[i], &buf[0x250 + 0x8 * i], 8);
	/* 0x250 */		exh->service_access_control[i][8] = '\0';
	/* 0x250 */ 	if(exh->service_access_control[i][0] == '\0') break; /* finished */
	/* 0x250 */ }
	/* 0x360 */ /* reserved */
	/* 0x36F */ exh->resource_limit_category = buf[0x36F];
	/* 0x370 */ TRY(do_kernel_caps(exh, buf));
	/* 0x3E0 */ /* reserved */
	/* 0x3F0 */ exh->arm9_access_control = buf[0x3F0];
	/* 0x3F1 */ /* (until 0x3FF) other descriptors, unused */
	/* 0x3FF */ exh->arm9_access_control_version = buf[0x3FF];
#undef SEG
	return NNC_R_OK;
}

const char *nnc_exheader_syscall_name(nnc_u8 id)
{
	if(id >= 0x7E) return NULL;
	return exheader_syscall_tab[id];
}

