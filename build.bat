@echo off
if not exist build mkdir build
pushd build
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64
cl /MTd /nologo /fp:fast /Gm- /GR- /EHa- /Zo /Od /Oi /WX /W4 /FC /Z7 /wd4189 /wd4505 /wd4201 /wd4100 ..\cg.cpp /link /INCREMENTAL:NO /OPT:REF user32.lib gdi32.lib
popd
