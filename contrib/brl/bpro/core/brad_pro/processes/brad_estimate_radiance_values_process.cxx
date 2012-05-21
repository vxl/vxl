//This is brl/bpro/core/brad_pro/processes/brad_estimate_radiance_values_process.cxx
//:
// \file
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <bsta/bsta_histogram.h>
#include <vnl/vnl_math.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vcl_string.h>
#ifdef DEBUG
#include <vcl_iostream.h>
#endif

#include <brdb/brdb_value.h>
//#include <brip/brip_vil_float_ops.h>

//:sets input and output types
bool brad_estimate_radiance_values_process_cons(bprb_func_process& pro)
{
  //inputs
  //0: The normalized image (pixel values are band-averaged radiance with units W m^-2 sr^-1 um-1
  //1: sun elevation (degrees above horizon)
  //2: Earth-Sun distance (astronomical units)
  //3: sensor elevation (degrees above horizon)
  //4: solar irradiance (W m^-2 um-1), (default provided)
  //5: downwelled irradiance (W m^-2 um-2) (default = 0 provided)
  //6: atmosphere optical depth (default provided)

  vcl_vector<vcl_string> input_types_(7);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "float";
  input_types_[2] = "float";
  input_types_[3] = "float";
  input_types_[4] = "float";
  input_types_[5] = "float";
  input_types_[6] = "float";

  if (!pro.set_input_types(input_types_))
    return false;

  //outputs: predicted radiance values for:
  //0: surface with reflectance = 0 (i.e. airlight value)
  //1: horizontal lambertian surface with reflectance = 1
  //2: sun-facing lambertian surface with reflectance = 1
  vcl_vector<vcl_string> output_types_(3);
  output_types_[0] = "float";
  output_types_[1] = "float";
  output_types_[2] = "float";

  if (!pro.set_output_types(output_types_))
    return false;

  // set default parameter values
  float Esun = 1381.79f;
  pro.set_input(4, new brdb_value_t<float>(Esun));
  float Edown = 0.0f;
  pro.set_input(5, new brdb_value_t<float>(Edown));
  float optical_depth = 0.10f;
  pro.set_input(6, new brdb_value_t<float>(optical_depth));

  return true;
}

bool brad_estimate_radiance_values_process(bprb_func_process& pro)
{
  //check number of inputs
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << " Invalid inputs" << vcl_endl;
    return false;
  }

  //get the inputs
  vil_image_view_base_sptr input_img =
    pro.get_input<vil_image_view_base_sptr>(0);

  double sun_el = pro.get_input<float>(1);
  double sun_dist = pro.get_input<float>(2);
  double sensor_el = pro.get_input<float>(3);
  double solar_irrad = pro.get_input<float>(4);
  double E_down = pro.get_input<float>(5);
  double optical_depth = pro.get_input<float>(6);

  //check inputs validity
  if (!input_img) {
    vcl_cout << pro.name() <<" :--  image  is null!\n";
    return false;
  }

  unsigned ni_ = input_img->ni();
  unsigned nj_ = input_img->nj();

  if (input_img->pixel_format() != VIL_PIXEL_FORMAT_FLOAT) {
     vcl_cerr << "ERROR: brad_estimate_radiance_values: expecting floating point image\n";
     return false;
  }
  vil_image_view<float>* image = dynamic_cast<vil_image_view<float>*>(input_img.ptr());
  if (!image) {
     vcl_cerr << "ERROR: brad_estimate_radiance_values: error casting to float image\n";
     return false;
  }
  // find min and max values in image
  float minval, maxval;
  vil_math_value_range(*image,minval,maxval);
  // compute histogram for image
  bsta_histogram<float> h(minval, maxval, 512);
  for (unsigned j = 0; j<nj_; ++j)
    for (unsigned i = 0; i<ni_; ++i)
      h.upcount((*image)(i, j), 1.0f);

  // compute airlight
  float frac = 0.0001f;
  double airlight = h.value_with_area_below(frac);

  vcl_cout << "min = " << minval << ", airlight = " << airlight << vcl_endl;

  // calculate atmosphere transmittance value
  double sun_el_rads = sun_el * vnl_math::pi_over_180;
  double sat_el_rads = sensor_el * vnl_math::pi_over_180;

  double T_sun = vcl_exp(-optical_depth / vcl_sin(sun_el_rads));
  double T_view = vcl_exp(-optical_depth / vcl_sin(sat_el_rads));

  double E_sun = solar_irrad/(sun_dist*sun_dist);

  // ideal Lambertian reflector, surface normal = [0 0 1]
  double sun_dot_norm = vcl_sin(sun_el_rads);
  double Lsat_horizontal = T_view*(E_sun*sun_dot_norm*T_sun + E_down)/vnl_math::pi + airlight;
  // ideal Lambertian reflector facing the sun
  // compute shape factor L for surface facing direction of sun
  double L = 1.0 - 0.5*vcl_cos(vnl_math::pi_over_2 - sun_el_rads);
  double Lsat_sun_facing = T_view*(E_sun*T_sun + E_down*L)/vnl_math::pi + airlight;

  vcl_cout << "max = " << maxval << " Lhoriz = " << Lsat_horizontal << " Lsun = " << Lsat_sun_facing << vcl_endl;

  pro.set_output_val<float>(0, (float)airlight);
  pro.set_output_val<float>(1, (float)Lsat_horizontal);
  pro.set_output_val<float>(2, (float)Lsat_sun_facing);

  return true;
}

