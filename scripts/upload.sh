#!/bin/sh

set -xe

port=$(arduino-cli board list --format json | jq -r '.[] | select(has("matching_boards")) | select(.matching_boards[].fqbn == "arduino:avr:leonardo") | .port.address')

arduino-cli upload -b arduino:avr:leonardo --verify --port $port --input-dir ./build $1
