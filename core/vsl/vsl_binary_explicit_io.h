// This is core/vsl/vsl_binary_explicit_io.h
#ifndef vsl_binary_explicit_io_h_
#define vsl_binary_explicit_io_h_
//:
// \file
// \brief Byte-swapping, arbitrary length integer conversion, and explicit I/O
// \author Ian Scott (Manchester) April 2001
//
// Include this file if you want to perform integer IO using fixed size encoding.
//
// If you want to read or write a large number of floating points, then;
// - Floats and doubles need byte swapped, and this can be done in situ.
// - Shorts, ints and longs need converted to/from the arbitrary length format.
//
// \par Implementation details:
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
//  emit bit 1
//  emit value embedded in 7 bits
// \endverbatim

#include <vxl_config.h>
#include <vcl_cassert.h>
#include <vcl_cstring.h>
#include <vcl_iostream.h>
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


//: Perform byte swapping in situ
// Where appropriate, swaps pairs of bytes (behaviour is system dependent)
// Apply this function to your floating-point data to convert from system
// format to I/O format. Apply the same function to do the reverse conversion.
//
// \param ptr   The buffer to be byte-swapped.
// \param nbyte The length of the fundamental type, e.g. sizeof(float).
// \param nelem The number of elements in the buffer (default: 1).
//
// The standard I/O format is little-endian. The code assumes that
// your system's floats and doubles are stored in memory in either
// little-endian or big-endian IEEE floating point formats.
//
// Note: There is no point in #ifdef-ing out calls to byte-swapping if you
// are on a little-endian machine. An optimising compiler will inline the
// function to nothing for little-endian machines anyway.
//
// If your computer doesn't use IEEE format reals, then we really should
// redesign the floating point IO.
// Proposed design notes:
// Should do conversion to and from a buffer, rather than in place,
// (since size not known in general)
// double and reals should be converted to IEEE format.
// Someone needs to write a long double format anyway.
// Don't forget to fix all the code that calls vsl_swap_bytes.
// Really should check anything that #includes this file.

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

