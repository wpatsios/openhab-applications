#!/bin/bash
cd "$HOME/firmware/main/"
sudo make PLATFORM=photon APP=$1 program-dfu
