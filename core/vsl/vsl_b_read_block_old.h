// This is core/vsl/vsl_b_read_block_old.h
#ifndef vsl_b_read_block_old_h_
#define vsl_b_read_block_old_h_
//:
// \file
// \brief Backwards compatibility support only.
// \author Ian Scott (Manchester) May 2003
//
// This file should only be used by existing binary io code that
// wishes to maintain backwards compatibility with existing VSL files.
// Users should have no reason to include this file.
//

#include <iostream>
#include <vxl_config.h>
#include <vcl_compiler_detection.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_binary_explicit_io.h>

// Whilst this file should not be used by users, it will likely never be deleted,
// and will remain in use by a number of files in vsl and vnl/io to provide
// backwards compatibility. If any of the functions are actually used, a
// deprecation warning will be sent to cerr.

#include <vcl_deprecated.h>

// The next declaration should be kept with its non-specialist definition.
// It was this mistake that lead to the full replacement of vsl_b_read_block
// and vsl_b_write_block with vsl_block_binary_{read,write}.

//: Read a block of values from a vsl_b_istream
// If you want to output a block of fundamental data types very efficiently,
// then just #include <vsl_binary_explicit_io.h>
// \deprecated in favour of vsl_block_binary_read
template <class T>
inline void vsl_b_read_block_old(vsl_b_istream &is, T* begin, std::size_t nelems)
{
  VXL_DEPRECATED_MACRO( "vsl_b_read_block_old()" );
  while (nelems--)
    vsl_b_read(is, *(begin++));
}

/////////////////////////////////////////////////////////////////////////

//: Read a block of doubles from a vsl_b_istream
// This function is very speed efficient.
// \deprecated in favour of vsl_block_binary_read
template <>
inline void vsl_b_read_block_old(vsl_b_istream &is, double* begin, std::size_t nelems)
{
  VXL_DEPRECATED_MACRO( "vsl_b_read_block_old()" );
  is.is().read((char*) begin, (unsigned long)(nelems*sizeof(double)));
  vsl_swap_bytes((char *)begin, sizeof(double), nelems);
}

/////////////////////////////////////////////////////////////////////////

//: Read a block of floats from a vsl_b_istream
// This function is very speed efficient.
// \deprecated in favour of vsl_block_binary_read
template <>
inline void vsl_b_read_block_old(vsl_b_istream &is, float* begin, std::size_t nelems)
{
  VXL_DEPRECATED_MACRO( "vsl_b_read_block_old()" );
  is.is().read((char*) begin, (unsigned long)(nelems*sizeof(float)));
  vsl_swap_bytes((char *)begin, sizeof(float), nelems);
}

/////////////////////////////////////////////////////////////////////////

//: Read a block of signed ints from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
// \deprecated in favour of vsl_block_binary_read
template <>
inline void vsl_b_read_block_old(vsl_b_istream &is, int* begin, std::size_t nelems)
{
  VXL_DEPRECATED_MACRO( "vsl_b_read_block_old()" );
  if (!is) return;
  std::size_t nbytes;
  vsl_b_read(is, nbytes);
  if (nbytes)
  {
    char *block = new char[VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(int)) * nelems];
    is.is().read(block, nbytes);
    std::size_t n_bytes_converted =
    vsl_convert_from_arbitrary_length((unsigned char *)block, begin, nelems);
    delete [] block;
    if (n_bytes_converted != nbytes)
    {
      std::cerr << "\nI/O ERROR: vsl_b_read_block(.., int*,..) :\n"
               << " Corrupted data stream\n";
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    }
  }
}


/////////////////////////////////////////////////////////////////////////

//: Read a block of unsigned ints from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
// \deprecated in favour of vsl_block_binary_read
template <>
inline void vsl_b_read_block_old(vsl_b_istream &is, unsigned int* begin, std::size_t nelems)
{
  VXL_DEPRECATED_MACRO( "vsl_b_read_block_old()" );
  std::size_t nbytes;
  vsl_b_read(is, nbytes);
  if (nbytes)
  {
    char *block = new char[VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(unsigned int)) * nelems];
    is.is().read(block, nbytes);
    std::size_t n_bytes_converted =
    vsl_convert_from_arbitrary_length((unsigned char *)block, begin, nelems);
    delete [] block;
    if (n_bytes_converted != nbytes)
    {
      std::cerr << "\nI/O ERROR: vsl_b_read_block(.., unsigned int*,..) :\n"
               << " Corrupted data stream\n";
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    }
  }
}


