#ifndef boxm_block_hxx_
#define boxm_block_hxx_

#include <iostream>
#include "boxm_block.h"

#include <vgl/io/vgl_io_box_3d.h>
template <class T>
void boxm_block<T>::init_tree(T * octree)
{
  if(octree_){
   //release old memory unless the same tree
   if(octree!=octree_)
     delete octree_;
   }
  octree_=octree;
  octree->set_bbox(bbox_);
}

template <class T>
void boxm_block<T>::set_tree(T * octree)
{
  if(octree_) //release old memory
    delete octree_;
  octree_=octree;
}

template <class T>
unsigned int boxm_block<T>::size()
{
  if (octree_) {
    return octree_->size();
  }
  else {
    return 0;
  }
}

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
      // if already allocated, destroy it and read from the file
      if (octree_ != nullptr) {
        delete octree_;
      }
      octree_ = new T();
      octree_->b_read(is);
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, boxm_block<T>&)\n"
               << "           Unknown version number "<< version << '\n';
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}


template <class T>
void boxm_block<T>::b_write(vsl_b_ostream &os, const bool save_internal_nodes, const bool platform_independent)
{
  vsl_b_write(os, version_no());
  vsl_b_write(os, bbox_);
#if 0
  std::cout << "Internal Nodes 3: " << save_internal_nodes << std::endl;
#endif
  octree_->b_write(os, save_internal_nodes, platform_independent);
}

#define BOXM_BLOCK_INSTANTIATE(T) \
template class boxm_block<T >

#endif
