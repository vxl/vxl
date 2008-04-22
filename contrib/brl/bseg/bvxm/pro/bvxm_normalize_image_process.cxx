#include "bvxm_normalize_image_process.h"

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_convert.h>
#include <brip/brip_vil_float_ops.h>
#include <vpgl/vpgl_camera.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_mog_grey_processor.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_voxel_traits.h>
#include <bvxm/bvxm_util.h>


bvxm_normalize_image_process::bvxm_normalize_image_process()
{
  //inputs
  input_data_.resize(5,brdb_value_sptr(0));
  input_types_.resize(5);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "bvxm_voxel_world_sptr";
  input_types_[3] = "vcl_string";
  input_types_[4] = "unsigned";

  //output
  output_data_.resize(3,brdb_value_sptr(0));
  output_types_.resize(3);
  output_types_[0]= "vil_image_view_base_sptr";
  output_types_[1]= "float";  // output a
  output_types_[2]= "float";  // output b

  parameters()->add("use most probable mode to create mog", "most_prob", true);   // otherwise uses expected values of the mixtures at voxels along the path of the rays 
  parameters()->add("start value for a", "a_start", 0.6f);
  parameters()->add("increment for a", "a_inc", 0.05f);
  parameters()->add("end value for a", "a_end", 1.8f);
  // The following defaults are carried from Thom's code.
  // However here we always work with images scaled to [0,1] interval.
  // Thom had assumed byte images, so the defaults are divided by 255 for b.
  parameters()->add("start value for intervals of b", "b_start", (100.0f/255.0f));  // start search in [-200, 200] with 100 increments for a byte image
  parameters()->add("end value for intervals of b", "b_end", (5.0f/255.0f));        // end search in [-10, 10] with 5 increments
  parameters()->add("ratio that determines increment value for intervals of b", "b_ratio", 0.5f);           // half the intervals, e.g. if start with 100, then 50, 25, 12, etc.
  parameters()->add("verbose", "verbose", false);
}

