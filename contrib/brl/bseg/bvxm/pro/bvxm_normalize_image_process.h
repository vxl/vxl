// This is brl/bseg/bvxm/pro/bvxm_normalize_image_process.h
#ifndef bvxm_normalize_image_process_h_
#define bvxm_normalize_image_process_h_
//:
// \file
// \brief A class for contrast normalization of images using a voxel world.
//  CAUTION: Input image is assumed to have type vxl_byte
//
// \author Ozge Can Ozcanli
// \date 02/13/2008
// \verbatim
//
// Modifications 
//   Ozge C Ozcanli - 03/25/08 - fixed a compiler warning as suggested by Daniel Lyddy
//
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

#include <vil/vil_image_view.h>

class bvxm_normalize_image_process : public bprb_process
{
 public:

  bvxm_normalize_image_process();

  //: Copy Constructor (no local data)
  bvxm_normalize_image_process(const bvxm_normalize_image_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~bvxm_normalize_image_process(){};

  //: Clone the process
  virtual bvxm_normalize_image_process* clone() const {return new bvxm_normalize_image_process(*this);}

  vcl_string name(){return "bvxmNormalizeImageProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};

template<class T>
inline bool normalize_image(const vil_image_view<T>& in_view, vil_image_view<T>& out_img, float a, float b, T max_value)
{
  unsigned ni = in_view.ni();
  unsigned nj = in_view.nj();
  unsigned np = in_view.nplanes();

 if (ni != out_img.ni() || nj != out_img.nj() || np != out_img.nplanes())
    return false;

  for (unsigned k=0;k<np;++k)
    for (unsigned j=0;j<nj;++j)
      for (unsigned i=0;i<ni;++i)
      {
        T p = (T)(a*in_view(i,j,k) + b);
  
        // Proposed fix
        T min_value = (T) 0;
        out_img(i, j, k) = vcl_min(vcl_max(min_value, p), max_value);
      }

  return true;
}

#endif // bvxm_normalize_image_process_h_
