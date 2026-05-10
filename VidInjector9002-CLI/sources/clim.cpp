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
