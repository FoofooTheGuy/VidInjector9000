<img src="https://github.com/FoofooTheGuy/VidInjector9000/assets/32585652/c6dddd9b-dfa7-4560-a7f6-2b478a97b6be" alt="drawing" width="225"/>

# VidInjector9000
A cross platform* injector for 3DS video CIAs

<sup><sup>*If builds for your operating system are not provided then you will have to compile it yourself, see [compiling](https://github.com/FoofooTheGuy/VidInjector9000#compiling).<sup><sup>

## Example Screenshots
<details>
<summary>Windows 8.1</summary>

### Light Mode
![light](https://github.com/FoofooTheGuy/VidInjector9000/assets/32585652/2e9f0305-65b4-4183-93a5-22dd2d0705d5)

### Dark Mode
![dark](https://github.com/FoofooTheGuy/VidInjector9000/assets/32585652/7774cb6a-b5f1-458a-8d04-ebce25f92cbf)
</details>

<details>
<summary>Manjaro gnome 23.0-230903</summary>

### Light Mode
![manjaro white](https://github.com/FoofooTheGuy/VidInjector9000/assets/32585652/5981cedb-e2a4-4f85-a71f-bdd07d58110a)

### Dark Mode
![manjaro black](https://github.com/FoofooTheGuy/VidInjector9000/assets/32585652/5ddfeda4-86f0-4fbc-b660-4d5821c32fdd)
</details>

<details>
<summary>Wine (Manjaro gnome 23.0-230903)</summary>

### Light Mode
![wine white](https://github.com/FoofooTheGuy/VidInjector9000/assets/32585652/c74ac568-cef4-4ad6-9065-1efd1e8a3566)

### Dark Mode
![wine black](https://github.com/FoofooTheGuy/VidInjector9000/assets/32585652/065a2f42-1040-41e4-a6cd-e4bd06b7edcb)
</details>

VidInjector9002 can...
--

- **Create video injects.** Input .moflex videos, images, and text, and the program will build cia file for you!
- **Create multi-video injects.** Similar to the above, but there can be multiple videos in one cia!
- **Unpack a video CIA.** Load parameters from a .cia file to edit and rebuild!
- **Save and load parameters.** Save your place and come back later, they can even be saved/loaded automatically!
- **Be configured for any language.** If you can translate it, you can add it! (See [translation](https://github.com/FoofooTheGuy/VidInjector9000#translation) for details)

**Tips:**

- Click the icon preview to change border modes!
- Open a project file (.vi9p) with the program to load in those parameters!
- Click the multi video banner preview to choose file!
- Click the logo in the top left to find to the latest release!
- If an image doesn't fit in, it will be resized and cropped!

You may ask...
--

**Q: How do I make .moflex files?**

A: Use Mobiclip Multicore Encoder. The most straight forward way to get that is on the Nintendo Developer Portal, but there are mirrors online somewhere...

**Q: Why did you even make this?**

A: I made this so that the process of injecting with 3D banners is more streamlined.

**Q: How is this different from all the other injectors?**

A: This is simple and gives you much control over your video injections. Plus, VidInjector9000 3.0 (aka VidInjector9002) is a GUI and not a CLI!

Known Issues
--
#### Sometimes UTF-8 file paths are not supported on Windows/Wine
Most things work with UTF-8 file paths but certain things don't. (the location of the program, SMDH reading, CIA reading, CIA writing) These should work on other platforms though.

#### Multi Video injects do not work on the regions China, Taiwan and Korea because there are no language files for those.
I am unable to add them myself because I don't know those languages and this isn't really in my "jurisdiction" so I would appreciate it if someone would translate the files and contribute the following:

romfs/lang/
- CN_Simp_Chinese/message.msbt
- KR_Hangeul/message.msbt
- TW_Trad_Chinese/message.msbt

To remedy this, you can use [lumalocaleswitcher](https://github.com/Possum/LumaLocaleSwitcher/releases/latest).

Compiling
--
1. Download xtd from [gammasoft71](https://github.com/gammasoft71/xtd) or [my fork](https://github.com/FoofooTheGuy/xtd)
2. Run the install script that corresponds to your operating system
3. run the command `xtdc build -r` in the directory of Vidinjector9002's cmakelists.txt
4. ???
5. profit!!

Tested compatibility
--
![](https://img.shields.io/badge/Fully%20works%20as%20expected-008000)

![](https://img.shields.io/badge/Some%20functionality%20is%20broken-FFA500)

![](https://img.shields.io/badge/Does%20not%20work-B22222)

<details>
<summary>Windows 8.1</summary>

![](https://img.shields.io/badge/xtd_0.1.1--beta-008000)

![](https://img.shields.io/badge/xtd_nightly-B22222)

Notes: The window is unstable.
</details>

<details>
<summary>Windows 10</summary>

![](https://img.shields.io/badge/xtd_0.1.1--beta-008000)

![](https://img.shields.io/badge/xtd_nightly-008000)

</details>

<details>
<summary>Ubuntu 20.04.2.0</summary>

![](https://img.shields.io/badge/xtd_0.1.1--beta-FFA500)

Notes: Everything works except async (building and extracting CIA)

![](https://img.shields.io/badge/xtd_nightly-008000)

</details>

<details>
<summary>Wine (Ubuntu 20.04.2.0)</summary>

![](https://img.shields.io/badge/xtd_0.1.1--beta-FFA500)

Unicode characters like in Japanese text do not appear on the form.
</details>

<details>
<summary>Wine (manjaro)</summary>

![](https://img.shields.io/badge/xtd_0.1.1--beta-FFA500)

Unicode characters like in Japanese text do not appear on the form.
</details>

Translation
--
- Put a translation text file and an m.bmp in Vidinjector9000Resources/language/\<language>
(the M in the image stands for Multi, so consider that when translating!)
- The folder name is recommended to be in ASCII format to avoid issues, but it might actually work so... who knows?
- the `language` string value in the file should be the name of the language in such language, not the word "English" translated

**Thank you to the following for the work in translating:**

[Deepl](https://www.deepl.com/translator)

[Google Translate](https://translate.google.com/)

[Rayz0rum](https://github.com/Spider-Sonic)

"Pixel"

- Please create a pull request or issue if you want to share your translation of the program.

Credits and Stuff
--
[xtd](https://github.com/gammasoft71/xtd): The GUI framework that made this whole thing possible

[nnc](https://github.com/MyPasswordIsWeak/nnc): Converting banner and icon images, packing and unpacking CIA files

[stb image](https://github.com/nothings/stb): Manipulating images

[miniz cpp](https://github.com/tfussell/miniz-cpp): Zip extracion

[3DBrew](https://www.3dbrew.org/): Emotional support
