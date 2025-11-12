#include "stbiStuff.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

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

	std::vector<uint8_t> scaled(width * height * num_channels);
	// scale it down also now we gotta convert the channels to stbir_pixel_layout i guess
	stbir_resize_uint8_linear(input_pixels, input_w, input_h, 0, scaled.data(), width, height, 0, static_cast<stbir_pixel_layout>(num_channels));
	crop_pixels(scaled.data(), width, height, num_channels, output_pixels, (width - output_w) / 2, (height - output_h) / 2, output_w, output_h);
}

int generateBlankBanner(std::string &outfile) {
	int ch = 4;
	int out_w = 200;
	int out_h = 120;
	int film_w = 264;
	int film_h = 154;
	std::vector<uint8_t> output_4c(out_w * out_h * ch);
	memset(output_4c.data(), 0xFF, out_w * out_h * ch);

	std::vector<uint8_t> output_film(film_w * film_h * 4);
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

int generateBannerPreview(std::string infile, std::string outfile, bool multibanner) {
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
		std::vector<uint8_t> output_pixels(w * h * och);
		nnc_unswizzle_zorder_le_rgb565_to_be_rgba8(reinterpret_cast<nnc_u16*>(&CGFXdecomp[dataOffset0]), reinterpret_cast<nnc_u32*>(output_pixels.data()), w & 0xFFFF, h & 0xFFFF);
		std::vector<uint8_t> output_cropped(out_w * out_w * och);
		crop_pixels(output_pixels.data(), w, h, och, output_cropped.data(), 0, 0, out_w, out_h);
		std::vector<uint8_t> output_film(film_w * film_h * 4);
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
			std::vector<uint8_t> input_data((w * h * ich) + 0x20);
			input.read(reinterpret_cast<char*>(input_data.data()), (w * h * ich) + 0x20);
			input.close();
			for (int i = 0; i < 0x1C; i++) {
				if (input_data[i] != bimgheader[i]) {
					//setDefaultBannerPreview(bannerpreview, &bannererror);
					return 20;
				}
			}
			std::vector<uint8_t> output_pixels(w * h * och);
			nnc_unswizzle_zorder_le_rgb565_to_be_rgba8(reinterpret_cast<nnc_u16*>(&input_data[0x20]), reinterpret_cast<nnc_u32*>(output_pixels.data()), w & 0xFFFF, h & 0xFFFF);
			std::vector<uint8_t> output_cropped(out_w * out_w * och);
			crop_pixels(output_pixels.data(), w, h, och, output_cropped.data(), 0, 0, out_w, out_h);
			std::vector<uint8_t> output_film(film_w * film_h * 4);
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
			std::vector<uint8_t> output_pixels(out_w * out_h * ch);

			if (w == out_w && h == out_h) memcpy(output_pixels.data(), input_pixels, w * h * ch);
			else resize_crop(input_pixels, w, h, output_pixels.data(), out_w, out_h, ch);//scale to 200x120 if needed
			free(input_pixels);
			std::vector<uint8_t> output_4c(out_w * out_h * 4);
			std::vector<uint8_t> white(out_w * out_h * 4);
			memset(white.data(), 0xFF, out_w * out_h * 4);
			layer_pixels(output_4c.data(), output_pixels.data(), white.data(), out_w, out_h, ch, out_w, out_h, 4, 0, 0);
			std::vector<uint8_t> output_film(film_w * film_h * 4);
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
		std::vector<uint8_t> output_4c(largeWH * largeWH * 4);
		std::vector<uint8_t> white_background(largeWH * largeWH * 4);//fix the bugs by not fixing the bugs! :D
		memset(white_background.data(), FF, largeWH * largeWH * 4);
		layer_pixels(output_4c.data(), output_pixels.data(), white_background.data(), largeWH, largeWH, ch, largeWH, largeWH, 4, 0, 0);//it warns about output_pixels being potentially uninitialized but that is impossible
		layer_pixels(output_4c.data(), icon_border48, output_4c.data(), largeWH, largeWH, 4, largeWH, largeWH, 4, 0, 0);
		ch = 4;// important later
		output_pixels = std::vector<uint8_t>(largeWH * largeWH * ch);
		memcpy(output_pixels.data(), output_4c.data(), largeWH * largeWH * ch);
	}
	else if (borderMode == 2) {//inside border
		std::vector<uint8_t> output_4c(largeWH * largeWH * 4);
		std::vector<uint8_t> white_background(largeWH * largeWH * 4);//fix the bugs by not fixing the bugs! :D
		memset(white_background.data(), FF, largeWH * largeWH * 4);
		layer_pixels(output_4c.data(), output_pixels.data(), white_background.data(), largeWH, largeWH, ch, largeWH, largeWH, 4, 0, 0);
		ch = 4;
		std::vector<uint8_t> scaled(largeWH * largeWH * ch);
		stbir_resize_uint8_linear(output_4c.data(), largeWH, largeWH, 0, scaled.data(), largeWH - 10, largeWH - 10, 0, STBIR_RGBA);//scale it down
		layer_pixels(output_4c.data(), icon_border48, scaled.data(), largeWH, largeWH, ch, largeWH - 10, largeWH - 10, ch, 5, 5);
		output_pixels = std::vector<uint8_t>(largeWH * largeWH * ch);
		memcpy(output_pixels.data(), output_4c.data(), largeWH * largeWH * ch);
	}

	large_3c = std::vector<uint8_t>(largeWH * largeWH * 3);
	if (ch == 4) {//rgba
		std::vector<uint8_t> white_background(largeWH * largeWH * 4);
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
		std::vector<uint8_t> white_background(largeWH * largeWH * ch);
		std::vector<uint8_t> output_4c(largeWH * largeWH * 4);
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
				std::vector<uint8_t> input_data((w * h * ich) + 0x20);
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
	std::vector<uint8_t> white_background(largeWH * largeWH * 4);// fix the bugs by not fixing the bugs! :D
	memset(white_background.data(), FF, largeWH * largeWH * 4);
	layer_pixels(large_4c.data(), output_pixels.data(), white_background.data(), largeWH, largeWH, ch, largeWH, largeWH, 4, 0, 0);

	small_4c = std::vector<uint8_t>(smallWH * smallWH * 4);
	stbir_resize_uint8_linear(large_4c.data(), largeWH, largeWH, 0, small_4c.data(), smallWH, smallWH, 0, STBIR_RGBA);// make the small icon

	if (borderMode == 1) {
		layer_pixels(small_4c.data(), icon_border24, small_4c.data(), smallWH, smallWH, 4, smallWH, smallWH, 4, 0, 0);

		layer_pixels(large_4c.data(), icon_border48, large_4c.data(), largeWH, largeWH, 4, largeWH, largeWH, 4, 0, 0);
	}
	else if (borderMode == 2) {
		std::vector<uint8_t> scaled((smallWH - 6) * (smallWH - 6) * 4);
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
	smdhOut << "SMDH";// make smdh!
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
