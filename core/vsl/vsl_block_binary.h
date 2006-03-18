// This is core/vsl/vsl_block_binary.h
#ifndef vsl_block_binary_io_h_
#define vsl_block_binary_io_h_
//:
// \file
// \brief Set of functions to do binary IO on a block of values.
// \author Ian Scott, ISBE Manchester, Feb 2003

#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_binary_explicit_io.h>

// IMS Hack: MSVC6.0 has the annoying habit of occasionally forgetting that inline implies static.
#ifdef VCL_VC_6
#define VCL_VC_6_STATIC static
#else
#define VCL_VC_6_STATIC /**/
#endif


inline void vsl_block_binary_read_confirm_specialisation(vsl_b_istream &is,
                                                         bool specialised)
{
  if (!is) return;
  bool b;
  vsl_b_read(is, b);
  if (b != specialised)
  {
    vcl_cerr << "I/O ERROR: vsl_block_binary_read()\n";
    if (specialised)
      vcl_cerr << "           Data was saved using unspecialised slow form and is being loaded\n"
               << "           using specialised fast form.\n\n";
    else
      vcl_cerr << "           Data was saved using specialised fast form and is being loaded\n"
               << "           using unspecialised slow form.\n\n";

    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
  }
}


//: Write a block of values to a vsl_b_ostream, potentially very efficiently for fundamental types.
template <class T>
inline void vsl_block_binary_write(vsl_b_ostream &os, const T* begin, unsigned nelems);

//: Read a block of values from a vsl_b_ostream, potentially very efficiently for fundamental types.
template <class T>
inline void vsl_block_binary_read(vsl_b_istream &is, T* begin, unsigned nelems);


/////////////////////////////////////////////////////////////////////////
//: Write a block of doubles to a vsl_b_ostream
// This function is very speed efficient, but
// temporarily allocates a block of memory the size of the
// block being read.
VCL_DEFINE_SPECIALIZATION
VCL_VC_6_STATIC inline void vsl_block_binary_write(vsl_b_ostream &os, const double* begin, unsigned nelems)
{
  vsl_b_write(os, true); // Error check that this is a specialised version
  double *block = new double[nelems];

  vsl_swap_bytes_to_buffer((const char *)begin, (char *)block, sizeof(double), nelems);

  os.os().write((const char*) block, (unsigned long)(nelems*sizeof(double)));
  delete [] block;
}

//: Read a block of doubles from a vsl_b_istream
// This function is very speed efficient.
VCL_DEFINE_SPECIALIZATION
VCL_VC_6_STATIC inline void vsl_block_binary_read(vsl_b_istream &is, double* begin, unsigned nelems)
{
  vsl_block_binary_read_confirm_specialisation(is, true);
  if (!is) return;
  is.is().read((char*) begin, (unsigned long)(nelems*sizeof(double)));
  vsl_swap_bytes((char *)begin, sizeof(double), nelems);
}

/////////////////////////////////////////////////////////////////////////

//: Write a block of floats to a vsl_b_ostream
// This function is very speed efficient, but
// temporarily allocates a block of memory the size of the
// block being read.
VCL_DEFINE_SPECIALIZATION
VCL_VC_6_STATIC inline void vsl_block_binary_write(vsl_b_ostream &os, const float* begin, unsigned nelems)
{
  vsl_b_write(os, true); // Error check that this is a specialised version
  float *block = new float[nelems];
  vsl_swap_bytes_to_buffer((const char *)begin, (char *)block, sizeof(float), nelems);

  os.os().write((const char*) block, (unsigned long)(nelems*sizeof(float)));
  delete [] block;
}

//: Read a block of floats from a vsl_b_istream
// This function is very speed efficient.
VCL_DEFINE_SPECIALIZATION
VCL_VC_6_STATIC inline void vsl_block_binary_read(vsl_b_istream &is, float* begin, unsigned nelems)
{
  vsl_block_binary_read_confirm_specialisation(is, true);
  if (!is) return;
  is.is().read((char*) begin, (unsigned long)(nelems*sizeof(float)));
  vsl_swap_bytes((char *)begin, sizeof(float), nelems);
}

/////////////////////////////////////////////////////////////////////////

//: Write a block of signed ints to a vsl_b_ostream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
VCL_DEFINE_SPECIALIZATION
VCL_VC_6_STATIC inline void vsl_block_binary_write(vsl_b_ostream &os, const int* begin, unsigned nelems)
{
  vsl_b_write(os, true); // Error check that this is a specialised version
  char *block = new char[VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(int)) * nelems];
  unsigned long nbytes = vsl_convert_to_arbitrary_length(begin, (unsigned char *)block, nelems);

  vsl_b_write(os, nbytes);

  os.os().write( block, nbytes);
  delete [] block;
}

//: Read a block of signed ints from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
VCL_DEFINE_SPECIALIZATION
VCL_VC_6_STATIC inline void vsl_block_binary_read(vsl_b_istream &is, int* begin, unsigned nelems)
{
  vsl_block_binary_read_confirm_specialisation(is, true);
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
      vcl_cerr << "\nI/O ERROR: vsl_block_binary_read(.., int*,..)"
               << " Corrupted data stream\n";
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    }
  }
}


