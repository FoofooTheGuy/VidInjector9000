#include "archive.hpp"

int writeCIA(std::string& romfsPath, std::string& tempPath, std::string& productCode, std::string& outCIA) {
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

	if (chrcount(productCode) != 4) {
		std::cout << ErrorText << ' ' << ProductCodetext << ' ' << BadValue << std::endl;
		goto out10;
	}
	{
		std::string productCodeFull = "CTR-H-" + productCode;
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

	return 0;
}

void writeTAR(std::string& tempPath, std::string& outTAR) {
	std::error_code error;

	std::cout << CreatingFile << ' ' << outTAR.substr(outTAR.find_last_of("/\\") + 1) << std::endl;

	std::cout << TARremoveExisting << std::endl;
	if (std::filesystem::exists(std::filesystem::path((const char8_t*)&*outTAR.c_str()), error)) {//write wont work properly if it already exists
		std::filesystem::remove_all(std::filesystem::path((const char8_t*)&*outTAR.c_str()), error);
		if (error) {
			std::cout << ErrorText << ' ' << outTAR << '\n' << error.message() << std::endl;
		}
	}
	if(error) {
		std::cout << ErrorText << ' ' << outTAR << '\n' << error.message() << std::endl;
	}

	mtar_t tar;

	std::cout << TARopenArchive << std::endl;
	/* Open archive for writing */
	mtar_open(&tar, outTAR.c_str(), "wb");

	std::filesystem::path tarpath = tempPath + "/luma";
	if (tarpath.is_relative()) {
		tarpath = std::filesystem::absolute(tarpath);
	}
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

	return;
}
