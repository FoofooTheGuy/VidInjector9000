import os
import sys
import shutil
import subprocess

if sys.platform == "win32" or sys.platform == "cygwin":
    bin = "VidInjector9001.exe"
else:
    bin = "VidInjector9001"
cpp = "c++17"

try:
    shutil.rmtree("obj")
except:
    print("tree not found")

try:
    os.remove(bin)
except:
    print("failed to delete " + bin)

try:
    os.makedirs("obj/Debug")
except:
    print("failed to create dir")

files = ""
for s in os.listdir("."):
    if s.find(".c") != -1:
        print(s)
        subprocess.call("g++ -Wall -c -g " + s + " -std=" + cpp + " -o obj/Debug/" + s.rsplit('.', 1)[0] + ".o")
        files += "obj/Debug/" + s.rsplit('.', 1)[0] + ".o"

subprocess.call("g++ -static -static-libgcc -static-libstdc++ -o \"" + bin + "\" " + files)