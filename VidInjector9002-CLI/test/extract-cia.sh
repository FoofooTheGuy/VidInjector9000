#!/usr/bin/env bash

CLI="../build/VidInjector9002-CLI"

# extract
${CLI} -extract_cia out/a.cia out/a

printf '\n\n\n'

# print
${CLI} -print_parameters out/a/parameters.vi9p
