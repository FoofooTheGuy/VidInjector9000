#!/usr/bin/env bash

CLI="../build/VidInjector9002-CLI"

# set icon
${CLI} -set_parameter out/a.vi9p 2 'big buck icon.png' out/a.vi9p

# set icon border (no border)
${CLI} -set_parameter out/a.vi9p 3 0 out/a.vi9p

# generate preview
${CLI} -generate_preview out/a.vi9p 2 out/icon0.png

# set icon border (under border)
${CLI} -set_parameter out/a.vi9p 3 1 out/a.vi9p

# generate preview
${CLI} -generate_preview out/a.vi9p 2 out/icon1.png

# set icon border (within border)
${CLI} -set_parameter out/a.vi9p 3 2 out/a.vi9p

# generate preview
${CLI} -generate_preview out/a.vi9p 2 out/icon2.png