//: Perform byte swapping to a buffer
// Same as vsl_swap_bytes, but saves the results in a buffer.
// In general use vsl_swap_bytes where possible, because it is faster.
inline void vsl_swap_bytes_to_buffer( const char * source, char * dest, int nbyte, int nelem = 1)
{
#if VXL_LITTLE_ENDIAN
  vcl_memcpy(dest, source, nbyte * nelem);
#else

  assert(source != dest);

  // If the byte order of the file
  // does not match the intel byte order
  // then the bytes should be swapped

  const int nbyte_x_2 = nbyte*2;
  dest += nbyte - 1;

  for (int n = 0; n < nelem; n++ )
  {
    for (int i = 0; i < nbyte; i++ )
      *dest-- = *source++;

    dest += nbyte_x_2;
  }
#endif
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define macro( T ) \
inline const char * vsl_type_string(T /*dummy*/) { return #T; }
macro (short);
macro (unsigned short);
macro (int);
macro (unsigned int);
macro (long);
macro (unsigned long);
#if VXL_HAS_INT_64 && !VXL_INT_64_IS_LONG
macro (vxl_int_64);
macro (vxl_uint_64);
#endif
#if 0
// This test will be replaced with !VCL_PTRDIFF_T_IS_A_STANDARD_TYPE
// When that macro is working.
macro(vcl_ptrdiff_t);
macro(vcl_size_t);
#endif
#undef macro
#endif // DOXYGEN_SHOULD_SKIP_THIS

//: The maximum length of buffer to use with arbitrary length integers
// This macro tells you the size of buffer you need when using
// vsl_convert_ints_to_arbitrary_length().
// You should give the macro the size of the type you want to convert.
// If you are converting several integers at once, multiply the value by
// the number of integers.
#define VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(size_of_type) \
  (((size_of_type * 8)/7) + ((((size_of_type * 8) % 7) == 0) ? 0: 1))


//: Implement arbitrary length conversion for unsigned integers.
// This function should only be used by this header file.
// Returns the number of bytes written
template <class T>
inline unsigned long vsl_convert_to_arbitrary_length_unsigned_impl(
  const T* ints, unsigned char *buffer, unsigned long count)
{
  unsigned char* ptr = buffer;
  while (count-- > 0)
  {
    // The inside of this loop is run once per integer
    T v = *(ints++);
    while (v > 127)
    {
      *(ptr++) = (unsigned char)(v & 127);
      v >>= 7;
    }
    *(ptr++) = (unsigned char)(v | 128);
  }
  return (unsigned long)(ptr - buffer);
}


//: Implement arbitrary length conversion for signed integers.
// This function should only be used by this header file.
// Returns the number of bytes written
template <class T>
inline unsigned long vsl_convert_to_arbitrary_length_signed_impl(
  const T* ints, unsigned char *buffer, unsigned long count)
{
  unsigned char* ptr = buffer;
  while (count-- > 0)
  {
    // The inside of this loop is run once per integer
    T v = *(ints++);
    while (v > 63 || v < -64)
    {
      *(ptr++) = (unsigned char)(v & 127);
      v >>= 7;
    }
    *(ptr++) = (unsigned char)((v & 127) | 128);
  }
  return (unsigned long)(ptr - buffer);
}


//: Implement arbitrary length conversion for signed integers.
// This function should only be used by this header file.
template <class T>
inline unsigned long vsl_convert_from_arbitrary_length_signed_impl(
  const unsigned char* buffer, T *ints, unsigned long count)
{
  assert (count != 0);
  const unsigned char* ptr = buffer;
  while (count-- > 0)
  {
    // The inside of this loop is run once per integer

    T v = 0; // The value being loaded
    unsigned char b= *(ptr++);
    int bitsLoaded = 0;
    while ((b & 128) == 0)
    {
      v += ((T)b) << bitsLoaded;
      bitsLoaded += 7;
      b = *(ptr++);
    }

    // At the end of the loop, the last seven bits have not been added
    // Now check that number has not and will not overflow
    int bitsLeft = sizeof(T)*8 - bitsLoaded;
    if (bitsLeft < 7)
    {
      if (bitsLeft <= 0 ||
          b & 64 ?
              (((signed char)b >> (bitsLeft-1)) != -1) :
              (((b & 127) >> (bitsLeft-1)) != 0) )
      {
        vcl_cerr << "\nI/O ERROR: vsl_convert_from_arbitrary_length(.., "
                 << vsl_type_string(T()) << "*,..)\n"
                 << "has attempted to convert a number that is too large to fit into a "
                 << vsl_type_string(T()) << '\n';
        return 0;
      }
    }

    // Now add the last 1<=n<=7 bits.
    *(ints++) = v |            // the stuff found before the final 7 bits
      ( ((T)(b & 63)) << bitsLoaded) | // the value of the penultimate 6 bits
      ( ((T)(b & 64)) ? (-64 << bitsLoaded) : 0); // the value of the final bit.
  }
  return (unsigned long)(ptr - buffer);
}

//: Implement arbitrary length conversion for unsigned integers.
// This function should only be used by this header file.
template <class T>
inline unsigned long vsl_convert_from_arbitrary_length_unsigned_impl(
  const unsigned char* buffer, T *ints, unsigned long count = 1)
{
  assert (count != 0);
  const unsigned char* ptr = buffer;
  while (count-- > 0)
  {
    // The inside of this loop is run once per integer
    T v = 0;
    unsigned char b = *(ptr++);
    int bitsLoaded = 0;
    while ((b & 128) == 0)
    {
      v += ((T)b) << bitsLoaded;
      bitsLoaded += 7;
      b = *(ptr++);
    }

    // At the end of the loop, the last seven bits have not been added
    // First check that number has not and will not overflow
    int bitsLeft = sizeof(T)*8 - bitsLoaded;
    if (bitsLeft < 7)
    {
      if (bitsLeft <= 0 || ((b & 127) >> bitsLeft) != 0)
      {
        vcl_cerr << "\nI/O ERROR: vsl_convert_from_arbitrary_length(.., "
                 << vsl_type_string(T()) << "*,..)\n"
                 << "has attempted to convert a number that is too large to fit into a "
                 << vsl_type_string(T()) << '\n';
        return 0;
      }
    }

    // Now add the last 7 bits.
    *(ints++) = v + ( ((T)(b & 127)) << bitsLoaded);
  }
  return (unsigned long)(ptr - buffer);
}

/////////////////////////////////////////////////////////////////////////

//: Encode an array of ints into an arbitrary length format.
// The return value is the number of bytes used.
// buffer should be at least as long as
// VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(unsigned long)) * count
inline unsigned long vsl_convert_to_arbitrary_length(const unsigned long* ints,
                                                     unsigned char *buffer,
                                                     unsigned long count = 1)
{
  return vsl_convert_to_arbitrary_length_unsigned_impl(ints, buffer, count);
}

/////////////////////////////////////////////////////////////////////////

//: Decode a buffer of arbitrary length integers
// Converts from the integers from the arbitrary length format into
// an array of normal longs.
// \param buffer The buffer to be converted.
// \param count  Number of integers expected. Cannot be zero.
// \param ints   should point to a buffer at least as long as count.
// \return the number of bytes used, or zero on error.
inline unsigned long vsl_convert_from_arbitrary_length(const unsigned char* buffer,
                                                       unsigned long *ints,
                                                       unsigned long count = 1)
{
  return vsl_convert_from_arbitrary_length_unsigned_impl(buffer, ints, count);
}


/////////////////////////////////////////////////////////////////////////

//: Encode an array of ints into an arbitrary length format.
// The return value is the number of bytes used.
// buffer should be at least as long as
// VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(long)) * count
inline unsigned long vsl_convert_to_arbitrary_length(const long* ints,
                                                     unsigned char *buffer,
                                                     unsigned long count = 1)
{
  return vsl_convert_to_arbitrary_length_signed_impl(ints, buffer, count);
}

/////////////////////////////////////////////////////////////////////////

//: Decode a buffer of arbitrary length integers
// Converts from the integers from the arbitrary length format into
// an array of normal longs.
// \param buffer The buffer to be converted.
// \param count  Number of integers expected. Cannot be zero.
// \param ints   should point to a buffer at least as long as count.
// \return the number of bytes used, or zero on error.
inline unsigned long vsl_convert_from_arbitrary_length(const unsigned char* buffer,
                                                       long *ints,
                                                       unsigned long count = 1)
{
  return vsl_convert_from_arbitrary_length_signed_impl(buffer, ints, count);
}


/////////////////////////////////////////////////////////////////////////

//: Encode an array of ints into an arbitrary length format.
// The return value is the number of bytes used.
// buffer should be at least as long as
// VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(unsigned int)) * count
inline unsigned long vsl_convert_to_arbitrary_length(const unsigned int* ints,
                                                     unsigned char *buffer,
                                                     unsigned long count = 1)
{
  return vsl_convert_to_arbitrary_length_unsigned_impl(ints, buffer, count);
}


/////////////////////////////////////////////////////////////////////////

//: Decode a buffer of arbitrary length integers
// Converts from the integers from the arbitrary length format into
// an array of normal ints.
// \param buffer The buffer to be converted.
// \param count  Number of integers expected. Cannot be zero.
// \param ints   should point to a buffer at least as long as count.
// \return the number of bytes used, or zero on error.
inline unsigned long vsl_convert_from_arbitrary_length(const unsigned char* buffer,
                                                       unsigned int *ints,
                                                       unsigned long count = 1)
{
  return vsl_convert_from_arbitrary_length_unsigned_impl(buffer, ints, count);
}


/////////////////////////////////////////////////////////////////////////

//: Encode an array of ints into an arbitrary length format.
// The return value is the number of bytes used.
// buffer should be at least as long as
// VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(int)) * count
inline unsigned long vsl_convert_to_arbitrary_length(const int* ints,
                                                     unsigned char *buffer,
                                                     unsigned long count = 1)
{
  return vsl_convert_to_arbitrary_length_signed_impl(ints, buffer, count);
}


/////////////////////////////////////////////////////////////////////////

//: Decode a buffer of arbitrary length integers
// Converts from the integers from the arbitrary length format into
// an array of normal ints.
// \param buffer The buffer to be converted.
// \param count  Number of integers expected. Cannot be zero.
// \param ints   should point to a buffer at least as long as count.
// \return the number of bytes used, or zero on error.
inline unsigned long vsl_convert_from_arbitrary_length(const unsigned char* buffer,
                                                       int *ints,
                                                       unsigned long count = 1)
{
  return vsl_convert_from_arbitrary_length_signed_impl(buffer, ints, count);
}


/////////////////////////////////////////////////////////////////////////

//: Encode an array of ints into an arbitrary length format.
// The return value is the number of bytes used.
// buffer should be at least as long as
// VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(unsigned short)) * count
inline unsigned long vsl_convert_to_arbitrary_length(const unsigned short* ints,
                                                     unsigned char *buffer,
                                                     unsigned long count = 1)
{
  return vsl_convert_to_arbitrary_length_unsigned_impl(ints, buffer, count);
}


/////////////////////////////////////////////////////////////////////////

//: Decode a buffer of arbitrary length integers
// Converts from the integers from the arbitrary length format into
// an array of normal ints.
// \param buffer The buffer to be converted.
// \param count  Number of integers expected. Cannot be zero.
// \param ints   should point to a buffer at least as long as count.
// \return the number of bytes used, or zero on error.
inline unsigned long vsl_convert_from_arbitrary_length(const unsigned char* buffer,
                                                       unsigned short *ints,
                                                       unsigned long count = 1)
{
  return vsl_convert_from_arbitrary_length_unsigned_impl(buffer, ints, count);
}


/////////////////////////////////////////////////////////////////////////

//: Encode an array of ints into an arbitrary length format.
// The return value is the number of bytes used.
// buffer should be at least as long as
// VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(short)) * count
inline unsigned long vsl_convert_to_arbitrary_length(const short* ints,
                                                     unsigned char *buffer,
                                                     unsigned long count = 1)
{
  return vsl_convert_to_arbitrary_length_signed_impl(ints, buffer, count);
}


/////////////////////////////////////////////////////////////////////////

//: Decode a buffer of arbitrary length integers
// Converts from the integers from the arbitrary length format into
// an array of normal ints.
// \param buffer The buffer to be converted.
// \param count  Number of integers expected. Cannot be zero.
// \param ints   should point to a buffer at least as long as count.
// \return the number of bytes used, or zero on error.
inline unsigned long vsl_convert_from_arbitrary_length(const unsigned char* buffer,
                                                       short *ints,
                                                       unsigned long count = 1)
{
  return vsl_convert_from_arbitrary_length_signed_impl(buffer, ints, count);
}

/////////////////////////////////////////////////////////////////////////

#if VXL_HAS_INT_64 && !VXL_INT_64_IS_LONG

//: Decode a buffer of arbitrary length integers
// Converts from the integers from the arbitrary length format into
// an array of normal longs.
// \param buffer The buffer to be converted.
// \param count  Number of integers expected. Cannot be zero.
// \param ints   should point to a buffer at least as long as count.
// \return the number of bytes used, or zero on error.
inline unsigned long vsl_convert_from_arbitrary_length(const unsigned char* buffer,
                                                       vxl_uint_64 *ints,
                                                       unsigned long count = 1)
{
  return vsl_convert_from_arbitrary_length_unsigned_impl(buffer, ints, count);
}

//: Decode a buffer of arbitrary length integers
// Converts from the integers from the arbitrary length format into
// an array of normal longs.
// \param buffer The buffer to be converted.
// \param count  Number of integers expected. Cannot be zero.
// \param ints   should point to a buffer at least as long as count.
// \return the number of bytes used, or zero on error.
inline unsigned long vsl_convert_from_arbitrary_length(const unsigned char* buffer,
                                                       vxl_int_64 *ints,
                                                       unsigned long count = 1)
{
  return vsl_convert_from_arbitrary_length_signed_impl(buffer, ints, count);
}

//: Encode an array of ints into an arbitrary length format.
// The return value is the number of bytes used.
// buffer should be at least as long as
// VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(long)) * count
inline unsigned long vsl_convert_to_arbitrary_length(const vxl_int_64* ints,
                                                     unsigned char *buffer,
                                                     unsigned long count = 1)
{
  return vsl_convert_to_arbitrary_length_signed_impl(ints, buffer, count);
}

//: Encode an array of ints into an arbitrary length format.
// The return value is the number of bytes used.
// buffer should be at least as long as
// VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(unsigned long)) * count
inline unsigned long vsl_convert_to_arbitrary_length(const vxl_uint_64* ints,
                                                     unsigned char *buffer,
                                                     unsigned long count = 1)
{
  return vsl_convert_to_arbitrary_length_unsigned_impl(ints, buffer, count);
}

#endif // VXL_HAS_INT_64

/////////////////////////////////////////////////////////////////////////

#if 0
// This test will be replaced with !VCL_PTRDIFF_T_IS_A_STANDARD_TYPE
// When that macro is working.

//: Encode an array of ints into an arbitrary length format.
// The return value is the number of bytes used.
// buffer should be at least as long as
// VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(vcl_size_t)) * count
inline unsigned long vsl_convert_to_arbitrary_length(const vcl_size_t* ints,
                                                     unsigned char *buffer,
                                                     unsigned long count = 1)
{
  return vsl_convert_to_arbitrary_length_unsigned_impl(ints, buffer, count);
}


/////////////////////////////////////////////////////////////////////////

//: Decode a buffer of arbitrary length integers
// Converts from the integers from the arbitrary length format into
// an array of normal ints.
// \param buffer The buffer to be converted.
// \param count  Number of integers expected. Cannot be zero.
// \param ints   should point to a buffer at least as long as count.
// \return the number of bytes used, or zero on error.
inline unsigned long vsl_convert_from_arbitrary_length(const unsigned char* buffer,
                                                       vcl_size_t *ints,
                                                       unsigned long count = 1)
{
  return vsl_convert_from_arbitrary_length_unsigned_impl(buffer, ints, count);
}

/////////////////////////////////////////////////////////////////////////

//: Encode an array of ints into an arbitrary length format.
// The return value is the number of bytes used.
// buffer should be at least as long as
// VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(vcl_ptrdiff_t)) * count
inline unsigned long vsl_convert_to_arbitrary_length(const vcl_ptrdiff_t* ints,
                                                     unsigned char *buffer,
                                                     unsigned long count = 1)
{
  return vsl_convert_to_arbitrary_length_signed_impl(ints, buffer, count);
}


/////////////////////////////////////////////////////////////////////////

//: Decode a buffer of arbitrary length integers
// Converts from the integers from the arbitrary length format into
// an array of normal ints.
// \param buffer The buffer to be converted.
// \param count  Number of integers expected. Cannot be zero.
// \param ints   should point to a buffer at least as long as count.
// \return the number of bytes used, or zero on error.
inline unsigned long vsl_convert_from_arbitrary_length(const unsigned char* buffer,
                                                       vcl_ptrdiff_t *ints,
                                                       unsigned long count = 1)
{
  return vsl_convert_from_arbitrary_length_signed_impl(buffer, ints, count);
}
#endif // 0

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

//: Write an unsigned int as 16 bits to vsl_b_ostream
// If your signed int cannot be represented in 16 bits (e.g. on a 32 bit
// platform) the stream's error flag will be set.
//
// Warning: This function should be used infrequently and carefully. Under
// all normal circumstances, the generic vsl_b_read and vsl_b_write in
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
// all normal circumstances, the generic vsl_b_read and vsl_b_write in
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
// platform) the stream's error flag will be set.
//
// Warning: This function should be used infrequently and carefully. Under
// all normal circumstances, the generic vsl_b_read and vsl_b_write in
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
// all normal circumstances, the generic vsl_b_read and vsl_b_write in
// vsl_binary_io.h will be perfectly adequate.
//
// This function will only read values saved using vsl_b_write_int_16().
inline void vsl_b_read_int_16(vsl_b_istream& is, long& n )
{
  is.is().read( ( char* )&n, 2 );
  if ((*(((unsigned char*)&n)+1) & 128) == 1)
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
