emcc -flto -O3 *.o libalut.a libftgl.a libGL.a libGLU.a libglpng.a -o index.html -lopenal -sUSE_SDL=2 -sUSE_LIBPNG -sUSE_FREETYPE -sASYNCIFY -sFULL_ES2 --preload-file data/ --closure 1
