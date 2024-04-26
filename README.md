# Description:

This is a library that allows you capture a still image of window shown on the top level and capture a image. If the window is obscured, or behind another  window or minimized, the program brings it to
front to capture the image.

Depending on how you compile it can either save the image in .bmp format or it can convert the image data in memory to a matrix that can be processed by opencv. 
The libray does not contain a main function therefore you have inlcude the `api.h` in your project and `CaptureWindow(const wchar_t* exeName)` function to capture the window. 
However a test main function is provided which you can compile using the following commands:

Compile option for .bmp
```
g++ os_windows.cpp -o os_windows.exe -lgdi32 -DSAVE_BMP -DTEST -g
```

Compile option for opencv matrix return for further processing
```
g++ os_windows.cpp -o os_windows.exe -I"path/yourOpencvIncludeDir" -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lgdi32 -DSAVE_BMP -DTEST -g
```

