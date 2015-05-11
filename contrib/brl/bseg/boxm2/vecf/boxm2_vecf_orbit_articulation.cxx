#include "boxm2_vecf_orbit_articulation.h"

boxm2_vecf_orbit_articulation::boxm2_vecf_orbit_articulation(){
  double ddt = 0.9;
  for(double dt = 0.0;dt<=ddt; dt +=0.1){
    boxm2_vecf_orbit_params params;
    params.eyelid_dt_ = dt;
    params_.push_back(params);
  }
  // look left right
  vcl_vector<vgl_vector_3d<double> > emv;
  emv.push_back(vgl_vector_3d<double>(0.0, 0.0, 1.0));  
  emv.push_back(vgl_vector_3d<double>(-0.1736, 0.0, 0.9848));
  emv.push_back(vgl_vector_3d<double>(-0.26, 0.0, 0.968));
  emv.push_back(vgl_vector_3d<double>(-0.1736, 0.0, 0.9848));
  emv.push_back(vgl_vector_3d<double>(0.0, 0.0, 1.0));  
  emv.push_back( vgl_vector_3d<double>(0.1736, 0.0, 0.9848));
  emv.push_back(vgl_vector_3d<double>(0.26, 0.0, 0.968));
  emv.push_back(vgl_vector_3d<double>(0.1736, 0.0, 0.9848));
  emv.push_back(vgl_vector_3d<double>(0.0, 0.0, 1.0));  
  for(vcl_vector<vgl_vector_3d<double> >::iterator eit = emv.begin();
      eit != emv.end(); eit++){
    boxm2_vecf_orbit_params params;
    params.eyelid_dt_ = ddt;
    params.eye_pointing_dir_ = *eit;
    params_.push_back(params);
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
    params.eyelid_dt_ = ddt2;
    params.eye_pointing_dir_ = *eit;
    params_.push_back(params);
    ddt2 += 0.15;
  }

  for(int i = 0; i<5; ++i){
    boxm2_vecf_orbit_params params;
    if(i%2 == 0)
      params.eyelid_dt_ = 0;
    else
      params.eyelid_dt_ = ddt;
    params_.push_back(params);
  }
}

