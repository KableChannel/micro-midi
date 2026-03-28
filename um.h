// Copyright (c) Caleb Klomparens
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#ifndef __UM
#define __UM

#if defined(_WIN32) && defined(BUILD_SHARED_LIBS)
  #if defined(UM_EXPORT_LIBRARY)
    #define UM_EXPORT __declspec(dllexport)
  #else
    #define UM_EXPORT __declspec(dllimport)
  #endif 
#else
  #define UM_EXPORT
#endif // defined(_WIN32)

typedef enum
{
    UM_DEVICETYPE_INPUT,
    UM_DEVICETYPE_OUTPUT
} um_DeviceType;

typedef struct
{
    const char* name;
    um_DeviceType type;
} um_Device;


void um_list_devices(um_Device** outDevices, unsigned* outNumDevices);

void um_open_device(um_Device* inDevice);
void um_close_device(um_Device* inDevice);

#endif __UM