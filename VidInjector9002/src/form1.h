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

/// @brief Represents the namespace that contains application objects.
namespace VidInjector9002 {
  /// @brief Represents the main form.
  class form1 : public xtd::forms::form {
  public:
        /// @brief Initializes a new instance of the form1 class.
        form1();

        /// @brief The main entry point for the application.
        static void main();

        std::vector<xtd::ustring> load_files(xtd::ustring filter) {//input is like "All Image Files|*.bmp;*.gif;*.jpg;*.jpeg;*.png;*.tif;*.tiff;*.xpm|Bitmap Files|*.bmp|Gif Files|*.gif|Jpeg Files|*.jpg;*.jpeg|Png Files|*.png|Tiff Files|*.tif;*.tiff|xpm Files|*.xpm"
            xtd::forms::open_file_dialog dialog;
            std::vector<xtd::ustring> chosen;
            xtd::ustring file_names;
            dialog.multiselect(true);
            dialog.initial_directory(!file_names.empty() ? xtd::io::path::get_directory_name(file_names) : xtd::environment::get_folder_path(xtd::environment::special_folder::desktop));
            dialog.file_name(xtd::io::path::get_file_name(file_names));
            dialog.filter(filter);
            if (dialog.show_dialog() == xtd::forms::dialog_result::ok) {
                for (auto file : dialog.file_names()) {
                    file_names += "\"" + file + "\" ";
                    chosen.push_back(xtd::ustring::format(file_names));
                }
            }
            else chosen.assign(1, "");
            return chosen;
        }

        xtd::ustring load_file(xtd::ustring filter) {//input is like load_file
            xtd::forms::open_file_dialog dialog;
            xtd::ustring file_name;
            dialog.initial_directory(!file_name.empty() ? xtd::io::path::get_directory_name(file_name) : xtd::environment::get_folder_path(xtd::environment::special_folder::desktop));
            dialog.file_name(xtd::io::path::get_file_name(file_name));
            dialog.filter(filter);
            if (dialog.show_sheet_dialog(*this) == xtd::forms::dialog_result::ok) {
                file_name = dialog.file_name();
                return xtd::ustring::format(file_name);
            }
            return "";
        }

        xtd::ustring save_file(xtd::ustring filter, xtd::ustring defaultname) {//input is like load_file
            xtd::forms::save_file_dialog dialog;
            dialog.initial_directory(xtd::environment::get_folder_path(xtd::environment::special_folder::desktop));
            dialog.file_name(defaultname);
            dialog.filter(filter);
            if (dialog.show_sheet_dialog(*this) == xtd::forms::dialog_result::ok) {
                return xtd::ustring::format(dialog.file_name());
            }
            return "";
        }

        void on_form_closing(xtd::forms::form_closing_event_args& e) override {
            e.cancel(xtd::forms::message_box::show(*this, Losedata, ConfirmClose, xtd::forms::message_box_buttons::yes_no, xtd::forms::message_box_icon::question) == xtd::forms::dialog_result::no);
        };

        xtd::drawing::image empty(int width, int height) {
            auto texture = xtd::drawing::bitmap{ width, height };
            for (int y = 0; y < height; y++)
                for (int x = 0; x < width; x++)
                    texture.set_pixel(x, y, xtd::drawing::color::from_argb(0xFF, 0xFF, 0xFF));
            return texture;
        }

        xtd::drawing::bitmap pixels_to_image(unsigned char* pixels, int width, int height, int channels) {
            auto texture = xtd::drawing::bitmap{ width, height, };
            for (int y = 0; y < height; y++)
                for (int x = 0; x < width; x++) {
                    if (channels == 4) {
                        texture.set_pixel(x, y, xtd::drawing::color::from_argb(pixels[(y* width + x) * channels + 3], pixels[(y * width + x) * channels], pixels[(y * width + x) * channels + 1], pixels[(y * width + x) * channels + 2]));//argb
                    }
                    else if(channels == 3)
                        texture.set_pixel(x, y, xtd::drawing::color::from_argb(pixels[(y * width + x) * channels], pixels[(y * width + x) * channels + 1], pixels[(y * width + x) * channels + 2]));//rgb
                }
            return texture;
        }