bool bvxm_normalize_image_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  //get the inputs
  brdb_value_t<vil_image_view_base_sptr>* input0 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr());

  vil_image_view_base_sptr input_img = input0->value();

  brdb_value_t<vpgl_camera_double_sptr>* input1 =
    static_cast<brdb_value_t<vpgl_camera_double_sptr>* >(input_data_[1].ptr());

  vpgl_camera_double_sptr camera = input1->value();

  brdb_value_t<bvxm_voxel_world_sptr>* input2 =
    static_cast<brdb_value_t<bvxm_voxel_world_sptr>* >(input_data_[2].ptr());

  bvxm_voxel_world_sptr world = input2->value();

   brdb_value_t<vcl_string>* input3 =
    static_cast<brdb_value_t<vcl_string>* >(input_data_[3].ptr());

  vcl_string voxel_type = input3->value();

  brdb_value_t<unsigned>* input4 =
    static_cast<brdb_value_t<unsigned>* >(input_data_[4].ptr());
  unsigned bin_index = input4->value();

  // if the world is not updated yet, we just return the input image
  if ((voxel_type == "apm_mog_rgb") ){
    if (!world->num_observations<APM_MOG_RGB>(bin_index) ) {
      // return the input img
      brdb_value_sptr output0 = new brdb_value_t<vil_image_view_base_sptr>(input_img);
      output_data_[0] = output0;

      brdb_value_sptr output1 = new brdb_value_t<float>(1.0f); // a
      brdb_value_sptr output2 = new brdb_value_t<float>(0.0f); // b
      output_data_[1] = output1;
      output_data_[2] = output2;

      return true;
    }
  }
  if ((voxel_type == "apm_mog_grey") )
  {
    if (!world->num_observations<APM_MOG_GREY>(bin_index))
    {
      // return the input img
      brdb_value_sptr output0 = new brdb_value_t<vil_image_view_base_sptr>(input_img);
      output_data_[0] = output0;

      brdb_value_sptr output1 = new brdb_value_t<float>(1.0f); // a
      brdb_value_sptr output2 = new brdb_value_t<float>(0.0f); // b
      output_data_[1] = output1;
      output_data_[2] = output2;

      return true;
    }
  }
  // create metadata:
  bvxm_image_metadata observation(input_img,camera);

  // get parameters
  bool most_prob = true;
  parameters()->get_value("most_prob", most_prob);   // otherwise uses expected image

  bool verbose = false;
  parameters()->get_value("verbose", verbose);
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

  //vil_image_view_base_sptr input_image_sptr = new vil_image_view<vxl_byte>(input_img);
  //vil_image_view<float> input_img_float = vil_convert_cast( float(), input_image_sptr );
  unsigned ni = input_img->ni();
  unsigned nj = input_img->nj();
  unsigned nplanes = input_img->nplanes();

  vil_image_view<float>* input_img_float_stretched = new vil_image_view<float>( ni, nj, nplanes );
  vil_image_view<vxl_byte>* input_img_ptr = new vil_image_view<vxl_byte>(input_img);
  vil_convert_stretch_range_limited<vxl_byte>(*input_img_ptr, *input_img_float_stretched, 0, 255, 0.0f, 1.0f);

  // use the weight slab below to calculate total probability
  bvxm_voxel_slab<float> weights(ni, nj, 1);
  weights.fill(1.0f/(ni * nj));

  //1) get probability mixtures of all pixels in image
  bvxm_voxel_slab_base_sptr mog_image;

  float a = 1.0f; float b = 0.0f;
  float best_prob = 0.0;

  if (voxel_type == "apm_mog_rgb") {
    typedef bvxm_voxel_traits<APM_MOG_RGB>::voxel_datatype mog_type;
    typedef bvxm_voxel_traits<APM_MOG_RGB>::obs_datatype obs_datatype;

    if (most_prob) {
      world->mog_most_probable_image<APM_MOG_RGB>(observation, mog_image, bin_index); 
    } else {
      world->mixture_of_gaussians_image<APM_MOG_RGB>(observation, mog_image, bin_index);
    }

    bvxm_voxel_slab<mog_type>* mog_image_ptr = dynamic_cast<bvxm_voxel_slab<mog_type>*>(mog_image.ptr());

    bvxm_voxel_traits<APM_MOG_RGB>::appearance_processor apm_processor;
    if (verbose) {
      bvxm_voxel_slab<obs_datatype> exp_img = apm_processor.expected_color(*mog_image_ptr);
      vil_image_view_base_sptr temp_img = new vil_image_view<vxl_byte>(ni, nj, 3);
      bvxm_util::slab_to_img(exp_img, temp_img);
      vil_save(*temp_img, "./mixture_expected_img.png");
    }

    //2) optimize two parameters for the input image so that it is the maximally probable image seen wrt mog_image
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
          //vil_image_view<float> nimg( ni, nj, nplanes );
          //normalize_image<float>(input_img_float, nimg, sa, sb, 255.0);
          vil_image_view<float>* nimg = new vil_image_view<float>( ni, nj, nplanes );
          normalize_image(*input_img_float_stretched, *nimg, sa, sb, 1.0f);
          vil_image_view_base_sptr nimg_sptr = nimg;

          //vil_image_view<float>* nimg_stretched_ptr = new vil_image_view<float>( ni, nj, nplanes );
          //vil_image_view_base_sptr nimg_stretched_sptr = nimg_stretched_ptr;
          //vil_convert_stretch_range_limited(nimg, *nimg_stretched_ptr, 0.0f, 255.0f, 0.0f, 1.0f);

          // convert image to a voxel_slab
          bvxm_voxel_slab<obs_datatype> image_slab(ni, nj, 1);
          //bvxm_util::img_to_slab(nimg_stretched_ptr,image_slab);
          //nimg_stretched_sptr = 0;  // to clear up the space taken by new
          bvxm_util::img_to_slab(nimg_sptr,image_slab);
          nimg_sptr = 0;  // to clear up space

          bvxm_voxel_slab<float> prob = apm_processor.prob_density(*mog_image_ptr,image_slab); //prob( nimg );
          // find the total prob
          bvxm_voxel_slab<float> product(ni, nj, 1);
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
  }
  else if (voxel_type == "apm_mog_grey") {
    typedef bvxm_voxel_traits<APM_MOG_GREY>::voxel_datatype mog_type;
    typedef bvxm_voxel_traits<APM_MOG_GREY>::obs_datatype obs_datatype;

    if (most_prob) {
      world->mog_most_probable_image<APM_MOG_GREY>(observation, mog_image, bin_index); 
    } else {
      world->mixture_of_gaussians_image<APM_MOG_GREY>(observation, mog_image, bin_index);
    }
    
    bvxm_voxel_slab<mog_type>* mog_image_ptr = dynamic_cast<bvxm_voxel_slab<mog_type>*>(mog_image.ptr());

    bvxm_voxel_traits<APM_MOG_GREY>::appearance_processor apm_processor;
    if (verbose) {
      bvxm_voxel_slab<float> exp_img = apm_processor.expected_color(*mog_image_ptr);
      vil_image_view_base_sptr temp_img = new vil_image_view<vxl_byte>(ni, nj, 1);
      bvxm_util::slab_to_img(exp_img, temp_img);
      vil_save(*temp_img, "./mixture_expected_img.png");
    }

    //2) optimize two parameters for the input image so that it is the maximally probable image seen wrt mog_image
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
          //vil_image_view<float> nimg( ni, nj, nplanes );
          //normalize_image<float>(input_img_float, nimg, sa, sb, 255.0);
          vil_image_view<float>* nimg = new vil_image_view<float>(ni, nj, nplanes);
          normalize_image(*input_img_float_stretched, *nimg, sa, sb, 1.0f);
          vil_image_view_base_sptr nimg_sptr = nimg;

          //vil_image_view<float>* nimg_stretched_ptr = new vil_image_view<float>( ni, nj, nplanes );
          //vil_image_view_base_sptr nimg_stretched_sptr = nimg_stretched_ptr;
          //vil_convert_stretch_range_limited(nimg, *nimg_stretched_ptr, 0.0f, 255.0f, 0.0f, 1.0f);

          // convert image to a voxel_slab
          bvxm_voxel_slab<obs_datatype> image_slab(ni, nj, 1);
          //bvxm_util::img_to_slab(nimg_stretched_ptr,image_slab);
          //nimg_stretched_sptr = 0;  // to clear up the space taken by new
          bvxm_util::img_to_slab(nimg_sptr,image_slab);
          nimg_sptr = 0;  // to clear up space

          bvxm_voxel_slab<float> prob = apm_processor.prob_density(*mog_image_ptr,image_slab); //prob( nimg );
          // find the total prob
          bvxm_voxel_slab<float> product(ni, nj, 1);
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
  }
  else {
    vcl_cout << "In bvxm_normalize_image_process::execute() -- input appearance model: " << voxel_type << " is not supported\n";
    return false;
  }

  if (verbose) {
    vcl_ofstream file;
    file.open("./normalization_parameters.txt", vcl_ofstream::app);
    file << a << ' ' << b <<'\n';
  }

 

  // Normalize the image with the best a and b.
  //vil_image_view<vxl_byte> output_img(ni, nj, nplanes);
  //normalize_image<vxl_byte>(*input_image_sptr, output_img, a, b, 255);

  vil_image_view<float> output_img_float(ni, nj, nplanes);
  normalize_image(*input_img_float_stretched, output_img_float, a, b, 1.0f);

  vil_image_view<float> output_img_stretched(ni, nj, nplanes);
  vil_convert_stretch_range_limited<float>(output_img_float, output_img_stretched, 0.0f, 1.0f, 0.0f, 255.0f);
  vil_image_view<vxl_byte> output_img;
  vil_convert_cast(output_img_stretched, output_img);

  // return the output img
  brdb_value_sptr output0 =
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<vxl_byte>(output_img));
  output_data_[0] = output0;

  brdb_value_sptr output1 = new brdb_value_t<float>(a);
  brdb_value_sptr output2 = new brdb_value_t<float>(b*255.0f); // switch back to byte
  output_data_[1] = output1;
  output_data_[2] = output2;

  return true;
}