/////////////////////////////////////////////////////////////////////////


//: Read a block of signed shorts from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
// \deprecated in favour of vsl_block_binary_read
template <>
inline void vsl_b_read_block_old(vsl_b_istream &is, short* begin, std::size_t nelems)
{
  VXL_DEPRECATED_MACRO( "vsl_b_read_block_old()" );
  std::size_t nbytes;
  vsl_b_read(is, nbytes);
  if (nbytes)
  {
    char *block = new char[VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(short)) * nelems];
    is.is().read(block, nbytes);
    std::size_t n_bytes_converted =
    vsl_convert_from_arbitrary_length((unsigned char *)block, begin, nelems);
    delete [] block;
    if (n_bytes_converted != nbytes)
    {
      std::cerr << "\nI/O ERROR: vsl_b_read_block(.., short*,..) :\n"
               << " Corrupted data stream\n";
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    }
  }
}


/////////////////////////////////////////////////////////////////////////


//: Read a block of unsigned shorts from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
// \deprecated in favour of vsl_block_binary_read
template <>
inline void vsl_b_read_block_old(vsl_b_istream &is, unsigned short* begin, std::size_t nelems)
{
  VXL_DEPRECATED_MACRO( "vsl_b_read_block_old()" );
  std::size_t nbytes;
  vsl_b_read(is, nbytes);
  if (nbytes)
  {
    char *block = new char[VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(unsigned short)) * nelems];
    is.is().read(block, nbytes);
    std::size_t n_bytes_converted =
    vsl_convert_from_arbitrary_length((unsigned char *)block, begin, nelems);
    delete [] block;
    if (n_bytes_converted != nbytes)
    {
      std::cerr << "\nI/O ERROR: vsl_b_read_block(.., unsigned short*,..) :\n"
               << " Corrupted data stream\n";
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    }
  }
}


/////////////////////////////////////////////////////////////////////////


//: Read a block of signed longs from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
// \deprecated in favour of vsl_block_binary_read
template <>
inline void vsl_b_read_block_old(vsl_b_istream &is, long* begin, std::size_t nelems)
{
  VXL_DEPRECATED_MACRO( "vsl_b_read_block_old()" );
  std::size_t nbytes;
  vsl_b_read(is, nbytes);
  if (nbytes)
  {
    char *block = new char[VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(long)) * nelems];
    is.is().read(block, nbytes);
    std::size_t n_bytes_converted =
    vsl_convert_from_arbitrary_length((unsigned char *)block, begin, nelems);
    delete [] block;
    if (n_bytes_converted != nbytes)
    {
      std::cerr << "\nI/O ERROR: vsl_b_read_block(.., long*,..) :\n"
               << " Corrupted data stream\n";
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    }
  }
}


/////////////////////////////////////////////////////////////////////////


//: Read a block of unsigned longs from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
// \deprecated in favour of vsl_block_binary_read
template <>
inline void vsl_b_read_block_old(vsl_b_istream &is, unsigned long* begin, std::size_t nelems)
{
  VXL_DEPRECATED_MACRO( "vsl_b_read_block_old()" );
  std::size_t nbytes;
  vsl_b_read(is, nbytes);
  if (nbytes)
  {
    char *block = new char[VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(unsigned long)) * nelems];
    is.is().read(block, nbytes);
    std::size_t n_bytes_converted =
    vsl_convert_from_arbitrary_length((unsigned char *)block, begin, nelems);
    delete [] block;
    if (n_bytes_converted != nbytes)
    {
      std::cerr << "\nI/O ERROR: vsl_b_read_block(.., unsigned long*,..) :\n"
               << " Corrupted data stream\n";
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    }
  }
}

#endif // vsl_b_read_block_old_h_
