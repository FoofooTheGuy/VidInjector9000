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

    debugs.parent(parameters);
    debugs.auto_size(true);
    debugs.font(this->font());
    debugs.text(xtd::ustring::format("{}, {}", parameters.width(), parameters.height()));
    debugs.location({ 0, parameters.height() - debugs.height() });
    debugs.anchor(xtd::forms::anchor_styles::bottom | xtd::forms::anchor_styles::left);
    debugs.hide();//lazy way to get rid of this

    //logo button
    Logo.parent(parameters);
    Logo.cursor(xtd::forms::cursors::hand());
    Logo.image(pixels_to_image(logo90, 90, 90, 4));
    Logo.bounds({ 6, 6, 90, 90 });
    Logo.size_mode(xtd::forms::picture_box_size_mode::center_image);
    Logo.click += [&] {
        xtd::diagnostics::process::start("https://github.com/FoofooTheGuy/VidInjector9000/releases/latest");
    };

    //mode stuff
    modetxt.parent(parameters);
    modetxt.auto_size(true);
    modetxt.font({ this->font(), 15 });
    modetxt.location({ 105, 30 });
    modetxt.text(ModeText);

    mode.parent(parameters);
    mode.auto_size(true);
    mode.font(this->font());
    mode.location({ 105, modetxt.location().y() + modetxt.height()});//tether to modetxt
    mode.items().push_back_range({ SingleVideo, MultiVideo });
    mode.selected_index(0);

    //banner stuff
    bannertxt.parent(parameters);
    bannertxt.auto_size(true);
    bannertxt.font({ this->font(), 15 });
    bannertxt.location({ 20, 100 });
    bannertxt.text(BannerText);

    bannerbox.parent(parameters);
    bannerbox.font(this->font());
    bannerbox.location({ bannertxt.location().x() + bannertxt.width() + 5, bannertxt.location().y() + (bannertxt.height() - bannerbox.height()) / 2});//tether to bannertxt
    bannerbox.width(parameters.width() - bannerbox.location().x() - 599);
    bannerbox.cursor(xtd::forms::cursors::ibeam());

    bannerbrowse.parent(parameters);
    bannerbrowse.size({ 119, 35 });
    bannerbrowse.font(this->font());
    bannerbrowse.location({ parameters.width() - 597, bannerbox.location().y() - (bannerbrowse.height() / 2 - bannerbox.height() / 2) });//tether to bannerbox
    bannerbrowse.text(Browse);
    bannerbrowse.click += [&] {
        bannerbox.text(load_file(xtd::ustring::format("{} {}{}{}", SupportedImage200x120, SupportedImageList, CGFXList, AllFilesList)));
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
    bannerpreview.location({ (bannerbrowse.location().x() + bannerbrowse.width() + 2) + (((parameters.width() - (bannerbrowse.location().x() + bannerbrowse.width() + 2)) - bannerpreview.width()) / 2), (shortname.location().y() - (bannerpreview.height() + bannerpreviewtxt.height())) / 2 });
    {
        int ch = 4;
        int out_w = 200;
        int out_h = 120;
        int film_w = 264;
        int film_h = 154;
        unsigned char* output_4c = (unsigned char*)malloc(out_w * out_h * ch);
        memset(output_4c, 0xFF, out_w * out_h * ch);

        unsigned char* output_film = (unsigned char*)malloc(film_w * film_h * 4);
        layer_pixels(output_film, film_overlay, output_4c, film_w, film_h, 4, out_w, out_h, ch, 32, 11);
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
        int w, h, ch = 0;
        int out_w = 200;
        int out_h = 120;
        int film_w = 264;
        int film_h = 154;
        unsigned char Checker[4];
        bool CGFX = false;
        std::ifstream inbanner(bannerbox.text(), std::ios::binary);
        if (std::filesystem::exists(bannerbox.text().c_str())) {
            for (int i = 0; i < 4; i++) {
                inbanner >> Checker[i];//https://stackoverflow.com/a/2974735
                if (Checker[i] == bannerMagic[i]) {
                    bannerpreview.image(empty(0, 0));
                    customnotif.show();
                    CGFX = true;
                }
                else {
                    bannerpreview.image(pixels_to_image(film_overlay, 264, 154, 4));
                    customnotif.hide();
                    CGFX = false;
                    break;
                }
            }
        }
        if (!CGFX) {
            if (std::filesystem::exists(bannerbox.text().c_str()) && stbi_info(bannerbox.text().c_str(), &w, &h, &ch)) {
                unsigned char* input_pixels = stbi_load(bannerbox.text().c_str(), &w, &h, &ch, 0);
                unsigned char* output_pixels = (unsigned char*)malloc(out_w * out_h * ch);
                unsigned char* output_3c = (unsigned char*)malloc(out_w * out_h * 3);
                const uint8_t FF = 0xFF;

                if (w == out_w && h == out_h) memcpy(output_pixels, input_pixels, w * h * ch);
                else stbir_resize_uint8(input_pixels, w, h, 0, output_pixels, out_w, out_h, 0, ch);//scale to 200x120 if needed
                free(input_pixels);
                if (ch == 4) {//if png?
                    unsigned char* white_background = (unsigned char*)malloc(out_w * out_h * ch);
                    memset(white_background, FF, out_w * out_h * ch);
                    layer_pixels(output_pixels, output_pixels, white_background, out_w, out_h, ch, out_w, out_h, ch, 0, 0);
                    free(white_background);
                    int newi = 3;
                    for (int i = 3; i < out_w * out_h * 4; i += 4) {
                        output_3c[newi - 3] = output_pixels[i - 3];
                        output_3c[newi - 2] = output_pixels[i - 2];
                        output_3c[newi - 1] = output_pixels[i - 1];
                        newi += 3;
                    }
                }
                if (ch == 3) {
                    memcpy(output_3c, output_pixels, out_w * out_h * ch);
                }
                free(output_pixels);
                unsigned char* output_film = (unsigned char*)malloc(film_w * film_h * 4);
                //memcpy(output_film, film_overlay, film_w * film_h * 4);
                layer_pixels(output_film, film_overlay, output_3c, film_w, film_h, 4, out_w, out_h, 3, 32, 11);
                free(output_3c);
                bannerpreview.image(pixels_to_image(output_film, film_w, film_h, 4));
                bannererror.hide();
                free(output_film);
                customnotif.hide();
            }
            else {
                ch = 4;
                unsigned char* output_4c = (unsigned char*)malloc(out_w * out_h * ch);
                memset(output_4c, 0xFF, out_w * out_h * ch);

                unsigned char* output_film = (unsigned char*)malloc(film_w * film_h * 4);
                layer_pixels(output_film, film_overlay, output_4c, film_w, film_h, 4, out_w, out_h, ch, 32, 11);
                free(output_4c);

                customnotif.hide();
                bannerpreview.image(pixels_to_image(output_film, film_w, film_h, 4));
                bannererror.show();

                free(output_film);
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
        iconbox.text(load_file(xtd::ustring::format("{} {}{}", SupportedImage48x48, SupportedImageList)));
    };

    iconpreview.parent(parameters);
    iconpreview.border_style(xtd::forms::border_style::fixed_3d);
    iconpreview.size({ 52, 52 });
    iconpreview.image(empty(48, 48));
    //iconpreview.location({ iconbrowse.location().x() + iconbrowse.width() + 2, iconbrowse.location().y() + 2 });

    iconerror.parent(parameters);
    iconerror.auto_size(true);
    iconerror.font({ this->font(), xtd::drawing::font_style::bold });
    iconerror.location({ iconbox.location().x(), iconbrowse.location().y() + iconbrowse.height() });//put it below bannertxt
    iconerror.text(xtd::ustring::format("{} {}", ErrorText, ImageInfoError));
    iconerror.hide();

    iconbox.text_changed += [&] {
        if (autoSaveParams && loaded) saveSettings();
        int w, h, ch = 0;
        int largeWH = 48;
        if (std::filesystem::exists(iconbox.text().c_str()) && stbi_info(iconbox.text().c_str(), &w, &h, &ch)) {
            unsigned char* input_pixels = stbi_load(iconbox.text().c_str(), &w, &h, &ch, 0);
            unsigned char* output_pixels = (unsigned char*)malloc(largeWH * largeWH * ch);
            unsigned char* large_3c = (unsigned char*)malloc(largeWH * largeWH * 3);
            //const int smallLW = 24;
            const uint8_t FF = 0xFF;

            if (w == largeWH && h == largeWH) memcpy(output_pixels, input_pixels, w * h * ch);
            else stbir_resize_uint8(input_pixels, w, h, 0, output_pixels, largeWH, largeWH, 0, ch);//scale to 200x120 if needed

            if (ch == 4) {//if png?
                unsigned char* white_background = (unsigned char*)malloc(largeWH * largeWH * ch);
                memset(white_background, FF, largeWH * largeWH * ch);
                layer_pixels(output_pixels, output_pixels, white_background, largeWH, largeWH, ch, largeWH, largeWH, 4, 0, 0);
                free(white_background);
                int newi = 3;
                for (int i = 3; i < largeWH * largeWH * 4; i += 4) {
                    large_3c[newi - 3] = output_pixels[i - 3];
                    large_3c[newi - 2] = output_pixels[i - 2];
                    large_3c[newi - 1] = output_pixels[i - 1];
                    newi += 3;
                }
            }
            if (ch == 3) {
                memcpy(large_3c, output_pixels, largeWH * largeWH * ch);
            }
            iconpreview.image(pixels_to_image(large_3c, largeWH, largeWH, 3));
            stbi_image_free(input_pixels);
            free(output_pixels);
            free(large_3c);
            iconerror.hide();
        }
        else {
            iconpreview.image(empty(largeWH, largeWH));
            iconerror.show();
        }
    };

    shortnametxt.parent(parameters);
    shortnametxt.auto_size(true);
    shortnametxt.font({ this->font(), 15 } );
    shortnametxt.location({ 20, iconerror.location().y() + iconerror.height() + 1 });//put it below icontxt
    shortnametxt.text(ShortNameText);

    shortname.parent(parameters);
    shortname.font(this->font());
    shortname.location({ shortnametxt.location().x() + shortnametxt.width() + 5, shortnametxt.location().y() + (shortnametxt.height() - shortname.height()) / 2 });//tether to shortnametxt
    shortname.width(parameters.width() - shortname.location().x() - 23);
    shortname.cursor(xtd::forms::cursors::ibeam());

    shorterror.parent(parameters);
    shorterror.auto_size(true);
    shorterror.font({ this->font(), xtd::drawing::font_style::bold });
    shorterror.location({ shortname.location().x(), shortname.location().y() + shortname.height() });//put it below shortname
    shorterror.text(xtd::ustring::format("{} {} ({}/{})", ErrorText, TextTooLongError, UTF8toUTF16(shortname.text()).size() / 2, 64));
    shorterror.hide();

    shortname.text_changed += [&] {
        if (autoSaveParams && loaded) saveSettings();
        if (UTF8toUTF16(shortname.text()).size() > 0x80) {
            shorterror.text(xtd::ustring::format("{} {} ({}/{})", ErrorText, TextTooLongError, UTF8toUTF16(shortname.text()).size() / 2, 64));
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
    longname.font(this->font());
    longname.location({ longnametxt.location().x() + longnametxt.width() + 5, longnametxt.location().y() + (longnametxt.height() - longname.height()) / 2 });//tether to longnametxt
    longname.width(parameters.width() - longname.location().x() - 23);
    longname.cursor(xtd::forms::cursors::ibeam());

    longerror.parent(parameters);
    longerror.auto_size(true);
    longerror.font({ this->font(), xtd::drawing::font_style::bold });
    longerror.location({ longname.location().x(), longname.location().y() + longname.height() });//tether to longname box
    longerror.text(xtd::ustring::format("{} {} ({}/{})", ErrorText, TextTooLongError, UTF8toUTF16(longname.text()).size() / 2, 128));
    longerror.hide();

    longname.text_changed += [&] {
        if (autoSaveParams && loaded) saveSettings();
        if (UTF8toUTF16(longname.text()).size() > 0x100) {
            longerror.text(xtd::ustring::format("{} {} ({}/{})", ErrorText, TextTooLongError, UTF8toUTF16(longname.text()).size() / 2, 128));
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
    publisher.font(this->font());
    publisher.location({ publishertxt.location().x() + publishertxt.width() + 5, publishertxt.location().y() + (publishertxt.height() - publisher.height()) / 2 });//tether to publishertxt
    publisher.width(parameters.width() - publisher.location().x() - 23);
    publisher.cursor(xtd::forms::cursors::ibeam());

    publishererror.parent(parameters);
    publishererror.auto_size(true);
    publishererror.font({ this->font(), xtd::drawing::font_style::bold });
    publishererror.location({ publisher.location().x(), publisher.location().y() + publisher.height() });//tether to publisher box
    publishererror.text(xtd::ustring::format("{} {} ({}/{})", ErrorText, TextTooLongError, UTF8toUTF16(publisher.text()).size() / 2, 64));
    publishererror.hide();

    publisher.text_changed += [&] {
        if (autoSaveParams && loaded) saveSettings();
        if (UTF8toUTF16(publisher.text()).size() > 0x80) {
            publishererror.text(xtd::ustring::format("{} {} ({}/{})", ErrorText, TextTooLongError, UTF8toUTF16(publisher.text()).size() / 2, 64));
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
    copymulti.mouse_move += [&](object& sender, const xtd::forms::mouse_event_args& e) {
        MultiOnly.location({ copymulti.location().x() + e.location().x(), copymulti.location().y() + e.location().y() - MultiOnly.height() });
        if (MultiOnly.location().x() + MultiOnly.width() > parameters.width())
            MultiOnly.location({ copymulti.location().x() + e.location().x() - MultiOnly.width(), copymulti.location().y() + e.location().y() - MultiOnly.height() });
        MultiOnly.show();
    };
    copymulti.mouse_leave += [&] {
        MultiOnly.hide();
    };
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

    menutitletxt.parent(parameters);
    menutitletxt.auto_size(true);
    menutitletxt.font({ this->font(), 15 });
    menutitletxt.text(MenuTitleText);
    menutitletxt.enabled(mode.selected_index());

    menubannertxt.parent(parameters);
    menubannertxt.auto_size(true);
    menubannertxt.font({ this->font(), 15 });
    menubannertxt.text(MenuBannerText);
    menubannertxt.enabled(mode.selected_index());

    mediabox.parent(parameters);
    mediabox.border_style(xtd::forms::border_style::fixed_3d);
    mediabox.location({ copybox.location().x(), copybox.location().y() + copybox.height() + playertitletxt.height() });
    mediabox.size({ parameters.width() - 46, parameters.height() - (copybox.location().y() + copybox.height() + playertitletxt.height()) - debugs.height() });
    mediabox.auto_scroll(true);

    titlemulti.parent(parameters);
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
    titlemulti.location({ menutitletxt.location().x() + menutitletxt.width() + 3, menutitletxt.location().y() + ((menutitletxt.height() - titlemulti.height()) / 2) });

    bannermulti.parent(parameters);
    bannermulti.size({ 9, 9 });
    {
        int w = 9, h = 9, ch = 4;
        bannermulti.image(LightDark.checked() ? pixels_to_image(invert_pixels(stbi_load(xtd::ustring::format("{}/{}/language/{}/M.bmp", ProgramDir, resourcesPath, Languagedir).c_str(), &w, &h, &ch, 0), w, h, ch), w, h, ch) : pixels_to_image(stbi_load(xtd::ustring::format("{}/{}/language/{}/M.bmp", ProgramDir, resourcesPath, Languagedir).c_str(), &w, &h, &ch, 0), w, h, ch));
    }
    bannermulti.cursor(xtd::forms::cursors::help());
    bannermulti.mouse_move += [&](object& sender, const xtd::forms::mouse_event_args& e) {
        MultiOnly.location({ bannermulti.location().x() + e.location().x(), bannermulti.location().y() + e.location().y() - MultiOnly.height() });
        if(MultiOnly.location().x() + MultiOnly.width() > parameters.width())
            MultiOnly.location({ bannermulti.location().x() + e.location().x() - MultiOnly.width(), bannermulti.location().y() + e.location().y() - MultiOnly.height()});
        MultiOnly.show();
    };
    bannermulti.mouse_leave += [&] {
        MultiOnly.hide();
    };
    bannermulti.location({ menubannertxt.location().x() + menubannertxt.width() + 3, menubannertxt.location().y() + ((menubannertxt.height() - bannermulti.height()) / 2) });

    menubannerpreview.parent(parameters);
    if (mode.selected_index()) menubannerpreview.cursor(xtd::forms::cursors::hand());
    else menubannerpreview.cursor(xtd::forms::cursors::no());
    //menubannerpreview.border_style(xtd::forms::border_style::fixed_3d);
    menubannerpreview.size({ 264, 154 });
    menubannerpreview.location({ copybox.location().x() + copybox.width() + (parameters.width() - (copybox.location().x() + copybox.width())) / 2, copycheck.location().y() + ((copycheck.height() + copybox.height() + 3) - menubannerpreview.height()) / 2});
    menubannerpreview.click += [&] {
        if (autoSaveParams && loaded) saveSettings();
        if (mode.selected_index()) {
            xtd::ustring filepath = load_file(xtd::ustring::format("{} {}{}", SupportedImage200x120, SupportedImageList));
            if (filepath != "") text_box_array.at(bannerpreviewindex * columns + 3)->text(filepath);
            setMultiBannerPreview(bannerpreviewindex);
            //bannerpreviewleft.enabled(mode.selected_index() && bannerpreviewindex != 0);
            //bannerpreviewright.enabled(mode.selected_index() && bannerpreviewindex != rows - 1);
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
    bannerpreviewleft.image(xtd::forms::button_images::previous());
    bannerpreviewleft.click += [&] {
        if (bannerpreviewindex != 0)
            bannerpreviewindex--;
        bannerpreviewleft.enabled(bannerpreviewindex != 0);//if it returns false, it means it's at 0 and it turns false, or on if it's true
        setMultiBannerPreview(bannerpreviewindex);
        bannerpreviewleft.enabled(mode.selected_index() && bannerpreviewindex != 0);
        bannerpreviewright.enabled(mode.selected_index() && bannerpreviewindex != rows - 1);
        if (autoSaveParams && loaded) saveSettings();
    };
    bannerpreviewleft.enabled(mode.selected_index());

    bannerpreviewright.parent(parameters);
    bannerpreviewright.auto_size(true);
    bannerpreviewright.image(xtd::forms::button_images::next());
    bannerpreviewright.click += [&] {
        if (bannerpreviewindex != rows - 1)
            bannerpreviewindex++;
        bannerpreviewright.enabled(bannerpreviewindex != rows - 1);
        setMultiBannerPreview(bannerpreviewindex);
        bannerpreviewleft.enabled(mode.selected_index() && bannerpreviewindex != 0);
        bannerpreviewright.enabled(mode.selected_index() && bannerpreviewindex != rows - 1);
        if (autoSaveParams && loaded) saveSettings();
    };
    bannerpreviewright.enabled(mode.selected_index());

    for (int y = 0; y < rows; y++)
        for (int x = 0; x < columns; x++) {
            xtd::forms::text_box* box_new = new xtd::forms::text_box();
            text_box_array.push_back(box_new);//put box in array
            text_box_array.at(y * columns + x)->parent(mediabox);//assign it data
            text_box_array.at(y * columns + x)->font(this->font());
            text_box_array.at(y * columns + x)->width((mediabox.width() / columns) - (((copybox.location().x()) / columns) + 1));
            text_box_array.at(y * columns + x)->cursor(xtd::forms::cursors::ibeam());
        }
    for (int i = 2; i <= 3; i++)
        text_box_array.at(i)->enabled(mode.selected_index());
    setMultiBannerPreview(rows - 1);

    moflexbrowse.parent(mediabox);
    moflexbrowse.size({ 119, 35 });
    moflexbrowse.font(this->font());
    //moflexbrowse.location({ text_box_array.at((rows - 1) * columns + 1)->location().x() + (text_box_array.at((rows - 1) * columns + 1)->width() - moflexbrowse.width()) / 2, text_box_array.at((rows - 1) * columns + 1)->location().y() + text_box_array.at((rows - 1) * columns + 1)->height() });
    moflexbrowse.text(Browse);
    moflexbrowse.click += [&] {
        text_box_array[(rows - 1) * columns + 1]->text(load_file(MoflexFilesList));
    };

    multibannerbrowse.parent(mediabox);
    multibannerbrowse.size({ 119, 35 });
    multibannerbrowse.font(this->font());
    //multibannerbrowse.location({ text_box_array.at((rows - 1) * columns + 3)->location().x() + (text_box_array.at((rows - 1) * columns + 3)->width() - moflexbrowse.width()) / 2, text_box_array.at((rows - 1) * columns + 3)->location().y() + text_box_array.at((rows - 1) * columns + 3)->height() });
    multibannerbrowse.text(Browse);
    multibannerbrowse.click += [&] {
        xtd::ustring filepath = load_file(xtd::ustring::format("{} {}{}", SupportedImage200x120, SupportedImageList));
        if(filepath != "") text_box_array.at((rows - 1) * columns + 3)->text(filepath);
        setMultiBannerPreview(rows - 1);
        bannerpreviewleft.enabled(mode.selected_index() && bannerpreviewindex != 0);
        bannerpreviewright.enabled(mode.selected_index() && bannerpreviewindex != rows - 1);
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
    appendmedia.enabled(mode.selected_index());

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
    removemedia.enabled(mode.selected_index());

    //finalize
    finalize.parent(tab_control);
    finalize.text(FinalizeText);

    //title ID stuff
    titleIDtxt.parent(finalize);
    titleIDtxt.auto_size(true);
    titleIDtxt.font({ this->font(), 15 });
    titleIDtxt.text(xtd::ustring::format("{} 000400000", TitleIDText));

    titleIDbox.parent(finalize);
    titleIDbox.font(this->font());
    titleIDbox.width(55);
    titleIDbox.cursor(xtd::forms::cursors::ibeam());
    titleIDbox.text(xtd::ustring::format("{:X5}", RandomTID()));
    titleIDbox.text_changed += [&] {
        int index = titleIDbox.selection_start();
        if (index > 5) index = 5;
        xtd::ustring temp = titleIDbox.text();
        if (temp.size() > 5)
            temp = titleIDbox.text().substr(0, 5);
        for (auto &c : temp) {
            if (tolowerstr(std::string(1, c)).find_first_of("0123456789abcdef") != 0) {
                c = '0';
            }
        }

        unsigned long TID;
        stoul_s(TID, temp, true);
        if (!TIDisValid(TID)) TitleIDError.show();
        else TitleIDError.hide();

        while (TID <= 0xFFFFF && TID * 0x10 <= 0xFFFFF && TID != 0) TID = TID * 0x10;

        titleIDbox.text(xtd::ustring::format("{:X5}", TID));
        titleIDbox.select(index, 0);//put beam at the right place
    };

    TitleIDError.parent(finalize);
    TitleIDError.auto_size(true);
    TitleIDError.font({ this->font(), xtd::drawing::font_style::bold });
    TitleIDError.text(xtd::ustring::format("{} {}", ErrorText, BadValue));
    TitleIDError.hide();

    ZeroZero.parent(finalize);
    ZeroZero.auto_size(true);
    ZeroZero.font({ this->font(), 15 });
    ZeroZero.text("00");

    randomizeTitleID.parent(finalize);
    randomizeTitleID.size({ 38, 38 });
    randomizeTitleID.image(LightDark.checked() ? pixels_to_image(invert_pixels(randomize_array, 30, 27, 4), 30, 27, 4) : pixels_to_image(randomize_array, 30, 27, 4));//put shuffle icon here
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
    ApplicationName.width(95);
    ApplicationName.cursor(xtd::forms::cursors::ibeam());
    ApplicationName.text("video");
    ApplicationName.text_changed += [&] {
        int index = ApplicationName.selection_start();
        xtd::ustring temp = ApplicationName.text();
        for (auto& c : temp) {
            if (tolowerstr(std::string(1, c)).find_first_of("abcdefghijklmnopqrstuvwxyz") != 0) {//maybe numbers work too but i wont allow it because numbers in process name is ugly ;-;
                c = ' ';
            }
        }

        ApplicationName.text(temp);
        ApplicationName.select(index, 0);


        if (UTF8toUTF16(ApplicationName.text()).size() / 2 > 8) {//funny turn utf8 into utf16 and cut in half to ensure ascii sizes even though we already set it all to ascii only characters but whatever
            ApplicationError.text(xtd::ustring::format("{} {} ({}/{})", ErrorText, TextTooLongError, UTF8toUTF16(ApplicationName.text()).size() / 2, 8));
            ApplicationError.show();
        }
        else ApplicationError.hide();
    };

    ApplicationError.parent(finalize);
    ApplicationError.auto_size(true);
    ApplicationError.font({ this->font(), xtd::drawing::font_style::bold });
    ApplicationError.text(xtd::ustring::format("{} {} ({}/{})", ErrorText, TextTooLongError, UTF8toUTF16(ApplicationName.text()).size() / 2, 8));
    ApplicationError.hide();

    //product code stuff
    ProductCodetxt.parent(finalize);
    ProductCodetxt.auto_size(true);
    ProductCodetxt.font({ this->font(), 15 });
    ProductCodetxt.text(xtd::ustring::format("{} CTR-H-", ProductCodetext));

    ProductCode.parent(finalize);
    ProductCode.font(this->font());
    ProductCode.width(57);
    ProductCode.cursor(xtd::forms::cursors::ibeam());
    ProductCode.text("VDIJ");
    ProductCode.text_changed += [&] {
        int index = ProductCode.selection_start();
        if (index > 4) index = 4;
        xtd::ustring temp = ProductCode.text();
        if (temp.size() > 4)
            temp = ProductCode.text().substr(0, 4);
        for (auto& c : temp) {
            if (tolowerstr(std::string(1, c)).find_first_of("abcdefghijklmnopqrstuvwxyz0123456789") != 0) {
                c = 'A';
            }
        }

        ProductCode.text(toupperstr(temp));
        ProductCode.select(index, 0);
    };

    randomizeProductCode.parent(finalize);
    randomizeProductCode.size({ 38, 38 });
    randomizeProductCode.image(pixels_to_image(randomize_array, 30, 27, 4));//since invert image permanantly changed the picture, we can just do this now
    randomizeProductCode.click += [&] {
        std::string set = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        std::string outText = "";
        static std::mt19937 rng;
        for (int i = 0; i < 4; i++) {
            rng.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
            std::uniform_int_distribution<unsigned long> uniform(0, sizeof(set) - 1);
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
    //minorBarTxt.hide();

    minorBar.parent(finalize);
    minorBar.height(45);
    minorBar.maximum(0);
    minorBar.minimum(0);
    minorBar.style(xtd::forms::progress_bar_style::marquee);

    //major bar
    majorBarTxt.parent(finalize);
    majorBarTxt.auto_size(true);
    majorBarTxt.font({ this->font(), 15 });
    majorBarTxt.text(FormText);
    //majorBarTxt.hide();

    majorBar.parent(finalize);
    majorBar.size({ finalize.width() - 23, 50 });

    buildButt.parent(finalize);
    buildButt.auto_size(true);
    buildButt.font({ this->font(), 15 });
    buildButt.text(BuildCIAText);
    buildButt.size({ buildButt.width() * 2, buildButt.height() * 2});
    buildButt.click += [&] {
        builder.run_worker_async();
    };

    cancelBuildButt.parent(finalize);
    cancelBuildButt.auto_size(true);
    cancelBuildButt.font({ this->font(), 15 });
    cancelBuildButt.text(Cancel);
    cancelBuildButt.enabled(false);
    cancelBuildButt.click += [&] {
        builder.cancel_async();
    };

    builder.worker_supports_cancellation(true);
    builder.worker_reports_progress(true);
    builder.do_work += [&] {
        cancelBuildButt.enabled(true);
        minorBar.maximum(69);
        minorBar.minimum(0);
        //extract base
        std::filesystem::remove_all(xtd::ustring::format("{}/{}/temp", ProgramDir, resourcesPath).c_str());
        Generate_Files(xtd::ustring::format("{}/{}/temp", ProgramDir, resourcesPath).c_str(), mode.selected_index());
        //make movie_title.csv (player title)
        {
            std::filesystem::create_directories(xtd::ustring::format("{}/{}/temp/romfs/movie", ProgramDir, resourcesPath).c_str());
            std::ofstream movie_title(xtd::ustring::format("{}/{}/temp/romfs/movie/movie_title.csv", ProgramDir, resourcesPath).c_str(), std::ios_base::out | std::ios_base::binary);

            movie_title << "\xFF\xFE" + UTF8toUTF16("#JP,#EN,#FR,#GE,#IT,#SP,#CH,#KO,#DU,#PO,#RU,#TW\x0D\x0A");
            for (int i = 0; i < (mode.selected_index() ? rows : 1); i++) {
                std::string outstr = text_box_array.at(i * columns)->text();

                if (outstr[0] == '#') {//sneakily fix the string huhuhu
                    outstr[0] = '\\';
                    outstr.insert(1, "x23");
                }
                for (unsigned long long j = 0; j < outstr.size(); j++) {
                    if (outstr[j] == ',') {
                        outstr[j] = '\\';
                        outstr.insert(j + 1, "x2C");
                    }
                }
                for (int j = 0; j < 11; j++) {//do it 11 times because it needs to
                    movie_title << UTF8toUTF16(outstr + ",");
                }
                movie_title << UTF8toUTF16(outstr + "\x0D\x0A");//put the last stuff
            }
            movie_title.close();
            if (!std::filesystem::exists(xtd::ustring::format("{}/{}/temp/romfs/movie/movie_title.csv", ProgramDir, resourcesPath).c_str())) {
                xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}/{}/temp/romfs/movie/movie_title.csv", FailedToCreateFile, ProgramDir, resourcesPath), xtd::ustring::format("{} {}", ErrorText, FailedToFindPath), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                builder.cancel_async();
                return;
            }
        }
        //make settingsTL.csv (menu title and stuff)
        {
            std::filesystem::create_directories(xtd::ustring::format("{}/{}/temp/romfs/settings", ProgramDir, resourcesPath).c_str());
            std::ofstream settingsTL(xtd::ustring::format("{}/{}/temp/romfs/settings/settingsTL.csv", ProgramDir, resourcesPath).c_str(), std::ios_base::out | std::ios_base::binary);

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
                    + longname.text() + "\x0D\x0A"
                    "\x0D\x0A"
                    "# EN:\x0D\x0A"
                    + longname.text() + "\x0D\x0A"
                    "\x0D\x0A"
                    "# FR:\x0D\x0A"
                    + longname.text() + "\x0D\x0A"
                    "\x0D\x0A"
                    "# GE:\x0D\x0A"
                    + longname.text() + "\x0D\x0A"
                    "\x0D\x0A"
                    "# IT:\x0D\x0A"
                    + longname.text() + "\x0D\x0A"
                    "\x0D\x0A"
                    "# SP:\x0D\x0A"
                    + longname.text() + "\x0D\x0A"
                    "\x0D\x0A"
                    "# CN:\x0D\x0A"
                    + longname.text() + "\x0D\x0A"
                    "\x0D\x0A"
                    "# KO:\x0D\x0A"
                    + longname.text() + "\x0D\x0A"
                    "\x0D\x0A"
                    "# DU:\x0D\x0A"
                    + longname.text() + "\x0D\x0A"
                    "\x0D\x0A"
                    "# PO:\x0D\x0A"
                    + longname.text() + "\x0D\x0A"
                    "\x0D\x0A"
                    "# RU:\x0D\x0A"
                    + longname.text() + "\x0D\x0A"
                    "\x0D\x0A"
                    "# TW:\x0D\x0A"
                    + longname.text() + "\x0D\x0A"
                    "\x0D\x0A"
                    "# 拡張セーブデータのID（16進数）\x0D\x0A"//save data ID
                    + titleIDbox.text() + "\x0D\x0A"//make it the save as title ID because yes
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
                    "# おしらせのあり、なし\x0D\x0A"//not sure what this is, but if you enable it in single vid it instantly crashes... maybe it's the thing telling you to take a break? takes too long to test lol
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
                    + publisher.text().c_str() + "\x0D\x0A"//why does this have to be c_str but longname doesnt? oh well
                    "\x0D\x0A"
                    "# EN:\x0D\x0A"
                    + publisher.text().c_str() + "\x0D\x0A"
                    "\x0D\x0A"
                    "# FR:\x0D\x0A"
                    + publisher.text().c_str() + "\x0D\x0A"
                    "\x0D\x0A"
                    "# GE:\x0D\x0A"
                    + publisher.text().c_str() + "\x0D\x0A"
                    "\x0D\x0A"
                    "# IT:\x0D\x0A"
                    + publisher.text().c_str() + "\x0D\x0A"
                    "\x0D\x0A"
                    "# SP:\x0D\x0A"
                    + publisher.text().c_str() + "\x0D\x0A"
                    "\x0D\x0A"
                    "# CN:\x0D\x0A"
                    + publisher.text().c_str() + "\x0D\x0A"
                    "\x0D\x0A"
                    "# KO:\x0D\x0A"
                    + publisher.text().c_str() + "\x0D\x0A"
                    "\x0D\x0A"
                    "# DU:\x0D\x0A"
                    + publisher.text().c_str() + "\x0D\x0A"
                    "\x0D\x0A"
                    "# PO:\x0D\x0A"
                    + publisher.text().c_str() + "\x0D\x0A"
                    "\x0D\x0A"
                    "# RU:\x0D\x0A"
                    + publisher.text().c_str() + "\x0D\x0A"
                    "\x0D\x0A"
                    "# TW:\x0D\x0A"
                    + publisher.text().c_str() + "\x0D\x0A"
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
            if (!std::filesystem::exists(xtd::ustring::format("{}/{}/temp/romfs/settings/settingsTL.csv", ProgramDir, resourcesPath).c_str())) {
                xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}/{}/temp/romfs/settings/settingsTL.csv", FailedToCreateFile, ProgramDir, resourcesPath), xtd::ustring::format("{} {}", ErrorText, FailedToFindPath), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                builder.cancel_async();
                return;
            }
        }
        //make copyright stuff (multi vid only)
        if (mode.selected_index()) {
            std::filesystem::create_directories(xtd::ustring::format("{}/{}/temp/romfs/settings", ProgramDir, resourcesPath).c_str());//just in case Hehehhhah
            std::ofstream information_buttons(xtd::ustring::format("{}/{}/temp/romfs/settings/information_buttons.csv", ProgramDir, resourcesPath).c_str(), std::ios_base::out | std::ios_base::binary);
            information_buttons << (copycheck.checked() ? ("\xFF\xFE" + UTF8toUTF16("Copyright")) : "\xFF\xFE");
            information_buttons.close();
            if (!std::filesystem::exists(xtd::ustring::format("{}/{}/temp/romfs/settings/information_buttons.csv", ProgramDir, resourcesPath).c_str())) {
                xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}/{}/temp/romfs/settings/information_buttons.csv", FailedToCreateFile, ProgramDir, resourcesPath), xtd::ustring::format("{} {}", ErrorText, FailedToFindPath), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                builder.cancel_async();
                return;
            }

            if (copycheck.checked()) {
                std::ofstream copyrighttxt(xtd::ustring::format("{}/{}/temp/romfs/settings/copyright.txt", ProgramDir, resourcesPath).c_str(), std::ios_base::out | std::ios_base::binary);
                copyrighttxt << "\xFF\xFE" + UTF8toUTF16(copybox.text());
                copyrighttxt.close();
                if (!std::filesystem::exists(xtd::ustring::format("{}/{}/temp/romfs/settings/copyright.txt", ProgramDir, resourcesPath).c_str())) {
                    xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}/{}/temp/romfs/settings/copyright.txt", FailedToCreateFile, ProgramDir, resourcesPath), xtd::ustring::format("{} {}", ErrorText, FailedToFindPath), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                    builder.cancel_async();
                    return;
                }
            }
        }
        builder.report_progress(15);
        //copy moflex
        {
            unsigned char Checker[4];
            for (int i = 0; i < (mode.selected_index() ? rows : 1); i++) {
                if (!std::filesystem::exists(text_box_array.at(i * columns + 1)->text().c_str())) {
                    xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}, {} 2\n{} \"{}\"", row, i + 1, column, FailedToFindPath, text_box_array.at(i * columns + 1)->text()), xtd::ustring::format("{} {}", ErrorText, FailedToFindPath), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                    builder.cancel_async();
                    return;
                }
                std::string extension = text_box_array.at(i * columns + 1)->text();
                extension.erase(extension.begin(), extension.end() - 7);
                std::ifstream inmoflex(text_box_array.at(i * columns + 1)->text().c_str(), std::ios_base::in | std::ios::binary);
                for (int j = 0; j < 4; j++) {
                    inmoflex >> Checker[j];//https://stackoverflow.com/a/2974735
                    if (extension != ".moflex" || Checker[j] != moflexMagic[j]) {
                        xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}, {} 2\n\"{}\" {}", row, i + 1, column, text_box_array.at(i * columns + 1)->text(), MoflexError), xtd::ustring::format("{} {}", ErrorText, BadValue), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                        builder.cancel_async();
                        return;
                    }
                }
                std::filesystem::create_directories(xtd::ustring::format("{}/{}/temp/romfs/movie", ProgramDir, resourcesPath).c_str());
                if (mode.selected_index()) {
                    copyfile(text_box_array.at(i * columns + 1)->text().c_str(), xtd::ustring::format("{}/{}/temp/romfs/movie/movie_{}.moflex", ProgramDir, resourcesPath, i).c_str());
                    if (!std::filesystem::exists(xtd::ustring::format("{}/{}/temp/romfs/movie/movie_{}.moflex", ProgramDir, resourcesPath, i).c_str())) {//this probably only happens if there's no disk space
                        xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}/{}/temp/romfs/movie/movie_{}.moflex", FailedToCreateFile, ProgramDir, resourcesPath, i), xtd::ustring::format("{} {}", ErrorText, FailedToFindPath), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                        builder.cancel_async();
                        return;
                    }
                }
                else {
                    copyfile(text_box_array.at(i * columns + 1)->text().c_str(), xtd::ustring::format("{}/{}/temp/romfs/movie/movie.moflex", ProgramDir, resourcesPath).c_str());
                    if (!std::filesystem::exists(xtd::ustring::format("{}/{}/temp/romfs/movie/movie.moflex", ProgramDir, resourcesPath).c_str())) {//this probably only happens if there's no disk space
                        xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}/{}/temp/romfs/movie/movie.moflex", FailedToCreateFile, ProgramDir, resourcesPath), xtd::ustring::format("{} {}", ErrorText, FailedToFindPath), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                        builder.cancel_async();
                        return;
                    }
                }
            }
        }
        builder.report_progress(35);
        //convert to bimg (multi vid only)
        if (mode.selected_index()) {
            for (int i = 0; i < rows; i++) {
                unsigned char bimg[65568];
                if (!std::filesystem::exists(text_box_array.at(i * columns + 3)->text().c_str())) {
                    xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}, {} 4\n{} \"{}\"", row, i + 1, column, FailedToFindPath, text_box_array.at(i * columns + 3)->text()), xtd::ustring::format("{} {}", ErrorText, FailedToFindPath), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                    builder.cancel_async();
                    return;
                }
                if (convertToBimg(text_box_array.at(i * columns + 3)->text(), bimg, true)) {
                    std::filesystem::create_directories(xtd::ustring::format("{}/{}/temp/romfs/movie", ProgramDir, resourcesPath).c_str());
                    std::ofstream bimgfile(xtd::ustring::format("{}/{}/temp/romfs/movie/movie_{}.bimg", ProgramDir, resourcesPath, i).c_str(), std::ios_base::out | std::ios_base::binary);
                    bimgfile.write(reinterpret_cast<const char*>(bimg), sizeof(bimg));
                    bimgfile.close();
                }
                else {
                    if (!std::filesystem::exists(xtd::ustring::format("{}/{}/temp/romfs/movie/movie_{}.bimg", ProgramDir, resourcesPath, i).c_str())) {
                        xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}/{}/temp/romfs/movie/movie_{}.bimg", FailedToCreateFile, ProgramDir, resourcesPath, i), xtd::ustring::format("{} {}", ErrorText, FailedToFindPath), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                        builder.cancel_async();
                        return;
                    }
                }
            }
        }
        builder.report_progress(50);
        //do exefs (icon and banner)
        {
            if (!convertToIcon(iconbox.text(), xtd::ustring::format("{}/{}/temp/exefs/icon.bin", ProgramDir, resourcesPath).c_str(), UTF8toUTF16(shortname.text()), UTF8toUTF16(longname.text()), UTF8toUTF16(publisher.text()))) {
                xtd::forms::message_box::show(*this, xtd::ustring::format("{} \"{}\"", FailedToConvertImage, iconbox.text()), xtd::ustring::format("{} {}", ErrorText, BadValue), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                builder.cancel_async();
                return;
            }
            if (mode.selected_index()) {//multi vid needs an icon here so that it can make ext data or something (the game crashes if it isnt here)
                copyfile(xtd::ustring::format("{}/{}/temp/exefs/icon.bin", ProgramDir, resourcesPath).c_str(), xtd::ustring::format("{}/{}/temp/romfs/icon.icn", ProgramDir, resourcesPath).c_str());
                if (!std::filesystem::exists(xtd::ustring::format("{}/{}/temp/romfs/icon.icn", ProgramDir, resourcesPath).c_str())) {
                    xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}/{}/temp/romfs/icon.icn", FailedToCreateFile, ProgramDir, resourcesPath), xtd::ustring::format("{} {}", ErrorText, FailedToFindPath), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                    builder.cancel_async();
                    return;
                }
            }
            builder.report_progress(60);

            //make banner
            unsigned char Checker[4];
            bool CGFX = false;
            std::ifstream inbanner(bannerbox.text(), std::ios::binary);
            if (std::filesystem::exists(bannerbox.text().c_str())) {
                for (int i = 0; i < 4; i++) {
                    inbanner >> Checker[i];//https://stackoverflow.com/a/2974735
                    if (Checker[i] == bannerMagic[i]) {
                        bannerpreview.image(empty(0, 0));
                        customnotif.show();
                        CGFX = true;
                    }
                    else {
                        customnotif.hide();
                        CGFX = false;
                        break;
                    }
                }
            }
            if (CGFX) {
                copyfile(bannerbox.text(), xtd::ustring::format("{}/{}/temp/exefs/banner.bin", ProgramDir, resourcesPath).c_str());
            }
            else if (!CGFX) {
                unsigned char buffer[65536];
                if (!convertToBimg(bannerbox.text(), buffer, false)) {
                    xtd::forms::message_box::show(*this, xtd::ustring::format("{} \"{}\"", FailedToConvertImage, bannerbox.text()), xtd::ustring::format("{} {}", ErrorText, BadValue), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                    builder.cancel_async();
                    return;
                }

                //create bcmdl
                unsigned char* bcmdl;
                bcmdl = (unsigned char*)malloc(sizeof(bannerheader) + sizeof(buffer) + sizeof(bannerfooter));
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

                std::ofstream bnrfile(xtd::ustring::format("{}/{}/temp/exefs/banner.bin", ProgramDir, resourcesPath).c_str(), std::ios_base::out | std::ios_base::binary);
                bnrfile.write(reinterpret_cast<const char*>(bnr), bnrSize);
                bnrfile.close();
            }
            if (!std::filesystem::exists(xtd::ustring::format("{}/{}/temp/exefs/banner.bin", ProgramDir, resourcesPath).c_str())) {
                xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}/{}/temp/exefs/banner.bin", FailedToCreateFile, ProgramDir, resourcesPath), xtd::ustring::format("{} {}", ErrorText, FailedToFindPath), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                builder.cancel_async();
                return;
            }
        }
        //build CIA

    };

    builder.progress_changed += [&](object& sender, const xtd::forms::progress_changed_event_args& e) {
        majorBar.value(e.progress_percentage());
    };

    builder.run_worker_completed += [&] {
        majorBar.value(0);
        minorBar.maximum(0);
        minorBar.minimum(0);
        cancelBuildButt.enabled(false);
    };

    //settings
    settings.parent(tab_control);
    settings.text(OptionsText);

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
    savebutt.size({ settings.width() - 10, 35 });
    savebutt.location({ (settings.width() - savebutt.width()) / 2, (settings.height() / 3) - savebutt.height()});
    savebutt.click += [&] {
        parampath = save_file(ParamFilesList, DefaultParamFile);
        xtd::forms::dialog_result res = xtd::forms::dialog_result::yes;
        if (std::filesystem::exists(parampath.c_str()))
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
    loadbutt.size({ settings.width() - 10, savebutt.height() });
    loadbutt.location({ (settings.width() - loadbutt.width()) / 2, savebutt.location().y() + savebutt.height()});//tether to save button
    loadbutt.click += [&] {
        parampath = load_file(ParamFilesList);
        loadParameters();
    };

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
    for (const auto& entry : std::filesystem::directory_iterator(xtd::ustring::format("{}/{}/language", ProgramDir, resourcesPath).c_str())) {
        if (std::filesystem::exists(entry)) {
            xtd::ustring entrystr = entry.path().string();//https://stackoverflow.com/q/45401822
            xtd::ustring outstr = "";
            removeQuotes(entrystr);
            if (std::filesystem::is_directory(entry)) {
                if (fileParse(outstr, xtd::ustring::format("{}/Language.txt", entrystr).c_str(), inLangLanguage)) {
                    LanguageChoiche.items().push_back(outstr);
                    LanguageVec.push_back(entrystr.substr(entrystr.find_last_of("/\\") + 1));
                }
                else xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}\n{}.", FailedToFindVar, inLangLanguage, ValueNoChange), xtd::ustring::format("{} {}", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
            }
        }
        else xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}", FailedToFindPath, entry), xtd::ustring::format("{} {}", ErrorText, BadValue), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
    }
    for (int i = 0; i < LanguageVec.size(); i++) {
        if(LanguageVec.at(i) == Languagedir) {
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
    version.location({ (settings.width() - version.width()) / 2, settings.height() - version.height()});

    mode.selected_index_changed += [&](object& sender, const xtd::event_args& e) {
        mode.selected_index(as<xtd::forms::choice&>(sender).selected_index());
        copycheck.enabled(mode.selected_index());
        copybox.enabled(mode.selected_index() && copycheck.checked());
        menutitletxt.enabled(mode.selected_index());
        menubannertxt.enabled(mode.selected_index());
        for (int i = 2; i <= rows * columns - 1; i++)
            text_box_array.at(i)->enabled(mode.selected_index());
        if (mode.selected_index()) menubannerpreview.cursor(xtd::forms::cursors::hand());
        else menubannerpreview.cursor(xtd::forms::cursors::no());
        bannerpreviewleft.enabled(mode.selected_index() && bannerpreviewindex != 0);
        bannerpreviewright.enabled(mode.selected_index() && bannerpreviewindex != rows - 1);
        multibannerbrowse.enabled(mode.selected_index());
        appendmedia.enabled(mode.selected_index() && rows < 27);
        removemedia.enabled(mode.selected_index() && rows > 1);
        if (mode.selected_index()) indextxt.show();
        else indextxt.hide();
        if (mode.selected_index()) rowtxt.show();
        else rowtxt.hide();
        if (autoSaveParams && loaded) saveSettings();
    };

    resize += [&] {
        //debugs.text(xtd::ustring::format("{}, {}", parameters.width(), parameters.height()));
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

        mediabox.size({ parameters.width() - 46, parameters.height() - (copybox.location().y() + copybox.height() + playertitletxt.height()) - debugs.height() });
        if (mediabox.width() < ((bannermulti.location().x() - menubannertxt.location().x()) + bannermulti.width()) * columns) {
            for (int y = 0; y < rows; y++)
                for (int x = 0; x < columns; x++)
                    text_box_array.at(y * columns + x)->width((bannermulti.location().x() - menubannertxt.location().x()) + bannermulti.width() - 4);
        }
        else {
            for (int y = 0; y < rows; y++)
                for (int x = 0; x < columns; x++)
                    text_box_array.at(y * columns + x)->width((mediabox.width() / columns) - (((copybox.location().x()) / columns) + 1) / columns - 4);
        }
        for (int y = 0; y < rows; y++)
            for (int x = 1; x < columns; x++)
                text_box_array.at(y * columns + x)->location({ text_box_array.at(0)->location().x() + (x * text_box_array.at(y * columns + x)->width()), text_box_array.at(0)->location().y() + (y * text_box_array.at(y * columns + x)->height()) });
        playertitletxt.location({ mediabox.location().x() + text_box_array.at(0)->location().x() + (text_box_array.at(0)->width() - playertitletxt.width()) / 2, copybox.location().y() + copybox.height() });
        moflextxt.location({ mediabox.location().x() + text_box_array.at(1)->location().x() + (text_box_array.at(1)->width() - moflextxt.width()) / 2, copybox.location().y() + copybox.height() });
        menutitletxt.location({ mediabox.location().x() + text_box_array.at(2)->location().x() + (text_box_array.at(2)->width() - ((titlemulti.location().x() - menutitletxt.location().x()) + titlemulti.width())) / 2, copybox.location().y() + copybox.height() });
        menubannertxt.location({ mediabox.location().x() + text_box_array.at(3)->location().x() + (text_box_array.at(3)->width() - ((bannermulti.location().x() - menubannertxt.location().x()) + bannermulti.width())) / 2, copybox.location().y() + copybox.height() });
        titlemulti.location({ menutitletxt.location().x() + menutitletxt.width() + 3, menutitletxt.location().y() + ((menutitletxt.height() - titlemulti.height()) / 2) });
        bannermulti.location({ menubannertxt.location().x() + menubannertxt.width() + 3, menubannertxt.location().y() + ((menubannertxt.height() - bannermulti.height()) / 2) });
        moflexbrowse.location({ text_box_array.at((rows - 1) * columns + 1)->location().x() + (text_box_array.at((rows - 1) * columns + 1)->width() - moflexbrowse.width()) / 2, text_box_array.at((rows - 1) * columns + 1)->location().y() + text_box_array.at((rows - 1) * columns + 1)->height() });
        multibannerbrowse.location({ text_box_array.at((rows - 1) * columns + 3)->location().x() + (text_box_array.at((rows - 1) * columns + 3)->width() - moflexbrowse.width()) / 2, text_box_array.at((rows - 1) * columns + 3)->location().y() + text_box_array.at((rows - 1) * columns + 3)->height() });
        if (parameters.width() > copybox.location().x() + copybox.width() + FFrewind.width() + menubannerpreview.width()) {
            menubannerpreview.location({ FFrewind.location().x() + FFrewind.width() + (((parameters.width() - (FFrewind.location().x() + FFrewind.width())) - menubannerpreview.width()) / 2), copycheck.location().y() + ((copycheck.height() + copybox.height() + 3) - (menubannerpreview.height() + bannerpreviewleft.height())) / 2 });
        }
        else {
            menubannerpreview.location({ FFrewind.location().x() + FFrewind.width(), copycheck.location().y() + ((copycheck.height() + copybox.height() + 3) - (menubannerpreview.height() + bannerpreviewleft.height())) / 2 });
        }
        bannerpreviewleft.location({ menubannerpreview.location().x(), menubannerpreview.location().y() + menubannerpreview.height() });
        bannerpreviewright.location({ menubannerpreview.location().x() + menubannerpreview.width() - bannerpreviewright.width(), bannerpreviewleft.location().y() });
        indextxt.location({ menubannerpreview.location().x() + ((menubannerpreview.width()) - indextxt.width()) / 2, menubannerpreview.location().y() + menubannerpreview.height() });
        removemedia.location({ ((text_box_array.at(0)->width() * 4) - (removemedia.width() + appendmedia.width() + 2)) / 2, text_box_array.at((rows - 1) * columns + (columns - 1))->location().y() + text_box_array.at((rows - 1) * columns + (columns - 1))->height() + moflexbrowse.height() + 2 });
        appendmedia.location({ removemedia.location().x() + removemedia.width() + 2, removemedia.location().y() });
        rowtxt.location({ removemedia.location().x() + ((removemedia.width() + appendmedia.width() + 2) - rowtxt.width()) / 2, removemedia.location().y() + removemedia.height() });

        minorBar.width(finalize.width() - 23);
        majorBar.width(finalize.width() - 23);

        //jank incoming
        if (finalize.height() < minorBarTxt.height() + minorBar.height() + 5 + majorBarTxt.height() + majorBar.height() + 10 + buildButt.height() + cancelBuildButt.height() + randomizeTitleID.height() + TitleIDError.height() + Applicationtxt.height() + ApplicationError.height() + ProductCodetxt.height() + 4) {
            minorBarTxt.location({ (finalize.width() - minorBarTxt.width()) / 2, randomizeTitleID.height() + TitleIDError.height() + Applicationtxt.height() + ApplicationError.height() + ProductCodetxt.height() + 4 });
            minorBar.location({ (finalize.width() - minorBar.width()) / 2, minorBarTxt.location().y() + minorBarTxt.height() });

            majorBarTxt.location({ (finalize.width() - majorBarTxt.width()) / 2, minorBar.location().y() + minorBar.height() + 5 });
            majorBar.location({ (finalize.width() - majorBar.width()) / 2, majorBarTxt.location().y() + majorBarTxt.height() });

            buildButt.location({ (finalize.width() - buildButt.width()) / 2, majorBar.location().y() + majorBar.height() + 10 });
            cancelBuildButt.location({ (finalize.width() - cancelBuildButt.width()) / 2, buildButt.location().y() + buildButt.height() });
        }
        else if (finalize.height() / 2 > minorBarTxt.height() + minorBar.height() + 5 + majorBarTxt.height() + majorBar.height() + 10 + buildButt.height() + cancelBuildButt.height()) {
            minorBarTxt.location({ (finalize.width() - minorBarTxt.width()) / 2, (finalize.height() / 2) + ((finalize.height() / 2) - (minorBarTxt.height() + minorBar.height() + 5 + majorBarTxt.height() + majorBar.height() + 10 + buildButt.height() + cancelBuildButt.height())) / 2 });
            minorBar.location({ (finalize.width() - minorBar.width()) / 2, minorBarTxt.location().y() + minorBarTxt.height() });

            majorBarTxt.location({ (finalize.width() - majorBarTxt.width()) / 2, minorBar.location().y() + minorBar.height() + 5 });
            majorBar.location({ (finalize.width() - majorBar.width()) / 2, majorBarTxt.location().y() + majorBarTxt.height() });

            buildButt.location({ (finalize.width() - buildButt.width()) / 2, majorBar.location().y() + majorBar.height() + 10 });
            cancelBuildButt.location({ (finalize.width() - cancelBuildButt.width()) / 2, buildButt.location().y() + buildButt.height() });
        }
        else {
            minorBarTxt.location({ (finalize.width() - minorBarTxt.width()) / 2, finalize.height() - (minorBarTxt.height() + minorBar.height() + 5 + majorBarTxt.height() + majorBar.height() + 10 + buildButt.height() + cancelBuildButt.height()) });
            minorBar.location({ (finalize.width() - minorBar.width()) / 2, minorBarTxt.location().y() + minorBarTxt.height() });

            majorBarTxt.location({ (finalize.width() - majorBarTxt.width()) / 2, minorBar.location().y() + minorBar.height() + 5 });
            majorBar.location({ (finalize.width() - majorBar.width()) / 2, majorBarTxt.location().y() + majorBarTxt.height() });

            buildButt.location({ (finalize.width() - buildButt.width()) / 2, majorBar.location().y() + majorBar.height() + 10 });
            cancelBuildButt.location({ (finalize.width() - cancelBuildButt.width()) / 2, buildButt.location().y() + buildButt.height() });
        }

        if (minorBarTxt.location().y() > randomizeProductCode.location().y() + randomizeProductCode.height()) {
            titleIDtxt.location({ (finalize.width() - (titleIDtxt.width() + titleIDbox.width() + ZeroZero.width() + randomizeTitleID.width())) / 2, ((finalize.height() / 2) - (randomizeTitleID.height() + TitleIDError.height() + Applicationtxt.height() + ApplicationError.height() + ProductCodetxt.height())) / 2 });
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
        }
        else {
            titleIDtxt.location({ (finalize.width() - (titleIDtxt.width() + titleIDbox.width() + ZeroZero.width() + randomizeTitleID.width())) / 2, minorBarTxt.location().y() - (randomizeTitleID.height() + TitleIDError.height() + Applicationtxt.height() + ApplicationError.height() + ProductCodetxt.height()) });
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
        }

        maintitle.location({ (settings.width() - maintitle.width()) / 2, 3 });
        subtitle.location({ (settings.width() - subtitle.width()) / 2, maintitle.height() + maintitle.location().y() });
        savebutt.size({ settings.width() - 10, 35 });
        if (settings.height() < 335) {
            savebutt.location({ (settings.width() - savebutt.width()) / 2, (335 / 4) - savebutt.height() });
        }
        else {
            savebutt.location({ (settings.width() - savebutt.width()) / 2, (settings.height() / 4) - savebutt.height() });
        }

        loadbutt.size({ settings.width() - 10, 35 });
        loadbutt.location({ (settings.width() - loadbutt.width()) / 2, savebutt.location().y() + savebutt.height() });
        AutoSave.size({ settings.width() - 10, loadbutt.height() });
        AutoSave.location({ (settings.width() - AutoSave.width()) / 2, loadbutt.location().y() + loadbutt.height() });
        AutoLoad.size({ settings.width() - 10, AutoSave.height() });
        AutoLoad.location({ (settings.width() - AutoLoad.width()) / 2, AutoSave.location().y() + AutoSave.height() });
        LightDark.size({ settings.width() - 10, loadbutt.height() });
        LightDark.location({ (settings.width() - LightDark.width()) / 2, AutoLoad.location().y() + AutoLoad.height() });
        //fontchange.location({ (settings.width() - fontchange.width()) / 2, LightDark.location().y() + LightDark.height() });
        LanguageTitle.location({ (settings.width() - LanguageTitle.width()) / 2, LightDark.location().y() + LightDark.height() });
        LanguageChoiche.location({ (settings.width() - LanguageChoiche.width()) / 2, LanguageTitle.location().y() + LanguageTitle.height() });
        restartbutt.size({ settings.width() - 10, 35 });
        restartbutt.location({ (settings.width() - restartbutt.width()) / 2, LanguageChoiche.location().y() + LanguageChoiche.height() });


        if (settings.height() < restartbutt.location().y() + restartbutt.height() + version.height() + 6) {
            version.location({ (settings.width() - version.width()) / 2, restartbutt.location().y() + restartbutt.height() + 3 });
        }
        else {
            version.location({ (settings.width() - version.width()) / 2, settings.height() - version.height() - 3 });
        }
    };

    if (autoLoadParams || LoadFromArgv) {
        loadParameters();
    }
    loaded = true;
    top_most(false);

    for (int i = 0; i < rows * columns; i++) {//because it has to have updated rows and columns at boot
        text_box_array.at(i)->text_changed += [&] {
            if (autoSaveParams && loaded) saveSettings();
        };
    }
}

void form1::main() {
    //xtd::forms::application::enable_dark_mode();
    xtd::forms::application::run(form1());
}