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

# Compiling
**Windows (MSYS2)**
1. Download and install [MSYS2](https://www.msys2.org/) and [Python](https://www.python.org/downloads/)
2. Run build_deps.py
3. Run build.py

**Linux**
1. Install `make`, `python` or `python3`, and `perl` with your package manager if you don't already have them
2. Run build_deps.py
3. Run build.py

If `pip` doesn't work, run `python3 -m ensurepip`

If you don't have `ZipFile`, run `python3 -m pip install zipfile`

If compiling MbedTLS gives errors, do these and try again:
```
python3 -m pip install -U jsonschema
python3 -m pip install jinja2
```

# Usage

The goal of VidInjector9002-CLI is to do [everything VidInjector9002 can do](https://github.com/FoofooTheGuy/VidInjector9000?tab=readme-ov-file#vidinjector9002-can) but as a CLI so you can use it in automation and with greater cross platform compatibility.

Usage: VidInjector9002-CLI \<argument\> \[parameters...\]

**Arguments:**

<details>
<summary>-new | -n</summary>

Create a .vi9p file with default parameters

Usage:

VidInjector9002-CLI -new \<output .vi9p file\>

</details>



<details>
<summary>-set_parameter | -sp</summary>

Set a parameter based on which number of parameter to set (Output file can be the same as input file)

Usage:

VidInjector9002-CLI -set_parameter \<input .vi9p file\> \<number\> \<new value\> \<output .vi9p file\>

</details>



<details>
<summary>-print_parameters | -pp</summary>

Print the parameters in a .vi9p file in a numbered list

Usage:

VidInjector9002-CLI -print_parameters \<input .vi9p file\>

</details>



<details>
<summary>-add_row | -ar</summary>

Add parameters for a row and increment the INT:ROWS parameter (Output file can be the same as input file)

Usage:

VidInjector9002-CLI -add_row \<input .vi9p file\> \<output .vi9p file\>

</details>



<details>
<summary>-subtract_row | -sr</summary>

Remove the parameters from the last row and decrement the INT:ROWS parameter (Output file can be the same as input file)

Usage:

VidInjector9002-CLI -subtract_row \<input .vi9p file\> \<output .vi9p file\>

</details>



<details>
<summary>-build_cia | -bc</summary>

Build a .cia from parameters file. Note that the title IDs for this must range from C0000 - EFFFF in order to avoid confict with other titles.

Usage:

VidInjector9002-CLI -build_cia \<input .vi9p file\> \<unique ID\> \<application title\> \<product code latter\> \<output .cia file\>

----

The same as the previous one, except with an output .tar which will contain a Luma LayeredFS game patch. (Only use this if INT:SPLITPATCH is set in the input .vi9p)

Usage:

VidInjector9002-CLI -build_cia \<input .vi9p file\> \<unique ID\> \<application title\> \<product code latter\> \<output .cia file\> \<output .tar file\>

----

The same as the first one, except with the following defaults where the argument parameters are missing: (random unique ID) 'video' 'VDIJ'

Usage:

VidInjector9002-CLI -build_cia \<input .vi9p file\> \<output .cia file\>

----

The same as the previous one, except with an output .tar which will contain a Luma LayeredFS game patch. (Only use this if INT:SPLITPATCH is set in the input .vi9p)

Usage:

VidInjector9002-CLI -build_cia \<input .vi9p file\> \<output .cia file\> \<output .tar file\>

</details>



<details>
<summary>-extract_cia | -ec</summary>

Extract parameters from a .cia to a directory which will contain the romfs directory, exefs directory, and .vi9p file. Note that unnecessary files will not be extracted.

Usage:

VidInjector9002-CLI -extract_cia \<input .cia file\> \<output directory\>

----

The same as the previous one, except with the seed to decrypt the content.

Usage:

VidInjector9002-CLI -extract_cia \<input .cia file\> \<seed file\> \<output directory\>

</details>



<details>
<summary>-extract_tar | -et</summary>

Extract patch parameters from a .tar to a directory which will contain the romfs directory and .vi9p file. Set <output directory> to the directory of an extracted .cia to combine it all.

Usage:

VidInjector9002-CLI -extract_tar \<input .tar file\> \<output directory\>

</details>



<details>
<summary>-generate_preview | -gp</summary>

Generate a preview image for the banner or icon that matches \<number\>. (do -pp to see a list of possible numbers)

Usage:

Vidinjector9002-CLI -generate_preview \<input .vi9p file\> \<number\> \<output .png file\>

</details>

---
