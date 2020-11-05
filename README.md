# convert2h
A very straightforward CLI tool to convert images to libultra compatible texure header files.

## GNU Compiler Supported
To build the project type:
```
$ make
```
To test it out with a PNG:
*Currently only fullscreen conversion is possible (320x240). Fix coming very soon.*
```
$ ./png2c full_screen_image.png -f
```

## Dependencies
*CImg did not support transparency. The current version dropped CImg in favor of [libPNG](http://www.libpng.org/pub/png/libpng.html)*

* [libPNG]
* [TCLAP](http://tclap.sourceforge.net/)
