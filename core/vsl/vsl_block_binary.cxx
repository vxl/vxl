// This is core/vsl/vsl_block_binary.cxx
//:
// \file
// \brief Set of functions to do binary IO on a block of values.
// \author Ian Scott, ISBE Manchester, Feb 2003

#include "vsl_block_binary.h"
#include <vcl_cstddef.h>
#include <vcl_new.h>
#include <vcl_algorithm.h>
#include <vcl_cstdlib.h>
#include <vcl_cassert.h>

struct vsl_block_t
{
  char * ptr;
  vcl_size_t size;
};

vsl_block_t allocate_up_to(vcl_size_t nbytes)
{
  vsl_block_t block = {0, nbytes};
  while (true)
  {
#if VCL_HAS_EXCEPTIONS
    try
    {
      block.ptr = new char[block.size];
    }
    catch (const vcl_bad_alloc& )
    {
    }
#else
    //use malloc because gcc's new still tries to throw a bad alloc even with -fno_exceptions
    block.ptr = (char *)vcl_malloc(block.size);
#endif
    if (block.ptr)
      return block;
    block.size /= 2;
  }
}


//: Error checking.
void vsl_block_binary_read_confirm_specialisation(vsl_b_istream &is, bool specialised)
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


/////////////////////////////////////////////////////////////////////////

//: Write a block of floats to a vsl_b_ostream
template <class T>
void vsl_block_binary_write_float_impl(vsl_b_ostream &os, const T* begin, vcl_size_t nelems)
{
  vsl_b_write(os, true); // Error check that this is a specialised version

  const vcl_size_t wanted = sizeof(T) * nelems;
  vsl_block_t block = allocate_up_to(wanted);

  // multiple-block version works equally efficiently with single block
  const vcl_size_t items_per_block = block.size / sizeof(T);

  // convert and save the data from the start.
  while (nelems > 0)
  {
    vcl_size_t items = vcl_min(items_per_block, nelems);
    vcl_size_t bytes = sizeof(T) * items;
    vsl_swap_bytes_to_buffer((const char *)begin, (char *)block.ptr, sizeof(T), items);
    os.os().write( block.ptr, bytes);
    begin += items;
    nelems -= items;
  }
#if VCL_HAS_EXCEPTIONS
   delete [] block.ptr;
#else
  vcl_free(block.ptr);
#endif
}

//: Write a block of floats to a vsl_b_ostream
template <class T>
void vsl_block_binary_read_float_impl(vsl_b_istream &is, T* begin, vcl_size_t nelems)
{
  // There are no complications here, to deal with low memory,
  // because the byte swapping can be done in place.
  vsl_block_binary_read_confirm_specialisation(is, true);
  if (!is) return;
  is.is().read((char*) begin, nelems*sizeof(T));
  vsl_swap_bytes((char *)begin, sizeof(T), nelems);
}

/////////////////////////////////////////////////////////////////////////

//: Write a block of signed ints to a vsl_b_ostream
template <class T>
void vsl_block_binary_write_int_impl(vsl_b_ostream &os, const T* begin, vcl_size_t nelems)
{
  vsl_b_write(os, true); // Error check that this is a specialised version

  const vcl_size_t wanted = sizeof(T) * nelems;
  vsl_block_t block = allocate_up_to(wanted  );

  if (block.size == wanted)
  {
    // Do simple single block version
    vcl_size_t nbytes = vsl_convert_to_arbitrary_length(begin,
      (unsigned char *)block.ptr, nelems);
    vsl_b_write(os, nbytes);
    os.os().write( block.ptr, nbytes);
  }
  else
  {
    // Do multiple-block version
    const vcl_size_t items_per_block = block.size / VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(T));
    vcl_size_t n=nelems; //Number of items still to be converted.
    const T* p=begin; //Pointer to next block of data to be converted.
    assert (n > items_per_block);
    // Convert the data - just counting bytes for now.
    vcl_size_t n_bytes=0;
    while (n > 0)
    {
      vcl_size_t items = vcl_min(items_per_block, n);
      n_bytes += vsl_convert_to_arbitrary_length(p,
        (unsigned char *)block.ptr, items);
      p += items;
      n -= items;
    }

    vsl_b_write(os, n_bytes);
    n=nelems;
    p=begin;

    // Now convert and save the data from the start.
    while (n > 0)
    {
      vcl_size_t items = vcl_min(items_per_block, n);
      vcl_size_t bytes = vsl_convert_to_arbitrary_length(p,
        (unsigned char *)block.ptr, items );
      os.os().write( block.ptr, bytes);
      p += items;
      n -= items;
    }
  }
