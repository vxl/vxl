// This is core/vsl/vsl_block_binary.h
#ifndef vsl_block_binary_io_h_
#define vsl_block_binary_io_h_
//:
// \file
// \brief Set of functions to do binary IO on a block of values.
// \author Ian Scott, ISBE Manchester, Feb 2003

#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_binary_explicit_io.h>

//: Error checking.
void vsl_block_binary_read_confirm_specialisation(vsl_b_istream &is, bool specialised);

//: Write a block of values to a vsl_b_ostream, potentially very efficiently for fundamental types.
template <class T>
inline void vsl_block_binary_write(vsl_b_ostream &os, const T* begin, std::size_t nelems);

//: Read a block of values from a vsl_b_ostream, potentially very efficiently for fundamental types.
template <class T>
inline void vsl_block_binary_read(vsl_b_istream &is, T* begin, std::size_t nelems);


/////////////////////////////////////////////////////////////////////////

// Internal implementation
template <class T>
  void vsl_block_binary_write_float_impl(vsl_b_ostream &os, const T* begin, std::size_t nelems);

// Internal implementation
template <class T>
  void vsl_block_binary_read_float_impl(vsl_b_istream &is, T* begin, std::size_t nelems);


/////////////////////////////////////////////////////////////////////////
//: Write a block of doubles to a vsl_b_ostream
// This function is very speed efficient, but
// temporarily allocates a block of memory the size of the
// block being read.
template <>
inline void vsl_block_binary_write(vsl_b_ostream &os, const double* begin, std::size_t nelems)
{
  vsl_block_binary_write_float_impl(os, begin, nelems);
}

//: Read a block of doubles from a vsl_b_istream
// This function is very speed efficient.
template <>
inline void vsl_block_binary_read(vsl_b_istream &is, double* begin, std::size_t nelems)
{
  vsl_block_binary_read_float_impl(is, begin, nelems);
}

/////////////////////////////////////////////////////////////////////////

//: Write a block of floats to a vsl_b_ostream
// This function is very speed efficient, but
// temporarily allocates a block of memory the size of the
// block being read.
template <>
inline void vsl_block_binary_write(vsl_b_ostream &os, const float* begin, std::size_t nelems)
{
  vsl_block_binary_write_float_impl(os, begin, nelems);
}

//: Read a block of floats from a vsl_b_istream
// This function is very speed efficient.
template <>
inline void vsl_block_binary_read(vsl_b_istream &is, float* begin, std::size_t nelems)
{
  vsl_block_binary_read_float_impl(is, begin, nelems);
}

/////////////////////////////////////////////////////////////////////////

// Internal implementation
template <class T>
  void vsl_block_binary_write_int_impl(vsl_b_ostream &os, const T* begin, std::size_t nelems);

// Internal implementation
template <class T>
  void vsl_block_binary_read_int_impl(vsl_b_istream &is, T* begin, std::size_t nelems);

/////////////////////////////////////////////////////////////////////////

//: Write a block of signed ints to a vsl_b_ostream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
template <>
inline void vsl_block_binary_write(vsl_b_ostream &os, const int* begin, std::size_t nelems)
{
  vsl_block_binary_write_int_impl(os, begin, nelems);
}

//: Read a block of signed ints from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
template <>
inline void vsl_block_binary_read(vsl_b_istream &is, int* begin, std::size_t nelems)
{
  vsl_block_binary_read_int_impl(is, begin, nelems);
}

/////////////////////////////////////////////////////////////////////////

//: Write a block of unsigned ints to a vsl_b_ostream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
template <>
inline void vsl_block_binary_write(vsl_b_ostream &os, const unsigned int* begin, std::size_t nelems)
{
  vsl_block_binary_write_int_impl(os, begin, nelems);
}

//: Read a block of unsigned ints from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
template <>
inline void vsl_block_binary_read(vsl_b_istream &is, unsigned int* begin, std::size_t nelems)
{
  vsl_block_binary_read_int_impl(is, begin, nelems);
}


/////////////////////////////////////////////////////////////////////////

//: Write a block of signed shorts to a vsl_b_ostream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
template <>
inline void vsl_block_binary_write(vsl_b_ostream &os, const short* begin, std::size_t nelems)
{
  vsl_block_binary_write_int_impl(os, begin, nelems);
}

//: Read a block of signed shorts from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
template <>
inline void vsl_block_binary_read(vsl_b_istream &is, short* begin, std::size_t nelems)
{
  vsl_block_binary_read_int_impl(is, begin, nelems);
}


/////////////////////////////////////////////////////////////////////////

//: Write a block of unsigned shorts to a vsl_b_ostream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
template <>
inline void vsl_block_binary_write(vsl_b_ostream &os, const unsigned short* begin, std::size_t nelems)
{
  vsl_block_binary_write_int_impl(os, begin, nelems);
}

//: Read a block of unsigned shorts from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
template <>
inline void vsl_block_binary_read(vsl_b_istream &is, unsigned short* begin, std::size_t nelems)
{
  vsl_block_binary_read_int_impl(is, begin, nelems);
}