        //arg is which row to get the path from
        void setMultiBannerPreview(int y) {
            int w, h, ch = 0;
            int out_w = 200;
            int out_h = 120;
            int film_w = 264;
            int film_h = 154;
            if (std::filesystem::exists(text_box_array.at(y * columns + 3)->text().c_str()) && stbi_info(text_box_array.at(y * columns + 3)->text().c_str(), &w, &h, &ch)) {
                unsigned char* input_pixels = stbi_load(text_box_array.at(y * columns + 3)->text().c_str(), &w, &h, &ch, 0);
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
                menubannerpreview.image(pixels_to_image(output_film, film_w, film_h, 4));
                free(output_film);
            }
            else {
                ch = 4;
                unsigned char* output_4c = (unsigned char*)malloc(out_w * out_h * ch);
                memset(output_4c, 0xFF, out_w * out_h * ch);

                unsigned char* output_film = (unsigned char*)malloc(film_w * film_h * 4);
                layer_pixels(output_film, film_overlay, output_4c, film_w, film_h, 4, out_w, out_h, ch, 32, 11);
                free(output_4c);

                menubannerpreview.image(pixels_to_image(output_film, film_w, film_h, 4));

                free(output_film);
            }
            bannerpreviewindex = y;
            indextxt.text(xtd::ustring::format("{}/{}", bannerpreviewindex + 1, rows));
        }

        void doAppendMedia() {
            if (rows < 27) {
                for (int y = rows; y <= rows; y++)
                    for (int x = 0; x < columns; x++) {
                        xtd::forms::text_box* box_new = new xtd::forms::text_box();
                        text_box_array.push_back(box_new);//put box in array
                        text_box_array.at(y * columns + x)->parent(mediabox);//assign it data
                        text_box_array.at(y * columns + x)->font(this->font());
                        text_box_array.at(y * columns + x)->cursor(xtd::forms::cursors::ibeam());
                        if (mediabox.width() < ((bannermulti.location().x() - menubannertxt.location().x()) + bannermulti.width()) * columns) text_box_array.at(y * columns + x)->width((bannermulti.location().x() - menubannertxt.location().x()) + bannermulti.width());
                        else text_box_array.at(y * columns + x)->width((mediabox.width() / columns) - (((copybox.location().x()) / columns) + 1) / columns - 4);
                        text_box_array.at(y * columns + x)->location({ text_box_array.at(0)->location().x() + (x * text_box_array.at(y * columns + x)->width()), text_box_array.at(0)->location().y() + (y * text_box_array.at(y * columns + x)->height()) });
                    }
                rows++;
                multibannerbrowse.location({ text_box_array.at((rows - 1) * columns + 3)->location().x() + (text_box_array.at((rows - 1) * columns + 3)->width() - moflexbrowse.width()) / 2, text_box_array.at((rows - 1) * columns + 3)->location().y() + text_box_array.at((rows - 1) * columns + 3)->height() });
                moflexbrowse.location({ text_box_array.at((rows - 1) * columns + 1)->location().x() + (text_box_array.at((rows - 1) * columns + 1)->width() - moflexbrowse.width()) / 2, text_box_array.at((rows - 1) * columns + 1)->location().y() + text_box_array.at((rows - 1) * columns + 1)->height() });
                appendmedia.location({ ((text_box_array.at(0)->width() * 4) - (appendmedia.width() + removemedia.width() + 2)) / 2, text_box_array.at((rows - 1) * columns + (columns - 1))->location().y() + text_box_array.at((rows - 1) * columns + (columns - 1))->height() + moflexbrowse.height() + 2 });
                removemedia.location({ appendmedia.location().x() + appendmedia.width() + 2, appendmedia.location().y() });
            }
            if (rows == 27) appendmedia.enabled(false);
            for (int i = columns; i <= rows * columns - 1; i++)
                text_box_array.at(i)->enabled(mode.selected_index());
            rowtxt.location({ appendmedia.location().x() + ((appendmedia.width() + removemedia.width() + 2) - rowtxt.width()) / 2, appendmedia.location().y() + appendmedia.height() });
            rowtxt.text(xtd::ustring::format("{}/27", rows));
            removemedia.enabled(mode.selected_index());
            bannerpreviewleft.enabled(mode.selected_index() && bannerpreviewindex != 0);
            bannerpreviewright.enabled(mode.selected_index() && bannerpreviewindex != rows - 1);
            indextxt.text(xtd::ustring::format("{}/{}", bannerpreviewindex + 1, rows));
        }

