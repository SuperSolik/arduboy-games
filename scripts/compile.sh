#!/bin/sh

set -xe 

rm -rf build/* && arduino-cli compile -b arduino:avr:leonardo --build-cache-path ./build/ --build-path ./build/ $1
