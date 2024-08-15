#pragma once

#include <cstdint>

//put headers in here
extern const uint8_t moflexMagic[4];

extern const uint8_t bimgheader[32];

extern const uint8_t bannerMagic[4];

extern const uint8_t bannerheader[12928];

extern const uint8_t bannerfooter[18432];

//48x48, 4 channels (rgba)
//the overlay for the icon
extern const uint8_t icon_border48[9216];

//24x24, 4 channels (rgba)
//the overlay for the small icon
extern const uint8_t icon_border24[2304];

//264x154, 2 channels (ga)
extern const uint8_t film_overlay[81312];
