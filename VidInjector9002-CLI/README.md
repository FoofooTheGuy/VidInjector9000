```
          @@@@@@@@@
        @@@@@@@@@@@@@
     @@@@@@@   @@@@@@@
  @@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@   @@@@@@@@@@@
@@@@@@@@@@@@   @@@@@@@@@@@@
@@@@@@@@@@@@   @@@@@@@@@@@@
 @@@@@@@@@@@   @@@@@@@@@@@@
  @@@@@@@@@@   @@@@@@@@@@@@
   @@@@@@@@@   @@@@@@@@@@
     @@@@@@@   @@@@@@@@
      @@@@@@@@@@@@@@
        @@@@@@@@@
```
# VidInjector9002-CLI
A console program that interfaces with a VidInjector9002 parameters file.

## Compiling
**Windows**
1. Download and install [CMake](https://cmake.org/), [Git](https://git-scm.com/downloads/win), and [Microsoft Visual Studio](https://visualstudio.microsoft.com/) with the `Desktop development with C++` workload
2. Clone the entire repository `git clone --recurse-submodules -j8 https://github.com/FoofooTheGuy/VidInjector9000.git`
3. Run `cd VidInjector9000/VidInjector9002-CLI`
4. Run `./build.bat`

**Linux**
1. Install `make`, `cmake`, `gcc`, `g++`, and `git` with your package manager if you don't already have them
2. Clone the entire repository `git clone --recurse-submodules -j8 https://github.com/FoofooTheGuy/VidInjector9000.git`
3. Run `cd VidInjector9000/VidInjector9002-CLI`
4. Run `./build.sh`

**MacOS**
1. Install the [Homebrew](https://brew.sh/) package manager
2. Run `brew install make cmake`
3. Clone the entire repository `git clone --recurse-submodules -j8 https://github.com/FoofooTheGuy/VidInjector9000.git`
4. Run `cd VidInjector9000/VidInjector9002-CLI`
5. Run `./build.sh`

## Usage

Usage: VidInjector9002-CLI \<argument\> \[parameters...\]

**Arguments:**

<details>
<summary>-new | -n</summary>

Create a .vi9p file with default parameters

Usage:

VidInjector9002-CLI -new \<output .vi9p file\>

---

</details>



<details>
<summary>-set_parameter | -sp</summary>

Set a parameter based on which number of parameter to set (Output file can be the same as input file)

Usage:

VidInjector9002-CLI -set_parameter \<input .vi9p file\> \<number\> \<new value\> \<output .vi9p file\>

---

</details>



<details>
<summary>-set_smdh | -ss</summary>

Similar to -sp, this reads the titles from an SMDH and sets them to short name, long name, and publisher in the output .vi9p file. Useful for when first using an SMDH for the icon (Output file can be the same as input file)

Usage:

VidInjector9002-CLI -set_smdh \<input .vi9p file\> \<new value\> \<output .vi9p file\>

---

</details>



<details>
<summary>-print_parameters | -pp</summary>

Print the parameters in a .vi9p file in a numbered list

Usage:

VidInjector9002-CLI -print_parameters \<input .vi9p file\>

---

</details>



<details>
<summary>-add_row | -ar</summary>

Add parameters for a row and increment the INT:ROWS parameter (Output file can be the same as input file)

Usage:

VidInjector9002-CLI -add_row \<input .vi9p file\> \<output .vi9p file\>

---

</details>



<details>
<summary>-subtract_row | -sr</summary>

Remove the parameters from the last row and decrement the INT:ROWS parameter (Output file can be the same as input file)

Usage:

VidInjector9002-CLI -subtract_row \<input .vi9p file\> \<output .vi9p file\>

---

</details>



<details>
<summary>-build_cia | -bc</summary>

Build a .cia from parameters file. Note that the title IDs for this must range from C0000 - EFFFF in order to avoid confict with other titles.

Usage:

VidInjector9002-CLI -build_cia \<input .vi9p file\> \<unique ID\> \<application title\> \<product code latter\> \<output .cia file\>

---

The same as the previous one, except with an output .tar which will contain a Luma LayeredFS game patch. (Only use this if INT:SPLITPATCH is set in the input .vi9p)

Usage:

VidInjector9002-CLI -build_cia \<input .vi9p file\> \<unique ID\> \<application title\> \<product code latter\> \<output .cia file\> \<output .tar file\>

---

The same as the first one, except with the following defaults where the argument parameters are missing: (random unique ID) 'video' 'VDIJ'

Usage:

VidInjector9002-CLI -build_cia \<input .vi9p file\> \<output .cia file\>

---

The same as the previous one, except with an output .tar which will contain a Luma LayeredFS game patch. (Only use this if INT:SPLITPATCH is set in the input .vi9p)

Usage:

VidInjector9002-CLI -build_cia \<input .vi9p file\> \<output .cia file\> \<output .tar file\>

---

</details>



<details>
<summary>-extract_cia | -ec</summary>

Extract parameters from a .cia to a directory which will contain the romfs directory, exefs directory, and .vi9p file. Note that unnecessary files will not be extracted.

Usage:

VidInjector9002-CLI -extract_cia \<input .cia file\> \<output directory\>

---

The same as the previous one, except with the seed to decrypt the content.

Usage:

VidInjector9002-CLI -extract_cia \<input .cia file\> \<seed file\> \<output directory\>

---

</details>



<details>
<summary>-extract_tar | -et</summary>

Extract patch parameters from a .tar to a directory which will contain the romfs directory and .vi9p file. Set <output directory> to the directory of an extracted .cia to combine it all.

Usage:

VidInjector9002-CLI -extract_tar \<input .tar file\> \<output directory\>

---

</details>



<details>
<summary>-generate_preview | -gp</summary>

Generate a preview image for the banner or icon that matches \<number\>. (Do -pp to see a list of possible numbers)

Usage:

Vidinjector9002-CLI -generate_preview \<input .vi9p file\> \<number\> \<output .png file\>

---

</details>


<details>
<summary>-return_rows | -rr</summary>

Return the value of INT:ROWS from \<input .vi9p file\>. Note: This is only for reading purposes. Use -ar or -sr to change the amount of rows.

Usage:

Vidinjector9002-CLI -generate_preview \<input .vi9p file\>

---

</details>

## Known Issues

#### Unicode paths do not work on Windows
The program often crashes when given paths with special characters ([#8](https://github.com/FoofooTheGuy/VidInjector9000/issues/8))

#### (Help wanted) Multi Video injects do not work on the regions China, Taiwan and Korea because there are no language files for those.
I am unable to add them myself because I don't know those languages so I would appreciate it if someone would translate the files and contribute the following:

romfs/lang/
- CN_Simp_Chinese/message.msbt
- KR_Hangeul/message.msbt
- TW_Trad_Chinese/message.msbt

To remedy this, you can use [lumalocaleswitcher](https://github.com/Possum/LumaLocaleSwitcher/releases/latest).

## Credits and Stuff
~~[nnc](https://github.com/MyPasswordIsWeak/nnc)~~ [(my fork of nnc)](https://github.com/FoofooTheGuy/nnc): Converting banner and icon images, packing and unpacking CIA files

[stb image](https://github.com/nothings/stb): Manipulating images

[miniz cpp](https://github.com/tfussell/miniz-cpp): Zip extraction

[microtar](https://github.com/mudita/microtar): TAR reading and writing

[3DBrew](https://www.3dbrew.org/): Emotional support
