#include "form1.h"

using namespace VidInjector9002;

form1::form1() {
    //xtd::forms::message_box::show(*this, xtd::ustring::format("{}/{}/{}", ProgramDir, resourcesPath, settingsPath), FormText, xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::warning);
    top_most(true);
    loadSettings();
    //get the project path from args
    for (auto& v : argv) {
        size_t find = 0;
        find = v.find_last_of(".");
        if (find > v.size()) find = 0;//if find_first_of fails, this flings in the first numeral
        if (v.substr(find) == ".vi9p") {
            parampath = v;//give priority to the arg load
            LoadFromArgv = true;
            //xtd::forms::message_box::show(*this, xtd::ustring::format("found\n{}, {}, {}, {}", v, argv.at(0), parampath, find), FormText, xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::warning);
        }
    }
    argv.clear();

    text(FormText);
    client_size({ 1030, 885 });
    start_position(xtd::forms::form_start_position::center_screen);
    icon(xtd::drawing::icon::from_bitmap(pixels_to_image(logo90, 90, 90, 4)));

    tab_control.parent(*this);
    //tab_control.anchor(xtd::forms::anchor_styles::left | xtd::forms::anchor_styles::top | xtd::forms::anchor_styles::right | xtd::forms::anchor_styles::bottom);
    tab_control.location({ 5, 5 });
    tab_control.size({ client_size().width() - tab_control.location().x() * 2, client_size().height() - tab_control.location().y() * 2 });

    parameters.parent(tab_control);
    parameters.text(ParametersText);

    /*debugs.parent(parameters);
    debugs.auto_size(true);
    debugs.font(this->font());
    debugs.text(xtd::ustring::format("M{}, {}", finalize.width(), finalize.height()));
    debugs.location({ 0, 0 });*/

    //mode stuff
    modetxt.parent(parameters);
    modetxt.auto_size(true);
    modetxt.font({ this->font(), 15 });
    modetxt.location({ 20, 20 });
    modetxt.text(ModeText);

    mode.parent(parameters);
    mode.auto_size(true);
    mode.font(this->font());
    mode.location({ modetxt.location().x(), modetxt.location().y() + modetxt.height() });//tether to modetxt
    mode.items().push_back_range({ SingleVideo, MultiVideo });
    mode.selected_index(0);

    //banner stuff
    bannertxt.parent(parameters);
    bannertxt.auto_size(true);
    bannertxt.font({ this->font(), 15 });
    bannertxt.location({ mode.location().x(), 100});
    bannertxt.text(BannerText);

    bannerbox.parent(parameters);
    bannerbox.font(this->font());
    bannerbox.location({ bannertxt.location().x() + bannertxt.width() + 5, bannertxt.location().y() + (bannertxt.height() - bannerbox.height()) / 2 });//tether to bannertxt
    bannerbox.width(parameters.width() - bannerbox.location().x() - 599);
    bannerbox.cursor(xtd::forms::cursors::ibeam());

    bannerbrowse.parent(parameters);
    bannerbrowse.size({ 119, 35 });
    bannerbrowse.font(this->font());
    bannerbrowse.location({ parameters.width() - 597, bannerbox.location().y() - (bannerbrowse.height() / 2 - bannerbox.height() / 2) });//tether to bannerbox (this is here because the stuff that doesnt move is reliant on it)
    bannerbrowse.text(Browse);
    bannerbrowse.click += [&] {
        xtd::ustring filepath = load_file(xtd::ustring::format("{} {}{}{}", SupportedImage200x120, SupportedImageListBanner, CGFXList, AllFilesList), bannerbox.text(), xtd::environment::get_folder_path(xtd::environment::special_folder::my_pictures));
        if (!filepath.empty()) bannerbox.text(filepath);
    };

    bannererror.parent(parameters);
    bannererror.auto_size(true);
    bannererror.font({ this->font(), xtd::drawing::font_style::bold });
    bannererror.location({ bannerbox.location().x(), bannerbrowse.location().y() + bannerbrowse.height() });//put it below bannertxt
    bannererror.text(xtd::ustring::format("{} {}", ErrorText, ImageInfoError));
    bannererror.hide();

    bannerpreview.parent(parameters);
    //bannerpreview.border_style(xtd::forms::border_style::fixed_3d);
    bannerpreview.size({ 264, 154 });//add 4 to each if you do the border
    //bannerpreview.location({ (bannerbrowse.location().x() + bannerbrowse.width() + 2) + (((parameters.width() - (bannerbrowse.location().x() + bannerbrowse.width() + 2)) - bannerpreview.width()) / 2), (shortname.location().y() - (bannerpreview.height() + bannerpreviewtxt.height())) / 2 });
    {
        int ch = 4;
        int out_w = 200;
        int out_h = 120;
        int film_w = 264;
        int film_h = 154;
        uint8_t* output_4c = (uint8_t*)malloc(out_w * out_h * ch);
        memset(output_4c, 0xFF, out_w * out_h * ch);

        uint8_t* output_film = (uint8_t*)malloc(film_w * film_h * 4);
        layer_pixels(output_film, film_overlay, output_4c, film_w, film_h, 2, out_w, out_h, ch, 32, 11);
        free(output_4c);

        bannerpreview.image(pixels_to_image(output_film, film_w, film_h, 4));

        free(output_film);
    }

    bannerpreviewtxt.parent(parameters);
    bannerpreviewtxt.auto_size(true);
    bannerpreviewtxt.font(this->font());
    bannerpreviewtxt.text(BannerPreviewText);
    //bannerpreviewtxt.location({ bannerpreview.location().x() + (bannerpreview.width() - bannerpreviewtxt.width()) / 2, bannerpreview.location().y() + bannerpreview.height() });//put it below bannerpreview

    customnotif.parent(parameters);
    customnotif.auto_size(true);
    customnotif.font(this->font());
    //customnotif.location({ bannerpreview.location().x() + (bannerpreview.width() - customnotif.width()) / 2,  bannerpreview.location().y() + (bannerpreview.height() - customnotif.height()) / 2 });
    customnotif.text(CustomNotifText);
    customnotif.hide();

    bannerbox.text_changed += [&] {
        if (autoSaveParams && loaded) saveSettings();
        int w = 0, h = 0, ch = 0;
        int out_w = 200;
        int out_h = 120;
        int film_w = 264;
        int film_h = 154;
        uint8_t Checker[4];
        bool banner = false;
        int ich = sizeof(nnc_u16);
        std::ifstream inbanner(std::filesystem::path((const char8_t*)&*bannerbox.text().c_str()), std::ios::binary);
        if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*bannerbox.text().c_str()))) {
            setDefaultBannerPreview(bannerpreview, &bannererror);
            return;
        }
        for (int i = 0; i < 4; i++) {
            inbanner >> Checker[i];//https://stackoverflow.com/a/2974735
            if (Checker[i] == bannerMagic[i]) {
                bannerpreview.image(empty(0, 0));
                customnotif.show();
                banner = true;
            }
            else {
                bannerpreview.image(pixels_to_image(film_overlay, 264, 154, 2));
                customnotif.hide();
                banner = false;
                break;
            }
        }
        if (banner) {
            w = 256;
            h = 128;
            uint8_t* CGFXdecomp;
            uint8_t ret = 0;
            {
                uint32_t decompressedSize = 0;
                uint32_t compressedSize = 0;
                uint32_t CGFXoffset = 0;
                ret = getCBMDInfo(bannerbox.text(), &compressedSize, &decompressedSize, &CGFXoffset);
                if (ret > 0) {
                    bannerpreview.image(empty(0, 0));
                    customnotif.show();
                    return;
                }
                CGFXdecomp = (uint8_t*)malloc(decompressedSize);
                ret = CBMDgetCommonCGFX(bannerbox.text(), compressedSize, decompressedSize, CGFXoffset, CGFXdecomp);
                if (ret > 0) {
                    bannerpreview.image(empty(0, 0));
                    customnotif.show();
                    free(CGFXdecomp);
                    return;
                }
            }

            uint32_t dataOffset0;
            uint32_t height0;
            uint32_t width0;
            uint32_t mipmap0;
            uint32_t formatID0;
            uint32_t size0;
            ret = getCGFXtextureInfo(CGFXdecomp, "COMMON0", dataOffset0, height0, width0, mipmap0, formatID0, size0);
            if (ret > 0 || height0 != 0x80 || width0 != 0x100 || formatID0 != 0x3) {
                bannerpreview.image(empty(0, 0));
                customnotif.show();
                free(CGFXdecomp);
                return;
            }

            uint32_t dataOffset1;
            uint32_t height1;
            uint32_t width1;
            uint32_t mipmap1;
            uint32_t formatID1;
            uint32_t size1;
            ret = getCGFXtextureInfo(CGFXdecomp, "COMMON1", dataOffset1, height1, width1, mipmap1, formatID1, size1);
            if (ret > 0 || height1 != 0x80 || width1 != 0x100 || formatID1 != 0xC) {
                bannerpreview.image(empty(0, 0));
                customnotif.show();
                free(CGFXdecomp);
                return;
            }

            uint32_t hash = CRC32(&CGFXdecomp[dataOffset1], size1);//get COMMON1 hash
            if (hash != 0x2DB769E8) {
                bannerpreview.image(empty(0, 0));
                customnotif.show();
                free(CGFXdecomp);
                return;
            }
            int och = sizeof(nnc_u32);
            uint8_t* output_pixels = (uint8_t*)malloc(w * h * och);
            nnc_unswizzle_zorder_le_rgb565_to_be_rgba8(reinterpret_cast<nnc_u16*>(&CGFXdecomp[dataOffset0]), reinterpret_cast<nnc_u32*>(output_pixels), w & 0xFFFF, h & 0xFFFF);
            free(CGFXdecomp);
            uint8_t* output_cropped = (uint8_t*)malloc(out_w * out_w * och);
            crop_pixels(output_pixels, w, h, och, output_cropped, 0, 0, out_w, out_h);
            free(output_pixels);
            uint8_t* output_film = (uint8_t*)malloc(film_w * film_h * 4);
            layer_pixels(output_film, film_overlay, output_cropped, film_w, film_h, 2, out_w, out_h, 4, 32, 11);
            free(output_cropped);
            bannerpreview.image(pixels_to_image(output_film, film_w, film_h, 4));
            free(output_film);
            bannererror.hide();
            customnotif.hide();
            return;
        }
        std::string extension = bannerbox.text();
        if(extension.find_last_of(".") != std::string::npos)
            extension.erase(extension.begin(), extension.begin() + extension.find_last_of("."));
        if (extension == ".bimg") {
            if (std::filesystem::file_size(std::filesystem::path((const char8_t*)&*bannerbox.text().c_str())) == 0x10020) {
                w = 256;
                h = 128;
                int och = sizeof(nnc_u32);
                std::ifstream input;
                input.open(std::filesystem::path((const char8_t*)&*bannerbox.text().c_str()), std::ios_base::in | std::ios_base::binary);//input file
                uint8_t* input_data = (uint8_t*)malloc((w * h * ich) + 0x20);
                input.read(reinterpret_cast<char*>(input_data), (w * h * ich) + 0x20);
                input.close();
                for (int i = 0; i < 0x1C; i++) {
                    if (input_data[i] != bimgheader[i]) {
                        free(input_data);
                        setDefaultBannerPreview(bannerpreview, &bannererror);
                        return;
                    }
                }
                uint8_t* output_pixels = (uint8_t*)malloc(w * h * och);
                nnc_unswizzle_zorder_le_rgb565_to_be_rgba8(reinterpret_cast<nnc_u16*>(&input_data[0x20]), reinterpret_cast<nnc_u32*>(output_pixels), w & 0xFFFF, h & 0xFFFF);
                free(input_data);
                uint8_t* output_cropped = (uint8_t*)malloc(out_w * out_w * och);
                crop_pixels(output_pixels, w, h, och, output_cropped, 0, 0, out_w, out_h);
                free(output_pixels);
                uint8_t* output_film = (uint8_t*)malloc(film_w * film_h * 4);
                layer_pixels(output_film, film_overlay, output_cropped, film_w, film_h, 2, out_w, out_h, 4, 32, 11);
                //stbi_write_png(xtd::ustring::format("{}/{}/output_film.png", ProgramDir, resourcesPath).c_str(), film_w, film_h, och, output_film, 0);
                free(output_cropped);
                bannerpreview.image(pixels_to_image(output_film, film_w, film_h, 4));
                bannererror.hide();
                free(output_film);
                customnotif.hide();
            }
            else {
                setDefaultBannerPreview(bannerpreview, &bannererror);
            }
        }
        else if (!banner) {
            if (stbi_info(bannerbox.text().c_str(), &w, &h, &ch)) {
                uint8_t* input_pixels = stbi_load(bannerbox.text().c_str(), &w, &h, &ch, 0);
                uint8_t* output_pixels = (uint8_t*)malloc(out_w * out_h * ch);

                if (w == out_w && h == out_h) memcpy(output_pixels, input_pixels, w * h * ch);
                else resize_crop(input_pixels, w, h, output_pixels, out_w, out_h, ch);//scale to 200x120 if needed
                free(input_pixels);
                uint8_t* output_4c = (uint8_t*)malloc(out_w * out_h * 4);
                uint8_t* white = (uint8_t*)malloc(out_w * out_h * 4);
                memset(white, 0xFF, out_w * out_h * 4);
                layer_pixels(output_4c, output_pixels, white, out_w, out_h, ch, out_w, out_h, 4, 0, 0);
                free(white);
                free(output_pixels);
                uint8_t* output_film = (uint8_t*)malloc(film_w * film_h * 4);
                //memcpy(output_film, film_overlay, film_w * film_h * 4);
                layer_pixels(output_film, film_overlay, output_4c, film_w, film_h, 2, out_w, out_h, 4, 32, 11);
                free(output_4c);
                bannerpreview.image(pixels_to_image(output_film, film_w, film_h, 4));
                bannererror.hide();
                free(output_film);
                customnotif.hide();
            }
            else {
                setDefaultBannerPreview(bannerpreview, &bannererror);
            }
        }
    };

    //icon stuff
    icontxt.parent(parameters);
    icontxt.auto_size(true);
    icontxt.font({ this->font(), 15 });
    icontxt.location({ 20, bannererror.location().y() + bannererror.height() });//put it below customtxt
    icontxt.text(IconText);

    iconbox.parent(parameters);
    iconbox.font(this->font());
    iconbox.location({ icontxt.location().x() + icontxt.width() + 5, icontxt.location().y() + (icontxt.height() - iconbox.height()) / 2 });//tether to icontxt
    iconbox.width(parameters.width() - iconbox.location().x() - 654);
    iconbox.cursor(xtd::forms::cursors::ibeam());

    iconbrowse.parent(parameters);
    iconbrowse.size({ 119, 35 });
    iconbrowse.font(this->font());
    iconbrowse.location({ iconbox.location().x() + iconbox.width() + 1, iconbox.location().y() - (iconbrowse.height() / 2 - iconbox.height() / 2) });//tether to iconbox
    iconbrowse.text(Browse);
    iconbrowse.click += [&] {
        xtd::ustring filepath = load_file(xtd::ustring::format("{} {}{}{}", SupportedImage48x48, SupportedImageList, SMDHList, AllFilesList), iconbox.text(), xtd::environment::get_folder_path(xtd::environment::special_folder::my_pictures));
        if (!filepath.empty()) iconbox.text(filepath);
    };

    iconpreview.parent(parameters);
    iconpreview.cursor(xtd::forms::cursors::hand());
    iconpreview.border_style(xtd::forms::border_style::fixed_3d);
    iconpreview.size({ 52, 52 });
    //if(loaded) SetIconPreview();
    //iconpreview.location({ iconbrowse.location().x() + iconbrowse.width() + 2, iconbrowse.location().y() + 2 });

    iconerror.parent(parameters);
    iconerror.auto_size(true);
    iconerror.font({ this->font(), xtd::drawing::font_style::bold });
    iconerror.location({ iconbox.location().x(), iconbrowse.location().y() + iconbrowse.height() });//put it below bannertxt
    iconerror.text(xtd::ustring::format("{} {}", ErrorText, ImageInfoError));
    iconerror.hide();

    iconbox.text_changed += [&] {
        if (autoSaveParams && loaded) saveSettings();
        if (loaded) SetIconPreview();
    };

    iconpreview.click += [&] {
        borderMode++;
        if (borderMode > 2) borderMode = 0;
        SetIconPreview(false);
        if(autoSaveParams && loaded) saveSettings();
    };

    shortnametxt.parent(parameters);
    shortnametxt.auto_size(true);
    shortnametxt.font({ this->font(), 15 });
    shortnametxt.location({ 20, iconerror.location().y() + iconerror.height() + 1 });//put it below icontxt
    shortnametxt.text(ShortNameText);

    shortname.parent(parameters);
    shortname.multiline(true);
    shortname.font(this->font());
    shortname.location({ shortnametxt.location().x() + shortnametxt.width() + 5, shortnametxt.location().y() + (shortnametxt.height() - shortname.height()) / 2 });//tether to shortnametxt
    shortname.size({ parameters.width() - shortname.location().x() - 23, shortname.height() * 2 });
    shortname.cursor(xtd::forms::cursors::ibeam());

    shorterror.parent(parameters);
    shorterror.auto_size(true);
    shorterror.font({ this->font(), xtd::drawing::font_style::bold });
    shorterror.location({ shortname.location().x(), shortname.location().y() + shortname.height() });//put it below shortname
    shorterror.text(xtd::ustring::format("{} {} ({}/{})", ErrorText, TextTooLongError, chrcount(shortname.text()), 64));
    shorterror.hide();

    shortname.text_changed += [&] {
        if (autoSaveParams && loaded) saveSettings();
        if (chrcount(shortname.text()) > 64) {
            shorterror.text(xtd::ustring::format("{} {} ({}/{})", ErrorText, TextTooLongError, chrcount(shortname.text()), 64));
            shorterror.show();
        }
        else shorterror.hide();
    };

    longnametxt.parent(parameters);
    longnametxt.auto_size(true);
    longnametxt.font({ this->font(), 15 });
    longnametxt.location({ shortnametxt.location().x(), shorterror.location().y() + shorterror.height() });//put it below short error
    longnametxt.text(LongNameText);

    longname.parent(parameters);
    longname.multiline(true);
    longname.font(this->font());
    longname.location({ longnametxt.location().x() + longnametxt.width() + 5, longnametxt.location().y() + (longnametxt.height() - longname.height()) / 2 });//tether to longnametxt
    longname.size({ parameters.width() - longname.location().x() - 23, longname.height() * 2 });
    longname.cursor(xtd::forms::cursors::ibeam());

    longerror.parent(parameters);
    longerror.auto_size(true);
    longerror.font({ this->font(), xtd::drawing::font_style::bold });
    longerror.location({ longname.location().x(), longname.location().y() + longname.height() });//tether to longname box
    longerror.text(xtd::ustring::format("{} {} ({}/{})", ErrorText, TextTooLongError, chrcount(longname.text()), 128));
    longerror.hide();

    longname.text_changed += [&] {
        if (autoSaveParams && loaded) saveSettings();
        if (chrcount(longname.text()) > 128) {
            longerror.text(xtd::ustring::format("{} {} ({}/{})", ErrorText, TextTooLongError, chrcount(longname.text()), 128));
            longerror.show();
        }
        else longerror.hide();
    };

    publishertxt.parent(parameters);
    publishertxt.auto_size(true);
    publishertxt.font({ this->font(), 15 });
    publishertxt.location({ longnametxt.location().x(), longerror.location().y() + longerror.height() });//put it below long name
    publishertxt.text(PublisherText);

    publisher.parent(parameters);
    publisher.multiline(true);
    publisher.font(this->font());
    publisher.location({ publishertxt.location().x() + publishertxt.width() + 5, publishertxt.location().y() + (publishertxt.height() - publisher.height()) / 2 });//tether to publishertxt
    publisher.size({ parameters.width() - publisher.location().x() - 23, publisher.height() * 2 });
    publisher.cursor(xtd::forms::cursors::ibeam());

    publishererror.parent(parameters);
    publishererror.auto_size(true);
    publishererror.font({ this->font(), xtd::drawing::font_style::bold });
    publishererror.location({ publisher.location().x(), publisher.location().y() + publisher.height() });//tether to publisher box
    publishererror.text(xtd::ustring::format("{} {} ({}/{})", ErrorText, TextTooLongError, chrcount(publisher.text()), 64));
    publishererror.hide();

    publisher.text_changed += [&] {
        if (autoSaveParams && loaded) saveSettings();
        if (chrcount(publisher.text()) > 64) {
            publishererror.text(xtd::ustring::format("{} {} ({}/{})", ErrorText, TextTooLongError, chrcount(publisher.text()), 64));
            publishererror.show();
        }
        else publishererror.hide();
    };

    //multionly
    MultiOnly.parent(parameters);
    MultiOnly.font({ this->font(), 8 });
    MultiOnly.auto_size(true);
    MultiOnly.text(MultiOnlyText);
    MultiOnly.border_style(xtd::forms::border_style::fixed_single);
    MultiOnly.hide();

    //copyright stuff (multivideo only)
    copycheck.parent(parameters);
    copycheck.font({ this->font(), 15 });
    copycheck.flat_style(xtd::forms::flat_style::system);
    copycheck.auto_size(true);
    copycheck.text(CopyrightCheckText);
    copycheck.text_align(xtd::forms::content_alignment::middle_left);
    copycheck.location({ publishertxt.location().x(), publishererror.location().y() + publishererror.height() });
    copycheck.checked_changed += [&](object& sender, const xtd::event_args& e) {
        if (autoSaveParams && loaded) saveSettings();
        copybox.enabled(mode.selected_index() && copycheck.checked());
    };

    copybox.parent(parameters);
    copybox.multiline(true);
    copybox.accepts_return(true);
    copybox.location({ copycheck.location().x(), copycheck.location().y() + copycheck.height() + 2 });
    copybox.size({ 320, 200 });
    //copybox.content_align(xtd::forms::content_alignment::middle_center);
    copybox.cursor(xtd::forms::cursors::ibeam());
    copybox.text_changed += [&] {
        if (autoSaveParams && loaded) saveSettings();
    };

    copymulti.parent(parameters);
    copymulti.size({ 9, 9 });
    {
        int w = 9, h = 9, ch = 4;
        copymulti.image(LightDark.checked() ? pixels_to_image(invert_pixels(stbi_load(xtd::ustring::format("{}/{}/language/{}/M.bmp", ProgramDir, resourcesPath, Languagedir).c_str(), &w, &h, &ch, 0), w, h, ch), w, h, ch) : pixels_to_image(stbi_load(xtd::ustring::format("{}/{}/language/{}/M.bmp", ProgramDir, resourcesPath, Languagedir).c_str(), &w, &h, &ch, 0), w, h, ch));
    }
    copymulti.location({ copycheck.location().x() + copycheck.width() + 3, copycheck.location().y() + ((copycheck.height() - copymulti.height()) / 2) });
    copymulti.cursor(xtd::forms::cursors::help());
    {
        static bool justchanged = false;
        copymulti.mouse_move += [&](object& sender, const xtd::forms::mouse_event_args& e) {
            if (justchanged)
                return;
            MultiOnly.location({ copymulti.location().x() + e.location().x(), copymulti.location().y() + e.location().y() - MultiOnly.height() });
            if (MultiOnly.location().x() + MultiOnly.width() > parameters.width())
                MultiOnly.location({ copymulti.location().x() + e.location().x() - MultiOnly.width(), copymulti.location().y() + e.location().y() - MultiOnly.height() });
            justchanged = true;
            MultiOnly.show();
        };
        copymulti.mouse_leave += [&] {
            justchanged = false;
            MultiOnly.hide();
        };
    }
    copycheck.enabled(mode.selected_index());//set enabledness
    copybox.enabled(mode.selected_index());

    //fast forward/rewind and fade after 5 sec
    FFrewind.parent(parameters);
    FFrewind.font({ this->font(), 12 });
    FFrewind.flat_style(xtd::forms::flat_style::system);
    FFrewind.auto_size(true);
    FFrewind.text(FFrewindText);
    FFrewind.text_align(xtd::forms::content_alignment::middle_left);
    FFrewind.checked(true);
    //FFrewind.location({ copybox.location().x() + copybox.width() + 7, copycheck.location().y() + ((((copycheck.height() + copybox.height() + 2) / 2) - FFrewind.height()) / 2)});

    FadeOpt.parent(parameters);
    FadeOpt.font({ this->font(), 12 });
    FadeOpt.flat_style(xtd::forms::flat_style::system);
    FadeOpt.auto_size(true);
    FadeOpt.text(FadeOptText);
    FadeOpt.text_align(xtd::forms::content_alignment::middle_left);
    FadeOpt.checked(true);
    //FadeOpt.location({ FFrewind.location().x(), ((copycheck.height() + copybox.height() + 2) / 2) + copycheck.location().y() + ((((copycheck.height() + copybox.height() + 2) / 2) - FFrewind.height()) / 2)});

    //text box array stuff
    playertitletxt.parent(parameters);
    playertitletxt.auto_size(true);
    playertitletxt.font({ this->font(), 15 });
    playertitletxt.text(PlayerTitleText);

    moflextxt.parent(parameters);
    moflextxt.auto_size(true);
    moflextxt.font({ this->font(), 15 });
    moflextxt.text(MoflexFileText);

    /*menutitletxt.parent(parameters);
    menutitletxt.auto_size(true);
    menutitletxt.font({ this->font(), 15 });
    menutitletxt.text(MenuTitleText);
    menutitletxt.enabled(mode.selected_index());*/

    menubannertxt.parent(parameters);
    menubannertxt.auto_size(true);
    menubannertxt.font({ this->font(), 15 });
    menubannertxt.text(MenuBannerText);
    menubannertxt.enabled(mode.selected_index());

    mediabox.parent(parameters);
    mediabox.border_style(xtd::forms::border_style::fixed_3d);
    mediabox.location({ copybox.location().x(), copybox.location().y() + copybox.height() + playertitletxt.height() });
    //mediabox.size({ parameters.width() - copybox.location().x() * 2, parameters.height() - (copybox.location().y() + copybox.height() + playertitletxt.height()) - copybox.location().x() });
    mediabox.auto_scroll(true);

    /*titlemulti.parent(parameters);
    titlemulti.size({ 9, 9 });
    {
        int w = 9, h = 9, ch = 4;
        titlemulti.image(LightDark.checked() ? pixels_to_image(invert_pixels(stbi_load(xtd::ustring::format("{}/{}/language/{}/M.bmp", ProgramDir, resourcesPath, Languagedir).c_str(), &w, &h, &ch, 0), w, h, ch), w, h, ch) : pixels_to_image(stbi_load(xtd::ustring::format("{}/{}/language/{}/M.bmp", ProgramDir, resourcesPath, Languagedir).c_str(), &w, &h, &ch, 0), w, h, ch));
    }
    titlemulti.cursor(xtd::forms::cursors::help());
    titlemulti.mouse_move += [&](object& sender, const xtd::forms::mouse_event_args& e) {
        MultiOnly.location({ titlemulti.location().x() + e.location().x(), titlemulti.location().y() + e.location().y() - MultiOnly.height() });
        if (MultiOnly.location().x() + MultiOnly.width() > parameters.width())
            MultiOnly.location({ titlemulti.location().x() + e.location().x() - MultiOnly.width(), titlemulti.location().y() + e.location().y() - MultiOnly.height() });
        MultiOnly.show();
    };
    titlemulti.mouse_leave += [&] {
        MultiOnly.hide();
    };
    titlemulti.location({ menutitletxt.location().x() + menutitletxt.width() + 3, menutitletxt.location().y() + ((menutitletxt.height() - titlemulti.height()) / 2) });*/

    bannermulti.parent(parameters);
    bannermulti.size({ 9, 9 });
    {
        int w = 9, h = 9, ch = 4;
        bannermulti.image(LightDark.checked() ? pixels_to_image(invert_pixels(stbi_load(xtd::ustring::format("{}/{}/language/{}/M.bmp", ProgramDir, resourcesPath, Languagedir).c_str(), &w, &h, &ch, 0), w, h, ch), w, h, ch) : pixels_to_image(stbi_load(xtd::ustring::format("{}/{}/language/{}/M.bmp", ProgramDir, resourcesPath, Languagedir).c_str(), &w, &h, &ch, 0), w, h, ch));
    }
    bannermulti.cursor(xtd::forms::cursors::help());
    {
        static bool justchanged = false;
        bannermulti.mouse_move += [&](object& sender, const xtd::forms::mouse_event_args& e) {
            if (justchanged)
                return;
            MultiOnly.location({ bannermulti.location().x() + e.location().x(), bannermulti.location().y() + e.location().y() - MultiOnly.height() });
            if (MultiOnly.location().x() + MultiOnly.width() > parameters.width())
                MultiOnly.location({ bannermulti.location().x() + e.location().x() - MultiOnly.width(), bannermulti.location().y() + e.location().y() - MultiOnly.height() });
            justchanged = true;
            MultiOnly.show();
        };
        bannermulti.mouse_leave += [&] {
            justchanged = false;
            MultiOnly.hide();
        };
    }

    menubannerpreview.parent(parameters);
    menubannerpreview.cursor(mode.selected_index() ? xtd::forms::cursors::hand() : xtd::forms::cursors::no());
    //menubannerpreview.border_style(xtd::forms::border_style::fixed_3d);
    menubannerpreview.size({ 264, 154 });
    //menubannerpreview.location({ copybox.location().x() + copybox.width() + (parameters.width() - (copybox.location().x() + copybox.width())) / 2, copycheck.location().y() + ((copycheck.height() + copybox.height() + 3) - menubannerpreview.height()) / 2});
    menubannerpreview.click += [&] {
        if (autoSaveParams && loaded) saveSettings();
        if (mode.selected_index()) {
            int emptyRow = 0;
            if (mode.selected_index() == 1) {
                for (emptyRow = 0; emptyRow < rows - 1; emptyRow++) {
                    if (text_box_array[emptyRow * columns + 2]->text().empty()) {
                        break;
                    }
                }
            }
            xtd::ustring filepath = "";
            if(text_box_array.at(bannerpreviewindex * columns + 2)->text().empty())
                filepath = load_file(xtd::ustring::format("{} {}", SupportedImage200x120, SupportedImageListBanner), text_box_array.at((emptyRow - 1 < 0 ? 0 : emptyRow - 1) * columns + 2)->text(), xtd::environment::get_folder_path(xtd::environment::special_folder::my_pictures));
            else 
                filepath = load_file(xtd::ustring::format("{} {}", SupportedImage200x120, SupportedImageListBanner), text_box_array.at(bannerpreviewindex * columns + 2)->text(), xtd::environment::get_folder_path(xtd::environment::special_folder::my_pictures));
            if (!filepath.empty()) text_box_array.at(bannerpreviewindex * columns + 2)->text(filepath);
            setMultiBannerPreview(bannerpreviewindex);
        }
    };

    indextxt.parent(parameters);
    indextxt.auto_size(true);
    indextxt.font(this->font());
    indextxt.text(xtd::ustring::format("{}/{}", bannerpreviewindex, rows));
    if (mode.selected_index()) indextxt.show();
    else indextxt.hide();

    bannerpreviewleft.parent(parameters);
    bannerpreviewleft.auto_size(true);
    bannerpreviewleft.image(pixels_to_image(arrow_left, 6, 6, 2));
    bannerpreviewleft.click += [&] {
        if (bannerpreviewindex != 0)
            bannerpreviewindex--;
        bannerpreviewleft.enabled(bannerpreviewindex != 0);//if it returns false, it means it's at 0 and it turns false, or on if it's true
        setMultiBannerPreview(bannerpreviewindex);
        if (autoSaveParams && loaded) saveSettings();
    };
    bannerpreviewleft.enabled(mode.selected_index());

    bannerpreviewright.parent(parameters);
    bannerpreviewright.auto_size(true);
    bannerpreviewright.image(pixels_to_image(arrow_right, 6, 6, 2));
    bannerpreviewright.click += [&] {
        if (bannerpreviewindex != rows - 1)
            bannerpreviewindex++;
        bannerpreviewright.enabled(bannerpreviewindex != rows - 1);
        setMultiBannerPreview(bannerpreviewindex);
        if (autoSaveParams && loaded) saveSettings();
    };
    bannerpreviewright.enabled(mode.selected_index());

    for (uint8_t y = 0; y < rows; y++) {
        for (uint8_t x = 0; x < columns; x++) {
            xtd::forms::text_box* box_new = new xtd::forms::text_box();
            text_box_array.push_back(box_new);//put box in array
            text_box_array.at(y * columns + x)->parent(mediabox);//assign it data
            text_box_array.at(y * columns + x)->font(this->font());
            text_box_array.at(y * columns + x)->back_color(y % 2 ? (this->back_color() == xtd::drawing::color::white ? xtd::drawing::color::from_argb(0xF5, 0xF5, 0xF5) : xtd::drawing::color::from_argb(0x10, 0x10, 0x10)) : this->back_color());
            //text_box_array.at(y * columns + x)->width(((mediabox.width() - text_box_array.at(y * columns)->height() / 2) / columns) - ((copybox.location().x() / columns) + 1));
            text_box_array.at(y * columns + x)->cursor(xtd::forms::cursors::ibeam());
            text_box_array.at(y * columns + x)->text_changed += [&] {
                if (autoSaveParams && loaded) saveSettings();
            };
            if(y * columns + x > 1)
                text_box_array.at(y * columns + x)->enabled(mode.selected_index());
            if (x == 2) {//if column is multi banner
                text_box_array.at(y * columns + x)->text_changed += [&](object& sender, const xtd::event_args& e) {
                    if (loaded) {
                        for (uint8_t r = 0; r < rows; r++)
                            if (text_box_array.at(r * columns + 2)->get_hash_code() == sender.get_hash_code()) {
                                if(bannerpreviewindex == r)
                                    setMultiBannerPreview(r);
                            }
                    }
                };
                text_box_array.at(y * columns + x)->mouse_click += [&](object& sender, const xtd::forms::mouse_event_args& e) {
                    if (loaded) {
                        for (uint8_t r = 0; r < rows; r++)
                            if (text_box_array.at(r * columns + 2)->get_hash_code() == sender.get_hash_code()) {
                                setMultiBannerPreview(r);
                            }
                    }
                };
            }
        }
        for (uint8_t i = 0; i < 2; i++) {
            xtd::forms::picture_box* button_new = new xtd::forms::picture_box();
            butt_array.push_back(button_new);
            butt_array.at(y * 2 + i)->parent(mediabox);
            butt_array.at(y * 2 + i)->border_style(xtd::forms::border_style::fixed_3d);
            butt_array.at(y * 2 + i)->cursor(xtd::forms::cursors::hand());
            butt_array.at(y * 2 + i)->size({ text_box_array.at(0)->height() / 2, text_box_array.at(0)->height() / 2 });
            if (i == 0) {//if even (up button)
                butt_array.at(y * 2)->image(pixels_to_image(arrow_up, 6, 6, 2));
                butt_array.at(y * 2)->mouse_click += [&](object& sender, const xtd::forms::mouse_event_args& e) {
                    for (uint8_t r = 0; r < rows * 2; r += 2) {
                        if (butt_array.at(r)->get_hash_code() == sender.get_hash_code()) {
                            if (r == 0) return;//impossible?
                            for (uint8_t c = 0; c < columns; c++) {
                                xtd::ustring tempstr;
                                tempstr = text_box_array.at((r / 2 - 1) * columns + c)->text();
                                text_box_array.at((r / 2 - 1) * columns + c)->text(text_box_array.at((r / 2) * columns + c)->text());
                                text_box_array.at((r / 2) * columns + c)->text(tempstr);
                            }
                            if (r / 2 == bannerpreviewindex || r / 2 - 1 == bannerpreviewindex)
                                setMultiBannerPreview(bannerpreviewindex);
                            return;
                        }
                    }
                };
            }
            else if (i == 1) {//if odd (down button)
                butt_array.at(y * 2 + 1)->image(pixels_to_image(arrow_down, 6, 6, 2));
                butt_array.at(y * 2 + 1)->mouse_click += [&](object& sender, const xtd::forms::mouse_event_args& e) {
                    for (uint8_t r = 1; r < rows * 2; r += 2) {
                        if (butt_array.at(r)->get_hash_code() == sender.get_hash_code()) {
                            if (r == butt_array.size() - 1) return;//impossible?
                            for (uint8_t c = 0; c < columns; c++) {
                                xtd::ustring tempstr;
                                tempstr = text_box_array.at((r / 2 + 1) * columns + c)->text();
                                text_box_array.at((r / 2 + 1) * columns + c)->text(text_box_array.at((r / 2) * columns + c)->text());
                                text_box_array.at((r / 2) * columns + c)->text(tempstr);
                                if (r / 2 == bannerpreviewindex)
                                    setMultiBannerPreview(bannerpreviewindex);
                            }
                            if (r / 2 == bannerpreviewindex || r / 2 + 1 == bannerpreviewindex)
                                setMultiBannerPreview(bannerpreviewindex);
                            return;
                        }
                    }
                };
            }
        }
    }
    /*for (uint8_t y = 0; y < rows; y++) {
        
    }*/
    butt_array.at(0)->hide();
    butt_array.at(butt_array.size() - 1)->hide();
    setMultiBannerPreview(rows - 1);

    moflexbrowse.parent(mediabox);
    moflexbrowse.size({ 119, 35 });
    moflexbrowse.font(this->font());
    //moflexbrowse.location({ text_box_array.at((rows - 1) * columns + 1)->location().x() + (text_box_array.at((rows - 1) * columns + 1)->width() - moflexbrowse.width()) / 2, text_box_array.at((rows - 1) * columns + 1)->location().y() + text_box_array.at((rows - 1) * columns + 1)->height() });
    moflexbrowse.text(Browse);
    moflexbrowse.click += [&] {
        //disabled because it doesnt put the full path for some reason, only file name, and idk how to fix, if it's even possible for me to fix it
        /*if (mode.selected_index()) {
            std::vector<xtd::ustring> files = load_files(MoflexFilesList, text_box_array[(rows - 1) * columns + 1]->text(), xtd::environment::get_folder_path(xtd::environment::special_folder::my_videos));
            if (files.size() == 1 && !files.empty()) text_box_array[(rows - 1) * columns + 1]->text(files.at(0));
            else {
                for (size_t i = 0; i < files.size(); i++) {
                    if (i > rows - 1) break;
                    if (!files.at(i).empty()) text_box_array[i * columns + 1]->text(files.at(i));
                }
            }
        }
        else {*/
        int emptyRow = 0;
        if (mode.selected_index() == 1) {
            for (emptyRow = 0; emptyRow < rows - 1; emptyRow++) {
                if (text_box_array[emptyRow * columns + 1]->text().empty()) {
                    break;
                }
            }
        }
        xtd::ustring filepath = "";
        if(!text_box_array[(emptyRow - 1 < 0 ? 0 : emptyRow - 1) * columns + 1]->text().empty())
            filepath = load_file(MoflexFilesList, text_box_array[(emptyRow - 1 < 0 ? 0 : emptyRow - 1) * columns + 1]->text(), xtd::environment::get_folder_path(xtd::environment::special_folder::my_videos));
        else
            filepath = load_file(MoflexFilesList, text_box_array[(rows - 1) * columns + 1]->text(), xtd::environment::get_folder_path(xtd::environment::special_folder::my_videos));
        if (filepath.empty()) return;//big brain time

        text_box_array[emptyRow * columns + 1]->text(filepath);
        //}//unmark this if you ever find a fix for it lol
    };

    multibannerbrowse.parent(mediabox);
    multibannerbrowse.size({ 119, 35 });
    multibannerbrowse.font(this->font());
    //multibannerbrowse.location({ text_box_array.at((rows - 1) * columns + 3)->location().x() + (text_box_array.at((rows - 1) * columns + 3)->width() - moflexbrowse.width()) / 2, text_box_array.at((rows - 1) * columns + 3)->location().y() + text_box_array.at((rows - 1) * columns + 3)->height() });
    multibannerbrowse.text(Browse);
    multibannerbrowse.click += [&] {
        /*
        std::vector<xtd::ustring> files = load_files(xtd::ustring::format("{} {}{}", SupportedImage200x120, SupportedImageList));
        if (files.size() == 1 && !files.empty()) text_box_array[(rows - 1) * columns + 2]->text(files.at(0));
        else {
            for (size_t i = 0; i < files.size(); i++) {
                if (i > rows - 1) break;
                if (!files.at(i).empty()) text_box_array[i * columns + 2]->text(files.at(i));
            }
        }*/
        uint8_t emptyRow;
        for (emptyRow = 0; emptyRow < rows - 1; emptyRow++) {
            if (text_box_array[emptyRow * columns + 2]->text().empty()) {
                break;
            }
        }
        xtd::ustring filepath = "";
        //xtd::ustring filepath = load_file(xtd::ustring::format("{} {}", SupportedImage200x120, SupportedImageListBanner), text_box_array.at((rows - 1) * columns + 2)->text(), xtd::environment::get_folder_path(xtd::environment::special_folder::my_pictures));
        if (!text_box_array[(emptyRow - 1 < 0 ? 0 : emptyRow - 1) * columns + 2]->text().empty())
            filepath = load_file(xtd::ustring::format("{} {}", SupportedImage200x120, SupportedImageListBanner), text_box_array[(emptyRow - 1 < 0 ? 0 : emptyRow - 1) * columns + 2]->text(), xtd::environment::get_folder_path(xtd::environment::special_folder::my_pictures));
        else
            filepath = load_file(xtd::ustring::format("{} {}", SupportedImage200x120, SupportedImageListBanner), text_box_array[(rows - 1) * columns + 2]->text(), xtd::environment::get_folder_path(xtd::environment::special_folder::my_pictures));
        if (filepath.empty()) return;

        text_box_array.at(emptyRow * columns + 2)->text(filepath);
        setMultiBannerPreview(emptyRow);
    };
    multibannerbrowse.enabled(mode.selected_index());

    rowtxt.parent(mediabox);
    rowtxt.auto_size(true);
    rowtxt.font(this->font());
    rowtxt.text(xtd::ustring::format("{}/{}", rows, 27));
    if (mode.selected_index()) rowtxt.show();
    else rowtxt.hide();

    appendmedia.parent(mediabox);
    appendmedia.auto_size(true);
    appendmedia.font({ this->font(), 20 });
    appendmedia.text("+");
    appendmedia.click += [&] {
        doAppendMedia();
        for (int i = 0; i < rows * columns; i++) {
            text_box_array.at(i)->text_changed += [&] {
                if (autoSaveParams && loaded) saveSettings();
            };
        }
        if (autoSaveParams && loaded) saveSettings();
    };

    removemedia.parent(mediabox);
    removemedia.auto_size(true);
    removemedia.font({ this->font(), 20 });
    removemedia.text("-");
    removemedia.click += [&] {
        doRemoveMedia();
        for (int i = 0; i < rows * columns; i++) {
            text_box_array.at(i)->text_changed += [&] {
                if (autoSaveParams && loaded) saveSettings();
            };
        }
        if (autoSaveParams && loaded) saveSettings();
    };

    //finalize
    finalize.parent(tab_control);
    finalize.text(FinalizeText);

    //title ID stuff
    titleIDtxt.parent(finalize);
    titleIDtxt.auto_size(true);
    titleIDtxt.font({ this->font(), 15 });
    titleIDtxt.text(xtd::ustring::format("{} 000400000", TitleIDText));

    ZeroZero.parent(finalize);
    ZeroZero.auto_size(true);
    ZeroZero.font({ this->font(), 15 });
    ZeroZero.text("00");

    titleIDbox.parent(finalize);
    titleIDbox.font(this->font());
    titleIDbox.width(ZeroZero.width() * 3);
    titleIDbox.cursor(xtd::forms::cursors::ibeam());
    titleIDbox.text(xtd::ustring::format("{:X5}", RandomTID()));
    titleIDbox.text_changed += [&] {
        size_t index = titleIDbox.selection_start();
        if (index > 5) index = 5;
        xtd::ustring temp = titleIDbox.text();
        if (temp.size() > 5)
            temp = titleIDbox.text().substr(0, 5);
        for (auto& c : temp) {
            if (tolowerstr(std::string(1, c)).find_first_of("0123456789abcdef") != 0) {
                c = '0';
            }
        }

        uint32_t unique_id;
        stoul_s(unique_id, temp, true);
        if (!TIDisValid(unique_id)) TitleIDError.show();
        else TitleIDError.hide();

        while (unique_id <= 0xFFFFF && unique_id * 0x10 <= 0xFFFFF && unique_id != 0) unique_id = unique_id * 0x10;

        titleIDbox.text(xtd::ustring::format("{:X5}", unique_id));
        titleIDbox.select(index, 0);//put beam at the right place
    };

    TitleIDError.parent(finalize);
    TitleIDError.auto_size(true);
    TitleIDError.font({ this->font(), xtd::drawing::font_style::bold });
    TitleIDError.text(xtd::ustring::format("{} {}", ErrorText, BadValue));
    TitleIDError.hide();

    randomizeTitleID.parent(finalize);
    randomizeTitleID.size({ 38, 38 });
    randomizeTitleID.image(pixels_to_image(randomize_array, 30, 27, 2));
    randomizeTitleID.click += [&] {
        titleIDbox.text(xtd::ustring::format("{:X5}", RandomTID()));
        titleIDbox.select(titleIDbox.text().size(), 0);//put beam at the end
    };

    //application name stuff
    Applicationtxt.parent(finalize);
    Applicationtxt.auto_size(true);
    Applicationtxt.font({ this->font(), 15 });
    Applicationtxt.text(AppNameText);

    ApplicationName.parent(finalize);
    ApplicationName.font(this->font());
    ApplicationName.width(ZeroZero.width() * 5);
    ApplicationName.cursor(xtd::forms::cursors::ibeam());
    ApplicationName.text("video");
    ApplicationName.text_changed += [&] {
        size_t index = ApplicationName.selection_start();
        xtd::ustring temp = ApplicationName.text();
        for (auto& c : temp) {
            if (tolowerstr(std::string(1, c)).find_first_of("abcdefghijklmnopqrstuvwxyz") != 0) {//maybe numbers work too but i wont allow it because numbers in process name is ugly ;-;
                c = ' ';
            }
        }

        ApplicationName.text(temp);
        ApplicationName.select(index, 0);


        if (chrcount(ApplicationName.text()) < 1) {
            ApplicationError.text(xtd::ustring::format("{} {}", ErrorText, BadValue));
            ApplicationError.show();
        }
        else if (chrcount(ApplicationName.text()) > 8) {
            ApplicationError.text(xtd::ustring::format("{} {} ({}/{})", ErrorText, TextTooLongError, chrcount(ApplicationName.text()), 8));
            ApplicationError.show();
        }
        else ApplicationError.hide();
        if (minorBarTxt.location().y() > randomizeProductCode.location().y() + randomizeProductCode.height()) ApplicationError.location({ Applicationtxt.location().x() + ((Applicationtxt.width() + ApplicationName.width()) - ApplicationError.width()) / 2, Applicationtxt.location().y() + Applicationtxt.height() });
        else ApplicationError.location({ Applicationtxt.location().x() + ((Applicationtxt.width() + ApplicationName.width()) - ApplicationError.width()) / 2, Applicationtxt.location().y() + Applicationtxt.height() });
    };

    ApplicationError.parent(finalize);
    ApplicationError.auto_size(true);
    ApplicationError.font({ this->font(), xtd::drawing::font_style::bold });
    ApplicationError.text(xtd::ustring::format("{} {} ({}/{})", ErrorText, TextTooLongError, chrcount(ApplicationName.text()), 8));
    ApplicationError.hide();

    //product code stuff
    ProductCodetxt.parent(finalize);
    ProductCodetxt.auto_size(true);
    ProductCodetxt.font({ this->font(), 15 });
    ProductCodetxt.text(xtd::ustring::format("{} CTR-H-", ProductCodetext));

    ProductCode.parent(finalize);
    ProductCode.font(this->font());
    ProductCode.width(ZeroZero.width() * 2);
    ProductCode.cursor(xtd::forms::cursors::ibeam());
    ProductCode.text("VDIJ");
    ProductCode.text_changed += [&] {
        size_t index = ProductCode.selection_start();
        if (index > 4) index = 4;
        xtd::ustring temp = ProductCode.text();
        if (temp.size() > 4)
            temp = ProductCode.text().substr(0, 4);
        for (auto& c : temp) {
            if (tolowerstr(std::string(1, c)).find_first_of("abcdefghijklmnopqrstuvwxyz0123456789") != 0) {
                c = 'A';
            }
        }

        if (chrcount(ProductCode.text()) != 4) ProductCodeError.show();
        else ProductCodeError.hide();

        ProductCode.text(toupperstr(temp));
        ProductCode.select(index, 0);
    };

    ProductCodeError.parent(finalize);
    ProductCodeError.auto_size(true);
    ProductCodeError.font({ this->font(), xtd::drawing::font_style::bold });
    ProductCodeError.text(xtd::ustring::format("{} {}", ErrorText, BadValue));
    ProductCodeError.hide();

    randomizeProductCode.parent(finalize);
    randomizeProductCode.size({ 38, 38 });
    randomizeProductCode.image(pixels_to_image(randomize_array, 30, 27, 2));
    randomizeProductCode.click += [&] {
        std::string set = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        std::string outText = "";
        static std::mt19937 rng;
        for (int i = 0; i < 4; i++) {
            rng.seed(static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
            std::uniform_int_distribution<uint32_t> uniform(0, (set.size() - 1) & 0xFFFFFFFF);
            outText += set[uniform(rng)];
        }
        ProductCode.text(outText);
        ProductCode.select(ProductCode.text().size(), 0);
    };

    //minor bar
    minorBarTxt.parent(finalize);
    minorBarTxt.auto_size(true);
    minorBarTxt.font({ this->font(), 15 });
    minorBarTxt.text(FormText);
    minorBarTxt.hide();

    minorBar.parent(finalize);
    minorBar.height(45);
    minorBar.maximum(69);
    minorBar.minimum(0);
    minorBar.style(xtd::forms::progress_bar_style::continuous);
    minorBar.double_buffered(true);

    //major bar
    majorBarTxt.parent(finalize);
    majorBarTxt.auto_size(true);
    majorBarTxt.font({ this->font(), 15 });
    majorBarTxt.text(FormText);
    majorBarTxt.hide();

    majorBar.parent(finalize);
    majorBar.size({ finalize.width() - 23, 50 });
    majorBar.double_buffered(true);

    buildButt.parent(finalize);
    buildButt.auto_size(true);
    buildButt.font({ this->font(), 15 });
    buildButt.text(BuildCIAText);
    buildButt.size({ buildButt.width() * 2, buildButt.height() * 2 });

    cancelBuildButt.parent(finalize);
    cancelBuildButt.auto_size(true);
    cancelBuildButt.font({ this->font(), 15 });
    cancelBuildButt.text(Cancel);
    cancelBuildButt.enabled(false);
    cancelBuildButt.click += [&] {
        builder.cancel_async();
    };

    {
        static xtd::ustring outfile = "";

        buildButt.click += [&] {
            outfile = save_file(CiaFiles, xtd::ustring::format("{} [000400000{}00]", removeInvalids(longname.text()), titleIDbox.text()));
            {
                xtd::forms::dialog_result res = xtd::forms::dialog_result::yes;
                if (std::filesystem::exists(std::filesystem::path((const char8_t*)&*outfile.c_str())))
                    res = xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}\n{}", outfile.substr(outfile.find_last_of("/\\") + 1), AlreadyExists, ReplaceIt), ConfirmSave, xtd::forms::message_box_buttons::yes_no, xtd::forms::message_box_icon::question);
                if (res == xtd::forms::dialog_result::no || res == xtd::forms::dialog_result::none)
                    return;
                if (outfile.empty())
                    return;
            }
            minorBar.style(xtd::forms::progress_bar_style::marquee);
            ableObjects(false);
            builder.run_worker_async();
        };

        builder.worker_supports_cancellation(true);
        builder.worker_reports_progress(true);
        builder.do_work += [&] {
            cancelBuildButt.enabled(true);
            //minorBar.maximum(69);
            //minorBar.minimum(0);

            finalize.cursor(xtd::forms::cursors::app_starting());

            //extract base
            std::error_code error;
            std::filesystem::remove_all(std::filesystem::path((const char8_t*)&*tempPath.c_str()), error);
            if (error) {
                xtd::forms::message_box::show(*this, xtd::ustring::format("{}", error.message()), xtd::ustring::format("{}", ErrorText), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                xtd::forms::message_box::show(*this, xtd::ustring::format("{} \"{}\"", FailedToCreateFile, outfile), ErrorText, xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                return;
            }
            Generate_Files(tempPath.c_str(), mode.selected_index());

            majorBarTxt.text(xtd::ustring::format("{} romfs", CreatingFile));
            majorBarTxt.location({ (finalize.width() - majorBarTxt.width()) / 2, majorBarTxt.location().y() });
            majorBarTxt.show();

            //make movie_title.csv (player title)
            {
                minorBarTxt.text(xtd::ustring::format("{} romfs/movie_title.csv", CreatingFile));
                minorBarTxt.location({ (finalize.width() - minorBarTxt.width()) / 2, minorBarTxt.location().y() });
                minorBarTxt.show();

                std::filesystem::create_directories(std::filesystem::path((const char8_t*)&*xtd::ustring::format("{}/{}/temp/romfs/movie", ProgramDir, resourcesPath).c_str()));
                std::ofstream movie_title(xtd::ustring::format("{}/{}/temp/romfs/movie/movie_title.csv", ProgramDir, resourcesPath).c_str(), std::ios_base::out | std::ios_base::binary);

                movie_title << "\xFF\xFE" + UTF8toUTF16("#JP,#EN,#FR,#GE,#IT,#SP,#CH,#KO,#DU,#PO,#RU,#TW\x0D\x0A");
                for (int i = 0; i < (mode.selected_index() ? rows : 1); i++) {
                    std::string outstr = text_box_array.at(i * columns)->text();

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
                if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*xtd::ustring::format("{}/{}/temp/romfs/movie/movie_title.csv", ProgramDir, resourcesPath).c_str()))) {
                    xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}/{}/temp/romfs/movie/movie_title.csv", FailedToCreateFile, ProgramDir, resourcesPath), xtd::ustring::format("{} {}", ErrorText, FailedToFindPath), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                    //builder.cancel_async();
                    return;
                }
            }
            if (builder.cancellation_pending()) return;
            //make settingsTL.csv (menu title and stuff)
            {
                minorBarTxt.text(xtd::ustring::format("{} romfs/settings/settingsTL.csv", CreatingFile));
                minorBarTxt.location({ (finalize.width() - minorBarTxt.width()) / 2, minorBarTxt.location().y() });

                std::filesystem::create_directories(std::filesystem::path((const char8_t*)&*xtd::ustring::format("{}/romfs/settings", tempPath).c_str()));
                std::ofstream settingsTL(xtd::ustring::format("{}/romfs/settings/settingsTL.csv", tempPath).c_str(), std::ios_base::out | std::ios_base::binary);

                std::string outlongname = longname.text();
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

                std::string outpublisher = publisher.text();
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
                        + titleIDbox.text().c_str() + "\x0D\x0A"//make it the save as title ID because yes
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
                        + (FFrewind.checked() ? "true" : "false") + "\x0D\x0A"
                        "\x0D\x0A"
                        "# 優しさ演出のあり、なし\x0D\x0A"//gentleness
                        + (FadeOpt.checked() ? "true" : "false") + "\x0D\x0A");

                if (mode.selected_index()) {
                    settingsTL << UTF8toUTF16("\x0D\x0A"
                        "# 動画の数\x0D\x0A"//amount of videos
                        + std::to_string(rows) + "\x0D\x0A"
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
                if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*xtd::ustring::format("{}/romfs/settings/settingsTL.csv", tempPath).c_str()))) {
                    xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}/romfs/settings/settingsTL.csv", FailedToCreateFile, tempPath), xtd::ustring::format("{} {}", ErrorText, FailedToFindPath), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                    //builder.cancel_async();
                    return;
                }
            }
            if (builder.cancellation_pending()) return;
            //make copyright stuff (multi vid only)
            if (mode.selected_index()) {
                minorBarTxt.text(xtd::ustring::format("{} romfs/settings/information_buttons.csv", CreatingFile));
                minorBarTxt.location().x((finalize.width() - minorBarTxt.width()) / 2);

                std::filesystem::create_directories(xtd::ustring::format("{}/romfs/settings", tempPath).c_str());//just in case Hehehhhah
                std::ofstream information_buttons(xtd::ustring::format("{}/romfs/settings/information_buttons.csv", tempPath).c_str(), std::ios_base::out | std::ios_base::binary);
                information_buttons << (copycheck.checked() ? ("\xFF\xFE" + UTF8toUTF16("Copyright")) : "\xFF\xFE");
                information_buttons.close();
                if (!std::filesystem::exists(xtd::ustring::format("{}/romfs/settings/information_buttons.csv", tempPath).c_str())) {
                    xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}/romfs/settings/information_buttons.csv", FailedToCreateFile, tempPath), xtd::ustring::format("{} {}", ErrorText, FailedToFindPath), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                    //builder.cancel_async();
                    return;
                }

                if (copycheck.checked()) {
                    minorBarTxt.text(xtd::ustring::format("{} romfs/settings/copyright.txt", CreatingFile));
                    minorBarTxt.location().x((finalize.width() - minorBarTxt.width()) / 2);

                    std::ofstream copyrighttxt(xtd::ustring::format("{}/romfs/settings/copyright.txt", tempPath).c_str(), std::ios_base::out | std::ios_base::binary);
                    copyrighttxt << "\xFF\xFE" + UTF8toUTF16(copybox.text());
                    copyrighttxt.close();
                    if (!std::filesystem::exists(xtd::ustring::format("{}/romfs/settings/copyright.txt", tempPath).c_str())) {
                        xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}/romfs/settings/copyright.txt", FailedToCreateFile, tempPath), xtd::ustring::format("{} {}", ErrorText, FailedToFindPath), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                        //builder.cancel_async();
                        return;
                    }
                }
            }
            if (builder.cancellation_pending()) return;
            builder.report_progress(15);
            //copy moflex
            {
                uint8_t Checker[4];
                for (int i = 0; i < (mode.selected_index() ? rows : 1); i++) {
                    if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*text_box_array.at(i * columns + 1)->text().c_str()))) {
                        xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}, {} 2\n{} \"{}\"", row, i + 1, column, FailedToFindPath, text_box_array.at(i * columns + 1)->text()), xtd::ustring::format("{} {}", ErrorText, FailedToFindPath), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                        //builder.cancel_async();
                        return;
                    }
                    std::string extension = text_box_array.at(i * columns + 1)->text();
                    if (extension.find_last_of(".") != std::string::npos)
                        extension.erase(extension.begin(), extension.begin() + extension.find_last_of("."));
                    std::ifstream inmoflex(std::filesystem::path((const char8_t*)&*text_box_array.at(i * columns + 1)->text().c_str()), std::ios_base::in | std::ios::binary);
                    for (int j = 0; j < 4; j++) {
                        inmoflex >> Checker[j];//https://stackoverflow.com/a/2974735
                        if (extension != ".moflex" || Checker[j] != moflexMagic[j]) {
                            xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}, {} 2\n\"{}\" {}", row, i + 1, column, text_box_array.at(i * columns + 1)->text(), MoflexError), xtd::ustring::format("{} {}", ErrorText, BadValue), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                            //builder.cancel_async();
                            return;
                        }
                    }
                    std::filesystem::create_directories(std::filesystem::path((const char8_t*)&*xtd::ustring::format("{}/romfs/movie", tempPath).c_str()));
                    if (mode.selected_index()) {

                        if (builder.cancellation_pending()) return;
                        minorBarTxt.text(xtd::ustring::format("{} {}/{}", CopyingMoflex, i + 1, rows));
                        minorBarTxt.location({ (finalize.width() - minorBarTxt.width()) / 2, minorBarTxt.location().y() });

                        std::error_code error;
                        error = copyfile(text_box_array.at(i * columns + 1)->text().c_str(), xtd::ustring::format("{}/romfs/movie/movie_{}.moflex", tempPath, i).c_str());
                        if (error) {
                            xtd::forms::message_box::show(*this, xtd::ustring::format("{}", error.message()), xtd::ustring::format("{}", ErrorText), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                            xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}/romfs/movie/movie_{}.moflex", FailedToCreateFile, tempPath, i), xtd::ustring::format("{} {}", ErrorText, FailedToFindPath), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                            return;
                        }
                        if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*xtd::ustring::format("{}/romfs/movie/movie_{}.moflex", tempPath, i).c_str()))) {//this probably only happens if there's no disk space
                            xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}/romfs/movie/movie_{}.moflex", FailedToCreateFile, tempPath, i), xtd::ustring::format("{} {}", ErrorText, FailedToFindPath), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                            //builder.cancel_async();
                            return;
                        }
                    }
                    else {
                        if (builder.cancellation_pending()) return;
                        minorBarTxt.text(xtd::ustring::format("{} 1/1", CopyingMoflex));
                        minorBarTxt.location({ (finalize.width() - minorBarTxt.width()) / 2, minorBarTxt.location().y() });
                        std::error_code error;
                        error = copyfile(text_box_array.at(i * columns + 1)->text().c_str(), xtd::ustring::format("{}/romfs/movie/movie.moflex", tempPath).c_str());
                        if (error) {
                            xtd::forms::message_box::show(*this, xtd::ustring::format("{}", error.message()), xtd::ustring::format("{}", ErrorText), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                            xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}/romfs/movie/movie.moflex", FailedToCreateFile, tempPath), xtd::ustring::format("{} {}", ErrorText, FailedToFindPath), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                            return;
                        }
                        if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*xtd::ustring::format("{}/romfs/movie/movie.moflex", tempPath).c_str()))) {//this probably only happens if there's no disk space
                            xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}/romfs/movie/movie.moflex", FailedToCreateFile, tempPath), xtd::ustring::format("{} {}", ErrorText, FailedToFindPath), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                            //builder.cancel_async();
                            return;
                        }
                    }
                }
            }
            builder.report_progress(75);
            if (builder.cancellation_pending()) return;
            //convert to bimg (multi vid only)
            if (mode.selected_index()) {
                for (int i = 0; i < rows; i++) {
                    uint8_t bimg[256 * 128 * sizeof(nnc_u16) + 0x20];
                    /*if (!std::filesystem::exists(text_box_array.at(i * columns + 2)->text().c_str())) {//comment out because we want blank image if it failed
                        xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}, {} 3\n{} \"{}\"", row, i + 1, column, FailedToFindPath, text_box_array.at(i * columns + 2)->text()), xtd::ustring::format("{} {}", ErrorText, FailedToFindPath), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                        //builder.cancel_async();
                        return;
                    }*/

                    minorBarTxt.text(xtd::ustring::format("{} romfs/movie/movie_{}.bimg", CreatingFile, i));
                    minorBarTxt.location({ (finalize.width() - minorBarTxt.width()) / 2, minorBarTxt.location().y() });

                    uint8_t ret = convertToBimg(text_box_array.at(i * columns + 2)->text(), bimg, true);
                    if (ret > 0) {
                        xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}/romfs/movie/movie_{}.bimg\n({})", FailedToCreateFile, tempPath, i, ret), xtd::ustring::format("{} {}", ErrorText, FailedToFindPath), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                        //builder.cancel_async();
                        return;
                    }
                    std::filesystem::create_directories(std::filesystem::path((const char8_t*)&*xtd::ustring::format("{}/romfs/movie", tempPath).c_str()));
                    std::ofstream bimgfile(xtd::ustring::format("{}/romfs/movie/movie_{}.bimg", tempPath, i).c_str(), std::ios_base::out | std::ios_base::binary);
                    bimgfile.write(reinterpret_cast<const char*>(bimg), sizeof(bimg));
                    bimgfile.close();
                }
                //make movie_bnrname.csv
                std::filesystem::create_directories(std::filesystem::path((const char8_t*)&*xtd::ustring::format("{}/romfs/settings", tempPath).c_str()));
                std::ofstream movie_bnrname(xtd::ustring::format("{}/romfs/settings/movie_bnrname.csv", tempPath).c_str(), std::ios_base::out | std::ios_base::binary);
                movie_bnrname << "\xFF\xFE" + UTF8toUTF16(std::to_string(rows) + "\x0D\x0A");
                for (int i = 0; i < rows; i++) {
                    movie_bnrname << UTF8toUTF16("movie_" + std::to_string(i) + ".bimg\x0D\x0A");
                }
                movie_bnrname.close();
                if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*xtd::ustring::format("{}/romfs/settings/movie_bnrname.csv", tempPath).c_str()))) {
                    xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}/romfs/settings/movie_bnrname.csv", FailedToCreateFile, tempPath), xtd::ustring::format("{} {}", ErrorText, FailedToFindPath), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                    //builder.cancel_async();
                    return;
                }
            }
            if (builder.cancellation_pending()) return;
            builder.report_progress(100);
            //do exefs (icon and banner)
            {
                builder.report_progress(0);
                majorBarTxt.text(xtd::ustring::format("{} exefs", CreatingFile));
                majorBarTxt.location({ (finalize.width() - majorBarTxt.width()) / 2, majorBarTxt.location().y() });

                minorBarTxt.text(xtd::ustring::format("{} exefs/icon", CreatingFile));
                minorBarTxt.location({ (finalize.width() - minorBarTxt.width()) / 2, minorBarTxt.location().y() });

                uint8_t ret = 0;
                ret = convertToIcon(iconbox.text(), xtd::ustring::format("{}/exefs/icon", tempPath).c_str(), UTF8toUTF16(shortname.text()), UTF8toUTF16(longname.text()), UTF8toUTF16(publisher.text()), borderMode);
                if (ret > 0) {
                    xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}/exefs/icon", FailedToCreateFile, tempPath), xtd::ustring::format("{} {}", ErrorText, BadValue), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                    //builder.cancel_async();
                    return;
                }
                if (mode.selected_index()) {//multi vid needs an icon here so that it can make ext data or something (the game crashes if it isnt here)
                    majorBarTxt.text(xtd::ustring::format("{} romfs", CreatingFile));
                    majorBarTxt.location({ (finalize.width() - majorBarTxt.width()) / 2, majorBarTxt.location().y() });

                    minorBarTxt.text(xtd::ustring::format("{} romfs/icon.icn", CreatingFile));
                    minorBarTxt.location({ (finalize.width() - minorBarTxt.width()) / 2, minorBarTxt.location().y() });

                    std::error_code error;
                    error = copyfile(xtd::ustring::format("{}/exefs/icon", tempPath).c_str(), xtd::ustring::format("{}/romfs/icon.icn", tempPath).c_str());
                    if (error) {
                        xtd::forms::message_box::show(*this, xtd::ustring::format("{}", error.message()), xtd::ustring::format("{}", ErrorText), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                        xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}/romfs/icon.icn", FailedToCreateFile, tempPath), xtd::ustring::format("{} {}", ErrorText, FailedToFindPath), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                        return;
                    }
                    if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*xtd::ustring::format("{}/{}/temp/romfs/icon.icn", ProgramDir, resourcesPath).c_str()))) {
                        xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}/romfs/icon.icn", FailedToCreateFile, tempPath), xtd::ustring::format("{} {}", ErrorText, FailedToFindPath), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                        //builder.cancel_async();
                        return;
                    }
                    majorBarTxt.text(xtd::ustring::format("{} exefs", CreatingFile));
                    majorBarTxt.location({ (finalize.width() - majorBarTxt.width()) / 2, majorBarTxt.location().y() });
                }
                builder.report_progress(50);
                //make banner
                minorBarTxt.text(xtd::ustring::format("{} exefs/banner", CreatingFile));
                majorBarTxt.location({ (finalize.width() - majorBarTxt.width()) / 2, majorBarTxt.location().y() });

                uint8_t Checker[4];
                bool banner = false;
                std::ifstream inbanner(std::filesystem::path((const char8_t*)&*bannerbox.text().c_str()), std::ios::binary);
                if (std::filesystem::exists(std::filesystem::path((const char8_t*)&*bannerbox.text().c_str()))) {
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
                }
                if (banner) {
                    std::error_code error;
                    error = copyfile(bannerbox.text(), xtd::ustring::format("{}/exefs/banner", tempPath).c_str());
                    if (error) {
                        xtd::forms::message_box::show(*this, xtd::ustring::format("{}", error.message()), xtd::ustring::format("{}", ErrorText), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                        xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}/exefs/banner", FailedToCreateFile, tempPath), xtd::ustring::format("{} {}", ErrorText, FailedToFindPath), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                        return;
                    }
                }
                else if (!banner) {
                    uint8_t buffer[256 * 128 * sizeof(nnc_u16)];
                    ret = convertToBimg(bannerbox.text(), buffer, false);
                    if (ret > 0) {
                        xtd::forms::message_box::show(*this, xtd::ustring::format("{} \"{}\"\n({})", FailedToConvertImage, bannerbox.text(), ret), xtd::ustring::format("{} {}", ErrorText, BadValue), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                        //builder.cancel_async();
                        return;
                    }

                    //create bcmdl
                    uint8_t* bcmdl;
                    bcmdl = (uint8_t*)malloc(sizeof(bannerheader) + sizeof(buffer) + sizeof(bannerfooter));
                    memcpy(bcmdl, bannerheader, sizeof(bannerheader));
                    memcpy(bcmdl + sizeof(bannerheader), buffer, sizeof(buffer));
                    memcpy(bcmdl + sizeof(bannerheader) + sizeof(buffer), bannerfooter, sizeof(bannerfooter));

                    //build banner (stolen from bannertool)
                    CBMD cbmd;
                    memset(&cbmd, 0, sizeof(cbmd));

                    cbmd.cgfxSizes[0] = sizeof(bannerheader) + sizeof(buffer) + sizeof(bannerfooter);
                    cbmd.cgfxs[0] = bcmdl;

                    cbmd.cwavSize = sizeof(BCWAV_array);
                    cbmd.cwav = (void*)BCWAV_array;

                    uint32_t bnrSize = 0;

                    void* bnr = cbmd_build_data(&bnrSize, cbmd);

                    std::ofstream bnrfile(xtd::ustring::format("{}/exefs/banner", tempPath).c_str(), std::ios_base::out | std::ios_base::binary);
                    bnrfile.write(reinterpret_cast<const char*>(bnr), bnrSize);
                    bnrfile.close();
                }
                if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*xtd::ustring::format("{}/exefs/banner", tempPath).c_str()))) {
                    xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}/exefs/banner", FailedToCreateFile, tempPath), xtd::ustring::format("{} {}", ErrorText, FailedToFindPath), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                    //builder.cancel_async();
                    return;
                }
            }
            builder.report_progress(100);
            if (builder.cancellation_pending()) return;
            //modify exheader
            {
                if (ApplicationName.text().empty()) {
                    xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}", AppNameText, BadValue), xtd::ustring::format("{} {}", ErrorText, BadValue), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                    return;
                }
                uint32_t unique_id = 0;
                stoul_s(unique_id, titleIDbox.text().c_str(), true);
                if (!TIDisValid(unique_id)) {
                    xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}", TitleIDText, BadValue), xtd::ustring::format("{} {}", ErrorText, BadValue), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                    return;
                }

                std::fstream exheader(xtd::ustring::format("{}/exheader.bin", tempPath).c_str(), std::ios::in | std::ios::out | std::ios::binary);
                for (int i = 0; i < 8; i++) {//write application name only 8 bytes because that's the limit. i had to do this loop because it was being weird with .write
                    exheader.seekp(i);
                    exheader << char(ApplicationName.text().c_str()[i]);
                }
                exheader.seekp(0x1C9);
                exheader.write(reinterpret_cast<const char*>(&unique_id), sizeof(uint32_t));
                exheader.seekp(0x201);
                exheader.write(reinterpret_cast<const char*>(&unique_id), sizeof(uint32_t));
                exheader.seekp(0x601);
                exheader.write(reinterpret_cast<const char*>(&unique_id), sizeof(uint32_t));
                exheader.close();
            }
            //CIA creation
            {
                majorBarTxt.text(xtd::ustring::format("{} {}", CreatingFile, outfile.substr(outfile.find_last_of("/\\") + 1)));
                majorBarTxt.location({ (finalize.width() - majorBarTxt.width()) / 2, majorBarTxt.location().y() });
                minorBarTxt.text(xtd::ustring::format("{} {}", CreatingFile, outfile.substr(outfile.find_last_of("/\\") + 1)));
                minorBarTxt.location({ (finalize.width() - minorBarTxt.width()) / 2, minorBarTxt.location().y() });
                builder.report_progress(0);
                std::ofstream baseCIA(std::filesystem::path((const char8_t*)&*xtd::ustring::format("{}/base.cia", tempPath).c_str()), std::ios_base::out | std::ios_base::binary);
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
                nnc_seeddb sdb;

                if (builder.cancellation_pending()) return;

                TRYB(nnc_file_open(&f, xtd::ustring::format("{}/base.cia", tempPath).c_str()), out1); /* open the input file */
                TRYB(nnc_wfile_open(&wf, outfile.c_str()), out2); /* open the output file */
                TRYB(nnc_read_cia_header(NNC_RSP(&f), &cia_hdr), out3); /* read the cia header */
                nnc_cia_open_certchain(&cia_hdr, NNC_RSP(&f), &certchain); /* open the certificate chain for later copying it into the new cia */
                nnc_cia_open_ticket(&cia_hdr, NNC_RSP(&f), &ticket); /* open the ticket for later copying it into the new cia */
                nnc_cia_open_tmd(&cia_hdr, NNC_RSP(&f), &tmd_strm); /* open the tmd which we will modify some things of and then write tot he new cia */
                TRYB(nnc_read_tmd_header(NNC_RSP(&tmd_strm), &tmd), out3); /* parse the ticket */
                TRYB(nnc_cia_make_reader(&cia_hdr, NNC_RSP(&f), nnc_get_default_keyset(), &reader), out3); /* create a content (= NCCH) reader */
                TRYB(nnc_cia_open_content(&reader, 0, &ncch0stream, NULL), out4); /* open the first content (NCCH0) */
                TRYB(nnc_read_ncch_header(NNC_RSP(&ncch0stream), &ncch_hdr), out5); /* parse the NCCH header */

                if (builder.cancellation_pending()) goto out5;

                TRYB(nnc_vfs_init(&romfs), out5); /* initialize a VFS */
                TRYB(nnc_vfs_link_directory(&romfs.root_directory, xtd::ustring::format("{}/romfs", tempPath).c_str(), nnc_vfs_identity_transform, NULL), out6); /* populate the VFS, another source of files could be a RomFS, see #nnc_romfs_to_vfs */
                TRYB(nnc_vfs_init(&exefs), out5); /* initialize a VFS */
                TRYB(nnc_vfs_link_directory(&exefs.root_directory, xtd::ustring::format("{}/exefs", tempPath).c_str(), nnc_vfs_identity_transform, NULL), out10);

                builder.report_progress(10);

                if ((res = nnc_scan_seeddb(&sdb)) != NNC_R_OK) /* scan for a seeddb for use with "new crypto" and set it as the default */
                    nnc_set_default_seeddb(&sdb);
                TRYB(nnc_fill_keypair(&kp, nnc_get_default_keyset(), nnc_get_default_seeddb(), &ncch_hdr), out7); /* generate the cryptographic keys for if the NCCH is encrypted */
                if (nnc_file_open(&exheader, xtd::ustring::format("{}/exheader.bin", tempPath).c_str()) != NNC_R_OK) {/* open exheader file */
                    xtd::forms::message_box::show(*this, xtd::ustring::format("failed to open '{}/exheader.bin'", tempPath), xtd::ustring::format("{}", ErrorText), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                    goto out10;
                }
                nnc_exheader exhdr;
                if (nnc_read_exheader(NNC_RSP(&exheader), &exhdr) != NNC_R_OK) {
                    xtd::forms::message_box::show(*this, xtd::ustring::format("failed to read exheader from '{}/exheader.bin'", tempPath), xtd::ustring::format("{}", ErrorText), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                    goto out10;
                }
                TRYB(nnc_ncch_section_logo(&ncch_hdr, NNC_RSP(&ncch0stream), &logo), out9); /* logo stream */

                builder.report_progress(25);

                /* setup the parameters for building, for more options see the documentation. */
                ncch0.type = NNC_CIA_NCCHBUILD_BUILD;
                ncch0.ncch = &ncch0b;
                nnc_condense_ncch(&ncch0b.chdr, &ncch_hdr);
                ncch0b.wflags = NNC_NCCH_WF_ROMFS_VFS | NNC_NCCH_WF_EXEFS_VFS | NNC_NCCH_WF_EXHEADER_STREAM;
                if (builder.cancellation_pending()) goto out10;
                ncch0b.romfs = &romfs;
                ncch0b.exefs = &exefs;
                ncch0b.exheader = &exheader;
                ncch0b.logo = NNC_RSP(&logo);
                ncch0b.plain = NULL;
                ncch0b.chdr.partition_id = exhdr.title_id;
                ncch0b.chdr.title_id = exhdr.title_id;

                if (chrcount(ProductCode.text()) != 4) {
                    xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}", ProductCodetext, BadValue), xtd::ustring::format("{} {}", ErrorText, BadValue), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                    goto out10;
                }
                {
                    xtd::ustring productCodeFull = "";
                    productCodeFull += "CTR-H-";
                    productCodeFull += ProductCode.text();
                    strcpy(ncch0b.chdr.product_code, productCodeFull.c_str());//modify product code
                }

                tmd.content_count = 1;
                tmd.title_id = exhdr.title_id;
                builder.report_progress(35);
                {
                    //change the title ID of the ticket
                    char* ticket_contents = (char*)malloc(NNC_RS_CALL0(ticket, size));
                    nnc_u32 out_size = 0;
                    if (NNC_RS_CALL(ticket, read, (nnc_u8*)ticket_contents, NNC_RS_CALL0(ticket, size), &out_size) != NNC_R_OK)
                        goto out11;
                    if (out_size != NNC_RS_CALL0(ticket, size))
                        goto out11;
                    nnc_memory modified_ticket;
                    {
                        uint64_t TIDbigend = 0;
                        encode_bigend_u64(exhdr.title_id, &TIDbigend);
                        *(nnc_u64*)&ticket_contents[nnc_sig_dsize((nnc_sigtype)ticket_contents[3]) + 0xDC] = TIDbigend;
                    }
                    nnc_mem_open(&modified_ticket, ticket_contents, NNC_RS_CALL0(ticket, size));

                    if (builder.cancellation_pending()) goto out11;
                    builder.report_progress(50);
                    /* and finally write the cia */
                    res = nnc_write_cia(
                        NNC_CIA_WF_CERTCHAIN_STREAM | NNC_CIA_WF_TICKET_STREAM | NNC_CIA_WF_TMD_BUILD,
                        &certchain, &modified_ticket, &tmd, 1, &ncch0, NNC_WSP(&wf)
                    );
                    builder.report_progress(100);
                    /* cleanup code, with lots of labels to jump to in case of failure depending on where it failed */
                out11: free(ticket_contents);
                }
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
                    xtd::forms::message_box::show(*this, xtd::ustring::format("failed: {}", nnc_strerror(res)), ErrorText, xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                    return;
                }
            }
            if (std::filesystem::exists(outfile.c_str())) {
                xtd::forms::message_box::show(*this, xtd::ustring::format("{} \"{}\"", CiaBuilt, outfile), xtd::ustring::format("NNC"), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::information);
                if (std::filesystem::file_size(outfile.c_str()) > 4294967295) {
                    xtd::forms::message_box::show(*this, CiaBig, xtd::ustring::format("{} {}", ErrorText, BadValue), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                }
            }
            else {
                xtd::forms::message_box::show(*this, xtd::ustring::format("{} \"{}\"", FailedToCreateFile, outfile), ErrorText, xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
            }
        };
    }

    builder.progress_changed += [&](object& sender, const xtd::forms::progress_changed_event_args& e) {
        majorBar.value(e.progress_percentage());
    };

    builder.run_worker_completed += [&] {
        std::error_code error;
        std::filesystem::remove_all(std::filesystem::path((const char8_t*)&*tempPath.c_str()), error);
        if (error) {
            xtd::forms::message_box::show(*this, xtd::ustring::format("{}", error.message()), xtd::ustring::format("{}", ErrorText), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
        }
        minorBarTxt.hide();
        majorBarTxt.hide();
        majorBar.value(0);
        minorBar.style(xtd::forms::progress_bar_style::continuous);
        cancelBuildButt.enabled(false);
        ableObjects(true);
        finalize.cursor(xtd::forms::cursors::default_cursor());
    };

    //settings
    settings.parent(tab_control);
    settings.text(OptionsText);

    //logo button
    Logo.parent(settings);
    Logo.cursor(xtd::forms::cursors::hand());
    Logo.image(pixels_to_image(logo90, 90, 90, 4));
    Logo.bounds({ 6, 6, 90, 90 });
    Logo.size_mode(xtd::forms::picture_box_size_mode::center_image);
    Logo.click += [&] {
        xtd::diagnostics::process::start("https://github.com/FoofooTheGuy/VidInjector9000/releases/latest");
    };

    maintitle.parent(settings);
    maintitle.auto_size(true);
    maintitle.font({ this->font(), 15 });
    maintitle.text(FormText);
    maintitle.location({ (settings.width() - maintitle.width()) / 2, 3 });

    subtitle.parent(settings);
    subtitle.auto_size(true);
    subtitle.font(this->font());
    subtitle.text(ByMeText);
    subtitle.location({ (settings.width() - subtitle.width()) / 2, maintitle.height() + maintitle.location().y() });

    savebutt.parent(settings);
    savebutt.auto_size(true);
    savebutt.font({ this->font(), 12 });
    savebutt.text(SaveButtText);
    //savebutt.size({ settings.width() - 10, 35 });
    //savebutt.location({ (settings.width() - savebutt.width()) / 2, (settings.height() / 3) - savebutt.height() });
    savebutt.click += [&] {
        xtd::ustring filepath = save_file(ParamFilesList, (parampath.empty() ? DefaultParamFile : parampath.substr(parampath.find_last_of("/\\") + 1)), (parampath.empty() ? xtd::environment::get_folder_path(xtd::environment::special_folder::desktop) : parampath));
        if (filepath.empty())
            return;
        parampath = filepath;
        xtd::forms::dialog_result res = xtd::forms::dialog_result::yes;
        if (std::filesystem::exists(std::filesystem::path((const char8_t*)&*parampath.c_str())))
            res = xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}\n{}", parampath.substr(parampath.find_last_of("/\\") + 1), AlreadyExists, ReplaceIt), ConfirmSave, xtd::forms::message_box_buttons::yes_no, xtd::forms::message_box_icon::question);
        if (res == xtd::forms::dialog_result::no || res == xtd::forms::dialog_result::none)
            return;
        saveParameters();
        saveSettings();
    };

    loadbutt.parent(settings);
    loadbutt.auto_size(true);
    loadbutt.font({ this->font(), 12 });
    loadbutt.text(LoadButtText);
    //loadbutt.size({ settings.width() - 10, savebutt.height() });
    //loadbutt.location({ (settings.width() - loadbutt.width()) / 2, savebutt.location().y() + savebutt.height() });//tether to save button
    loadbutt.click += [&] {
        xtd::ustring filepath = load_file(ParamFilesList, parampath);
        if (filepath.empty())
            return;
        parampath = filepath;
        loadParameters();
        loaded = true;//re-enable this after the previous function disabled it (it's okay because if you clicked this, we're loaded)
        SetIconPreview();
    };

    loadCIAbutt.parent(settings);
    loadCIAbutt.auto_size(true);
    loadCIAbutt.font({ this->font(), 12 });
    loadCIAbutt.text(LoadCIAButtText);

    {//wut
        static xtd::ustring filepath;

        loadCIAbutt.click += [&] {
            parampath = xtd::ustring::format("{}/{}/{}", ProgramDir, resourcesPath, DefaultParamFile);
            filepath = load_file(CiaFiles, filepath);
            if (filepath.empty()) {
                return;
            }
            ableObjects(false);
            extractor.run_worker_async();
        };

        static nnc_result res;
        static bool cancel;
        extractor.do_work += [&] {

            settings.cursor(xtd::forms::cursors::app_starting());

            loaded = false;
            xtd::ustring romfspath = xtd::ustring::format("{}/romfs", exportsPath);
            xtd::ustring exefspath = xtd::ustring::format("{}/exefs", exportsPath);

            std::error_code error;
            std::filesystem::remove_all(std::filesystem::path((const char8_t*)&*exportsPath.c_str()), error);
            if (error) {
                xtd::forms::message_box::show(*this, xtd::ustring::format("{}", error.message()), xtd::ustring::format("{}", ErrorText), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                return;
            }
            //extract important files from the romfs and exefs of cia
            {
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
                nnc_subview sv;
                nnc_keypair kp;
                nnc_seeddb sdb;
                sdb.size = 1;
                nnc_file f;

                res = nnc_file_open(&f, filepath.c_str());
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
                    xtd::forms::dialog_result dres = xtd::forms::message_box::show(*this, xtd::ustring::format("{}\n{}", SeedNotFound, SelectSeed), ErrorText, xtd::forms::message_box_buttons::ok_cancel, xtd::forms::message_box_icon::warning);
                    if (dres != xtd::forms::dialog_result::ok) {
                        goto end2;
                    }
                    std::ifstream seedfile;
                    xtd::ustring seedpath = load_file(xtd::ustring::format("{}{}", SeedFiles, AllFilesList), filepath);
                    if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*seedpath.c_str())) || std::filesystem::file_size(std::filesystem::path((const char8_t*)&*seedpath.c_str())) != NNC_SEED_SIZE) {
                        goto end2;
                    }
                    uint8_t* seedData = (nnc_u8*)malloc(NNC_SEED_SIZE);
                    seedfile.open(std::filesystem::path((const char8_t*)&*seedpath.c_str()), std::ios_base::in | std::ios_base::binary);
                    seedfile.read(reinterpret_cast<char*>(seedData), NNC_SEED_SIZE);
                    memcpy(ent.seed, seedData, NNC_SEED_SIZE);
                    free(seedData);
                    ent.title_id = ncch_hdr.partition_id;
                    sdb.entries = &ent;
                    res = nnc_fill_keypair(&kp, &kset, &sdb, &ncch_hdr);
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
                    nnc_u8* buf = (nnc_u8*)malloc(headers[i].size);
                    NNC_RS_CALL(sv, seek_abs, 0);
                    if (NNC_RS_CALL(sv, read, buf, headers[i].size, &read_size) != NNC_R_OK || read_size != headers[i].size) {
                        xtd::forms::message_box::show(*this, xtd::ustring::format("{} \"{}\"", FailedToReadFile, headers[i].name), xtd::ustring::format("{}", ErrorText), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                        continue;
                    }
                    std::filesystem::create_directories(std::filesystem::path((const char8_t*)&*exefspath.c_str()));
                    sprintf(pathbuf, "%s/%s", exefspath.c_str(), fname);
                    FILE* ef = fopen(pathbuf, "wb");
                    if (fwrite(buf, headers[i].size, 1, ef) != 1)
                        xtd::forms::message_box::show(*this, xtd::ustring::format("{} \"{}\"", FailedToCreateFile, pathbuf), xtd::ustring::format("{}", ErrorText), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
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
                    xtd::forms::message_box::show(*this, extractErr, ErrorText, xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                }
                if (res != NNC_R_OK)
                {
                    xtd::forms::message_box::show(*this, nnc_strerror(res), ErrorText, xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                    //return;
                }
            }

            //ignore this
            {}
            //information_buttons.csv
            {
                uint8_t ret;
                std::vector<std::string> trimmed;
                if (std::filesystem::exists(std::filesystem::path((const char8_t*)&*xtd::ustring::format("{}/settings/information_buttons.csv", romfspath).c_str()))) {
                    ret = UTF16fileToUTF8str(xtd::ustring::format("{}/settings/information_buttons.csv", romfspath), &trimmed);
                    if (ret > 0) {
                        xtd::forms::message_box::show(*this, xtd::ustring::format("{}: \"information_buttons.csv\"", FailedToReadFile), xtd::ustring::format("{} ({})", ErrorText, ret), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                    }
                    else {
                        mode.selected_index(1);
                        copycheck.checked((trimmed.size() > 0 && strcmp(trimmed.at(0).c_str(), "Copyright") == 0) ? true : false);
                    }
                }
                else {
                    mode.selected_index(0);
                    copycheck.checked(false);
                }
                trimmed.clear();
            }
            //copyright.txt
            {
                if (std::filesystem::exists(std::filesystem::path((const char8_t*)&*xtd::ustring::format("{}/settings/copyright.txt", romfspath).c_str()))) {
                    std::string output = "";
                    std::string outputUTF8 = "";
                    std::string Line;
                    std::ifstream input;
                    input.open(std::filesystem::path((const char8_t*)&*xtd::ustring::format("{}/settings/copyright.txt", romfspath).c_str()), std::ios_base::in | std::ios_base::binary);
                    if (!input) {
                        xtd::forms::message_box::show(*this, xtd::ustring::format("{}/settings/copyright.txt", romfspath), xtd::ustring::format("{} {}", ErrorText, FailedToReadFile), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                        return;
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
                        xtd::forms::message_box::show(*this, xtd::ustring::format("{}/settings/copyright.txt", romfspath), xtd::ustring::format("{} {}", ErrorText, FailedToReadFile), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                        return;
                    }
                    output.erase(output.begin(), output.begin() + 2);//delete byte order mask
                    outputUTF8 = UTF16toUTF8(output);
                    copybox.text(outputUTF8);
                    input.close();
                }
                else {
                    copybox.text("");
                }
            }
        };

        extractor.run_worker_completed += [&] {
            if (cancel) {
                ableObjects(true);
                settings.cursor(xtd::forms::cursors::default_cursor());
                loaded = true;
                return;
            }
            if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*exportsPath.c_str()))) {
                ableObjects(true);
                settings.cursor(xtd::forms::cursors::default_cursor());
                xtd::forms::message_box::show(*this, xtd::ustring::format("\"{}\"", filepath), xtd::ustring::format("{} {}", ErrorText, FailedToReadFile), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                loaded = true;
                return;
            }

            settings.cursor(xtd::forms::cursors::wait_cursor());

            xtd::ustring romfspath = xtd::ustring::format("{}/romfs", exportsPath);
            xtd::ustring exefspath = xtd::ustring::format("{}/exefs", exportsPath);
            uint8_t ret;
            uint32_t row = 1;
            bool good = true;
            std::vector<std::string> trimmed;
            /*
            parse the text files and load the other stuff
            this needs to be here because we doRemoveMedia or doAppendMedia. apparently it can't change that asynchronously
            */
            //set banner and icon
            {
                bannerbox.text("");
                iconbox.text("");
                bannerbox.text(xtd::ustring::format("{}/banner.bin", exefspath));
                iconbox.text(xtd::ustring::format("{}/icon.bin", exefspath));
            }
            //settingTL.csv
            {
                ret = UTF16fileToUTF8str(xtd::ustring::format("{}/settings/settingsTL.csv", romfspath), &trimmed);
                if (ret > 0) {
                    FFrewind.checked(false);
                    FadeOpt.checked(false);
                    xtd::forms::message_box::show(*this, xtd::ustring::format("{}: \"settingsTL.csv\"", FailedToReadFile), xtd::ustring::format("{} ({})", ErrorText, ret), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                    good = false;
                }
                else {
                    if (trimmed.size() > 29) {
                        row = 1;
                        FFrewind.checked((strstr(trimmed.at(29).c_str(), "true") != NULL) ? true : false);
                        FadeOpt.checked((strstr(trimmed.at(30).c_str(), "true") != NULL) ? true : false);
                    }
                    /*if (trimmed.size() > 31) {
                        if (!stoul_s(row, trimmed.at(31), false))
                            row = 1;
                    }*/
                }
                /*if (row < rows) {
                    uint32_t oldrows = rows;
                    for (uint32_t i = 0; i < oldrows - row; i++)
                        doRemoveMedia();
                }
                else if (row > rows) {
                    uint32_t oldrows = rows;
                    for (uint32_t i = 0; i < row - oldrows; i++)
                        doAppendMedia();
                }*/
                trimmed.clear();
            }
            //movie_bnrname.csv
            {
                if (std::filesystem::exists(std::filesystem::path((const char8_t*)&*xtd::ustring::format("{}/settings/movie_bnrname.csv", romfspath).c_str()))) {
                    ret = UTF16fileToUTF8str(xtd::ustring::format("{}/settings/movie_bnrname.csv", romfspath), &trimmed);
                    if (ret > 0) {
                        xtd::forms::message_box::show(*this, xtd::ustring::format("{}: \"movie_bnrname.csv\"", FailedToReadFile), xtd::ustring::format("{} ({})", ErrorText, ret), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                        good = false;
                    }
                    else {
                        mode.selected_index(1);
                        row = 0;
                        std::vector<xtd::ustring> output;
                        for (auto& LN : trimmed) {
                            std::string extension = LN;
                            if (extension.find_last_of(".") != std::string::npos)
                                extension.erase(extension.begin(), extension.begin() + extension.find_last_of("."));
                            while (!extension.empty() && extension.back() != 'g')//remove any extra stuff like a line break (make sure last char is 'g')
                                extension.pop_back();
                            if (strcmp(extension.c_str(), ".bimg") == 0) {
                                while (!LN.empty() && LN.back() != 'g')
                                    LN.pop_back();
                                output.push_back(xtd::ustring::format("{}/movie/{}", romfspath, LN.c_str()));
                                ++row;
                            }
                        }
                        while (rows < output.size()) {
                            doAppendMedia();
                        }
                        while (rows > output.size()) {
                            doRemoveMedia();
                        }
                        for (int i = 0; i < output.size(); i++) {
                            text_box_array.at(i * columns + 2)->text(output.at(i));
                        }
                    }
                    setMultiBannerPreview(bannerpreviewindex);
                }
                else {//this pretty much means it's a single video
                    while (rows > 1) {
                        doRemoveMedia();
                    }
                    mode.selected_index(0);
                    text_box_array.at(2)->text("");
                    setMultiBannerPreview(0);
                }
                trimmed.clear();
            }
            //movie_title.csv
            {
                ret = UTF16fileToUTF8str(xtd::ustring::format("{}/movie/movie_title.csv", romfspath), &trimmed);
                if (ret > 0) {
                    xtd::forms::message_box::show(*this, xtd::ustring::format("{}: \"movie_title.csv\"", FailedToReadFile), xtd::ustring::format("{} ({})", ErrorText, ret), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                    while (rows > 1) {
                        doRemoveMedia();
                    }
                    for (int i = 0; i < rows; i++) {
                        text_box_array.at(i * columns + 0)->text("");
                    }
                    good = false;
                }
                else {
                    row = 0;
                    std::vector<xtd::ustring> output;
                    for (auto& LN : trimmed) {
                        while (LN[0] == ',') {
                            LN.erase(0, 1);
                        }
                        if (LN.find(',') < LN.size()) {
                            LN.erase(LN.find(','), LN.size() - 1);
                        }
                        output.push_back(LN);
                        ++row;
                    }
                    while (rows < output.size()) {
                        doAppendMedia();
                    }
                    while (rows > output.size()) {
                        doRemoveMedia();
                    }
                    for (int i = 0; i < output.size(); i++) {
                        text_box_array.at(i * columns + 0)->text(output.at(i));
                    }
                }
                trimmed.clear();
            }
            //set moflex files
            {
                if (std::filesystem::exists(std::filesystem::path((const char8_t*)&*xtd::ustring::format("{}/movie/movie.moflex", romfspath).c_str()))) {//single video only has this
                    text_box_array.at(1)->text(xtd::ustring::format("{}/movie/movie.moflex", romfspath));
                    if (rows > 1)
                        for (uint8_t i = 0; i < rows - 1; i++)
                            doRemoveMedia();
                }
                else {
                    for (int i = 0; i < rows; i++)
                        text_box_array.at(i * columns + 1)->text(std::filesystem::exists(std::filesystem::path((const char8_t*)&*xtd::ustring::format("{}/movie/movie_{}.moflex", romfspath, i).c_str())) ? xtd::ustring::format("{}/movie/movie_{}.moflex", romfspath, i) : "");
                }
            }
            ableObjects(true);

            settings.cursor(xtd::forms::cursors::default_cursor());

            if (res != NNC_R_OK)
                good = false;
            SetIconPreview();
            saveSettings();
            xtd::forms::message_box::show(*this, xtd::ustring::format(good ? "{} {}{}" : "{} {}.\n{}", good ? SuccessfullyLoaded : FailedToLoad, filepath.substr(filepath.find_last_of("/\\") + 1), good ? "." : ValidStillLoaded), ParametersLoaded, xtd::forms::message_box_buttons::ok, good ? xtd::forms::message_box_icon::information : xtd::forms::message_box_icon::warning);
            loaded = true;
        };
    }

    AutoSave.parent(settings);
    AutoSave.auto_size(true);
    AutoSave.appearance(xtd::forms::appearance::button);
    AutoSave.font({ this->font(), 12 });
    AutoSave.text(AutoSaveText);
    //AutoSave.size({ settings.width() - 10, loadbutt.height() });
    //AutoSave.location({ (settings.width() - AutoSave.width()) / 2, loadbutt.location().y() + loadbutt.height() });//tether to load button
    AutoSave.text_align(xtd::forms::content_alignment::middle_center);
    AutoSave.checked(autoSaveParams);
    AutoSave.check_state_changed += [&] {
        autoSaveParams = AutoSave.checked();
        saveParametersNow = false;
        if (loaded) saveSettings();
        saveParametersNow = true;
    };

    AutoLoad.parent(settings);
    AutoLoad.auto_size(true);
    AutoLoad.appearance(xtd::forms::appearance::button);
    AutoLoad.font({ this->font(), 12 });
    AutoLoad.text(AutoLoadText);
    //AutoLoad.size({ settings.width() - 10, AutoSave.height() });
    //AutoLoad.location({ (settings.width() - AutoLoad.width()) / 2, AutoSave.location().y() + AutoSave.height() });//tether to load button
    AutoLoad.text_align(xtd::forms::content_alignment::middle_center);
    AutoLoad.checked(autoLoadParams);
    AutoLoad.check_state_changed += [&] {
        autoLoadParams = AutoLoad.checked();
        saveParametersNow = false;
        if (loaded) saveSettings();
        saveParametersNow = true;
    };

    DeleteTempButt.parent(settings);
    DeleteTempButt.auto_size(true);
    DeleteTempButt.appearance(xtd::forms::appearance::button);
    DeleteTempButt.font({ this->font(), 12 });
    DeleteTempButt.text(DeleteTempFiles);
    DeleteTempButt.text_align(xtd::forms::content_alignment::middle_center);
    DeleteTempButt.checked(deleteTemp);
    DeleteTempButt.check_state_changed += [&] {
        deleteTemp = DeleteTempButt.checked();
        saveParametersNow = false;
        if (loaded) saveSettings();
        saveParametersNow = true;
    };

    LightDark.parent(settings);
    LightDark.auto_size(true);
    LightDark.appearance(xtd::forms::appearance::button);
    LightDark.font({ this->font(), 12 });
    LightDark.text(LightDark.checked() ? DarkModeText : LightModeText);
    LightDark.size({ settings.width() - 10, loadbutt.height() });
    LightDark.text_align(xtd::forms::content_alignment::middle_center);
    LightDark.check_state_changed += [&] {
        LightDark.text(LightDark.checked() ? DarkModeText : LightModeText);
        saveSettings();
        xtd::forms::message_box::show(*this, xtd::ustring::format("{}{}\n{}", LightDark.checked() ? DarkModeText : LightModeText, WasEnabled, RestartProgram), LightDark.checked() ? DarkModeText : LightModeText, xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::information);
    };

    LanguageTitle.parent(settings);
    LanguageTitle.auto_size(true);
    LanguageTitle.font({ this->font(), 15 });
    LanguageTitle.text(LanguageText);

    LanguageChoiche.parent(settings);
    LanguageChoiche.auto_size(true);
    LanguageChoiche.font(this->font());
    //https://stackoverflow.com/a/612176
    for (const auto& entry : std::filesystem::directory_iterator(std::filesystem::path((const char8_t*)&*xtd::ustring::format("{}/{}/language", ProgramDir, resourcesPath).c_str()))) {
        if (std::filesystem::exists(entry)) {
            xtd::ustring entrystr = entry.path().string();//https://stackoverflow.com/q/45401822
            xtd::ustring outstr = "";
            removeQuotes(entrystr);
            if (std::filesystem::is_directory(entry)) {
                std::vector<xtd::ustring> filelines = fileRead(xtd::ustring::format("{}/Language.txt", entrystr));
                if (parseLines(outstr, filelines, inLangLanguage)) {
                    LanguageChoiche.items().push_back(outstr);
                    LanguageVec.push_back(entrystr.substr(entrystr.find_last_of("/\\") + 1));
                }
                else xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}\n{}.", FailedToFindVar, inLangLanguage, ValueNoChange), xtd::ustring::format("{} {}", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
            }
        }
        else xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}", FailedToFindPath, entry), xtd::ustring::format("{} {}", ErrorText, BadValue), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
    }
    for (int i = 0; i < LanguageVec.size(); i++) {
        if (LanguageVec.at(i) == Languagedir) {
            LanguageChoiche.selected_index(i);
            break;
        }
        else LanguageChoiche.selected_index(0);
    }
    LanguageChoiche.selected_index_changed += [&](object& sender, const xtd::event_args& e) {
        LanguageChoiche.selected_index(as<xtd::forms::choice&>(sender).selected_index());
        Languagedir = LanguageVec.at(LanguageChoiche.selected_index());
        saveSettings();
        xtd::forms::message_box::show(*this, xtd::ustring::format("{}{}\n{}", LanguageChanged, LanguageChoiche.items().at(LanguageChoiche.selected_index()), RestartProgram), xtd::ustring::format("{}", LanguageChoiche.items().at(LanguageChoiche.selected_index())), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::information);
    };

    restartbutt.parent(settings);
    restartbutt.auto_size(true);
    restartbutt.font({ this->font(), 12 });
    restartbutt.text(RestartText);
    restartbutt.size({ settings.width() - 10, 35 });
    restartbutt.location({ (settings.width() - restartbutt.width()) / 2, (settings.height() / 3) - savebutt.height() });
    restartbutt.click += xtd::forms::application::restart;

    //disable because idk how to save and load the data to the settings file
    /*fontchange.parent(settings);
    fontchange.auto_size(true);
    fontchange.font({ this->font(), 12 });
    fontchange.text("Change font");
    fontchange.size({ settings.width() - 10, loadbutt.height() });
    fontchange.click += [&] {
        //xtd::forms::font_dialog.color(tab_control.fore_color());
        //xtd::forms::font_dialog.font(tab_control.font());
        if (font_dialog.show_dialog(*this) == xtd::forms::dialog_result::ok) {
            tab_control.fore_color(font_dialog.color());
            tab_control.font(font_dialog.font());
            parameters.fore_color(font_dialog.color());
            parameters.font(font_dialog.font());
            finialize.fore_color(font_dialog.color());
            finialize.font(font_dialog.font());
            settings.fore_color(font_dialog.color());
            settings.font(font_dialog.font());
            this->fore_color(font_dialog.color());
            this->font(font_dialog.font());
            saveSettings();
        }
    };*/

    version.parent(settings);
    version.auto_size(true);
    version.font(this->font());
    version.text(programVersion);
    version.location({ (settings.width() - version.width()) / 2, settings.height() - version.height() });

    mode.selected_index_changed += [&](object& sender, const xtd::event_args& e) {
        mode.selected_index(as<xtd::forms::choice&>(sender).selected_index());
        copycheck.enabled(mode.selected_index());
        copybox.enabled(mode.selected_index() && copycheck.checked());
        menubannertxt.enabled(mode.selected_index());
        for (uint8_t i = 0; i < rows * columns; i++) {
            if(i > 1)
                text_box_array.at(i)->enabled(mode.selected_index());
        }
        menubannerpreview.cursor(mode.selected_index() ? xtd::forms::cursors::hand() : xtd::forms::cursors::no());
        if (mode.selected_index()) setMultiBannerPreview(bannerpreviewindex);
        else setDefaultBannerPreview(menubannerpreview, nullptr);
        bannerpreviewleft.enabled(mode.selected_index() && bannerpreviewindex != 0);
        bannerpreviewright.enabled(mode.selected_index() && bannerpreviewindex != rows - 1);
        multibannerbrowse.enabled(mode.selected_index());
        appendmedia.enabled(mode.selected_index() && rows < 27);
        //removemedia.enabled(rows > 1);
        if (mode.selected_index()) indextxt.show();
        else indextxt.hide();
        if (mode.selected_index()) rowtxt.show();
        else rowtxt.hide();
        if (autoSaveParams && loaded) saveSettings();
    };

    resize += [&] {
        //debugs.text(xtd::ustring::format("{}", mediabox.height()));
        //std::this_thread::sleep_for(std::chrono::milliseconds(500));//did this to decrease the flicker but it's annoying so turn it off
        tab_control.size({ client_size().width() - tab_control.location().x() * 2, client_size().height() - tab_control.location().y() * 2 });
        if (parameters.width() < iconerror.location().x() + iconerror.width() + iconpreview.width() + 480) {
            iconbrowse.location({ (iconerror.location().x() + iconerror.width()) - iconbrowse.width() - 1, iconbox.location().y() - (iconbrowse.height() / 2 - iconbox.height() / 2) });
            bannerbrowse.location({ iconbrowse.location().x() + iconpreview.width() + 3, bannerbox.location().y() - (bannerbrowse.height() / 2 - bannerbox.height() / 2) });
        }
        else {
            iconbrowse.location({ parameters.width() - (597 + iconpreview.width() + 3), iconbox.location().y() - (iconbrowse.height() / 2 - iconbox.height() / 2) });
            bannerbrowse.location({ parameters.width() - 597, bannerbox.location().y() - (bannerbrowse.height() / 2 - bannerbox.height() / 2) });
        }

        if (bannerpreview.width() > parameters.width() - (bannerbrowse.location().x() + bannerbrowse.width())) {
            bannerpreview.location({ bannerbrowse.location().x() + bannerbrowse.width() + 2, (shortname.location().y() - (bannerpreview.height() + bannerpreviewtxt.height())) / 2 });
        }
        else {
            bannerpreview.location({ (bannerbrowse.location().x() + bannerbrowse.width() + 2) + (((parameters.width() - (bannerbrowse.location().x() + bannerbrowse.width())) - bannerpreview.width()) / 2), (shortname.location().y() - (bannerpreview.height() + bannerpreviewtxt.height())) / 2 });
        }

        if (parameters.width() - (iconpreview.location().x() + iconpreview.width()) < 23) {
            shortname.width((iconpreview.location().x() + iconpreview.width()) - shortname.location().x());
            longname.width((iconpreview.location().x() + iconpreview.width()) - longname.location().x());
            publisher.width((iconpreview.location().x() + iconpreview.width()) - publisher.location().x());
        }
        else {
            shortname.width(parameters.width() - shortname.location().x() - 23);
            longname.width(parameters.width() - longname.location().x() - 23);
            publisher.width(parameters.width() - publisher.location().x() - 23);
        }

        if (publisher.location().x() + publisher.width() < copybox.location().x() + 320) {
            copybox.width(publishertxt.width() + publisher.width() + 5);
        }
        else {
            copybox.width(320);
        }

        bannerbox.width(bannerbrowse.location().x() - bannerbox.location().x() - 2);
        iconbox.width(iconbrowse.location().x() - iconbox.location().x() - 2);
        bannerpreviewtxt.location({ bannerpreview.location().x() + (bannerpreview.width() - bannerpreviewtxt.width()) / 2, bannerpreview.location().y() + bannerpreview.height() });
        customnotif.location({ bannerpreview.location().x() + (bannerpreview.width() - customnotif.width()) / 2,  bannerpreview.location().y() + (bannerpreview.height() - customnotif.height()) / 2 });
        iconpreview.location({ iconbrowse.location().x() + iconbrowse.width() + 2, iconbrowse.location().y() + 1 });
        FFrewind.location({ copybox.location().x() + copybox.width() + 15, copycheck.location().y() + ((((copycheck.height() + copybox.height() + 2) / 2) - FFrewind.height()) / 2) });
        FadeOpt.location({ FFrewind.location().x(), ((copycheck.height() + copybox.height() + 2) / 2) + copycheck.location().y() + ((((copycheck.height() + copybox.height() + 2) / 2) - FFrewind.height()) / 2) });
        {//put this stuff in it's own scope for less variable clutter (???)
            std::vector<int> wideVec = { moflextxt.width(), playertitletxt.width(), menubannertxt.width() + bannermulti.width() + 3 };
            int WideMediaText = getLargestNumber(wideVec);

            mediabox.size({ parameters.width() - copybox.location().x() * 2, parameters.height() - (copybox.location().y() + copybox.height() + playertitletxt.height()) - copybox.location().x() });
            if (mediabox.height() < (text_box_array.at(0)->height() * rows) + moflexbrowse.height() + removemedia.height() + 2 + rowtxt.height()) {//if scroll is there
                if (mediabox.width() < WideMediaText * columns + (text_box_array.at(0)->height() / 2)) {
                    for (int y = 0; y < rows; y++)
                        for (int x = 0; x < columns; x++) {
                            if (rows == 1) text_box_array.at(y * columns + x)->width(WideMediaText);
                            else text_box_array.at(y * columns + x)->width(WideMediaText - ((text_box_array.at(0)->height() / 2) / columns));
                        }
                }
                else {
                    for (int y = 0; y < rows; y++)
                        for (int x = 0; x < columns; x++) {
                            if (rows == 1) text_box_array.at(y * columns + x)->width(((mediabox.width() - (mediabox.location().x()) / columns) - 20) / columns);
                            else text_box_array.at(y * columns + x)->width((((mediabox.width() - text_box_array.at(0)->height() / 2) - (mediabox.location().x()) / columns) - 20) / columns);
                        }
                }
            }
            else {
                if (mediabox.width() < WideMediaText * columns + (text_box_array.at(0)->height() / 2)) {
                    for (int y = 0; y < rows; y++)
                        for (int x = 0; x < columns; x++) {
                            if (rows == 1) text_box_array.at(y * columns + x)->width(WideMediaText);
                            else text_box_array.at(y * columns + x)->width(WideMediaText - ((text_box_array.at(0)->height() / 2) / columns));
                        }
                }
                else {
                    for (int y = 0; y < rows; y++)
                        for (int x = 0; x < columns; x++) {
                            if (rows == 1) text_box_array.at(y * columns + x)->width(((mediabox.width() - (mediabox.location().x()) / columns)) / columns);
                            else text_box_array.at(y * columns + x)->width((((mediabox.width() - text_box_array.at(0)->height() / 2) - (mediabox.location().x()) / columns)) / columns);
                        }
                }
            }
        }
        for (uint8_t y = 0; y < rows; y++) {
            for (uint8_t x = 1; x < columns; x++)
                text_box_array.at(y * columns + x)->location({ text_box_array.at(0)->location().x() + (x * text_box_array.at(y * columns + x)->width()), text_box_array.at(0)->location().y() + (y * text_box_array.at(y * columns + x)->height()) });

            for (int i = 0; i < 2; i++) {
                butt_array.at(y * 2 + i)->location({ text_box_array.at(y * columns + (columns - 1))->location().x() + text_box_array.at(y * columns + (columns - 1))->width(), i ? (text_box_array.at(y * columns)->location().y() + text_box_array.at(y * columns)->height() - butt_array.at(y * 2 + i)->height()) : text_box_array.at(y * columns)->location().y() });
            }
        }
        //playertitletxt.location({ mediabox.location().x() + text_box_array.at(0)->location().x() + (text_box_array.at(0)->width() - playertitletxt.width()) / 2, copybox.location().y() + copybox.height() });
        //moflextxt.location({ mediabox.location().x() + text_box_array.at(1)->location().x() + (text_box_array.at(1)->width() - moflextxt.width()) / 2, copybox.location().y() + copybox.height() });
        //menubannertxt.location({ mediabox.location().x() + text_box_array.at(2)->location().x() + (text_box_array.at(2)->width() - (menubannertxt.width() + bannermulti.width() + 3)) / 2, copybox.location().y() + copybox.height() });
        //bannermulti.location({ menubannertxt.location().x() + menubannertxt.width() + 3, menubannertxt.location().y() + ((menubannertxt.height() - bannermulti.height()) / 2) });
        moflexbrowse.location({ text_box_array.at((rows - 1) * columns + 1)->location().x() + (text_box_array.at((rows - 1) * columns + 1)->width() - moflexbrowse.width()) / 2, text_box_array.at((rows - 1) * columns + 1)->location().y() + text_box_array.at((rows - 1) * columns + 1)->height() });
        multibannerbrowse.location({ text_box_array.at((rows - 1) * columns + 2)->location().x() + (text_box_array.at((rows - 1) * columns + 2)->width() - moflexbrowse.width()) / 2, text_box_array.at((rows - 1) * columns + 2)->location().y() + text_box_array.at((rows - 1) * columns + 2)->height() });
        if(rows == 1) removemedia.location({ (((text_box_array.at(0)->width() * columns) - (removemedia.width() + appendmedia.width() + 2)) / 2) + text_box_array.at(0)->location().x(), text_box_array.at((rows - 1) * columns + (columns - 1))->location().y() + text_box_array.at((rows - 1) * columns + (columns - 1))->height() + moflexbrowse.height() + 2});
        else removemedia.location({ (((text_box_array.at(0)->width() * columns + (text_box_array.at(0)->height() / 2)) - (removemedia.width() + appendmedia.width() + 2)) / 2) + text_box_array.at(0)->location().x(), text_box_array.at((rows - 1) * columns + (columns - 1))->location().y() + text_box_array.at((rows - 1) * columns + (columns - 1))->height() + moflexbrowse.height() + 2});
        appendmedia.location({ removemedia.location().x() + removemedia.width() + 2, removemedia.location().y() });
        rowtxt.location({ removemedia.location().x() + ((removemedia.width() + appendmedia.width() + 2) - rowtxt.width()) / 2, removemedia.location().y() + removemedia.height() });

        if (parameters.width() > FFrewind.location().x() + FFrewind.width() + menubannerpreview.width()) {
            menubannerpreview.location({ FFrewind.location().x() + FFrewind.width() + (((parameters.width() - (FFrewind.location().x() + FFrewind.width())) - menubannerpreview.width()) / 2), copycheck.location().y() + ((copycheck.height() + copybox.height() + 3) - (menubannerpreview.height() + bannerpreviewleft.height())) / 2 });
        }
        else {
            menubannerpreview.location({ FFrewind.location().x() + FFrewind.width(), copycheck.location().y() + ((copycheck.height() + copybox.height() + 3) - (menubannerpreview.height() + bannerpreviewleft.height())) / 2 });
        }

        bannerpreviewleft.location({ menubannerpreview.location().x(), menubannerpreview.location().y() + menubannerpreview.height() });
        bannerpreviewright.location({ menubannerpreview.location().x() + menubannerpreview.width() - bannerpreviewright.width(), bannerpreviewleft.location().y() });
        indextxt.location({ menubannerpreview.location().x() + ((menubannerpreview.width()) - indextxt.width()) / 2, menubannerpreview.location().y() + menubannerpreview.height() });


        minorBar.width(finalize.width() - 23);
        majorBar.width(finalize.width() - 23);

        //jank incoming
        if (finalize.height() < minorBarTxt.height() + minorBar.height() + 5 + majorBarTxt.height() + majorBar.height() + 10 + buildButt.height() + cancelBuildButt.height() + randomizeTitleID.height() + TitleIDError.height() + Applicationtxt.height() + ApplicationError.height() + randomizeProductCode.height() + ProductCodeError.height()) {
            minorBarTxt.location({ (finalize.width() - minorBarTxt.width()) / 2, randomizeTitleID.height() + TitleIDError.height() + Applicationtxt.height() + ApplicationError.height() + randomizeProductCode.height() + ProductCodeError.height() });
            titleIDtxt.location({ (finalize.width() - (titleIDtxt.width() + titleIDbox.width() + ZeroZero.width() + randomizeTitleID.width())) / 2, (minorBarTxt.location().y() + minorBarTxt.height()) - (randomizeTitleID.height() + TitleIDError.height() + Applicationtxt.height() + ApplicationError.height() + randomizeProductCode.height() + ProductCodeError.height() + 5) });
        }
        else if (finalize.height() / 2 > minorBarTxt.height() + minorBar.height() + 5 + majorBarTxt.height() + majorBar.height() + 10 + buildButt.height() + cancelBuildButt.height()) {
            minorBarTxt.location({ (finalize.width() - minorBarTxt.width()) / 2, (finalize.height() / 2) + ((finalize.height() / 2) - (minorBarTxt.height() + minorBar.height() + 5 + majorBarTxt.height() + majorBar.height() + 10 + buildButt.height() + cancelBuildButt.height())) / 2 });
            titleIDtxt.location({ (finalize.width() - (titleIDtxt.width() + titleIDbox.width() + ZeroZero.width() + randomizeTitleID.width())) / 2, ((finalize.height() / 2) - (randomizeTitleID.height() + TitleIDError.height() + Applicationtxt.height() + ApplicationError.height() + randomizeProductCode.height() + ProductCodeError.height())) / 2 });
        }
        else {
            minorBarTxt.location({ (finalize.width() - minorBarTxt.width()) / 2, finalize.height() - (minorBarTxt.height() + minorBar.height() + 5 + majorBarTxt.height() + majorBar.height() + 10 + buildButt.height() + cancelBuildButt.height()) });
            titleIDtxt.location({ (finalize.width() - (titleIDtxt.width() + titleIDbox.width() + ZeroZero.width() + randomizeTitleID.width())) / 2, ((finalize.height() / 2) - (randomizeTitleID.height() + TitleIDError.height() + Applicationtxt.height() + ApplicationError.height() + randomizeProductCode.height() + ProductCodeError.height())) / 2 });
        }
        minorBar.location({ (finalize.width() - minorBar.width()) / 2, minorBarTxt.location().y() + minorBarTxt.height() });

        majorBarTxt.location({ (finalize.width() - majorBarTxt.width()) / 2, minorBar.location().y() + minorBar.height() + 5 });
        majorBar.location({ (finalize.width() - majorBar.width()) / 2, majorBarTxt.location().y() + majorBarTxt.height() });

        buildButt.location({ (finalize.width() - buildButt.width()) / 2, majorBar.location().y() + majorBar.height() + 10 });
        cancelBuildButt.location({ (finalize.width() - cancelBuildButt.width()) / 2, buildButt.location().y() + buildButt.height() });

        titleIDbox.location({ titleIDtxt.location().x() + titleIDtxt.width(), titleIDtxt.location().y() + (titleIDtxt.height() - titleIDbox.height()) / 2 });
        ZeroZero.location({ titleIDbox.location().x() + titleIDbox.width(), titleIDtxt.location().y() });
        randomizeTitleID.location({ ZeroZero.location().x() + ZeroZero.width(), ZeroZero.location().y() + (ZeroZero.height() - randomizeTitleID.height()) / 2 });
        TitleIDError.location({ titleIDtxt.location().x() + ((titleIDtxt.width() + titleIDbox.width() + ZeroZero.width() + randomizeTitleID.width()) - TitleIDError.width()) / 2, randomizeTitleID.location().y() + randomizeTitleID.height() });

        Applicationtxt.location({ (finalize.width() - (Applicationtxt.width() + ApplicationName.width() + 4)) / 2, TitleIDError.location().y() + TitleIDError.height() });
        ApplicationName.location({ Applicationtxt.location().x() + Applicationtxt.width() + 4, Applicationtxt.location().y() + (Applicationtxt.height() - ApplicationName.height()) / 2 });
        ApplicationError.location({ Applicationtxt.location().x() + ((Applicationtxt.width() + ApplicationName.width()) - ApplicationError.width()) / 2, Applicationtxt.location().y() + Applicationtxt.height() });

        ProductCodetxt.location({ (finalize.width() - (ProductCodetxt.width() + ProductCode.width() + randomizeProductCode.width())) / 2, ApplicationError.location().y() + ApplicationError.height() });
        ProductCode.location({ ProductCodetxt.location().x() + ProductCodetxt.width(), ProductCodetxt.location().y() + (ProductCodetxt.height() - ProductCode.height()) / 2 });
        randomizeProductCode.location({ ProductCode.location().x() + ProductCode.width(), ProductCodetxt.location().y() + (ProductCodetxt.height() - randomizeProductCode.height()) / 2 });
        ProductCodeError.location({ ProductCodetxt.location().x() + ((ProductCodetxt.width() + ProductCode.width() + randomizeProductCode.width()) - ProductCodeError.width()) / 2, ProductCodetxt.location().y() + ProductCodetxt.height() });


        Logo.location({ (settings.width() - Logo.width()) / 2, 5 });
        maintitle.location({ (settings.width() - maintitle.width()) / 2, Logo.height() + Logo.location().y() });
        subtitle.location({ (settings.width() - subtitle.width()) / 2, maintitle.height() + maintitle.location().y() });
        savebutt.size({ settings.width() - 10, 35 });
        if ((settings.height() / 4) - savebutt.height() - (subtitle.location().y() + subtitle.height()) <= 0) {
            savebutt.location({ (settings.width() - savebutt.width()) / 2, subtitle.location().y() + subtitle.height()});
        }
        else {
            savebutt.location({ (settings.width() - savebutt.width()) / 2, (settings.height() / 4) - savebutt.height() });
        }

        loadbutt.size({ savebutt.width(), savebutt.height() });
        loadbutt.location({ (settings.width() - loadbutt.width()) / 2, savebutt.location().y() + savebutt.height() });
        loadCIAbutt.size({ savebutt.width(), savebutt.height() });
        loadCIAbutt.location({ (settings.width() - loadCIAbutt.width()) / 2, loadbutt.location().y() + loadbutt.height() });
        AutoSave.size({ savebutt.width(), savebutt.height() });
        AutoSave.location({ (settings.width() - AutoSave.width()) / 2, loadCIAbutt.location().y() + loadCIAbutt.height() });
        AutoLoad.size({ savebutt.width(), savebutt.height() });
        AutoLoad.location({ (settings.width() - AutoLoad.width()) / 2, AutoSave.location().y() + AutoSave.height() });
        DeleteTempButt.size({ savebutt.width(), savebutt.height() });
        DeleteTempButt.location({ (settings.width() - DeleteTempButt.width()) / 2, AutoLoad.location().y() + AutoLoad.height() });
        LightDark.size({ savebutt.width(), savebutt.height()});
        LightDark.location({ (settings.width() - LightDark.width()) / 2, DeleteTempButt.location().y() + DeleteTempButt.height() });
        //fontchange.location({ (settings.width() - fontchange.width()) / 2, LightDark.location().y() + LightDark.height() });
        LanguageTitle.location({ (settings.width() - LanguageTitle.width()) / 2, LightDark.location().y() + LightDark.height() });
        LanguageChoiche.location({ (settings.width() - LanguageChoiche.width()) / 2, LanguageTitle.location().y() + LanguageTitle.height() });
        restartbutt.size({ savebutt.width(), savebutt.height() });
        restartbutt.location({ (settings.width() - restartbutt.width()) / 2, LanguageChoiche.location().y() + LanguageChoiche.height() });


        if (settings.height() < restartbutt.location().y() + restartbutt.height() + version.height() + 6) {
            version.location({ (settings.width() - version.width()) / 2, restartbutt.location().y() + restartbutt.height() + 3 });
        }
        else {
            version.location({ (settings.width() - version.width()) / 2, settings.height() - version.height() - 3 });
        }
    };

    text_box_array.at(columns - 1)->location_changed += [&] {
        playertitletxt.location({ mediabox.location().x() + text_box_array.at(0)->location().x() + (text_box_array.at(0)->width() - playertitletxt.width()) / 2, copybox.location().y() + copybox.height() });
        moflextxt.location({ mediabox.location().x() + text_box_array.at(1)->location().x() + (text_box_array.at(1)->width() - moflextxt.width()) / 2, copybox.location().y() + copybox.height() });
        menubannertxt.location({ mediabox.location().x() + text_box_array.at(2)->location().x() + (text_box_array.at(2)->width() - (menubannertxt.width() + bannermulti.width() + 3)) / 2, copybox.location().y() + copybox.height() });
        bannermulti.location({ menubannertxt.location().x() + menubannertxt.width() + 3, menubannertxt.location().y() + ((menubannertxt.height() - bannermulti.height()) / 2) });
        playertitletxt.refresh();
        moflextxt.refresh();
        menubannertxt.refresh();
        bannermulti.refresh();
    };

    if (autoLoadParams || LoadFromArgv) {
        loadParameters();
    }

    appendmedia.enabled(mode.selected_index() && rows < 27);
    removemedia.enabled(rows > 1);
    SetIconPreview();
    loaded = true;
    top_most(false);
}

void form1::main() {
    //xtd::forms::application::enable_dark_mode();
    xtd::forms::application::run(form1());
}