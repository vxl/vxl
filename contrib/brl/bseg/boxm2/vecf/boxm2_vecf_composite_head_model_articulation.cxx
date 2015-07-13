#include "boxm2_vecf_composite_head_model_articulation.h"

boxm2_vecf_composite_head_model_articulation::boxm2_vecf_composite_head_model_articulation(){
  double ddt = 0.9;
  for(double dt = 0.0;dt<=ddt; dt +=0.1){
    boxm2_vecf_orbit_params  params;
    boxm2_vecf_composite_head_parameters head_params;
    params.eyelid_dt_ = dt;
    head_params.l_orbit_params_ = params;
    head_params.r_orbit_params_ = params;
    params_.push_back(head_params);
  }
  // look left right
  vcl_vector<vgl_vector_3d<double> > emv;
  emv.push_back(vgl_vector_3d<double>(0.0, 0.0, 1.0));
  emv.push_back(vgl_vector_3d<double>(-0.1736, 0.0, 0.9848));
  emv.push_back(vgl_vector_3d<double>(-0.26, 0.0, 0.968));
  emv.push_back(vgl_vector_3d<double>(-0.1736, 0.0, 0.9848));
  emv.push_back(vgl_vector_3d<double>(0.0, 0.0, 1.0));
  emv.push_back(vgl_vector_3d<double>(0.1736, 0.0, 0.9848));
  emv.push_back(vgl_vector_3d<double>(0.26, 0.0, 0.968));
  emv.push_back(vgl_vector_3d<double>(0.1736, 0.0, 0.9848));
  emv.push_back(vgl_vector_3d<double>(0.0, 0.0, 1.0));
  for(vcl_vector<vgl_vector_3d<double> >::iterator eit = emv.begin();
      eit != emv.end(); eit++){
    boxm2_vecf_orbit_params params;
    boxm2_vecf_composite_head_parameters head_params;
    params.eyelid_dt_ = ddt;
    //    params.eyelid_intensity_ =10;
    params.eye_pointing_dir_ = *eit;
    head_params.l_orbit_params_ = params;
    head_params.r_orbit_params_ = params;
    params_.push_back(head_params);
  }

  // look up
  vcl_vector<vgl_vector_3d<double> > emv2;
  emv2.push_back(vgl_vector_3d<double>(0.0, 0.0, 1.0));
  emv2.push_back(vgl_vector_3d<double>(0.0, 0.0871, 0.9962));
  emv2.push_back(vgl_vector_3d<double>(0.0, 0.1736, 0.9848));
  double ddt2 = 0.75;
  for(vcl_vector<vgl_vector_3d<double> >::iterator eit = emv2.begin();
      eit != emv2.end(); eit++){
    boxm2_vecf_orbit_params params;
    boxm2_vecf_composite_head_parameters head_params;
    params.eyelid_dt_ = ddt2;
    params.eye_pointing_dir_ = *eit;
    params_.push_back(head_params);
    head_params.l_orbit_params_ = params;
    head_params.r_orbit_params_ = params;
    ddt2 += 0.15;
  }

  for(int i = 0; i<5; ++i){
    boxm2_vecf_orbit_params params;
    boxm2_vecf_composite_head_parameters head_params;
    if(i%2 == 0)
      params.eyelid_dt_ = 0;
    else
      params.eyelid_dt_ = ddt;
    head_params.l_orbit_params_ = params;
    head_params.r_orbit_params_ = params;
    params_.push_back(head_params);
  }

for(int i = 0; i<6; ++i){
  boxm2_vecf_orbit_params l_params,r_params;
  r_params.eyelid_dt_ = 0;
    boxm2_vecf_composite_head_parameters head_params;
    if(i%3 == 0)
      l_params.eyelid_dt_ = 0;
    else if(i%3 == 1)
      l_params.eyelid_dt_ = ddt/2;
    else
      l_params.eyelid_dt_ = ddt;
    head_params.l_orbit_params_ = l_params;
    head_params.r_orbit_params_ = r_params;
    params_.push_back(head_params);
  }

for(int i = 0; i<6; ++i){
  boxm2_vecf_orbit_params l_params,r_params;
  l_params.eyelid_dt_ = 0;
    boxm2_vecf_composite_head_parameters head_params;
    if(i%3 == 0)
      r_params.eyelid_dt_ = 0;
    else if(i%3 == 1)
      r_params.eyelid_dt_ = ddt/2;
    else
      r_params.eyelid_dt_ = ddt;
    head_params.l_orbit_params_ = l_params;
    head_params.r_orbit_params_ = r_params;
    params_.push_back(head_params);
  }


}
