#pragma once
#include <stdint.h>

//90x90px, 4 channels (rgba)
//this is to fill the space in the top right
extern uint8_t logo90[32400];

//264x154px, 2 channels (grayscale alpha)
//this is used to show the banner preview
extern uint8_t film_overlay[81312];

//30x27, 2 channels (grayscale alpha)
//this is used for the randomize button
extern uint8_t randomize_array[1620];

//48x48, 4 channels (rgba)
//the overlay for the icon
extern uint8_t icon_border[9216];

//6x6, 2 channels (grayscale alpha)
extern uint8_t arrow_up[72];

//6x6, 2 channels (grayscale alpha)
extern uint8_t arrow_down[72];

//6x6, 2 channels (grayscale alpha)
extern uint8_t arrow_left[72];

//6x6, 2 channels (grayscale alpha)
extern uint8_t arrow_right[72];
