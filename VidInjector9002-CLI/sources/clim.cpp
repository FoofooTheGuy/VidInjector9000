#include "clim.hpp"

void write_footer(std::ofstream *out, const footer *clim) {
	out->write(reinterpret_cast<const char*>(&clim->magic), sizeof(clim->magic));
	out->write(reinterpret_cast<const char*>(&clim->endianess), sizeof(clim->endianess));
	out->write(reinterpret_cast<const char*>(&clim->footersize), sizeof(clim->footersize));
	out->write(reinterpret_cast<const char*>(&clim->version), sizeof(clim->version));
	out->write(reinterpret_cast<const char*>(&clim->filesize), sizeof(clim->filesize));
	out->write(reinterpret_cast<const char*>(&clim->blockcount), sizeof(clim->blockcount));
	out->write(reinterpret_cast<const char*>(&clim->imag.magic), sizeof(clim->imag.magic));
	out->write(reinterpret_cast<const char*>(&clim->imag.chunksize), sizeof(clim->imag.chunksize));
	out->write(reinterpret_cast<const char*>(&clim->imag.width), sizeof(clim->imag.width));
	out->write(reinterpret_cast<const char*>(&clim->imag.height), sizeof(clim->imag.height));
	out->write(reinterpret_cast<const char*>(&clim->imag.format), sizeof(clim->imag.format));
	out->write(reinterpret_cast<const char*>(&clim->imag.reserved), sizeof(clim->imag.reserved));
	out->write(reinterpret_cast<const char*>(&clim->imag.reserved1), sizeof(clim->imag.reserved1));
	out->write(reinterpret_cast<const char*>(&clim->imag.reserved2), sizeof(clim->imag.reserved2));
	out->write(reinterpret_cast<const char*>(&clim->imag.footeroffset), sizeof(clim->imag.footeroffset));
}

size_t read_footer(std::ifstream *file, footer *clim) {
	const uint8_t bclim_footer_size = 0x28;

	file->seekg(0, std::ios_base::end);
	size_t filesize = file->tellg();
	if (filesize < bclim_footer_size) { // bruh
		return 0;
	}
	// seek to footer
	file->seekg(filesize - bclim_footer_size, std::ios_base::beg);
	size_t read_footeroffset = file->tellg();
	
	file->read(reinterpret_cast<char*>(&clim->magic), sizeof(clim->magic));
	file->read(reinterpret_cast<char*>(&clim->endianess), sizeof(clim->endianess));
	file->read(reinterpret_cast<char*>(&clim->footersize), sizeof(clim->footersize));
	file->read(reinterpret_cast<char*>(&clim->version), sizeof(clim->version));
	file->read(reinterpret_cast<char*>(&clim->filesize), sizeof(clim->filesize));
	file->read(reinterpret_cast<char*>(&clim->blockcount), sizeof(clim->blockcount));
	file->read(reinterpret_cast<char*>(&clim->imag.magic), sizeof(clim->imag.magic));
	file->read(reinterpret_cast<char*>(&clim->imag.chunksize), sizeof(clim->imag.chunksize));
	file->read(reinterpret_cast<char*>(&clim->imag.width), sizeof(clim->imag.width));
	file->read(reinterpret_cast<char*>(&clim->imag.height), sizeof(clim->imag.height));
	file->read(reinterpret_cast<char*>(&clim->imag.format), sizeof(clim->imag.format));
	file->read(reinterpret_cast<char*>(&clim->imag.reserved), sizeof(clim->imag.reserved));
	file->read(reinterpret_cast<char*>(&clim->imag.reserved1), sizeof(clim->imag.reserved1));
	file->read(reinterpret_cast<char*>(&clim->imag.reserved2), sizeof(clim->imag.reserved2));
	file->read(reinterpret_cast<char*>(&clim->imag.footeroffset), sizeof(clim->imag.footeroffset));
	return read_footeroffset;
}

int read_clim_image(const std::string &inpath, uint8_t* rgb565_pixels) {
	std::error_code error;
	const int out_w = 400;
	const int out_h = 240;
	const int full_w = 512;
	const int full_h = 256;
	
	if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*inpath.c_str()), error)) {
		return 1;
	}
	if (error) {
		std::cout << inpath << " (" << error.message() << ')' << std::endl;
		return 2;
	}
	
	std::string dotbclim = ".bclim";
	std::string extension = inpath;
	extension.erase(extension.begin(), extension.end() - dotbclim.size());
	if (extension != dotbclim) {
		return 3;
	}
	
	std::ifstream infile;
	infile.open(std::filesystem::path((const char8_t*)&*inpath.c_str()), std::ios_base::in | std::ios_base::binary);
	
	footer r_footer;
	size_t read_footeroffset = read_footer(&infile, &r_footer);
	if (!read_footeroffset) {
		return 4;
	}
	
	// check the info
	for(size_t i = 0; i < sizeof(r_footer.magic); i++) {
		//printf("%X\n", r_footer.magic[i]);
		if (r_footer.magic[i] != "CLIM"[i]) { // hehehe
			return 5;
		}
	}
	
	if (r_footer.version != 0x0202) {
		//std::cout << r_footer.version << std::endl;
		return 6;
	}
	
	for(size_t i = 0; i < sizeof(r_footer.imag.magic); i++) {
		//printf("%X\n", r_footer.imag.magic[i]);
		if (r_footer.imag.magic[i] != "imag"[i]) { // hehehe
			return 7;
		}
	}
	
	if (r_footer.imag.format != encoding::RGB565) { // TODO: decode other image encodings?
		//std::cout << r_footer.imag.format << std::endl;
		return 8;
	}
	
	if (r_footer.imag.footeroffset != read_footeroffset) {
		//std::cout << r_footer.imag.footeroffset << std::endl;
		return 9;
	}
	
	if(r_footer.imag.footeroffset != full_w * full_h * 2) {
		return 10;
	}
		
	if (r_footer.imag.width != out_w && r_footer.imag.height != out_h) {
		//std::cout << "width height " << r_footer.imag.width << ' ' << r_footer.imag.height << std::endl;
		return 11;
	}
	
	infile.seekg(0); // we should now be at the image data
	infile.read(reinterpret_cast<char*>(rgb565_pixels), r_footer.imag.footeroffset);
	return 0;
}
