
#include "nnc/smdh.h"
#include <assert.h>
#include <string.h>
#include "./internal.h"

#define NNC_LARGEICON_BYTESIZE (2 * NNC_SMDH_ICON_DIM_LARGE * NNC_SMDH_ICON_DIM_LARGE)
#define NNC_SMALLICON_BYTESIZE (2 * NNC_SMDH_ICON_DIM_SMALL * NNC_SMDH_ICON_DIM_SMALL)


result nnc_read_smdh(rstream *rs, nnc_smdh *smdh)
{
	assert(sizeof(smdh->titles) == 0x2000 && "smdh->titles was not properly packed");

	u8 data[0x36C0];
	result ret;
	TRY(read_at_exact(rs, 0, data, sizeof(data)));
	/* 0x0000 */ if(memcmp(data, "SMDH", 4) != 0)
	/* 0x0000 */ 	return NNC_R_CORRUPT;
	/* 0x0004 */ smdh->version = LE16P(&data[0x04]);
	/* 0x0006 */ /* reserved */
	/* 0x0008 */ memcpy(smdh->titles, &data[0x0008], sizeof(smdh->titles));
	/* 0x2008 */ memcpy(smdh->game_ratings, &data[0x2008], sizeof(smdh->game_ratings));
	/* 0x2018 */ smdh->lockout = LE32P(&data[0x2018]);
	/* 0x201C */ smdh->match_maker_id = LE32P(&data[0x201C]);
	/* 0x2020 */ smdh->match_maker_bit_id = LE64P(&data[0x2020]);
	/* 0x2028 */ smdh->flags = LE32P(&data[0x2028]);
	/* 0x202C */ smdh->eula_version_minor = data[0x202C];
	/* 0x202D */ smdh->eula_version_major = data[0x202D];
	/* 0x202E */ /* reserved */
	/* 0x2030 */ smdh->optimal_animation_frame = nnc_load_f32_le(&data[0x2030]);
	/* 0x2034 */ smdh->cec_id = LE32P(&data[0x2034]);
	/* 0x2038 */ /* reserved */
	/* 0x2040 */ memcpy(smdh->icon_small, &data[0x2040], NNC_SMALLICON_BYTESIZE);
	/* 0x24C0 */ memcpy(smdh->icon_large, &data[0x24C0], NNC_LARGEICON_BYTESIZE);
	return NNC_R_OK;
}

result nnc_write_smdh(nnc_smdh *smdh, nnc_wstream *ws)
{
	assert(sizeof(smdh->titles) == 0x2000 && "smdh->titles was not properly packed");

	if(smdh->version != 0)
		return NNC_R_INVAL;

	u8 data[0x36C0];

	/* 0x0000 */ memcpy(&data[0x0000], "SMDH", 4);
	/* 0x0004 */ U16P(&data[0x0004]) = LE16(smdh->version);
	/* 0x0006 */ memset(&data[0x0006], 0x00, 2);
	/* 0x0008 */ memcpy(&data[0x0008], smdh->titles, sizeof(smdh->titles));
	/* 0x2008 */ memcpy(&data[0x2008], smdh->game_ratings, sizeof(smdh->game_ratings));
	/* 0x2018 */ U32P(&data[0x2018]) = LE32(smdh->lockout);
	/* 0x201C */ U32P(&data[0x201C]) = LE32(smdh->match_maker_id);
	/* 0x2020 */ U64P(&data[0x2020]) = LE64(smdh->match_maker_bit_id);
	/* 0x2028 */ U32P(&data[0x2028]) = LE32(smdh->flags);
	/* 0x202C */ data[0x202C] = smdh->eula_version_minor;
	/* 0x202D */ data[0x202D] = smdh->eula_version_major;
	/* 0x202E */ memset(&data[0x202E], 0x00, 2);
	/* 0x2030 */ nnc_store_f32_le(&data[0x2030], smdh->optimal_animation_frame);
	/* 0x2034 */ U32P(&data[0x2034]) = LE32(smdh->cec_id);
	/* 0x2038 */ memset(&data[0x2038], 0x00, 8);
	/* 0x2040 */ memcpy(&data[0x2040], smdh->icon_small, NNC_SMALLICON_BYTESIZE);
	/* 0x24C0 */ memcpy(&data[0x24C0], smdh->icon_large, NNC_LARGEICON_BYTESIZE);

	return NNC_WS_PCALL(ws, write, data, sizeof(data));
}