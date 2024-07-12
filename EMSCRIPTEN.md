# Emscripten

## Build

```
autoreconf
emconfigure ./configure
emmake make
```

## Link

```
em++ -flto -O3 -fno-rtti -fno-exceptions *.o libalut.a libftgl.a libGL.a libGLU.a libglpng.a -o
 index.html -lopenal -sUSE_SDL=2 -sUSE_LIBPNG -sUSE_FREETYPE -sFULL_ES2 -lGL -sASYNCIFY -sASYNCIFY_IGNORE_INDIRECT -sASYNCIFY_ONLY=@../funcs.txt -
sENVIRONMENT=web --preload-file ../data/@data/ --closure 1
```