        void doRemoveMedia() {
            if (rows > 1) {
                for (int x = columns; x > 0; x--) {
                    delete[] text_box_array.at(((rows - 1) * columns + x) - 1);//murder last row
                    text_box_array.pop_back();
                }
                rows--;
                multibannerbrowse.location({ text_box_array.at((rows - 1) * columns + 3)->location().x() + (text_box_array.at((rows - 1) * columns + 3)->width() - moflexbrowse.width()) / 2, text_box_array.at((rows - 1) * columns + 3)->location().y() + text_box_array.at((rows - 1) * columns + 3)->height() });
                moflexbrowse.location({ text_box_array.at((rows - 1) * columns + 1)->location().x() + (text_box_array.at((rows - 1) * columns + 1)->width() - moflexbrowse.width()) / 2, text_box_array.at((rows - 1) * columns + 1)->location().y() + text_box_array.at((rows - 1) * columns + 1)->height() });
                appendmedia.location({ ((text_box_array.at(0)->width() * 4) - (appendmedia.width() + removemedia.width() + 2)) / 2, text_box_array.at((rows - 1) * columns + (columns - 1))->location().y() + text_box_array.at((rows - 1) * columns + (columns - 1))->height() + moflexbrowse.height() + 2 });
                removemedia.location({ appendmedia.location().x() + appendmedia.width() + 2, appendmedia.location().y() });
            }
            if (rows == 1) removemedia.enabled(false);
            for (int i = columns; i <= rows * columns - 1; i++)
                text_box_array.at(i)->enabled(mode.selected_index());
            rowtxt.location({ appendmedia.location().x() + ((appendmedia.width() + removemedia.width() + 2) - rowtxt.width()) / 2, appendmedia.location().y() + appendmedia.height() });
            rowtxt.text(xtd::ustring::format("{}/27", rows));
            appendmedia.enabled(mode.selected_index());
            if (bannerpreviewindex == rows)//if you were previewing the removed one, change the selected index
                setMultiBannerPreview(rows - 1);
            bannerpreviewleft.enabled(mode.selected_index() && bannerpreviewindex != 0);
            bannerpreviewright.enabled(mode.selected_index() && bannerpreviewindex != rows - 1);
            indextxt.text(xtd::ustring::format("{}/{}", bannerpreviewindex + 1, rows));
        }

