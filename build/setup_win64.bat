@echo off

mkdir st-win64
cd st-win64
cmake -G "Visual Studio 15 2017 Win64" ../../src/engine
cd ..
