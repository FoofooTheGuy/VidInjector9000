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

## Compiling from source
**Windows**
1. Download and install [CMake](https://cmake.org/), [Git](https://git-scm.com/downloads/win), and [Microsoft Visual Studio](https://visualstudio.microsoft.com/) with the `Desktop development with C++` workload
2. Clone the entire repository `git clone --recurse-submodules -j8 https://github.com/FoofooTheGuy/VidInjector9000.git`
3. Open `wxWidgets-3.2.6/build/msw/wx_vc17.sln` in Visual Studio
4. Set `Solution Configuration` to `Release` and build the solution
5. Run `build.bat`

**Linux**
1. Install `cmake` and `git` with your package manager
2. Clone the entire repository `git clone --recurse-submodules -j8 https://github.com/FoofooTheGuy/VidInjector9000.git`
3. Install dependencies for wxWidgets. For example, on debian:
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
4. Run `install-wxwidgets.sh` Note: Alternatively, you might be able to install `wxwidgets` with your package manager
5. Run `build.sh`

## Translation
I gave up trying to translate the program into languages I don't know. So if you want to help, please create a pull request or issue to share your translation.

- the `Language` value in the file should be the name of the language in such language, not the word "English" translated.
- Put translations in `VidInjector9000Resources/language/<Language>/Language.txt`
