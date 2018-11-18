#include <sstream>
#include <iostream>
#include <iomanip>
#include "bvxm_grid_to_image_stack.h"
//
#include <vil/vil_save.h>
#include <vul/vul_file.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template<>
bool bvxm_grid_to_image_stack::write_grid_to_image_stack(bvxm_voxel_grid<vnl_float_3> *grid, std::string directory)
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
  bvxm_voxel_grid<vnl_float_3>::iterator grid_it = grid->begin();
  unsigned i =0;
  for (; grid_it != grid->end(); ++grid_it, i++)
  {
    std::stringstream filename;
    filename << directory << std::setw(4) << std::setfill('0') << i << bvxm_extension<unsigned char>();
    vil_image_view_base_sptr img = new vil_image_view<vil_rgb<unsigned char> >(ni, nj, 1);
    bvxm_slab_to_image::slab_to_image(*grid_it, img);
    vil_save(*img.ptr(), filename.str().c_str());
  }
  return true;
}

template<>
bool bvxm_grid_to_image_stack::write_grid_to_image_stack(bvxm_voxel_grid<vnl_float_4> *grid, std::string directory)
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
  bvxm_voxel_grid<vnl_float_4>::iterator grid_it = grid->begin();
  unsigned i =0;
  for (; grid_it != grid->end(); ++grid_it, i++)
  {
    std::stringstream filename;
    filename << directory << std::setw(4) << std::setfill('0') << i << bvxm_extension<unsigned char>();
    vil_image_view_base_sptr img = new vil_image_view<vil_rgba<unsigned char> >(ni, nj, 1);
    bvxm_slab_to_image::slab_to_image(*grid_it, img);
    vil_save(*img.ptr(), filename.str().c_str());
  }
  return true;
}

template<>
std::string bvxm_extension<float>() { return ".tiff"; }

template<>
std::string bvxm_extension<unsigned char>() { return ".tiff"; }
