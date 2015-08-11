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
    head_params.look_dir_ = *eit;
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
    head_params.look_dir_ = *eit;
    head_params.l_orbit_params_ = params;
    head_params.r_orbit_params_ = params;
    ddt2 += 0.15;
    params_.push_back(head_params);
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

  // blink
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
//adjust brow angle
//  double brow_angle=2, angle=0;
//  unsigned char intensity =75;
// for(int i = 0; i<10; ++i){
//   boxm2_vecf_orbit_params l_params,r_params;
//     r_params.eyelid_dt_ = 0.9;
//     l_params.eyelid_dt_ = 0.9;
//     boxm2_vecf_composite_head_parameters head_params;
//     r_params.brow_angle_rad_ = brow_angle;
//     l_params.dphi_rad_ = angle;
//     angle+=0.1;
//     brow_angle+=0.2;
//     intensity+=10;
//     r_params.lower_eyelid_intensity_ = intensity;
//     head_params.l_orbit_params_ = l_params;
//     head_params.r_orbit_params_ = r_params;
//     params_.push_back(head_params);
//   }


//scale eyelid radius
 double rad = 18;
for(int i = 0; i<6; ++i){
  boxm2_vecf_orbit_params l_params,r_params;
      r_params.eyelid_dt_ = 0.9;
      l_params.eyelid_dt_ = 0.9;
    // r_params.scale_x_coef_ = 1.34;
    // r_params.scale_y_coef_ = 1.34;
    r_params.dphi_rad_ = -4.5 * vnl_math::pi/180;
    //    r_params.eyelid_crease_scale_y_coef_ = 0.97;
    boxm2_vecf_composite_head_parameters head_params;
    r_params.eye_radius_ = rad;
    rad-= 1;
    head_params.l_orbit_params_ = l_params;
    head_params.r_orbit_params_ = r_params;
    params_.push_back(head_params);
  }
//scale head
 // double scale =1.0;
 // for(int i = 0; i<6; ++i){
 //  scale-=0.05;
 //  boxm2_vecf_orbit_params l_params,r_params;
 //  l_params.eyelid_dt_ = 0;
 //    boxm2_vecf_composite_head_parameters head_params;
 //    if(i%3 == 0)
 //      r_params.eyelid_dt_ = 0;
 //    else if(i%3 == 1)
 //      r_params.eyelid_dt_ = ddt/2;
 //    else
 //      r_params.eyelid_dt_ = ddt;
 //    head_params.l_orbit_params_ = l_params;
 //    head_params.r_orbit_params_ = r_params;
 //    head_params.head_scale_.x_ = scale;
 //    params_.push_back(head_params);
 //  }


}
