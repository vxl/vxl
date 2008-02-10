// This is brl/bseg/bbgm/pro/bbgm_update_dist_image_stream_process.cxx

//:
// \file

#include "bbgm_update_dist_image_stream_process.h"
#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <brip/brip_vil_float_ops.h>
#include <bbgm/bbgm_image_of.h>
#include <bbgm/bbgm_image_sptr.h>
#include <bbgm/bbgm_update.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gauss_if3.h>
#include <bsta/bsta_gauss_f1.h>
#include <bsta/bsta_mixture.h>
#include <bsta/algo/bsta_adaptive_updater.h>
#include <brdb/brdb_value.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>
#include <vidl2/vidl2_image_list_istream.h>
#include <vidl2/vidl2_frame.h>
#include <vidl2/vidl2_convert.h>
//: Constructor
bbgm_update_dist_image_stream_process::bbgm_update_dist_image_stream_process():
  istream_(0), ni_(0), nj_(0)
{
  //input  
  input_data_.resize(7,brdb_value_sptr(0));
  input_types_.resize(7);
  input_types_[0]= "bbgm_image_sptr"; 
  input_types_[1]= "vidl2_istream_sptr";
  input_types_[2]= "int"; //max_components
  input_types_[3]= "int"; //window size
  input_types_[4]= "float"; //initial_variance
  input_types_[5]= "float"; //g_thresh
  input_types_[6]= "float"; //min_stdev

  //initialize the image to null
  input_data_[0]=new brdb_value_t<bbgm_image_sptr>(0);
  //output 
  output_data_.resize(1, brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0]="bbgm_image_sptr"; 
}


//: Destructor
bbgm_update_dist_image_stream_process::~bbgm_update_dist_image_stream_process()
{
}
//: Initialize the process
bool
bbgm_update_dist_image_stream_process::init()
{
  //extract the stream
  brdb_value_t<vidl2_istream_sptr>* input1 = 
    static_cast<brdb_value_t<vidl2_istream_sptr>* >(input_data_[1].ptr());
  istream_ = input1->value();
  if(!(istream_ && istream_->is_open())){
    vcl_cerr << "In bbgm_update_dist_image_stream_process::init() -"
             << " invalid input stream\n";
    return false;
  }
  if(istream_->is_seekable())
    istream_->seek_frame(0);
  vidl2_frame_sptr f = istream_->current_frame();
  if(!f){
    vcl_cerr << "In bbgm_update_dist_image_stream_process::init() -"
             << " invalid initial frame\n";
    return false;
  } 
  ni_ = f->ni(); nj_ = f->nj();
  return true;
}

//: Execute the process
bool
bbgm_update_dist_image_stream_process::execute()
{
  // Sanity check
  if(!this->verify_inputs()){
    vcl_cerr << "In bbgm_update_dist_image_stream_process::execute() -"
             << " invalid inputs\n";
    return false;
  }
    
  // Retrieve background image
  brdb_value_t<bbgm_image_sptr>* input0 = 
    static_cast<brdb_value_t<bbgm_image_sptr>* >(input_data_[0].ptr());

  bbgm_image_sptr bgm = input0->value();

  //Retrieve max components
  brdb_value_t<int>* input2 = 
    static_cast<brdb_value_t<int>* >(input_data_[2].ptr());
  int max_components = input2->value();

  //Retrieve window_size
  brdb_value_t<int>* input3 =
    static_cast<brdb_value_t<int>* >(input_data_[3].ptr());
  int window_size = input3->value();

  //Retrieve initial_variance 
  brdb_value_t<float>* input4 = 
    static_cast<brdb_value_t<float>* >(input_data_[4].ptr());
  float initial_variance = input4->value();

  //Retrieve g_thresh 
  brdb_value_t<float>* input5 = 
    static_cast<brdb_value_t<float>* >(input_data_[5].ptr());
  float g_thresh = input5->value();

  //Retrieve min_stdev
  brdb_value_t<float>* input6 = 
    static_cast<brdb_value_t<float>* >(input_data_[6].ptr());
  float min_stdev = input6->value();

  typedef bsta_gauss_if3 bsta_gauss_t;
  typedef bsta_gauss_t::vector_type _vector;
  typedef bsta_num_obs<bsta_gauss_if3> gauss_type;
  typedef bsta_mixture<gauss_type> mix_gauss_type;
  typedef bsta_num_obs<mix_gauss_type> obs_mix_gauss_type;

  // get the templated mixture model
  bbgm_image_sptr model_sptr;
  if(!bgm){
    model_sptr = new bbgm_image_of<obs_mix_gauss_type>(ni_, nj_, obs_mix_gauss_type());
  }
  else model_sptr = bgm;
  bbgm_image_of<obs_mix_gauss_type> *model = 
    static_cast<bbgm_image_of<obs_mix_gauss_type>*>(model_sptr.ptr());

  bsta_gauss_t init_gauss(_vector(0.0f), _vector(initial_variance) );

#if 0
  bsta_mg_window_updater<mix_gauss_type> updater( init_gauss,
                                                  max_components);
#endif
  bsta_mg_grimson_window_updater<mix_gauss_type> updater( init_gauss,
                                                          max_components,
                                                          g_thresh,
                                                          min_stdev,
                                                          window_size);


  while(istream_->advance()){
    // get frame from stream
    vidl2_frame_sptr f = istream_->current_frame();
    vil_image_view_base_sptr fb = vidl2_convert_wrap_in_view(*f);
    if(!fb)
      return false;
    vil_image_view<float> frame = *vil_convert_cast(float(), fb);  
    if(fb->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
      vil_math_scale_values(frame,1.0/255.0);

    update(*model,frame,updater);
    vcl_cout << "updated frame # "<< istream_->frame_number() 
             << " format " << fb->pixel_format() << " nplanes " 
             << fb->nplanes()<< '\n';
  }
  brdb_value_sptr output0 = new brdb_value_t<bbgm_image_sptr>(model);
  output_data_[0] = output0;

  return true;
}




  
