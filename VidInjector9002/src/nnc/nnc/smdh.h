/** \file  smdh.h
 *  \brief Functions relating to SMDH.
 *  \see   https://www.3dbrew.org/wiki/SMDH
 */
#ifndef inc_nnc_smdh_h
#define inc_nnc_smdh_h

#include "stream.h"
#include "base.h"
NNC_BEGIN

/** Bits in a rating that are set to the minimum age. */
#define NNC_RATING_AGE_BITS 0x1f
/** Returns the minimum age rating if set and 0 otherwise. */
#define NNC_RATING_MIN_AGE(a) (((a) & NNC_RATING_ACTIVE) ? ((a) & NNC_RATING_AGE_BITS) : 0)
/** \brief Amount of titles in an SMDH.
 *  \note If you simply want to iterate over all titles in an SMDH
 *        use \ref NNC_TITLE_MAX instead.
 */
#define NNC_SMDH_TITLES 0x10
 /** Amount of game ratings in an SMDH. */
#define NNC_SMDH_RATINGS 0x10
/** Pack the EULA of an SMDH header into one value for inequality checks. */
#define NNC_EULA_PACK(smdh) ((smdh).eula_version_major * 0xFF + (smdh).eula_version_minor)
/** The dimensions (for the x and y axes) of the small SMDH icon. */
#define NNC_SMDH_ICON_DIM_SMALL 24
/** The dimensions (for the x and y axes) of the large SMDH icon. */
#define NNC_SMDH_ICON_DIM_LARGE 48

enum nnc_title_lang {
	NNC_TITLE_JAPANESE = 0,  ///< Japanese.
	NNC_TITLE_ENGLISH = 1,  ///< English.
	NNC_TITLE_FRENCH = 2,  ///< French.
	NNC_TITLE_GERMAN = 3,  ///< German.
	NNC_TITLE_ITALIAN = 4,  ///< Italian.
	NNC_TITLE_SPANISH = 5,  ///< Spanish.
	NNC_TITLE_SIMPLIFIED_CHINESE = 6,  ///< Simplified chinese.
	NNC_TITLE_KOREAN = 7,  ///< Korean.
	NNC_TITLE_DUTCH = 8,  ///< Dutch.
	NNC_TITLE_PORTUGUESE = 9,  ///< Portuguese.
	NNC_TITLE_RUSSIAN = 10, ///< Russian
	NNC_TITLE_TRADITIONAL_CHINESE = 11, ///< Traditional chinese.
	NNC_TITLE_MAX = 12, ///< Maximum used languaged used for iterating.
};

enum nnc_region_lockout {
	NNC_LOCKOUT_JAPAN = 0x1,        ///< Japan.
	NNC_LOCKOUT_NORTH_AMERICA = 0x2,        ///< North America.
	NNC_LOCKOUT_EUROPE = 0x4,        ///< Europe.
	NNC_LOCKOUT_AUSTRALIA = 0x8,        ///< Australia.
	NNC_LOCKOUT_CHINA = 0x10,       ///< China.
	NNC_LOCKOUT_KOREA = 0x20,       ///< Korea.
	NNC_LOCKOUT_TAIWAN = 0x40,       ///< Taiwan.
	NNC_LOCKOUT_FREE = 0x7FFFFFFF, ///< Region free, note this is not a bitmask but an exact value.
};

enum nnc_smdh_flags {
	NNC_SMDH_FLAG_VISIBLE = 0x1,    ///< Application is visible on home menu.
	NNC_SMDH_FLAG_AUTOBOOT = 0x2,    ///< Automatically boot gamecard.
	NNC_SMDH_FLAG_ALLOW_3D = 0x4,    ///< Allow use of 3D with parental controls.
	NNC_SMDH_FLAG_REQUIRE_EULA = 0x8,    ///< Make home menu check the EULA version.
	NNC_SMDH_FLAG_AUTOSAVE = 0x10,   ///< Autosave warning when closing application.
	NNC_SMDH_FLAG_EXTENDED_BANNER = 0x20,   ///< Application uses an extended banner.
	NNC_SMDH_FLAG_RATING_REQUIRED = 0x40,   ///< Game rating required.
	NNC_SMDH_FLAG_SAVEDATA = 0x80,   ///< Savedata warning when closing application.
	NNC_SMDH_FLAG_RECORD_USAGE = 0x100,  ///< Record application usage in play log.
	NNC_SMDH_FLAG_DISABLE_BACKUP = 0x400,  ///< Disable system settings savedata backup.
	NNC_SMDH_FLAG_N3DS_ONLY = 0x1000, ///< Makes home menu disallow booting on old 3ds'.
};

