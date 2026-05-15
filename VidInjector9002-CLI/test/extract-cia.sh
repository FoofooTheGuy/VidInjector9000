#!/usr/bin/env bash

CLI="../build/VidInjector9002-CLI"

# extract
valgrind --leak-check=full ${CLI} -extract_cia out/a.cia out/a
