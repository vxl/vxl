#ifndef vsl_binary_explicit_io_h_
#define vsl_binary_explicit_io_h_

//:
// \file
// \author Ian Scott (Manchester) April 2001
// \brief Byte-swapping, arbitrary length integer conversion, and explicit I/O
// Include this file if you wish to do binary IO for large numbers of
// multi-byte fundamental types (short, int, long, float, double) at the same
// time, or if you want to perform integer IO using fixed size encoding.
//
// If you want to read or write a large number of floating points, then;
// \verbatim
// Floats and doubles need byte swapped, and this can be done in situ.
// Shorts, ints and longs need converted to/from the arbitrary length format.
// \endverbatim
//
// Implementation details:
//
// The arbitrary length encoding takes the number and breaks it into 7 bit
// nibbles. Each nibble is saved with the 8th bit set if this is the last byte.
// The nibbles are stored in little endian order. ie The first byte of the
// encoded format represents the least significant 7 bits.
// 
// The algorithm used to encode an unsigned value works as follows
// \verbatim
//  while value is greater than 2^7-1
//    emit 0 bit
//    emit least significant 7 bits of value.
//    shift value right 7 bits
//   emit bit 1
//  emit value embedded in 7 bits
// \endverbatim

#include <vxl_config.h>
#include "vsl_binary_io.h"

// Both VXL_LITTLE_ENDIAN && VXL_BIG_ENDIAN should be defined
// One should equal 1 and the other equal 0;
#if VXL_LITTLE_ENDIAN == VXL_BIG_ENDIAN
  extern "There is a problem with the ENDIAN indication macros.";
#endif
#if VXL_LITTLE_ENDIAN+VXL_BIG_ENDIAN != 1
  extern "There is a problem with the ENDIAN indication macros.";
#endif

/////////////////////////////////////////////////////////////////////////


//: Perform byte swapping.
// Where appropriate, swaps pairs of bytes (behaviour is system dependent)
// Apply this function to your floating-point data to convert from system
// format to I/O format. Apply the same function to do the reverse conversion.
//
// The standard I/O format is little-endian. The code assumes that
// your system's floats and doubles are stored in memory in either
// little-endian or big-endian IEEE floating point formats.
//
// Note: There is no point in #ifdef-ing out calls to byte-swapping if you
// are on a little-endian machine. An optimising compiler will inline the
// function to nothing for little-endian machines anyway.
inline void vsl_swap_bytes( char * ptr, int nbyte, int nelem = 1)
{
#if VXL_LITTLE_ENDIAN 
  return;
#else
  // If the byte order of the file
  // does not match the intel byte order
  // then the bytes should be swapped
  char temp;
  char *ptr1, *ptr2;

  int nbyte2 = nbyte/2;
  for (int n = 0; n < nelem; n++ ) {
    ptr1 = ptr;
    ptr2 = ptr1 + nbyte - 1;
    for (int i = 0; i < nbyte2; i++ ) {
      temp = *ptr1;
      *ptr1++ = *ptr2;
      *ptr2-- = temp;
    }

    ptr += nbyte;
  }
#endif
}



/////////////////////////////////////////////////////////////////////////

//: The maximum length of buffer to use with arbitrary length integers
// This macro tells you the size of buffer you need when using
// vsl_convert_ints_to_arbitrary_length().
// You should give the macro the size of the type you want to convert.
// If you are converting several integers at once, multiply the value by
// the number of integers.
#define VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(size_of_type) \
  (((size_of_type * 8)/7) + ((((size_of_type * 8) % 7) == 0) ? 0: 1))


/////////////////////////////////////////////////////////////////////////

//: Encode an array of ints into an arbitrary length format.
// The return value is the number of bytes used.
// buffer should be at least as long as
// VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(unsigned long)) * count
inline unsigned vsl_convert_to_arbitrary_length(const unsigned long* ints,
                                          unsigned char *buffer,
                                          unsigned long count = 1)
{
  unsigned char* ptr = buffer;
  while (count-- > 0)
  {
// The inside of this loop is run once per integer 
    unsigned long v = *(ints++);
    while (v > 127)
    {
      *(ptr++) = v & 127;
      v >>= 7;
    }
    *(ptr++) = v | 128;
  }
  return (unsigned int)(ptr - buffer);
}


/////////////////////////////////////////////////////////////////////////

