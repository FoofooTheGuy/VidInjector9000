#include "image.hpp"

void crop_pixels(const uint8_t* input, int width, int height, int channels, uint8_t* output, int x_offset, int y_offset, int out_w, int out_h) {
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++) {
			if (x >= x_offset && x < width - x_offset && y >= y_offset && y < height - y_offset && (x - x_offset) < out_w && (y - y_offset) < out_h) {
				for (int ch = 0; ch < channels; ch++)
					output[((y - y_offset) * out_w + (x - x_offset)) * channels + ch] = input[(y * width + x) * channels + ch];
			}
		}
}

void ToRGBA(const uint8_t* input, uint8_t* output, int width, int height, int channels) {
	if (channels == 1) { // grayscale
		int j = 0;
		for (int i = 0; i < width * height; i++) {
			for (int ch = 0; ch < 3; ch++)
				output[j + ch] = input[i];
			output[j + 3] = 0xFF;
			j += 4;
		}
	}
	else if (channels == 2) { // grayscale alpha
		int j = 0;
		for (int i = 0; i < width * height * channels; i += channels) {
			for (int ch = 0; ch < 3; ch++)
				output[j + ch] = input[i];
			output[j + 3] = input[i + 1];
			j += 4;
		}
	}
	else if (channels == 3) { // rgb
		int j = 0;
		for (int i = 0; i < width * height * channels; i += channels) {
			for (int ch = 0; ch < 3; ch++)
				output[j + ch] = input[i + ch];
			output[j + 3] = 0xFF;
			j += 4;
		}
	}
	else if (channels == 4) { // rgba
		memcpy(output, input, width * height * channels);
	}
}

void ToRGB(const uint8_t* input, uint8_t* output, int width, int height, int channels) {
	if (channels == 1) { // grayscale
		int j = 0;
		for (int i = 0; i < width * height * 3; i += 3) {
			for (int c = 0; c < 3; c++) {
				output[i + c] = input[j];
			}
			j++;
		}
	}
	else if (channels == 3) { // rgb
		memcpy(output, input, width * height * channels);
	}
	else if (channels == 2 || channels == 4) { // grayscale alpha or rgba
		std::vector<uint8_t> white_background(width * height * channels, 0xFF);
		std::vector<uint8_t> output_4c(width * height * 4);
		layer_pixels(output_4c.data(), input, white_background.data(), width, height, channels, width, height, channels, 0, 0);
		int j = 0;
		for (int i = 0; i < width * height * 4; i += 4) {
			for (int c = 0; c < 3; c++) {
				output[j + c] = output_4c[i + c];
			}
			j += 3;
		}
	}
}

//only works if the foreground image is larger than the background image
void layer_pixels(uint8_t* out, const uint8_t* foreground, const uint8_t* background, int forewidth, int foreheight, int forechannels, int backwidth, int backheight, int backchannels, int x_offset, int y_offset) {
	std::vector<uint8_t> foreground_4c(forewidth * foreheight * 4);
	std::vector<uint8_t> background_4c(backwidth * backheight * 4);
	const uint8_t FF = 0xFF;

	ToRGBA(foreground, foreground_4c.data(), forewidth, foreheight, forechannels);
	ToRGBA(background, background_4c.data(), backwidth, backheight, backchannels);

	for (int y = 0; y < foreheight; y++)
		for (int x = 0; x < forewidth; x++) {
			if (x >= x_offset && x < forewidth - x_offset && y >= y_offset && y < backheight + y_offset) {
				//https://stackoverflow.com/a/64655571
				uint8_t alpha_out = foreground_4c.at((y * foreheight + x) * 4 + 3) + (background_4c.at(((y - y_offset) * backwidth + (x - x_offset)) * 4 + 3) * (FF - foreground_4c.at((y * foreheight + x) * 4 + 3)) / FF);
				for (int ch = 0; ch < 3; ch++)
					out[(y * forewidth + x) * 4 + ch] = (foreground_4c.at((y * forewidth + x) * 4 + ch) * foreground_4c.at((y * forewidth + x) * 4 + 3) + background_4c.at(((y - y_offset) * backwidth + (x - x_offset)) * 4 + ch) * FF * (FF - foreground_4c.at((y * forewidth + x) * 4 + 3)) / FF) / alpha_out;
				out[(y * forewidth + x) * 4 + 3] = alpha_out;
			}
			else {
				for (int ch = 0; ch < 4; ch++)
					out[(y * forewidth + x) * 4 + ch] = foreground_4c.at((y * forewidth + x) * 4 + ch);
			}
		}
}