/////////////////////////////////////////////////////////////////////////

//: Write a block of unsigned ints to a vsl_b_ostream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
VCL_DEFINE_SPECIALIZATION
VCL_VC_6_STATIC inline void vsl_block_binary_write(vsl_b_ostream &os, const unsigned int* begin, unsigned nelems)
{
  vsl_b_write(os, true); // Error check that this is a specialised version
  char *block = new char[VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(unsigned int)) * nelems];

  unsigned long nbytes = vsl_convert_to_arbitrary_length(begin, (unsigned char *)block, nelems);
  vsl_b_write(os, nbytes);

  os.os().write( block, nbytes);
  delete [] block;
}

//: Read a block of unsigned ints from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
VCL_DEFINE_SPECIALIZATION
VCL_VC_6_STATIC inline void vsl_block_binary_read(vsl_b_istream &is, unsigned int* begin, unsigned nelems)
{
  vsl_block_binary_read_confirm_specialisation(is, true);
  if (!is) return;
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
      vcl_cerr << "\nI/O ERROR: vsl_block_binary_read(.., unsigned int*,..)"
               << " Corrupted data stream\n";
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    }
  }
}


/////////////////////////////////////////////////////////////////////////

//: Write a block of signed shorts to a vsl_b_ostream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
VCL_DEFINE_SPECIALIZATION
VCL_VC_6_STATIC inline void vsl_block_binary_write(vsl_b_ostream &os, const short* begin, unsigned nelems)
{
  vsl_b_write(os, true); // Error check that this is a specialised version
  char *block = new char[VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(short)) * nelems];

  unsigned long nbytes = vsl_convert_to_arbitrary_length(begin, (unsigned char *)block, nelems);
  vsl_b_write(os, nbytes);

  os.os().write( block, nbytes);
  delete [] block;
}

//: Read a block of signed shorts from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
VCL_DEFINE_SPECIALIZATION
VCL_VC_6_STATIC inline void vsl_block_binary_read(vsl_b_istream &is, short* begin, unsigned nelems)
{
  vsl_block_binary_read_confirm_specialisation(is, true);
  if (!is) return;
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
      vcl_cerr << "\nI/O ERROR: vsl_block_binary_read(.., short*,..)"
               << " Corrupted data stream\n";
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    }
  }
}


/////////////////////////////////////////////////////////////////////////

//: Write a block of unsigned shorts to a vsl_b_ostream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
VCL_DEFINE_SPECIALIZATION
VCL_VC_6_STATIC inline void vsl_block_binary_write(vsl_b_ostream &os, const unsigned short* begin, unsigned nelems)
{
  vsl_b_write(os, true); // Error check that this is a specialised version
  char *block = new char[VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(unsigned short)) * nelems];

  unsigned long nbytes = vsl_convert_to_arbitrary_length(begin, (unsigned char *)block, nelems);
  vsl_b_write(os, nbytes);

  os.os().write( block, nbytes);
  delete [] block;
}

//: Read a block of unsigned shorts from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
VCL_DEFINE_SPECIALIZATION
VCL_VC_6_STATIC inline void vsl_block_binary_read(vsl_b_istream &is, unsigned short* begin, unsigned nelems)
{
  vsl_block_binary_read_confirm_specialisation(is, true);
  if (!is) return;
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
      vcl_cerr << "\nI/O ERROR: vsl_block_binary_read(.., unsigned short*,..)"
               << " Corrupted data stream\n";
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    }
  }
}


/////////////////////////////////////////////////////////////////////////

//: Write a block of signed longs to a vsl_b_ostream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
VCL_DEFINE_SPECIALIZATION
VCL_VC_6_STATIC inline void vsl_block_binary_write(vsl_b_ostream &os, const long* begin, unsigned nelems)
{
  vsl_b_write(os, true); // Error check that this is a specialised version
  char *block = new char[VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(long)) * nelems];

  unsigned long nbytes = vsl_convert_to_arbitrary_length(begin, (unsigned char *)block, nelems);
  vsl_b_write(os, nbytes);

  os.os().write( block, nbytes);
  delete [] block;
}

//: Read a block of signed longs from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
VCL_DEFINE_SPECIALIZATION
VCL_VC_6_STATIC inline void vsl_block_binary_read(vsl_b_istream &is, long* begin, unsigned nelems)
{
  vsl_block_binary_read_confirm_specialisation(is, true);
  if (!is) return;
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
      vcl_cerr << "\nI/O ERROR: vsl_block_binary_read(.., long*,..)"
               << " Corrupted data stream\n";
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    }
  }
}


/////////////////////////////////////////////////////////////////////////

