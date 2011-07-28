// This is core/vsl/vsl_block_binary_rle.h
#ifndef vsl_block_binary_rle_h_
#define vsl_block_binary_rle_h_
//:
// \file
// \brief Efficiently Store/Load a block of values using Run length encoding.
// \author Ian Scott, Imorphics, Jun 2010

#include <vcl_algorithm.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_binary_explicit_io.h>
#include <vcl_iostream.h>

//: Write a block of values to a vsl_b_ostream, as (value count) pairs.
template <class T>
inline void vsl_block_binary_rle_write(vsl_b_ostream &os, const T* begin, vcl_size_t nelems)
{
  short version=1;
  vsl_b_write(os, version);

  if (nelems==0) return;

  const T* last=begin;
  const T* current=begin+1;
  vsl_b_write(os, *last);
  vcl_size_t block_count=1;
  while (--nelems) // pre-decrement, since we have already written one element.
  {
    if (*last!=*current)
    {
      vsl_b_write(os, block_count);
      last=current;
      vsl_b_write(os, *last);
      block_count=0;
    }
    ++current;
    ++block_count;
  }
  vsl_b_write(os, block_count);
}

//: Read a block of values from a vsl_b_ostream, potentially very efficiently for fundamental types.
template <class T>
inline void vsl_block_binary_rle_read(vsl_b_istream &is, T* begin, vcl_size_t nelems)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1:
    {
      if (nelems==0) return;

      T* last=begin;
      while (nelems)
      {
        vsl_b_read(is, *last);
        vcl_size_t block_count;
        vsl_b_read(is, block_count);
        if (block_count > nelems)
        {
          vcl_cerr << "I/O ERROR: vsl_block_binary_rle_read(&is, T* begin, vcl_size_t nelems)\n"
                   << "           Too many elements in stream.\n";
          is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
          return;
        }
        vcl_fill(last+1, last+block_count, *last);
        last+=block_count;
        nelems-=block_count;
      }
      break;
    }
   default:
    vcl_cerr << "I/O ERROR: vsl_block_binary_rle_read(&is, T* begin, vcl_size_t nelems)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

#endif // vsl_block_binary_rle_h_
