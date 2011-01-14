#include "bvxm_grid_to_image_stack.h"
//
#include <vil/vil_save.h>
#include <vul/vul_file.h>
#include <vcl_sstream.h>
#include <vcl_iomanip.h>

template<>
bool bvxm_grid_to_image_stack::write_grid_to_image_stack(bvxm_voxel_grid<vnl_float_3> *grid, vcl_string directory)
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
    vcl_stringstream filename;
    filename << directory << vcl_setw(4) << vcl_setfill('0') << i << bvxm_extension<unsigned char>();
    vil_image_view_base_sptr img = new vil_image_view<vil_rgb<unsigned char> >(ni, nj, 1);
    bvxm_slab_to_image::slab_to_image(*grid_it, img);
    vil_save(*img.ptr(), filename.str().c_str());
  }
  return true;
}

template<>
bool bvxm_grid_to_image_stack::write_grid_to_image_stack(bvxm_voxel_grid<vnl_float_4> *grid, vcl_string directory)
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
    vcl_stringstream filename;
    filename << directory << vcl_setw(4) << vcl_setfill('0') << i << bvxm_extension<unsigned char>();
    vil_image_view_base_sptr img = new vil_image_view<vil_rgba<unsigned char> >(ni, nj, 1);
    bvxm_slab_to_image::slab_to_image(*grid_it, img);
    vil_save(*img.ptr(), filename.str().c_str());
  }
  return true;
}

template<>
vcl_string bvxm_extension<float>() { return ".tiff"; }

template<>
vcl_string bvxm_extension<unsigned char>() { return ".tiff"; }

