/** \file  exheader.h
 *  \brief Functions relating to the extended header.
 *  \see   https://www.3dbrew.org/wiki/NCCH/Extended_Header
 */
#ifndef inc_nnc_exheader_h
#define inc_nnc_exheader_h

#include "stream.h"
#include "base.h"
NNC_BEGIN

/** Gets the actual memory type of a kflags field, see \ref nnc_exheader_memory_type for possible values. */
#define NNC_EXHEADER_MEMTYPE(kflags) \
	((kflags & NNC_EXHDR_KFLAG_MEMTYPE_FIELD) >> 8)
/** Maximum amount of memory mappings. */
#define NNC_MAX_MMAPS 10
/** Maximum amount of dependencies */
#define NNC_MAX_DEPENDENCIES 48
/** Amount of resource descriptors */
#define NNC_EXHDR_RESOURCE_DESCRIPTORS_COUNT 16
/** Service access control max before 9.3.0. */
#define NNC_EXHDR_SAC_PRE 32
/** Service access control max after 9.3.0. */
#define NNC_EXHDR_SAC_930 34
/** Size of the \ref nnc_exheader::allowed_syscalls array. */
#define NNC_EXHDR_SYSCALL_SIZE 8
/** Amount of useful bits in each \ref nnc_exheader::allowed_syscalls array element. */
#define NNC_EXHDR_SYSCALL_USEFUL_BITS 24

typedef struct nnc_exheader_codeseginfo {
	nnc_u32 address;   ///< Address of segment.
	nnc_u32 max_pages; ///< Maximum pages, each page is 0x1000 bytes large.
	nnc_u32 size;      ///< Size of segment.
} nnc_exheader_codeseginfo;

enum nnc_exheader_kflag {
	NNC_EXHDR_KFLAG_ALLOW_DEBUG         = 0x01,   ///< Allow debug.
	NNC_EXHDR_KFLAG_FORCE_DEBUG         = 0x02,   ///< Force debug.
	NNC_EXHDR_KFLAG_ALLOW_NON_ALPHANUM  = 0x04,   ///< Allow non-alphanumeric.
	NNC_EXHDR_KFLAG_SHARED_PAGE_WRITING = 0x08,   ///< Shared page writing.
	NNC_EXHDR_KFLAG_PRIVILEGE_PRIORITY  = 0x10,   ///< Privilege priority.
	NNC_EXHDR_KFLAG_ALLOW_MAIN_ARGS     = 0x20,   ///< Allow main() args.
	NNC_EXHDR_KFLAG_RUNNABLE_SLEEP      = 0x40,   ///< Runnable on sleep.
	NNC_EXHDR_KFLAG_MEMTYPE_FIELD       = 0x380,  ///< Memory type bitfield, see \ref NNC_EXHEADER_MEMTYPE
	NNC_EXHDR_KFLAG_SPECIAL_MEMORY      = 0x800,  ///< Special memory.
	NNC_EXHDR_KFLAG_CPU_CORE2           = 0x1000, ///< Allow access CPU core 2.
};

enum nnc_exheader_memory_type {
	NNC_EXHDR_APPLICATION = 1, ///< Application.
	NNC_EXHDR_SYSTEM      = 2, ///< System.
	NNC_EXHDR_BASE        = 3, ///< Base.
};

enum nnc_exheader_n3ds_mode {
	NNC_EXHDR_N3DS_LEGACY = 0, ///< = \ref NNC_EXHDR_O3DS_PROD.
	NNC_EXHDR_N3DS_PROD   = 1, ///< 124MiB of memory.
	NNC_EXHDR_N3DS_DEV1   = 2, ///< 178MiB of memory.
	NNC_EXHDR_N3DS_DEV2   = 3, ///< 124MiB of memory.
};

enum nnc_exheader_o3ds_mode {
	NNC_EXHDR_O3DS_PROD = 0, ///< 64MiB of memory.
	NNC_EXHDR_O3DS_DEV1 = 2, ///< 96MiB of memory.
	NNC_EXHDR_O3DS_DEV2 = 3, ///< 80MiB of memory.
	NNC_EXHDR_O3DS_DEV3 = 4, ///< 72MiB of memory.
	NNC_EXHDR_O3DS_DEV4 = 5, ///< 32MiB of memory.
};

enum nnc_exheader_aci_flags {
	NNC_EXHDR_ACI_COMPRESS_CODE  = 1, ///< ExeFS ".code" is compressed.
	NNC_EXHDR_ACI_SD_APPLICATION = 2, ///< Application is for SD.
};

enum nnc_exheader_sci_flags {
	NNC_EXHDR_SCI_ENABLE_L2_CACHE = 1, ///< L2 cache enabled (n3ds).
	NNC_EXHDR_SCI_CPUSPEED_804MHZ = 2, ///< 804MHz CPU speed (n3ds).
};

