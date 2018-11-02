// This is mul/vil3d/io/vil3d_io_image_view.h
#ifndef vil3d_io_image_view_h_
#define vil3d_io_image_view_h_
//:
// \file
// \author Tim Cootes

#include <iostream>
#include <cstddef>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil3d/vil3d_image_view.h>
#include <vil/io/vil_io_memory_chunk.h>
#include <vil/io/vil_io_smart_ptr.h>

//: Binary save vil3d_image_view<T> to stream.
template<class T>
inline void vsl_b_write(vsl_b_ostream &os, const vil3d_image_view<T>& image)
{
  constexpr short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, image.ni());
  vsl_b_write(os, image.nj());
  vsl_b_write(os, image.nk());
  vsl_b_write(os, image.nplanes());
  vsl_b_write(os, image.istep());
  vsl_b_write(os, image.jstep());
  vsl_b_write(os, image.kstep());
  vsl_b_write(os, image.planestep());
  if (image.size()>0)
  {
    vsl_b_write(os, image.memory_chunk());

    std::ptrdiff_t offset = (image.origin_ptr()-reinterpret_cast<const T*>(image.memory_chunk()->data()));
    vsl_b_write(os, offset);
  }
}


//: Binary load vil3d_image_view<T> from stream.
template<class T>
inline void vsl_b_read(vsl_b_istream &is, vil3d_image_view<T>& image)
{
  if (!is) return;

  unsigned ni,nj,nk,np;
  std::ptrdiff_t istep,jstep,kstep,pstep;
  vil_memory_chunk_sptr chunk;
  std::ptrdiff_t offset;

  short w;
  vsl_b_read(is, w);
  switch(w)
  {
  case 1:
    vsl_b_read(is, ni);
    vsl_b_read(is, nj);
    vsl_b_read(is, nk);
    vsl_b_read(is, np);
    vsl_b_read(is, istep);
    vsl_b_read(is, jstep);
    vsl_b_read(is, kstep);
    vsl_b_read(is, pstep);
    if (ni*nj*np==0) image.set_size(0,0,0);
    else
    {
      vsl_b_read(is, chunk);
      vsl_b_read(is, offset);
      const T* data = reinterpret_cast<const T*>(chunk->data());
      image = vil3d_image_view<T>(chunk,data+offset,
                                  ni,nj,nk,np,istep,jstep,kstep,pstep);
    }
    break;

  default:
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vil3d_image_view<T>&)\n"
             << "           Unknown version number "<< w << "\n";
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//: Binary load vil3d_image_view<T> from stream  onto the heap
template<class T>
inline void vsl_b_read(vsl_b_istream &is, vil3d_image_view<T>*& p)
{
  delete p;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    p = new vil3d_image_view<T>();
    vsl_b_read(is, *p);
  }
  else
    p = 0;
}

//: Print human readable summary of a vil3d_image_view<T> object to a stream
template<class T>
inline void vsl_print_summary(std::ostream& os,const vil3d_image_view<T>& image)
{
  image.print(os);
}

#endif // vil3d_io_image_view_h_
