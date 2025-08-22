#include "CTR_stuff.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// formerly in image.cpp
void crop_pixels(const uint8_t* input, int width, int height, int channels, uint8_t* output, int x_offset, int y_offset, int out_w, int out_h) {
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++) {
			if (x >= x_offset && x < width - x_offset && y >= y_offset && y < height - y_offset && (x - x_offset) < out_w && (y - y_offset) < out_h) {
				for (int ch = 0; ch < channels; ch++)
					output[((y - y_offset) * out_w + (x - x_offset)) * channels + ch] = input[(y * width + x) * channels + ch];
			}
		}
}

void resize_crop(const uint8_t* input_pixels, int input_w, int input_h, uint8_t* output_pixels, int output_w, int output_h, int num_channels) {
	int width, height;

	//"inspired" by https://github.com/endlessm/chromium-browser/blob/aa8c819d5ad2fcb3854a688a0401975eca721f43/ui/gfx/favicon_size.cc#L13
	if (input_w > input_h) {//panoramic
		height = output_h;
		width = static_cast<int>(output_h * static_cast<float>(input_w) / static_cast<float>(input_h));
	}
	else if (input_w < input_h) {//portrait
		height = static_cast<int>(input_h / (static_cast<float>(input_w) / static_cast<float>(output_w)));
		width = output_w;
	}
	else {//square
		width = (output_w >= output_h) ? output_w : output_h;
		height = (output_w >= output_h) ? output_w : output_h;
	}
	if (width < output_w) {
		height = static_cast<int>(input_h / (static_cast<float>(input_w) / static_cast<float>(output_w)));
		width = output_w;
	}
	if (height < output_h) {
		height = output_h;
		width = static_cast<int>(output_h * static_cast<float>(input_w) / static_cast<float>(input_h));
	}

	std::vector<uint8_t> scaled = std::vector<uint8_t>(width * height * num_channels);
	// scale it down also now we gotta convert the channels to stbir_pixel_layout i guess
	stbir_resize_uint8_linear(input_pixels, input_w, input_h, 0, scaled.data(), width, height, 0, static_cast<stbir_pixel_layout>(num_channels));
	crop_pixels(scaled.data(), width, height, num_channels, output_pixels, (width - output_w) / 2, (height - output_h) / 2, output_w, output_h);
}

void ToRGBA(const uint8_t* input, uint8_t* output, int width, int height, int channels) {
	if (channels == 1) {//grayscale
		int j = 0;
		for (int i = 0; i < width * height; i++) {
			for (int ch = 0; ch < 3; ch++)
				output[j + ch] = input[i];
			output[j + 3] = 0xFF;
			j += 4;
		}
	}
	else if (channels == 2) {//grayscale alpha
		int j = 0;
		for (int i = 0; i < width * height * channels; i += channels) {
			for (int ch = 0; ch < 3; ch++)
				output[j + ch] = input[i];
			output[j + 3] = input[i + 1];
			j += 4;
		}
	}
	else if (channels == 3) {//rgb
		int j = 0;
		for (int i = 0; i < width * height * channels; i += channels) {
			for (int ch = 0; ch < 3; ch++)
				output[j + ch] = input[i + ch];
			output[j + 3] = 0xFF;
			j += 4;
		}
	}
	else if (channels == 4) {//rgba
		memcpy(output, input, width * height * channels);
	}
}

//only works if the forwground image is larger than the background image
void layer_pixels(uint8_t* out, const uint8_t* foreground, const uint8_t* background, int forewidth, int foreheight, int forechannels, int backwidth, int backheight, int backchannels, int x_offset, int y_offset) {
	std::vector<uint8_t> foreground_4c = std::vector<uint8_t>(forewidth * foreheight * 4);
	std::vector<uint8_t> background_4c = std::vector<uint8_t>(backwidth * backheight * 4);
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

int generateBlankBanner(std::string &outfile) {
	int ch = 4;
	int out_w = 200;
	int out_h = 120;
	int film_w = 264;
	int film_h = 154;
	std::vector<uint8_t> output_4c = std::vector<uint8_t>(out_w * out_h * ch);
	memset(output_4c.data(), 0xFF, out_w * out_h * ch);

	std::vector<uint8_t> output_film = std::vector<uint8_t>(film_w * film_h * 4);
	layer_pixels(output_film.data(), film_overlay, output_4c.data(), film_w, film_h, 2, out_w, out_h, ch, 32, 11);
	
	std::cout << CreatingFile << ' ' << std::filesystem::absolute(outfile.c_str()) << std::endl;
	stbi_write_png(outfile.c_str(), film_w, film_h, 4, output_film.data(), 0);
	std::error_code error;
	if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*outfile.c_str()), error)) {
		return 28;
	}
	if (error) {
		std::cout << ErrorText << ' ' << outfile << '\n' << error.message() << std::endl;
		return 29;
	}
	return 0;
}

//use multibanner true if you are doing multibanner
int generateBannerPreview(std::string infile, std::string outfile, bool multibanner = false) {
	//first fix the outfile extension if needed
	{
		std::string outExtension = outfile;
		if(outExtension.find_last_of(".") != std::string::npos) {
			outExtension.erase(outExtension.begin(), outExtension.begin() + outExtension.find_last_of("."));
		}
		if(tolowerstr(outExtension) != ".png") {
			outfile += ".png";
		}
	}
		
	int w = 0, h = 0, ch = 0;
	int out_w = 200;
	int out_h = 120;
	int film_w = 264;
	int film_h = 154;
	uint8_t Checker[4];
	bool banner = false;
	int ich = sizeof(nnc_u16);
	std::ifstream inbanner(std::filesystem::path((const char8_t*)&*infile.c_str()), std::ios::binary);
	std::error_code error;
	if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*infile.c_str()), error)) {
		int blankret = generateBlankBanner(outfile);
		return 16 + blankret;//kind of stupid, (16 or 44 or 45) at least now you know if it was an infile problem
	}
	if (error) {
		std::cout << ErrorText << ' ' << infile << '\n' << error.message() << std::endl;
		int blankret = generateBlankBanner(outfile);
		return 18 + blankret;//18 or 46 or 47
	}
	for (int i = 0; i < 4; i++) {
		inbanner >> Checker[i];//https://stackoverflow.com/a/2974735
		if (Checker[i] == bannerMagic[i]) {
			banner = true;
		}
		else {
			banner = false;
			break;
		}
	}
	if (banner && !multibanner) {
		w = 256;
		h = 128;
		std::vector<uint8_t> CGFXdecomp;
		uint8_t ret = 0;
		{
			uint32_t decompressedSize = 0;
			uint32_t compressedSize = 0;
			uint32_t CGFXoffset = 0;
			ret = getCBMDInfo(infile, &compressedSize, &decompressedSize, &CGFXoffset);
			if (ret > 0) {
				//bannerpreview.image(empty(0, 0));
				//customnotif.show();
				return 17;
			}
			CGFXdecomp = std::vector<uint8_t>(decompressedSize);
			ret = CBMDgetCommonCGFX(infile, compressedSize, CGFXoffset, CGFXdecomp.data());
			if (ret > 0) {
				//bannerpreview.image(empty(0, 0));
				//customnotif.show();
				return 17;//same as the previous one, i guess this will just mean it is a custom banner thus no preview which is fine really
			}
		}

		uint32_t dataOffset0;
		uint32_t height0;
		uint32_t width0;
		uint32_t mipmap0;
		uint32_t formatID0;
		uint32_t size0;
		ret = getCGFXtextureInfo(CGFXdecomp.data(), "COMMON0", dataOffset0, height0, width0, mipmap0, formatID0, size0);
		if (ret > 0 || height0 != 0x80 || width0 != 0x100 || formatID0 != 0x3) {
			//bannerpreview.image(empty(0, 0));
			//customnotif.show();
			return 17;
		}

		uint32_t dataOffset1;
		uint32_t height1;
		uint32_t width1;
		uint32_t mipmap1;
		uint32_t formatID1;
		uint32_t size1;
		ret = getCGFXtextureInfo(CGFXdecomp.data(), "COMMON1", dataOffset1, height1, width1, mipmap1, formatID1, size1);
		if (ret > 0 || height1 != 0x80 || width1 != 0x100 || formatID1 != 0xC) {
			//bannerpreview.image(empty(0, 0));
			//customnotif.show();
			return 17;
		}

		uint32_t hash = CRC32(&CGFXdecomp[dataOffset1], size1);//get COMMON1 hash
		if (hash != 0x2DB769E8) {
			//bannerpreview.image(empty(0, 0));
			//customnotif.show();
			return 17;
		}
		int och = sizeof(nnc_u32);
		std::vector<uint8_t> output_pixels = std::vector<uint8_t>(w * h * och);
		nnc_unswizzle_zorder_le_rgb565_to_be_rgba8(reinterpret_cast<nnc_u16*>(&CGFXdecomp[dataOffset0]), reinterpret_cast<nnc_u32*>(output_pixels.data()), w & 0xFFFF, h & 0xFFFF);
		std::vector<uint8_t> output_cropped = std::vector<uint8_t>(out_w * out_w * och);
		crop_pixels(output_pixels.data(), w, h, och, output_cropped.data(), 0, 0, out_w, out_h);
		std::vector<uint8_t> output_film = std::vector<uint8_t>(film_w * film_h * 4);
		layer_pixels(output_film.data(), film_overlay, output_cropped.data(), film_w, film_h, 2, out_w, out_h, 4, 32, 11);
		//write banner png here
		std::cout << CreatingFile << ' ' << std::filesystem::absolute(outfile.c_str()) << std::endl;
		stbi_write_png(outfile.c_str(), film_w, film_h, 4, output_film.data(), 0);
		if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*outfile.c_str()), error)) {
			/*int blankret = generateBlankBanner(outfile);//we dont actually want blank banner since it's an output problem
			if(blankret)
				return blankret;*/
			return 18;
		}
		if (error) {
			std::cout << ErrorText << ' ' << outfile << '\n' << error.message() << std::endl;
			/*int blankret = generateBlankBanner(outfile);
			if(blankret)
				return blankret;*/
			return 19;
		}
		//bannerpreview.image(pixels_to_image(output_film.data(), film_w, film_h, 4));
		//bannererror.hide();
		//customnotif.hide();
		return 0;
	}
	std::string extension = infile;
	if(extension.find_last_of(".") != std::string::npos)
		extension.erase(extension.begin(), extension.begin() + extension.find_last_of("."));
	if (extension == ".bimg") {
		if (std::filesystem::file_size(std::filesystem::path((const char8_t*)&*infile.c_str()), error) == 0x10020) {
			w = 256;
			h = 128;
			int och = sizeof(nnc_u32);
			std::ifstream input;
			input.open(std::filesystem::path((const char8_t*)&*infile.c_str()), std::ios_base::in | std::ios_base::binary);//input file
			std::vector<uint8_t> input_data = std::vector<uint8_t>((w * h * ich) + 0x20);
			input.read(reinterpret_cast<char*>(input_data.data()), (w * h * ich) + 0x20);
			input.close();
			for (int i = 0; i < 0x1C; i++) {
				if (input_data[i] != bimgheader[i]) {
					//setDefaultBannerPreview(bannerpreview, &bannererror);
					return 20;
				}
			}
			std::vector<uint8_t> output_pixels = std::vector<uint8_t>(w * h * och);
			nnc_unswizzle_zorder_le_rgb565_to_be_rgba8(reinterpret_cast<nnc_u16*>(&input_data[0x20]), reinterpret_cast<nnc_u32*>(output_pixels.data()), w & 0xFFFF, h & 0xFFFF);
			std::vector<uint8_t> output_cropped = std::vector<uint8_t>(out_w * out_w * och);
			crop_pixels(output_pixels.data(), w, h, och, output_cropped.data(), 0, 0, out_w, out_h);
			std::vector<uint8_t> output_film = std::vector<uint8_t>(film_w * film_h * 4);
			layer_pixels(output_film.data(), film_overlay, output_cropped.data(), film_w, film_h, 2, out_w, out_h, 4, 32, 11);
			//write banner png here
			std::cout << CreatingFile << ' ' << std::filesystem::absolute(outfile.c_str()) << std::endl;
			stbi_write_png(outfile.c_str(), film_w, film_h, 4, output_film.data(), 0);
			if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*outfile.c_str()), error)) {
				/*int blankret = generateBlankBanner(outfile);
				if(blankret)
					return blankret;*/
				return 21;
			}
			if (error) {
				std::cout << ErrorText << ' ' << outfile << '\n' << error.message() << std::endl;
				/*int blankret = generateBlankBanner(outfile);
				if(blankret)
					return blankret;*/
				return 22;
			}
			//bannerpreview.image(pixels_to_image(output_film.data(), film_w, film_h, 4));
			//bannererror.hide();
			//customnotif.hide();
		}
		else {
			//setDefaultBannerPreview(bannerpreview, &bannererror);
		}
		if (error) {
			//xtd::forms::message_box::show(*this, xtd::ustring::format("{}\n{}", bannerbox.text(), error.message()), xtd::ustring::format("{}", ErrorText), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
			return 23;
		}
		return 0;
	}
	else if (!banner) {
		if (stbi_info(infile.c_str(), &w, &h, &ch)) {
			uint8_t* input_pixels = stbi_load(infile.c_str(), &w, &h, &ch, 0);
			if (input_pixels == NULL) {
				//maybe put a message here
				return 24;
			}
			std::vector<uint8_t> output_pixels = std::vector<uint8_t>(out_w * out_h * ch);

			if (w == out_w && h == out_h) memcpy(output_pixels.data(), input_pixels, w * h * ch);
			else resize_crop(input_pixels, w, h, output_pixels.data(), out_w, out_h, ch);//scale to 200x120 if needed
			free(input_pixels);
			std::vector<uint8_t> output_4c = std::vector<uint8_t>(out_w * out_h * 4);
			std::vector<uint8_t> white = std::vector<uint8_t>(out_w * out_h * 4);
			memset(white.data(), 0xFF, out_w * out_h * 4);
			layer_pixels(output_4c.data(), output_pixels.data(), white.data(), out_w, out_h, ch, out_w, out_h, 4, 0, 0);
			std::vector<uint8_t> output_film = std::vector<uint8_t>(film_w * film_h * 4);
			//memcpy(output_film, film_overlay, film_w * film_h * 4);
			layer_pixels(output_film.data(), film_overlay, output_4c.data(), film_w, film_h, 2, out_w, out_h, 4, 32, 11);
			//write banner png here
			std::cout << CreatingFile << ' ' << std::filesystem::absolute(outfile.c_str()) << std::endl;
			stbi_write_png(outfile.c_str(), film_w, film_h, 4, output_film.data(), 0);
			if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*outfile.c_str()), error)) {
				/*int blankret = generateBlankBanner(outfile);
				if(blankret)
					return blankret;*/
				return 25;
			}
			if (error) {
				std::cout << ErrorText << ' ' << outfile << '\n' << error.message() << std::endl;
				/*int blankret = generateBlankBanner(outfile);
				if(blankret)
					return blankret;*/
				return 26;
			}
			//bannerpreview.image(pixels_to_image(output_film.data(), film_w, film_h, 4));
			//bannererror.hide();
			//customnotif.hide();
			return 0;
		}
		/*else {//i dont think we even need this since we do it later anyway
			int blankret = generateBlankBanner(outfile);
			return 27 + blankret;//27 or 55 or 56 (this only happens if input is bad)
		}*/
	}
	//if we got here, something is wrong
	int blankret = generateBlankBanner(outfile);
	if(blankret)
		return 27 + blankret;
	return 28;
}

