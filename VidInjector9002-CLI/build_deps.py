import os
import shutil
import subprocess
from zipfile import ZipFile

with ZipFile("nnc.zip", 'r') as zObject:
	zObject.extractall(path=".")
	
with ZipFile("mbedtls-3.4.0.zip", 'r') as zObject:
	zObject.extractall(path=".")

os.chdir("mbedtls-3.4.0")
os.environ["CC"] = "gcc"
subprocess.call(["make", "-C", "library"])
shutil.copyfile("library/libmbedcrypto.a", "../libmbedcrypto.a")
os.chdir("../nnc")
subprocess.call(["make"])
shutil.copyfile("libnnc.a", "../1libnnc.a")

try:
    raw_input("Press Enter to finish...")
except:
    input("Press Enter to finish...")