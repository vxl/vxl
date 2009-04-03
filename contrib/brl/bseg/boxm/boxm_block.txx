#ifndef boxm_block_txx_
#define boxm_block_txx_

#include "boxm_block.h"

#include <vgl/io/vgl_io_box_3d.h>

template <class T>
void boxm_block<T>::b_read(vsl_b_istream &is)
{
  if (!is) return;

  short version;
  vsl_b_read(is,version);
  switch (version)
  {
    case (1):
      vsl_b_read(is, bbox_);
      octree_ = new T();
      octree_->b_read(is);
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, boxm_block<T>&)\n"
               << "           Unknown version number "<< version << '\n';
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}


template <class T>
void boxm_block<T>::b_write(vsl_b_ostream &os)
{
  vsl_b_write(os, version_no());
  vsl_b_write(os, bbox_);
  octree_->b_write(os);
}

#define BOXM_BLOCK_INSTANTIATE(T) \
template boxm_block<T >

#endif
