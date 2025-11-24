#!/bin/bash

mkdir deps
cd deps
git clone --depth 1 https://github.com/raysan5/raylib.git
rm -rf raylib/.git
cd ..