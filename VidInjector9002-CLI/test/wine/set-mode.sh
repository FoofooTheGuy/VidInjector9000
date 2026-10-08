#!/usr/bin/env bash

CLI="../../build/VidInjector9002-CLI.exe"

# set mode to extended
wine ${CLI} -set_parameter out/a.vi9p 0 2 out/a.vi9p
