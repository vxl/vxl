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

#include <vxl_config.h>
#include <vcl_cassert.h>
#include <vcl_cstring.h>
#include <vcl_iostream.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_binary_explicit_io.h>

// The next two declarations should be kept with their non-specialist definitions.
// It was this mistake that lead to their full replacement with vsl_block_binary*


//: Read a block of values from a vsl_b_istream
// If you want to output a block of fundamental data types very efficiently,
// then just #include <vsl_binary_explicit_io.h>
// \deprecated in favour of vsl_block_binary_read
template <class T>
#ifdef VCL_VC60
static
#endif
inline void vsl_b_read_block_old(vsl_b_istream &is, T* begin, unsigned nelems)
{
  while (nelems--)
    vsl_b_read(is, *(begin++));
}


/////////////////////////////////////////////////////////////////////////

//: Read a block of doubles from a vsl_b_istream
// This function is very speed efficient.
// \deprecated in favour of vsl_block_binary_read
VCL_DEFINE_SPECIALIZATION
#ifdef VCL_VC60
static
#endif
inline void vsl_b_read_block_old(vsl_b_istream &is, double* begin, unsigned nelems)
{
  is.is().read((char*) begin, (unsigned long)(nelems*sizeof(double)));
  vsl_swap_bytes((char *)begin, sizeof(double), nelems);
}

/////////////////////////////////////////////////////////////////////////

//: Read a block of floats from a vsl_b_istream
// This function is very speed efficient.
// \deprecated in favour of vsl_block_binary_read
VCL_DEFINE_SPECIALIZATION
#ifdef VCL_VC60
static
#endif
inline void vsl_b_read_block_old(vsl_b_istream &is, float* begin, unsigned nelems)
{
  is.is().read((char*) begin, (unsigned long)(nelems*sizeof(float)));
  vsl_swap_bytes((char *)begin, sizeof(float), nelems);
}

/////////////////////////////////////////////////////////////////////////

//: Read a block of signed ints from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
// \deprecated in favour of vsl_block_binary_read
VCL_DEFINE_SPECIALIZATION
#ifdef VCL_VC60
static
#endif
inline void vsl_b_read_block_old(vsl_b_istream &is, int* begin, unsigned nelems)
{
  if (!is) return;
  unsigned long nbytes;
  vsl_b_read(is, nbytes);
  if (nbytes)
  {
    char *block = new char[VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(int)) * nelems];
    is.is().read(block, nbytes);
    unsigned long n_bytes_converted =
    vsl_convert_from_arbitrary_length((unsigned char *)block, begin, nelems);
    delete [] block;
    if (n_bytes_converted != nbytes)
    {
      vcl_cerr << "\nI/O ERROR: vsl_b_read_block(.., int*,..)"
               << " Corrupted data stream\n";
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    }
  }
}


/////////////////////////////////////////////////////////////////////////

//: Read a block of unsigned ints from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
// \deprecated in favour of vsl_block_binary_read
VCL_DEFINE_SPECIALIZATION
#ifdef VCL_VC60
static
#endif
inline void vsl_b_read_block_old(vsl_b_istream &is, unsigned int* begin, unsigned nelems)
{
  unsigned long nbytes;
  vsl_b_read(is, nbytes);
  if (nbytes)
  {
    char *block = new char[VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(unsigned int)) * nelems];
    is.is().read(block, nbytes);
    unsigned long n_bytes_converted =
    vsl_convert_from_arbitrary_length((unsigned char *)block, begin, nelems);
    delete [] block;
    if (n_bytes_converted != nbytes)
    {
      vcl_cerr << "\nI/O ERROR: vsl_b_read_block(.., unsigned int*,..)"
               << " Corrupted data stream\n";
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    }
  }
}


/////////////////////////////////////////////////////////////////////////


//: Read a block of signed shorts from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
// \deprecated in favour of vsl_block_binary_read
VCL_DEFINE_SPECIALIZATION
#ifdef VCL_VC60
static
#endif
inline void vsl_b_read_block_old(vsl_b_istream &is, short* begin, unsigned nelems)
{
  unsigned long nbytes;
  vsl_b_read(is, nbytes);
  if (nbytes)
  {
    char *block = new char[VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(short)) * nelems];
    is.is().read(block, nbytes);
    unsigned long n_bytes_converted =
    vsl_convert_from_arbitrary_length((unsigned char *)block, begin, nelems);
    delete [] block;
    if (n_bytes_converted != nbytes)
    {
      vcl_cerr << "\nI/O ERROR: vsl_b_read_block(.., short*,..)"
               << " Corrupted data stream\n";
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    }
  }
}


/////////////////////////////////////////////////////////////////////////


//: Read a block of unsigned shorts from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
// \deprecated in favour of vsl_block_binary_read
VCL_DEFINE_SPECIALIZATION
#ifdef VCL_VC60
static
#endif
inline void vsl_b_read_block_old(vsl_b_istream &is, unsigned short* begin, unsigned nelems)
{
  unsigned long nbytes;
  vsl_b_read(is, nbytes);
  if (nbytes)
  {
    char *block = new char[VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(unsigned short)) * nelems];
    is.is().read(block, nbytes);
    unsigned long n_bytes_converted =
    vsl_convert_from_arbitrary_length((unsigned char *)block, begin, nelems);
    delete [] block;
    if (n_bytes_converted != nbytes)
    {
      vcl_cerr << "\nI/O ERROR: vsl_b_read_block(.., unsigned short*,..)"
               << " Corrupted data stream\n";
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    }
  }
}


/////////////////////////////////////////////////////////////////////////



//: Read a block of signed longs from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
// \deprecated in favour of vsl_block_binary_read
VCL_DEFINE_SPECIALIZATION
#ifdef VCL_VC60
static
#endif
inline void vsl_b_read_block_old(vsl_b_istream &is, long* begin, unsigned nelems)
{
  unsigned long nbytes;
  vsl_b_read(is, nbytes);
  if (nbytes)
  {
    char *block = new char[VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(long)) * nelems];
    is.is().read(block, nbytes);
    unsigned long n_bytes_converted =
    vsl_convert_from_arbitrary_length((unsigned char *)block, begin, nelems);
    delete [] block;
    if (n_bytes_converted != nbytes)
    {
      vcl_cerr << "\nI/O ERROR: vsl_b_read_block(.., long*,..)"
               << " Corrupted data stream\n";
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    }
  }
}


/////////////////////////////////////////////////////////////////////////



//: Read a block of unsigned longs from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
// \deprecated in favour of vsl_block_binary_read
VCL_DEFINE_SPECIALIZATION
#ifdef VCL_VC60
static
#endif
inline void vsl_b_read_block_old(vsl_b_istream &is, unsigned long* begin, unsigned nelems)
{
  unsigned long nbytes;
  vsl_b_read(is, nbytes);
  if (nbytes)
  {
    char *block = new char[VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(unsigned long)) * nelems];
    is.is().read(block, nbytes);
    unsigned long n_bytes_converted =
    vsl_convert_from_arbitrary_length((unsigned char *)block, begin, nelems);
    delete [] block;
    if (n_bytes_converted != nbytes)
    {
      vcl_cerr << "\nI/O ERROR: vsl_b_read_block(.., unsigned long*,..)"
               << " Corrupted data stream\n";
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    }
  }
}




#endif // vsl_b_read_block_old_h_
