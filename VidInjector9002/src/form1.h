/// @file
/// @brief Contains form1 class.
#pragma once
#include <thread>
//#include <xtd/xtd>
#include <filesystem>
#include <vector>
#include <string>
#include <cstring>

#include "images.h"
#include "formatting.hpp"
#include "arrays.h"
#include "strings.hpp"//<xtd/xtd> is included in here
#include "CTR_stuff.hpp"//stb is included in here
#include "nnc/nnc/cia.h"
#include "nnc/nnc/ticket.h"
#include "nnc/nnc/swizzle.h"

/// @brief Represents the namespace that contains application objects.
namespace VidInjector9002 {
    /// @brief Represents the main form.
    class form1 : public xtd::forms::form {
    public:
        /// @brief Initializes a new instance of the form1 class.
        form1();

        /// @brief The main entry point for the application.
        static void main();

        xtd::ustring removeInvalids(xtd::ustring str) {//replace invalid characters with '_', like for file names
            xtd::ustring out;
            for (const auto& c : str) {
                if (c == '\"' || c == '\\' || c == '/' || c == ':' || c == '<' || c == '>' || c == '*' || c == '?' || c == '|') out += '_';
                else out += c;
            }
            return out;
        }

        //filter is like "All Image Files|*.bmp;*.gif;*.jpg;*.jpeg;*.png;*.tif;*.tiff;*.xpm|Bitmap Files|*.bmp|Gif Files|*.gif|Jpeg Files|*.jpg;*.jpeg|Png Files|*.png|Tiff Files|*.tif;*.tiff|xpm Files|*.xpm"
        std::vector<xtd::ustring> load_files(xtd::ustring filter, xtd::ustring old_path = "", xtd::ustring start_folder = xtd::environment::get_folder_path(xtd::environment::special_folder::desktop)) {
            xtd::forms::open_file_dialog dialog;
            std::vector<xtd::ustring> chosen;
            dialog.multiselect(true);
            dialog.initial_directory(!old_path.empty() ? xtd::io::path::get_directory_name(old_path) : start_folder);
            dialog.file_name(xtd::io::path::get_file_name(old_path));
            dialog.filter(filter);
            if (dialog.show_sheet_dialog(*this) == xtd::forms::dialog_result::ok) {
                for (auto file : dialog.file_names()) {
                    chosen.push_back(xtd::ustring::format(file));
                }
            }
            else chosen.assign(1, "");
            return chosen;
        }

        xtd::ustring load_file(xtd::ustring filter, xtd::ustring old_path = "", xtd::ustring start_folder = xtd::environment::get_folder_path(xtd::environment::special_folder::desktop)) {//input is like load_files
            xtd::forms::open_file_dialog dialog;
            dialog.initial_directory(!old_path.empty() ? xtd::io::path::get_directory_name(old_path) : start_folder);
            dialog.file_name(xtd::io::path::get_file_name(old_path));
            dialog.filter(filter);
            if (dialog.show_sheet_dialog(*this) == xtd::forms::dialog_result::ok) {
                return xtd::ustring::format(dialog.file_name());
            }
            return "";
        }

        xtd::ustring save_file(xtd::ustring filter, xtd::ustring defaultname, xtd::ustring start_folder = xtd::environment::get_folder_path(xtd::environment::special_folder::desktop)) {//input is like load_file
            xtd::forms::save_file_dialog dialog;
            dialog.initial_directory(start_folder);
            dialog.file_name(defaultname);
            dialog.filter(filter);
            if (dialog.show_sheet_dialog(*this) == xtd::forms::dialog_result::ok) {
                return xtd::ustring::format(dialog.file_name());
            }
            return "";
        }

        void on_form_closing(xtd::forms::form_closing_event_args& e) override {
            xtd::forms::dialog_result res = xtd::forms::message_box::show(*this, Losedata, ConfirmClose, xtd::forms::message_box_buttons::yes_no, xtd::forms::message_box_icon::question);
            if (res == xtd::forms::dialog_result::no) {//no close
                e.cancel(true);
            }
            else if (res == xtd::forms::dialog_result::yes) {//yes close
                if (deleteTemp) {
                    std::filesystem::remove_all(exportsPath.c_str());
                    std::filesystem::remove_all(tempPath.c_str());
                }
                &xtd::forms::application::exit_thread;//obliterate the process
            }
        };

        xtd::drawing::image empty(int width, int height) {
            auto texture = xtd::drawing::bitmap{ width, height };
            for (int y = 0; y < height; y++)
                for (int x = 0; x < width; x++)
                    texture.set_pixel(x, y, xtd::drawing::color::from_argb(0xFF, 0xFF, 0xFF));
            return texture;
        }

        xtd::drawing::bitmap pixels_to_image(uint8_t* pixels, int width, int height, int channels) {
            auto texture = xtd::drawing::bitmap{ width, height, };
            if (channels == 4) {
                for (int y = 0; y < height; y++)
                    for (int x = 0; x < width; x++)
                        texture.set_pixel(x, y, xtd::drawing::color::from_argb(pixels[(y * width + x) * channels + 3], pixels[(y * width + x) * channels], pixels[(y * width + x) * channels + 1], pixels[(y * width + x) * channels + 2]));//argb
            }
            else if (channels == 3) {
                for (int y = 0; y < height; y++)
                    for (int x = 0; x < width; x++)
                        texture.set_pixel(x, y, xtd::drawing::color::from_argb(pixels[(y * width + x) * channels], pixels[(y * width + x) * channels + 1], pixels[(y * width + x) * channels + 2]));//rgb
            }
            else if (channels == 2) {
                for (int y = 0; y < height; y++)
                    for (int x = 0; x < width; x++)
                        texture.set_pixel(x, y, xtd::drawing::color::from_argb(pixels[(y * width + x) * channels + 1], pixels[(y * width + x) * channels], pixels[(y * width + x) * channels], pixels[(y * width + x) * channels]));//a grayscale
            }
            else if (channels == 1) {
                for (int y = 0; y < height; y++)
                    for (int x = 0; x < width; x++)
                        texture.set_pixel(x, y, xtd::drawing::color::from_argb(pixels[y * width + x], pixels[y * width + x], pixels[y * width + x]));//grayscale

            }
            return texture;
        }

        void setDefaultBannerPreview(xtd::forms::picture_box& banner, xtd::forms::label* error) {
            int ch = 4;
            int out_w = 200;
            int out_h = 120;
            int film_w = 264;
            int film_h = 154;
            uint8_t* output_4c = (uint8_t*)malloc(out_w * out_h * ch);
            memset(output_4c, 0xFF, out_w * out_h * ch);

            uint8_t* output_film = (uint8_t*)malloc(film_w * film_h * 4);
            layer_pixels(output_film, film_overlay, output_4c, film_w, film_h, 4, out_w, out_h, ch, 32, 11);
            free(output_4c);

            banner.image(pixels_to_image(output_film, film_w, film_h, 4));
            if (error != nullptr) error->show();

            free(output_film);
        }

