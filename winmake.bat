echo off
rmdir /s /q build/
mkdir build 
mkdir build\dll 
mkdir build\tests 
mkdir build\lib 
mkdir build\core 
mkdir build\lang\ 
mkdir build\lang\builtin 
mkdir build\lang\builtin\modules
mingw32-make