//TODO: write small icon?
int generateIconPreview(std::string infile, int borderMode, std::string outfile) {
	//first fix the outfile extension if needed
	{
		std::string outExtension = outfile;
		if(outExtension.find_last_of(".") != std::string::npos) {
			outExtension.erase(outExtension.begin(), outExtension.begin() + outExtension.find_last_of("."));
		}
		if(tolowerstr(outExtension) != ".png") {
			outfile += ".png";
		}
	}
	
	int w = 0, h = 0, comp = 0, ch = 0;
	uint16_t largeWH = 48;
	uint8_t* input_pixels;
	std::vector<uint8_t> output_pixels;
	std::vector<uint8_t> large_3c;
	//uint16_t smallLW = 24;
	const uint8_t FF = 0xFF;
	bool smdhinput = true;

	while(smdhinput)
	{
		nnc_smdh smdh;
		nnc_file f;

		if (nnc_file_open(&f, infile.c_str()) != NNC_R_OK) {
			smdhinput = false;
			break;
		}

		if (nnc_read_smdh(NNC_RSP(&f), &smdh) != NNC_R_OK) {
			smdhinput = false;
		}

		if (smdhinput) {

			/*if (DoText) {
				for (int i = 0; i < NNC_SMDH_TITLES; i++) {
					shortname.text(to_UTF8(smdh.titles[i].short_desc, sizeof(smdh.titles[i].short_desc) / 2));
					longname.text(to_UTF8(smdh.titles[i].long_desc, sizeof(smdh.titles[i].long_desc) / 2));
					publisher.text(to_UTF8(smdh.titles[i].publisher, sizeof(smdh.titles[i].publisher) / 2));
					if (!shortname.text().empty() && !longname.text().empty() && !publisher.text().empty())
						break;
				}
				borderMode = 0;//if we just wanted the image, this would not be wanted
			}*/
			
			ch = 4;
			output_pixels = std::vector<uint8_t>(largeWH * largeWH * ch);
			nnc_unswizzle_zorder_le_rgb565_to_be_rgba8(reinterpret_cast<nnc_u16*>(smdh.icon_large), reinterpret_cast<nnc_u32*>(output_pixels.data()), largeWH, largeWH);
		}
		NNC_RS_CALL0(f, close);
		break;
	}

	if (!smdhinput) {
		if (!stbi_info(infile.c_str(), &w, &h, &comp)) {
			w = largeWH;
			h = largeWH;
			ch = 4;
			input_pixels = (uint8_t*)malloc(largeWH * largeWH * ch);
			if (input_pixels == NULL) {
				return 30;
			}
			memset(input_pixels, FF, largeWH * largeWH * ch);
		}
		else {
			input_pixels = stbi_load(infile.c_str(), &w, &h, &ch, 0);
			if (input_pixels == NULL) {
				return 31;
			}
		}
		output_pixels = std::vector<uint8_t>(largeWH * largeWH * ch);
		if (w == largeWH && h == largeWH) memcpy(output_pixels.data(), input_pixels, w * h * ch);
		else resize_crop(input_pixels, w, h, output_pixels.data(), largeWH, largeWH, ch);//scale to 48x48 if needed
		free(input_pixels);
	}
	if (borderMode == 1) {//under border
		std::vector<uint8_t> output_4c = std::vector<uint8_t>(largeWH * largeWH * 4);
		std::vector<uint8_t> white_background = std::vector<uint8_t>(largeWH * largeWH * 4);//fix the bugs by not fixing the bugs! :D
		memset(white_background.data(), FF, largeWH * largeWH * 4);
		layer_pixels(output_4c.data(), output_pixels.data(), white_background.data(), largeWH, largeWH, ch, largeWH, largeWH, 4, 0, 0);//it warns about output_pixels being potentially uninitialized but that is impossible
		layer_pixels(output_4c.data(), icon_border48, output_4c.data(), largeWH, largeWH, 4, largeWH, largeWH, 4, 0, 0);
		ch = 4;// important later
		output_pixels = std::vector<uint8_t>(largeWH * largeWH * ch);
		memcpy(output_pixels.data(), output_4c.data(), largeWH * largeWH * ch);
	}
	else if (borderMode == 2) {//inside border
		std::vector<uint8_t> output_4c = std::vector<uint8_t>(largeWH * largeWH * 4);
		std::vector<uint8_t> white_background = std::vector<uint8_t>(largeWH * largeWH * 4);//fix the bugs by not fixing the bugs! :D
		memset(white_background.data(), FF, largeWH * largeWH * 4);
		layer_pixels(output_4c.data(), output_pixels.data(), white_background.data(), largeWH, largeWH, ch, largeWH, largeWH, 4, 0, 0);
		ch = 4;
		std::vector<uint8_t> scaled = std::vector<uint8_t>(largeWH * largeWH * ch);
		stbir_resize_uint8_linear(output_4c.data(), largeWH, largeWH, 0, scaled.data(), largeWH - 10, largeWH - 10, 0, STBIR_RGBA);//scale it down
		layer_pixels(output_4c.data(), icon_border48, scaled.data(), largeWH, largeWH, ch, largeWH - 10, largeWH - 10, ch, 5, 5);
		output_pixels = std::vector<uint8_t>(largeWH * largeWH * ch);
		memcpy(output_pixels.data(), output_4c.data(), largeWH * largeWH * ch);
	}

	large_3c = std::vector<uint8_t>(largeWH * largeWH * 3);
	if (ch == 4) {//rgba
		std::vector<uint8_t> white_background = std::vector<uint8_t>(largeWH * largeWH * 4);
		memset(white_background.data(), FF, largeWH * largeWH * 4);
		layer_pixels(output_pixels.data(), output_pixels.data(), white_background.data(), largeWH, largeWH, ch, largeWH, largeWH, 4, 0, 0);
		int newi = 0;
		for (int i = 0; i < largeWH * largeWH * ch; i += ch) {
			for (int c = 0; c < 3; c++)
				large_3c[newi + c] = output_pixels[i + c];
			newi += 3;
		}
	}
	else if (ch == 3) {//rgb
		memcpy(large_3c.data(), output_pixels.data(), largeWH * largeWH * ch);
	}
	else if (ch == 2) {//grayscale a
		std::vector<uint8_t> white_background = std::vector<uint8_t>(largeWH * largeWH * ch);
		std::vector<uint8_t> output_4c = std::vector<uint8_t>(largeWH * largeWH * 4);
		memset(white_background.data(), FF, largeWH * largeWH * ch);
		layer_pixels(output_4c.data(), output_pixels.data(), white_background.data(), largeWH, largeWH, ch, largeWH, largeWH, ch, 0, 0);
		int newi = 0;
		for (int i = 0; i < largeWH * largeWH * 4; i += 4) {
			for (int c = 0; c < 3; c++)
				large_3c[newi + c] = output_4c[i + c];
			newi += 3;
		}
	}
	else if (ch == 1) {//grayscale
		int ch1 = 0;
		for (int i = 0; i < largeWH * largeWH * 3; i += 3) {
			for (int c = 0; c < 3; c++)
				large_3c[i + c] = output_pixels[ch1];
			ch1++;
		}
	}
	//write icon png here
	std::cout << CreatingFile << ' ' << std::filesystem::absolute(outfile.c_str()) << std::endl;
	stbi_write_png(outfile.c_str(), largeWH, largeWH, 3, large_3c.data(), 0);
	//iconpreview.image(pixels_to_image(large_3c.data(), largeWH, largeWH, 3));
	//iconerror.hide();

	if ((!std::filesystem::exists(std::filesystem::path((const char8_t*)&*infile.c_str())) || (!stbi_info(infile.c_str(), &w, &h, &ch) && !smdhinput)) && !infile.empty()) {
		//iconerror.show();
		return 32;
	}
	return 0;
}

int generate_preview(std::string inpath, uint8_t number, std::string outpath) {
	VI9Pparameters parameters;
	
	int res = loadParameters(inpath, &parameters);
	//uint32_t outrealint = 0;
	switch(number) {
		case 0: {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << IntMultiParam << ")\n" << NothingToDo << std::endl;
			return 7;
		}
		case 1: {
			res = generateBannerPreview(parameters.banner, outpath);
			//std::cout << banner << std::endl;
			return res;
		}
		case 2: {
			res = generateIconPreview(parameters.icon, parameters.iconBorder, outpath);
			//std::cout << icon << std::endl;
			break;
		}
		case 3: {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << IntIconBorderParam << ")\n" << NothingToDo << std::endl;
			//iconBorder = ((outrealint & 0xFF) > 2 ? 2 : (outrealint & 0xFF));
			return 8;
		}
		case 4: {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << StrSNameParam << ")\n" << NothingToDo << std::endl;
			//Sname = newValue;
			return 9;
		}
		case 5: {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << StrLNameParam << ")\n" << NothingToDo << std::endl;
			//Lname = newValue;
			return 10;
		}
		case 6: {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << StrPublisherParam << ")\n" << NothingToDo << std::endl;
			//publisher = newValue;
			return 11;
		}
		case 7: {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << IntCopycheckParam << ")\n" << NothingToDo << std::endl;
			//copycheck = outrealint ? 1 : 0;
			return 12;
		}
		case 8: {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << StrCopyrightParam << ")\n" << NothingToDo << std::endl;
			//copyrightInfo = newValue;
			return 13;
		}
		case 9: {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << IntFFrewindParam << ")\n" << NothingToDo << std::endl;
			//FFrewind = outrealint ? 1 : 0;
			return 14;
		}
		case 10: {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << IntFadeOptParam << ")\n" << NothingToDo << std::endl;
			//FadeOpt = outrealint ? 1 : 0;
			return 15;
		}
		case 11: {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << IntSplitPatchParam << ")\n" << NothingToDo << std::endl;
			//splitPos = (rows > 1) ? (outrealint < rows ? outrealint : 1) : 0;
			return 16;
		}
	}
	//beware of incoming jank. tread lightly...
	uint8_t rowcase = 12;
	for((void)rowcase; rowcase < (parameters.PTitleVec.size() & 0xFF); rowcase++) {
		if(number == rowcase) {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << StrPTitleParam << ")\n" << NothingToDo << std::endl;
			return 17;
		}
	}
	rowcase = 12 + parameters.rows;
	for((void)rowcase; rowcase < (parameters.MoflexVec.size() & 0xFF); rowcase++) {
		if(number == rowcase) {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << StrMoflexParam << ")\n" << NothingToDo << std::endl;
			return 18;
		}
	}
	rowcase = 12 + (parameters.rows * 2);
	for(auto &row : parameters.MBannerVec) {
		if(number == rowcase) {
			res = generateBannerPreview(row, outpath, true);
			//std::cout << row << std::endl;
			return res;
		}
		rowcase++;
	}
	return res;
}


std::error_code Generate_Files(std::string dir, bool Multi) {
	std::error_code error;
	if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*dir.c_str()), error)) {
		if (error) {
			return error;
		}
		std::filesystem::create_directories(std::filesystem::path((const char8_t*)&*dir.c_str()), error);
	}
	if (error) {
		return error;
	}
	miniz_cpp::zip_file file;
	file.load(Multi ? Multivid : Singlevid);
	std::vector<std::string> list = file.namelist();
	for (auto& member : list) {//plant seeds
		if (member.find_last_of("/") == member.size() - 1) {
			std::filesystem::create_directory(std::filesystem::path((const char8_t*)&*std::string(dir + "/" + member).c_str()), error);
			if (error) {
				return error;
			}
		}
	}
	file.extractall(dir, list);//grow fruit (don't you mean grow tree?)
	return error;
}

bool TIDisValid(uint32_t TID) {//https://github.com/ihaveamac/videoinject/blob/master/videoinject.py#L9
	uint32_t min = 0xC0000;
	uint32_t max = 0xEFFFF;
	switch (TID)
	{
	case 0xc0d00://Side-Scrolling Example
	case 0xce1cc://CHMM
	case 0xd921e://homebrew launcher loader
	case 0xda001://Smash Bros Dummy Application
	case 0xda002://3ds quick shutdown
	case 0xda003://Wifi Toggle
	case 0xDEAD1://MCU Bricker
	case 0xe7a5a://NASA ALL
	case 0xec100://PKSM
	case 0xEC600://Video player for 3DS
	case 0xed990://NotifyMii
	case 0xED000://Google Translate
	case 0xEE3EE://MurderLands
	case 0xeffec://FileKong
	case 0xeffed://TriaAl
	case 0:
	{
		return false;
	}
	break;
	default:
		if (TID > max || TID < min) return false;
	}
	return true;
}