//: Decode a buffer of arbitrary length integers
// Converts from the integers from the arbitrary length format into
// an array of normal longs.
// The return value is the number of bytes used.
// ints* should be at least as long as count.
// The functions will abort if the input data will not fit into
// an unsigned long.
inline unsigned vsl_convert_from_arbitrary_length(const unsigned char* buffer,
                                            unsigned long *ints,
                                            unsigned long count = 1)
{
  const unsigned char* ptr = buffer;
  while (count-- > 0)
  {
// The inside of this loop is run once per integer 
    unsigned long v = 0;
    unsigned char b = *(ptr++);
    int bitsLoaded = 0;
    while ((b & 128) == 0)
    {
      v += ((unsigned long)b) << bitsLoaded;
      bitsLoaded += 7;
      b = *(ptr++);
    }
// At the end of the loop, the last seven bits have not been added
// First check that number has not and will not overflow
    int bitsLeft = sizeof(unsigned long)*8 - bitsLoaded;
    if (bitsLeft < 7)
    {
      if (bitsLeft <= 0 || ((b & 127) >> bitsLeft) != 0)
      {
        vcl_cerr << "\nI/O ERROR: vsl_convert_from_arbitrary_length() has "
                 << "attempted to\n convert a number that is too large "
                 << "to fit into an unsigned long" << vcl_endl; 
        vcl_cerr << "Aborting with " << count << " integers left to convert"
                 << vcl_endl;
        abort();
      }
    }      
    
// Now add the last 7 bits.
    *(ints++) = v + ( ((unsigned long)(b & 127)) << bitsLoaded);
  }
  return (unsigned int)(ptr - buffer);
}


/////////////////////////////////////////////////////////////////////////

//: Encode an array of ints into an arbitrary length format.
// The return value is the number of bytes used.
// buffer should be at least as long as
// VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(signed long)) * count
inline unsigned vsl_convert_to_arbitrary_length(const signed long* ints,
                                          unsigned char *buffer,
                                          unsigned long count = 1)
{
  unsigned char* ptr = buffer;
  while (count-- > 0)
  {
// The inside of this loop is run once per integer 
    signed long v = *(ints++);
    while (v > 63 || v < -64)
    {
      *(ptr++) = v & 127;
      v >>= 7;
    }
    *(ptr++) = (v & 127) | 128;
  }
  
  return (unsigned int)(ptr - buffer);
}

/////////////////////////////////////////////////////////////////////////

//: Decode a buffer of arbitrary length integers
// Converts from the integers from the arbitrary length format into
// an array of normal longs.
// The return value is the number of bytes used.
// ints* should be at least as long as count.
// The functions will abort if the input data will not fit into
// a signed long.
inline unsigned vsl_convert_from_arbitrary_length(const unsigned char* buffer,
                                            signed long *ints,
                                            unsigned long count = 1)
{
  const unsigned char* ptr = buffer;
  while (count-- > 0)
  {
// The inside of this loop is run once per integer 
    signed long v = 0; // The value being loaded
    unsigned char b= *(ptr++);
    int bitsLoaded = 0;
    while ((b & 128) == 0)
    {
      v += ((signed long)b) << bitsLoaded;
      bitsLoaded += 7;
      b = *(ptr++);
    }

// At the end of the loop, the last seven bits have not been added
// First check that number has not and will not overflow
    int bitsLeft = sizeof(signed long)*8 - bitsLoaded;
    if (bitsLeft < 7)
    {
      if (bitsLeft <= 0 ||
            b & 64 ?
              (((signed char)b >> (bitsLeft-1)) != -1) :
              (((b & 127) >> (bitsLeft-1)) != 0) )
      {
        vcl_cerr << "\nI/O ERROR: vsl_convert_from_arbitrary_length() has "
                 << "attempted to\n convert a number that is too large "
                 << "to fit into a signed long" << vcl_endl;
        vcl_cerr << "Aborting with " << count << " integers left to convert"
                 << vcl_endl;
        abort();
      }
    }        

// Now add the last 1<=n<=7 bits.
    *(ints++) = v |            // the stuff found before the final 7 bits
      (((signed long)(b & 63)) << bitsLoaded) | // the value of the penultimate 6 bits
      (((signed long)(b & 64)) ? (-64 << bitsLoaded) : 0); // the value of the final bit.
  }
  return (unsigned int)(ptr - buffer);
}


/////////////////////////////////////////////////////////////////////////

