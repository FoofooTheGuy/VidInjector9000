#!/usr/bin/env bash

CLI="../build/VidInjector9002-CLI"

# set Ptitles
${CLI} -set_parameter out/a.vi9p 12 '9 ONE あ' out/a.vi9p
${CLI} -set_parameter out/a.vi9p 13 '8 TWO あ' out/a.vi9p
${CLI} -set_parameter out/a.vi9p 14 '7 THREE あ' out/a.vi9p
${CLI} -set_parameter out/a.vi9p 15 '6 FOUR あ' out/a.vi9p