enum nnc_exheader_resource_limit_category {
	NNC_EXHDR_RESLIM_CAT_APPLICATION = 0, ///< For use with normal applications.
	NNC_EXHDR_RESLIM_CAT_SYS_APPLET  = 1, ///< For system applets.
	NNC_EXHDR_RESLIM_CAT_LIB_APPLET  = 2, ///< For library applets.
	NNC_EXHDR_RESLIM_CAT_OTHER       = 3, ///< Other, i.e. sysmodules.
};

enum nnc_exheader_arm9_access_control {
	NNC_EXHDR_MOUNT_NAND     = 0x1,   ///< Mount nand:/ (CTRNAND).
	NNC_EXHDR_MOUNT_NAND_RO  = 0x2,   ///< Mount nand:/ro/ (write access).
	NNC_EXHDR_MOUNT_TWLN     = 0x4,   ///< Mount twln:/ (TWLNAND).
	NNC_EXHDR_MOUNT_WNAND    = 0x8,   ///< Mount wnand:/.
	NNC_EXHDR_MOUNT_CARD_SPI = 0x10,  ///< Mount SPI card.
	NNC_EXHDR_USE_SDIF3      = 0x20,  ///< Use SDIF3.
	NNC_EXHDR_CREATE_SEED    = 0x40,  ///< Create seed.
	NNC_EXHDR_USE_CARD_SPI   = 0x80,  ///< Use SPI card.
	NNC_EXHDR_SD_APPLICATION = 0x100, ///< SD application.
	NNC_EXHDR_MOUNT_SDMC     = 0x200, ///< Mount sdmc:/ (SD card).
};

enum nnc_exheader_other_attribs {
	NNC_EXHDR_ATTRIB_NO_ROMFS          = 0x1, ///< Does not use a RomFS.
	NNC_EXHDR_ATTRIB_EXTENDED_SAVEDATA = 0x2, ///< Uses extended savedata access.
};

enum nnc_exheader_filesystem_access_info {
	NNC_EXHDR_FS_SYSAPP_CAT        = 0x1,      ///< System application category.
	NNC_EXHDR_FS_HWCHK_CAT         = 0x2,      ///< Hardware check category.
	NNC_EXHDR_FS_FSTOOL_CAT        = 0x4,      ///< FS tool category.
	NNC_EXHDR_FS_DEBUG             = 0x8,      ///< Debug.
	NNC_EXHDR_FS_TWL_CARD_BACKUP   = 0x10,     ///< TWL card backup.
	NNC_EXHDR_FS_TWL_NAND_DATA     = 0x20,     ///< TWL NAND data.
	NNC_EXHDR_FS_BOSS              = 0x40,     ///< BOSS (streetpass).
	NNC_EXHDR_FS_SDMC_R            = 0x80,     ///< Access to sdmc:/ (read)
	NNC_EXHDR_FS_CORE              = 0x100,    ///< Core.
	NNC_EXHDR_FS_NAND_RO           = 0x200,    ///< Access to nand:/ro/ (read).
	NNC_EXHDR_FS_NAND_RW           = 0x400,    ///< Access to nand:/rw/.
	NNC_EXHDR_FS_NAND_RO_W         = 0x800,    ///< Access to nand:/ro/ (write).
	NNC_EXHDR_FS_SYS_SETTINGS_CAT  = 0x1000,   ///< System settings category.
	NNC_EXHDR_FS_CARDBOARD         = 0x2000,   ///< Cardboard.
	NNC_EXHDR_FS_EXPORT_IMPORT_IVS = 0x4000,   ///< Export/import IVS.
	NNC_EXHDR_FS_SDMC_W            = 0x8000,   ///< Access to sdmc:/ (write).
	NNC_EXHDR_FS_SWITCH_CLEANUP    = 0x10000,  ///< Switch cleanup, since 3.0.0.
	NNC_EXHDR_FS_SAVEDATA_MOVE     = 0x20000,  ///< Savedata move, since 5.0.0.
	NNC_EXHDR_FS_SHOP              = 0x40000,  ///< Shop, since 5.0.0.
	NNC_EXHDR_FS_SHELL             = 0x80000,  ///< Shell, since 5.0.0.
	NNC_EXHDR_FS_HMM_CAT           = 0x100000, ///< Home menu category, since 6.0.0.
	NNC_EXHDR_FS_SEEDDB            = 0x200000, ///< SeedDB, since 9.6.0.
};

