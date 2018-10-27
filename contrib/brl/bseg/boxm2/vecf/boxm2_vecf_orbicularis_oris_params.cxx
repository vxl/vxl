// This is contrib/brl/bseg/boxm2/vecf/boxm2_vecf_orbicularis_oris_params.cxx
#include "boxm2_vecf_orbicularis_oris_params.h"
//:
// \file
std::ostream& operator << (std::ostream& os, boxm2_vecf_orbicularis_oris_params const& orbicularis_oris_pr)
{
  if (!os) {
    std::cout << "FATAL! Bad parameter file stream\n";
    return os;
  }
  auto& pr = const_cast<boxm2_vecf_orbicularis_oris_params &>(orbicularis_oris_pr);
  os << "principal_eigenvector_L1: " << pr.principal_eigenvector_1_.x() << ' ' << pr.principal_eigenvector_1_.y() << ' ' << pr.principal_eigenvector_1_.z() << "\n";
  os << "principal_eigenvector_L2: " << pr.principal_eigenvector_2_.x() << ' ' << pr.principal_eigenvector_2_.y() << ' ' << pr.principal_eigenvector_2_.z() << "\n";
  os << "lambda: " << pr.lambda_ << '\n';
  os << "gamma: "  << pr.gamma_ << '\n';
  os << "planar_surface_dist_thresh: " << pr.planar_surface_dist_thresh_ << '\n';
  os << "tilt_angle_in_deg: " << pr.tilt_angle_in_deg_ << '\n';
  os << "scale_factor: " << pr.scale_factor_ << '\n';
  return os;
}

std::istream& operator >> (std::istream& is, boxm2_vecf_orbicularis_oris_params& pr)
{
  if (!is) {
    std::cout << "FATAL! Bad parameter file stream\n";
    return is;
  }
  std::string st;
  double x, y, z;
  // read principle eigenvector L1
  is >> st >> x >> y >> z;
  pr.principal_eigenvector_1_.set(x, y, z);
  // read principle eigenvector L2
  is >> st >> x >> y >> z;
  pr.principal_eigenvector_2_.set(x, y, z);
  // read lambda and gamma
  is >> st >> pr.lambda_;
  is >> st >> pr.gamma_;
  is >> st >> pr.planar_surface_dist_thresh_;
  is >> st >> pr.tilt_angle_in_deg_;
  is >> st >> pr.scale_factor_;
  return is;
}
