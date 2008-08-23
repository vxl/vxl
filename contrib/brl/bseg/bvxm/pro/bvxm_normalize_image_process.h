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
//  Modifications
//   Ozge C Ozcanli - 03/25/08 - fixed a compiler warning as suggested by Daniel Lyddy
//   Isabel Restrepo- 08/22/08 - Moved most of calculations to template function
//                    norm_parameters<bvxm_voxel_type APM_T>.
//                  - Added support for multichannel appereance model processor,
//                  - Removed support for rgb_mog_processor
// \endverbatim

#include <vcl_string.h>
#include <vcl_algorithm.h>
#include <vcl_cmath.h>
#ifdef DEBUG
#include <vcl_iostream.h>
#endif
#include <bprb/bprb_process.h>
#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view.h>
#include <bvxm/bvxm_voxel_world.h>

class bvxm_normalize_image_process : public bprb_process
{
public:

  bvxm_normalize_image_process();

  //: Copy Constructor (no local data)
  bvxm_normalize_image_process(const bvxm_normalize_image_process& other): bprb_process(*static_cast<const bprb_process*>(&other)) {}

  ~bvxm_normalize_image_process() {}

  //: Clone the process
  virtual bvxm_normalize_image_process* clone() const { return new bvxm_normalize_image_process(*this); }

  vcl_string name() { return "bvxmNormalizeImageProcess"; }

  bool init() { return true; }
  bool execute();
  bool finish() { return true; }

protected:

  //:This local function calculates and retrieves optimal normalization parameters
  template <bvxm_voxel_type APM_T> 
  bool norm_parameters(vil_image_view_base_sptr const &input_img,
    vil_image_view<float>*  &input_img_float_streched,
    vpgl_camera_double_sptr const &camera,
    bvxm_voxel_world_sptr const &world,
    unsigned const bin_index,
    unsigned const scale_index,
    bool verbose,
    float &a, float &b);
  unsigned ni_;
  unsigned nj_;
  unsigned nplanes_;

};

