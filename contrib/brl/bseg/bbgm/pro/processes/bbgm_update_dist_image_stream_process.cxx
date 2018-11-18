// This is brl/bseg/bbgm/pro/processes/bbgm_update_dist_image_stream_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bbgm/bbgm_image_of.h>
#include <bbgm/bbgm_image_sptr.h>
#include <bbgm/bbgm_update.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gauss_if3.h>
#include <bsta/bsta_mixture.h>
#include <bsta/algo/bsta_adaptive_updater.h>
#include <brdb/brdb_value.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vil/vil_math.h>
#include <vil/vil_convert.h>
#include <vidl/vidl_istream_sptr.h>
#include <vidl/vidl_frame.h>
#include <vidl/vidl_convert.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

namespace {
  vidl_istream_sptr istr = nullptr;
  unsigned ni = 0;
  unsigned nj = 0;
};

bool bbgm_update_dist_image_stream_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> in_types(9), out_types(1);
  in_types[0]= "bbgm_image_sptr";//pointer to initial distribution image (typically null)
  in_types[1]= "vidl_istream_sptr";//the video stream
  in_types[2]= "int"; //max_components
  in_types[3]= "int"; //window size
  in_types[4]= "float"; //initial_variance
  in_types[5]= "float"; //g_thresh
  in_types[6]= "float"; //min_stdev
  in_types[7]= "int"; // start frame number
  in_types[8]= "int"; // end frame number
  pro.set_input_types(in_types);

  out_types[0]="bbgm_image_sptr";// the updated distribution image
  pro.set_output_types(out_types);
  return true;
}

bool bbgm_update_dist_image_stream_process_init(bprb_func_process& pro)
{
  //extract the stream
  istr = pro.get_input<vidl_istream_sptr>(1);
  if (!(istr && istr->is_open())) {
    std::cerr << "In bbgm_update_dist_image_stream_process::init() -"
             << " invalid input stream\n";
    return false;
  }
  if (istr->is_seekable())
    istr->seek_frame(0);
  vidl_frame_sptr f = istr->current_frame();
  if (!f) {
    std::cerr << "In bbgm_update_dist_image_stream_process::init() -"
             << " invalid initial frame\n";
    return false;
  }
  ni = f->ni(); nj = f->nj();
  std::cout << " initialized, stream frame size: " << ni << ", " << nj << ", stream at frame # " << istr->frame_number() << std::endl;
  std::cout.flush();

  pro.set_input(0, new brdb_value_t<bbgm_image_sptr>(nullptr));

  return true;
}

//: Execute the process function
bool bbgm_update_dist_image_stream_process(bprb_func_process& pro)
{
  // Sanity check
  if (!pro.verify_inputs()) {
    std::cerr << "In bbgm_update_dist_image_stream_process::execute() -"
             << " invalid inputs\n";
    return false;
  }

  // Retrieve background image
  bbgm_image_sptr bgm = pro.get_input<bbgm_image_sptr>(0);

  //Retrieve max components
  int max_components = pro.get_input<int>(2);

  //Retrieve window_size
  int window_size = pro.get_input<int>(3);

  //Retrieve initial_variance
  auto initial_variance = pro.get_input<float>(4);

  //Retrieve g_thresh
  auto g_thresh = pro.get_input<float>(5);

  //Retrieve min_stdev
  auto min_stdev = pro.get_input<float>(6);

  //Retrieve start frame number
  int start_frame = pro.get_input<int>(7);

  //Retrieve end frame number
  int end_frame = pro.get_input<int>(8);

  std::cout << " will start at frame # " << start_frame << " will end at frame # " << end_frame << std::endl;

  typedef bsta_gauss_if3 bsta_gauss_t;
  typedef bsta_gauss_t::vector_type vector_;
  typedef bsta_num_obs<bsta_gauss_if3> gauss_type;
  typedef bsta_mixture<gauss_type> mix_gauss_type;
  typedef bsta_num_obs<mix_gauss_type> obs_mix_gauss_type;

  // get the templated mixture model
  bbgm_image_sptr model_sptr;
  if (!bgm) {
    model_sptr = new bbgm_image_of<obs_mix_gauss_type>(ni, nj, obs_mix_gauss_type());
    std::cout << " Initialized the bbgm image\n";
    std::cout.flush();
  }
  else model_sptr = bgm;
  auto *model =
    static_cast<bbgm_image_of<obs_mix_gauss_type>*>(model_sptr.ptr());

  bsta_gauss_t init_gauss(vector_(0.0f), vector_(initial_variance) );

#if 0
  bsta_mg_window_updater<mix_gauss_type> updater( init_gauss,
                                                  max_components);
#endif
  bsta_mg_grimson_window_updater<mix_gauss_type> updater( init_gauss,
                                                          max_components,
                                                          g_thresh,
                                                          min_stdev,
                                                          window_size);


  while (istr->advance() && (end_frame<0||(int)(istr->frame_number()) <= end_frame)) {
    // get frame from stream
    if ((int)(istr->frame_number()) >= start_frame) {
      vidl_frame_sptr f = istr->current_frame();
      vil_image_view_base_sptr fb = vidl_convert_wrap_in_view(*f);
      if (!fb)
        return false;
      vil_image_view<float> frame = *vil_convert_cast(float(), fb);
      if (fb->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
        vil_math_scale_values(frame,1.0/255.0);

      update(*model,frame,updater);
      std::cout << "updated frame # "<< istr->frame_number()
               << " format " << fb->pixel_format() << " nplanes "
               << fb->nplanes()<< '\n';
      std::cout.flush();
    }
  }

  brdb_value_sptr output = new brdb_value_t<bbgm_image_sptr>(model_sptr);
  pro.set_output(0, output);
  return true;
}
