//:
// \file
// \author Tim Cootes

#include "vil2_io_image_view.h"

//: Binary save vil2_image_view to stream.
void vsl_b_write(vsl_b_ostream &os, const vil2_image_view<T>& image)
{
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, image.ni());
  vsl_b_write(os, image.nj());
  vsl_b_write(os, image.np());
  vsl_b_write(os, image.istep());
  vsl_b_write(os, image.jstep());
  vsl_b_write(os, image.planestep());
  vsl_b_write(os, image.memory_chunk());

  int offset = (image.top_left_ptr()-(const T*)image.memory_chunk().data());
  vsl_b_write(os, offset);
}


//: Binary load vil2_image_view from stream.
void vsl_b_read(vsl_b_istream &is, vil2_image_view<T>& image)
{
  if (!is) return;

  unsigned ni,nj,np;
  int istep,jstep,pstep;
  vil2_memory_chunk_sptr chunk;
  int offset;

  short w;
  vsl_b_read(is, w);
  switch(w)
  {
  case 1:
	vsl_b_read(os, ni);
	vsl_b_read(os, nj);
	vsl_b_read(os, np);
	vsl_b_read(os, istep);
	vsl_b_read(os, jstep);
	vsl_b_read(os, pstep);
	vsl_b_read(os, chunk);
	vsl_b_read(os, offset);
	image = vil2_image_view(chunk,(const T* chunk.data())+offset,
                            ni,nj,np,istep,jstep,pstep);
    break;

  default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vil2_image_view<T>&) \n";
    vcl_cerr << "           Unknown version number "<< w << "\n";
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

#undef read_case_macro

//: Binary load vil2_image_view from stream  onto the heap
void vsl_b_read(vsl_b_istream &is, vil2_image_view<T>*& p)
{
  delete p;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    p = new vil2_image_view();
    vsl_b_read(is, *p);
  }
  else
    p = 0;
}

//: Print human readable summary of a vil2_image_view object to a stream
void vsl_print_summary(vcl_ostream& os,const vil2_image_view<T>& image)
{
  image.print(os);
}
