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
	stbir_resize_uint8(input_pixels, input_w, input_h, 0, scaled.data(), width, height, 0, num_channels);//scale it down
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
			ret = CBMDgetCommonCGFX(infile, compressedSize, decompressedSize, CGFXoffset, CGFXdecomp.data());
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
		return blankret;
	return 0;
}

int generate_preview(std::string inpath, int number, std::string outpath) {
	int mode = 0;
	std::string banner = "";
	std::string icon = "";
	int iconBorder = 2;
	std::string Sname = "";
	std::string Lname = "";
	std::string publisher = "";
	int copycheck = 0;
	std::string copyrightInfo = "";
	int FFrewind = 1;
	int FadeOpt = 1;
	uint8_t rows = 1;
	std::vector<std::string> PTitleVec = std::vector<std::string>(1, "");
	std::vector<std::string> MoflexVec = std::vector<std::string>(1, "");
	std::vector<std::string> MBannerVec = std::vector<std::string>(1, "");
	uint8_t splitPos = 0;
	int BannerPreviewIndex = 0;
	
	int res = loadParameters(inpath, mode, banner, icon, iconBorder, Sname, Lname, publisher, copycheck, copyrightInfo, FFrewind, FadeOpt, rows, PTitleVec, MoflexVec, MBannerVec, splitPos, BannerPreviewIndex);
	//uint32_t outrealint = 0;
	switch(number) {
		case 0: {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << IntMultiParam << ")\n" << NothingToDo << std::endl;
			return 4;
		}
		case 1: {
			res = generateBannerPreview(banner, outpath);
			//std::cout << banner << std::endl;
			return res;
		}
		case 2: {
			std::cout << icon << std::endl;
			break;
		}
		case 3: {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << IntIconBorderParam << ")\n" << NothingToDo << std::endl;
			//iconBorder = ((outrealint & 0xFF) > 2 ? 2 : (outrealint & 0xFF));
			return 5;
		}
		case 4: {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << StrSNameParam << ")\n" << NothingToDo << std::endl;
			//Sname = newValue;
			return 6;
		}
		case 5: {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << StrLNameParam << ")\n" << NothingToDo << std::endl;
			//Lname = newValue;
			return 7;
		}
		case 6: {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << StrPublisherParam << ")\n" << NothingToDo << std::endl;
			//publisher = newValue;
			return 8;
		}
		case 7: {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << IntCopycheckParam << ")\n" << NothingToDo << std::endl;
			//copycheck = outrealint ? 1 : 0;
			return 9;
		}
		case 8: {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << StrCopyrightParam << ")\n" << NothingToDo << std::endl;
			//copyrightInfo = newValue;
			return 10;
		}
		case 9: {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << IntFFrewindParam << ")\n" << NothingToDo << std::endl;
			//FFrewind = outrealint ? 1 : 0;
			return 11;
		}
		case 10: {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << IntFadeOptParam << ")\n" << NothingToDo << std::endl;
			//FadeOpt = outrealint ? 1 : 0;
			return 12;
		}
		case 11: {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << IntSplitPatchParam << ")\n" << NothingToDo << std::endl;
			//splitPos = (rows > 1) ? (outrealint < rows ? outrealint : 1) : 0;
			return 13;
		}
	}
	//beware of incoming jank. tread lightly...
	int rowcase = 12;
	for((void)rowcase; rowcase < PTitleVec.size(); rowcase++) {
		if(number == rowcase) {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << StrPTitleParam << ")\n" << NothingToDo << std::endl;
			return 14;
		}
	}
	rowcase = 12 + rows;
	for((void)rowcase; rowcase < MoflexVec.size(); rowcase++) {
		if(number == rowcase) {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << StrMoflexParam << ")\n" << NothingToDo << std::endl;
			return 15;
		}
	}
	rowcase = 12 + (rows * 2);
	for(auto &row : MBannerVec) {
		if(number == rowcase) {
			res = generateBannerPreview(row, outpath, true);
			//std::cout << row << std::endl;
			return res;
		}
		rowcase++;
	}
	return res;
}