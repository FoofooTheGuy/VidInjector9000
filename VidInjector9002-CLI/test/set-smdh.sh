#!/usr/bin/env bash

CLI="../build/VidInjector9002-CLI"

# set smdh
${CLI} -set_smdh out/a.vi9p 'icon.bin' out/a.vi9p

# set icon border (no border)
${CLI} -set_parameter out/a.vi9p 3 0 out/a.vi9p

# generate preview
${CLI} -generate_preview out/a.vi9p 2 out/smdh.png
