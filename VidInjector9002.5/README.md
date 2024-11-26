<img src="https://github.com/user-attachments/assets/e3abe8b2-2b8e-47c4-a434-f9c8330f135b" alt="drawing" width="225"/>

# VidInjector9002.5
A GUI wrapper for VidInjector9002-CLI

# Installation
**Windows**
1. Install the [Microsoft Visual C++ Redistributable](https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads)
2. Run `VidInjector9002.5.exe`

**Linux**

**Arch**
1. Install `gtk3` with your package manager
2. Run `VidInjector9002.5`

# Compiling from source
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
libgtk3-dev \
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
