#!/usr/bin/env bash

CLI="../../build/VidInjector9002-CLI.exe"

wine ${CLI} -build_cia out/a.vi9p out/あ.cia

echo $?