//: Write a block of unsigned longs to a vsl_b_ostream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
VCL_DEFINE_SPECIALIZATION
VCL_VC_6_STATIC inline void vsl_block_binary_write(vsl_b_ostream &os, const unsigned long* begin, unsigned nelems)
{
  vsl_b_write(os, true); // Error check that this is a specialised version
  char *block = new char[VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(unsigned long)) * nelems];

  unsigned long nbytes = vsl_convert_to_arbitrary_length(begin, (unsigned char *)block, nelems);
  vsl_b_write(os, nbytes);

  os.os().write( block, nbytes);
  delete [] block;
}

//: Read a block of unsigned longs from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
VCL_DEFINE_SPECIALIZATION
VCL_VC_6_STATIC inline void vsl_block_binary_read(vsl_b_istream &is, unsigned long* begin, unsigned nelems)
{
  vsl_block_binary_read_confirm_specialisation(is, true);
  if (!is) return;
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
      vcl_cerr << "\nI/O ERROR: vsl_block_binary_read(.., unsigned long*,..)"
               << " Corrupted data stream\n";
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    }
  }
}


/////////////////////////////////////////////////////////////////////////

#if 0
// This test will be replaced with !VCL_PTRDIFF_T_IS_A_STANDARD_TYPE
// When that macro is working.

//: Write a block of vcl_ptrdiff_ts to a vsl_b_ostream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
inline void vsl_block_binary_write(vsl_b_ostream &os, const vcl_ptrdiff_t* begin, unsigned nelems)
{
  vsl_b_write(os, true); // Error check that this is a specialised version
  char *block = new char[VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(vcl_ptrdiff_t)) * nelems];

  unsigned long nbytes = vsl_convert_to_arbitrary_length(begin, (unsigned char *)block, nelems);
  vsl_b_write(os, nbytes);

  os.os().write( block, nbytes);
  delete [] block;
}

//: Read a block of vcl_ptrdiff_ts from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
inline void vsl_block_binary_read(vsl_b_istream &is, vcl_ptrdiff_t* begin, unsigned nelems)
{
  vsl_block_binary_read_confirm_specialisation(is, true);
  if (!is) return;
  unsigned long nbytes;
  vsl_b_read(is, nbytes);
  if (nbytes)
  {
    char *block = new char[VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(vcl_ptrdiff_t)) * nelems];
    is.is().read(block, nbytes);
    unsigned long n_bytes_converted =
    vsl_convert_from_arbitrary_length((unsigned char *)block, begin, nelems);
    delete [] block;
    if (n_bytes_converted != nbytes)
    {
      vcl_cerr << "\nI/O ERROR: vsl_block_binary_read(.., vcl_ptrdiff_t*,..)"
               << " Corrupted data stream\n";
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    }
  }
}


/////////////////////////////////////////////////////////////////////////

//: Write a block of vcl_size_ts to a vsl_b_ostream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
inline void vsl_block_binary_write(vsl_b_ostream &os, const vcl_size_t* begin, unsigned nelems)
{
  vsl_b_write(os, true); // Error check that this is a specialised version
  char *block = new char[VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(vcl_size_t)) * nelems];

  unsigned long nbytes = vsl_convert_to_arbitrary_length(begin, (unsigned char *)block, nelems);
  vsl_b_write(os, nbytes);

  os.os().write( block, nbytes);
  delete [] block;
}

//: Read a block of vcl_size_ts from a vsl_b_istream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
inline void vsl_block_binary_read(vsl_b_istream &is, vcl_size_t* begin, unsigned nelems)
{
  vsl_block_binary_read_confirm_specialisation(is, true);
  if (!is) return;
  unsigned long nbytes;
  vsl_b_read(is, nbytes);
  if (nbytes)
  {
    char *block = new char[VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(vcl_size_t)) * nelems];
    is.is().read(block, nbytes);
    unsigned long n_bytes_converted =
    vsl_convert_from_arbitrary_length((unsigned char *)block, begin, nelems);
    delete [] block;
    if (n_bytes_converted != nbytes)
    {
      vcl_cerr << "\nI/O ERROR: vsl_block_binary_read(.., vcl_size_t*,..)"
               << " Corrupted data stream\n";
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    }
  }
}

#endif // 0


//: Write a block of values to a vsl_b_ostream
// If you want to output a block of fundamental data types very efficiently,
// then just #include <vsl_binary_explicit_io.h>
template <class T>
VCL_VC_6_STATIC inline void vsl_block_binary_write(vsl_b_ostream &os, const T* begin, unsigned nelems)
{
  vsl_b_write(os, false); // Error check that this is a specialised version
  while (nelems--)
    vsl_b_write(os, *(begin++));
}

//: Read a block of values from a vsl_b_istream
// If you want to output a block of fundamental data types very efficiently,
// then just #include <vsl_binary_explicit_io.h>
template <class T>
VCL_VC_6_STATIC inline void vsl_block_binary_read(vsl_b_istream &is, T* begin, unsigned nelems)
{
  vsl_block_binary_read_confirm_specialisation(is, false);
  if (!is) return;
  while (nelems--)
    vsl_b_read(is, *(begin++));
}

#endif // vsl_block_binary_io_h_
