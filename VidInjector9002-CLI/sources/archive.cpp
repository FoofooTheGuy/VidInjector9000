#include "archive.hpp"

int writeCIA(const std::string& romfsPath, const std::string& tempPath, const std::string& productCode, const std::string& outCIA) {
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

void writeTAR(const std::string& tempPath, const std::string& outTAR) {
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


int extractCIA(const std::string& inArc, const std::string& outDir, const std::string& seedPath) {
	std::error_code error;
	static nnc_result res;
	
	std::string romfspath = outDir + "/romfs";
	std::string exefspath = outDir + "/exefs";
	
	#define CUREC_SIZE 1024
	
	std::string extractErr = "";
	
	// stolen from the NNC cia test code and other places
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
	
	res = nnc_cia_open_content(&reader, 0, &ncch, &chunk); // read main content (index 0)
	if (res != NNC_R_OK)
		goto end1;
	
	// this section is here to test CBC seeking
	NNC_RS_CALL(ncch, seek_abs, 0);
	// DSiWare doesn't have an NCCH, lets not make this a fatal error...
	res = nnc_read_ncch_header(NNC_RSP(&ncch), &ncch_hdr);
	if (res != NNC_R_OK)
		goto end2;
	
	res = nnc_fill_keypair(&kp, &kset, NULL, &ncch_hdr);
	if (res == NNC_R_SEED_NOT_FOUND) {
		uint8_t seedData[NNC_SEED_SIZE];
		std::ifstream seedfile;
		if (seedPath.substr(seedPath.find_last_of("/\\") + 1) == "seeddb.bin") {
			if(getSeedFromTID(seedPath, ncch_hdr.title_id, seedData) != 0)
				std::cout << ErrorText << ' ' << SeedNotFound << std::endl;
		}
		else {
			if (std::filesystem::file_size(std::filesystem::path((const char8_t*)&*seedPath.c_str()), error) != NNC_SEED_SIZE) {
				if (error) {
					std::cout << ErrorText << ' ' << seedPath << '\n' << error.message() << std::endl;
				}
				goto end2;
			}
			if (error) {
				std::cout << ErrorText << ' ' << seedPath << '\n' << error.message() << std::endl;
				goto end2;
			}
			seedfile.open(std::filesystem::path((const char8_t*)&*seedPath.c_str()), std::ios_base::in | std::ios_base::binary);
			seedfile.read(reinterpret_cast<char*>(seedData), NNC_SEED_SIZE);
		}
		memcpy(ent.seed, seedData, NNC_SEED_SIZE);
		ent.title_id = ncch_hdr.title_id;
		sdb.entries = &ent;
		res = nnc_fill_keypair(&kp, &kset, &sdb, &ncch_hdr);
		if (error) {
			std::cout << ErrorText << ' ' << seedPath << '\n' << error.message() << std::endl;
			goto end2;
		}
	}
	if (res != NNC_R_OK) {
		goto end2;
	}
	
	res = nnc_ncch_exefs_full_stream(&ers, &ncch_hdr, NNC_RSP(&ncch), &kp);
	if (res != NNC_R_OK)
		goto end2;
	
	res = nnc_read_exefs_header(NNC_RSP(&ers), headers, NULL); // read exefs
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
	
	res = nnc_ncch_section_romfs(&ncch_hdr, NNC_RSP(&ncch), &kp, &rrs); // read romfs
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
	return 0;
}

int extractTAR(const std::string& inArc, const std::string& outDir, VI9Pparameters* parameters) {
	std::error_code error;
	
	std::string tempPath = resourcesPath + "/CLItemp";
	std::string exportsPath = tempPath + "/exports";
	std::string romfspath = outDir + "/romfs";
	std::string exefspath = outDir + "/exefs";
	
	int ret = 0;
	mtar_t tar;
	
	/* Open archive for reading */
	ret = mtar_open(&tar, inArc.c_str(), "rb"); // doesn't work with unicode
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
	for (const auto& entry : std::filesystem::recursive_directory_iterator(std::filesystem::path((const char8_t*)&*std::string(exportsPath + "/luma").c_str()), error)) { // do this because we dont know the title ID
		auto filename = std::filesystem::canonical(entry);
		if (std::filesystem::is_directory(entry, error) && filename.string().rfind("romfs") != std::string::npos) { // this only works because we know there isnt gonna be a "romfs" subdir
			lumaromfs = filename.string();
			break;
		}
	}
	for (int i = 0; i < MAX_ROWS; i++) { // uh probably better than checking the string
		if (std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(lumaromfs + "/movie/movie_" + std::to_string(i) + ".moflex").c_str()))) {
			parameters->splitPos = i & 0xFF;
			break;
		}
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
	std::filesystem::remove_all(exportsPath.c_str(), error); // lol
	if (error) {
		std::cout << ErrorText << ' ' << exportsPath << '\n' << error.message() << std::endl;
	}
	
	return 0;
}

// stolen from NNC romfs test
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
		//std::string extension = dir;
		//if (extension.find_last_of(".") != std::string::npos)
		//	extension.erase(extension.begin(), extension.begin() + extension.find_last_of("."));
		if (ent.type == nnc_romfs_info::NNC_ROMFS_DIR && strcmp(dir.c_str(), "movie") != 0 && strcmp(dir.c_str(), "settings") != 0) {// R.I.P. to all the files that we do not care about
			continue;
		}
		strcpy(pathbuf + len, dir.c_str());
		if (ent.type == nnc_romfs_info::NNC_ROMFS_DIR) {
			extract_dir(ctx, &ent, pathbuf, baselen);
		}
		else
		{
			nnc_wfile outf;
			res = nnc_wfile_open(&outf, pathbuf);
			if (res != NNC_R_OK) {
				continue;
			}
			/* empty files just need to be touched */
			if (ent.u.f.size)
			{
				nnc_u32 r;
				nnc_subview sv;
				res = nnc_romfs_open_subview(ctx, &sv, &ent);
				if (res != NNC_R_OK) {
					return "nnc_romfs_open_subview(ctx, &sv, &ent);";
				}
				nnc_copy(NNC_RSP(&sv), NNC_WSP(&outf), &r);
			}
			NNC_WS_CALL0(outf, close);
		}
	}
	if (res != NNC_R_OK) {
		return nnc_strerror(res);
	}
	return "";
}

// based off of NNC because NNC wasnt working
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
