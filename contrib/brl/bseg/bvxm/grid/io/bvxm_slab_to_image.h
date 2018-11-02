// This is brl/bseg/bvxm/grid/io/bvxm_slab_to_image.h
#ifndef bvxm_io_voxel_slab_h
#define bvxm_io_voxel_slab_h
//:
// \file
// \brief Converts slab to a vil_image
// \author Isabel Restrepo mir@lems.brown.edu
// \date  June 26, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vector>
#include <iostream>
#include "../bvxm_voxel_slab.h"
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_float_3.h>
#include <vnl/vnl_float_4.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_pixel_format.h>
#include <vil/vil_save.h> // for debug saving
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class bvxm_slab_to_image
{
 public:
  //:Converts slab to a vil_image. Datatype is a vnl_vector_fixed
  template<class T, unsigned N>
  static bool slab_to_image(bvxm_voxel_slab<vnl_vector_fixed<T,N> > const &slab, vil_image_view_base_sptr image);

  //Converts slab to a vil_image. Datatype is a scalar
  template<class T>
  static bool slab_to_image(bvxm_voxel_slab<T> const& slab, vil_image_view_base_sptr image);

  //: Simple function to save a slab into an image. Used for debugging where the type of output image is not so important
  template<class T>
  static void write_slab_as_image(bvxm_voxel_slab<T> const& slab_in,std::string filename);

  //: Simple function to save a slab into an image. Used for debugging where the type of output image is not so important
  template<class T, unsigned N>
  static void write_slab_as_image(bvxm_voxel_slab<vnl_vector_fixed<T,N> > const& slab_in,std::string filename);
};


//:Saves slab to a vil_image. Datatype is a vnl_vector_fixed
template<class T, unsigned N>
bool bvxm_slab_to_image::slab_to_image(bvxm_voxel_slab<vnl_vector_fixed<T,N> > const &slab, vil_image_view_base_sptr image)
{
  // check image is preallocated to correct size
  if ( (slab.nx() != image->ni()) || (slab.ny() != image->nj()) )
  {
    std::cerr << "error: slab and image are different sizes.\n";
    return false;
  }

  // take care of pixel format issues. might want to specialize this function for rgb, etc
  switch (image->pixel_format())
  {
    case VIL_PIXEL_FORMAT_BYTE:
      if (image->nplanes() ==N)
      {
        if (vil_image_view<unsigned char> *img_view = dynamic_cast<vil_image_view<unsigned char>*>(image.ptr()))
        {
          std::vector<vil_image_view<unsigned char>::iterator> img_its;
          for (unsigned p=0; p<N; ++p)
          {
            vil_image_view<unsigned char>::iterator plane_it = img_view->begin() + (p*img_view->planestep());
            img_its.push_back(plane_it);
          }

          typename bvxm_voxel_slab<vnl_vector_fixed<T,N> >::const_iterator slab_it = slab.begin();
          for (; slab_it != slab.end(); ++slab_it)
          {
            for (unsigned p=0; p<N; ++p)
            {
              *(img_its[p]) =  (unsigned char)(((*slab_it)[p] * 127.0) + 0.5) + 127;
              ++(img_its[p]);
            }
          }
        }
        else
        {
          std::cerr << "error in slab_to_image: failed to cast image_view_base to image_view\n";
          return false;
        }
      }
      else
      {
        std::cerr << "error in slab_to_image: incorrect number of image planes\n";
        return false;
      }
      break;

    case VIL_PIXEL_FORMAT_RGBA_BYTE:
      if (image->nplanes() ==1)
      {
        if (vil_image_view<vil_rgba<unsigned char> > *img_view = dynamic_cast<vil_image_view<vil_rgba<unsigned char> >*>(image.ptr()))
        {
          vil_image_view<vil_rgba<unsigned char> >::iterator img_it = img_view->begin();
#if 0
          for (unsigned p=0; p<N; ++p)
          {
            vil_image_view<vil_rgba<unsigned char> >::iterator plane_it = img_view->begin();
            img_its.push_back(plane_it);
          }
#endif
          typename bvxm_voxel_slab<vnl_vector_fixed<T,N> >::const_iterator slab_it = slab.begin();
          if (N != 4)
          {
            std::cerr << "error in slab_to_image: failed to cast image_view_base to image_view: N should be 4\n";
            return false;
          }
          for (; slab_it != slab.end(); ++slab_it)
          {
            (*img_it) = vil_rgba<unsigned char>((unsigned char)(*slab_it)[0],
                                                (unsigned char)(*slab_it)[1],
                                                (unsigned char)(*slab_it)[2],
                                                (unsigned char)std::floor((*slab_it)[3]));
#if 0
            *(img_its[p]) = (unsigned char)(((*slab_it)[p] * 127.0) + 0.5) + 127;
            std::cout<<(int)(*img_it).R()<<' '<<(int)((unsigned char)(*slab_it)[3])<<std::endl;
#endif
            ++(img_it);
          }
        }
        else
        {
          std::cerr << "error in slab_to_image: failed to cast image_view_base to image_view\n";
          return false;
        }
      }
      else
      {
        std::cerr << "error in slab_to_image: incorrect number of image planes\n";
        return false;
      }
      break;

    case VIL_PIXEL_FORMAT_FLOAT:
      if (image->nplanes() ==N)
      {
          if (vil_image_view<float> *img_view = dynamic_cast<vil_image_view<float>*>(image.ptr()))
          {
              std::vector<vil_image_view<float>::iterator> img_its;
              for (unsigned p=0; p<N; ++p)
              {
                  vil_image_view<float>::iterator plane_it = img_view->begin() + (p*img_view->planestep());
                  img_its.push_back(plane_it);
              }

              typename bvxm_voxel_slab<vnl_vector_fixed<T,N> >::const_iterator slab_it = slab.begin();
              for (; slab_it != slab.end(); ++slab_it)
              {
                  for (unsigned p=0; p<N; ++p)
                  {
                      *(img_its[p]) = (float)((*slab_it)[p]);
                      ++(img_its[p]);
                  }
              }
          }
        else
        {
          std::cerr << "error in slab_to_image: failed to cast image_view_base to image_view\n";
          return false;
        }
      }
      else
      {
        std::cerr << "error in slab_to_image: incorrect number of image planes\n";
        return false;
      }
      break;

    case VIL_PIXEL_FORMAT_RGB_BYTE:
      if (image->nplanes() ==1)
      {
        if (vil_image_view<vil_rgb<unsigned char> > *img_view = dynamic_cast<vil_image_view<vil_rgb<unsigned char> >*>(image.ptr()))
        {
          vil_image_view<vil_rgb<unsigned char> >::iterator img_it = img_view->begin();

          typename bvxm_voxel_slab<vnl_vector_fixed<T,N> >::const_iterator slab_it = slab.begin();
          if (N != 3)
          {
            std::cerr << "error in slab_to_image: failed to cast image_view_base to image_view: N should be 3\n";
            return false;
          }
          for (; slab_it != slab.end(); ++slab_it)
          {
            (*img_it) = vil_rgb<unsigned char>((unsigned char)((*slab_it)[0]*127+127),
                                                (unsigned char)((*slab_it)[1]*127+127),
                                                (unsigned char)((*slab_it)[2]*127+127));
            ++(img_it);
          }
        }
        else
        {
          std::cerr << "error in slab_to_image: failed to cast image_view_base to image_view\n";
          return false;
        }
      }
      else
      {
        std::cerr << "error in slab_to_image: incorrect number of image planes\n";
        return false;
      }
      break;


    default:
      std::cerr << "img_to_slab: unsupported pixel type\n";
      return false;
      break;
  }

  return true;
}

