# convert2h
A very straightforward CLI tool to convert images to libultra compatible texure header files.

## GNU Compiler Supported
To build the project type:
```
$ make
```
To test it out with a PNG:
```
$ ./convert2h texture.png
```

## Dependencies
* [CImg](http://cimg.eu/)
*CImg does not support transparency. The upcoming version 2 will no longer utilize the CImg library.*