//: Encode an array of ints into an arbitrary length format.
// The return value is the number of bytes used.
// buffer should be at least as long as
// VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(unsigned int)) * count
inline unsigned vsl_convert_to_arbitrary_length(const unsigned int* ints,
                                          unsigned char *buffer,
                                          unsigned long count = 1)
{
  unsigned char* ptr = buffer;
  while (count-- > 0)
  {
// The inside of this loop is run once per integer 
    unsigned int v = *(ints++);
    while (v > 127)
    {
      *(ptr++) = v & 127;
      v >>= 7;
    }
    *(ptr++) = v | 128;
  }
  return (unsigned int)(ptr - buffer);
}


/////////////////////////////////////////////////////////////////////////

//: Decode a buffer of arbitrary length integers
// Converts from the integers from the arbitrary length format into
// an array of normal ints.
// The return value is the number of bytes used.
// ints* should be at least as long as count.
// The functions will abort if the input data will not fit into
// an unsigned int.
inline unsigned vsl_convert_from_arbitrary_length(const unsigned char* buffer,
                                            unsigned int *ints,
                                            unsigned long count = 1)
{
  const unsigned char* ptr = buffer;
  while (count-- > 0)
  {
// The inside of this loop is run once per integer 
    unsigned int v = 0;
    unsigned char b = *(ptr++);
    int bitsLoaded = 0;
    while ((b & 128) == 0)
    {
      v += ((unsigned int)b) << bitsLoaded;
      bitsLoaded += 7;
      b = *(ptr++);
    }

// At the end of the loop, the last seven bits have not been added
// First check that number has not and will not overflow
    int bitsLeft = sizeof(unsigned int)*8 - bitsLoaded;
    if (bitsLeft < 7)
    {
      if (bitsLeft <= 0 || ((b & 127) >> bitsLeft) != 0)
      {
        vcl_cerr << "\nI/O ERROR: vsl_convert_from_arbitrary_length() has "
                 << "attempted to\n convert a number that is too large "
                 << "to fit into an unsigned int" << vcl_endl;
        vcl_cerr << "Aborting with " << count << " integers left to convert"
                 << vcl_endl;
       abort();
      }
    }      
    
// Now add the last 7 bits.
    *(ints++) = v + ( ((unsigned int)(b & 127)) << bitsLoaded);
  }
  return (unsigned int)(ptr - buffer);
}


/////////////////////////////////////////////////////////////////////////

//: Encode an array of ints into an arbitrary length format.
// The return value is the number of bytes used.
// buffer should be at least as long as
// VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(signed int)) * count
inline unsigned vsl_convert_to_arbitrary_length(const signed int* ints,
                                          unsigned char *buffer,
                                          unsigned long count = 1)
{
  unsigned char* ptr = buffer;
  while (count-- > 0)
  {
// The inside of this loop is run once per integer 
    signed int v = *(ints++);
    while (v > 63 || v < -64)
    {
      *(ptr++) = v & 127;
      v >>= 7;
    }
    *(ptr++) = (v & 127) | 128;
  }
  
  return (unsigned int)(ptr - buffer);
}


/////////////////////////////////////////////////////////////////////////

//: Decode a buffer of arbitrary length integers
// Converts from the integers from the arbitrary length format into
// an array of normal ints.
// The return value is the number of bytes used.
// ints* should be at least as long as count.
// The functions will abort if the input data will not fit into
// a signed int.
inline unsigned vsl_convert_from_arbitrary_length(const unsigned char* buffer,
                                            signed int *ints,
                                            unsigned long count = 1)
{
  const unsigned char* ptr = buffer;
  while (count-- > 0)
  {
// The inside of this loop is run once per integer 
    signed int v = 0; // The value being loaded
    unsigned char b= *(ptr++);
    int bitsLoaded = 0;
    while ((b & 128) == 0)
    {
      v += ((signed int)b) << bitsLoaded;
      bitsLoaded += 7;
      b = *(ptr++);
    }

// At the end of the loop, the last seven bits have not been added
// First check that number has not and will not overflow
    int bitsLeft = sizeof(signed int)*8 - bitsLoaded;
    if (bitsLeft < 7)
    {
      if (bitsLeft <= 0 ||
            b & 64 ?
              (((signed char)b >> (bitsLeft-1)) != -1) :
              (((b & 127) >> (bitsLeft-1)) != 0) )
      {
        vcl_cerr << "\nI/O ERROR: vsl_convert_from_arbitrary_length() has "
                 << "attempted to\n convert a number that is too large "
                 << "to fit into a signed int" << vcl_endl;
        vcl_cerr << "Aborting with " << count << " integers left to convert"
                 << vcl_endl;
        abort();
      }
    }        


// Now add the last 1<=n<=7 bits.
    *(ints++) = v |            // the stuff found before the final 7 bits
      ( ((signed int)(b & 63)) << bitsLoaded) | // the value of the penultimate 6 bits
      ( ((signed int)(b & 64)) ? (-64 << bitsLoaded) : 0); // the value of the final bit.
  }
  return (unsigned int)(ptr - buffer);
}


