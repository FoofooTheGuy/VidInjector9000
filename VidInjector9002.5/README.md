<img src="https://github.com/user-attachments/assets/e3abe8b2-2b8e-47c4-a434-f9c8330f135b" alt="drawing" width="225"/>

# VidInjector9002.5
A GUI wrapper for VidInjector9002-CLI. Made with pure [wxWidgets](https://github.com/wxWidgets/wxWidgets) in pure code.

## Screenshots
<details>
<summary>Arch Linux (GNOME 47.2)</summary>

### System (Light Mode)
![System Light](https://github.com/user-attachments/assets/0b8cb59e-be9b-4fb9-a1ec-1081b2e8cffa)

### System (Dark Mode)
![System](https://github.com/user-attachments/assets/bb26a54e-d4eb-47fb-a76d-937ab96b7398)

### Light Mode
![Light](https://github.com/user-attachments/assets/4c2059d3-0ce8-4c34-a4ed-fdab3a1535f0)

### Dark Mode
![Dark](https://github.com/user-attachments/assets/916fa097-1a49-4a7d-8ecf-6547d53823d2)

</details>

<details>
<summary>Linux Mint (Cinnamon 6.2.7)</summary>

### System (Light Mode)
![Screenshot from 2024-12-30 11-47-50](https://github.com/user-attachments/assets/20e56092-9eba-4e45-9e4d-f28dc2767881)

### System (Dark Mode)
![Screenshot from 2024-12-30 11-46-22](https://github.com/user-attachments/assets/833b8de2-343d-4056-a024-051babc0f8f2)

### Light Mode
![Screenshot from 2024-12-30 11-46-39](https://github.com/user-attachments/assets/086e3c54-2970-4f50-abd3-1e668700d70e)

### Dark Mode
![Screenshot from 2024-12-30 11-46-53](https://github.com/user-attachments/assets/a97144a5-6440-4217-af19-a1b7cac5830b)

</details>

<details>
<summary>MacOS (Sonoma 14.3)</summary>

### System (Light Mode)
![system](https://github.com/user-attachments/assets/f53e9cca-69ff-4c09-8039-e79778682ee7)

### System (Dark Mode)
![system dark](https://github.com/user-attachments/assets/62d95d87-1d98-445c-866a-225f0317fbc5)

### Light Mode
![light](https://github.com/user-attachments/assets/20a2f9cc-ceaa-47f7-941e-28e1a4a54a73)

### Dark Mode
![dark](https://github.com/user-attachments/assets/3d5b0e4a-407f-480a-9553-8888ee9a507b)

</details>

<details>
<summary>Windows 7</summary>

### System
![system crop](https://github.com/user-attachments/assets/3d1316c0-c097-4361-84cb-92b5079a3476)

### Light Mode
![light crop](https://github.com/user-attachments/assets/a973ff88-9415-4277-b844-d4c21e4fe780)

### Dark Mode
![dark crop](https://github.com/user-attachments/assets/a7ec957c-3796-4605-9b2f-6d4cdb2c57ee)

</details>

<details>
<summary>Windows 8.1</summary>

### System
![system](https://github.com/user-attachments/assets/a6df53a4-cbdd-446b-b613-9aca0fefecb2)

### Light Mode
![light](https://github.com/user-attachments/assets/1990f3ac-8789-4c49-b86e-8f3d006b61ea)

### Dark Mode
![dark](https://github.com/user-attachments/assets/17b9b818-a8e2-4410-bce5-8a290c6eb9b7)

</details>

<details>
<summary>Windows 10</summary>

### System
![system](https://github.com/user-attachments/assets/811bdb6a-42d8-4cb7-b963-1456b861734d)

### Light Mode
![light](https://github.com/user-attachments/assets/cc6117ee-b366-4b35-9eb7-fd824fafca0b)

### Dark Mode
![dark](https://github.com/user-attachments/assets/f8db3e8f-cd92-42af-b1ca-296cd7ec7f83)

</details>

<details>
<summary>Windows 11</summary>

### System
![system](https://github.com/user-attachments/assets/3504dc0f-432c-4e44-ae3e-ca78980d0c90)

### Light Mode
![light](https://github.com/user-attachments/assets/38d581f1-5154-489a-b01d-e72d7029876a)

### Dark Mode
![dark](https://github.com/user-attachments/assets/faf1d9a2-1f01-4d43-8dba-0a2ed7416515)

</details>

## Installation
**Windows**
1. Install the [Microsoft Visual C++ Redistributable](https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads)
2. Run `VidInjector9002.5.exe`

**Linux**

**Arch**
1. Install `gtk3` with your package manager (`pacman -S gtk3`)
2. Run `VidInjector9002.5`

## Compiling
**Windows**
1. Download and install [CMake](https://cmake.org/), [Git](https://git-scm.com/downloads/win), and [Microsoft Visual Studio](https://visualstudio.microsoft.com/) with the `Desktop development with C++` workload
2. Clone the entire repository `git clone --recurse-submodules https://github.com/FoofooTheGuy/VidInjector9000.git`
3. In `VidInjector9000/VidInjector9002.5/`, Open `wxWidgets-3.3.0/build/msw/wx_vc17.sln` in Visual Studio
4. Set `Solution Configuration` to `Release` and build the solution
5. Close Microsoft Visual Studio and go back to `VidInjector9000/VidInjector9002.5/` to run `build.bat`

**Linux**
1. Install dependencies for wxWidgets. For example, on debian:
```
sudo apt install \
libgtk-3-dev \
libcurl4-openssl-dev \
libsecret-1-dev \
libwebkitgtk-6.0-dev \
libwebkit2gtk-4.1-dev \
libsoup-3.0-dev \
libgstreamer-plugins-base1.0-dev \
libsdl1.2-dev \
libsdl2-dev \
libnotify-dev \
libmspack-dev \
libgspell-1-dev
```
2. Install `cmake`, `make`, `gcc`, `g++`, and `git` with your package manager
3. Clone the entire repository `git clone --recurse-submodules https://github.com/FoofooTheGuy/VidInjector9000.git`
4. Run `cd VidInjector9000/VidInjector9002.5`
5. Run `install-wxwidgets.sh` Note: Alternatively, you might be able to install `wxwidgets` with your package manager
6. Run `build.sh`

**MacOS**
1. Install the [Homebrew](https://brew.sh/) package manager
2. Run `brew install cmake make`
3. Clone the entire repository `git clone --recurse-submodules https://github.com/FoofooTheGuy/VidInjector9000.git`
4. Run `cd VidInjector9000/VidInjector9002.5`
5. Run `install-wxwidgets.sh`
6. Run `build.sh`

## Compiling Continued
In order to properly use this program, you must also compile `VidInjector9002-CLI`
1. See [The instructions for compiling VidInjector9002-CLI](https://github.com/FoofooTheGuy/VidInjector9000/blob/main/VidInjector9002-CLI/README.md#compiling)
2. move the `VidInjector9002-CLI(.exe)` binary to the `VidInjector9000Resources/` directory within the same directory of `VidInjector9002.5(.exe)`. (Create a new directory if it is not there.) If you are on MacOS, move the file `VidInjector9002-CLI.app/Contents/MacOS/VidInjector9002-CLI`

## Translation
I gave up on trying to translate the program into languages I don't know. So if you want to help, please create a pull request or issue to share your translation. Here are some useful tips on how it should be done:

- Please use a text editor like [geany](https://www.geany.org/) or [notepad++](https://notepad-plus-plus.org/) to ensure all of the text is shown correctly when editing.
- The `Language` value in the file should be the name of the language in said language, not the word "English" translated.
- Put translations in `VidInjector9000Resources/language/<Language>/Language.txt`.

**Translators:**
- [sheepgreen](https://github.com/slippersheepig) (Simplified Chinese)
- [Tommy Nguyen](https://github.com/duynho2003) (Vietnamese)
