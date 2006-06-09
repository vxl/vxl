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
inline void vsl_block_binary_write(vsl_b_ostream &os, const T* begin, vcl_size_t nelems);

//: Read a block of values from a vsl_b_ostream, potentially very efficiently for fundamental types.
template <class T>
inline void vsl_block_binary_read(vsl_b_istream &is, T* begin, vcl_size_t nelems);


/////////////////////////////////////////////////////////////////////////

// Internal implementation
template <class T>
  void vsl_block_binary_write_float_impl(vsl_b_ostream &os, const T* begin, vcl_size_t nelems);

// Internal implementation
template <class T>
  void vsl_block_binary_read_float_impl(vsl_b_istream &is, T* begin, vcl_size_t nelems);


/////////////////////////////////////////////////////////////////////////
//: Write a block of doubles to a vsl_b_ostream
// This function is very speed efficient, but
// temporarily allocates a block of memory the size of the
// block being read.
VCL_DEFINE_SPECIALIZATION
inline void vsl_block_binary_write(vsl_b_ostream &os, const double* begin, vcl_size_t nelems)
{
  vsl_block_binary_write_float_impl(os, begin, nelems);
}

//: Read a block of doubles from a vsl_b_istream
// This function is very speed efficient.
VCL_DEFINE_SPECIALIZATION
inline void vsl_block_binary_read(vsl_b_istream &is, double* begin, vcl_size_t nelems)
{
  vsl_block_binary_read_float_impl(is, begin, nelems);
}

/////////////////////////////////////////////////////////////////////////

//: Write a block of floats to a vsl_b_ostream
// This function is very speed efficient, but
// temporarily allocates a block of memory the size of the
// block being read.
VCL_DEFINE_SPECIALIZATION
inline void vsl_block_binary_write(vsl_b_ostream &os, const float* begin, vcl_size_t nelems)
{
  vsl_block_binary_write_float_impl(os, begin, nelems);
}

//: Read a block of floats from a vsl_b_istream
// This function is very speed efficient.
VCL_DEFINE_SPECIALIZATION
inline void vsl_block_binary_read(vsl_b_istream &is, float* begin, vcl_size_t nelems)
{
  vsl_block_binary_read_float_impl(is, begin, nelems);
}

/////////////////////////////////////////////////////////////////////////

// Internal implementation
template <class T>
  void vsl_block_binary_write_int_impl(vsl_b_ostream &os, const T* begin, vcl_size_t nelems);

// Internal implementation
template <class T>
  void vsl_block_binary_read_int_impl(vsl_b_istream &is, T* begin, vcl_size_t nelems);
  
/////////////////////////////////////////////////////////////////////////

//: Write a block of signed ints to a vsl_b_ostream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
VCL_DEFINE_SPECIALIZATION
inline void vsl_block_binary_write(vsl_b_ostream &os, const int* begin, vcl_size_t nelems)
{
  vsl_block_binary_write_int_impl(os, begin, nelems);
}

//: Read a block of signed ints from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
VCL_DEFINE_SPECIALIZATION
inline void vsl_block_binary_read(vsl_b_istream &is, int* begin, vcl_size_t nelems)
{
  vsl_block_binary_read_int_impl(is, begin, nelems);
}

/////////////////////////////////////////////////////////////////////////

//: Write a block of unsigned ints to a vsl_b_ostream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
VCL_DEFINE_SPECIALIZATION
inline void vsl_block_binary_write(vsl_b_ostream &os, const unsigned int* begin, vcl_size_t nelems)
{
  vsl_block_binary_write_int_impl(os, begin, nelems);
}

//: Read a block of unsigned ints from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
VCL_DEFINE_SPECIALIZATION
inline void vsl_block_binary_read(vsl_b_istream &is, unsigned int* begin, vcl_size_t nelems)
{
  vsl_block_binary_read_int_impl(is, begin, nelems);
}


/////////////////////////////////////////////////////////////////////////

//: Write a block of signed shorts to a vsl_b_ostream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
VCL_DEFINE_SPECIALIZATION
inline void vsl_block_binary_write(vsl_b_ostream &os, const short* begin, vcl_size_t nelems)
{
  vsl_block_binary_write_int_impl(os, begin, nelems);
}

//: Read a block of signed shorts from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
VCL_DEFINE_SPECIALIZATION
inline void vsl_block_binary_read(vsl_b_istream &is, short* begin, vcl_size_t nelems)
{
  vsl_block_binary_read_int_impl(is, begin, nelems);
}


