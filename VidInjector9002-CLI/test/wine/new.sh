#!/usr/bin/env bash

CLI="../../build/VidInjector9002-CLI.exe"
echo ${CLI}

mkdir out

# create new vi9p file
wine ${CLI} -new out/a.vi9p