template<>
bool bvxm_slab_to_image::slab_to_image(bvxm_voxel_slab<vnl_float_3> const& slab, vil_image_view_base_sptr image);

template<>
bool bvxm_slab_to_image::slab_to_image(bvxm_voxel_slab<vnl_float_4> const& slab, vil_image_view_base_sptr image);

template<class T>
bool bvxm_slab_to_image::slab_to_image(bvxm_voxel_slab<T> const& slab, vil_image_view_base_sptr image)
{
  // check image is preallocated to correct size
  if ( (slab.nx() != image->ni()) || (slab.ny() != image->nj()) )
  {
    std::cerr << "error: slab and image are different sizes.\n";
    return false;
  }

  // take care of pixel format issues. might want to specialize this function for rgb, etc
  switch (image->pixel_format())
  {
    case VIL_PIXEL_FORMAT_BYTE:
      if (vil_image_view<unsigned char> *img_view = dynamic_cast<vil_image_view<unsigned char>*>(image.ptr()))
      {
        vil_image_view<unsigned char>::iterator img_it = img_view->begin();
        typename bvxm_voxel_slab<T>::const_iterator slab_it = slab.begin();
        for (; img_it != img_view->end(); ++img_it, ++slab_it) {
          *img_it =  (unsigned char)std::floor(((*slab_it)*255));
        }
      }
      else
      {
        std::cerr << "error: failed to cast image_view_base to image_view\n";
        return false;
      }
      break;

    case VIL_PIXEL_FORMAT_FLOAT:
      if (vil_image_view<float> *img_view = dynamic_cast<vil_image_view<float>*>(image.ptr()))
      {
        vil_image_view<float>::iterator img_it = img_view->begin();
        typename bvxm_voxel_slab<T>::const_iterator slab_it = slab.begin();
        for (; img_it != img_view->end(); ++img_it, ++slab_it) {
          *img_it = (float)(*slab_it*255);
        }
      }
      else
      {
        std::cerr << "error: failed to cast image_view_base to image_view\n";
        return false;
      }
      break;

    default:
      std::cerr << "img_to_slab: unsupported pixel type\n";
      return false;
      break;
  }

  return true;
}

// used for debugging
template<class T, unsigned N>
void bvxm_slab_to_image::write_slab_as_image(bvxm_voxel_slab<vnl_vector_fixed<T,N> > const& slab_in,std::string filename)
{
  vil_image_view<T> img(slab_in.nx(),slab_in.ny(),N);
  std::vector<typename vil_image_view<T>::iterator> img_its;
  for (unsigned p=0; p < N; ++p) {
    typename vil_image_view<T>::iterator plane_it = img.begin() + (p*img.planestep());
    img_its.push_back(plane_it);
  }
  typename bvxm_voxel_slab<vnl_vector_fixed<T,N> >::const_iterator slab_it = slab_in.begin();
  for (; slab_it != slab_in.end(); ++slab_it) {
    for (unsigned p=0; p<N; ++p) {
      *(img_its[p]) = (*slab_it)[p];
      ++(img_its[p]);
    }
  }
  vil_save(img,filename.c_str());

  return;
}

// used for debugging
template<class T>
void bvxm_slab_to_image::write_slab_as_image(bvxm_voxel_slab<T> const& slab_in,std::string filename)
{
  vil_image_view<T> img(slab_in.nx(),slab_in.ny(),1);
  typename vil_image_view<T>::iterator img_it = img.begin();
  typename bvxm_voxel_slab<T>::const_iterator slab_it = slab_in.begin();
  for (; img_it != img.end(); ++img_it, ++slab_it) {
    *img_it = *slab_it;
  }
  vil_save(img,filename.c_str());

  return;
}


#endif
