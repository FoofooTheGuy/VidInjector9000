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
