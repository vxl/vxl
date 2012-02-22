#ifndef boxm2_compute_normal_albedo_functor_opt_h_
#define boxm2_compute_normal_albedo_functor_opt_h_
//:
// \file

#include <boxm2/boxm2_data_traits.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/boxm2_data.h>
#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/io/boxm2_stream_cache.h>
#include <vcl_vector.h>
#include <vnl/vnl_cost_function.h>

#include <brad/brad_image_metadata.h>
#include <brad/brad_atmospheric_parameters.h>

class boxm2_compute_normal_albedo_cost_function : public vnl_cost_function
{
 public:
   boxm2_compute_normal_albedo_cost_function(vcl_vector<double> const& radiances,
                                             vcl_vector<double> const& visibilities,
                                             vcl_vector<double> const& radiance_scales,
                                             vcl_vector<double> const& radiance_offsets,
                                             vcl_vector<double> const& radiance_var_scales,
                                             vcl_vector<double> const& radiance_var_offsets)
   : vnl_cost_function(1),
     num_images_(radiances.size()),
     radiances_(radiances),
     visibilities_(visibilities),
     radiance_scales_(radiance_scales),
     radiance_offsets_(radiance_offsets),
     radiance_var_scales_(radiance_var_scales),
     radiance_var_offsets_(radiance_var_offsets)
   { }

   virtual double f(vnl_vector<double> const& x);
   //virtual void gradf(vnl_vector<double> const& x, vnl_vector<double>& gradient);

 private:
  unsigned int num_images_;

  vcl_vector<double> const& radiances_;
  vcl_vector<double> const& visibilities_;

  // cache scale and offset values for expected radiance and expected radiance variance
  // expected radiance = scale * reflectance + offset
  vcl_vector<double> const& radiance_scales_;
  vcl_vector<double> const& radiance_offsets_;
  // radiance variance = scale*reflectance*reflectance + offset
  vcl_vector<double> const& radiance_var_scales_;
  vcl_vector<double> const& radiance_var_offsets_;
};


class boxm2_compute_normal_albedo_functor_opt
{
 public:
  typedef boxm2_data_traits<BOXM2_NORMAL_ALBEDO_ARRAY>::datatype naa_datatype;
  typedef boxm2_data_traits<BOXM2_ALPHA>::datatype alpha_datatype;
  typedef boxm2_data_traits<BOXM2_AUX0>::datatype aux0_datatype;
  typedef boxm2_data_traits<BOXM2_AUX1>::datatype aux1_datatype;
  typedef boxm2_data_traits<BOXM2_AUX2>::datatype aux2_datatype;
  typedef boxm2_data_traits<BOXM2_AUX3>::datatype aux3_datatype;

  //: standard constructor
  boxm2_compute_normal_albedo_functor_opt(bool update_alpha)  : update_alpha_(update_alpha) {}

  bool init_data(vcl_vector<brad_image_metadata> const& metadata,
                 vcl_vector<brad_atmospheric_parameters> const& atm_params,
                 boxm2_stream_cache_sptr str_cache,
                 boxm2_data_base * alpha_data,
                 boxm2_data_base * normal_albedo_model);

  bool process_cell(unsigned int index, bool is_leaf = false, float side_len = 0.0);

 private:
  boxm2_data<BOXM2_NORMAL_ALBEDO_ARRAY>* naa_model_data_;
  boxm2_data<BOXM2_ALPHA>* alpha_data_;

  boxm2_stream_cache_sptr str_cache_;
  boxm2_block_id id_;
  vcl_vector<brad_image_metadata> metadata_;
  vcl_vector<brad_atmospheric_parameters> atm_params_;

  unsigned int num_normals_;
  unsigned int num_images_;
  bool update_alpha_;

  // cache scale and offset values for expected radiance, and expected radiance variance
  // expected radiance = scale * reflectance + offset
  vcl_vector<vcl_vector<double> > radiance_scales_;
  vcl_vector<vcl_vector<double> > radiance_offsets_;
  // radiance variance = scale*reflectance*reflectance + offset
  vcl_vector<vcl_vector<double> > radiance_var_scales_;
  vcl_vector<vcl_vector<double> > radiance_var_offsets_;
};

#endif // boxm2_compute_normal_albedo_functor_opt_h_
