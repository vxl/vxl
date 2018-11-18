#include "bvxm_slab_to_image.h"

template<>
bool bvxm_slab_to_image::slab_to_image(bvxm_voxel_slab<vnl_float_3> const& slab, vil_image_view_base_sptr image)
{
  // check image is preallocated to correct size
  if ( (slab.nx() != image->ni()) || (slab.ny() != image->nj()) )
  {
    std::cerr << "error: slab and image are different sizes.\n";
    return false;
  }

  // take care of pixel format issues. might want to specialize this function for rgb, etc
  if (image->pixel_format()== VIL_PIXEL_FORMAT_RGB_BYTE)
  {
      if (auto *img_view = dynamic_cast<vil_image_view<vil_rgb<unsigned char> >*>(image.ptr()))
      {
          vil_image_view<vil_rgb<unsigned char> >::iterator img_it = img_view->begin();
          bvxm_voxel_slab<vnl_float_3>::const_iterator slab_it = slab.begin();
          for (; img_it != img_view->end(); ++img_it, ++slab_it) {
              *img_it =  vil_rgb<unsigned char>((unsigned char)((*slab_it)[0]),
                                                (unsigned char)((*slab_it)[1]),
                                                (unsigned char)((*slab_it)[2]));
          }
      }
  }
  else
  {
      std::cerr << "error: failed to cast image_view_base to image_view\n";
      return false;
  }

  return true;
}

template<>
bool bvxm_slab_to_image::slab_to_image(bvxm_voxel_slab<vnl_float_4> const& slab, vil_image_view_base_sptr image)
{
    // check image is preallocated to correct size
    if ( (slab.nx() != image->ni()) || (slab.ny() != image->nj()) )
    {
        std::cerr << "error: slab and image are different sizes.\n";
        return false;
    }
    // take care of pixel format issues. might want to specialize this function for rgb, etc
    if (image->pixel_format()== VIL_PIXEL_FORMAT_RGBA_BYTE)
    {
        if (auto *img_view = dynamic_cast<vil_image_view<vil_rgba<unsigned char> >*>(image.ptr()))
        {
            vil_image_view<vil_rgba<unsigned char> >::iterator img_it = img_view->begin();
            bvxm_voxel_slab<vnl_float_4>::const_iterator slab_it = slab.begin();
            for (; img_it != img_view->end(); ++img_it, ++slab_it) {
                *img_it =  vil_rgba<unsigned char>((unsigned char)((*slab_it)[0]),
                                                   (unsigned char)((*slab_it)[1]),
                                                   (unsigned char)((*slab_it)[2]),
                                                   (unsigned char)((*slab_it)[3]));
            }
        }
    }
    else
    {
        std::cerr << "error: failed to cast image_view_base to image_view\n";
        return false;
    }

    return true;
}