        /*do this to your file data
        out: pointer to output data
        inpath: path of parameters file
        query: variable to take from in e.g. STRING:VER
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
                StrVerParam << "=\"" << VI9P300 << "\"\n" <<
                IntMultiParam << "=\"" << std::to_string(mode.selected_index()) << "\"\n" <<
                StrBannerParam << "=\"" << bannerbox.text() << "\"\n" <<
                StrIconParam << "=\"" << iconbox.text() << "\"\n" <<
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
                    StrMTitleParam << "(" << std::to_string(y) << ")=\"" << text_box_array.at(y * columns + 2)->text() << "\"\n" <<
                    StrMBannerParam << "(" << std::to_string(y) << ")=\"" << text_box_array.at(y * columns + 3)->text() << "\"\n";
            }
            outparams <<
                IntPreIndexParam << "=\"" << std::to_string(bannerpreviewindex) << "\"\n";
            outparams.close();
        }

        bool loadParameters() {
            //xtd::forms::message_box::show(*this, xtd::ustring::format("{}", parampath), FormText, xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::warning);
            xtd::ustring outstr = "";
            unsigned long outrealint = 0;
            bool good = true;
            if (parampath == "") {
                //xtd::forms::message_box::show(*this, xtd::ustring::format("{} ({}) {}", TheFile, parampath, DoesntExist), xtd::ustring::format("{} {}", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                return false;
            }
            if (fileParse(outstr, parampath, StrVerParam)) {
                if (outstr != VI9P300) {
                    xtd::forms::message_box::show(*this, xtd::ustring::format("{} ({})\n{} {}", BadVersion, outstr, SupportedVersion, VI9P300), xtd::ustring::format("{} {}", ErrorText, BadValue), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
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
                    unsigned long oldrows = rows;
                    for (unsigned long i = 0; i < oldrows - outrealint; i++) {
                        doRemoveMedia();
                    }
                }
                else if (outrealint > rows) {
                    unsigned long oldrows = rows;
                    for (unsigned long i = 0; i < outrealint - oldrows; i++) {
                        doAppendMedia();
                    }
                }
                rows = outrealint;
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
                if (fileParse(outstr, parampath, xtd::ustring::format("{}({})", StrMTitleParam, y))) {
                    text_box_array.at(y * columns + 2)->text(outstr);
                }
                else {
                    good = false;
                    xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}({})\n{}.", FailedToFindVar, StrMTitleParam, y, ValueNoChange), xtd::ustring::format("{} {}", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                }
                if (fileParse(outstr, parampath, xtd::ustring::format("{}({})", StrMBannerParam, y))) {
                    text_box_array.at(y * columns + 3)->text(outstr);
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
                bannerpreviewindex = outrealint;
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
                std::filesystem::create_directory(xtd::ustring::format("{}/{}/language/English", ProgramDir, resourcesPath).c_str());
                std::ofstream M_file(xtd::ustring::format("{}/{}/language/English/M.bmp", ProgramDir, resourcesPath).c_str(), std::ios_base::out | std::ios_base::binary);
                M_file.write(reinterpret_cast<const char*>(M_bmp), sizeof(M_bmp));
                M_file.close();
                xtd::forms::message_box::show(*this, xtd::ustring::format("{} \"{}\"", FailedToFindPath, xtd::ustring::format("{}/{}/language/{}/Language.txt", ProgramDir, resourcesPath, Lang)), xtd::ustring::format("{} {}", ErrorText, BadValue), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
                return false;
            }
            std::vector<xtd::ustring*> inLangVec = { &inLangLanguage, &inLangFormText, &inLangByMeText, &inLangParametersText, &inLangFinalizeText, &inLangOptionsText, &inLangModeText, &inLangSingleVideo, &inLangMultiVideo, &inLangBannerText, &inLangBrowse, &inLangSupportedImage48x48, &inLangSupportedImage200x120, &inLangSupportedImageList, &inLangCGFXList, &inLangAllFilesList, &inLangMoflexFilesList, &inLangParamFilesList, &inLangErrorText, &inLangImageInfoError, &inLangBannerPreviewText, &inLangCustomNotifText, &inLangIconText, &inLangShortNameText, &inLangLongNameText, &inLangPublisherText, &inLangTextTooLongError, &inLangMultiOnlyText, &inLangCopyrightCheckText, &inLangFFrewindText, &inLangFadeOptText, &inLangPlayerTitleText, &inLangMoflexFileText, &inLangMenuTitleText, &inLangMenuBannerText, &inLangSaveButtText, &inLangLoadButtText, &inLangAutoSaveText, &inLangAutoLoadText, &inLangLightModeText, &inLangDarkModeText, &inLangLanguageText, &inLangRestartText, &inLangConfirmClose, &inLangLosedata, &inLangConfirmSave, &inLangAlreadyExists, &inLangReplaceIt, &inLangBadValue, &inLangBadVersion, &inLangSupportedVersion, &inLangBeANumber, &inLangMissingVariableError, &inLangFailedToFindVar, &inLangFailedToFindPath, &inLangFailedToCreateFile, &inLangFailedToConvertImage, &inLangValueNoChange, &inLangnoMoreThan27, &inLangParametersLoaded, &inLangSuccessfullyLoaded, &inLangFailedToLoad, &inLangValidStillLoaded, &inLangTheFile, &inLangDoesntExist, &inLangLanguageChanged, &inLangRestartProgram, &inLangWasEnabled, &inLangTitleIDText, &inLangAppNameText, &inLangProductCodetext, &inLangBuildCIAText, &inLangCancel, &inLangMoflexError, };
            std::vector<xtd::ustring*> LangVec = { &Language, &FormText, &ByMeText, &ParametersText, &FinalizeText, &OptionsText, &ModeText, &SingleVideo, &MultiVideo, &BannerText, &Browse, &SupportedImage48x48, &SupportedImage200x120, &SupportedImageList, &CGFXList, &AllFilesList, &MoflexFilesList, &ParamFilesList, &ErrorText, &ImageInfoError, &BannerPreviewText, &CustomNotifText, &IconText, &ShortNameText, &LongNameText, &PublisherText, &TextTooLongError, &MultiOnlyText, &CopyrightCheckText, &FFrewindText, &FadeOptText, &PlayerTitleText, &MoflexFileText, &MenuTitleText, &MenuBannerText, &SaveButtText, &LoadButtText, &AutoSaveText, &AutoLoadText, &LightModeText, &DarkModeText, &LanguageText, &RestartText, &ConfirmClose, &Losedata, &ConfirmSave, &AlreadyExists, &ReplaceIt, &BadValue, &BadVersion, &SupportedVersion, &BeANumber, &MissingVariableError, &FailedToFindVar, &FailedToFindPath, &FailedToCreateFile, &FailedToConvertImage, &ValueNoChange, &noMoreThan27, &ParametersLoaded, &SuccessfullyLoaded, &FailedToLoad, &ValidStillLoaded, &TheFile, &DoesntExist, &LanguageChanged, &RestartProgram, &WasEnabled, &TitleIDText, &AppNameText, &ProductCodetext, &BuildCIAText, &Cancel, &MoflexError, };
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
                if(!std::filesystem::exists(parampath.c_str())) parampath = xtd::ustring::format("{}/{}/{}", ProgramDir, resourcesPath, DefaultParamFile);
                saveParameters();
            }
            std::ofstream settingsfile(xtd::ustring::format("{}/{}/{}", ProgramDir, resourcesPath, settingsPath).c_str(), std::ios_base::out | std::ios_base::binary);
            settingsfile <<
                StrDefaultLanguage << "=\"" << Languagedir << "\"\n" <<
                IntAutoSaveParams << "=\"" << autoSaveParams << "\"\n" <<
                IntAutoLoadParams << "=\"" << autoLoadParams << "\"\n" <<
                IntDarkMode << "=\"" << LightDark.checked() << "\"\n" <<
                StrParamsPath << "=\"" << parampath << "\"\n";
            settingsfile.close();
        }

        bool loadSettings() {
            xtd::ustring outstr = "";
            unsigned long outrealint = 0;
            bool good = true;
            if (!std::filesystem::exists(xtd::ustring::format("{}/{}/{}", ProgramDir, resourcesPath, settingsPath).c_str())) {
                std::filesystem::create_directories(xtd::ustring::format("{}/{}", ProgramDir, resourcesPath).c_str());
                std::ofstream settingsfile(xtd::ustring::format("{}/{}/{}", ProgramDir, resourcesPath, settingsPath).c_str(), std::ios_base::out | std::ios_base::binary);
                settingsfile <<
                    StrDefaultLanguage << "=\"English\"\n" <<
                    IntAutoSaveParams << "=\"0\"\n" <<
                    IntAutoLoadParams << "=\"0\"\n" <<
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

        void copyfile(std::string inpath, std::string outpath) {//also works with directories
            if (std::filesystem::exists(outpath))
                std::filesystem::remove_all(outpath);
            if (std::filesystem::exists(inpath))
                std::filesystem::copy(inpath, outpath, std::filesystem::copy_options::recursive);
        }

        //objects
        std::vector<xtd::ustring> argv = xtd::environment::get_command_line_args();
        xtd::ustring ProgramDir = xtd::ustring::format("{}", argv.at(0).substr(0, argv.at(0).find_last_of("\\/")));
        std::vector<xtd::ustring> LanguageVec;

        bool loaded = false;
        xtd::ustring parampath;
        bool saveParametersNow = true;//spaghetti mode: activate
        bool LoadFromArgv = false;
        bool autoSaveParams = false;
        bool autoLoadParams = false;
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
        int columns = 4;
        int rows = 1;
        xtd::forms::label playertitletxt;
        xtd::forms::label menutitletxt;
        xtd::forms::picture_box titlemulti;
        xtd::forms::label moflextxt;
        xtd::forms::label menubannertxt;
        xtd::forms::picture_box bannermulti;

        std::vector<xtd::forms::text_box*> text_box_array;
        xtd::forms::button moflexbrowse;
        xtd::forms::button multibannerbrowse;
        xtd::forms::picture_box menubannerpreview;

        int bannerpreviewindex = 0;//to show which row to preview (0-26)
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
        xtd::forms::button randomizeProductCode;
        xtd::forms::text_box ProductCode;
        xtd::forms::label minorBarTxt;
        xtd::forms::progress_bar minorBar;
        xtd::forms::label majorBarTxt;
        xtd::forms::progress_bar majorBar;
        xtd::forms::button buildButt;
        xtd::forms::button cancelBuildButt;
        
        xtd::forms::background_worker builder;

        //settings
        xtd::forms::label maintitle;
        xtd::forms::label subtitle;
        xtd::forms::button savebutt;
        xtd::forms::button loadbutt;
        xtd::forms::check_box AutoSave;
        xtd::forms::check_box AutoLoad;
        xtd::forms::check_box LightDark;
        //xtd::forms::button fontchange;
        xtd::forms::label LanguageTitle;
        xtd::forms::choice LanguageChoiche;
        xtd::forms::button restartbutt;
        xtd::forms::label version;
        xtd::forms::settings Settings;

        unsigned char* banner_image;
        unsigned char* large_icon;
        unsigned char* small_icon;
  };
}