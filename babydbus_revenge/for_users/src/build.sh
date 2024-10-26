#!/bin/sh

set -e

cd src
sudo apt install -y libdbus-1-dev libdbus-glib-1-dev
make

cd ..
cd dist
docker build -t babydbus .