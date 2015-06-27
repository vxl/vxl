#include "boxm2_vecf_orbit_params.h"
#include <vcl_iostream.h>
vcl_ostream&  operator<<(vcl_ostream& s, boxm2_vecf_orbit_params const& pr){
  if(!s){
    vcl_cout << "Bad parameter file stream\n";
    return s;
  }
  s << "eye radius: " << pr.eye_radius_ << '\n';
 // s << "medial_socket_radius: " << pr.medial_socket_radius() << '\n';
  //s << "lateral_socket_radius: " << pr.lateral_socket_radius() << '\n';
  s << "trans_x: " << pr.x_trans() <<  '\n';
  s << "trans_y: " << pr.y_trans() <<  '\n';
  s << "trans_z: " << pr.z_trans() <<  '\n';
  s << "x_scale: " << pr.scale_x() << '\n';
  s << "y_scale: " << pr.scale_y() << '\n';
  s << "crease_y_scale: " << pr.eyelid_crease_scale_y() << '\n';
  s << "phi rotation angle (deg): " << pr.dphi_rad_*(180.0/3.14159) << '\n';
  s << "inferior_margin_t: " << pr.lower_eyelid_tmin_ << '\n';
  s << "superior_margin_t: " << pr.eyelid_tmin_ << '\n';
  s << " mid crease z: " << pr.mid_eyelid_crease_z_ << '\n';
  s << "crease_t: " << pr.eyelid_crease_ct_ << '\n';
  return s;
}

