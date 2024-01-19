import os
import shutil
import subprocess

subprocess.call(["unzip", "-q", "-o", "nnc.zip"])
subprocess.call(["unzip", "-q", "-o", "mbedtls-3.4.0.zip"])
os.chdir("mbedtls-3.4.0")
os.environ["CC"] = "gcc"
subprocess.call(["make"])
shutil.copyfile("library/libmbedcrypto.a", "../libmbedcrypto.a")
os.chdir("../nnc")
subprocess.call(["make"])
shutil.copyfile("libnnc.a", "../1libnnc.a")

try:
    raw_input("Press Enter to finish...")
except:
    input("Press Enter to finish...")