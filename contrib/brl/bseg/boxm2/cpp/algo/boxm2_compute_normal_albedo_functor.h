#ifndef boxm2_compute_normal_albedo_functor_h_
#define boxm2_compute_normal_albedo_functor_h_
//:
// \file

#include <iostream>
#include <vector>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/boxm2_data.h>
#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/io/boxm2_stream_cache.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <brad/brad_image_metadata.h>
#include <brad/brad_atmospheric_parameters.h>

class boxm2_compute_normal_albedo_functor
{
 public:
  typedef boxm2_data_traits<BOXM2_NORMAL_ALBEDO_ARRAY>::datatype naa_datatype;
  typedef boxm2_data_traits<BOXM2_ALPHA>::datatype alpha_datatype;
  typedef boxm2_data_traits<BOXM2_AUX0>::datatype aux0_datatype;
  typedef boxm2_data_traits<BOXM2_AUX1>::datatype aux1_datatype;
  typedef boxm2_data_traits<BOXM2_AUX2>::datatype aux2_datatype;
  typedef boxm2_data_traits<BOXM2_AUX3>::datatype aux3_datatype;

  //: standard constructor
  boxm2_compute_normal_albedo_functor(bool update_alpha)  : update_alpha_(update_alpha) {}

  bool init_data(std::vector<brad_image_metadata> const& metadata,
                 std::vector<brad_atmospheric_parameters> const& atm_params,
                 const boxm2_stream_cache_sptr& str_cache,
                 boxm2_data_base * alpha_data,
                 boxm2_data_base * normal_albedo_model);

  bool process_cell(unsigned int index, bool is_leaf = false, float side_len = 0.0);

 private:
  boxm2_data<BOXM2_NORMAL_ALBEDO_ARRAY>* naa_model_data_;
  boxm2_data<BOXM2_ALPHA>* alpha_data_;

  boxm2_stream_cache_sptr str_cache_;
  boxm2_block_id id_;
  std::vector<brad_image_metadata> metadata_;
  std::vector<brad_atmospheric_parameters> atm_params_;

  unsigned int num_normals_;
  unsigned int num_images_;
  bool update_alpha_;

  // cache sun positions
  std::vector<vgl_vector_3d<double> > sun_positions_;
  // cache normal directions
  std::vector<vgl_vector_3d<double> > normals_;

  // cache scale and offset values for reflectance, expected radiance, and expected radiance variance
  // reflectance = scale * radiance + offset
  std::vector<std::vector<double> > reflectance_scales_;
  std::vector<std::vector<double> > reflectance_offsets_;
  // expected radiance = scale * reflectance + offset
  std::vector<std::vector<double> > radiance_scales_;
  std::vector<std::vector<double> > radiance_offsets_;
  // radiance variance = scale*reflectance*reflectance + offset
  std::vector<std::vector<double> > radiance_var_scales_;
  std::vector<std::vector<double> > radiance_var_offsets_;

};

#endif // boxm2_compute_normal_albedo_functor_h_
