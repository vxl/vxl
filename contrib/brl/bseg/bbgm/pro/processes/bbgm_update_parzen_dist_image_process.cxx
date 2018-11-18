// This is brl/bseg/bbgm/pro/processes/bbgm_update_parzen_dist_image_process.cxx
//:
// \file

#include <iostream>
#include <bprb/bprb_func_process.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
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
bool bbgm_update_parzen_dist_image_process_cons(bprb_func_process& pro)
{
  //input
  std::vector<std::string> in_types(5), out_types(1);
  in_types[0]= "bbgm_image_sptr"; // the initial parzen distribution image
  in_types[1]= "vil_image_view_base_sptr"; // the update image
  in_types[2]= "float"; //bandwidth
  in_types[3]= "int"; //max number of samples
  in_types[4]= "float"; //equality tolerance
  pro.set_input_types(in_types);

  //output
  out_types[0]="bbgm_image_sptr";// the updated distribution image
  pro.set_output_types(out_types);
  return true;
}


bool bbgm_update_parzen_dist_image_process_init(bprb_func_process& pro)
{
  pro.set_input(0, new brdb_value_t<bbgm_image_sptr>(nullptr));
  return true;
}

//: the execute process
bool bbgm_update_parzen_dist_image_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.verify_inputs()){
    std::cerr << "In bbgm_update_parzen_dist_image_process::execute() -"
             << " invalid inputs\n";
    return false;
  }
  // Retrieve background image
  bbgm_image_sptr bgm = pro.get_input<bbgm_image_sptr>(0);

  //Retrieve update image
  vil_image_view_base_sptr update_image =
    pro.get_input<vil_image_view_base_sptr>(1);

  vil_image_view<float> img = *vil_convert_cast(float(), update_image);
  if (update_image->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    vil_math_scale_values(img,1.0/255.0);

  unsigned ni = img.ni();
  unsigned nj = img.nj();
  unsigned np = img.nplanes();

  //Retrieve bandwidth
  auto bandwidth = pro.get_input<float>(2);

  //Retrieve maximum number of samples
  auto max_samples = pro.get_input<unsigned>(3);

  auto tol = pro.get_input<float>(4);

  if(np!=3)
    {
      std::cout << "Parzen update only implemented for color\n";
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
  auto *model =
    static_cast<bbgm_image_of<parzen_f3_t>*>(model_sptr.ptr());

  float frac_back = 0.5f;
  bsta_parzen_adapt_bw_updater<parzen_f3_t> updater(tol*bandwidth, max_samples,
                                           frac_back);

  update(*model,img,updater);

  brdb_value_sptr output = new brdb_value_t<bbgm_image_sptr>(model);
  pro.set_output(0, output);
  return true;
}