template <bvxm_voxel_type APM_T>
bool bvxm_normalize_image_process::norm_parameters(vil_image_view_base_sptr const &input_img,
                                                   vil_image_view<float>* &input_img_float_stretched,
                                                   vpgl_camera_double_sptr const &camera,
                                                   bvxm_voxel_world_sptr const &world,
                                                   unsigned const bin_index,
                                                   unsigned const scale_index,
                                                   bool verbose,
                                                   float &a, float &b)
{
  //1)Set up the data

  // create metadata:
  bvxm_image_metadata observation(input_img,camera);

  // get parameters
  bool most_prob = true;
  parameters()->get_value("most_prob", most_prob);   // otherwise uses expected image

  float a_start=0, a_end=0, a_inc=0;
  parameters()->get_value("a_start", a_start);
  parameters()->get_value("a_inc", a_inc);
  parameters()->get_value("a_end", a_end);

  float b_start=0, b_end=0, b_ratio=0;
  parameters()->get_value("b_start", b_start);
  parameters()->get_value("b_ratio", b_ratio);
  parameters()->get_value("b_end", b_end);

  if (verbose) {
    if (most_prob) 
      vcl_cout << "using most probable modes' colors to create mog image ";
    else 
      vcl_cout << "using expected colors to create mog image ";

    vcl_cout << "normalization parameters to be used in this run:\n"
      << "a_start: " << a_start << " a_end: " << a_end << " a_inc: " << a_inc << vcl_endl
      << "b_start: " << b_start << " b_end: " << b_end << " b_ratio: " << b_ratio << vcl_endl;
  }

  // CAUTION: Assumption: Input image is of type vxl_byte
  if (input_img->pixel_format() != VIL_PIXEL_FORMAT_BYTE) {
    vcl_cout << "Input image pixel format is not VIL_PIXEL_FORMAT_BYTE!\n";
    return false;
  }

  vil_image_view<vxl_byte>* input_img_ptr = new vil_image_view<vxl_byte>(input_img);
  vil_convert_stretch_range_limited<vxl_byte>(*input_img_ptr, *input_img_float_stretched, 0, 255, 0.0f, 1.0f);

  // use the weight slab below to calculate total probability
  bvxm_voxel_slab<float> weights(ni_, nj_, 1);
  weights.fill(1.0f/(ni_ * nj_));

  //2) get probability mixtures of all pixels in image
  bvxm_voxel_slab_base_sptr mog_image;

  a = 1.0f; 
  b = 0.0f;
  float best_prob = 0.0;

  typedef typename bvxm_voxel_traits<APM_T>::voxel_datatype mog_type;
  typedef typename bvxm_voxel_traits<APM_T>::obs_datatype obs_datatype;

  if (most_prob) {
    world->mog_most_probable_image<APM_T>(observation, mog_image, bin_index,scale_index); 
  } else {
    world->mixture_of_gaussians_image<APM_T>(observation, mog_image, bin_index,scale_index);
  }

  bvxm_voxel_slab<mog_type>* mog_image_ptr = dynamic_cast<bvxm_voxel_slab<mog_type>* >(mog_image.ptr());

  typename bvxm_voxel_traits<APM_T>::appearance_processor apm_processor;
  if (verbose) {
    bvxm_voxel_slab<obs_datatype> exp_img = apm_processor.expected_color(*mog_image_ptr);
    vil_image_view_base_sptr temp_img = new vil_image_view<vxl_byte>(ni_, nj_, nplanes_);
    bvxm_util::slab_to_img(exp_img, temp_img);
    vil_save(*temp_img, "./mixture_expected_img.png");
  }


  //3) optimize two parameters for the input image so that it is the maximally probable image seen wrt mog_image
  for ( float sa = a_start; sa <= a_end; sa+=a_inc ){
    float sb_best = 0.0f;
    bool tried_zero = false;
    for ( float sb_inc = b_start; sb_inc > b_end; sb_inc *= b_ratio ) {
      float sb_best_prob = 0.0;
      for ( float sb = sb_best-2*sb_inc; sb <= sb_best+2.01*sb_inc; sb+= sb_inc ) {
        if (sb == 0.0f) {
          if (tried_zero)
            continue;
          else
            tried_zero = true;
        }

        vil_image_view<float>* nimg = new vil_image_view<float>( ni_, nj_, nplanes_ );
        normalize_image(*input_img_float_stretched, *nimg, sa, sb, 1.0f);
        vil_image_view_base_sptr nimg_sptr = nimg;

        // convert image to a voxel_slab
        bvxm_voxel_slab<obs_datatype> image_slab(ni_, nj_, 1);
        bvxm_util::img_to_slab(nimg_sptr,image_slab);
        nimg_sptr = 0;  // to clear up space

        bvxm_voxel_slab<float> prob = apm_processor.prob_density(*mog_image_ptr,image_slab); //prob( nimg );

        // find the total prob
        bvxm_voxel_slab<float> product(ni_, nj_, 1);
        bvxm_util::multiply_slabs(prob, weights, product);
        float this_prob = bvxm_util::sum_slab(product);

        //vcl_cerr << this_prob << ' ';
        if ( this_prob < 0 ) {
          vcl_cout << "In bvxm_normalize_image_process::execute() -- prob is negative, Exiting!\n";
          return false;
        }

        if ( this_prob > sb_best_prob ){ sb_best_prob = this_prob; sb_best = sb; }
        if ( this_prob > best_prob ){ best_prob = this_prob; a = sa; b = sb; }
      }
      //vcl_cerr << '\n';
    }
    //vcl_cerr << '\n';
  }
  if (verbose) 
  {
    vcl_ofstream file;
    file.open("./normalization_parameters.txt", vcl_ofstream::app);
    file << a << ' ' << b <<'\n';
  }

  return true;
}

//: Float specialized function to normalize and image given a,b where new_I = a*I +b;
inline bool normalize_image(const vil_image_view<float>& in_view, vil_image_view<float>& out_img, float a, float b, float max_value)
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
        float p = (float)(a*in_view(i,j,k) + b);

        // Proposed fix
        float min_value = (float) 0;
        out_img(i, j, k) = vcl_min(vcl_max(min_value, p), max_value);
      }

      return true;
}

//: Byte specialized function to normalize and image given a,b where new_I = a*I +b;
inline bool normalize_image(const vil_image_view<vxl_byte>& in_view, vil_image_view<vxl_byte>& out_img, float a, float b, unsigned char max_value)
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
        int p = (int)vcl_floor(a*in_view(i,j,k) + b);
        if ( !(p >= 0) ) out_img(i, j, k)  = 0;
        else if ( p > 255 ) out_img(i, j, k)  = 255;
        else out_img(i, j, k)  = p;
      }
#ifdef DEBUG
      vcl_cerr << "entered byte case..................\n";
#endif
      return true;
}

#endif // bvxm_normalize_image_process_h_
