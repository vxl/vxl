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
    block.ptr = (char *)vcl_malloc(block.size);
#endif
    if (block.ptr)
      return block;
    block.size /= 2;
  }
}

/////////////////////////////////////////////////////////////////////////

//: Write a block of signed ints to a vsl_b_ostream
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
VCL_DEFINE_SPECIALIZATION
void vsl_block_binary_write(vsl_b_ostream &os, const int* begin, unsigned nelems)
{
  vsl_b_write(os, true); // Error check that this is a specialised version

  const vcl_size_t wanted = VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(int)) * nelems;
  vsl_block_t block = allocate_up_to(wanted  );
  
  if(block.size == wanted)
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
    const vcl_size_t items_per_block = block.size / VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(int));
    vcl_size_t n=nelems; //Number of items still to be converted.
    const int* p=begin; //Pointer to next block of data to be converted.
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
// This function is very speed efficient, but
// temporarily allocates a block of memory the about 1.2 times
// size of the block being read.
VCL_DEFINE_SPECIALIZATION
void vsl_block_binary_read(vsl_b_istream &is, int* begin, unsigned nelems)
{
  vsl_block_binary_read_confirm_specialisation(is, true);
  if (!is) return;
  unsigned long nbytes;
  vsl_b_read(is, nbytes);
  if (nbytes==0) return;


  vsl_block_t block = allocate_up_to(nbytes);

  vcl_size_t n_bytes_converted = 0;
  if(block.size == nbytes)
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

