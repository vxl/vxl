//:
// \file
#ifndef gevd_pixel_h_
#define gevd_pixel_h_

#include "gevd_bufferxy.h"

typedef unsigned char byte;                     // 8-bit  [0 255]
const int bits_per_ptr = sizeof(void*) * 8;
const int bits_per_float = sizeof(float) * 8;   // number of bits for a pixel
const int bits_per_short = sizeof(short) * 8;
const int bits_per_char = sizeof(char) * 8;
const int bits_per_byte = sizeof(byte) * 8;


// These macros will be used to get pixels with known type.
// GetElementAddr is now overloaded with both gevd_bufferxy and const gevd_bufferxy.

//: Get reference to pixel as a byte/char, unsigned/signed char (8-bit) number, at indexes (x, y) in buf.
// This function is used to get and set pixel on non-const gevd_bufferxy.

inline byte&
bytePixel(gevd_bufferxy& buf, int x, int y)
{
  return (*((byte*) buf.GetElementAddr(x,y)));
}

//: Get reference to pixel as a byte/char, unsigned/signed char (8-bit) number, at indexes (x, y) in buf.
// This function is used to get and set pixel on non-const gevd_bufferxy.

inline char&
charPixel(gevd_bufferxy& buf, int x, int y)
{
  return (*((char*) buf.GetElementAddr(x,y)));
}

//: Get reference to pixel as a short (16-bit) number, at indexes (x, y) in buf.

inline short&
shortPixel(gevd_bufferxy& buf, int x, int y)
{
  return (*((short*) buf.GetElementAddr(x,y)));
}

//: Get reference to pixel as an unsigned short (16-bit) number, at indices (x, y) in buf.

inline unsigned short&
ushortPixel(gevd_bufferxy& buf, int x, int y)
{
  return (*((unsigned short*) buf.GetElementAddr(x,y)));
}

//: Get reference to pixel as a float number, at indexes (x, y) in buf.
// float is generally used to avoid overflow and unnecessary conversion
// between float/double for math computation and byte/short for storage.

inline float&
floatPixel(gevd_bufferxy& buf, int x, int y)
{
  return (*((float*) buf.GetElementAddr(x,y)));
}

//: Get reference to pixel as a pointer, at indexes (x, y).

inline void*&
ptrPixel(gevd_bufferxy& buf, int x, int y)
{
  return (*((void**) buf.GetElementAddr(x,y)));
}

//: Get reference to pixel as a byte, at indexes (x, y).

inline const byte&
bytePixel(const gevd_bufferxy& buf, int x, int y)
{
  return (*((const byte*) buf.GetElementAddr(x,y)));
}


//: Get reference to pixel as a char, at indexes (x, y).

inline const char&
charPixel(const gevd_bufferxy& buf, int x, int y)
{
  return (*((const char*)buf.GetElementAddr(x,y)));
}

//: Get reference to pixel as a short, at indexes (x, y).

inline const short&
shortPixel(const gevd_bufferxy& buf, int x, int y)
{
  return (*((const short*)buf.GetElementAddr(x,y)));
}

//: Get reference to pixel as an unsigned short, at indices (x, y).

inline const unsigned short&
ushortPixel(const gevd_bufferxy& buf, int x, int y)
{
  return (*((const unsigned short*)buf.GetElementAddr(x,y)));
}

//: Get reference to pixel as a float, at indexes (x, y).

inline const float&
floatPixel(const gevd_bufferxy& buf, int x, int y)
{
  return (*((const float*)buf.GetElementAddr(x,y)));
}

//: Get const reference to pixel at indexes (x, y).
// This function is used to get pixel on const gevd_bufferxy,
// as a const reference to a byte/char/short/float/void*.

inline void*const&
ptrPixel(const gevd_bufferxy& buf, int x, int y)
{
  return (*((void*const*)buf.GetElementAddr(x,y)));
}

#endif
