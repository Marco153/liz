cls
cl /O2  /Zi src/main.cpp /I src/include /I "C:\Program Files (x86)\OpenAL 1.1 SDK\include" /I "C:\Program Files (x86)\libsndfile-1.2.2-win64\include" /MDd /Fe./build/liz.exe /link /libpath:src/lib/  Ole32.lib  glfw3.lib OpenAL32.lib gdi32.lib user32.lib shell32.lib sndfile.lib opengl32.lib /NODEFAULTLIB:MSVCRT
