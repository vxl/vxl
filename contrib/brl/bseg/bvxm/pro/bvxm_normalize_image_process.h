// This is contrib/brl/bseg/bvxm/pro/bvxm_normalize_image_process.h
#ifndef bvxm_normalize_image_process_h_
#define bvxm_normalize_image_process_h_

//:
// \file
// \brief // A class for contrast normalization of images using a voxel world . 
//           CAUTION: 1) Input image is assumed to have type vxl_byte
//                    2) Even if the image is RGB, it is converted to grey and the best gain and offset is calculated from the grey image
//           
// \author Ozge Can Ozcanli
// \date 02/13/08
// \verbatim
//
// \Modifications 

#include <vcl_string.h>
#include <bprb/bprb_process.h>

#include <vil/vil_image_view.h>
#include <vcl_cmath.h>

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
 
 private:

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
        //T p = (T)vcl_floor(a*in_view(i,j,k) + b);
        T p = (T)(a*in_view(i,j,k) + b);
        if (p < 0) 
          out_img(i,j,k) = 0;
        else if ( p > max_value ) 
          out_img(i,j,k) = max_value;
        else 
          out_img(i,j,k) = p;
      }

  return true;
}


#endif // bvma_update_process_h_

