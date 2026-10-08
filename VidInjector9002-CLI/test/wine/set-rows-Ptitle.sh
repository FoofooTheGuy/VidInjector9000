#!/usr/bin/env bash

CLI="../../build/VidInjector9002-CLI.exe"

# set Ptitles
wine ${CLI} -set_parameter out/a.vi9p 12 '_ONE あ' out/a.vi9p
wine ${CLI} -set_parameter out/a.vi9p 13 '_TWO あ' out/a.vi9p
wine ${CLI} -set_parameter out/a.vi9p 14 '_THREE あ' out/a.vi9p
wine ${CLI} -set_parameter out/a.vi9p 15 '_FOUR あ' out/a.vi9p
