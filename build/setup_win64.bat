@echo off

mkdir st-win64
cd st-win64
cmake -G "Visual Studio 17 2022" ../../src/engine
cd ..