        //arg is which row to get the path from
        void setMultiBannerPreview(int y) {
            int w = 0, h = 0, ch = 0;
            int out_w = 200;
            int out_h = 120;
            int film_w = 264;
            int film_h = 154;
            if (std::filesystem::exists(text_box_array.at(y * columns + 2)->text().c_str())) {
                std::string extension = text_box_array.at(y * columns + 2)->text().c_str();
                if (extension.find_last_of(".") != std::string::npos)
                    extension.erase(extension.begin(), extension.begin() + extension.find_last_of("."));
                if (extension == ".bimg") {
                    if (std::filesystem::file_size(text_box_array.at(y * columns + 2)->text().c_str()) == 0x10020) {
                        w = 256;
                        h = 128;
                        int ich = sizeof(nnc_u16);
                        int och = sizeof(nnc_u32);
                        std::ifstream input;
                        input.open(text_box_array.at(y * columns + 2)->text().c_str(), std::ios_base::in | std::ios_base::binary);
                        uint8_t* input_data = (uint8_t*)malloc((w * h * ich) + 0x20);
                        char Byte;
                        int it = 0;
                        input.read(&Byte, 1);//grab first byte of data
                        while (input) {//continue until input stream fails
                            input_data[it] = Byte;
                            input.read(&Byte, 1);//grab next byte of file
                            it++;
                        }
                        input.close();
                        for (int i = 0; i < 0x1C; i++) {
                            if (input_data[i] != bimgheader[i]) {
                                free(input_data);
                                setDefaultBannerPreview(menubannerpreview, nullptr);
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
                        layer_pixels(output_film, film_overlay, output_cropped, film_w, film_h, 4, out_w, out_h, 4, 32, 11);
                        free(output_cropped);
                        menubannerpreview.image(pixels_to_image(output_film, film_w, film_h, 4));
                        free(output_film);
                    }
                    else {
                        setDefaultBannerPreview(menubannerpreview, nullptr);
                    }
                }
                else if (stbi_info(text_box_array.at(y * columns + 2)->text().c_str(), &w, &h, &ch)) {
                    uint8_t* input_pixels = stbi_load(text_box_array.at(y * columns + 2)->text().c_str(), &w, &h, &ch, 0);
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
                    layer_pixels(output_film, film_overlay, output_4c, film_w, film_h, 4, out_w, out_h, 4, 32, 11);
                    free(output_4c);
                    menubannerpreview.image(pixels_to_image(output_film, film_w, film_h, 4));
                    free(output_film);
                }
                else {
                    setDefaultBannerPreview(menubannerpreview, nullptr);
                }
            }
            else {
                setDefaultBannerPreview(menubannerpreview, nullptr);
            }
            bannerpreviewindex = ((y & 0xFF) > 26 ? 26 : (y & 0xFF));//this should never be greater than 26 but whatever
            indextxt.text(xtd::ustring::format("{}/{}", bannerpreviewindex + 1, rows));
        }

        //set DoText to false if you dont want the text from the smdh to be loaded
        void SetIconPreview(bool DoText = true) {
            int w = 0, h = 0, comp = 0, ch = 0;
            uint16_t largeWH = 48;
            uint8_t* input_pixels;
            uint8_t* output_pixels;
            uint8_t* large_3c;
            //const int smallLW = 24;
            const uint8_t FF = 0xFF;
            bool smdhinput = true;

            while(smdhinput)
            {
                nnc_smdh smdh;
                nnc_file f;

                if (nnc_file_open(&f, iconbox.text().c_str()) != NNC_R_OK) {
                    smdhinput = false;
                    break;
                }

                if (nnc_read_smdh(NNC_RSP(&f), &smdh) != NNC_R_OK) {
                    smdhinput = false;
                }

                if (smdhinput) {

                    if (DoText) {
                        for (int i = 0; i < NNC_SMDH_TITLES; i++) {
                            shortname.text(to_UTF8(smdh.titles[i].short_desc, sizeof(smdh.titles[i].short_desc) / 2));
                            longname.text(to_UTF8(smdh.titles[i].long_desc, sizeof(smdh.titles[i].long_desc) / 2));
                            publisher.text(to_UTF8(smdh.titles[i].publisher, sizeof(smdh.titles[i].publisher) / 2));
                            if (!shortname.text().empty() && !longname.text().empty() && !publisher.text().empty())
                                break;
                        }
                        borderMode = 0;//if we just wanted the image, this would not be wanted
                    }

                    ch = 4;
                    output_pixels = (uint8_t*)malloc(largeWH * largeWH * ch);
                    nnc_unswizzle_zorder_le_rgb565_to_be_rgba8(reinterpret_cast<nnc_u16*>(smdh.icon_large), reinterpret_cast<nnc_u32*>(output_pixels), largeWH, largeWH);
                }
                NNC_RS_CALL0(f, close);
                break;
            }

            if (!smdhinput) {
                if (!stbi_info(iconbox.text().c_str(), &w, &h, &comp)) {
                    w = largeWH;
                    h = largeWH;
                    ch = 4;
                    input_pixels = (uint8_t*)malloc(largeWH * largeWH * ch);
                    memset(input_pixels, FF, largeWH * largeWH * ch);
                }
                else input_pixels = stbi_load(iconbox.text().c_str(), &w, &h, &ch, 0);
                output_pixels = (uint8_t*)malloc(largeWH * largeWH * ch);
                if (w == largeWH && h == largeWH) memcpy(output_pixels, input_pixels, w * h * ch);
                else resize_crop(input_pixels, w, h, output_pixels, largeWH, largeWH, ch);//scale to 48x48 if needed
                stbi_image_free(input_pixels);//basically the same as free() but whatever
            }
            if (borderMode == 1) {//under border
                uint8_t* output_4c = (uint8_t*)malloc(largeWH * largeWH * 4);
                uint8_t* white_background = (uint8_t*)malloc(largeWH * largeWH * 4);//fix the bugs by not fixing the bugs! :D
                memset(white_background, FF, largeWH * largeWH * 4);
                layer_pixels(output_4c, output_pixels, white_background, largeWH, largeWH, ch, largeWH, largeWH, 4, 0, 0);//it warns about output_pixels being potentially uninitialized but that is impossible
                free(white_background);
                layer_pixels(output_4c, icon_border, output_4c, largeWH, largeWH, 4, largeWH, largeWH, 4, 0, 0);
                ch = 4;
                free(output_pixels);
                output_pixels = (uint8_t*)malloc(largeWH * largeWH * ch);
                memcpy(output_pixels, output_4c, largeWH * largeWH * ch);
                free(output_4c);
            }
            else if (borderMode == 2) {//inside border
                uint8_t* output_4c = (uint8_t*)malloc(largeWH * largeWH * 4);
                uint8_t* white_background = (uint8_t*)malloc(largeWH * largeWH * 4);//fix the bugs by not fixing the bugs! :D
                memset(white_background, FF, largeWH * largeWH * 4);
                layer_pixels(output_4c, output_pixels, white_background, largeWH, largeWH, ch, largeWH, largeWH, 4, 0, 0);
                free(white_background);
                ch = 4;
                uint8_t* scaled = (uint8_t*)malloc(largeWH * largeWH * ch);
                stbir_resize_uint8(output_4c, largeWH, largeWH, 0, scaled, largeWH - 10, largeWH - 10, 0, ch);//scale it down
                layer_pixels(output_4c, icon_border, scaled, largeWH, largeWH, ch, largeWH - 10, largeWH - 10, ch, 5, 5);
                free(scaled);
                free(output_pixels);
                output_pixels = (uint8_t*)malloc(largeWH * largeWH * ch);
                memcpy(output_pixels, output_4c, largeWH * largeWH * ch);
                free(output_4c);
            }

            large_3c = (uint8_t*)malloc(largeWH * largeWH * 3);
            if (ch == 4) {//rgba
                uint8_t* white_background = (uint8_t*)malloc(largeWH * largeWH * 4);
                memset(white_background, FF, largeWH * largeWH * 4);
                layer_pixels(output_pixels, output_pixels, white_background, largeWH, largeWH, ch, largeWH, largeWH, 4, 0, 0);
                free(white_background);
                int newi = 0;
                for (int i = 0; i < largeWH * largeWH * ch; i += ch) {
                    for (int c = 0; c < 3; c++)
                        large_3c[newi + c] = output_pixels[i + c];
                    newi += 3;
                }
            }
            else if (ch == 3) {//rgb
                memcpy(large_3c, output_pixels, largeWH * largeWH * ch);
            }
            else if (ch == 2) {//grayscale a
                uint8_t* white_background = (uint8_t*)malloc(largeWH * largeWH * ch);
                uint8_t* output_4c = (uint8_t*)malloc(largeWH * largeWH * 4);
                memset(white_background, FF, largeWH * largeWH * ch);
                layer_pixels(output_4c, output_pixels, white_background, largeWH, largeWH, ch, largeWH, largeWH, ch, 0, 0);
                free(white_background);
                int newi = 0;
                for (int i = 0; i < largeWH * largeWH * 4; i += 4) {
                    for (int c = 0; c < 3; c++)
                        large_3c[newi + c] = output_4c[i + c];
                    newi += 3;
                }
                free(output_4c);
            }
            else if (ch == 1) {//grayscale
                int ch1 = 0;
                for (int i = 0; i < largeWH * largeWH * 3; i += 3) {
                    for (int c = 0; c < 3; c++)
                        large_3c[i + c] = output_pixels[ch1];
                    ch1++;
                }
            }
            iconpreview.image(pixels_to_image(large_3c, largeWH, largeWH, 3));
            free(output_pixels);
            free(large_3c);
            iconerror.hide();


            if ((!std::filesystem::exists(iconbox.text().c_str()) || (!stbi_info(iconbox.text().c_str(), &w, &h, &ch) && !smdhinput)) && !iconbox.text().empty()) {
                iconerror.show();
            }
        }

        void doAppendMedia() {
            removemedia.enabled(false);
            appendmedia.enabled(false);
            if (rows < 27) {
                for (int y = rows; y <= rows; y++)
                    for (int x = 0; x < columns; x++) {
                        xtd::forms::text_box* box_new = new xtd::forms::text_box();
                        text_box_array.push_back(box_new);//put box in array
                        text_box_array.at(y * columns + x)->parent(mediabox);//assign it data
                        text_box_array.at(y * columns + x)->font(this->font());
                        text_box_array.at(y * columns + x)->cursor(xtd::forms::cursors::ibeam());
                        if (mediabox.height() < (text_box_array.at(0)->height() * rows) + moflexbrowse.height() + removemedia.height() + 2 + rowtxt.height() + 5) {
                            if (mediabox.width() < ((bannermulti.location().x() - menubannertxt.location().x()) + bannermulti.width()) * columns) {
                                text_box_array.at(y * columns + x)->width(menubannertxt.width() + bannermulti.width() + 3);
                            }
                            else {
                                text_box_array.at(y * columns + x)->width(((mediabox.width() - (mediabox.location().x()) / columns) - 20) / columns);
                            }
                        }
                        else {
                            if (mediabox.width() < ((bannermulti.location().x() - menubannertxt.location().x()) + bannermulti.width()) * columns) {
                                text_box_array.at(y * columns + x)->width(menubannertxt.width() + bannermulti.width() + 3);
                            }
                            else {
                                text_box_array.at(y * columns + x)->width(((mediabox.width() - (mediabox.location().x()) / columns)) / columns);
                            }
                        }
                        text_box_array.at(y * columns + x)->location({ text_box_array.at(0)->location().x() + (x * text_box_array.at(y * columns + x)->width()), text_box_array.at(0)->location().y() + (y * text_box_array.at(y * columns + x)->height()) });
                    }
                rows++;
            }
            for (int i = columns; i <= rows * columns - 1; i++)
                text_box_array.at(i)->enabled(mode.selected_index());
            rowtxt.text(xtd::ustring::format("{}/27", rows));
            removemedia.enabled(mode.selected_index());
            bannerpreviewleft.enabled(mode.selected_index() && bannerpreviewindex != 0);
            bannerpreviewright.enabled(mode.selected_index() && bannerpreviewindex != rows - 1);
            indextxt.text(xtd::ustring::format("{}/{}", bannerpreviewindex + 1, rows));
            removemedia.enabled(true);
            appendmedia.enabled(rows < 27);
            if (mediabox.height() < (text_box_array.at(0)->height() * rows) + moflexbrowse.height() + removemedia.height() + 2 + rowtxt.height() + 5) {//if the scroll appears, change the size of everything
                if (mediabox.width() < (menubannertxt.width() + bannermulti.width() + 3) * columns) {
                    for (int y = 0; y < rows; y++)
                        for (int x = 0; x < columns; x++)
                            text_box_array.at(y * columns + x)->width(menubannertxt.width() + bannermulti.width() + 3);
                }
                else {
                    for (int y = 0; y < rows; y++)
                        for (int x = 0; x < columns; x++)
                            text_box_array.at(y * columns + x)->width(((mediabox.width() - (mediabox.location().x()) / columns) - 20) / columns);
                }
                for (int y = 0; y < rows; y++)
                    for (int x = 1; x < columns; x++)
                        text_box_array.at(y * columns + x)->location({ text_box_array.at(0)->location().x() + (x * text_box_array.at(y * columns + x)->width()), text_box_array.at(0)->location().y() + (y * text_box_array.at(y * columns + x)->height()) });
            }
            playertitletxt.location({ mediabox.location().x() + text_box_array.at(0)->location().x() + (text_box_array.at(0)->width() - playertitletxt.width()) / 2, copybox.location().y() + copybox.height() });
            moflextxt.location({ mediabox.location().x() + text_box_array.at(1)->location().x() + (text_box_array.at(1)->width() - moflextxt.width()) / 2, copybox.location().y() + copybox.height() });
            menubannertxt.location({ mediabox.location().x() + text_box_array.at(2)->location().x() + (text_box_array.at(2)->width() - ((bannermulti.location().x() - menubannertxt.location().x()) + bannermulti.width())) / 2, copybox.location().y() + copybox.height() });
            bannermulti.location({ menubannertxt.location().x() + menubannertxt.width() + 3, menubannertxt.location().y() + ((menubannertxt.height() - bannermulti.height()) / 2) });
            moflexbrowse.location({ text_box_array.at((rows - 1) * columns + 1)->location().x() + (text_box_array.at((rows - 1) * columns + 1)->width() - moflexbrowse.width()) / 2, text_box_array.at((rows - 1) * columns + 1)->location().y() + text_box_array.at((rows - 1) * columns + 1)->height() });
            multibannerbrowse.location({ text_box_array.at((rows - 1) * columns + 2)->location().x() + (text_box_array.at((rows - 1) * columns + 2)->width() - moflexbrowse.width()) / 2, text_box_array.at((rows - 1) * columns + 2)->location().y() + text_box_array.at((rows - 1) * columns + 2)->height() });
            removemedia.location({ ((text_box_array.at(0)->width() * columns) - (removemedia.width() + appendmedia.width() + 2)) / 2, text_box_array.at((rows - 1) * columns + (columns - 1))->location().y() + text_box_array.at((rows - 1) * columns + (columns - 1))->height() + moflexbrowse.height() + 2 });
            appendmedia.location({ removemedia.location().x() + removemedia.width() + 2, removemedia.location().y() });
            rowtxt.location({ removemedia.location().x() + ((removemedia.width() + appendmedia.width() + 2) - rowtxt.width()) / 2, removemedia.location().y() + removemedia.height() });
        }

        void doRemoveMedia() {
            removemedia.enabled(false);
            appendmedia.enabled(false);
            if (rows > 1) {
                for (int x = columns; x > 0; x--) {
                    delete[] text_box_array.at(((rows - 1) * columns + x) - 1);//murder last row
                    text_box_array.pop_back();
                }
                rows--;
            }
            for (int i = columns; i <= rows * columns - 1; i++)
                text_box_array.at(i)->enabled(mode.selected_index());
            rowtxt.text(xtd::ustring::format("{}/27", rows));
            appendmedia.enabled(mode.selected_index());
            if (bannerpreviewindex == rows)//if you were previewing the removed one, change the selected index
                setMultiBannerPreview(rows - 1);
            bannerpreviewleft.enabled(mode.selected_index() && bannerpreviewindex != 0);
            bannerpreviewright.enabled(mode.selected_index() && bannerpreviewindex != rows - 1);
            indextxt.text(xtd::ustring::format("{}/{}", bannerpreviewindex + 1, rows));
            removemedia.enabled(rows > 1);
            appendmedia.enabled(true);
            if (mediabox.height() < (text_box_array.at(0)->height() * rows) + moflexbrowse.height() + removemedia.height() + 2 + rowtxt.height() + 5) {
                if (mediabox.width() < (menubannertxt.width() + bannermulti.width() + 3) * columns) {
                    for (int y = 0; y < rows; y++)
                        for (int x = 0; x < columns; x++)
                            text_box_array.at(y * columns + x)->width(menubannertxt.width() + bannermulti.width() + 3);
                }
                else {
                    for (int y = 0; y < rows; y++)
                        for (int x = 0; x < columns; x++)
                            text_box_array.at(y * columns + x)->width(((mediabox.width() - (mediabox.location().x()) / columns) - 20) / columns);
                }
            }
            else {//do this here because if you are removing it can remove the scroll
                if (mediabox.width() < (menubannertxt.width() + bannermulti.width() + 3) * columns) {
                    for (int y = 0; y < rows; y++)
                        for (int x = 0; x < columns; x++)
                            text_box_array.at(y * columns + x)->width(menubannertxt.width() + bannermulti.width() + 3);
                }
                else {
                    for (int y = 0; y < rows; y++)
                        for (int x = 0; x < columns; x++)
                            text_box_array.at(y * columns + x)->width(((mediabox.width() - (mediabox.location().x()) / columns)) / columns);
                }
            }
            for (int y = 0; y < rows; y++)
                for (int x = 1; x < columns; x++)
                    text_box_array.at(y * columns + x)->location({ text_box_array.at(0)->location().x() + (x * text_box_array.at(y * columns + x)->width()), text_box_array.at(0)->location().y() + (y * text_box_array.at(y * columns + x)->height()) });
            playertitletxt.location({ mediabox.location().x() + text_box_array.at(0)->location().x() + (text_box_array.at(0)->width() - playertitletxt.width()) / 2, copybox.location().y() + copybox.height() });
            moflextxt.location({ mediabox.location().x() + text_box_array.at(1)->location().x() + (text_box_array.at(1)->width() - moflextxt.width()) / 2, copybox.location().y() + copybox.height() });
            menubannertxt.location({ mediabox.location().x() + text_box_array.at(2)->location().x() + (text_box_array.at(2)->width() - ((bannermulti.location().x() - menubannertxt.location().x()) + bannermulti.width())) / 2, copybox.location().y() + copybox.height() });
            bannermulti.location({ menubannertxt.location().x() + menubannertxt.width() + 3, menubannertxt.location().y() + ((menubannertxt.height() - bannermulti.height()) / 2) });
            moflexbrowse.location({ text_box_array.at((rows - 1) * columns + 1)->location().x() + (text_box_array.at((rows - 1) * columns + 1)->width() - moflexbrowse.width()) / 2, text_box_array.at((rows - 1) * columns + 1)->location().y() + text_box_array.at((rows - 1) * columns + 1)->height() });
            multibannerbrowse.location({ text_box_array.at((rows - 1) * columns + 2)->location().x() + (text_box_array.at((rows - 1) * columns + 2)->width() - moflexbrowse.width()) / 2, text_box_array.at((rows - 1) * columns + 2)->location().y() + text_box_array.at((rows - 1) * columns + 2)->height() });
            removemedia.location({ ((text_box_array.at(0)->width() * columns) - (removemedia.width() + appendmedia.width() + 2)) / 2, text_box_array.at((rows - 1) * columns + (columns - 1))->location().y() + text_box_array.at((rows - 1) * columns + (columns - 1))->height() + moflexbrowse.height() + 2 });
            appendmedia.location({ removemedia.location().x() + removemedia.width() + 2, removemedia.location().y() });
            rowtxt.location({ removemedia.location().x() + ((removemedia.width() + appendmedia.width() + 2) - rowtxt.width()) / 2, removemedia.location().y() + removemedia.height() });
            bannermulti.hide();//so it doesnt have the text messing it up when the stuff moves horizontally
            bannermulti.show();
        }

        void encode_bigend_u64(uint64_t value, void* dest)//https://stackoverflow.com/a/36552262
        {
            value =
                ((value & 0xFF00000000000000u) >> 56u) |
                ((value & 0x00FF000000000000u) >> 40u) |
                ((value & 0x0000FF0000000000u) >> 24u) |
                ((value & 0x000000FF00000000u) >> 8u) |
                ((value & 0x00000000FF000000u) << 8u) |
                ((value & 0x0000000000FF0000u) << 24u) |
                ((value & 0x000000000000FF00u) << 40u) |
                ((value & 0x00000000000000FFu) << 56u);
            memcpy(dest, &value, sizeof(uint64_t));
        }

        /*do this to your file data
        out: pointer to output data
        inpath: path of parameters file
        query: variable to take from inpath e.g. STRING:VER
        return: true if it's success, false if it's not found*/
        bool fileParse(xtd::ustring& out, xtd::ustring inpath, std::string query) {
            if (!std::filesystem::exists(inpath.c_str())) return false;
            std::vector<xtd::ustring> filelines;
            std::ifstream infile(inpath, std::ios_base::in | std::ios_base::binary);
            char Byte;
            xtd::ustring line = "";
            //split file into the vector
            infile.read(&Byte, 1);//grab first byte of file
            while (infile) {//continue until input stream fails
                line += Byte;//append byte to string
                size_t first = 0;
                size_t second = 0;
                first = line.find_first_of("\"");
                if (first > line.size()) first = 0;//if find_first_of fails, this flings in the first numeral
                if (first) second = line.substr(first).find_last_of("\"");
                if (line.substr(first).substr(second) == "\"\n") {//put the line and clear the string for the next line
                    filelines.push_back(line);
                    line = "";
                }
                //printf("%1X\n", Byte);
                infile.read(&Byte, 1);//grab next byte of file
            }
            for (auto& str : filelines) {
                if (str.substr(0, str.find_first_of("=")) == query) {
                    size_t first = 0;
                    size_t second = 0;
                    first = str.find_first_of("\"") + 1;
                    if (first > str.size()) first = 0;
                    if (first) second = str.substr(first).find_last_of("\"");
                    out = str.substr(first).substr(0, second);
                    return true;
                }
            }
            return false;
        }

        void saveParameters() {
            std::ofstream outparams(parampath, std::ios_base::out | std::ios_base::binary);
            outparams <<
                StrVerParam << "=\"" << VI9PVER << "\"\n" <<
                IntMultiParam << "=\"" << std::to_string(mode.selected_index()) << "\"\n" <<
                StrBannerParam << "=\"" << bannerbox.text() << "\"\n" <<
                StrIconParam << "=\"" << iconbox.text() << "\"\n" <<
                IntIconBorderParam << "=\"" << borderMode << "\"\n" <<
                StrSNameParam << "=\"" << shortname.text() << "\"\n" <<
                StrLNameParam << "=\"" << longname.text() << "\"\n" <<
                StrPublisherParam << "=\"" << publisher.text() << "\"\n" <<
                IntCopycheckParam << "=\"" << std::to_string(copycheck.checked()) << "\"\n" <<
                StrCopyrightParam << "=\"" << copybox.text() << "\"\n" <<
                IntFFrewindParam << "=\"" << std::to_string(FFrewind.checked()) << "\"\n" <<
                IntFadeOptParam << "=\"" << std::to_string(FadeOpt.checked()) << "\"\n" <<
                IntRowsParam << "=\"" << std::to_string(rows) << "\"\n";
            for (int y = 0; y < rows; y++) {
                outparams <<
                    StrPTitleParam << "(" << std::to_string(y) << ")=\"" << text_box_array.at(y * columns + 0)->text() << "\"\n" <<
                    StrMoflexParam << "(" << std::to_string(y) << ")=\"" << text_box_array.at(y * columns + 1)->text() << "\"\n" <<
                    //StrMTitleParam << "(" << std::to_string(y) << ")=\"" << text_box_array.at(y * columns + 2)->text() << "\"\n" <<
                    StrMBannerParam << "(" << std::to_string(y) << ")=\"" << text_box_array.at(y * columns + 2)->text() << "\"\n";
            }
            outparams <<
                IntPreIndexParam << "=\"" << std::to_string(bannerpreviewindex) << "\"\n";
            outparams.close();
        }

        bool loadParameters() {
            loaded = false;//do this so stuff this calls doesnt try to save the parameters while it's loading
            //xtd::forms::message_box::show(*this, xtd::ustring::format("{}", parampath), FormText, xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::warning);
            xtd::ustring outstr = "";
            uint32_t outrealint = 0;
            bool good = true;
            if (parampath == "") {
                //xtd::forms::message_box::show(*this, xtd::ustring::format("{} ({}) {}", TheFile, parampath, DoesntExist), xtd::ustring::format("{} {}", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                return false;
            }
            if (fileParse(outstr, parampath, StrVerParam)) {
                if (outstr != VI9PVER) {
                    xtd::forms::message_box::show(*this, xtd::ustring::format("{} ({})\n{} {}", BadVersion, outstr, SupportedVersion, VI9PVER), xtd::ustring::format("{} {}", ErrorText, BadValue), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                    return false;
                }
            }
            else {
                good = false;
                xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}", FailedToFindVar, StrVerParam), xtd::ustring::format("{} {}", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                return false;
            }
            if (fileParse(outstr, parampath, IntMultiParam)) {
                if (!stoul_s(outrealint, outstr)) {
                    xtd::forms::message_box::show(*this, xtd::ustring::format("{} ({})\n{} {}", BadValue, outstr, IntMultiParam, BeANumber), xtd::ustring::format("{} {}", ErrorText, BadValue), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                    outrealint = 0;
                    good = false;
                }
                mode.selected_index(outrealint);
            }
            else {
                good = false;
                xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}\n{}.", FailedToFindVar, IntMultiParam, ValueNoChange), xtd::ustring::format("{} {}", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
            }
            if (fileParse(outstr, parampath, StrBannerParam)) {
                bannerbox.text(outstr);
            }
            else {
                good = false;
                xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}\n{}.", FailedToFindVar, StrBannerParam, ValueNoChange), xtd::ustring::format("{} {}", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
            }
            if (fileParse(outstr, parampath, StrIconParam)) {
                iconbox.text(outstr);
            }
            else {
                good = false;
                xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}\n{}.", FailedToFindVar, StrIconParam, ValueNoChange), xtd::ustring::format("{} {}", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
            }
            if (fileParse(outstr, parampath, IntIconBorderParam)) {
                if (!stoul_s(outrealint, outstr)) {
                    outrealint = 2;
                }
                borderMode = ((outrealint & 0xFF) > 2 ? 2 : (outrealint & 0xFF));
            }
            else {
                good = false;
                xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}\n{}.", FailedToFindVar, IntIconBorderParam, ValueNoChange), xtd::ustring::format("{} {}", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
            }
            if (fileParse(outstr, parampath, StrSNameParam)) {
                shortname.text(outstr);
            }
            else {
                good = false;
                xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}\n{}.", FailedToFindVar, StrSNameParam, ValueNoChange), xtd::ustring::format("{} {}", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
            }
            if (fileParse(outstr, parampath, StrLNameParam)) {
                longname.text(outstr);
            }
            else {
                good = false;
                xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}\n{}.", FailedToFindVar, StrLNameParam, ValueNoChange), xtd::ustring::format("{} {}", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
            }
            if (fileParse(outstr, parampath, StrPublisherParam)) {
                publisher.text(outstr);
            }
            else {
                good = false;
                xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}\n{}.", FailedToFindVar, StrPublisherParam, ValueNoChange), xtd::ustring::format("{} {}", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
            }
            if (fileParse(outstr, parampath, IntCopycheckParam)) {
                if (!stoul_s(outrealint, outstr)) {
                    xtd::forms::message_box::show(*this, xtd::ustring::format("{} ({})\n{} {}", BadValue, outstr, IntCopycheckParam, BeANumber), xtd::ustring::format("{} {}", ErrorText, BadValue), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                    outrealint = 0;
                    good = false;
                }
                copycheck.checked(outrealint);
            }
            else {
                good = false;
                xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}\n{}.", FailedToFindVar, IntCopycheckParam, ValueNoChange), xtd::ustring::format("{} {}", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
            }
            if (fileParse(outstr, parampath, StrCopyrightParam)) {
                copybox.text(outstr);
            }
            else {
                good = false;
                xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}\n{}.", FailedToFindVar, StrCopyrightParam, ValueNoChange), xtd::ustring::format("{} {}", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
            }
            if (fileParse(outstr, parampath, IntFFrewindParam)) {
                if (!stoul_s(outrealint, outstr)) {
                    xtd::forms::message_box::show(*this, xtd::ustring::format("{} ({})\n{} {}", BadValue, outstr, IntFFrewindParam, BeANumber), xtd::ustring::format("{} {}", ErrorText, BadValue), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                    outrealint = 0;
                    good = false;
                }
                FFrewind.checked(outrealint);
            }
            else {
                good = false;
                xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}\n{}.", FailedToFindVar, IntFFrewindParam, ValueNoChange), xtd::ustring::format("{} {}", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
            }
            if (fileParse(outstr, parampath, IntFadeOptParam)) {
                if (!stoul_s(outrealint, outstr)) {
                    xtd::forms::message_box::show(*this, xtd::ustring::format("{} ({})\n{} {}", BadValue, outstr, IntFadeOptParam, BeANumber), xtd::ustring::format("{} {}", ErrorText, BadValue), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                    outrealint = 0;
                    good = false;
                }
                FadeOpt.checked(outrealint);
            }
            else {
                good = false;
                xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}\n{}.", FailedToFindVar, IntFadeOptParam, ValueNoChange), xtd::ustring::format("{} {}", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
            }
            if (fileParse(outstr, parampath, IntRowsParam)) {
                if (!stoul_s(outrealint, outstr)) {
                    xtd::forms::message_box::show(*this, xtd::ustring::format("{} ({})\n{} {}", BadValue, outstr, IntRowsParam, BeANumber), xtd::ustring::format("{} {}", ErrorText, BadValue), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                    outrealint = 0;
                    good = false;
                }
                if (outrealint > 27) {
                    xtd::forms::message_box::show(*this, xtd::ustring::format("{} ({})\n{}.", BadValue, outstr, noMoreThan27), xtd::ustring::format("{} {}", ErrorText, BadValue), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                    outrealint -= outrealint - 27;
                    good = false;
                }
                if (outrealint < rows) {
                    uint32_t oldrows = rows;
                    for (uint32_t i = 0; i < oldrows - outrealint; i++) {
                        doRemoveMedia();
                    }
                }
                else if (outrealint > rows) {
                    uint32_t oldrows = rows;
                    for (uint32_t i = 0; i < outrealint - oldrows; i++) {
                        doAppendMedia();
                    }
                }
                rows = ((outrealint & 0xFF) > 27 ? 27 : (outrealint & 0xFF));
            }
            else {
                good = false;
                xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}\n{}.", FailedToFindVar, IntRowsParam, ValueNoChange), xtd::ustring::format("{} {}", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
            }
            for (int y = 0; y < rows; y++) {
                if (fileParse(outstr, parampath, xtd::ustring::format("{}({})", StrPTitleParam, y))) {
                    text_box_array.at(y * columns + 0)->text(outstr);
                }
                else {
                    good = false;
                    xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}({})\n{}.", FailedToFindVar, StrPTitleParam, y, ValueNoChange), xtd::ustring::format("{} {}", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                }
                if (fileParse(outstr, parampath, xtd::ustring::format("{}({})", StrMoflexParam, y))) {
                    text_box_array.at(y * columns + 1)->text(outstr);
                }
                else {
                    good = false;
                    xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}({})\n{}.", FailedToFindVar, StrMoflexParam, y, ValueNoChange), xtd::ustring::format("{} {}", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                }
                if (fileParse(outstr, parampath, xtd::ustring::format("{}({})", StrMBannerParam, y))) {
                    text_box_array.at(y * columns + 2)->text(outstr);
                }
                else {
                    good = false;
                    xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}({})\n{}.", FailedToFindVar, StrMBannerParam, y, ValueNoChange), xtd::ustring::format("{} {}", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                }
            }
            if (fileParse(outstr, parampath, IntPreIndexParam)) {
                if (!stoul_s(outrealint, outstr)) {
                    xtd::forms::message_box::show(*this, xtd::ustring::format("{} ({})\n{} {}", BadValue, outstr, IntPreIndexParam, BeANumber), xtd::ustring::format("{} {}", ErrorText, BadValue), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                    outrealint = 0;
                    good = false;
                }
                bannerpreviewindex = ((outrealint & 0xFF) > 26 ? 26 : (outrealint & 0xFF));
                setMultiBannerPreview(bannerpreviewindex);
                bannerpreviewleft.enabled(mode.selected_index() && bannerpreviewindex != 0);
                bannerpreviewright.enabled(mode.selected_index() && bannerpreviewindex != rows - 1);
            }
            else {
                good = false;
                xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}\n{}.", FailedToFindVar, IntPreIndexParam, ValueNoChange), xtd::ustring::format("{} {}", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
            }
            xtd::forms::message_box::show(*this, xtd::ustring::format(good ? "{} {}{}" : "{} {}.\n{}", good ? SuccessfullyLoaded : FailedToLoad, parampath.substr(parampath.find_last_of("/\\") + 1), good ? "." : ValidStillLoaded), ParametersLoaded, xtd::forms::message_box_buttons::ok, good ? xtd::forms::message_box_icon::information : xtd::forms::message_box_icon::warning);
            return good;
        }

        bool loadLanguage(xtd::ustring Lang) {
            xtd::ustring outstr = "";
            if (!std::filesystem::exists(xtd::ustring::format("{}/{}/language/{}/Language.txt", ProgramDir, resourcesPath, Lang).c_str())) {
                std::filesystem::create_directories(xtd::ustring::format("{}/{}/language/English", ProgramDir, resourcesPath).c_str());
                std::ofstream M_file(xtd::ustring::format("{}/{}/language/English/M.bmp", ProgramDir, resourcesPath).c_str(), std::ios_base::out | std::ios_base::binary);
                M_file.write(reinterpret_cast<const char*>(M_bmp), sizeof(M_bmp));
                M_file.close();
                xtd::forms::message_box::show(*this, xtd::ustring::format("{} \"{}\"", FailedToFindPath, xtd::ustring::format("{}/{}/language/{}/Language.txt", ProgramDir, resourcesPath, Lang)), xtd::ustring::format("{} {}", ErrorText, BadValue), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                return false;
            }
            std::vector<xtd::ustring*> inLangVec = { &inLangLanguage, &inLangFormText, &inLangByMeText, &inLangParametersText, &inLangFinalizeText, &inLangOptionsText, &inLangModeText, &inLangSingleVideo, &inLangMultiVideo, &inLangBannerText, &inLangBrowse, &inLangSupportedImage48x48, &inLangSupportedImage200x120, &inLangSupportedImageListBanner, &inLangSupportedImageList, &inLangCGFXList, &inLangSMDHList, &inLangAllFilesList, &inLangMoflexFilesList, &inLangParamFilesList, &inLangErrorText, &inLangImageInfoError, &inLangBannerPreviewText, &inLangCustomNotifText, &inLangIconText, &inLangShortNameText, &inLangLongNameText, &inLangPublisherText, &inLangTextTooLongError, &inLangMultiOnlyText, &inLangCopyrightCheckText, &inLangFFrewindText, &inLangFadeOptText, &inLangPlayerTitleText, &inLangMoflexFileText, &inLangMenuBannerText, &inLangSaveButtText, &inLangLoadButtText, &inLangLoadCIAButtText, &inLangAutoSaveText, &inLangAutoLoadText, &inLangDeleteTempFiles, &inLangLightModeText, &inLangDarkModeText, &inLangLanguageText, &inLangRestartText, &inLangConfirmClose, &inLangLosedata, &inLangConfirmSave, &inLangAlreadyExists, &inLangReplaceIt, &inLangBadValue, &inLangBadVersion, &inLangSupportedVersion, &inLangBeANumber, &inLangMissingVariableError, &inLangFailedToFindVar, &inLangFailedToFindPath, &inLangFailedToFindFile, &inLangFailedToReadFile, &inLangFailedToCreateFile, &inLangFailedToConvertImage, &inLangValueNoChange, &inLangnoMoreThan27, &inLangParametersLoaded, &inLangSuccessfullyLoaded, &inLangFailedToLoad, &inLangValidStillLoaded, &inLangTheFile, &inLangDoesntExist, &inLangLanguageChanged, &inLangRestartProgram, &inLangWasEnabled, &inLangTitleIDText, &inLangAppNameText, &inLangProductCodetext, &inLangBuildCIAText, &inLangCancel, &inLangMoflexError, &inLangrow, &inLangcolumn, &inLangCreatingFile, &inLangCopyingMoflex, &inLangCiaFiles, &inLangCiaBuilt, };
            std::vector<xtd::ustring*> LangVec = { &Language, &FormText, &ByMeText, &ParametersText, &FinalizeText, &OptionsText, &ModeText, &SingleVideo, &MultiVideo, &BannerText, &Browse, &SupportedImage48x48, &SupportedImage200x120, &SupportedImageListBanner, &SupportedImageList, &CGFXList, &SMDHList, &AllFilesList, &MoflexFilesList, &ParamFilesList, &ErrorText, &ImageInfoError, &BannerPreviewText, &CustomNotifText, &IconText, &ShortNameText, &LongNameText, &PublisherText, &TextTooLongError, &MultiOnlyText, &CopyrightCheckText, &FFrewindText, &FadeOptText, &PlayerTitleText, &MoflexFileText, &MenuBannerText, &SaveButtText, &LoadButtText, &LoadCIAButtText, &AutoSaveText, &AutoLoadText, &DeleteTempFiles, &LightModeText, &DarkModeText, &LanguageText, &RestartText, &ConfirmClose, &Losedata, &ConfirmSave, &AlreadyExists, &ReplaceIt, &BadValue, &BadVersion, &SupportedVersion, &BeANumber, &MissingVariableError, &FailedToFindVar, &FailedToFindPath, &FailedToFindFile, &FailedToReadFile, &FailedToCreateFile, &FailedToConvertImage, &ValueNoChange, &noMoreThan27, &ParametersLoaded, &SuccessfullyLoaded, &FailedToLoad, &ValidStillLoaded, &TheFile, &DoesntExist, &LanguageChanged, &RestartProgram, &WasEnabled, &TitleIDText, &AppNameText, &ProductCodetext, &BuildCIAText, &Cancel, &MoflexError, &row, &column, &CreatingFile, &CopyingMoflex, &CiaFiles, &CiaBuilt, };
            for (size_t i = 0; i < inLangVec.size(); i++) {
                if (fileParse(outstr, xtd::ustring::format("{}/{}/language/{}/Language.txt", ProgramDir, resourcesPath, Lang), *inLangVec[i])) {
                    *LangVec[i] = outstr;
                }
                else {
                    xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}\n{}.", FailedToFindVar, *inLangVec[i], ValueNoChange), xtd::ustring::format("{} {}", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                }
            }
            Languagedir = Lang;
            return true;
        }

        void saveSettings() {
            if (autoSaveParams && saveParametersNow) {
                if (!std::filesystem::exists(parampath.c_str())) parampath = xtd::ustring::format("{}/{}/{}", ProgramDir, resourcesPath, DefaultParamFile);
                saveParameters();
            }
            std::ofstream settingsfile(xtd::ustring::format("{}/{}/{}", ProgramDir, resourcesPath, settingsPath).c_str(), std::ios_base::out | std::ios_base::binary);
            settingsfile <<
                StrDefaultLanguage << "=\"" << Languagedir << "\"\n" <<
                IntAutoSaveParams << "=\"" << autoSaveParams << "\"\n" <<
                IntAutoLoadParams << "=\"" << autoLoadParams << "\"\n" <<
                IntDeleteTempFiles << "=\"" << deleteTemp << "\"\n" <<
                IntDarkMode << "=\"" << LightDark.checked() << "\"\n" <<
                StrParamsPath << "=\"" << parampath << "\"\n";
            settingsfile.close();
        }

        bool loadSettings() {
            xtd::ustring outstr = "";
            uint32_t outrealint = 0;
            bool good = true;
            if (!std::filesystem::exists(xtd::ustring::format("{}/{}/{}", ProgramDir, resourcesPath, settingsPath).c_str())) {
                std::filesystem::create_directories(xtd::ustring::format("{}/{}", ProgramDir, resourcesPath).c_str());
                std::ofstream settingsfile(xtd::ustring::format("{}/{}/{}", ProgramDir, resourcesPath, settingsPath).c_str(), std::ios_base::out | std::ios_base::binary);
                settingsfile <<
                    StrDefaultLanguage << "=\"English\"\n" <<
                    IntAutoSaveParams << "=\"0\"\n" <<
                    IntAutoLoadParams << "=\"0\"\n" <<
                    IntDeleteTempFiles << "=\"1\"\n" <<
                    IntDarkMode << "=\"0\"\n" <<
                    StrParamsPath << "=\"\"\n";
                settingsfile.close();
            }
            if (fileParse(outstr, xtd::ustring::format("{}/{}/{}", ProgramDir, resourcesPath, settingsPath).c_str(), StrDefaultLanguage)) {
                if (!loadLanguage(outstr)) {
                    loadLanguage("English");
                    good = false;
                }
            }
            else {
                good = false;
                xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}\n{}.", FailedToFindVar, StrDefaultLanguage, ValueNoChange), xtd::ustring::format("{} {}", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
            }
            if (fileParse(outstr, xtd::ustring::format("{}/{}/{}", ProgramDir, resourcesPath, settingsPath).c_str(), IntAutoSaveParams)) {
                if (!stoul_s(outrealint, outstr)) {
                    xtd::forms::message_box::show(*this, xtd::ustring::format("{} ({})\n{} {}", BadValue, outstr, IntAutoSaveParams, BeANumber), xtd::ustring::format("{} {}", ErrorText, BadValue), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                    outrealint = 0;
                    good = false;
                }
                autoSaveParams = outrealint;
            }
            else {
                good = false;
                xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}\n{}.", FailedToFindVar, IntAutoSaveParams, ValueNoChange), xtd::ustring::format("{} {}", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
            }
            if (fileParse(outstr, xtd::ustring::format("{}/{}/{}", ProgramDir, resourcesPath, settingsPath).c_str(), IntAutoLoadParams)) {
                if (!stoul_s(outrealint, outstr)) {
                    xtd::forms::message_box::show(*this, xtd::ustring::format("{} ({})\n{} {}", BadValue, outstr, IntAutoLoadParams, BeANumber), xtd::ustring::format("{} {}", ErrorText, BadValue), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                    outrealint = 0;
                    good = false;
                }
                autoLoadParams = outrealint;
            }
            else {
                good = false;
                xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}\n{}.", FailedToFindVar, IntAutoLoadParams, ValueNoChange), xtd::ustring::format("{} {}", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
            }
            if (fileParse(outstr, xtd::ustring::format("{}/{}/{}", ProgramDir, resourcesPath, settingsPath).c_str(), IntDeleteTempFiles)) {
                if (!stoul_s(outrealint, outstr)) {
                    xtd::forms::message_box::show(*this, xtd::ustring::format("{} ({})\n{} {}", BadValue, outstr, IntDeleteTempFiles, BeANumber), xtd::ustring::format("{} {}", ErrorText, BadValue), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                    outrealint = 0;
                    good = false;
                }
                deleteTemp = outrealint;
            }
            else {
                good = false;
                xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}\n{}.", FailedToFindVar, IntDeleteTempFiles, ValueNoChange), xtd::ustring::format("{} {}", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
            }
            if (fileParse(outstr, xtd::ustring::format("{}/{}/{}", ProgramDir, resourcesPath, settingsPath).c_str(), IntDarkMode)) {
                if (!stoul_s(outrealint, outstr)) {
                    xtd::forms::message_box::show(*this, xtd::ustring::format("{} ({})\n{} {}", BadValue, outstr, IntDarkMode, BeANumber), xtd::ustring::format("{} {}", ErrorText, BadValue), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                    outrealint = 0;
                    good = false;
                }
                LightDark.checked(outrealint);
                //make all the containers black or white
                this->fore_color(LightDark.checked() ? xtd::drawing::color::white : xtd::drawing::color::black);
                this->back_color(LightDark.checked() ? xtd::drawing::color::black : xtd::drawing::color::white);
                //tab_control.fore_color(fore_color());
                //tab_control.back_color(back_color());//these dont work idk why
                parameters.fore_color(fore_color());
                parameters.back_color(back_color());
                finalize.fore_color(fore_color());
                finalize.back_color(back_color());
                settings.fore_color(fore_color());
                settings.back_color(back_color());
            }
            else {
                good = false;
                xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}\n{}.", FailedToFindVar, IntDarkMode, ValueNoChange), xtd::ustring::format("{} {}", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
            }
            if (fileParse(outstr, xtd::ustring::format("{}/{}/{}", ProgramDir, resourcesPath, settingsPath).c_str(), StrParamsPath)) {
                parampath = outstr;
            }
            else {
                good = false;
                xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}\n{}.", FailedToFindVar, StrParamsPath, ValueNoChange), xtd::ustring::format("{} {}", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
            }
            return good;
        }

        void ableObjects(bool able) {//true for enable all modifying buttons and stuff, false for disable all
            mode.enabled(able);
            bannerbox.enabled(able);
            bannerbrowse.enabled(able);
            iconbox.enabled(able);
            iconbrowse.enabled(able);
            iconpreview.enabled(able);
            shortname.enabled(able);
            longname.enabled(able);
            publisher.enabled(able);
            copycheck.enabled(able ? mode.selected_index() : able);
            copybox.enabled(able ? (mode.selected_index() && copycheck.checked()) : able);
            FFrewind.enabled(able);
            FadeOpt.enabled(able);
            for (int i = 0; i < 2; i++) {
                text_box_array.at(i)->enabled(able);
            }
            for (int i = 2; i <= rows * columns - 1; i++)
                text_box_array.at(i)->enabled(able ? mode.selected_index() : able);
            moflexbrowse.enabled(able);
            multibannerbrowse.enabled(able ? mode.selected_index() : able);
            menubannerpreview.enabled(able);
            appendmedia.enabled(rows < 27 && (able ? mode.selected_index() : able));
            removemedia.enabled(rows > 1 && (able ? mode.selected_index() : able));
            titleIDbox.enabled(able);
            randomizeTitleID.enabled(able);
            ApplicationName.enabled(able);
            ProductCode.enabled(able);
            randomizeProductCode.enabled(able);
            buildButt.enabled(able);
            loadbutt.enabled(able);
            loadCIAbutt.enabled(able);
        }

        //objects
        std::vector<xtd::ustring> argv = xtd::environment::get_command_line_args();
        xtd::ustring ProgramDir = xtd::ustring::format("{}", argv.at(0).substr(0, argv.at(0).find_last_of("\\/")));
        xtd::ustring tempPath = xtd::ustring::format("{}/{}/temp", ProgramDir, resourcesPath);
        xtd::ustring exportsPath = xtd::ustring::format("{}/{}/exports", ProgramDir, resourcesPath);
        std::vector<xtd::ustring> LanguageVec;

        bool loaded = false;
        xtd::ustring parampath;
        bool saveParametersNow = true;//spaghetti mode: activate
        bool LoadFromArgv = false;
        bool autoSaveParams = false;
        bool autoLoadParams = false;
        bool deleteTemp = true;
        uint8_t borderMode = 2;
        xtd::forms::font_dialog font_dialog;
        xtd::forms::label MultiOnly;

        xtd::forms::tab_control tab_control;
        xtd::forms::tab_page parameters;
        xtd::forms::tab_page finalize;
        xtd::forms::tab_page settings;

        xtd::forms::picture_box Logo;

        xtd::forms::label modetxt;
        xtd::forms::choice mode;

        xtd::forms::label bannertxt;
        xtd::forms::text_box bannerbox;
        xtd::forms::button bannerbrowse;
        xtd::forms::picture_box bannerpreview;
        xtd::forms::label bannerpreviewtxt;
        xtd::forms::label debugs;
        xtd::forms::label bannererror;
        xtd::forms::label customnotif;

        xtd::forms::label icontxt;
        xtd::forms::text_box iconbox;
        xtd::forms::button iconbrowse;
        xtd::forms::picture_box iconpreview;
        xtd::forms::label iconerror;

        xtd::forms::label shortnametxt;
        xtd::forms::text_box shortname;
        xtd::forms::label shorterror;
        xtd::forms::label longnametxt;
        xtd::forms::text_box longname;
        xtd::forms::label longerror;
        xtd::forms::label publishertxt;
        xtd::forms::text_box publisher;
        xtd::forms::label publishererror;

        xtd::forms::check_box copycheck;
        xtd::forms::text_box copybox;
        xtd::forms::picture_box copymulti;

        xtd::forms::check_box FFrewind;
        xtd::forms::check_box FadeOpt;

        xtd::forms::panel mediabox;
        uint8_t columns = 3;
        uint8_t rows = 1;
        xtd::forms::label playertitletxt;
        xtd::forms::label moflextxt;
        xtd::forms::label menubannertxt;
        xtd::forms::picture_box bannermulti;

        std::vector<xtd::forms::text_box*> text_box_array;
        xtd::forms::button moflexbrowse;
        xtd::forms::button multibannerbrowse;
        xtd::forms::picture_box menubannerpreview;

        uint8_t bannerpreviewindex = 0;//to show which row to preview (0-26)
        xtd::forms::label indextxt;
        xtd::forms::button bannerpreviewleft;
        xtd::forms::button bannerpreviewright;

        xtd::forms::button appendmedia;
        xtd::forms::button removemedia;
        xtd::forms::label rowtxt;

        //finalize
        xtd::forms::label titleIDtxt;
        xtd::forms::text_box titleIDbox;
        xtd::forms::label TitleIDError;
        xtd::forms::label ZeroZero;
        xtd::forms::button randomizeTitleID;

        xtd::forms::label Applicationtxt;
        xtd::forms::text_box ApplicationName;
        xtd::forms::label ApplicationError;

        xtd::forms::label ProductCodetxt;
        xtd::forms::text_box ProductCode;
        xtd::forms::label ProductCodeError;
        xtd::forms::button randomizeProductCode;

        xtd::forms::label minorBarTxt;
        xtd::forms::progress_bar minorBar;

        xtd::forms::label majorBarTxt;
        xtd::forms::progress_bar majorBar;

        xtd::forms::button buildButt;
        xtd::forms::button cancelBuildButt;

        xtd::forms::background_worker builder;
        xtd::forms::background_worker extractor;

        //settings
        xtd::forms::label maintitle;
        xtd::forms::label subtitle;
        xtd::forms::button savebutt;
        xtd::forms::button loadbutt;
        xtd::forms::button loadCIAbutt;
        xtd::forms::check_box AutoSave;
        xtd::forms::check_box AutoLoad;
        xtd::forms::check_box DeleteTempButt;
        xtd::forms::check_box LightDark;
        //xtd::forms::button fontchange;
        xtd::forms::label LanguageTitle;
        xtd::forms::choice LanguageChoiche;
        xtd::forms::button restartbutt;
        xtd::forms::label version;
        xtd::forms::settings Settings;

        uint8_t* banner_image;
        uint8_t* large_icon;
        uint8_t* small_icon;

    };
}