uint32_t RandomTID() {
	uint32_t min = 0xC0000;
	uint32_t max = 0xEFFFF;
	uint32_t TID = 0;
	static std::mt19937 rng;

	while (!TIDisValid(TID)) {//loop until we get a good value
		rng.seed(static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
		std::uniform_int_distribution<unsigned long> uniform(min, max);
		TID = uniform(rng);
	}
	return TID;
}

uint8_t convertToBimg(const std::string input, uint8_t* outBuffer, bool writeHeader)// true for write header, false for dont write header
{
	uint8_t* input_pixels;
	std::vector<uint8_t> output_pixels;
	std::vector<uint8_t> output_4c;
	std::vector<uint8_t> white;
	std::vector<uint8_t> output_fin;
	int w, h, ch, comp;
	const int new_w = 256;
	const int new_h = 128;
	const int out_w = 200;
	const int out_h = 120;
	const uint8_t FF = 0xFF;
	std::error_code error;
	if (std::filesystem::exists(std::filesystem::path((const char8_t*)&*input.c_str()), error)) {
		std::string extension = input;
		extension.erase(extension.begin(), extension.end() - 5);
		if (extension == ".bimg") {
			if (std::filesystem::file_size(std::filesystem::path((const char8_t*)&*input.c_str()), error) == 0x10020) {
				w = 256;
				h = 128;
				int ich = sizeof(nnc_u16);
				std::ifstream infile;
				infile.open(std::filesystem::path((const char8_t*)&*input.c_str()), std::ios_base::in | std::ios_base::binary);//input file
				std::vector<uint8_t> input_data = std::vector<uint8_t>((w * h * ich) + 0x20);
				infile.read(reinterpret_cast<char*>(input_data.data()), (w * h * ich) + 0x20);
				infile.close();
				for (int i = 0; i < 0x1C; i++) {
					if (input_data[i] != bimgheader[i]) {
						return 1;
					}
				}
				if (writeHeader) {
					memcpy(outBuffer, input_data.data(), (w * h * ich) + 0x20);
				}
				else {
					memcpy(outBuffer, &input_data[0x20], (w * h * ich));
				}
				return 0;
			}
			return 2;
		}
	}
	if(error)
		return 3;
	if (!stbi_info(input.c_str(), &w, &h, &comp)) {
		w = out_w;
		h = out_h;
		ch = 4;
		input_pixels = (uint8_t*)malloc(out_w * out_h * ch);
		if (input_pixels == NULL) {
			return 4;
		}
		memset(input_pixels, FF, out_w * out_h * ch);
	}
	else input_pixels = stbi_load(input.c_str(), &w, &h, &ch, 0);
	if (input_pixels == NULL) {
		return 5;
	}
	output_pixels = std::vector<uint8_t>(out_w * out_h * ch);
	if (w == out_w && h == out_h) memcpy(output_pixels.data(), input_pixels, w * h * ch);
	else resize_crop(input_pixels, w, h, output_pixels.data(), out_w, out_h, ch);//scale to 200x120 if needed
	free(input_pixels);

	output_4c = std::vector<uint8_t>(out_w * out_h * 4);
	ToRGBA(output_pixels.data(), output_4c.data(), out_w, out_h, ch);
	white = std::vector<uint8_t>(out_w * out_h * 4);
	//stbi_write_png("output_pixels.png", out_w, out_h, ch, output_pixels, 0);
	memset(white.data(), 0xFF, out_w * out_h * 4);
	layer_pixels(output_4c.data(), output_pixels.data(), white.data(), out_w, out_h, ch, out_w, out_h, 4, 0, 0);
	//stbi_write_png("output_4c.png", out_w, out_h, 4, output_pixels, 0);

	//layer 200x120 image on a 256x128 image
	output_fin = std::vector<uint8_t>(new_w * new_h * 4);

	memset(output_fin.data(), 0, new_w * new_h * 4);
	for (int i = 3; i < new_w * new_h * 4; i += 4)
		output_fin[i] = 0xFF;//make alpha 0xFF
	//layer_pixels(output_fin, output_4c, black, out_w, out_h, 4, new_w, new_h, 3, 0, 0);//why doesnt this work for this????
	for (int y = 0; y < out_h; y++)
		for (int x = 0; x < out_w; x++) {
			for (int c = 0; c < 4; c++)
				output_fin[(y * (new_w)+x) * 4 + c] = output_4c[(y * (out_w)+x) * 4 + c];
		}
	//stbi_write_png("output_fin.png", new_w, new_h, 4, output_fin, 0);

	uint8_t tiledbanner[new_w * new_h * sizeof(nnc_u16)];
	nnc_swizzle_zorder_be_rgba8_to_le_rgb565(reinterpret_cast<nnc_u32*>(output_fin.data()), reinterpret_cast<nnc_u16*>(tiledbanner), new_w, new_h);
	if (writeHeader) {
		memcpy(outBuffer, bimgheader, sizeof(bimgheader));
		memcpy(outBuffer + sizeof(bimgheader), tiledbanner, sizeof(tiledbanner));
	}
	else {
		memcpy(outBuffer, tiledbanner, sizeof(tiledbanner));
	}
	return 0;
}

uint8_t convertToIcon(const std::string input, std::string output, std::string shortname, std::string longname, std::string publisher, int borderMode) {//bare bones SMDH creation. thanks 3dbrew
	uint8_t* input_pixels;
	std::vector<uint8_t> output_pixels;
	std::vector<uint8_t> large_4c;
	std::vector<uint8_t> small_4c;
	int w, h, ch, comp;
	const int largeWH = 48;
	const int smallWH = 24;
	const uint8_t FF = 0xFF;
	bool smdhinput = true;
	nnc_smdh smdhIn;

	while (smdhinput)
	{
		nnc_file f;

		if (nnc_file_open(&f, input.c_str()) != NNC_R_OK) {
			smdhinput = false;
			break;
		}

		if (nnc_read_smdh(NNC_RSP(&f), &smdhIn) != NNC_R_OK) {
			smdhinput = false;
		}

		if (smdhinput) {
			ch = 4;
			output_pixels = std::vector<uint8_t>(largeWH * largeWH * ch);
			nnc_unswizzle_zorder_le_rgb565_to_be_rgba8(reinterpret_cast<nnc_u16*>(smdhIn.icon_large), reinterpret_cast<nnc_u32*>(output_pixels.data()), largeWH, largeWH);
		}
		NNC_RS_CALL0(f, close);
		break;
	}

	if (!smdhinput) {
		if (!stbi_info(input.c_str(), &w, &h, &comp)) {
			w = largeWH;
			h = largeWH;
			ch = 4;
			input_pixels = (uint8_t*)malloc(largeWH * largeWH * ch);
			if (input_pixels == NULL) {
				return 1;
			}
			memset(input_pixels, FF, largeWH * largeWH * ch);
		}
		else input_pixels = stbi_load(input.c_str(), &w, &h, &ch, 0);
		if (input_pixels == NULL) {
			return 2;
		}
		output_pixels = std::vector<uint8_t>(largeWH * largeWH * ch);
		if (w == largeWH && h == largeWH) memcpy(output_pixels.data(), input_pixels, w * h * ch);
		else resize_crop(input_pixels, w, h, output_pixels.data(), largeWH, largeWH, ch);// scale to 48x48 if needed
		free(input_pixels);
	}

	large_4c = std::vector<uint8_t>(largeWH * largeWH * 4);
	std::vector<uint8_t> white_background = std::vector<uint8_t>(largeWH * largeWH * 4);// fix the bugs by not fixing the bugs! :D
	memset(white_background.data(), FF, largeWH * largeWH * 4);
	layer_pixels(large_4c.data(), output_pixels.data(), white_background.data(), largeWH, largeWH, ch, largeWH, largeWH, 4, 0, 0);

	small_4c = std::vector<uint8_t>(smallWH * smallWH * 4);
	stbir_resize_uint8_linear(large_4c.data(), largeWH, largeWH, 0, small_4c.data(), smallWH, smallWH, 0, STBIR_RGBA);// make the small icon

	if (borderMode == 1) {
		layer_pixels(small_4c.data(), icon_border24, small_4c.data(), smallWH, smallWH, 4, smallWH, smallWH, 4, 0, 0);

		layer_pixels(large_4c.data(), icon_border48, large_4c.data(), largeWH, largeWH, 4, largeWH, largeWH, 4, 0, 0);
	}
	else if (borderMode == 2) {
		std::vector<uint8_t> scaled = std::vector<uint8_t>((smallWH - 6) * (smallWH - 6) * 4);
		stbir_resize_uint8_linear(small_4c.data(), smallWH, smallWH, 0, scaled.data(), smallWH - 6, smallWH - 6, 0, STBIR_RGBA);// scale it down
		layer_pixels(small_4c.data(), icon_border24, scaled.data(), smallWH, smallWH, 4, smallWH - 6, smallWH - 6, 4, 3, 3);

		scaled.clear();
		scaled = std::vector<uint8_t>((largeWH - 10) * (largeWH - 10) * 4);
		stbir_resize_uint8_linear(large_4c.data(), largeWH, largeWH, 0, scaled.data(), largeWH - 10, largeWH - 10, 0, STBIR_RGBA);// scale it down
		layer_pixels(large_4c.data(), icon_border48, scaled.data(), largeWH, largeWH, 4, largeWH - 10, largeWH - 10, 4, 5, 5);
	}

	uint8_t tiledsmall[smallWH * smallWH * sizeof(nnc_u16)];
	uint8_t tiledlarge[largeWH * largeWH * sizeof(nnc_u16)];
	if (borderMode == 0 && smdhinput) {
		memcpy(tiledsmall, smdhIn.icon_small, smallWH * smallWH * sizeof(nnc_u16));
		memcpy(tiledlarge, smdhIn.icon_large, largeWH * largeWH * sizeof(nnc_u16));
	}
	else {
		nnc_swizzle_zorder_be_rgba8_to_le_rgb565(reinterpret_cast<nnc_u32*>(small_4c.data()), reinterpret_cast<nnc_u16*>(tiledsmall), smallWH, smallWH);
		nnc_swizzle_zorder_be_rgba8_to_le_rgb565(reinterpret_cast<nnc_u32*>(large_4c.data()), reinterpret_cast<nnc_u16*>(tiledlarge), largeWH, largeWH);
	}
	//stbi_write_png("imag.png", largeWH, largeWH, 4, large_4c, 0);
	if (shortname.size() > 0x80 || longname.size() > 0x100 || publisher.size() > 0x80) {
		return 3;
	}
	std::ofstream smdhOut(std::filesystem::path((const char8_t*)&*output.c_str()), std::ios_base::out | std::ios_base::binary);
	smdhOut << "SMDH";//make smdh!
	for (int i = 0; i < 4; i++)
		smdhOut << '\0';
	for (int i = 0; i < 16; i++) {
		smdhOut << shortname;
		for (size_t j = 0; j < 0x80 - shortname.size(); j++)
			smdhOut << '\0';
		smdhOut << longname;
		for (size_t j = 0; j < 0x100 - longname.size(); j++)
			smdhOut << '\0';
		smdhOut << publisher;
		for (size_t j = 0; j < 0x80 - publisher.size(); j++)
			smdhOut << '\0';
	}
	for (int i = 0; i < 0x10; i++)
		smdhOut << '\0';
	smdhOut << "\xFF\xFF\xFF\x7F";//region free
	for (int i = 0; i < 0xC; i++)
		smdhOut << '\0';
	smdhOut << "\x01\x04";//visible, no save backups
	for (int i = 0; i < 0x16; i++)
		smdhOut << '\0';
	smdhOut.write(reinterpret_cast<const char*>(tiledsmall), sizeof(tiledsmall));
	smdhOut.write(reinterpret_cast<const char*>(tiledlarge), sizeof(tiledlarge));
	smdhOut.close();

	//stbi_write_png(output.c_str(), largeLW, largeLW, 3, large_3c, 0);
	return 0;
}


// Ported from: https://github.com/svn2github/3DS-Explorer/blob/master/3DSExplorer/DSDecmp/Formats/Nitro/LZ11.cs

uint32_t lz11_get_occurence_length(uint8_t* newPtr, uint32_t newLength, uint8_t* oldPtr, uint32_t oldLength, uint32_t* disp) {
	if (disp != NULL) {
		*disp = 0;
	}

	if (newLength == 0) {
		return 0;
	}

	uint32_t maxLength = 0;
	if (oldLength > 0) {
		for (uint32_t i = 0; i < oldLength - 1; i++) {
			uint8_t* currentOldStart = oldPtr + i;
			uint32_t currentLength = 0;
			for (uint32_t j = 0; j < newLength; j++) {
				if (*(currentOldStart + j) != *(newPtr + j)) {
					break;
				}

				currentLength++;
			}

			if (currentLength > maxLength) {
				maxLength = currentLength;
				if (disp != NULL) {
					*disp = oldLength - i;
				}

				if (maxLength == newLength) {
					break;
				}
			}
		}
	}

	return maxLength;
}

void* lz11_compress(uint32_t* size, void* input, uint32_t inputSize) {
	if (inputSize > 0xFFFFFF) {
		return NULL;
	}

	std::stringstream ss;

	uint8_t header[4] = { 0x11, (uint8_t)(inputSize & 0xFF), (uint8_t)((inputSize >> 8) & 0xFF), (uint8_t)((inputSize >> 16) & 0xFF) };
	ss.write((char*)header, 4);

	uint32_t compressedLength = 4;
	uint8_t outbuffer[8 * 4 + 1];
	outbuffer[0] = 0;
	uint32_t bufferlength = 1;
	uint32_t bufferedBlocks = 0;
	uint32_t readBytes = 0;
	while (readBytes < inputSize) {
		if (bufferedBlocks == 8) {
			ss.write((char*)outbuffer, bufferlength);
			compressedLength += bufferlength;
			outbuffer[0] = 0;
			bufferlength = 1;
			bufferedBlocks = 0;
		}

		uint32_t disp = 0;
		uint32_t oldLength = MIN(readBytes, 0x1000);
		uint32_t length = lz11_get_occurence_length((uint8_t*)input + readBytes, MIN(inputSize - readBytes, 0x10110), (uint8_t*)input + readBytes - oldLength, oldLength, &disp);
		if (length < 3) {
			outbuffer[bufferlength++] = *((uint8_t*)input + (readBytes++));
		}
		else {
			readBytes += length;
			outbuffer[0] |= (uint8_t)(1 << (7 - bufferedBlocks));
			if (length > 0x110) {
				outbuffer[bufferlength] = 0x10;
				outbuffer[bufferlength] |= (uint8_t)(((length - 0x111) >> 12) & 0x0F);
				bufferlength++;
				outbuffer[bufferlength] = (uint8_t)(((length - 0x111) >> 4) & 0xFF);
				bufferlength++;
				outbuffer[bufferlength] = (uint8_t)(((length - 0x111) << 4) & 0xF0);
			}
			else if (length > 0x10) {
				outbuffer[bufferlength] = 0x00;
				outbuffer[bufferlength] |= (uint8_t)(((length - 0x111) >> 4) & 0x0F);
				bufferlength++;
				outbuffer[bufferlength] = (uint8_t)(((length - 0x111) << 4) & 0xF0);
			}
			else {
				outbuffer[bufferlength] = (uint8_t)(((length - 1) << 4) & 0xF0);
			}

			outbuffer[bufferlength] |= (uint8_t)(((disp - 1) >> 8) & 0x0F);
			bufferlength++;
			outbuffer[bufferlength] = (uint8_t)((disp - 1) & 0xFF);
			bufferlength++;
		}

		bufferedBlocks++;
	}

	if (bufferedBlocks > 0) {
		ss.write((char*)outbuffer, bufferlength);
		compressedLength += bufferlength;
	}

	if (compressedLength % 4 != 0) {
		uint32_t padLength = 4 - (compressedLength % 4);
		uint8_t* pad = new uint8_t[padLength];
		memset(pad, 0, (size_t)padLength);

		ss.write((char*)pad, padLength);
		compressedLength += padLength;
		delete[] pad;
	}

	void* buf = malloc((size_t)compressedLength);
	ss.read((char*)buf, compressedLength);

	if (size != NULL) {
		*size = (uint32_t)compressedLength;
	}

	return buf;
}

void* cbmd_build_data(uint32_t* size, CBMD cbmd) {//this is here because it has to be static for some reason (it's from bannertool)
	CBMDHeader header;
	memset(&header, 0, sizeof(header));

	memcpy(header.magic, "CBMD", sizeof(header.magic));

	uint32_t outputSize = sizeof(CBMDHeader);

	void* compressedCGFXs[14] = { NULL };
	uint32_t compressedCGFXSizes[14] = { 0 };
	for (uint32_t i = 0; i < CBMD_NUM_CGFXS; i++) {
		if (cbmd.cgfxs[i] != NULL) {
			header.cgfxOffsets[i] = outputSize;

			compressedCGFXs[i] = lz11_compress(&compressedCGFXSizes[i], cbmd.cgfxs[i], cbmd.cgfxSizes[i]);
			outputSize += compressedCGFXSizes[i];
		}
	}

	outputSize = (outputSize + 0xF) & ~0xF;

	if (cbmd.cwav != NULL) {
		header.cwavOffset = outputSize;

		outputSize += cbmd.cwavSize;
	}

	void* output = calloc(outputSize, sizeof(uint8_t));
	if (output == NULL) {
		for (uint32_t i = 0; i < CBMD_NUM_CGFXS; i++) {
			if (cbmd.cgfxs[i] != NULL) {
				free(compressedCGFXs[i]);
			}
		}
		return NULL;
	}

	memcpy(output, &header, sizeof(header));

	for (uint32_t i = 0; i < CBMD_NUM_CGFXS; i++) {
		if (compressedCGFXs[i] != NULL) {
			memcpy(&((uint8_t*)output)[header.cgfxOffsets[i]], compressedCGFXs[i], compressedCGFXSizes[i]);
			free(compressedCGFXs[i]);
		}
	}

	if (cbmd.cwav != NULL) {
		memcpy(&((uint8_t*)output)[header.cwavOffset], cbmd.cwav, cbmd.cwavSize);
	}

	if (size != NULL) {
		*size = outputSize;
	}

	return output;
}

uint32_t Get_Decompressed_size(const uint8_t* in) {
	uint32_t decompressed_size;
	memcpy(&decompressed_size, in, sizeof(uint32_t));
	in += 4;

	uint8_t type = decompressed_size & 0xFF;
	if (type != 0x11) return 0xFFFFFFFF;
	decompressed_size >>= 8;
	return decompressed_size;
}

uint32_t DecompressLZ11(const uint8_t* in, uint8_t* out) {//https://github.com/citra-emu/citra/blob/bfb6a5b5de1e7a89ceebabe33e792d5c03a2cf46/src/core/hle/service/apt/apt.cpp#L135
	uint32_t decompressed_size;
	memcpy(&decompressed_size, in, sizeof(uint32_t));
	in += 4;

	uint8_t type = decompressed_size & 0xFF;
	if (type != 0x11) return 0xFFFFFFFF;
	decompressed_size >>= 8;

	uint32_t current_out_size = 0;
	uint8_t flags = 0, mask = 1;
	while (current_out_size < decompressed_size) {
		if (mask == 1) {
			flags = *(in++);
			mask = 0x80;
		}
		else {
			mask >>= 1;
		}

		if (flags & mask) {
			uint8_t byte1 = *(in++);
			uint32_t length = byte1 >> 4;
			uint32_t offset;
			if (length == 0) {
				uint8_t byte2 = *(in++);
				uint8_t byte3 = *(in++);
				length = (((byte1 & 0x0F) << 4) | (byte2 >> 4)) + 0x11;
				offset = (((byte2 & 0x0F) << 8) | byte3) + 0x1;
			}
			else if (length == 1) {
				uint8_t byte2 = *(in++);
				uint8_t byte3 = *(in++);
				uint8_t byte4 = *(in++);
				length = (((byte1 & 0x0F) << 12) | (byte2 << 4) | (byte3 >> 4)) + 0x111;
				offset = (((byte3 & 0x0F) << 8) | byte4) + 0x1;
			}
			else {
				uint8_t byte2 = *(in++);
				length = (byte1 >> 4) + 0x1;
				offset = (((byte1 & 0x0F) << 8) | byte2) + 0x1;
			}

			for (uint32_t i = 0; i < length; i++) {
				*out = *(out - offset);
				++out;
			}

			current_out_size += length;
		}
		else {
			*(out++) = *(in++);
			current_out_size++;
		}
	}
	return decompressed_size;
}

uint8_t getCGFXtextureInfo(uint8_t* CGFX, const std::string symbol, uint32_t& dataOffset, uint32_t& height, uint32_t& width, uint32_t& mipmap, uint32_t& formatID, uint32_t& size) {
	uint32_t* CGFXmagic = reinterpret_cast<uint32_t*>(&CGFX[0]);
	if (*CGFXmagic != 0x58464743) {//CGFX
		return 1;
	}

	uint16_t* CGFXheaderSize = reinterpret_cast<uint16_t*>(&CGFX[0x6]);
	for (uint32_t N = 0; N < *reinterpret_cast<uint32_t*>(&CGFX[0x10]); N++) {//loop through Number of Dicts
		uint32_t DICToffset = *reinterpret_cast<uint32_t*>(&CGFX[*CGFXheaderSize + 0xC + (N * 8)]);
		DICToffset += (*CGFXheaderSize + 0xC + (N * 8));//since it's self-relative, do this
		for (uint32_t i = 0; i < *reinterpret_cast<uint32_t*>(&CGFX[DICToffset + 0x8]); i++) {//loop through entries in DICT N
			uint32_t symbolOffset = *reinterpret_cast<uint32_t*>(&CGFX[DICToffset + 0x1C + (0x10 * i) + 0x8]);
			symbolOffset += (DICToffset + 0x1C + (0x10 * i) + 0x8);//self-relative
			uint32_t objectOffset = *reinterpret_cast<uint32_t*>(&CGFX[DICToffset + 0x1C + (0x10 * i) + 0xC]);
			objectOffset += (DICToffset + 0x1C + (0x10 * i) + 0xC);//self-relative
			std::string isymbol = "";
			const auto* ch = &CGFX[symbolOffset];
			while (*ch)
				isymbol += *ch++;
			if (strcmp(isymbol.c_str(), symbol.c_str()) == 0) {
				height = *reinterpret_cast<uint32_t*>(&CGFX[objectOffset + 0x18]);
				width = *reinterpret_cast<uint32_t*>(&CGFX[objectOffset + 0x1C]);
				mipmap = *reinterpret_cast<uint32_t*>(&CGFX[objectOffset + 0x28]);
				formatID = *reinterpret_cast<uint32_t*>(&CGFX[objectOffset + 0x34]);
				size = *reinterpret_cast<uint32_t*>(&CGFX[objectOffset + 0x44]);
				dataOffset = *reinterpret_cast<uint32_t*>(&CGFX[objectOffset + 0x48]);
				dataOffset += (objectOffset + 0x48);//self-relative
				return 0;
			}
		}
	}
	return 2;
}

uint8_t getCBMDInfo(const std::string inpath, uint32_t* compressedSize, uint32_t* decompressedSize, uint32_t* CGFXoffset) {
	std::ifstream CBMD;
	CBMD.open(std::filesystem::path((const char8_t*)&*inpath.c_str()), std::ios_base::in | std::ios_base::binary);
	uint32_t CBMDmagic = 0;
	CBMD.seekg(0);
	CBMD.read(reinterpret_cast<char*>(&CBMDmagic), 0x4);
	if (CBMDmagic != 0x444D4243) {
		return 1;
	}
	//Offset for common CGFX
	uint32_t _CGFXoffset = 0;
	CBMD.seekg(0x8);
	CBMD.read(reinterpret_cast<char*>(&_CGFXoffset), 0x4);
	//BCWAV offset
	uint32_t BCWAVoffset = 0;
	CBMD.seekg(0x84);
	CBMD.read(reinterpret_cast<char*>(&BCWAVoffset), 0x4);
	std::vector<uint8_t> CGFX = std::vector<uint8_t>(BCWAVoffset - _CGFXoffset);
	//get stuff and decompress that stuff
	CBMD.seekg(_CGFXoffset);
	CBMD.read(reinterpret_cast<char*>(CGFX.data()), BCWAVoffset - _CGFXoffset);

	uint32_t decompressedSize_ = Get_Decompressed_size(CGFX.data());
	if (decompressedSize_ > 0x80000) {
		//delete[] CGFX;
		return 2;
	}
	*decompressedSize = decompressedSize_;
	*compressedSize = BCWAVoffset - _CGFXoffset;
	*CGFXoffset = _CGFXoffset;
	//delete[] CGFX;
	return 0;
}

uint8_t CBMDgetCommonCGFX(const std::string inpath, const uint32_t compressedSize, const uint32_t CGFXoffset, uint8_t* outbuff) {
	std::ifstream CBMD;
	CBMD.open(std::filesystem::path((const char8_t*)&*inpath.c_str()), std::ios_base::in | std::ios_base::binary);
	uint32_t CBMDmagic = 0;
	CBMD.seekg(0);
	CBMD.read(reinterpret_cast<char*>(&CBMDmagic), 0x4);
	if (CBMDmagic != 0x444D4243) {
		return 3;
	}
	std::vector<uint8_t> CGFX = std::vector<uint8_t>(compressedSize);
	//get stuff and decompress that stuff
	CBMD.seekg(CGFXoffset);
	CBMD.read(reinterpret_cast<char*>(CGFX.data()), compressedSize);

	if (DecompressLZ11(CGFX.data(), outbuff) == 0) {
		//delete[] CGFX;
		return 4;
	}
	//delete[] CGFX;

	uint32_t* CGFXmagic = reinterpret_cast<uint32_t*>(&outbuff[0]);
	if (*CGFXmagic != 0x58464743) {//CGFX
		return 5;
	}
	return 0;
}

uint8_t getCBMDTexture(const std::string inpath, const std::string symbol, uint8_t* outbuff) {
	uint32_t decompressedSize = 0;
	uint32_t compressedSize = 0;
	uint32_t CGFXoffset = 0;
	uint8_t ret = getCBMDInfo(inpath, &compressedSize, &decompressedSize, &CGFXoffset);
	if (ret > 0) {
		return ret;
	}
	std::vector<uint8_t> CGFXdecomp = std::vector<uint8_t>(decompressedSize);
	ret = CBMDgetCommonCGFX(inpath, compressedSize, CGFXoffset, CGFXdecomp.data());
	if (ret > 0) {
		//delete[] CGFXdecomp;
		return ret;
	}

	uint32_t dataOffset;
	uint32_t height;
	uint32_t width;
	uint32_t mipmap;
	uint32_t formatID;
	uint32_t size;
	ret = getCGFXtextureInfo(CGFXdecomp.data(), symbol, dataOffset, height, width, mipmap, formatID, size);
	if (ret == 0) {
		memcpy(outbuff, &CGFXdecomp[dataOffset], size);
		//delete[] CGFXdecomp;
		return ret;
	}
	//delete[] CGFXdecomp;
	return ret;
}

int setIcon(std::string inpath, std::string &Sname, std::string &Lname, std::string &Publisher) {
	bool smdhinput = true;
	
	while(smdhinput)
	{
		nnc_smdh smdh;
		nnc_file f;

		if (nnc_file_open(&f, inpath.c_str()) != NNC_R_OK) {
			smdhinput = false;
			return 7;
		}

		if (nnc_read_smdh(NNC_RSP(&f), &smdh) != NNC_R_OK) {
			smdhinput = false;
			NNC_RS_CALL0(f, close);
			return 8;
		}

		if (smdhinput) {
			for (int i = 0; i < NNC_SMDH_TITLES; i++) {
				Sname = to_UTF8(smdh.titles[i].short_desc, sizeof(smdh.titles[i].short_desc) / 2);
				Lname = to_UTF8(smdh.titles[i].long_desc, sizeof(smdh.titles[i].long_desc) / 2);
				Publisher = to_UTF8(smdh.titles[i].publisher, sizeof(smdh.titles[i].publisher) / 2);
				if (!Sname.empty() && !Lname.empty() && !Publisher.empty())
					break;
			}
		}
		NNC_RS_CALL0(f, close);
		return 0;
	}
	return 8;
}

int SetSMDH(std::string inpath, std::string Newicon, std::string outpath) {
	VI9Pparameters parameters;
	
	int res = loadParameters(inpath, &parameters);
	
	parameters.icon = Newicon;
	
	res = setIcon(parameters.icon, parameters.Sname, parameters.Lname, parameters.publisher);
	
	saveParameters(outpath, parameters);
	
	return res;
}

int build_archive(std::string inVi9p, std::string outCIA, std::string outTAR, uint32_t uniqueID, std::string ApplicationName, std::string ProductCode) {
	//default finalization stuff
	/*uint32_t uniqueID = RandomTID();
	std::string ApplicationName = "video";
	std::string ProductCode = "VDIJ";*/
	char uniqueIDstr [6];
	sprintf(uniqueIDstr, "%05X", uniqueID);
	std::cout << UniqueIDText << ": " << uniqueIDstr << std::endl;

	VI9Pparameters parameters;
	
	int res = loadParameters(inVi9p, &parameters);
	
	bool dopatch = 0;
	std::error_code error;
	
	std::string tempPath = resourcesPath + "/CLItemp";
	std::string romfsPath;
	do {
		if (dopatch) {
			romfsPath = tempPath + "/luma/titles/000400000" + uniqueIDstr + "00/romfs";
			std::cout << romfsPath << std::endl;
		}
		else {
			romfsPath = tempPath + "/romfs";
		}
		//extract base
		{
			std::filesystem::remove_all(std::filesystem::path((const char8_t*)&*tempPath.c_str()), error);
			if (error) {
				std::cout << ErrorText << ' ' << tempPath << '\n' << error.message() << std::endl;
				std::cout << ErrorText << ' ' << FailedToCreateFile << " \"" << (dopatch ? outTAR : outCIA) << '\"' << std::endl;
				return 7;
			}
		}
		if(!dopatch)
			Generate_Files(tempPath.c_str(), parameters.mode);
		std::cout << CreatingFile << " romfs" << std::endl;
		//make movie_title.csv (player title)
		{
			std::cout << CreatingFile << " romfs/movie_title.csv" << std::endl;
			
			std::filesystem::create_directories(std::filesystem::path((const char8_t*)&*std::string(romfsPath + "/movie").c_str()), error);
			if (error) {
				std::cout << ErrorText << ' ' << romfsPath + "/movie" << '\n' << error.message() << std::endl;
				std::cout << ErrorText << ' ' << FailedToCreateFile << " \"" << (dopatch ? outTAR : outCIA) << '\"' << std::endl;
				return 8;
			}
			//if(dopatch) return 0;
			std::ofstream movie_title(std::string(romfsPath + "/movie/movie_title.csv").c_str(), std::ios_base::out | std::ios_base::binary);

			movie_title << "\xFF\xFE" + UTF8toUTF16("#JP,#EN,#FR,#GE,#IT,#SP,#CH,#KO,#DU,#PO,#RU,#TW\x0D\x0A");
			for (int i = 0; i < (parameters.mode ? ((parameters.splitPos && !dopatch) ? parameters.splitPos : parameters.rows) : 1); i++) {
				std::string outstr = parameters.PTitleVec.at(i);

				if (outstr[0] == '#') {//sneakily fix the string huhuhu
					outstr[0] = '\\';
					outstr.insert(1, "x23");
				}
				for (size_t j = 0; j < outstr.size(); j++) {
					if (outstr[j] == ',') {
						outstr[j] = '\\';
						outstr.insert(j + 1, "x2C");
					}
				}
				for (size_t j = 0; j < outstr.size(); j++) {
					if (outstr[j] == '\n') {
						outstr[j] = '\\';
						outstr.insert(j + 1, "n");
					}
				}
				for (int j = 0; j < 11; j++) {//do it 11 times because it needs to
					movie_title << UTF8toUTF16(outstr + ",");
				}
				movie_title << UTF8toUTF16(outstr + "\x0D\x0A");//put the last stuff
			}
			movie_title.close();
			if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(romfsPath + "/movie/movie_title.csv").c_str()), error)) {
				std::cout << ErrorText << ' ' << FailedToFindPath << '\n' << FailedToCreateFile << ' ' << romfsPath << "/movie/movie_title.csv" << std::endl;
				return 9;
			}
			if(error)
				return 10;
		}
		//make settingsTL.csv (menu title and stuff)
		{
			std::cout << CreatingFile << " romfs/settings/settingsTL.csv" << std::endl;
			
			std::filesystem::create_directories(std::filesystem::path((const char8_t*)&*std::string(romfsPath + "/settings").c_str()), error);
			if (error) {
				std::cout << ErrorText << ' ' << romfsPath + "/settings" << '\n' << error.message() << std::endl;
				std::cout << ErrorText << ' ' << FailedToCreateFile << " \"" << (dopatch ? outTAR : outCIA) << '\"' << std::endl;
				return 11;
			}
			std::ofstream settingsTL(std::string(romfsPath + "/settings/settingsTL.csv").c_str(), std::ios_base::out | std::ios_base::binary);

			std::string outlongname = parameters.Lname;
			if (outlongname[0] == '#') {//sneakily fix the string huhuhu
				outlongname[0] = '\\';
				outlongname.insert(1, "x23");
			}
			for (size_t j = 0; j < outlongname.size(); j++) {
				if (outlongname[j] == ',') {
					outlongname[j] = '\\';
					outlongname.insert(j + 1, "x2C");
				}
			}
			for (size_t j = 0; j < outlongname.size(); j++) {
				if (outlongname[j] == '\n') {
					outlongname[j] = '\\';
					outlongname.insert(j + 1, "n");
				}
			}

			std::string outpublisher = parameters.publisher;
			if (outpublisher[0] == '#') {//sneakily fix the string huhuhu
				outpublisher[0] = '\\';
				outpublisher.insert(1, "x23");
			}
			for (size_t j = 0; j < outpublisher.size(); j++) {
				if (outpublisher[j] == ',') {
					outpublisher[j] = '\\';
					outpublisher.insert(j + 1, "x2C");
				}
			}
			for (size_t j = 0; j < outpublisher.size(); j++) {
				if (outpublisher[j] == '\n') {
					outpublisher[j] = '\\';
					outpublisher.insert(j + 1, "n");
				}
			}

			settingsTL << "\xFF\xFE" +
				UTF8toUTF16("# おしらせURL\x0D\x0A"//hard to read because of line breaks but hey better than hex
					"# JP:\x0D\x0A"
					"none\x0D\x0A"
					"\x0D\x0A"
					"# EN:\x0D\x0A"
					"none\x0D\x0A"
					"\x0D\x0A"
					"# FR:\x0D\x0A"
					"none\x0D\x0A"
					"\x0D\x0A"
					"# GE:\x0D\x0A"
					"none\x0D\x0A"
					"\x0D\x0A"
					"# IT:\x0D\x0A"
					"none\x0D\x0A"
					"\x0D\x0A"
					"# SP:\x0D\x0A"
					"none\x0D\x0A"
					"\x0D\x0A"
					"# CN:\x0D\x0A"
					"none\x0D\x0A"
					"\x0D\x0A"
					"# KO:\x0D\x0A"
					"none\x0D\x0A"
					"\x0D\x0A"
					"# DU:\x0D\x0A"
					"none\x0D\x0A"
					"\x0D\x0A"
					"# PO:\x0D\x0A"
					"none\x0D\x0A"
					"\x0D\x0A"
					"# RU:\x0D\x0A"
					"none\x0D\x0A"
					"\x0D\x0A"
					"# TW:\x0D\x0A"
					"none\x0D\x0A"
					"\x0D\x0A"
					"# アプリ名（ロングネーム）\x0D\x0A"//app long name
					"# JP:\x0D\x0A"
					+ outlongname + "\x0D\x0A"
					"\x0D\x0A"
					"# EN:\x0D\x0A"
					+ outlongname + "\x0D\x0A"
					"\x0D\x0A"
					"# FR:\x0D\x0A"
					+ outlongname + "\x0D\x0A"
					"\x0D\x0A"
					"# GE:\x0D\x0A"
					+ outlongname + "\x0D\x0A"
					"\x0D\x0A"
					"# IT:\x0D\x0A"
					+ outlongname + "\x0D\x0A"
					"\x0D\x0A"
					"# SP:\x0D\x0A"
					+ outlongname + "\x0D\x0A"
					"\x0D\x0A"
					"# CN:\x0D\x0A"
					+ outlongname + "\x0D\x0A"
					"\x0D\x0A"
					"# KO:\x0D\x0A"
					+ outlongname + "\x0D\x0A"
					"\x0D\x0A"
					"# DU:\x0D\x0A"
					+ outlongname + "\x0D\x0A"
					"\x0D\x0A"
					"# PO:\x0D\x0A"
					+ outlongname + "\x0D\x0A"
					"\x0D\x0A"
					"# RU:\x0D\x0A"
					+ outlongname + "\x0D\x0A"
					"\x0D\x0A"
					"# TW:\x0D\x0A"
					+ outlongname + "\x0D\x0A"
					"\x0D\x0A"
					"# 拡張セーブデータのID（16進数）\x0D\x0A"//save data ID
					+ uniqueIDstr + "\x0D\x0A"//make it the save as unique ID because yes
					"\x0D\x0A"
					"# NADLタスクのID\x0D\x0A"
					"none\x0D\x0A"
					"\x0D\x0A"
					"# タスクの実行間隔（h）（10進数）\x0D\x0A"
					"0\x0D\x0A"
					"\x0D\x0A"
					"# タスクの実行回数（10進数）\x0D\x0A"
					"0\x0D\x0A"
					"\x0D\x0A"
					"# おしらせのあり、なし\x0D\x0A"//not sure what this is, but if you enable it in single vid it instantly crashes... maybe it's the thing telling you to take a break? nah because it's false and that still appears
					"false\x0D\x0A"
					"\x0D\x0A"
					"# 早送り、巻戻しボタンのあり、なし\x0D\x0A"//ff rewind
					+ (parameters.FFrewind ? "true" : "false") + "\x0D\x0A"
					"\x0D\x0A"
					"# 優しさ演出のあり、なし\x0D\x0A"//gentleness
					+ (parameters.FadeOpt ? "true" : "false") + "\x0D\x0A");

			if (parameters.mode) {
				settingsTL << UTF8toUTF16("\x0D\x0A"
					"# 動画の数\x0D\x0A"//amount of videos
					+ std::to_string((parameters.splitPos && !dopatch) ? parameters.splitPos : parameters.rows) + "\x0D\x0A"
					"\x0D\x0A"
					"# 動画パブリッシャー名\x0D\x0A"//publisher name
					"# JP:\x0D\x0A"
					+ outpublisher + "\x0D\x0A"
					"\x0D\x0A"
					"# EN:\x0D\x0A"
					+ outpublisher + "\x0D\x0A"
					"\x0D\x0A"
					"# FR:\x0D\x0A"
					+ outpublisher + "\x0D\x0A"
					"\x0D\x0A"
					"# GE:\x0D\x0A"
					+ outpublisher + "\x0D\x0A"
					"\x0D\x0A"
					"# IT:\x0D\x0A"
					+ outpublisher + "\x0D\x0A"
					"\x0D\x0A"
					"# SP:\x0D\x0A"
					+ outpublisher + "\x0D\x0A"
					"\x0D\x0A"
					"# CN:\x0D\x0A"
					+ outpublisher + "\x0D\x0A"
					"\x0D\x0A"
					"# KO:\x0D\x0A"
					+ outpublisher + "\x0D\x0A"
					"\x0D\x0A"
					"# DU:\x0D\x0A"
					+ outpublisher + "\x0D\x0A"
					"\x0D\x0A"
					"# PO:\x0D\x0A"
					+ outpublisher + "\x0D\x0A"
					"\x0D\x0A"
					"# RU:\x0D\x0A"
					+ outpublisher + "\x0D\x0A"
					"\x0D\x0A"
					"# TW:\x0D\x0A"
					+ outpublisher + "\x0D\x0A"
					"\x0D\x0A"
					"# WEBブラウザ用のURL\x0D\x0A"//web browser URL (?)
					"# JP:\x0D\x0A"
					"\x0D\x0A"
					"\x0D\x0A"
					"# EN:\x0D\x0A"
					"\x0D\x0A"
					"\x0D\x0A"
					"# FR:\x0D\x0A"
					"\x0D\x0A"
					"\x0D\x0A"
					"# GE:\x0D\x0A"
					"\x0D\x0A"
					"\x0D\x0A"
					"# IT:\x0D\x0A"
					"\x0D\x0A"
					"\x0D\x0A"
					"# SP:\x0D\x0A"
					"\x0D\x0A"
					"\x0D\x0A"
					"# CN:\x0D\x0A"
					"\x0D\x0A"
					"\x0D\x0A"
					"# KO:\x0D\x0A"
					"\x0D\x0A"
					"\x0D\x0A"
					"# DU:\x0D\x0A"
					"\x0D\x0A"
					"\x0D\x0A"
					"# PO:\x0D\x0A"
					"\x0D\x0A"
					"\x0D\x0A"
					"# RU:\x0D\x0A"
					"\x0D\x0A"
					"\x0D\x0A"
					"# TW:");
			}
			settingsTL.close();
			if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(romfsPath + "/settings/settingsTL.csv").c_str()), error)) {
				std::cout << ErrorText << ' ' << FailedToFindPath << '\n' << FailedToCreateFile << ' ' << romfsPath << "/settings/settingsTL.csv" << std::endl;
				return 12;
			}
			if(error)
				return 13;
		}
		//make copyright stuff (multi vid only)
		if (parameters.mode && !dopatch) {
			std::cout << CreatingFile << " romfs/settings/information_buttons.csv" << std::endl;
			
			std::filesystem::create_directories(std::string(romfsPath + "/settings").c_str(), error);//just in case Hehehhhah
			if (error) {
				std::cout << ErrorText << ' ' << romfsPath + "/settings" << '\n' << error.message() << std::endl;
				std::cout << ErrorText << ' ' << FailedToCreateFile << " \"" << (dopatch ? outTAR : outCIA) << '\"' << std::endl;
				return 14;
			}
			std::ofstream information_buttons(std::string(romfsPath + "/settings/information_buttons.csv").c_str(), std::ios_base::out | std::ios_base::binary);
			information_buttons << (parameters.copycheck ? ("\xFF\xFE" + UTF8toUTF16("Copyright")) : "\xFF\xFE");
			information_buttons.close();
			if (!std::filesystem::exists(std::string(romfsPath + "/settings/information_buttons.csv").c_str(), error)) {
				std::cout << ErrorText << ' ' << FailedToFindPath << '\n' << FailedToCreateFile << ' ' << romfsPath << "/settings/information_buttons.csv" << std::endl;
				return 15;
			}
			if(error)
				return 16;

			if (parameters.copycheck) {
				std::cout << CreatingFile << " romfs/settings/copyright.txt" << std::endl;
				
				std::ofstream copyrighttxt(std::string(romfsPath + "/settings/copyright.txt").c_str(), std::ios_base::out | std::ios_base::binary);
				copyrighttxt << "\xFF\xFE" + UTF8toUTF16(parameters.copyrightInfo);
				copyrighttxt.close();
				if (!std::filesystem::exists(std::string(romfsPath + "/settings/copyright.txt").c_str(), error)) {
					std::cout << ErrorText << ' ' << FailedToFindPath << '\n' << FailedToCreateFile << ' ' << romfsPath << "/settings/copyright.txt" << std::endl;
					return 17;
				}
				if(error)
					return 18;
			}
		}
		//copy moflex
		{
			uint8_t Checker[4];
			for (int i = dopatch ? parameters.splitPos : 0; i < (parameters.mode ? ((parameters.splitPos && !dopatch) ? parameters.splitPos : parameters.rows) : 1); i++) {
				if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*parameters.MoflexVec.at(i).c_str()), error)) {
					std::cout << ErrorText << ' ' << FailedToFindPath << " \"" << parameters.MoflexVec.at(i) << "\"\n" << StrMoflexParam << '(' << i << ')' << std::endl;
					return 19;
				}
				if(error)
					return 20;
				std::string extension = parameters.MoflexVec.at(i).c_str();
				if (extension.find_last_of(".") != std::string::npos)
					extension.erase(extension.begin(), extension.begin() + extension.find_last_of("."));
				std::ifstream inmoflex(std::filesystem::path((const char8_t*)&*parameters.MoflexVec.at(i).c_str()), std::ios_base::in | std::ios::binary);
				for (int j = 0; j < 4; j++) {
					inmoflex >> Checker[j];//https://stackoverflow.com/a/2974735
					if (extension != ".moflex" || Checker[j] != moflexMagic[j]) {
						std::cout << ErrorText << ' ' << BadValue << "\n\"" << parameters.MoflexVec.at(i) << "\" " << MoflexError << std::endl;
						return 21;
					}
				}
				std::filesystem::create_directories(std::filesystem::path((const char8_t*)&*std::string(romfsPath + "/movie").c_str()), error);
				if (error) {
					std::cout << ErrorText << ' ' << romfsPath + "/movie" << '\n' << error.message() << std::endl;
					std::cout << ErrorText << ' ' << FailedToCreateFile << " \"" << (dopatch ? outTAR : outCIA) << '\"' << std::endl;
					return 22;
				}
				if (parameters.mode) {
					std::cout << CopyingMoflex << ' ' << std::to_string(i + 1) << '/' << std::to_string(parameters.rows) << std::endl;
					//std::error_code error;
					error = copyfile(parameters.MoflexVec.at(i).c_str(), std::string(romfsPath + "/movie/movie_" + std::to_string(i) + ".moflex").c_str());
					if (error) {
						std::cout << ErrorText << ' ' << FailedToCopyFile << "\n\"" << parameters.MoflexVec.at(i) << "\" -> \"" << romfsPath << "/movie/movie_" << std::to_string(i) << ".moflex\"\n" << error.message() << std::endl;
						std::cout << ErrorText << ' ' << FailedToCreateFile << ' ' << romfsPath << "/movie/movie_" << std::to_string(i) << ".moflex" << std::endl;
						return 23;
					}
					if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(romfsPath + "/movie/movie_" + std::to_string(i) + ".moflex").c_str()), error)) {//this probably only happens if there's no disk space
						std::cout << ErrorText << ' ' << FailedToFindPath << '\n' << FailedToCreateFile << ' ' << romfsPath << "/movie/movie_" << std::to_string(i) << ".moflex" << std::endl;
						return 24;
					}
					if(error)
						return 25;
				}
				else {
					std::cout << CopyingMoflex << " 1/1" << std::endl;
					//std::error_code error;
					error = copyfile(parameters.MoflexVec.at(i).c_str(), std::string(romfsPath + "/movie/movie.moflex").c_str());
					if (error) {
						std::cout << ErrorText << ' ' << FailedToCopyFile << "\n\"" << parameters.MoflexVec.at(i) << "\" -> \"" << romfsPath << "/movie/movie.moflex\"\n" << error.message() << std::endl;
						std::cout << ErrorText << ' ' << FailedToCreateFile << ' ' << romfsPath << "/movie/movie.moflex" << std::endl;
						return 26;
					}
					if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(romfsPath + "/movie/movie.moflex").c_str()), error)) {//this probably only happens if there's no disk space
						std::cout << ErrorText << ' ' << FailedToFindPath << '\n' << FailedToCreateFile << ' ' << romfsPath << "/movie/movie.moflex" << std::endl;
						return 27;
					}
					if(error)
						return 28;
				}
			}
		}
		//convert to bimg (multi vid only)
		if (parameters.mode) {
			for (int i = dopatch ? parameters.splitPos : 0; i < ((parameters.splitPos && !dopatch) ? parameters.splitPos : parameters.rows); i++) {
				std::vector<uint8_t> bimg = std::vector<uint8_t>(256 * 128 * sizeof(nnc_u16) + 0x20);
				
				std::cout << CreatingFile << " romfs/movie/movie_" << std::to_string(i) << ".bimg" << std::endl;
				
				uint8_t ret = convertToBimg(parameters.MBannerVec.at(i), bimg.data(), true);
				if (ret > 0) {
					std::cout << ErrorText << ' ' << FailedToFindPath << '\n' << FailedToCreateFile << ' ' << romfsPath << "/movie/movie_" << std::to_string(i) << ".bimg\n(" << std::to_string(ret) << ')' << std::endl;
					return 29;
				}
				std::filesystem::create_directories(std::filesystem::path((const char8_t*)&*std::string(romfsPath + "/movie").c_str()), error);
				if (error) {
					std::cout << ErrorText << ' ' << romfsPath + "/movie" << '\n' << error.message() << std::endl;
					std::cout << ErrorText << ' ' << FailedToCreateFile << " \"" << (dopatch ? outTAR : outCIA) << '\"' << std::endl;
					return 30;
				}
				std::ofstream bimgfile(std::string(romfsPath + "/movie/movie_" + std::to_string(i) + ".bimg").c_str(), std::ios_base::out | std::ios_base::binary);
				bimgfile.write(reinterpret_cast<const char*>(bimg.data()), bimg.size());
				bimgfile.close();
			}
			//make movie_bnrname.csv
			std::filesystem::create_directories(std::filesystem::path((const char8_t*)&*std::string(romfsPath + "/settings").c_str()), error);
			if (error) {
				std::cout << ErrorText << ' ' << romfsPath + "/settings" << '\n' << error.message() << std::endl;
				std::cout << ErrorText << ' ' << FailedToCreateFile << " \"" << (dopatch ? outTAR : outCIA) << '\"' << std::endl;
				return 31;
			}
			std::ofstream movie_bnrname(std::string(romfsPath + "/settings/movie_bnrname.csv").c_str(), std::ios_base::out | std::ios_base::binary);
			movie_bnrname << "\xFF\xFE" + UTF8toUTF16(std::to_string(parameters.rows) + "\x0D\x0A");
			for (int i = 0; i < parameters.rows; i++) {
				movie_bnrname << UTF8toUTF16("movie_" + std::to_string(i) + ".bimg\x0D\x0A");
			}
			movie_bnrname.close();
			if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(romfsPath + "/settings/movie_bnrname.csv").c_str()), error)) {
				std::cout << ErrorText << ' ' << FailedToFindPath << '\n' << FailedToCreateFile << ' ' << romfsPath << "/settings/movie_bnrname.csv" << std::endl;
				return 32;
			}
			if(error)
				return 33;
		}
		//do exefs (icon and banner)
		if (!dopatch) {//dont need exefs for luma patch
			std::cout << CreatingFile << " exefs/icon" << std::endl;
			uint8_t ret = 0;
			ret = convertToIcon(parameters.icon, std::string(tempPath + "/exefs/icon"), UTF8toUTF16(parameters.Sname), UTF8toUTF16(parameters.Lname), UTF8toUTF16(parameters.publisher), parameters.iconBorder);
			if (ret > 0) {
				std::cout << ErrorText << ' ' << FailedToFindPath << '\n' << FailedToCreateFile << ' ' << tempPath << "/exefs/icon\n(" << std::to_string(ret) << ')' << std::endl;
				return 34;
			}
			if (parameters.mode) {//multi vid needs an icon here so that it can make ext data or something (the game crashes if it isnt here)
				std::cout << CreatingFile << " romfs/icon.icn" << std::endl;
				//std::error_code error;
				error = copyfile(std::string(tempPath + "/exefs/icon").c_str(), std::string(romfsPath + "/icon.icn").c_str());
				if (error) {
					std::cout << ErrorText << ' ' << FailedToCopyFile << "\n\"" << tempPath << "/exefs/icon\" -> \"" << romfsPath << "/icon.icn\"\n" << error.message() << std::endl;
					std::cout << ErrorText << ' ' << FailedToCreateFile << ' ' << romfsPath << "/icon.icn" << std::endl;
					return 35;
				}
				if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(romfsPath + "/icon.icn").c_str()), error)) {
					std::cout << ErrorText << ' ' << FailedToFindPath << '\n' << FailedToCreateFile << ' ' << romfsPath << "/icon.icn" << std::endl;
					return 36;
				}
				if(error)
					return 37;
			}
			//make banner
			std::cout << CreatingFile << " exefs/banner" << std::endl;
			uint8_t Checker[4];
			bool bannerbool = false;
			std::ifstream inbanner(std::filesystem::path((const char8_t*)&*parameters.banner.c_str()), std::ios::binary);
			if (std::filesystem::exists(std::filesystem::path((const char8_t*)&*parameters.banner.c_str()), error)) {
				for (int i = 0; i < 4; i++) {
					inbanner >> Checker[i];//https://stackoverflow.com/a/2974735
					if (Checker[i] == bannerMagic[i]) {
						bannerbool = true;
					}
					else {
						bannerbool = false;
						break;
					}
				}
			}
			if(error)
				return 38;
			if (bannerbool) {
				//std::error_code error;
				error = copyfile(parameters.banner, std::string(tempPath + "/exefs/banner").c_str());
				if (error) {
					std::cout << ErrorText << ' ' << FailedToCopyFile << "\n\"" << parameters.banner << "\" -> \"" << tempPath << "/exefs/banner\"\n" << error.message() << std::endl;
					std::cout << ErrorText << ' ' << FailedToCreateFile << ' ' << tempPath << "/exefs/banner" << std::endl;
					return 39;
				}
			}
			else if (!bannerbool) {
				uint8_t buffer[256 * 128 * sizeof(nnc_u16)];
				ret = convertToBimg(parameters.banner, buffer, false);
				if (ret > 0) {
					std::cout << ErrorText << ' ' << BadValue << '\n' << FailedToConvertImage << " \"" << parameters.banner << "\"\n(" << std::to_string(ret) << ')' << std::endl;
					return 40;
				}

				//create bcmdl
				std::vector<uint8_t> bcmdl;
				bcmdl = std::vector<uint8_t>(sizeof(bannerheader) + sizeof(buffer) + sizeof(bannerfooter));
				memcpy(bcmdl.data(), bannerheader, sizeof(bannerheader));
				memcpy(bcmdl.data() + sizeof(bannerheader), buffer, sizeof(buffer));
				memcpy(bcmdl.data() + sizeof(bannerheader) + sizeof(buffer), bannerfooter, sizeof(bannerfooter));

				//build banner (stolen from bannertool)
				CBMD cbmd;
				memset(&cbmd, 0, sizeof(cbmd));

				cbmd.cgfxSizes[0] = sizeof(bannerheader) + sizeof(buffer) + sizeof(bannerfooter);
				cbmd.cgfxs[0] = bcmdl.data();

				cbmd.cwavSize = sizeof(BCWAV_array);
				cbmd.cwav = (void*)BCWAV_array;

				uint32_t bnrSize = 0;

				void* bnr = cbmd_build_data(&bnrSize, cbmd);

				std::ofstream bnrfile(std::string(tempPath + "/exefs/banner").c_str(), std::ios_base::out | std::ios_base::binary);
				bnrfile.write(reinterpret_cast<const char*>(bnr), bnrSize);
				bnrfile.close();
			}
			if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(tempPath + "/exefs/banner").c_str()), error)) {
				std::cout << ErrorText << ' ' << FailedToFindPath << '\n' << FailedToCreateFile << ' ' << tempPath << "/exefs/banner" << std::endl;
				return 41;
			}
			if(error)
				return 42;
		}
		//modify exheader
		if (!dopatch) {//dont need this in a patch either
			std::fstream exheader(std::string(tempPath + "/exheader.bin").c_str(), std::ios::in | std::ios::out | std::ios::binary);
			for (int i = 0; i < 8; i++) {//write application name only 8 bytes because that's the limit. i had to do this loop because it was being weird with .write ???
				exheader.seekp(i);
				exheader << char(ApplicationName.c_str()[i]);
			}
			exheader.seekp(0x1C9);
			exheader.write(reinterpret_cast<const char*>(&uniqueID), sizeof(uint32_t));
			exheader.seekp(0x201);
			exheader.write(reinterpret_cast<const char*>(&uniqueID), sizeof(uint32_t));
			exheader.seekp(0x601);
			exheader.write(reinterpret_cast<const char*>(&uniqueID), sizeof(uint32_t));
			exheader.close();
		}
		//CIA creation
		if (!dopatch) {
			std::cout << CreatingFile << ' ' << outCIA.substr(outCIA.find_last_of("/\\") + 1) << std::endl;

			std::ofstream baseCIA(std::filesystem::path((const char8_t*)&*std::string(tempPath + "/base.cia").c_str()), std::ios_base::out | std::ios_base::binary);
			baseCIA.write(reinterpret_cast<const char*>(base_cia), sizeof(base_cia));
			baseCIA.close();

			#define TRYB(expr, lbl) if((res = ( expr )) != NNC_R_OK) goto lbl

			nnc_subview certchain, ticket, tmd_strm, logo;
			nnc_buildable_ncch ncch0b;
			nnc_tmd_header tmd;
			nnc_cia_writable_ncch ncch0;
			nnc_ncch_header ncch_hdr;
			nnc_cia_content_reader reader;
			nnc_cia_content_stream ncch0stream;
			nnc_file exheader;
			nnc_cia_header cia_hdr;
			nnc_result res;
			nnc_wfile wf;
			nnc_file f;
			nnc_vfs romfs;
			nnc_vfs exefs;
			nnc_keypair kp;
			nnc_seeddb sdb = {};
			
			std::cout << CIAopenInput << std::endl;
			TRYB(nnc_file_open(&f, std::string(tempPath + "/base.cia").c_str()), out1); /* open the input file */
			std::cout << CIAopenOutput << std::endl;
			TRYB(nnc_wfile_open(&wf, outCIA.c_str()), out2); /* open the output file */
			std::cout << CIAreadHeader << std::endl;
			TRYB(nnc_read_cia_header(NNC_RSP(&f), &cia_hdr), out3); /* read the cia header */
			std::cout << CIAopenCertificate << std::endl;
			nnc_cia_open_certchain(&cia_hdr, NNC_RSP(&f), &certchain); /* open the certificate chain for later copying it into the new cia */
			std::cout << CIAopenTicket << std::endl;
			nnc_cia_open_ticket(&cia_hdr, NNC_RSP(&f), &ticket); /* open the ticket for later copying it into the new cia */
			std::cout << CIAopenTMD << std::endl;
			nnc_cia_open_tmd(&cia_hdr, NNC_RSP(&f), &tmd_strm); /* open the tmd which we will modify some things of and then write to the new cia */
			std::cout << CIAparseTicket << std::endl;
			TRYB(nnc_read_tmd_header(NNC_RSP(&tmd_strm), &tmd), out3); /* parse the ticket */
			std::cout << CIAcreateReader << std::endl;
			TRYB(nnc_cia_make_reader(&cia_hdr, NNC_RSP(&f), nnc_get_default_keyset(), &reader), out3); /* create a content (= NCCH) reader */
			std::cout << CIAopenFirstContent << std::endl;
			TRYB(nnc_cia_open_content(&reader, 0, &ncch0stream, NULL), out4); /* open the first content (NCCH0) */
			std::cout << CIAparseNCCH << std::endl;
			TRYB(nnc_read_ncch_header(NNC_RSP(&ncch0stream), &ncch_hdr), out5); /* parse the NCCH header */

			std::cout << CIAinitializeVFS << std::endl;
			TRYB(nnc_vfs_init(&romfs), out5); /* initialize a VFS */
			std::cout << CIApopulateVFS << " (RomFS)" << std::endl;
			TRYB(nnc_vfs_link_directory(&romfs.root_directory, romfsPath.c_str(), nnc_vfs_identity_transform, NULL), out6); /* populate the VFS, another source of files could be a RomFS, see #nnc_romfs_to_vfs */
			std::cout << CIAinitializeVFS << std::endl;
			TRYB(nnc_vfs_init(&exefs), out5); /* initialize a VFS */
			std::cout << CIApopulateVFS << " (ExeFS)" << std::endl;
			TRYB(nnc_vfs_link_directory(&exefs.root_directory, std::string(tempPath + "/exefs").c_str(), nnc_vfs_identity_transform, NULL), out10);

			std::cout << CIAscanSeeddb << std::endl;
			if ((res = nnc_scan_seeddb(&sdb)) != NNC_R_OK) /* scan for a seeddb for use with "new crypto" and set it as the default */
				nnc_set_default_seeddb(&sdb);
			std::cout << CIAgenerateKeys << std::endl;
			TRYB(nnc_fill_keypair(&kp, nnc_get_default_keyset(), nnc_get_default_seeddb(), &ncch_hdr), out7); /* generate the cryptographic keys for if the NCCH is encrypted */
			std::cout << CIAopenExheader << std::endl;
			if (nnc_file_open(&exheader, std::string(tempPath + "/exheader.bin").c_str()) != NNC_R_OK) {/* open exheader file */
				std::cout << ErrorText << ' ' << FailedToReadFile << " \"" << tempPath << "/exheader.bin\"" << std::endl;
				goto out10;
			}
			nnc_exheader exhdr;
			std::cout << CIAreadExheader << std::endl;
			if (nnc_read_exheader(NNC_RSP(&exheader), &exhdr) != NNC_R_OK) {
				std::cout << ErrorText << ' ' << FailedToReadExHeader << " \"" << tempPath << "/exheader.bin\"" << std::endl;
				goto out10;
			}
			std::cout << CIAopenLogo << std::endl;
			TRYB(nnc_ncch_section_logo(&ncch_hdr, NNC_RSP(&ncch0stream), &logo), out9); /* logo stream */

			/* setup the parameters for building, for more options see the documentation. */
			std::cout << CIAsetParameters << std::endl;
			ncch0.type = NNC_CIA_NCCHBUILD_BUILD;
			ncch0.ncch = &ncch0b;
			nnc_condense_ncch(&ncch0b.chdr, &ncch_hdr);
			ncch0b.wflags = NNC_NCCH_WF_ROMFS_VFS | NNC_NCCH_WF_EXEFS_VFS | NNC_NCCH_WF_EXHEADER_STREAM;
			ncch0b.romfs = &romfs;
			ncch0b.exefs = &exefs;
			ncch0b.exheader = &exheader;
			ncch0b.logo = NNC_RSP(&logo);
			ncch0b.plain = NULL;
			ncch0b.chdr.partition_id = exhdr.title_id;
			ncch0b.chdr.title_id = exhdr.title_id;

			if (chrcount(ProductCode) != 4) {
				std::cout << ErrorText << ' ' << ProductCodetext << ' ' << BadValue << std::endl;
				goto out10;
			}
			{
				std::string productCodeFull = "CTR-H-" + ProductCode;
				strcpy(ncch0b.chdr.product_code, productCodeFull.c_str());//modify product code
			}

			tmd.content_count = 1;
			tmd.title_id = exhdr.title_id;
			{
				std::cout << CIAsetTitleID << std::endl;
				//change the title ID of the ticket
				std::vector<char> ticket_contents = std::vector<char>(NNC_RS_CALL0(ticket, size));
				nnc_u32 out_size = 0;
				if (NNC_RS_CALL(ticket, read, (nnc_u8*)ticket_contents.data(), NNC_RS_CALL0(ticket, size), &out_size) != NNC_R_OK)
					goto out10;
				if (out_size != NNC_RS_CALL0(ticket, size))
					goto out10;
				nnc_memory modified_ticket;
				{
					uint64_t TIDbigend = 0;
					encode_bigend_u64(exhdr.title_id, &TIDbigend);
					*(nnc_u64*)&ticket_contents[nnc_sig_dsize((nnc_sigtype)ticket_contents[3]) + 0xDC] = TIDbigend;
				}
				nnc_mem_open(&modified_ticket, ticket_contents.data(), NNC_RS_CALL0(ticket, size));

				std::cout << CIAwriteCIA << std::endl;
				/* and finally write the cia */
				res = nnc_write_cia(
					NNC_CIA_WF_CERTCHAIN_STREAM | NNC_CIA_WF_TICKET_STREAM | NNC_CIA_WF_TMD_BUILD,
					&certchain, &modified_ticket, &tmd, 1, &ncch0, NNC_WSP(&wf)
				);
			}
		std::cout << CIAcleanUp << std::endl;
		/* cleanup code, with lots of labels to jump to in case of failure depending on where it failed */
		out10: nnc_vfs_free(&exefs);
		out9: NNC_RS_CALL0(exheader, close);
			//out8: NNC_RS_CALL0(exefs, close);
		out7: nnc_free_seeddb(&sdb);
		out6: nnc_vfs_free(&romfs);
		out5: NNC_RS_CALL0(ncch0stream, close);
		out4: nnc_cia_free_reader(&reader);
		out3: NNC_WS_CALL0(wf, close);
		out2: NNC_RS_CALL0(f, close);
		out1:
			if (res != NNC_R_OK)
			{
				std::cout << "failed: " << nnc_strerror(res) << std::endl;
				return res;
			}
		}
		else {//TAR creation
			std::cout << CreatingFile << ' ' << outTAR.substr(outTAR.find_last_of("/\\") + 1) << std::endl;
			
			std::cout << TARremoveExisting << std::endl;
			if (std::filesystem::exists(std::filesystem::path((const char8_t*)&*outTAR.c_str()), error)) {//write wont work properly if it already exists
				std::filesystem::remove_all(std::filesystem::path((const char8_t*)&*outTAR.c_str()), error);
				if (error) {
					std::cout << ErrorText << ' ' << outTAR << '\n' << error.message() << std::endl;
				}
			}
			if(error)
				std::cout << ErrorText << ' ' << outTAR << '\n' << error.message() << std::endl;
			
			mtar_t tar;

			std::cout << TARopenArchive << std::endl;
			/* Open archive for writing */
			mtar_open(&tar, outTAR.c_str(), "wb");

			std::filesystem::path tarpath = tempPath + "/luma";
			if (tarpath.is_relative())
				tarpath = std::filesystem::absolute(tarpath);
			std::string tarpathstr = tarpath.string();
			tarpathstr = fixSlashes(tarpathstr);
			error = add_directory(&tar, tarpathstr, 30000000);//CRASH
			if(error) {
				std::cout << ErrorText << ' ' << tarpathstr << '\n' << error.message() << std::endl;
				std::cout << ErrorText << ' ' << FailedToCreateFile << " \"" << outTAR << '\"' << std::endl;
			}

			std::cout << TARfinalize << std::endl;
			/* Finalize -- this needs to be the last thing done before closing */
			mtar_finalize(&tar);

			std::cout << TARclose << std::endl;
			/* Close archive */
			mtar_close(&tar);
		}
		if (parameters.mode && !outTAR.empty() && parameters.splitPos && !dopatch) {
			dopatch = true;
		}
		else if (dopatch) {
			dopatch = false;
		}
	} while (dopatch);
	std::filesystem::remove_all(std::filesystem::path((const char8_t*)&*tempPath.c_str()), error);
	if (error) {
		std::cout << ErrorText << ' ' << tempPath << '\n' << error.message() << std::endl;
		//no return becuase nobody cares
	}
	return res;
}