enum nnc_game_rating_slot {
	NNC_RATING_CERO = 0,  ///< Used in Japan.
	NNC_RATING_ESRB = 1,  ///< Used in the USA.
	NNC_RATING_USK = 3,  ///< Used in Germany.
	NNC_RATING_PEGI_GEN = 4,  ///< Used in most of Europe.
	NNC_RATING_PEGI_PRT = 6,  ///< Used in Portugal.
	NNC_RATING_PEGI_BBFC = 7,  ///< Used in England.
	NNC_RATING_COB = 8,  ///< Used in Australia.
	NNC_RATING_GRB = 9,  ///< Used in South Korea
	NNC_RATING_CGSRR = 10, ///< Used in Taiwan.
};

enum nnc_game_rating {
	NNC_RATING_NONE = 0x0,  ///< Inactive rating.
	NNC_RATING_ALL_AGES = 0x20, ///< No age restriction.
	NNC_RATING_PENDING = 0x40, ///< Rating pending.
	NNC_RATING_ACTIVE = 0x80, ///< Rating active.
};

/** All fields in this struct are UTF16 encoded, use \ref nnc_utf16_to_utf8 to decode. */
typedef struct nnc_smdh_title {
	nnc_u16 short_desc[0x40];  ///< Short title description.
	nnc_u16 long_desc[0x80];   ///< Long title description.
	nnc_u16 publisher[0x40];   ///< Title publisher.
} nnc_smdh_title;

typedef struct nnc_smdh {
	nnc_u16 version;                        ///< SMDH version.
	nnc_smdh_title titles[NNC_SMDH_TITLES]; ///< Titles, get appropriate language with \ref nnc_title_lang.
	nnc_u8 game_ratings[NNC_SMDH_RATINGS];  ///< Game ratings, see \ref NNC_RATING_MIN_AGE, \ref nnc_game_rating_slot and \ref nnc_game_rating.
	nnc_u32 lockout;                        ///< Region lockout, see \ref nnc_region_lockout.
	nnc_u32 match_maker_id;                 ///< Match maker ID.
	nnc_u64 match_maker_bit_id;             ///< Match maker BIT ID.
	nnc_u32 flags;                          ///< Flags, see \ref nnc_smdh_flags.
	nnc_u16 eula_version_minor;             ///< EULA minor version.
	nnc_u16 eula_version_major;             ///< EULA major version.
	nnc_f32 optimal_animation_frame;        ///< Optimal animation default frame for BNR.
	nnc_u32 cec_id;                         ///< CEC/Streetpass ID.
	nnc_u16 icon_small[24][24];             ///< Small icon, z-order curve swizzled, little endian.
	nnc_u16 icon_large[48][48];             ///< Large icon, z-order curve swizzled, little endian.
} nnc_smdh;

/** \brief       Read an SMDH from a stream.
 *  \param rs    Stream to read SMDH from.
 *  \param smdh  Output SMDH.
 *  \see         \link swizzle.h \endlink contains functions to unswizzle the icon graphics.
 */
nnc_result nnc_read_smdh(nnc_rstream* rs, nnc_smdh* smdh);

/** \brief       Write an SMDH.
 *  \param smdh  SMDH to write.
 *  \param ws    Output write stream.
 */
nnc_result nnc_write_smdh(nnc_smdh* smdh, nnc_wstream* ws);

NNC_END
#endif