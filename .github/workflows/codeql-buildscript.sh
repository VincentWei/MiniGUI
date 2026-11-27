#!/usr/bin/env bash

sudo apt-get update
sudo apt-get install -y autoconf
./autogen.sh
./configure; make -j$(nproc);
