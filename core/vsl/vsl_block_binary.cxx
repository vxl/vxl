// This is core/vsl/vsl_block_binary.cxx
//:
// \file
// \brief Set of functions to do binary IO on a block of values.
// \author Ian Scott, ISBE Manchester, Feb 2003

#include <cstddef>
#include <new>
#include <algorithm>
#include <cstdlib>
#include "vsl_block_binary.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

struct vsl_block_t
{
  char * ptr;
  std::size_t size;
};

vsl_block_t allocate_up_to(std::size_t nbytes)
{
  vsl_block_t block = {nullptr, nbytes};
  while (true)
  {
    try
    {
      block.ptr = new char[block.size];
    }
    catch (const std::bad_alloc& )
    {
    }
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
    std::cerr << "I/O ERROR: vsl_block_binary_read()\n";
    if (specialised)
      std::cerr << "           Data was saved using unspecialised slow form and is being loaded\n"
               << "           using specialised fast form.\n\n";
    else
      std::cerr << "           Data was saved using specialised fast form and is being loaded\n"
               << "           using unspecialised slow form.\n\n";

    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
  }
}


/////////////////////////////////////////////////////////////////////////

//: Write a block of floats to a vsl_b_ostream
template <class T>
void vsl_block_binary_write_float_impl(vsl_b_ostream &os, const T* begin, std::size_t nelems)
{
  vsl_b_write(os, true); // Error check that this is a specialised version

  const std::size_t wanted = sizeof(T) * nelems;
  vsl_block_t block = allocate_up_to(wanted);

  // multiple-block version works equally efficiently with single block
  const std::size_t items_per_block = block.size / sizeof(T);

  // convert and save the data from the start.
  while (nelems > 0)
  {
    std::size_t items = std::min(items_per_block, nelems);
    std::size_t bytes = sizeof(T) * items;
    vsl_swap_bytes_to_buffer((const char *)begin, (char *)block.ptr, sizeof(T), items);
    os.os().write( block.ptr, bytes);
    begin += items;
    nelems -= items;
  }
   delete [] block.ptr;
}

//: Read a block of floats from a vsl_b_ostream
template <class T>
void vsl_block_binary_read_float_impl(vsl_b_istream &is, T* begin, std::size_t nelems)
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
void vsl_block_binary_write_int_impl(vsl_b_ostream &os, const T* begin, std::size_t nelems)
{

  vsl_b_write(os, true); // Error check that this is a specialised version

  const std::size_t wanted = VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(T)) * nelems;
  vsl_block_t block = allocate_up_to(wanted  );

  if (block.size == wanted)
  {
    // Do simple single block version
    std::size_t nbytes = vsl_convert_to_arbitrary_length(begin,
                                                        (unsigned char *)block.ptr, nelems);
    vsl_b_write(os, nbytes);
    os.os().write( block.ptr, nbytes);
  }
  else
  {
    // Do multiple-block version
    const std::size_t items_per_block = block.size / VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(T));
    std::size_t n=nelems; //Number of items still to be converted.
    const T* p=begin; //Pointer to next block of data to be converted.
    assert (n > items_per_block);
    // Convert the data - just counting bytes for now.
    std::size_t n_bytes=0;
    while (n > 0)
    {
      std::size_t items = std::min(items_per_block, n);
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
      std::size_t items = std::min(items_per_block, n);
      std::size_t bytes = vsl_convert_to_arbitrary_length(p,
                                                         (unsigned char *)block.ptr, items );
      os.os().write( block.ptr, bytes);
      p += items;
      n -= items;
    }
  }
   delete [] block.ptr;
}

/////////////////////////////////////////////////////////////////////////