/////////////////////////////////////////////////////////////////////////

//: Write a block of unsigned shorts to a vsl_b_ostream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
VCL_DEFINE_SPECIALIZATION
inline void vsl_block_binary_write(vsl_b_ostream &os, const unsigned short* begin, vcl_size_t nelems)
{
  vsl_block_binary_write_int_impl(os, begin, nelems);
}

//: Read a block of unsigned shorts from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
VCL_DEFINE_SPECIALIZATION
inline void vsl_block_binary_read(vsl_b_istream &is, unsigned short* begin, vcl_size_t nelems)
{
  vsl_block_binary_read_int_impl(is, begin, nelems);
}


/////////////////////////////////////////////////////////////////////////

//: Write a block of signed longs to a vsl_b_ostream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
VCL_DEFINE_SPECIALIZATION
inline void vsl_block_binary_write(vsl_b_ostream &os, const long* begin, vcl_size_t nelems)
{
  vsl_block_binary_write_int_impl(os, begin, nelems);
}

//: Read a block of signed longs from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
VCL_DEFINE_SPECIALIZATION
inline void vsl_block_binary_read(vsl_b_istream &is, long* begin, vcl_size_t nelems)
{
  vsl_block_binary_read_int_impl(is, begin, nelems);
}


/////////////////////////////////////////////////////////////////////////

//: Write a block of unsigned longs to a vsl_b_ostream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
VCL_DEFINE_SPECIALIZATION
inline void vsl_block_binary_write(vsl_b_ostream &os, const unsigned long* begin, vcl_size_t nelems)
{
  vsl_block_binary_write_int_impl(os, begin, nelems);
}

//: Read a block of unsigned longs from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
VCL_DEFINE_SPECIALIZATION
inline void vsl_block_binary_read(vsl_b_istream &is, unsigned long* begin, vcl_size_t nelems)
{
  vsl_block_binary_read_int_impl(is, begin, nelems);
}


/////////////////////////////////////////////////////////////////////////

#if 0
// This test will be replaced with !VCL_PTRDIFF_T_IS_A_STANDARD_TYPE
// When that macro is working.

//: Write a block of vcl_ptrdiff_ts to a vsl_b_ostream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
VCL_DEFINE_SPECIALIZATION
inline void vsl_block_binary_write(vsl_b_ostream &os, const vcl_ptrdiff_t* begin, vcl_size_t nelems)
{
  vsl_block_binary_write_int_impl(os, begin, nelems);
}

//: Read a block of vcl_ptrdiff_ts from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
VCL_DEFINE_SPECIALIZATION
inline void vsl_block_binary_read(vsl_b_istream &is, vcl_ptrdiff_t* begin, vcl_size_t nelems)
{
  vsl_block_binary_read_int_impl(is, begin, nelems);
}


/////////////////////////////////////////////////////////////////////////

//: Write a block of vcl_size_ts to a vsl_b_ostream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
VCL_DEFINE_SPECIALIZATION
inline void vsl_block_binary_write(vsl_b_ostream &os, const vcl_size_t* begin, vcl_size_t nelems)
{
  vsl_block_binary_write_int_impl(os, begin, nelems);
}

//: Read a block of vcl_size_ts from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
VCL_DEFINE_SPECIALIZATION
inline void vsl_block_binary_read(vsl_b_istream &is, vcl_size_t* begin, vcl_size_t nelems)
{
  vsl_block_binary_read_int_impl(is, begin, nelems);
}

#endif // 0


//: Write a block of values to a vsl_b_ostream
// If you want to output a block of fundamental data types very efficiently,
// then just #include <vsl_binary_explicit_io.h>
template <class T>
inline void vsl_block_binary_write(vsl_b_ostream &os, const T* begin, vcl_size_t nelems)
{
  vsl_b_write(os, false); // Error check that this is a specialised version
  while (nelems--)
    vsl_b_write(os, *(begin++));
}

//: Read a block of values from a vsl_b_istream
// If you want to output a block of fundamental data types very efficiently,
// then just #include <vsl_binary_explicit_io.h>
template <class T>
inline void vsl_block_binary_read(vsl_b_istream &is, T* begin, vcl_size_t nelems)
{
  vsl_block_binary_read_confirm_specialisation(is, false);
  if (!is) return;
  while (nelems--)
    vsl_b_read(is, *(begin++));
}

#endif // vsl_block_binary_io_h_
