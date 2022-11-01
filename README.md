# VidInjector9000
A step-by-step injector for 3DS video CIAs focused on presentation

Outdated Walk Through
---
[<img width="1012" height="-1" src="https://i.ytimg.com/vi/JSvrmVNuGLA/maxresdefault.jpg">
How to Create Nintendo 3DS Video Injections with VidInjector9001](https://www.youtube.com/watch?v=JSvrmVNuGLA)

Usage
--
1. Download [VidInjector9001.zip](https://github.com/FoofooTheGuy/VidInjector9000/releases/latest/download/VidInjector9001.zip), extract it somewhere, and run VidInjector9001.exe.
2. Type the letter corresponding to which part you want to do and follow the directions on screen.
3. Bask in the glory of the official looking banner!

**Tips:**

- Drag and drop a .txt file when inputting text for jobs (except finalization) and you will be able to use the contents of the file as input
- When typing the letter for which job you want or when it prompts for Y/N, it is not case sensitive!
- It also only checks for the first letter, so you can actually type something like "yes" and it will still work!

You may ask...
--

**Q: How do I make moflex files?**

A: Use Mobiclip Multicore Encoder. The most straight forward way to get that is on the Nintendo Developer Portal, but there are mirrors online somewhere...

**Q: Why did you even make this?**

A: I made this so that the process of injecting with 3D banners is more streamlined. Also, VidInjector9000 2.0+ (aka VidInjector9001) can create multi video injects with MultiVidInjector5000!

**Q: How is this different from all the other injectors?**

A: This is simple and gives you much control over your injections.

Known Issues
--
#### Multi Video injects do not work on the regions China, Taiwan and Korea because there are no [language files](https://github.com/FoofooTheGuy/VidInjector9000/tree/main/VidInjector9000/Vidinjector9000Resources/files/templates/MultiVideo/romfs/lang) for those.
I am unable to add them myself because I don't know those languages so I would appreciate it if someone would translate the files and contribute the following:

[(VidInjector9000/VidInjector9000/Vidinjector9000Resources/files/templates/MultiVideo/romfs/lang/)](https://github.com/FoofooTheGuy/VidInjector9000/tree/main/VidInjector9000/Vidinjector9000Resources/files/templates/MultiVideo/romfs/lang)
- CN_Simp_Chinese/message.msbt
- KR_Hangeul/message.msbt
- TW_Trad_Chinese/message.msbt

Building
--
**Windows (MinGW)**

0. A better guide can be found [here](https://www.geeksforgeeks.org/complete-guide-to-install-c17-in-windows/)
1. Download the latest [MinGW](https://nuwen.net/mingw.html) and put the contents on your computer.
2. Add the bin folder to PATH
3. drag [VidInjector9001.cpp](https://github.com/FoofooTheGuy/VidInjector9000/raw/main/VidInjector9000/src/VidInjector9001.cpp) onto [build.bat](https://github.com/FoofooTheGuy/VidInjector9000/raw/main/VidInjector9000/src/build.bat) so that it builds it (or run build.bat with VidInjector9001.cpp as the argument)

**\*nix and whatnot**

The program should be compatible with these operating systems, but exact instructions are questionable.


Credits and Stuff
--
[videoinject](https://github.com/ihaveamac/videoinject): Example functions and inspiration

[bannertool](https://github.com/Steveice10/bannertool): Compiling icon.bin and banner.bin

[makerom](https://github.com/3DSGuy/Project_CTR): building CIA

[stb image](https://github.com/nothings/stb): Manipulating images
