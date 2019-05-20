@echo off
if not exist "Build" mkdir Build
if not exist "Build\Web" mkdir "Build\Web"

C:\APPS\emsdk\emscripten\1.38.30\emcc.bat src/main.cpp -s USE_WEBGL2=1 -s USE_SDL=2 -o Build\Web\out.html