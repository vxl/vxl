// This is brl/bseg/bvxm/grid/io/bvxm_grid_to_image_stack.h
#ifndef bvxm_grid_to_image_stack_io_h
#define bvxm_grid_to_image_stack_io_h
//:
// \file
// \brief A Class to save a voxel grid as a stack of images
//
// \author Isabel Restrepo mir@lems.brown.edu
//
// \date  June 29, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bvxm/grid/bvxm_voxel_grid.h>
#include "bvxm_slab_to_image.h"
#include <vul/vul_file.h>
#include <vcl_iostream.h>
#include <vil/vil_save.h>
#include <vnl/vnl_vector_fixed.h>
#include <vcl_iomanip.h>
#include <vcl_sstream.h>

template<class T>
vcl_string bvxm_extension();

template<>
vcl_string bvxm_extension<float>();

template<>
vcl_string bvxm_extension<unsigned char>() ;

template<class T>
class bvxm_image_traits;

template<>
class bvxm_image_traits<vnl_vector_fixed<float,3> >
{
 public:
  typedef vil_rgb<unsigned char> pixel_type;
  static vcl_string extension() { return ".png"; }
};

template<>
class bvxm_image_traits<vnl_vector_fixed<float,4> >
{
 public:
  typedef vil_rgba<unsigned char> pixel_type;
  static vcl_string extension() { return ".tiff"; }
};


class bvxm_grid_to_image_stack
{
 public:
  template<class T, unsigned N>
  static bool write_grid_to_image_stack(bvxm_voxel_grid<vnl_vector_fixed<T,N> > *grid, vcl_string directory);

  template<class T>
  static bool write_grid_to_image_stack(bvxm_voxel_grid<T> *grid, vcl_string directory);
};


// saves a voxel grid as a stack of images that can ble loaded by dristhi
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
  for (; grid_it != grid->end(); ++grid_it, i++)
  {
    vcl_stringstream filename;
    filename << directory << "/slab_" <<vcl_setw(4) << vcl_setfill('0') <<  i << bvxm_image_traits<vnl_vector_fixed<T, N> >::extension();
    vil_image_view_base_sptr img = new vil_image_view< typename bvxm_image_traits<vnl_vector_fixed<T, N> >::pixel_type>(ni, nj);
    bvxm_slab_to_image::slab_to_image((*grid_it), img);
    vil_save(*img.ptr(), filename.str().c_str());
  }
  return true;
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
  for (; grid_it != grid->end(); ++grid_it, i++)
  {
    vcl_stringstream filename;
    filename << directory << vcl_setw(4) << vcl_setfill('0') << i << bvxm_extension<unsigned char>();
    vil_image_view_base_sptr img = new vil_image_view<unsigned char>(ni, nj, 1);
    bvxm_slab_to_image::slab_to_image(*grid_it, img);
    vil_save(*img.ptr(), filename.str().c_str());
  }
  return true;
}

template<>
bool bvxm_grid_to_image_stack::write_grid_to_image_stack(bvxm_voxel_grid<vnl_float_3> *grid, vcl_string directory);
template<>
bool bvxm_grid_to_image_stack::write_grid_to_image_stack(bvxm_voxel_grid<vnl_float_4> *grid, vcl_string directory);
#endif
