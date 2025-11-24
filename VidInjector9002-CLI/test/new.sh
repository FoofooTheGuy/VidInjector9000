#!/usr/bin/env bash

CLI="../build/VidInjector9002-CLI"
echo ${CLI}

mkdir out

# create new vi9p file
${CLI} -new out/a.vi9p
