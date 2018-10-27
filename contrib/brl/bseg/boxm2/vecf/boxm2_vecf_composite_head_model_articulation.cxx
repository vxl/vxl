#include "boxm2_vecf_composite_head_model_articulation.h"
void boxm2_vecf_composite_head_model_articulation::register_play_sequences(boxm2_vecf_composite_head_parameters const&  head_params){
  {
  std::vector<boxm2_vecf_composite_head_parameters> default_articulation;
    double ddt = 0.9;
    for(double dt = 0.0;dt<=ddt; dt +=0.1){
      boxm2_vecf_composite_head_parameters head_params_local_cpy(head_params);
      head_params_local_cpy.l_orbit_params_.eyelid_dt_  = dt;
      head_params_local_cpy.r_orbit_params_.eyelid_dt_ = dt;
      default_articulation.push_back(head_params_local_cpy);
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
      boxm2_vecf_composite_head_parameters head_params_local_cpy(head_params);

      head_params_local_cpy.look_dir_ = eit;


      head_params_local_cpy.l_orbit_params_.eye_pointing_dir_ = eit;
      head_params_local_cpy.r_orbit_params_.eye_pointing_dir_ = eit;

      head_params_local_cpy.l_orbit_params_.eyelid_dt_ = ddt;
      head_params_local_cpy.r_orbit_params_.eyelid_dt_ = ddt;

      default_articulation.push_back(head_params_local_cpy);
    }

    // look up
    std::vector<vgl_vector_3d<double> > emv2;
    emv2.emplace_back(0.0, 0.0, 1.0);
    emv2.emplace_back(0.0, 0.0871, 0.9962);
    emv2.emplace_back(0.0, 0.1736, 0.9848);
    double ddt2 = 0.75;
    for(auto & eit : emv2){
      boxm2_vecf_composite_head_parameters head_params_local_cpy(head_params);


      head_params_local_cpy.look_dir_ = eit;

      head_params_local_cpy.l_orbit_params_.eye_pointing_dir_ = eit;
      head_params_local_cpy.r_orbit_params_.eye_pointing_dir_ = eit;

      head_params_local_cpy.l_orbit_params_.eyelid_dt_ = ddt2;
      head_params_local_cpy.r_orbit_params_.eyelid_dt_ = ddt2;


      ddt2 += 0.15;
      default_articulation.push_back(head_params_local_cpy);
    }

    for(int i = 0; i<5; ++i){

      boxm2_vecf_composite_head_parameters head_params_local_cpy(head_params);

      if(i % 2 == 0){
        head_params_local_cpy.l_orbit_params_.eyelid_dt_ = 0;
        head_params_local_cpy.r_orbit_params_.eyelid_dt_ = 0;
      }else{
        head_params_local_cpy.l_orbit_params_.eyelid_dt_ = ddt;
        head_params_local_cpy.r_orbit_params_.eyelid_dt_ = ddt;
      }

      default_articulation.push_back(head_params_local_cpy);
    }

    // blink
    for(int i = 0; i<6; ++i){

      boxm2_vecf_composite_head_parameters head_params_local_cpy(head_params);
      boxm2_vecf_orbit_params& l_params = head_params_local_cpy.l_orbit_params_;
      boxm2_vecf_orbit_params& r_params = head_params_local_cpy.r_orbit_params_;
      r_params.eyelid_dt_ = 0;
      if(i%3 == 0)
        l_params.eyelid_dt_ = 0;
      else if(i%3 == 1)
        l_params.eyelid_dt_ = ddt/2;
      else
        l_params.eyelid_dt_ = ddt;

      default_articulation.push_back(head_params_local_cpy);
    }
  play_sequence_map_["default"]=default_articulation;
  }


//============================== brow angle articulation ====================
  {
    std::vector<boxm2_vecf_composite_head_parameters> brow_angle_articulation;
    //  adjust brow angle
    double brow_angle=2, angle=0;
    unsigned char intensity =75;
    for(int i = 0; i<10; ++i){

      boxm2_vecf_composite_head_parameters head_params_local_cpy(head_params);
      boxm2_vecf_orbit_params& l_params = head_params_local_cpy.l_orbit_params_;
      boxm2_vecf_orbit_params& r_params = head_params_local_cpy.r_orbit_params_;

      r_params.eyelid_dt_ = 0.9;
      l_params.eyelid_dt_ = 0.9;
      r_params.brow_angle_rad_ = brow_angle;
      l_params.dphi_rad_ = angle;
      angle+=0.1;
      brow_angle+=0.2;
      intensity+= ( unsigned char ) 10;
      r_params.lower_eyelid_intensity_ = intensity;

      brow_angle_articulation.push_back(head_params_local_cpy);
    }
  play_sequence_map_["brow_angle"]=brow_angle_articulation;
  }

//============================== eyelid radius articulation ====================
  {
    std::vector<boxm2_vecf_composite_head_parameters> eye_radius_articulation;
    double rad = 18;

    for(int i = 0; i<6; ++i){

      boxm2_vecf_composite_head_parameters head_params_local_cpy(head_params);

      boxm2_vecf_orbit_params& l_params = head_params_local_cpy.l_orbit_params_;
      boxm2_vecf_orbit_params& r_params = head_params_local_cpy.r_orbit_params_;

      r_params.eyelid_dt_ = 0.9;
      l_params.eyelid_dt_ = 0.9;
      r_params.eye_radius_ = rad;
      rad-= 1;
      eye_radius_articulation.push_back(head_params_local_cpy);
    }
  play_sequence_map_["eye_radius"]=eye_radius_articulation;
  }

//============================== head scale articulation ====================
  {
    std::vector<boxm2_vecf_composite_head_parameters> head_scale_articulation;
    double ddt = 0.9;
    //scale head
    double scale =1.0;
    for(int i = 0; i<6; ++i){
      scale-=0.05;
      boxm2_vecf_composite_head_parameters head_params_local_cpy(head_params);
      boxm2_vecf_orbit_params& l_params = head_params_local_cpy.l_orbit_params_;
      boxm2_vecf_orbit_params& r_params = head_params_local_cpy.r_orbit_params_;

      l_params.eyelid_dt_ = 0;
      if(i % 3 == 0)
        r_params.eyelid_dt_ = 0;
      else if(i % 3 == 1)
        r_params.eyelid_dt_ = ddt/2;
      else
        r_params.eyelid_dt_ = ddt;

      head_params_local_cpy.head_scale_.x_ = scale;
      head_scale_articulation.push_back(head_params_local_cpy);
    }
  play_sequence_map_["head_scale"] = head_scale_articulation;
  }
//============================== eyelid_dt open and close articulation ====================
  {
    std::vector<boxm2_vecf_composite_head_parameters> eyelid_dt_open_and_close_articulation;
    vgl_vector_3d<double> look_dir_z(0.0, 0.0, 1.0);
    //slowly close
    double incr =0.1;
    double stop = 0.9;
    for(int i=0; i<1;i++){
      double ddt = stop;
      for(double dt = ddt ; dt>=0; dt-=incr){
      boxm2_vecf_composite_head_parameters head_params_local_cpy(head_params);
        head_params_local_cpy.l_orbit_params_. eyelid_dt_ = dt;
        head_params_local_cpy.r_orbit_params_ .eyelid_dt_ = dt;

        head_params_local_cpy.l_orbit_params_. eye_pointing_dir_ = look_dir_z;
        head_params_local_cpy.r_orbit_params_ .eye_pointing_dir_ = look_dir_z;

        eyelid_dt_open_and_close_articulation.push_back(head_params_local_cpy);
      }
      ddt = 0.0;
      //slowly open
      for(double dt = ddt ; dt<=stop; dt+=incr){
      boxm2_vecf_composite_head_parameters head_params_local_cpy(head_params);
        head_params_local_cpy.l_orbit_params_. eyelid_dt_ = dt;
        head_params_local_cpy.r_orbit_params_ .eyelid_dt_ = dt;
        head_params_local_cpy.l_orbit_params_. eye_pointing_dir_ = look_dir_z;
        head_params_local_cpy.r_orbit_params_ .eye_pointing_dir_ = look_dir_z;

        eyelid_dt_open_and_close_articulation.push_back(head_params_local_cpy);
      }
    }

    std::vector<vgl_vector_3d<double> > emv2;
    emv2.emplace_back(0.0, 0.0, 1.0);
    emv2.emplace_back(0.0, 0.0871, 0.9962);
    emv2.emplace_back(0.0, 0.1736, 0.9848);

    double ddt2 = stop; unsigned pause_duration = 1;
    for(auto & eit : emv2){
      boxm2_vecf_composite_head_parameters head_params_local_cpy(head_params);
      for( unsigned j=0;j<pause_duration;j++){

      head_params_local_cpy.look_dir_ = eit;

      head_params_local_cpy.l_orbit_params_.eye_pointing_dir_ = eit;
      head_params_local_cpy.r_orbit_params_.eye_pointing_dir_ = eit;

      head_params_local_cpy.l_orbit_params_.eyelid_dt_ = ddt2;
      head_params_local_cpy.r_orbit_params_.eyelid_dt_ = ddt2;


      ddt2 += incr;
      eyelid_dt_open_and_close_articulation.push_back(head_params_local_cpy);
      }
    }

    //pause
    for (int i=0;i<30;i++){
      boxm2_vecf_composite_head_parameters head_params_local_cpy(head_params);
      head_params_local_cpy.l_orbit_params_. eye_pointing_dir_ = look_dir_z;
      head_params_local_cpy.r_orbit_params_ .eye_pointing_dir_ = look_dir_z;
      head_params_local_cpy.l_orbit_params_. eyelid_dt_ = 0.9;
      head_params_local_cpy.r_orbit_params_ .eyelid_dt_ = 0.9;
      eyelid_dt_open_and_close_articulation.push_back(head_params_local_cpy);
    }

    play_sequence_map_["eyelid_open_and_close"] = eyelid_dt_open_and_close_articulation;
  }

//============================== eyelid_dt close articulation ====================
  {
    std::vector<boxm2_vecf_composite_head_parameters > eyelid_dt_closed_articulation;
    //slowly close
    for(int i=0; i<1;i++){
      double ddt = 0.9;
      boxm2_vecf_composite_head_parameters head_params_local_cpy(head_params);
      for(double dt = ddt ; dt>=0; dt-=0.2){
        head_params_local_cpy.l_orbit_params_. eyelid_dt_ = dt;
        head_params_local_cpy.r_orbit_params_ .eyelid_dt_ = dt;
        eyelid_dt_closed_articulation.push_back(head_params_local_cpy);
      }

    }
    //pause
    for (int i=0;i<30;i++){
      boxm2_vecf_composite_head_parameters head_params_local_cpy(head_params);
      head_params_local_cpy.l_orbit_params_. eyelid_dt_ = 0.0;
      head_params_local_cpy.r_orbit_params_ .eyelid_dt_ = 0.0;
      eyelid_dt_closed_articulation.push_back(head_params_local_cpy);
    }

    play_sequence_map_["eyelid_close"] = eyelid_dt_closed_articulation;
  }
//============================== look dir articulation ====================
  {

    std::vector<boxm2_vecf_composite_head_parameters > look_dir_articulation;
    std::vector<vgl_vector_3d<double> > emv;
    boxm2_vecf_composite_head_parameters head_params_local_cpy(head_params);

    head_params_local_cpy.l_orbit_params_.eyelid_dt_ = 0.9;
    head_params_local_cpy.r_orbit_params_.eyelid_dt_ = 0.9;
    vgl_vector_3d<double> look_dir_z(0.0, 0.0, 1.0);

    vgl_vector_3d<double>  l_look_dir_cur = head_params_local_cpy.l_orbit_params_.eye_pointing_dir_;
    vgl_vector_3d<double>  r_look_dir_cur = head_params_local_cpy.r_orbit_params_.eye_pointing_dir_;

    vgl_rotation_3d<double> l_rotation(look_dir_z,l_look_dir_cur);
    vgl_rotation_3d<double> r_rotation(look_dir_z,r_look_dir_cur);
    vgl_rotation_3d<double> identity(look_dir_z,look_dir_z);

    vnl_quaternion<double> l_qa = l_rotation.as_quaternion();
    vnl_quaternion<double> r_qa = r_rotation.as_quaternion();
    vnl_quaternion<double> qb = identity.as_quaternion();
    unsigned    num_frames = 30;

    double l_cos_half_theta = l_qa.r()*qb.r() + l_qa.x()*qb.x() + l_qa.y()*qb.y() + l_qa.z()*qb.z();
    double l_half_theta = std::acos(l_cos_half_theta);
    double l_sin_half_theta = std::sin(l_half_theta);

    double r_cos_half_theta = r_qa.r()*qb.r() + r_qa.x()*qb.x() + r_qa.y()*qb.y() + r_qa.z()*qb.z();
    double r_half_theta = std::acos(r_cos_half_theta);
    double r_sin_half_theta = std::sin(r_half_theta);

    for (unsigned i = 0; i<=num_frames; ++i) {
      double t = ((double)i)/num_frames;
      vnl_quaternion<double> l_qu = l_qa * std::sin((1-t)*l_half_theta) + qb * std::sin(t*l_half_theta);
      l_qu /= l_sin_half_theta;

      vnl_quaternion<double> r_qu = r_qa * std::sin((1-t)*r_half_theta) + qb * std::sin(t*r_half_theta);
      r_qu /= r_sin_half_theta;

      vnl_matrix_fixed<double,3,3> interped_rot_r = vgl_rotation_3d<double>(r_qu).as_matrix();
      vnl_matrix_fixed<double,3,3> interped_rot_l = vgl_rotation_3d<double>(l_qu).as_matrix();
      head_params_local_cpy.l_orbit_params_.eye_pointing_dir_ = vgl_vector_3d<double>(interped_rot_l.get(0,2),interped_rot_l.get(1,2),interped_rot_l.get(2,2));
      head_params_local_cpy.r_orbit_params_.eye_pointing_dir_ = vgl_vector_3d<double>(interped_rot_r.get(0,2),interped_rot_r.get(1,2),interped_rot_r.get(2,2));

      look_dir_articulation.push_back(head_params_local_cpy);
    }

    emv.emplace_back(0.0, 0.0, 1.0);
    emv.emplace_back(-0.1736, 0.0, 0.9848);
    emv.emplace_back(-0.26, 0.0, 0.968);
    emv.emplace_back(-0.1736, 0.0, 0.9848);
    emv.emplace_back(0.0, 0.0, 1.0);
    emv.emplace_back(0.1736, 0.0, 0.9848);
    emv.emplace_back(0.26, 0.0, 0.968);
    emv.emplace_back(0.1736, 0.0, 0.9848);
    emv.emplace_back(0.0, 0.0, 1.0);
    unsigned pause_duration = 7;
    for(auto & eit : emv){
      for (unsigned j = 0; j<pause_duration; j++){
        boxm2_vecf_composite_head_parameters head_params_local_cpy(head_params);
        head_params_local_cpy.look_dir_ = eit;
        head_params_local_cpy.l_orbit_params_.eye_pointing_dir_ = eit;
        head_params_local_cpy.r_orbit_params_.eye_pointing_dir_ = eit;
        look_dir_articulation.push_back(head_params_local_cpy);
      }
    }

    play_sequence_map_["look_dir"] = look_dir_articulation;
  }
//============================== close while looking down ====================
{
    std::vector<boxm2_vecf_composite_head_parameters > eyelid_dt_close_look_dir_articulation;
    //slowly close
    boxm2_vecf_composite_head_parameters head_params_local_cpy(head_params);

    vgl_vector_3d<double> look_dir_down(0.0, -sin(15 * vnl_math::pi/180), cos(15 * vnl_math::pi/180));

    vgl_vector_3d<double> Z(0.0,0.0,1);

    vgl_vector_3d<double>  l_look_dir_cur = head_params_local_cpy.l_orbit_params_.eye_pointing_dir_;
    vgl_vector_3d<double>  r_look_dir_cur = head_params_local_cpy.r_orbit_params_.eye_pointing_dir_;

    vgl_rotation_3d<double> l_rotation(Z,l_look_dir_cur);
    vgl_rotation_3d<double> r_rotation(Z,r_look_dir_cur);

    vgl_rotation_3d<double> down(Z,look_dir_down);

    vnl_quaternion<double> l_qa = l_rotation.as_quaternion();
    vnl_quaternion<double> r_qa = r_rotation.as_quaternion();
    vnl_quaternion<double> qb   = down.as_quaternion();


    double l_cos_half_theta = l_qa.r()*qb.r() + l_qa.x()*qb.x() + l_qa.y()*qb.y() + l_qa.z()*qb.z();
    double l_half_theta     = std::acos(l_cos_half_theta);
    double l_sin_half_theta = std::sin(l_half_theta);

    double r_cos_half_theta = r_qa.r()*qb.r() + r_qa.x()*qb.x() + r_qa.y()*qb.y() + r_qa.z()*qb.z();
    double r_half_theta     = std::acos(r_cos_half_theta);
    double r_sin_half_theta = std::sin(r_half_theta);

    for(int i=0; i<1;i++){
      double ddt = 0.9; unsigned  count = 0;
      unsigned    num_frames = 10; double dt_incr =1.0/(30);
      boxm2_vecf_composite_head_parameters head_params_local_cpy(head_params);
      for(double dt = ddt ; dt>=0 ; dt -= dt_incr){
        double t = ((double)count)/num_frames;

        count = count < num_frames ? count + 1 : count;
        vnl_quaternion<double> l_qu = l_qa * std::sin((1-t)*l_half_theta) + qb * std::sin(t*l_half_theta);
        l_qu /= l_sin_half_theta;

        vnl_quaternion<double> r_qu = r_qa * std::sin((1-t)*r_half_theta) + qb * std::sin(t*r_half_theta);
        r_qu /= r_sin_half_theta;

        vnl_matrix_fixed<double,3,3> interped_rot_r = vgl_rotation_3d<double>(r_qu).as_matrix();
        vnl_matrix_fixed<double,3,3> interped_rot_l = vgl_rotation_3d<double>(l_qu).as_matrix();
        head_params_local_cpy.l_orbit_params_.eye_pointing_dir_ = vgl_vector_3d<double>(interped_rot_l.get(0,2),interped_rot_l.get(1,2),interped_rot_l.get(2,2));
        head_params_local_cpy.r_orbit_params_.eye_pointing_dir_ = vgl_vector_3d<double>(interped_rot_r.get(0,2),interped_rot_r.get(1,2),interped_rot_r.get(2,2));
        head_params_local_cpy.l_orbit_params_. eyelid_dt_ = dt;
        head_params_local_cpy.r_orbit_params_ .eyelid_dt_ = dt;
        eyelid_dt_close_look_dir_articulation.push_back(head_params_local_cpy);

      }
    }
    //    pause
    for (int i=0;i<30;i++){
      boxm2_vecf_composite_head_parameters head_params_local_cpy(head_params);
      head_params_local_cpy.l_orbit_params_. eyelid_dt_ = 0.0;
      head_params_local_cpy.r_orbit_params_ .eyelid_dt_ = 0.0;
      eyelid_dt_close_look_dir_articulation.push_back(head_params_local_cpy);
    }

    play_sequence_map_["eyelid_close_and_look_dir"] = eyelid_dt_close_look_dir_articulation;
  }
 std::vector<boxm2_vecf_composite_head_parameters> template_1;
 {
   unsigned   num_frames  = 4;
   std::vector<vgl_vector_3d<double> > emv;
   //4 look directions
    emv.emplace_back(-0.26, 0.0, 0.968);
    emv.emplace_back(-0.1736, 0.0, 0.9848);
    emv.emplace_back(0.1736, 0.0, 0.9848);
    emv.emplace_back(0.26, 0.0, 0.968);



    for (unsigned i = 0; i<4; ++i) {
      boxm2_vecf_composite_head_parameters head_params_local_cpy(head_params);
      head_params_local_cpy.l_orbit_params_.eye_pointing_dir_ = emv[i];
      head_params_local_cpy.r_orbit_params_.eye_pointing_dir_ = emv[i];
      head_params_local_cpy.l_orbit_params_.eyelid_dt_ = 0.94;
      head_params_local_cpy.r_orbit_params_.eyelid_dt_ = 0.94;

      template_1.push_back(head_params_local_cpy);
    }
    //three eyelids
    vgl_vector_3d<double> look_dir_z(0,0,1);
    std::vector<double> dts;
    dts.push_back(0.5);
    dts.push_back(0);
    dts.push_back(0.95);
    for (unsigned i = 0; i<2; ++i) {
    boxm2_vecf_composite_head_parameters head_params_local_cpy(head_params);
      head_params_local_cpy.l_orbit_params_.eye_pointing_dir_ = look_dir_z;
      head_params_local_cpy.r_orbit_params_.eye_pointing_dir_ = look_dir_z;
      head_params_local_cpy.l_orbit_params_.eyelid_dt_ = dts[i];
      head_params_local_cpy.r_orbit_params_.eyelid_dt_ = dts[i];
      template_1.push_back(head_params_local_cpy);
    }

 }
 play_sequence_map_["template_articulation_1"] = template_1;
}
