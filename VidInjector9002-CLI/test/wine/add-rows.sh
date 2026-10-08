#!/usr/bin/env bash

CLI="../../build/VidInjector9002-CLI.exe"

# set mode to multi
wine ${CLI} -set_parameter out/a.vi9p 0 1 out/a.vi9p

# add 3 more rows so we have 4
wine ${CLI} -add_row out/a.vi9p out/a.vi9p
wine ${CLI} -add_row out/a.vi9p out/a.vi9p
wine ${CLI} -add_row out/a.vi9p out/a.vi9p
