// This is brl/bseg/bvxm/grid/io/bvxm_grid_to_image_stack_io.h
#ifndef bvxm_grid_to_image_stack_io_h
#define bvxm_grid_to_image_stack_io_h

//:
// \file
// \brief A Class to save a voxel grid as a stack of images
//
// \author Isabel Restrepo mir@lems.brown.edu
//
// \date  6/29/09
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "../bvxm_voxel_grid.h"
#include "bvxm_slab_to_image.h"
#include <vul/vul_file.h>
#include <vcl_iostream.h>
#include <vil/vil_save.h>

template<class T>
vcl_string bvxm_extension();

template<>
vcl_string bvxm_extension<float>(){return ".tiff";}
  
template<>
vcl_string bvxm_extension<unsigned char>(){return ".png";}


class bvxm_grid_to_image_stack
{
 public:
  
  template<class T, unsigned N>
  static bool write_grid_to_image_stack(bvxm_voxel_grid<vnl_vector_fixed<T,N> > *grid, vcl_string directory);
  
  template<class T>
  static bool write_grid_to_image_stack(bvxm_voxel_grid<T> *grid, vcl_string directory);
  
};


// saves a voxel grid of a 3-d vnl_vectors. The world is saved as a stack of RGB images that can ble loaded by dristhi
template<class T, unsigned N>
bool bvxm_grid_to_image_stack::write_grid_to_image_stack(bvxm_voxel_grid<vnl_vector_fixed<T, N> > *grid, vcl_string directory)
{
  
  if (vul_file::is_directory(directory))
    vul_file::delete_file_glob(directory+"/*");
  else {
    if (vul_file::exists(directory))
      vul_file::delete_file_glob(directory);
    vul_file::make_directory(directory);
  }
  
  unsigned ni = grid->grid_size().x();
  unsigned nj = grid->grid_size().y();
   
  typename bvxm_voxel_grid<vnl_vector_fixed<T, N> >::iterator grid_it = grid->begin();
  unsigned i =0;
  for(; grid_it != grid->end(); ++grid_it, i++)
  {
    vcl_stringstream filename;
    filename << directory << "/slab_" << i << bvxm_extension<T>();
   // vil_image_view_base_sptr img = new vil_image_view<T>(ni, nj, N);
    bvxm_slab_to_image::write_slab_as_image((*grid_it), filename.str());
  //  vil_save(*img.ptr(), filename.str().c_str());
  }
  
}
    
    
// saves a voxel grid of a 3-d vnl_vectors. The world is saved as a stack of RGB images that can ble loaded by dristhi
template<class T>
bool bvxm_grid_to_image_stack::write_grid_to_image_stack(bvxm_voxel_grid<T> *grid, vcl_string directory)
{
  if (vul_file::is_directory(directory))
  vul_file::delete_file_glob(directory+"/*");
  else {
  if (vul_file::exists(directory))
    vul_file::delete_file_glob(directory);
  vul_file::make_directory(directory);
  }


  unsigned ni = grid->grid_size().x();
  unsigned nj = grid->grid_size().y();
  typename bvxm_voxel_grid<T>::iterator grid_it = grid->begin();
  unsigned i =0;
  for(; grid_it != grid->end(); ++grid_it, i++)
  {
    vcl_stringstream filename;
    filename << directory << "/slab_" << i << bvxm_extension<T>();
 //   vil_image_view_base_sptr img = new vil_image_view<T>(ni, nj, 1);
    bvxm_slab_to_image::write_slab_as_image(*grid_it, filename.str());
  //  vil_save(*img.ptr(), filename.str().c_str());
  }

}
#endif