int extract_archive(std::string inArc, std::string outDir, bool dopatch, std::string seedpath) {
	VI9Pparameters parameters;
	parameters.PTitleVec.clear();
	parameters.MoflexVec.clear();
	parameters.MBannerVec.clear();
	
	if (std::filesystem::path((const char8_t*)&*outDir.c_str()).is_relative())
		outDir = std::filesystem::absolute(std::filesystem::path((const char8_t*)&*outDir.c_str())).string();
	
	std::string tempPath = resourcesPath + "/CLItemp";
	std::string exportsPath = tempPath + "/exports";
	std::string romfspath = outDir + "/romfs";
	std::string exefspath = outDir + "/exefs";
	std::vector<std::string> trimmed;
	
	static nnc_result res;
	std::error_code error;
	/*std::filesystem::remove_all(std::filesystem::path((const char8_t*)&*outDir.c_str()), error);//probably dont delete outDir because people might have other stuff in here for some reason
	if (error) {
		std::cout << ErrorText << ' ' << outDir << '\n' << error.message() << std::endl;
		return 1;
	}*/
	std::filesystem::create_directory(std::filesystem::path((const char8_t*)&*outDir.c_str()), error);
	if (error) {
		std::cout << ErrorText << ' ' << outDir << '\n' << error.message() << std::endl;
		return 1;
	}
	//extract important files from the romfs and exefs of cia
	if(!dopatch) {
		#define CUREC_SIZE 1024

		std::string extractErr = "";

		//stolen from the NNC cia test code and other places
		nnc_exefs_file_header headers[NNC_EXEFS_MAX_FILES];
		nnc_keyset kset = NNC_KEYSET_INIT;
		nnc_seeddb::nnc_seeddb_entry ent;
		nnc_cia_content_reader reader;
		nnc_cia_content_stream ncch;
		nnc_ncch_section_stream rrs;
		nnc_ncch_exefs_stream ers;
		nnc_ncch_header ncch_hdr;
		nnc_chunk_record* chunk;
		nnc_cia_header header;
		nnc_romfs_info info;
		nnc_romfs_ctx ctx;
		nnc_keypair kp;
		nnc_subview sv;
		nnc_seeddb sdb = {};
		sdb.size = 1;
		nnc_file f;

		res = nnc_file_open(&f, inArc.c_str());
		if (res != NNC_R_OK)
			goto end;

		res = nnc_read_cia_header(NNC_RSP(&f), &header);
		if (res != NNC_R_OK)
			goto end0;

		nnc_keyset_default(&kset, NNC_KEYSET_RETAIL);
		res = nnc_cia_make_reader(&header, NNC_RSP(&f), &kset, &reader);
		if (res != NNC_R_OK)
			goto end1;

		res = nnc_cia_open_content(&reader, 0, &ncch, &chunk);//read main content (index 0)
		if (res != NNC_R_OK)
			goto end1;

		//this section is here to test CBC seeking
		NNC_RS_CALL(ncch, seek_abs, 0);
		//DSiWare doesn't have an NCCH, lets not make this a fatal error...
		res = nnc_read_ncch_header(NNC_RSP(&ncch), &ncch_hdr);
		if (res != NNC_R_OK)
			goto end2;

		res = nnc_fill_keypair(&kp, &kset, NULL, &ncch_hdr);
		if (res == NNC_R_SEED_NOT_FOUND) {
			uint8_t seedData[NNC_SEED_SIZE];
			std::ifstream seedfile;
			if (seedpath.substr(seedpath.find_last_of("/\\") + 1) == "seeddb.bin") {
				if(getSeedFromTID(seedpath, ncch_hdr.title_id, seedData) != 0)
					std::cout << ErrorText << ' ' << SeedNotFound << std::endl;
			}
			else {
				if (std::filesystem::file_size(std::filesystem::path((const char8_t*)&*seedpath.c_str()), error) != NNC_SEED_SIZE) {
					if (error) {
						std::cout << ErrorText << ' ' << seedpath << '\n' << error.message() << std::endl;
					}
					goto end2;
				}
				if (error) {
					std::cout << ErrorText << ' ' << seedpath << '\n' << error.message() << std::endl;
					goto end2;
				}
				seedfile.open(std::filesystem::path((const char8_t*)&*seedpath.c_str()), std::ios_base::in | std::ios_base::binary);
				seedfile.read(reinterpret_cast<char*>(seedData), NNC_SEED_SIZE);
			}
			memcpy(ent.seed, seedData, NNC_SEED_SIZE);
			ent.title_id = ncch_hdr.title_id;
			sdb.entries = &ent;
			res = nnc_fill_keypair(&kp, &kset, &sdb, &ncch_hdr);
			if (error) {
				std::cout << ErrorText << ' ' << seedpath << '\n' << error.message() << std::endl;
				goto end2;
			}
		}
		if (res != NNC_R_OK) {
			goto end2;
		}

		res = nnc_ncch_exefs_full_stream(&ers, &ncch_hdr, NNC_RSP(&ncch), &kp);
		if (res != NNC_R_OK)
			goto end2;

		res = nnc_read_exefs_header(NNC_RSP(&ers), headers, NULL);//read exefs
		if (res != NNC_R_OK)
			goto end2;

		char pathbuf[2048];
		for (nnc_u8 i = 0; i < NNC_EXEFS_MAX_FILES && nnc_exefs_file_in_use(&headers[i]); ++i)
		{
			nnc_exefs_subview(NNC_RSP(&ers), &sv, &headers[i]);

			const char* fname = NULL;
			if (strcmp(headers[i].name, "banner") == 0)
				fname = "banner.bin";
			else if (strcmp(headers[i].name, "icon") == 0)
				fname = "icon.bin";
			else
			{
				continue;
			}

			nnc_u32 read_size;
			std::vector<nnc_u8> buf = std::vector<nnc_u8>(headers[i].size);
			NNC_RS_CALL(sv, seek_abs, 0);
			if (NNC_RS_CALL(sv, read, buf.data(), headers[i].size, &read_size) != NNC_R_OK || read_size != headers[i].size) {
				std::cout << ErrorText << ' ' << FailedToReadFile << " \"" << headers[i].name << '\"' << std::endl;
				continue;
			}
			std::filesystem::create_directories(std::filesystem::path((const char8_t*)&*exefspath.c_str()), error);
			if (error) {
				std::cout << ErrorText << ' ' << exefspath << '\n' << error.message() << std::endl;
				return 2;
			}
			sprintf(pathbuf, "%s/%s", exefspath.c_str(), fname);
			FILE* ef = fopen(pathbuf, "wb");
			if (fwrite(buf.data(), headers[i].size, 1, ef) != 1)
				std::cout << ErrorText << ' ' << FailedToCreateFile << " \"" << pathbuf << '\"' << std::endl;
			fclose(ef);
		}

		res = nnc_ncch_section_romfs(&ncch_hdr, NNC_RSP(&ncch), &kp, &rrs);//read romfs
		if (res != NNC_R_OK)
			goto end2;

		res = nnc_init_romfs(NNC_RSP(&rrs), &ctx);
		if (res != NNC_R_OK)
			goto end2;

		res = nnc_get_info(&ctx, &info, "/");
		if (res != NNC_R_OK)
			goto end2;

		extractErr = extract_dir(&ctx, &info, romfspath.c_str(), romfspath.size());

		end2:
			NNC_RS_CALL0(ncch, close);
		end1:
			nnc_cia_free_reader(&reader);
		end0:
			NNC_RS_CALL0(f, close);
		end:
			if (!extractErr.empty()) {
				std::cout << ErrorText << ' ' << extractErr << std::endl;
			}
			if (res != NNC_R_OK)
			{
				std::cout << ErrorText << ' ' << nnc_strerror(res) << std::endl;
				return 3;
			}
			//nnc_free_seeddb(&sdb);
	}
	else {
		int ret = 0;
		mtar_t tar;

		/* Open archive for reading */
		ret = mtar_open(&tar, inArc.c_str(), "rb");//doesnt work with unicode
		if (ret) {
			std::cout << ErrorText << ' ' << inArc << '\n' << mtar_strerror(ret) << std::endl;
			return 4;
		}
		ret = extract_content(&tar, inArc, exportsPath, 30000000);
		if (ret) {
			std::cout << ErrorText << ' ' << inArc << '\n' << mtar_strerror(ret) << std::endl;
			mtar_close(&tar);
			return 5;
		}
		mtar_close(&tar);
		std::filesystem::create_directory(std::filesystem::path((const char8_t*)&*romfspath.c_str()), error);
		std::string lumaromfs = "";
		for (const auto& entry : std::filesystem::recursive_directory_iterator(std::filesystem::path((const char8_t*)&*std::string(exportsPath + "/luma").c_str()), error)) {//do this because we dont know the title ID
			auto filename = std::filesystem::canonical(entry);
			if (std::filesystem::is_directory(entry, error) && filename.string().rfind("romfs") != std::string::npos) {//this only works because we know there isnt gonna be a "romfs" subdir
				lumaromfs = filename.string();
				break;
			}
		}
		for (int i = 0; i < MAX_ROWS; i++)//uh probably better than checking the string
			if (std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(lumaromfs + "/movie/movie_" + std::to_string(i) + ".moflex").c_str()))) {
				parameters.splitPos = i & 0xFF;
				break;
			}
		for (const auto& entry : std::filesystem::recursive_directory_iterator(std::filesystem::path((const char8_t*)&*lumaromfs.c_str()), error)) {
			auto filename = std::filesystem::canonical(entry);
			std::string outdir = std::string(outDir + "/romfs/");
			outdir += std::filesystem::relative(filename, std::filesystem::path((const char8_t*)&*lumaromfs.c_str())).string();
			if (std::filesystem::is_directory(filename, error)) {
				std::filesystem::create_directory(std::filesystem::path((const char8_t*)&*outdir.c_str()), error);
				if (error) {
					std::cout << ErrorText << ' ' << filename << '\n' << error.message() << std::endl;
					return 6;
				}
			}
			else if (std::filesystem::is_regular_file(filename, error)) {
				std::filesystem::rename(filename, std::filesystem::path((const char8_t*)&*outdir.c_str()), error);
			}

			if (std::filesystem::is_regular_file(entry, error)) {
				if (error) {
					std::cout << ErrorText << ' ' << filename << '\n' << error.message() << std::endl;
				}
			}
		}
		std::filesystem::remove_all(exportsPath.c_str(), error);//lol
		if (error) {
			std::cout << ErrorText << ' ' << exportsPath << '\n' << error.message() << std::endl;
		}
	}
	//information_buttons.csv
	{
		uint8_t ret;
		std::vector<std::string> trimmed;
		if (std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(romfspath + "/settings/information_buttons.csv").c_str()))) {
			ret = UTF16fileToUTF8str(std::string(romfspath + "/settings/information_buttons.csv"), &trimmed);
			if (ret > 0) {
				std::cout << ErrorText << " (" << std::to_string(ret) << ")\n" << FailedToReadFile << ": \"information_buttons.csv\"" << std::endl;
			}
			else {
				parameters.mode = 1;
				parameters.copycheck = (trimmed.size() > 0 && strcmp(trimmed.at(0).c_str(), "Copyright") == 0) ? 1 : 0;
			}
		}
		trimmed.clear();
	}
	//copyright.txt
	{
		if (std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(romfspath + "/settings/copyright.txt").c_str()))) {
			std::string output = "";
			std::string outputUTF8 = "";
			std::string Line;
			std::ifstream input;
			input.open(std::filesystem::path((const char8_t*)&*std::string(romfspath + "/settings/copyright.txt").c_str()), std::ios_base::in | std::ios_base::binary);
			if (!input) {
				std::cout << ErrorText << ' ' << FailedToReadFile << '\n' << romfspath + "/settings/copyright.txt" << std::endl;
				return 7;
			}

			char Byte;
			//size_t it = 0;
			input.read(&Byte, 1);//grab first byte of file
			while (input) {//continue until input stream fails
				output += Byte;//append byte to string
				input.read(&Byte, 1);//grab next byte of file
			}
			input.close();
			if (output[0] == 0xFE && output[1] == 0xFF) {//if little endian (they should be in big endian anyway and i dont want to convert it)
				std::cout << ErrorText << ' ' << FailedToReadFile << '\n' << romfspath + "/settings/copyright.txt" << std::endl;
				return 8;
			}
			output.erase(output.begin(), output.begin() + 2);//delete byte order mask
			outputUTF8 = UTF16toUTF8(output);
			parameters.copyrightInfo = outputUTF8;//why is it like this? im too scared to change it
			input.close();
		}
	}
	uint8_t ret;
	bool good = true;
	//set banner and icon
	{
		parameters.banner = std::string(exefspath + "/banner.bin");//do it like this because the vi9p is in outDir
		parameters.icon = std::string(exefspath + "/icon.bin");
		parameters.iconBorder = 0;
		
		(void)setIcon(std::string(exefspath + "/icon.bin").c_str(), parameters.Sname, parameters.Lname, parameters.publisher);
	}
	//settingTL.csv
	{
		ret = UTF16fileToUTF8str(std::string(romfspath + "/settings/settingsTL.csv"), &trimmed);
		if (ret > 0) {
			std::cout << ErrorText << " (" << std::to_string(ret) << ")\n" << FailedToReadFile << ": \"settingsTL.csv\"" << std::endl;
			good = false;
		}
		else {
			if (trimmed.size() > 29) {
				parameters.rows = 1;
				parameters.FFrewind = (strstr(trimmed.at(29).c_str(), "true") != NULL) ? 1 : 0;
				parameters.FadeOpt = (strstr(trimmed.at(30).c_str(), "true") != NULL) ? 1 : 0;
			}
		}
		trimmed.clear();
	}
	//movie_bnrname.csv
	{
		if (std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(romfspath + "/settings/movie_bnrname.csv").c_str()))) {
			ret = UTF16fileToUTF8str(std::string(romfspath + "/settings/movie_bnrname.csv"), &trimmed);
			if (ret > 0) {
				std::cout << ErrorText << " (" << std::to_string(ret) << ")\n" << FailedToReadFile << ": \"movie_bnrname.csv\"" << std::endl;
				good = false;
			}
			else {
				parameters.mode = 1;
				parameters.rows = 0;
				std::vector<std::string> output;
				for (auto& LN : trimmed) {
					std::string extension = LN;
					if (extension.find_last_of(".") != std::string::npos)
						extension.erase(extension.begin(), extension.begin() + extension.find_last_of("."));
					while (!extension.empty() && extension.back() != 'g')//remove any extra stuff like a line break (make sure last char is 'g')
						extension.pop_back();
					if (strcmp(extension.c_str(), ".bimg") == 0) {
						while (!LN.empty() && LN.back() != 'g')
							LN.pop_back();
						output.push_back(std::string(romfspath + "/movie/" + LN));
						++parameters.rows;
					}
				}
				if (output.size() > MAX_ROWS) {
					std::cout << ErrorText << ' ' << BadValue << '\n' << BadValue << ": (" << std::to_string(output.size()) << ")\n" << noMoreThan27 << '.' << std::endl;
					while (output.size() > MAX_ROWS) {
						output.pop_back();
						--parameters.rows;
					}
				}
				for (size_t i = 0; i < output.size(); i++) {
					parameters.MBannerVec.push_back(output.at(i));
					//text_box_array.at(i * columns + 2)->text(output.at(i));
				}
			}
			//setMultiBannerPreview(bannerpreviewindex);
		}
		else {//this pretty much means it's a single video
			parameters.rows = 1;
			parameters.mode = 0;
			parameters.MBannerVec.push_back("");
			//setMultiBannerPreview(0);
		}
		trimmed.clear();
	}
	//movie_title.csv
	{
		ret = UTF16fileToUTF8str(std::string(romfspath + "/movie/movie_title.csv"), &trimmed);
		if (ret > 0) {
			std::cout << ErrorText << " (" << std::to_string(ret) << ")\n" << FailedToReadFile << ": \"movie_title.csv\"" << std::endl;
			parameters.rows = 1;
			for (int i = 0; i < parameters.rows; i++) {
				parameters.PTitleVec.push_back("");
			}
			good = false;
		}
		else {
			parameters.rows = 0;
			std::vector<std::string> output;
			for (auto& LN : trimmed) {
				while (LN[0] == ',') {
					LN.erase(0, 1);
				}
				if (LN.find(',') < LN.size()) {
					LN.erase(LN.find(','), LN.size() - 1);
				}
				output.push_back(LN);
				++parameters.rows;
			}
			if (output.size() > MAX_ROWS) {
				std::cout << ErrorText << ' ' << BadValue << '\n' << BadValue << ": (" << std::to_string(output.size()) << ")\n" << noMoreThan27 << '.' << std::endl;
				while (output.size() > MAX_ROWS) {
					output.pop_back();
					--parameters.rows;
				}
			}
			for (size_t i = 0; i < output.size(); i++) {
				//std::cout << output.at(i) << std::endl;
				parameters.PTitleVec.push_back(output.at(i));
			}
		}
		trimmed.clear();
	}
	//set moflex files
	{
		if (std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(romfspath + "/movie/movie.moflex").c_str()))) {//single video only has this
			parameters.MoflexVec.push_back(std::string(romfspath + "/movie/movie.moflex"));
			if (parameters.rows > 1)
				parameters.rows = 1;
		}
		else {
			for (int i = 0; i < parameters.rows; i++) {
				parameters.MoflexVec.push_back(std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(romfspath + "/movie/movie_" + std::to_string(i) + ".moflex").c_str())) ? std::string(romfspath + "/movie/movie_" + std::to_string(i) + ".moflex") : "");
				std::cout << (std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(romfspath + "/movie/movie_" + std::to_string(i) + ".moflex").c_str())) ? std::string(romfspath + "/movie/movie_" + std::to_string(i) + ".moflex") : "") << std::endl;
			}
		}
	}
	if (res != NNC_R_OK)
		good = false;
	if(good) std::cout << ParametersLoaded << '\n' << SuccessfullyLoaded << ' ' << inArc.substr(inArc.find_last_of("/\\") + 1) << '.' << std::endl;
	else std::cout << ParametersLoaded << '\n' << FailedToLoad << ' ' << inArc.substr(inArc.find_last_of("/\\") + 1) << ".\n" << ValidStillLoaded << std::endl;

	std::cout << CreatingFile << " \"" << outDir + "/parameters.vi9p\"" << std::endl;
	
	saveParameters(std::string(outDir + "/parameters.vi9p"), parameters);
	
	return 0;
}