//: Read a block of signed ints from a vsl_b_istream
template <class T>
void vsl_block_binary_read_int_impl(vsl_b_istream &is, T* begin, std::size_t nelems)
{
  vsl_block_binary_read_confirm_specialisation(is, true);
  if (!is) return;
  std::size_t nbytes;
  vsl_b_read(is, nbytes);
  if (nbytes==0) return;


  vsl_block_t block = allocate_up_to(nbytes);

  std::size_t n_bytes_converted = 0;
  if (block.size == nbytes)
  {
    // Do simple single block version
    is.is().read(block.ptr, block.size);
    n_bytes_converted =
      vsl_convert_from_arbitrary_length((unsigned char *)block.ptr, begin, nelems);
  }
  else    // Do multi-block version
  {
    std::size_t offset=0;
    std::size_t bytes_left = nbytes;
    std::size_t bytes_read = 0;
    while (nelems > 0)
    {
      assert (offset < block.size);

      // fill block beyond offset with as much as possible.
      std::size_t bytes = std::min((std::size_t)nbytes-bytes_read, block.size-offset);
      is.is().read(block.ptr+offset, bytes);
      bytes_read += bytes;

      if (!is) break;

      // count number of ints in block.
      std::size_t elems=0;
      for (unsigned char *p = (unsigned char *)block.ptr, *p_end=p+bytes+offset; p!=p_end; ++p)
        elems += *p >> 7;

      if (elems > nelems)
      {
        std::cerr << "\nI/O ERROR: vsl_block_binary_read(.., int*,..)"
                 << " Corrupted data stream\n";
        is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
        break;
      }

      // convert ints;
      std::size_t bytes_converted =
        vsl_convert_from_arbitrary_length((unsigned char *)block.ptr, begin, elems);
      nelems -= elems;
      begin += elems;

      offset = (bytes + offset) - bytes_converted; // avoid overflow.
      n_bytes_converted += bytes_converted;
      bytes_left -= bytes_converted;

      // shift remaining (offset) bytes to front of block.
      std::memcpy(block.ptr, block.ptr + bytes_converted, offset);
    }
    if (bytes_left != 0 || nelems != 0 || bytes_read != nbytes)
    {
      std::cerr << "\nI/O ERROR: vsl_block_binary_read(.., int*,..)"
               << " Corrupted data stream\n";
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    }
  }
  if (n_bytes_converted != nbytes)
  {
    std::cerr << "\nI/O ERROR: vsl_block_binary_read(.., int*,..)"
             << " Corrupted data stream\n";
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
  }
   delete [] block.ptr;
}

/////////////////////////////////////////////////////////////////////////

//: Write a block of bytes to a vsl_b_ostream
template <class T>
void vsl_block_binary_write_byte_impl(vsl_b_ostream &os, const T* begin, std::size_t nelems)
{
  vsl_b_write(os, true); // Error check that this is a specialised version
  os.os().write((char*) begin, nelems);
}

//: Read a block of bytes from a vsl_b_ostream
template <class T>
void vsl_block_binary_read_byte_impl(vsl_b_istream &is, T* begin, std::size_t nelems)
{
  // There are no complications here, to deal with low memory,
  // because the load is done in place.
  vsl_block_binary_read_confirm_specialisation(is, true);
  if (!is) return;
  is.is().read((char*) begin, nelems);
}


// Instantiate templates for POD types.

template void vsl_block_binary_write_float_impl(vsl_b_ostream &, const double*, std::size_t);
template void vsl_block_binary_write_float_impl(vsl_b_ostream &, const float*, std::size_t);

template void vsl_block_binary_read_float_impl(vsl_b_istream &, double*, std::size_t);
template void vsl_block_binary_read_float_impl(vsl_b_istream &, float*, std::size_t);

template void vsl_block_binary_write_int_impl(vsl_b_ostream &, const long*, std::size_t);
template void vsl_block_binary_write_int_impl(vsl_b_ostream &, const unsigned long*, std::size_t);
template void vsl_block_binary_write_int_impl(vsl_b_ostream &, const int*, std::size_t);
template void vsl_block_binary_write_int_impl(vsl_b_ostream &, const unsigned int*, std::size_t);
template void vsl_block_binary_write_int_impl(vsl_b_ostream &, const short*, std::size_t);
template void vsl_block_binary_write_int_impl(vsl_b_ostream &, const unsigned short*, std::size_t);

template void vsl_block_binary_read_int_impl(vsl_b_istream &, long*, std::size_t);
template void vsl_block_binary_read_int_impl(vsl_b_istream &, unsigned long*, std::size_t);
template void vsl_block_binary_read_int_impl(vsl_b_istream &, int*, std::size_t);
template void vsl_block_binary_read_int_impl(vsl_b_istream &, unsigned int*, std::size_t);
template void vsl_block_binary_read_int_impl(vsl_b_istream &, short*, std::size_t);
template void vsl_block_binary_read_int_impl(vsl_b_istream &, unsigned short*, std::size_t);

template void vsl_block_binary_write_byte_impl(vsl_b_ostream &, const signed char*, std::size_t);
template void vsl_block_binary_write_byte_impl(vsl_b_ostream &, const unsigned char*, std::size_t);

template void vsl_block_binary_read_byte_impl(vsl_b_istream &, signed char*, std::size_t);
template void vsl_block_binary_read_byte_impl(vsl_b_istream &, unsigned char*, std::size_t);

#if VXL_INT_64_IS_LONGLONG
template void vsl_block_binary_write_int_impl(vsl_b_ostream &, const vxl_int_64*, std::size_t);
template void vsl_block_binary_write_int_impl(vsl_b_ostream &, const vxl_uint_64*, std::size_t);
template void vsl_block_binary_read_int_impl(vsl_b_istream &, vxl_int_64*, std::size_t);
template void vsl_block_binary_read_int_impl(vsl_b_istream &, vxl_uint_64*, std::size_t);
#endif //VXL_HAS_INT_64 && !VXL_INT_64_IS_LONG
