#!/bin/sh

set -xe

# NOTE: scripts are for my personal usage, hence the hardcoded port
port=$(arduino-cli board list --format json | jq -r '.[] | select(has("matching_boards")) | select(.matching_boards[].fqbn == "arduino:avr:leonardo") | .port.address')

arduino-cli upload -b arduino:avr:leonardo --verify --port $port --input-dir ./build $1
