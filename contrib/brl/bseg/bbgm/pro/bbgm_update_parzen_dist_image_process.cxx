// This is brl/bseg/bbgm/pro/bbgm_update_parzen_dist_image_process.cxx
#include "bbgm_update_parzen_dist_image_process.h"
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <bbgm/bbgm_image_of.h>
#include <bbgm/bbgm_image_sptr.h>
#include <bbgm/bbgm_update.h>
#include <bsta/bsta_parzen_sphere.h>
#include <bsta/io/bsta_io_parzen_sphere.h>
#include <bsta/algo/bsta_parzen_updater.h>
#include <brdb/brdb_value.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>

//: Constructor
bbgm_update_parzen_dist_image_process::bbgm_update_parzen_dist_image_process()
{
  //input
  input_data_.resize(5,brdb_value_sptr(0));
  input_types_.resize(5);
  input_types_[0]= "bbgm_image_sptr";
  input_types_[1]= "vil_image_view_base_sptr";
  input_types_[2]= "float"; //bandwidth
  input_types_[3]= "int"; //max number of samples
  input_types_[4]= "float"; //equality tolerance

  //output
  output_data_.resize(1, brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0]="bbgm_image_sptr";
}


//: Destructor
bbgm_update_parzen_dist_image_process::~bbgm_update_parzen_dist_image_process()
{
}

bool bbgm_update_parzen_dist_image_process::init()
{
  input_data_[0]=
    new brdb_value_t<bbgm_image_sptr>(0);
  return true;
}

//: Execute the process
bool
bbgm_update_parzen_dist_image_process::execute()
{
  // Sanity check
  if (!this->verify_inputs()){
    vcl_cerr << "In bbgm_update_parzen_dist_image_process::execute() -"
             << " invalid inputs\n";
    return false;
  }
  // Retrieve background image
  brdb_value_t<bbgm_image_sptr>* input0 =
    static_cast<brdb_value_t<bbgm_image_sptr>* >(input_data_[0].ptr());

  bbgm_image_sptr bgm = input0->value();

  //Retrieve update image
  brdb_value_t<vil_image_view_base_sptr>* input1 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[1].ptr());
  vil_image_view_base_sptr update_image = input1->value();

  vil_image_view<float> img = *vil_convert_cast(float(), update_image);
  if (update_image->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    vil_math_scale_values(img,1.0/255.0);

  unsigned ni = img.ni();
  unsigned nj = img.nj();
  unsigned np = img.nplanes();

  //Retrieve bandwidth
  brdb_value_t<float>* input2 =
    static_cast<brdb_value_t<float>* >(input_data_[2].ptr());
  float bandwidth = input2->value();

  //Retrieve maximum number of samples
  brdb_value_t<unsigned>* input3 =
    static_cast<brdb_value_t<unsigned>* >(input_data_[3].ptr());
  unsigned max_samples = input3->value();

  brdb_value_t<float>* input4 =
    static_cast<brdb_value_t<float>* >(input_data_[4].ptr());
  float tol = input4->value();

  if(np!=3)
    {
      vcl_cout << "Parzen update only implemented for color\n";
      return false;
    }

  typedef bsta_parzen_sphere<float,3> parzen_f3_t;

  //cast the model to an image of parzen distributions
  bbgm_image_sptr model_sptr;
  if (!bgm) {
    parzen_f3_t par;
    par.set_bandwidth(bandwidth);
    model_sptr = new bbgm_image_of<parzen_f3_t>(ni,nj,par);
  }
  else model_sptr = bgm;
  bbgm_image_of<parzen_f3_t> *model =
    static_cast<bbgm_image_of<parzen_f3_t>*>(model_sptr.ptr());

  float frac_back = 0.5f;
  bsta_parzen_adapt_bw_updater<parzen_f3_t> updater(tol*bandwidth, max_samples,
                                           frac_back);

  update(*model,img,updater);

  brdb_value_sptr output0 = new brdb_value_t<bbgm_image_sptr>(model);
  output_data_[0] = output0;
  return true;
}

