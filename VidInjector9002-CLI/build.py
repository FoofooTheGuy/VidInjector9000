import os
import sys
import shutil
import subprocess

bin = "VidInjector9002-CLI"
cpp = "gnu++20"

if os.path.exists(bin):
    os.remove(bin)

if not os.path.exists("obj"):
    os.makedirs("obj")

command = ["g++", "-static", "-static-libgcc", "-static-libstdc++", "-o", bin]
for s in os.listdir("src"):
    if s.find(".h") != -1:
        #print(s)
        if os.path.exists("obj/" + s.rsplit('.', 1)[0] + ".o"):
            ofile = os.path.getmtime("obj/" + s.rsplit('.', 1)[0] + ".o")
        else:
            ofile = 0.0
        if os.path.exists("src/" + s):
            hfile = os.path.getmtime("src/" + s)
        else:
            hfile = 1.0
        if(hfile > ofile):
            if os.path.exists("obj/" + s.rsplit('.', 1)[0] + ".o"):
                os.remove("obj/" + s.rsplit('.', 1)[0] + ".o")
for s in os.listdir("src"):
    if s.find(".c") != -1:
        print(s)
        if os.path.exists("obj/" + s.rsplit('.', 1)[0] + ".o"):
            ofile = os.path.getmtime("obj/" + s.rsplit('.', 1)[0] + ".o")
        else:
            ofile = 0.0
        if os.path.exists("src/" + s):
            cfile = os.path.getmtime("src/" + s)
        else:
            cfile = 1.0
        if(cfile > ofile):
            if os.path.exists("obj/" + s.rsplit('.', 1)[0] + ".o"):
                os.remove("obj/" + s.rsplit('.', 1)[0] + ".o")
            subprocess.call(["g++", "-Wall", "-c", "src/" + s, "-std=" + cpp, "-Innc/include", "-Imbedtls-3.4.0/include", "-o", "obj/" + s.rsplit('.', 1)[0] + ".o"])
        command.append("obj/" + s.rsplit('.', 1)[0] + ".o")
for s in os.listdir("."):
    if s.find(".a") != -1:
        print(s)
        command.append(s)
subprocess.call(command)
try:
    raw_input("Press Enter to finish...")
except:
    input("Press Enter to finish...")