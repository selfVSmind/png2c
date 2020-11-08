# png2c
A very straightforward CLI tool to convert images to libultra compatible texure header files.

## GNU Compiler Supported
To build the project type:
```
$ make
```
To test it out with a full screen PNG (320x240):
```
$ ./png2c full_screen_image.png -f
```
To convert and standard texture:
```
$ ./png2c my_texture.png
```

## Dependencies
*CImg did not support transparency. The current version dropped CImg in favor of [libPNG](http://www.libpng.org/pub/png/libpng.html)*

* [libPNG](http://www.libpng.org/pub/png/libpng.html)
* [TCLAP](http://tclap.sourceforge.net/)
