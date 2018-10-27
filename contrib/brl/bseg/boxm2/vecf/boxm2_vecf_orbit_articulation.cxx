#include "boxm2_vecf_orbit_articulation.h"

boxm2_vecf_orbit_articulation::boxm2_vecf_orbit_articulation(){

  //======================= default articulation =====================
  std::vector<boxm2_vecf_orbit_params> default_articulation;
  {
    double ddt = 0.9;
    for(double dt = 0.0;dt<=ddt; dt +=0.1){
      boxm2_vecf_orbit_params  params;
      params.eyelid_dt_ = dt;
      default_articulation.push_back(params);
    }
    // look left right
    std::vector<vgl_vector_3d<double> > emv;
    emv.emplace_back(0.0, 0.0, 1.0);
    emv.emplace_back(-0.1736, 0.0, 0.9848);
    emv.emplace_back(-0.26, 0.0, 0.968);
    emv.emplace_back(-0.1736, 0.0, 0.9848);
    emv.emplace_back(0.0, 0.0, 1.0);
    emv.emplace_back(0.1736, 0.0, 0.9848);
    emv.emplace_back(0.26, 0.0, 0.968);
    emv.emplace_back(0.1736, 0.0, 0.9848);
    emv.emplace_back(0.0, 0.0, 1.0);
    for(auto & eit : emv){
      boxm2_vecf_orbit_params params;
      params.eyelid_dt_ = ddt;
      params.eye_pointing_dir_ = eit;
      default_articulation.push_back(params);
    }

    // look up
    std::vector<vgl_vector_3d<double> > emv2;
    emv2.emplace_back(0.0, 0.0, 1.0);
    emv2.emplace_back(0.0, 0.0871, 0.9962);
    emv2.emplace_back(0.0, 0.1736, 0.9848);
    double ddt2 = 0.75;
    for(auto & eit : emv2){
      boxm2_vecf_orbit_params params;
      params.eyelid_dt_ = ddt2;
      params.eye_pointing_dir_ = eit;
      default_articulation.push_back(params);
      ddt2 += 0.15;
    }

    for(int i = 0; i<5; ++i){
      boxm2_vecf_orbit_params params;
      if(i%2 == 0)
        params.eyelid_dt_ = 0;
      else
        params.eyelid_dt_ = ddt;
      default_articulation.push_back(params);
    }
  }
  play_sequence_map_["default"] = default_articulation;
  this->set_play_sequence("default") ;

 //================== dphi adjustment ==============================
 std::vector<boxm2_vecf_orbit_params> dphi_articulation;
 {
   double brow_angle=2, angle=0;
   for(int i = 0; i<10; ++i){
     boxm2_vecf_orbit_params params;
     params.eyelid_dt_ = 0.9;
     params.brow_angle_rad_ = brow_angle;
     params.dphi_rad_ = angle;
     angle+=0.05;
     brow_angle+=0.2;
     dphi_articulation.push_back(params);
   }
 }
 play_sequence_map_["dphi_articulation"] = dphi_articulation;
//================== crazy rotation ==============================
 std::vector<boxm2_vecf_orbit_params> rotation_articulation;
 {
   unsigned   num_frames  = 30;
   vgl_vector_3d<double> look_dir_cur(0,0,-1);
   vgl_vector_3d<double> look_dir_z(0,0,1);

   vgl_rotation_3d<double> rotation(look_dir_z,look_dir_cur);
   vgl_rotation_3d<double> identity(look_dir_z,look_dir_z);

   vnl_quaternion<double> qa = rotation.as_quaternion();
   vnl_quaternion<double> qb = identity.as_quaternion();
   double cos_half_theta = qa.r()*qb.r() + qa.x()*qb.x() + qa.y()*qb.y() + qa.z()*qb.z();
    double half_theta = std::acos(cos_half_theta);
    double sin_half_theta = std::sin(half_theta);

   for (unsigned i = 0; i<=num_frames; ++i) {
   boxm2_vecf_orbit_params params;
      double t = ((double)i)/num_frames;
      vgl_vector_3d<double> q = look_dir_cur * std::sin((1-t)*half_theta) + look_dir_z * std::sin(t*half_theta);
      q /= sin_half_theta;
      params.eye_pointing_dir_ = q;

      rotation_articulation.push_back(params);
    }
 }
 play_sequence_map_["rotation_articulation"] = rotation_articulation;

std::vector<boxm2_vecf_orbit_params> template_1;
 {
   unsigned   num_frames  = 4;
   std::vector<vgl_vector_3d<double> > emv;
   //4 look directions
    emv.emplace_back(-0.26, 0.0, 0.968);
    emv.emplace_back(-0.1736, 0.0, 0.9848);
    emv.emplace_back(0.1736, 0.0, 0.9848);
    emv.emplace_back(0.26, 0.0, 0.968);



    for (unsigned i = 0; i<4; ++i) {
      boxm2_vecf_orbit_params params;
      params.eye_pointing_dir_ = emv[i];
      template_1.push_back(params);
    }
    //three eyelids
    vgl_vector_3d<double> look_dir_z(0,0,1);
    std::vector<double> dts;
    dts.push_back(0.5);
    dts.push_back(0);
    dts.push_back(0.95);
    for (unsigned i = 0; i<2; ++i) {
      boxm2_vecf_orbit_params params;
      params.eye_pointing_dir_ = look_dir_z;
      params.eyelid_dt_ = dts[i];
      template_1.push_back(params);
    }

 }
 play_sequence_map_["template_articulation_1"] = template_1;


  //==============add more articulations here=======================

 std::vector<boxm2_vecf_orbit_params> new_articulation;
 {

 }
 play_sequence_map_["new_articulation"] = new_articulation;
}
