
#include "nnc/smdh.h"
#include <assert.h>
#include <string.h>
#include "./internal.h"


result nnc_read_smdh(rstream *rs, nnc_smdh *smdh)
{
	assert(sizeof(smdh->titles) == 0x2000 && "smdh->titles was not properly packed");

	u8 header[0x2040];
	result ret;
	TRY(read_at_exact(rs, 0, header, sizeof(header)));
	/* 0x0000 */ if(memcmp(header, "SMDH", 4) != 0)
	/* 0x0000 */ 	return NNC_R_CORRUPT;
	/* 0x0004 */ smdh->version = LE16P(&header[0x04]);
	/* 0x0006 */ /* reserved */
	/* 0x0008 */ memcpy(smdh->titles, &header[0x0008], sizeof(smdh->titles));
	/* 0x2008 */ memcpy(smdh->game_ratings, &header[0x2008], sizeof(smdh->game_ratings));
	/* 0x2018 */ smdh->lockout = LE32P(&header[0x2018]);
	/* 0x201C */ smdh->match_maker_id = LE32P(&header[0x201C]);
	/* 0x2020 */ smdh->match_maker_bit_id = LE64P(&header[0x2020]);
	/* 0x2028 */ smdh->flags = LE32P(&header[0x2028]);
	/* 0x202C */ smdh->eula_version_minor = header[0x202C];
	/* 0x202C */ smdh->eula_version_major = header[0x202E];
	/* 0x202E */ /* reserved */
	/* 0x2030 */ smdh->optimal_animation_frame = LE32P(&header[0x2030]);
	/* 0x2034 */ smdh->cec_id = LE32P(&header[0x2034]);
	return NNC_R_OK;
}

result nnc_read_smdh_graphics(); // TODO

