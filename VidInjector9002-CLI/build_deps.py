import os
import shutil
import subprocess

#subprocess.call(["git", "clone", "https://github.com/MyPasswordIsWeak/nnc.git", "--depth", "1"])
#subprocess.call(["git", "clone", "https://github.com/Mbed-TLS/mbedtls.git"])
os.chdir("mbedtls")
os.environ["CC"] = "gcc"
subprocess.call(["make"])
shutil.copyfile("library/libmbedcrypto.a", "../libmbedcrypto.a")
os.chdir("../nnc")
print("\n\n\n\nPLEASE READ:\n\nBefore compiling nnc, edit nnc/Makefile in a text editor and add \"-I../mbedtls/include\" (without quotes) to CFLAGS.\n")
try:
    raw_input("Press Enter to continue...")
except:
    input("Press Enter to continue...")
subprocess.call(["make"])
shutil.copyfile("libnnc.a", "../1libnnc.a")

try:
    raw_input("Press Enter to finish...")
except:
    input("Press Enter to finish...")