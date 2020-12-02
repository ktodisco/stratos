@echo off

mkdir st-win64
cd st-win64
cmake -G "Visual Studio 16 2019" ../../src/engine
cd ..