uint8_t UTF16fileToUTF8str(const std::string path, std::vector<std::string>* outVec) {
	std::string output = "";
	std::string outputUTF8 = "";
	std::string Line;
	std::ifstream input;
	input.open(std::filesystem::path((const char8_t*)&*path.c_str()), std::ios_base::in | std::ios_base::binary);
	if (!input)
		return 1;

	char Byte;
	//size_t it = 0;
	input.read(&Byte, 1);//grab first byte of file
	while (input) {//continue until input stream fails
		output += Byte;//append byte to string
		input.read(&Byte, 1);//grab next byte of file
	}
	input.close();
	if ((output[0] & 0xFF) == 0xFE && (output[1] & 0xFF) == 0xFF)//if little endian (they should be in big endian anyway and i dont want to convert it)
		return 2;
	output.erase(output.begin(), output.begin() + 2);//delete byte order mask
	outputUTF8 = UTF16toUTF8(output);
	std::istringstream Textstream(outputUTF8);

	while (getline(Textstream, Line)) {
		if (Line[0] == '#' || Line[0] == 0xA || Line[0] == 0xD)//do 0xD too because I think windows is being annoying with line breaks as usual
			continue;
		outVec->push_back(Line);
		//printf("%s\n", Line.c_str());
	}
	return 0;
}