/////////////////////////////////////////////////////////////////////////

//: Encode an array of ints into an arbitrary length format.
// The return value is the number of bytes used.
// buffer should be at least as long as
// VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(unsigned short)) * count
inline unsigned vsl_convert_to_arbitrary_length(const unsigned short* ints,
                                          unsigned char *buffer,
                                          unsigned long count = 1)
{
  unsigned char* ptr = buffer;
  while (count-- > 0)
  {
// The inside of this loop is run once per integer 
    unsigned short v = *(ints++);
    while (v > 127)
    {
      *(ptr++) = v & 127;
      v >>= 7;
    }
    *(ptr++) = v | 128;
  }
  return (unsigned int)(ptr - buffer);
}


/////////////////////////////////////////////////////////////////////////

//: Decode a buffer of arbitrary length integers
// Converts from the integers from the arbitrary length format into
// an array of normal ints.
// The return value is the number of bytes used.
// ints* should be at least as long as count.
// The functions will abort if the input data will not fit into
// an unsigned short.
inline unsigned vsl_convert_from_arbitrary_length(const unsigned char* buffer,
                                            unsigned short *ints,
                                            unsigned long count = 1)
{
  const unsigned char* ptr = buffer;
  while (count-- > 0)
  {
// The inside of this loop is run once per integer 
    unsigned short v = 0;
    unsigned char b = *(ptr++);
    int bitsLoaded = 0;
    while ((b & 128) == 0)
    {
      v += ((unsigned short)b) << bitsLoaded;
      bitsLoaded += 7;
      b = *(ptr++);
    }
    
// At the end of the loop, the last seven bits have not been added
// First check that number has not and will not overflow
    int bitsLeft = sizeof(unsigned short)*8 - bitsLoaded;
    if (bitsLeft < 7)
    {
      if (bitsLeft <= 0 || ((b & 127) >> bitsLeft) != 0)
      {
        vcl_cerr << "\nI/O ERROR: vsl_convert_from_arbitrary_length() has "
                 << "attempted to\n convert a number that is too large "
                 << "to fit into an unsigned short" << vcl_endl;
        vcl_cerr << "Aborting with " << count << " integers left to convert"
                 << vcl_endl;
        abort();
      }
    }      
    
// Now add the last 7 bits.
    *(ints++) = v + ( ((unsigned short)(b & 127)) << bitsLoaded);
  }
  return (unsigned int)(ptr - buffer);
}


/////////////////////////////////////////////////////////////////////////

//: Encode an array of ints into an arbitrary length format.
// The return value is the number of bytes used.
// buffer should be at least as long as
// VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(signed short)) * count
inline unsigned vsl_convert_to_arbitrary_length(const signed short* ints,
                                          unsigned char *buffer,
                                          unsigned long count = 1)
{
  unsigned char* ptr = buffer;
  while (count-- > 0)
  {
// The inside of this loop is run once per integer 
    signed short v = *(ints++);
    while (v > 63 || v < -64)
    {
      *(ptr++) = v & 127;
      v >>= 7;
    }
    *(ptr++) = (v & 127) | 128;
  }
  
  return (unsigned int)(ptr - buffer);
}


/////////////////////////////////////////////////////////////////////////

