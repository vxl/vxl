#include "boxm2_vecf_middle_fat_pocket_params.h"
std::ostream&  operator << (std::ostream& s, boxm2_vecf_middle_fat_pocket_params const& middle_fat_pr){
    if(!s){
    std::cout << "FATAL! Bad parameter file stream\n";
    return s;
  }
  // bad practice --- but easy
  auto & pr = const_cast<boxm2_vecf_middle_fat_pocket_params &>(middle_fat_pr);
  s << "origin: " << pr.origin_.x() << ' ' << pr.origin_.y() << ' ' << pr.origin_.z() << '\n';
  s << "normal: " << pr.normal_.x() << ' ' << pr.normal_.y() << ' ' << pr.normal_.z() << '\n';
  s << "principal_eigenvector: " << pr.principal_eigenvector_.x() << ' '
    << pr.principal_eigenvector_.y() << ' ' << pr.principal_eigenvector_.z() << '\n';
  s << "lambda: " << pr.lambda_ << '\n';
  s << "gamma: " << pr.gamma_ << '\n';
  s << "tolerance: " << pr.tolerance_ << '\n';
  s << "max_norm_distance: " << pr.max_norm_distance_ << '\n';
  s << "scale_at_midpt: " << pr.scale_at_midpt_ << '\n';
  s << "scale_at_max: " << pr.scale_at_max_ << '\n';
  return s;}

std::istream&  operator >> (std::istream& s, boxm2_vecf_middle_fat_pocket_params& pr){
  if(!s){
    std::cout << "Bad parameter file stream\n";
    return s;
  }
  std::string st;
  double x, y, z;
  s >> st >> x >> y >> z;
  pr.origin_.set(x, y, z);

  s >> st >> x >> y >> z;
  pr.normal_.set(x, y, z);

  s >> st >> x >> y >> z;
  pr.principal_eigenvector_.set(x, y, z);

  s >> st >> pr.lambda_;
  s >> st >> pr.gamma_;
  s >> st >> pr.tolerance_;
  s >> st >> pr.max_norm_distance_;
  s >> st >> pr.scale_at_midpt_;
  s >> st >> pr.scale_at_max_;
  return s;}
