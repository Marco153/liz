### Warning

This language is not at all production ready, it changes all the time, so dont try to do anything serious with it.

## How to

At the moment, the compiler only compiles a whole folder with files inside.
It has two commands: ```run``` and ```webgame```.

run will compile and run the program at the same time

```liz.exe run path/to/folder```

webgame will compile and output a index.html file to a specified path. It's similar to run, in that it also compiles a whole folder, but it differs from it because it also expects the folder to have another folder called a "image" and the folder containing the code to be named "files", within which contains the
game textures. Webgame will use base64 to encode both the wasm code and the textures data in some html tags, so that it doesnt need to do any fetching.

```liz.exe webgame path/to/folder path/to/output```
