@echo off
REM set CommonCompilerFlags =-MTd -nologo -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -DHANDMADE_INTERNAL=1 -DHANDMADE_SLOW=1 -DHANDMADE_WIN32=1 -FC
set CommonLinkerFlags = -incremental:no  user32.lib gdi32.lib winmm.lib
mkdir ..\build
pushd ..\build

call "c:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
set path = d:\handmadehero\misc;%path%

del *.pdb > NUL 3 > NUL

cl -Zi d:\loz\code\handmade.cpp -Fmhandmade.map -LD /link -incremental:no -opt:ref -PDB:handmade_%random%.pdb  -EXPORT:gameUpdateAndRender
cl -Zi d:\loz\code\win32_handmade.cpp /link -incremental:no   user32.lib gdi32.lib winmm.lib

d