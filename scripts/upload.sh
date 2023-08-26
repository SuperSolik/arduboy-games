#!/bin/sh

set -xe

# NOTE: scripts are for my personal usage, hence the hardcoded port


arduino-cli upload -b arduino:avr:leonardo --verify --port /dev/cu.usbmodem101 --input-dir ./build $1
