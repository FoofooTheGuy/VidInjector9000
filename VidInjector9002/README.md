<img src="https://github.com/FoofooTheGuy/VidInjector9000/assets/32585652/c6dddd9b-dfa7-4560-a7f6-2b478a97b6be" alt="drawing" width="225"/>

# VidInjector9000
A cross platform* injector for 3DS video CIAs

<sup><sup>*If builds for your operating system are not provided then you will have to compile it yourself, see [compiling](https://github.com/FoofooTheGuy/VidInjector9000/VidI#compiling).<sup><sup>

## Screenshots
<details>
<summary>Windows 8.1</summary>

### Light Mode
![image](https://github.com/FoofooTheGuy/VidInjector9000/assets/32585652/94aeda22-e9ab-4870-ad44-772bd53ebc63)

### Dark Mode
![image](https://github.com/FoofooTheGuy/VidInjector9000/assets/32585652/f28dbc0f-acb6-412f-935c-56271ffe96ee)
</details>

<details>
<summary>Windows 10</summary>

### Light Mode
![light](https://github.com/FoofooTheGuy/VidInjector9000/assets/32585652/045d4761-8456-4b0e-9de9-7bbe46b5b748)

### Dark Mode
![dark](https://github.com/FoofooTheGuy/VidInjector9000/assets/32585652/01373504-f6f4-4c5a-b22b-560c868fa4f7)
</details>

<details>
<summary>Ubuntu 20.04.2.0</summary>

### Light Mode
![Screenshot from 2023-09-07 10-21-19](https://github.com/FoofooTheGuy/VidInjector9000/assets/32585652/f4d2cead-3798-4a73-a8de-3ddbce46f3f7)

### Dark Mode
![Screenshot from 2023-09-07 10-28-46](https://github.com/FoofooTheGuy/VidInjector9000/assets/32585652/c25c3cc1-1f3d-446e-ae33-58c3898c9d10)
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
#### Unicode file paths are not supported
At least in my testing, I've found that unicode in the file paths lead to it getting confused but it may or may not work on your OS, I'm not sure..

#### Multi Video injects do not work on the regions China, Taiwan and Korea because there are no language files for those.
I am unable to add them myself because I don't know those languages so I would appreciate it if someone would translate the files and contribute the following:

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

Translation
--
- Put a translation text file and an m.bmp in Vidinjector9000Resources/language/\<language>
(the M in the image stands for Multi, so consider that when translating!)
- The folder name is recommended to be in ASCII format to avoid issues, but it might actually work so... who knows?
- the `language` string value in the file should be the name of the language in such language, not the word "English" translated

**Thank you to the following for the work in translating:**

[Deepl](https://www.deepl.com/translator)

[Doogle Translate](https://translate.google.com/)

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