/////////////////////////////////////////////////////////////////////////

//: Write a block of signed longs to a vsl_b_ostream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
template <>
inline void vsl_block_binary_write(vsl_b_ostream &os, const long* begin, std::size_t nelems)
{
  vsl_block_binary_write_int_impl(os, begin, nelems);
}

//: Read a block of signed longs from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
template <>
inline void vsl_block_binary_read(vsl_b_istream &is, long* begin, std::size_t nelems)
{
  vsl_block_binary_read_int_impl(is, begin, nelems);
}


/////////////////////////////////////////////////////////////////////////

//: Write a block of unsigned longs to a vsl_b_ostream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
template <>
inline void vsl_block_binary_write(vsl_b_ostream &os, const unsigned long* begin, std::size_t nelems)
{
  vsl_block_binary_write_int_impl(os, begin, nelems);
}

//: Read a block of unsigned longs from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
template <>
inline void vsl_block_binary_read(vsl_b_istream &is, unsigned long* begin, std::size_t nelems)
{
  vsl_block_binary_read_int_impl(is, begin, nelems);
}

/////////////////////////////////////////////////////////////////////////
#if VXL_INT_64_IS_LONGLONG

//: Write a block of signed longs to a vsl_b_ostream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
template <>
inline void vsl_block_binary_write(vsl_b_ostream &os, const vxl_int_64* begin, std::size_t nelems)
{
  vsl_block_binary_write_int_impl(os, begin, nelems);
}

//: Read a block of signed longs from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
template <>
inline void vsl_block_binary_read(vsl_b_istream &is, vxl_int_64* begin, std::size_t nelems)
{
  vsl_block_binary_read_int_impl(is, begin, nelems);
}


/////////////////////////////////////////////////////////////////////////

//: Write a block of unsigned longs to a vsl_b_ostream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
template <>
inline void vsl_block_binary_write(vsl_b_ostream &os, const vxl_uint_64* begin, std::size_t nelems)
{
  vsl_block_binary_write_int_impl(os, begin, nelems);
}

//: Read a block of unsigned longs from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
template <>
inline void vsl_block_binary_read(vsl_b_istream &is, vxl_uint_64* begin, std::size_t nelems)
{
  vsl_block_binary_read_int_impl(is, begin, nelems);
}
#endif //VXL_HAS_INT_64 && !VXL_INT_64_IS_LONG

/////////////////////////////////////////////////////////////////////////

// Internal implementation
template <class T>
  void vsl_block_binary_write_byte_impl(vsl_b_ostream &os, const T* begin, std::size_t nelems);

// Internal implementation
template <class T>
  void vsl_block_binary_read_byte_impl(vsl_b_istream &is, T* begin, std::size_t nelems);

  /////////////////////////////////////////////////////////////////////////

//: Write a block of unsigned chars to a vsl_b_ostream
// This function is very speed and space efficient.
template <>
inline void vsl_block_binary_write(vsl_b_ostream &os, const unsigned char* begin, std::size_t nelems)
{
  vsl_block_binary_write_byte_impl(os, begin, nelems);
}

//: Read a block of unsigned chars from a vsl_b_istream
// This function is very speed and space efficient.
template <>
inline void vsl_block_binary_read(vsl_b_istream &is, unsigned char* begin, std::size_t nelems)
{
  vsl_block_binary_read_byte_impl(is, begin, nelems);
}

/////////////////////////////////////////////////////////////////////////

//: Write a block of signed chars to a vsl_b_ostream
// This function is very speed and space efficient.
template <>
inline void vsl_block_binary_write(vsl_b_ostream &os, const signed char* begin, std::size_t nelems)
{
  vsl_block_binary_write_byte_impl(os, begin, nelems);
}

//: Read a block of signed chars from a vsl_b_istream
// This function is very speed and space efficient.
template <>
inline void vsl_block_binary_read(vsl_b_istream &is, signed char* begin, std::size_t nelems)
{
  vsl_block_binary_read_byte_impl(is, begin, nelems);
}

/////////////////////////////////////////////////////////////////////////


//: Write a block of values to a vsl_b_ostream
// If you want to output a block of fundamental data types very efficiently,
// then just #include <vsl_binary_explicit_io.h>
template <class T>
inline void vsl_block_binary_write(vsl_b_ostream &os, const T* begin, std::size_t nelems)
{
  vsl_b_write(os, false); // Error check that this is a specialised version
  while (nelems--)
    vsl_b_write(os, *(begin++));
}

//: Read a block of values from a vsl_b_istream
// If you want to output a block of fundamental data types very efficiently,
// then just #include <vsl_binary_explicit_io.h>
template <class T>
inline void vsl_block_binary_read(vsl_b_istream &is, T* begin, std::size_t nelems)
{
  vsl_block_binary_read_confirm_specialisation(is, false);
  if (!is) return;
  while (nelems--)
    vsl_b_read(is, *(begin++));
}

#endif // vsl_block_binary_io_h_
