// This is mul/vil2/io/vil2_io_image_view.h
#ifndef vil2_io_image_view_h_
#define vil2_io_image_view_h_
//:
// \file
// \author Tim Cootes

#include <vcl_cstddef.h>
#include <vil2/vil2_image_view.h>
#include <vil2/io/vil2_io_memory_chunk.h>
#include <vil2/io/vil2_io_smart_ptr.h>

//: Binary save vil2_image_view<T> to stream.
template<class T>
inline void vsl_b_write(vsl_b_ostream &os, const vil2_image_view<T>& image)
{
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, image.ni());
  vsl_b_write(os, image.nj());
  vsl_b_write(os, image.nplanes());
  vsl_b_write(os, image.istep());
  vsl_b_write(os, image.jstep());
  vsl_b_write(os, image.planestep());
  if (image.size()>0)
  {
    vsl_b_write(os, image.memory_chunk());

    vcl_ptrdiff_t offset = (image.top_left_ptr()-(const T*)image.memory_chunk()->data());
    vsl_b_write(os, offset);
  }
}


//: Binary load vil2_image_view<T> from stream.
template<class T>
inline void vsl_b_read(vsl_b_istream &is, vil2_image_view<T>& image)
{
  if (!is) return;

  unsigned ni,nj,np;
  vcl_ptrdiff_t istep,jstep,pstep;
  vil2_memory_chunk_sptr chunk;
  vcl_ptrdiff_t offset;

  short w;
  vsl_b_read(is, w);
  switch(w)
  {
  case 1:
    vsl_b_read(is, ni);
    vsl_b_read(is, nj);
    vsl_b_read(is, np);
    vsl_b_read(is, istep);
    vsl_b_read(is, jstep);
    vsl_b_read(is, pstep);
    if (ni*nj*np==0) image.set_size(0,0,0);
    else
    {
      vsl_b_read(is, chunk);
      vsl_b_read(is, offset);
      const T* data = (const T*) chunk->data();
      image = vil2_image_view<T>(chunk,data+offset,
                                 ni,nj,np,istep,jstep,pstep);
    }
    break;

  default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vil2_image_view<T>&)\n"
             << "           Unknown version number "<< w << "\n";
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//: Binary load vil2_image_view<T> from stream  onto the heap
template<class T>
inline void vsl_b_read(vsl_b_istream &is, vil2_image_view<T>*& p)
{
  delete p;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    p = new vil2_image_view<T>();
    vsl_b_read(is, *p);
  }
  else
    p = 0;
}

//: Print human readable summary of a vil2_image_view<T> object to a stream
template<class T>
inline void vsl_print_summary(vcl_ostream& os,const vil2_image_view<T>& image)
{
  image.print(os);
}

#endif