#if VCL_HAS_EXCEPTIONS
   delete [] block.ptr;
#else
  vcl_free(block.ptr);
#endif
}

/////////////////////////////////////////////////////////////////////////

//: Read a block of signed ints from a vsl_b_istream
template <class T>
void vsl_block_binary_read_int_impl(vsl_b_istream &is, T* begin, vcl_size_t nelems)
{
  vsl_block_binary_read_confirm_specialisation(is, true);
  if (!is) return;
  vcl_size_t nbytes;
  vsl_b_read(is, nbytes);
  if (nbytes==0) return;


  vsl_block_t block = allocate_up_to(nbytes);

  vcl_size_t n_bytes_converted = 0;
  if (block.size == nbytes)
  {
    // Do simple single block version
    is.is().read(block.ptr, block.size);
    n_bytes_converted =
      vsl_convert_from_arbitrary_length((unsigned char *)block.ptr, begin, nelems);
  }
  else    // Do multi-block version
  {
    vcl_size_t offset=0;
    vcl_size_t bytes_left = nbytes;
    vcl_size_t bytes_read = 0;
    while (nelems > 0)
    {
      assert (offset < block.size);

      // fill block beyond offset with as much as possible.
      vcl_size_t bytes = vcl_min((vcl_size_t)nbytes-bytes_read, block.size-offset);
      is.is().read(block.ptr+offset, bytes);
      bytes_read += bytes;

      if (!is) break;

      // count number of ints in block.
      vcl_size_t elems=0;
      for (unsigned char *p = (unsigned char *)block.ptr, *p_end=p+bytes+offset; p!=p_end; ++p)
        elems += *p >> 7;

      if (elems > nelems)
      {
        vcl_cerr << "\nI/O ERROR: vsl_block_binary_read(.., int*,..)"
                 << " Corrupted data stream\n";
        is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
        break;
      }

      // convert ints;
      vcl_size_t bytes_converted =
        vsl_convert_from_arbitrary_length((unsigned char *)block.ptr, begin, elems);
      nelems -= elems;
      begin += elems;

      offset = (bytes + offset) - bytes_converted; // avoid overflow.
      n_bytes_converted += bytes_converted;
      bytes_left -= bytes_converted;

      // shift remaining (offset) bytes to front of block.
      vcl_memcpy(block.ptr, block.ptr + bytes_converted, offset);
    }
    if (bytes_left != 0 || nelems != 0 || bytes_read != nbytes)
    {
      vcl_cerr << "\nI/O ERROR: vsl_block_binary_read(.., int*,..)"
               << " Corrupted data stream\n";
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    }
  }
  if (n_bytes_converted != nbytes)
  {
    vcl_cerr << "\nI/O ERROR: vsl_block_binary_read(.., int*,..)"
             << " Corrupted data stream\n";
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
  }
#if VCL_HAS_EXCEPTIONS
   delete [] block.ptr;
#else
  vcl_free(block.ptr);
#endif
}


// Instantiate templates for POD types.

template void vsl_block_binary_write_float_impl(vsl_b_ostream &, const double*, vcl_size_t);
template void vsl_block_binary_write_float_impl(vsl_b_ostream &, const float*, vcl_size_t);

template void vsl_block_binary_read_float_impl(vsl_b_istream &, double*, vcl_size_t);
template void vsl_block_binary_read_float_impl(vsl_b_istream &, float*, vcl_size_t);

template void vsl_block_binary_write_int_impl(vsl_b_ostream &, const long*, vcl_size_t);
template void vsl_block_binary_write_int_impl(vsl_b_ostream &, const unsigned long*, vcl_size_t);
template void vsl_block_binary_write_int_impl(vsl_b_ostream &, const int*, vcl_size_t);
template void vsl_block_binary_write_int_impl(vsl_b_ostream &, const unsigned int*, vcl_size_t);
template void vsl_block_binary_write_int_impl(vsl_b_ostream &, const short*, vcl_size_t);
template void vsl_block_binary_write_int_impl(vsl_b_ostream &, const unsigned short*, vcl_size_t);

template void vsl_block_binary_read_int_impl(vsl_b_istream &, long*, vcl_size_t);
template void vsl_block_binary_read_int_impl(vsl_b_istream &, unsigned long*, vcl_size_t);
template void vsl_block_binary_read_int_impl(vsl_b_istream &, int*, vcl_size_t);
template void vsl_block_binary_read_int_impl(vsl_b_istream &, unsigned int*, vcl_size_t);
template void vsl_block_binary_read_int_impl(vsl_b_istream &, short*, vcl_size_t);
template void vsl_block_binary_read_int_impl(vsl_b_istream &, unsigned short*, vcl_size_t);

