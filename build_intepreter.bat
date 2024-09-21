cls
cl  /Zi src/main.cpp /I src/include /MDd /Fe./build/liz.exe /link /libpath:src/lib/ glfw3.lib gdi32.lib user32.lib shell32.lib opengl32.lib /NODEFAULTLIB:MSVCRT