//stolen from NNC romfs test
std::string extract_dir(nnc_romfs_ctx* ctx, nnc_romfs_info* info, const char* path, int baselen) {
	std::error_code error;
	std::filesystem::create_directories(std::filesystem::path((const char8_t*)&*path), error);
	if (error) {
		std::cout << ErrorText << ' ' << path << '\n' << error.message() << std::endl;
		return "create_directories failed";
	}

	nnc_result res = NNC_R_OK;
	nnc_romfs_iterator it = nnc_romfs_mkit(ctx, info);
	nnc_romfs_info ent;
	char pathbuf[2048];
	size_t len = strlen(path);
	strcpy(pathbuf, path);
	pathbuf[len++] = '/';
	while (nnc_romfs_next(&it, &ent))
	{
		std::string dir(nnc_romfs_info_filename(ctx, &ent));
		if (ent.type == nnc_romfs_info::NNC_ROMFS_DIR && strcmp(dir.c_str(), "movie") != 0 && strcmp(dir.c_str(), "settings") != 0)//R.I.P. all the files that we do not care about
			continue;
		strcpy(pathbuf + len, dir.c_str());
		if (ent.type == nnc_romfs_info::NNC_ROMFS_DIR)
			extract_dir(ctx, &ent, pathbuf, baselen);
		else
		{
			nnc_wfile outf;
			/* empty files just need to be touched */
			if (ent.u.f.size)
			{
				nnc_u32 r;
				nnc_subview sv;
				res = nnc_romfs_open_subview(ctx, &sv, &ent);
				if (res != NNC_R_OK)
					goto out;
				res = nnc_wfile_open(&outf, pathbuf);
				if (res != NNC_R_OK) {
					return "nnc_wfile_open(&outf, pathbuf);";
				}
				nnc_copy(NNC_RSP(&sv), NNC_WSP(&outf), &r);
				if (r != ent.u.f.size) goto out;
			out:
				NNC_WS_CALL0(outf, close);
				continue;
			}
			NNC_WS_CALL0(outf, close);
		}
	}
	if (res != NNC_R_OK)
		return nnc_strerror(res);
	return "";
}

//based off of NNC because NNC wasnt working
uint8_t getSeedFromTID(std::string infile, uint64_t TID, uint8_t* outseed) {
	std::ifstream seeddb;
	seeddb.open(std::filesystem::path((const char8_t*)&*infile.c_str()), std::ios_base::in | std::ios_base::binary);
	uint32_t expected_size = 0;
	seeddb.seekg(0);
	seeddb.read(reinterpret_cast<char*>(&expected_size), 0x4);
	for (uint32_t i = 0x10; i < (expected_size * 0x20) + 0x10; i += 0x20) {
		uint64_t inTID = 0;
		seeddb.seekg(i);
		seeddb.read(reinterpret_cast<char*>(&inTID), 0x8);
		if (inTID == TID) {
			seeddb.seekg(i + 0x8);
			seeddb.read(reinterpret_cast<char*>(outseed), 0x10);//read seed
			return 0;
		}
	}
	return 1;
}