//: Decode a buffer of arbitrary length integers
// Converts from the integers from the arbitrary length format into
// an array of normal ints.
// The return value is the number of bytes used.
// ints* should be at least as long as count.
// The functions will abort if the input data will not fit into
// a signed short.
inline unsigned vsl_convert_from_arbitrary_length(const unsigned char* buffer,
                                            signed short *ints,
                                            unsigned long count = 1)
{
  const unsigned char* ptr = buffer;
  while (count-- > 0)
  {
// The inside of this loop is run once per integer 
    
    signed short v = 0; // The value being loaded
    unsigned char b= *(ptr++);
    int bitsLoaded = 0;
    while ((b & 128) == 0)
    {
      v += ((signed short)b) << bitsLoaded;
      bitsLoaded += 7;
      b = *(ptr++);
    }

// At the end of the loop, the last seven bits have not been added
// Now check that number has not and will not overflow
    int bitsLeft = sizeof(signed short)*8 - bitsLoaded;
    if (bitsLeft < 7)
    {
      if (bitsLeft <= 0 ||
            b & 64 ?
              (((signed char)b >> (bitsLeft-1)) != -1) :
              (((b & 127) >> (bitsLeft-1)) != 0) )
      {
        vcl_cerr << "\nI/O ERROR: vsl_convert_from_arbitrary_length() has "
                 << "attempted to\n convert a number that is too large "
                 << "to fit into a signed short" << vcl_endl;
        vcl_cerr << "Aborting with " << count << " integers left to convert"
                 << vcl_endl;
        abort();
      }
    }        

// Now add the last 1<=n<=7 bits.
    *(ints++) = v |            // the stuff found before the final 7 bits
      ( ((signed short)(b & 63)) << bitsLoaded) | // the value of the penultimate 6 bits
      ( ((signed short)(b & 64)) ? (-64 << bitsLoaded) : 0); // the value of the final bit.

  }
  return (unsigned int)(ptr - buffer);
}


/////////////////////////////////////////////////////////////////////////

//: Write an unsigned int as 16 bits to vsl_b_ostream
// If your signed int cannot be represented in 16 bits (e.g. on a 32 bit
// platform) the function will abort().
// 
// Warning: This function should be used infrequently and carefully. Under
// all normal cicurmstances, the generic vsl_b_read and vsl_b_write in
// vsl_binary_io.h will be perfectly adequate.
//
// You must use vsl_b_read_uint_16() to read the value saved with this
// function.
inline void vsl_b_write_uint_16(vsl_b_ostream& os, unsigned long n )
{
  assert(n < (1 << 16));
  vsl_swap_bytes(( char* )&n, sizeof(long) );
  os.os().write( ( char* )&n, 2 );
}

//: Read an unsigned int as 16 bits from vsl_b_istream
// 
// Warning: This function should be used infrequently and carefully. Under
// all normal cicurmstances, the generic vsl_b_read and vsl_b_write in
// vsl_binary_io.h will be perfectly adequate.
//
// This function will only read values saved using vsl_b_write_uint_16().
inline void vsl_b_read_uint_16(vsl_b_istream& is, unsigned long& n )
{
  n = 0;
  is.is().read( ( char* )&n, 2 );
  vsl_swap_bytes(( char* )&n, sizeof(long) );
}


/////////////////////////////////////////////////////////////////////////

//: Write a signed int as 16 bits to vsl_b_ostream
// If your signed int cannot be represented in 16 bits (e.g. on a 32 bit
// platform) the function will abort().
// 
// Warning: This function should be used infrequently and carefully. Under
// all normal cicurmstances, the generic vsl_b_read and vsl_b_write in
// vsl_binary_io.h will be perfectly adequate.
//
// You must vsl_b_read_int_16() to read the value saved with this function.
inline void vsl_b_write_int_16(vsl_b_ostream& os, long n )
{
  assert(n < 32768 && n >= - 32768);
  vsl_swap_bytes(( char* )&n, sizeof(long) );
  os.os().write( ( char* )&n, 2 );
}

//: Read a signed int as 16 bits from vsl_b_istream
// 
// Warning: This function should be used infrequently and carefully. Under
// all normal cicurmstances, the generic vsl_b_read and vsl_b_write in
// vsl_binary_io.h will be perfectly adequate.
//
// This function will only read values saved using vsl_b_write_int_16().
inline void vsl_b_read_int_16(vsl_b_istream& is, long& n )
{
  is.is().read( ( char* )&n, 2 );
  if (*(((unsigned char*)&n)+1) & 128 == 1)
  {  
    vsl_swap_bytes(( char* )&n, sizeof(long) );
    n |= -65536l;
  }
  else
  {
    vsl_swap_bytes(( char* )&n, sizeof(long) );
    n &= 65535l;
  }

}


#endif // vsl_binary_explicit_io_h_
