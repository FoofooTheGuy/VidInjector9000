import os
import shutil
import subprocess

os.chdir("nnc")

subprocess.call(["cmake", "-B", "build"])
subprocess.call(["cmake", "--build build", "--config", "Release", "--parallel", "4"])

shutil.copyfile("build/libnnc.a", "../libnnc.a")

try:
    raw_input("Press Enter to finish...")
except:
    input("Press Enter to finish...")
