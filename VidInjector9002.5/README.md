<img src="./Vidinjector9002.5 logo.svg" alt="drawing" width="225"/>

# VidInjector9002.5
A GUI wrapper for VidInjector9002-CLI. Made with pure [wxWidgets](https://github.com/wxWidgets/wxWidgets) in pure code.

## Screenshots
<details>
<summary>Arch Linux (XFCE)</summary>

### System (Light Mode)
<img width="1272" height="700" alt="system light" src="https://github.com/user-attachments/assets/4d112232-5efb-4bec-8571-b809cdb1bd24" />

### System (Dark Mode)
<img width="1282" height="718" alt="system dark" src="https://github.com/user-attachments/assets/ea9189b0-9895-43dd-943c-793bc169cf82" />

### Light Mode
<img width="1272" height="700" alt="light" src="https://github.com/user-attachments/assets/72e83f40-e0fa-45f7-8a69-76100ce9bd5d" />

### Dark Mode
<img width="1272" height="700" alt="dark" src="https://github.com/user-attachments/assets/05b6bf26-39f2-4d69-80ed-e33bec9c33a6" />

</details>

<details>
<summary>MacOS (Sonoma 14.7.7)</summary>

### System (Light Mode)
<img width="1262" height="812" alt="system light" src="https://github.com/user-attachments/assets/b7dd74e0-b0ec-4894-976f-9d57e24fca4b" />

### System (Dark Mode)
<img width="1262" height="812" alt="system dark" src="https://github.com/user-attachments/assets/df7fc5bc-43e1-404d-826c-7b6285ab1638" />

### Light Mode
<img width="1262" height="812" alt="light" src="https://github.com/user-attachments/assets/d2356f79-a5a1-4e7f-8b38-6e355a020af5" />

### Dark Mode
<img width="1262" height="812" alt="dark" src="https://github.com/user-attachments/assets/91469d26-a02f-458a-8cc2-b8962b7cf5b7" />

</details>

<details>
<summary>Windows 7</summary>

### System
<img width="1150" height="700" alt="system" src="https://github.com/user-attachments/assets/1488a798-ecf4-496f-a668-1765c55ce2ee" />

### Light Mode
<img width="1150" height="700" alt="light" src="https://github.com/user-attachments/assets/dff31912-44f1-49f2-8beb-198577955e84" />

### Dark Mode
<img width="1150" height="700" alt="dark" src="https://github.com/user-attachments/assets/99ff21f8-a337-442a-b68c-2293e88e048f" />

</details>

<details>
<summary>Windows 8.1</summary>

### System
<img width="1150" height="700" alt="system" src="https://github.com/user-attachments/assets/0138247a-cd9e-416d-8ed1-37fd086702ad" />

### Light Mode
<img width="1150" height="700" alt="light" src="https://github.com/user-attachments/assets/f5271a7d-87d8-462a-b077-f4b32ba3e52b" />

### Dark Mode
<img width="1150" height="700" alt="dark" src="https://github.com/user-attachments/assets/9ebe78ee-0e24-470b-b5a5-59ecdf831f81" />

</details>

<details>
<summary>Windows 10</summary>

### System
<img width="1136" height="693" alt="system" src="https://github.com/user-attachments/assets/c06aa33e-572c-4dca-ab59-7b2e5d7f8cd2" />

### Light Mode
<img width="1136" height="693" alt="light" src="https://github.com/user-attachments/assets/4e5f54aa-906a-4757-a2fd-b7ac571e3568" />

### Dark Mode
<img width="1136" height="693" alt="dark" src="https://github.com/user-attachments/assets/af2991f1-fbec-4dc3-88ee-41ff8a7a3ade" />

</details>

<details>
<summary>Windows 11</summary>

### System
<img width="1136" height="693" alt="system" src="https://github.com/user-attachments/assets/bc360782-dcd9-4e1e-8047-4b41614f5db1" />

### Light Mode
<img width="1136" height="693" alt="light" src="https://github.com/user-attachments/assets/f46a4005-d605-4153-81af-5a5ebc00f289" />

### Dark Mode
<img width="1136" height="693" alt="dark" src="https://github.com/user-attachments/assets/5b2bc2a4-696c-49ec-8f25-2edccc4dc035" />

</details>

## Installation
**Windows**
1. Install the [Microsoft Visual C++ Redistributable](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist)
2. Run `VidInjector9002.5.exe`

**Linux**

**Arch**
1. Install the dependencies with your package manager (`pacman -S gtk3 libmspack`)
2. Run `VidInjector9002.5`

## Compiling
**Windows**
1. Download and install [CMake](https://cmake.org/), [Git](https://git-scm.com/downloads/win), and [Microsoft Visual Studio](https://visualstudio.microsoft.com/) with the `Desktop development with C++` workload
2. Clone the entire repository `git clone --recurse-submodules https://github.com/FoofooTheGuy/VidInjector9000.git`
3. Run `cd VidInjector9000/VidInjector9002.5`
4. Run `build.bat`

**Linux**
1. Install `cmake`, `make`, `gcc`, `g++`, and `git` with your package manager
2. Clone the entire repository `git clone --recurse-submodules https://github.com/FoofooTheGuy/VidInjector9000.git`
3. Run `cd VidInjector9000/VidInjector9002.5`
4. Install dependencies for wxWidgets. For example, on debian:
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
5. Run `build.sh`

**MacOS**
1. Install the [Homebrew](https://brew.sh/) package manager
2. Run `brew install cmake make`
3. Clone the entire repository `git clone --recurse-submodules https://github.com/FoofooTheGuy/VidInjector9000.git`
4. Run `cd VidInjector9000/VidInjector9002.5`
6. Run `build.sh`

## Compiling Continued
In order to properly use this program, you must also compile `VidInjector9002-CLI`
1. See [The instructions for compiling VidInjector9002-CLI](https://github.com/FoofooTheGuy/VidInjector9000/blob/main/VidInjector9002-CLI/README.md#compiling)
2. move the `VidInjector9002-CLI(.exe)` binary to the `VidInjector9000Resources/` directory within the same directory of `VidInjector9002.5(.exe)`. (Create a new directory if it is not there.)

## Translation
I gave up on trying to translate the program into languages I don't know. So if you want to help, please create a pull request or issue to share your translation. Here are some useful tips on how it should be done:

- Please use a text editor like [geany](https://www.geany.org/) or [notepad++](https://notepad-plus-plus.org/) to ensure all of the text is shown correctly when editing.
- The `Language` value in the file should be the name of the language in said language, not the word "English" translated.
- Put translations in `VidInjector9000Resources/language/<Language>/Language.txt`.

**Translators:**
- [sheepgreen](https://github.com/slippersheepig) (Simplified Chinese)
- [Tommy Nguyen](https://github.com/duynho2003) (Vietnamese)