typedef struct nnc_exheader {
	/* system control info */
	char application_title[9];                                                ///< Application title, NULL-terminated.
	nnc_u8 sci_flags;                                                         ///< See \ref nnc_exheader_aci_flags.
	nnc_u16 remaster_version;                                                 ///< Title version.
	nnc_exheader_codeseginfo text;                                            ///< Text segment.
	nnc_u32 stack_size;                                                       ///< Size of the stack.
	nnc_exheader_codeseginfo ro;                                              ///< RO segment.
	nnc_exheader_codeseginfo data;                                            ///< Data segment.
	nnc_u32 bss_size;                                                         ///< BSS size.
	nnc_u64 dependencies[NNC_MAX_DEPENDENCIES];                               ///< Title IDs of title dependencies, that is, these must be installed before launching the title, terminated by a '\0' if smaller than \ref NNC_MAX_DEPENDENCIES.
	nnc_u64 savedata_size;                                                    ///< Size of the savedata.
	nnc_u64 jump_id;                                                          ///< Jump ID.
		/* ARM11 local system capabilities */
	nnc_u64 title_id;                                                         ///< Title ID, also known as a program ID.
	nnc_u32 core_version;                                                     ///< Title ID low of the required FIRM.
	nnc_u8 aci_flags;                                                         ///< See \ref nnc_exheader_sci_flags.
	nnc_u8 n3ds_mode;                                                         ///< See \ref nnc_exheader_n3ds_mode.
	nnc_u8 ideal_processor;                                                   ///< Ideal processor.
	nnc_u8 affinity_mask;                                                     ///< Affinity mask.
	nnc_u8 o3ds_mode;                                                         ///< See \ref nnc_exheader_o3ds_mode.
	nnc_u8 priority;                                                          ///< Main thread priority.
	nnc_u16 resource_limit_descriptors[NNC_EXHDR_RESOURCE_DESCRIPTORS_COUNT]; ///< Resource limit descriptors.
	nnc_u64 extdata_id;                                                       ///< Extdata ID.
	nnc_u32 system_savedata_ids[2];                                           ///< System savedata IDs.
	nnc_u32 storage_accessible_unique_ids[2];                                 ///< Storage accessible unique IDs.
	nnc_u64 access_info;                                                      ///< Filesystem access info, see \ref nnc_exheader_filesystem_access_info.
	nnc_u8 other_attribs;                                                     ///< Other attributes, see \ref nnc_exheader_other_attribs.
	char service_access_control[NNC_EXHDR_SAC_930][9];                        ///< Service access control, that is, NULL-terminated service names that this title requires.
	nnc_u8 resource_limit_category;                                           ///< See \ref nnc_exheader_resource_limit_category.
		/* ARM11 kernel capabilities */
	nnc_u32 allowed_syscalls[NNC_EXHDR_SYSCALL_SIZE];                         ///< Large bitfield containing a mask of all allowed system calls (SVCs), to get the real ID of the syscall you have to multiply the index by \ref NNC_EXHDR_SYSCALL_USEFUL_BITS and then add the bit index, see \ref nnc_exheader_syscall_name.
	nnc_u8 kernel_release_minor;                                              ///< Minor kernel release version.
	nnc_u8 kernel_release_major;                                              ///< Major kernel release version.
	nnc_u32 max_handles;                                                      ///< Maximum handles.
	nnc_u16 kflags;                                                           ///< Kernel flags, see \ref nnc_exheader_kflag
	struct nnc_exheader_mem_mapping {
		nnc_u32 range_start, ///< Start of the range.
		        range_end;   ///< End of the range, not inclusive.
		enum nnc_exheader_mem_mapping_type {
			NNC_EXHDR_MMAP_END_ARRAY = 0, ///< Terminator, see \ref nnc_exheader::memory_mappings.
			NNC_EXHDR_MMAP_STATIC_RW = 1, ///< Static read/write.
			NNC_EXHDR_MMAP_STATIC_RO = 2, ///< Static read.
			NNC_EXHDR_MMAP_IO_RW     = 3, ///< IO read/write.
			NNC_EXHDR_MMAP_IO_RO     = 4, ///< IO read.
		} type;              ///< Range type
	} memory_mappings[NNC_MAX_MMAPS];                                         ///< Memory mappings, terminated by type being NNC_EXHDR_MMAP_END_ARRAY if smaller than \ref NNC_MAX_MMAPS.
	/* no interrupts because i don't know what they do */
		/* ARM9 access control */
	nnc_u16 arm9_access_control;                                              ///< See \ref nnc_exheader_arm9_access_control. 15 bytes in the original struct but everything after bit9 is unused.
	nnc_u8 arm9_access_control_version;                                       ///< ARM9 access control version, either 2 or 3.
} nnc_exheader;


/** \brief      Reads an exheader.
 *  \param rs   Stream to read exheader from.
 *  \param exh  Output exheader
 *  \returns
 *  \p NNC_R_UNSUPPORTED => Exheader contains interrupts in descriptors. \n
 *  \p NNC_R_CORRUPT => Invalid descriptors detected.
 */
nnc_result nnc_read_exheader(nnc_rstream *rs, nnc_exheader *exh);

/** \brief     Returns the name of a system call ID.
 *  \param id  The ID to look up.
 */
const char *nnc_exheader_syscall_name(nnc_u8 id);

NNC_END
#endif

