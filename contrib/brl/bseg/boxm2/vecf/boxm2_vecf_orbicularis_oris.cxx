// This is contrib/brl/bseg/boxm2/vecf/boxm2_vecf_orbicularis_oris.cxx
#include <iomanip>
#include <limits>
#include "boxm2_vecf_orbicularis_oris.h"
//
// \file
#include <vgl/vgl_distance.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_line_3d_2_points.h>

void boxm2_vecf_orbicularis_oris::fill_boundary_map()
{
  boundary_knots_["lower_body_start"] = 4.0;
  boundary_knots_["lower_body_end"] = 16.0 ;
}

void boxm2_vecf_orbicularis_oris::rotation_matrix(vgl_vector_3d<double> const& l1, vgl_vector_3d<double> const& l2, vgl_vector_3d<double> const& l3,
                                                  vnl_matrix_fixed<double, 3, 3>& rot_matrix) const
{
  vgl_vector_3d<double> e1(1.0, 0.0, 0.0);
  vgl_vector_3d<double> e2(0.0, 1.0, 0.0);
  vgl_vector_3d<double> e3(0.0, 0.0, 1.0);
  vgl_vector_3d<double> unit_l1 = l1;
  vgl_vector_3d<double> unit_l2 = l2;
  vgl_vector_3d<double> unit_l3 = l3;
  normalize(unit_l1);
  normalize(unit_l2);
  normalize(unit_l3);
  rot_matrix[0][0] = dot_product(e1,unit_l1);
  rot_matrix[0][1] = dot_product(e2,unit_l1);
  rot_matrix[0][2] = dot_product(e3,unit_l1);
  rot_matrix[1][0] = dot_product(e1,unit_l2);
  rot_matrix[1][1] = dot_product(e2,unit_l2);
  rot_matrix[1][2] = dot_product(e3,unit_l2);
  rot_matrix[2][0] = dot_product(e1,unit_l3);
  rot_matrix[2][1] = dot_product(e2,unit_l3);
  rot_matrix[2][2] = dot_product(e3,unit_l3);
  return;
}

boxm2_vecf_orbicularis_oris::boxm2_vecf_orbicularis_oris(std::string const& geometry_file, double const& cross_section_interval)
{
  std::ifstream istr(geometry_file.c_str());
  if (!istr) {
    std::cout << " invalid path for orbicularis oris geometry: " << geometry_file << '\n';
    return;
  }
  this->cross_section_interval_ = cross_section_interval;
  std::map<std::string, std::string> oo_path_map;
  std::string component, path;
  while (istr >> component >> path)
    oo_path_map[component] = path;
  std::map<std::string, std::string>::iterator mit;
  if (oo_path_map.find("axis_spline") != oo_path_map.end()) {
    std::ifstream sstr(oo_path_map["axis_spline"].c_str());
    this->read_axis_spline(sstr);
    sstr.close();
  }
  if (this->axis_.n_knots() == 0){
    std::cout << "load axis from " << oo_path_map["axis_spline"] << " failed!\n";
    return;
  }
  if (oo_path_map.find("cross_section_pointset") != oo_path_map.end()) {
    std::ifstream cstr(oo_path_map["cross_section_pointset"].c_str());
    this->load_cross_section_pointsets(cstr);
    cstr.close();
  }
  fill_boundary_map();
}

// tilt the lower part of the muscle to mimic the open mouth
boxm2_vecf_orbicularis_oris boxm2_vecf_orbicularis_oris::tilt() const
{
  double tilt_in_rad = params_.tilt_angle_in_deg_ / vnl_math::deg_per_rad;
  // define the rotation axis
  double st, et;
  st = this->boundary_knots_.find("lower_body_start")->second;
  et = this->boundary_knots_.find("lower_body_end")->second;
  vgl_point_3d<double> startp = this->axis_(st);
  vgl_point_3d<double> endp = this->axis_(et);
  vgl_vector_3d<double> rot_axis = startp-endp;
  normalize(rot_axis);
  std::cout << "st: " << st << "et: " << et << std::endl;
  std::cout << "start point: " << startp << std::endl;
  std::cout << "end   point: " << endp << std::endl;
  std::cout << "rotation axis: " << rot_axis << std::endl;

  // deform the axis
  std::vector<vgl_point_3d<double> > def_knots;
  double n_knots = this->axis_.n_knots();
  for (double t = 0; t < n_knots; t+= 1.0)
  {
    vgl_point_3d<double> ori_p = this->axis_(t);
    if ( t <= st || t >= et) {
      def_knots.push_back(ori_p);
      continue;
    }
    // compute the projection point of p to rotation axis
    vgl_vector_3d<double> sp = ori_p - startp;
    vgl_point_3d<double> proj_p = startp + dot_product(sp, rot_axis)*rot_axis;
    // define rotation from e1e2e3 to l1l2l3 (l1 == rotation axis , l2 == ori_p - proj_p , l3 == l1xl2)
    vgl_vector_3d<double> ori_lp = ori_p - proj_p;
    vgl_vector_3d<double> l3 = cross_product(rot_axis, ori_lp);
    // compute the rotation matrix
    vnl_matrix_fixed<double, 3, 3> rot_matrix;
    this->rotation_matrix(rot_axis, ori_lp, l3, rot_matrix);
    vgl_rotation_3d<double> rot(rot_matrix);
    vgl_rotation_3d<double> inv_rot(rot_matrix.transpose());
    // deform the point
    vgl_vector_3d<double> rot_lp = rot * ori_lp;
    double def_y = rot_lp.length() * std::cos(tilt_in_rad);
    double def_z = rot_lp.length() * std::sin(tilt_in_rad);
    vgl_point_3d<double> def_rot_lp(rot_lp.x(), def_y, def_z);
    vgl_point_3d<double> inv_rot_lp = inv_rot * def_rot_lp;
    vgl_point_3d<double> def_p(inv_rot_lp.x() + proj_p.x(), inv_rot_lp.y() + proj_p.y(), inv_rot_lp.z() + proj_p.z());
    def_knots.push_back(def_p);
  }
  vgl_cubic_spline_3d<double> def_axis(def_knots);

  // deform the points
  auto n_cs = static_cast<unsigned>(this->cross_sections_.size());
  vgl_pointset_3d<double> def_ptsets;
  for (unsigned c_idx = 0; c_idx < n_cs; c_idx++)
  {
    double ct = this->cross_sections_[c_idx].t();
    vgl_pointset_3d<double> cs_pts = this->cross_sections_[c_idx].pts();
    unsigned c_npts = cs_pts.npts();
    if ( ct <= st || ct >= et) {
      for (unsigned i = 0; i < c_npts; i++)
        def_ptsets.add_point_with_normal(cs_pts.p(i), cs_pts.n(i));
      continue;
    }
    // deform the point (note that the surface normal shall remain fixed sine surface doesn't change during tilting)
    for (unsigned i = 0; i < c_npts; i++) {
      vgl_point_3d<double> ori_p = cs_pts.p(i);
      vgl_vector_3d<double> ori_n = cs_pts.n(i);
      vgl_vector_3d<double> sp = ori_p - startp;
      vgl_point_3d<double> proj_p = startp + dot_product(sp, rot_axis)*rot_axis;
      // define rotation from e1e2e3 to l1l2l3 (l1 == rotation axis , l2 == ori_p - proj_p , l3 == l1xl2)
      vgl_vector_3d<double> ori_lp = ori_p - proj_p;
      vgl_vector_3d<double> l3 = cross_product(rot_axis, ori_lp);
      // compute the rotation matrix
      vnl_matrix_fixed<double, 3, 3> rot_matrix;
      this->rotation_matrix(rot_axis, ori_lp, l3, rot_matrix);
      vgl_rotation_3d<double> rot(rot_matrix);
      vgl_rotation_3d<double> inv_rot(rot_matrix.transpose());
      // deform the point
      vgl_vector_3d<double> rot_lp = rot * ori_lp;
      double def_y = rot_lp.length() * std::cos(tilt_in_rad);
      double def_z = rot_lp.length() * std::sin(tilt_in_rad);
      vgl_point_3d<double> def_rot_lp(rot_lp.x(), def_y, def_z);
      vgl_point_3d<double> inv_rot_lp = inv_rot * def_rot_lp;
      vgl_point_3d<double> def_p(inv_rot_lp.x() + proj_p.x(), inv_rot_lp.y() + proj_p.y(), inv_rot_lp.z() + proj_p.z());
      def_ptsets.add_point_with_normal(def_p, ori_n);
    }
  }

  // create new cross sections based on deformed axis and point set
  // create new cross section
  std::vector<vgl_pointset_3d<double> > cs_ptsets;
  std::vector<vgl_plane_3d<double> > cs_planes;
  std::vector<double> tset;
  std::vector<vgl_point_3d<double> > pset;
  double mt = def_axis.max_t();
  for (double t = 0.0; t <= mt; t += this->cross_section_interval_) {
    vgl_point_3d<double> p = def_axis(t);
    vgl_plane_3d<double> norm_plane = def_axis.normal_plane(t);
    cs_ptsets.emplace_back();
    cs_planes.push_back(norm_plane);
    tset.push_back(t);
    pset.push_back(p);
  }
  // scan through the deformed pointset and assign each point to a cross section
  auto nc = static_cast<unsigned>(cs_ptsets.size());
  unsigned npts = this->aggregate_pointset().npts();
  for (unsigned i = 0; i < npts; i++)
  {
    vgl_point_3d<double>  pi = def_ptsets.p(i);
    vgl_vector_3d<double> ni = def_ptsets.n(i);
    double min_dist = std::numeric_limits<double>::max();
    unsigned min_j = 0;
    double dp, dk, d;
    for (unsigned j = 0; j < nc; j++) {
      dp = vgl_distance(pi, cs_planes[j]);
      dk = vgl_distance(pi, pset[j]);
      d = dp + dk;
      if (d < min_dist) {
        min_dist = d;
        min_j = j;
      }
    }
    cs_ptsets[min_j].add_point_with_normal(pi, ni);
  }
  // construct new cross sections
  std::vector<bvgl_cross_section> def_cs;
  for (unsigned i = 0; i < nc; i++) {
    bvgl_cross_section cs(tset[i], pset[i], cs_planes[i], cs_ptsets[i]);
    def_cs.push_back(cs);
  }

  boxm2_vecf_orbicularis_oris target(def_axis, def_cs, this->cross_section_interval_);
  return target;
}

// adjust the size of the muscle
boxm2_vecf_orbicularis_oris boxm2_vecf_orbicularis_oris::scale() const
{
  // compute the stretch along principal axes
  double s1 = params_.scale_factor_;
  // get the center of the muscle
  vgl_point_3d<double> c_3d;
  vgl_pointset_3d<double> ptsets_all = this->aggregate_pointset();
  unsigned npts = ptsets_all.npts();
  double cx = 0.0, cy = 0.0, cz = 0.0;
  for (unsigned i = 0; i < npts; i++) {
    vgl_point_3d<double> pt = ptsets_all.p(i);
    cx += pt.x();  cy += pt.y();  cz += pt.z();
  }
  cx /= npts;
  cy /= npts;
  cz /= npts;
  c_3d.set(cx, cy, cz);
  // deform the axis
  // at each knot, the principal contraction axis, l1, follows the vector from the knot to the ankle point c_3d
  // the other two principal axes are define on the plane that perpendicular to l1
  // l2 = cross_product(e3, l1) --> l2 will stay close to e2
  // l3 = cross_product(l1, l2)
  vgl_vector_3d<double> e1(1.0, 0.0, 0.0);
  vgl_vector_3d<double> e2(0.0, 1.0, 0.0);
  vgl_vector_3d<double> e3(0.0, 0.0, 1.0);
  std::vector<vgl_point_3d<double> > deformed_knots;
  vgl_cubic_spline_3d<double> ori_axis = this->axis();
  double n_knots = ori_axis.n_knots();
  for (double t = 0.0; t < n_knots; t+=1.0)
  {
    vgl_point_3d<double> ori_p = ori_axis(t);
    // define the principal axes
    vgl_vector_3d<double> l1 = c_3d - ori_p;
    normalize(l1);
    vgl_vector_3d<double> l2 = normalized(cross_product(e3, l1));
    vgl_vector_3d<double> l3 = normalized(cross_product(l1, l2));
    // compute the rotation matrix
    vnl_matrix_fixed<double, 3, 3> rot_matrix;
    this->rotation_matrix(l1, l2, l3, rot_matrix);
    vgl_rotation_3d<double> rot(rot_matrix);
    vgl_rotation_3d<double> inv_rot(rot_matrix.transpose());
    // deform the point
    vgl_vector_3d<double> ori_lp = ori_p - c_3d;
    vgl_vector_3d<double> rot_lp = rot*ori_lp;
    // stretch along principal axes
    double sx = s1 * rot_lp.x();
    vgl_point_3d<double> deformed_lp(sx, rot_lp.y(), rot_lp.z());
    // rotate back
    vgl_point_3d<double> inv_rot_p = inv_rot * deformed_lp;
    vgl_point_3d<double> deformed_p(inv_rot_p.x() + c_3d.x(), inv_rot_p.y() + c_3d.y(), inv_rot_p.z() + c_3d.z());
    deformed_knots.push_back(deformed_p);
  }
  vgl_cubic_spline_3d<double> deformed_axis(deformed_knots);
  vgl_pointset_3d<double> def_ptset_all;
  for (unsigned i = 0; i < npts; i++)
  {
    vgl_point_3d<double>  ori_p = ptsets_all.p(i);
    vgl_vector_3d<double> ori_n = ptsets_all.n(i);
    // compute the rotation matrix
    vgl_vector_3d<double> l1 = c_3d - ori_p;
    normalize(l1);
    vgl_vector_3d<double> l2 = normalized(cross_product(e3, l1));
    vgl_vector_3d<double> l3 = normalized(cross_product(l1, l2));
    // compute the rotation matrix
    vnl_matrix_fixed<double, 3, 3> rot_matrix;
    this->rotation_matrix(l1, l2, l3, rot_matrix);
    vgl_rotation_3d<double> rot(rot_matrix);
    vgl_rotation_3d<double> inv_rot(rot_matrix.transpose());
    // deform the point
    vgl_vector_3d<double> ori_lp = ori_p - c_3d;
    vgl_vector_3d<double> rot_lp = rot*ori_lp;
    double sx = s1 * rot_lp.x();
    vgl_point_3d<double> deformed_lp(sx, rot_lp.y(), rot_lp.z());
    // rotate back
    vgl_point_3d<double> inv_rot_p = inv_rot * deformed_lp;
    vgl_point_3d<double> def_p(inv_rot_p.x() + c_3d.x(), inv_rot_p.y() + c_3d.y(), inv_rot_p.z() + c_3d.z());
    // the normal should remain same since entire muscle undergoes same scaling
    def_ptset_all.add_point_with_normal(def_p, ori_n);
  }

  // create new cross section
  std::vector<vgl_pointset_3d<double> > cs_ptsets;
  std::vector<vgl_plane_3d<double> > cs_planes;
  std::vector<double> tset;
  std::vector<vgl_point_3d<double> > pset;
  double mt = deformed_axis.max_t();
  for (double t = 0.0; t <= mt; t += this->cross_section_interval_) {
    vgl_point_3d<double> p = deformed_axis(t);
    vgl_plane_3d<double> norm_plane = deformed_axis.normal_plane(t);
    cs_ptsets.emplace_back();
    cs_planes.push_back(norm_plane);
    tset.push_back(t);
    pset.push_back(p);
  }
  // scan through the deformed pointset and assign each point to a cross section
  auto nc = static_cast<unsigned>(cs_ptsets.size());
  for (unsigned i = 0; i < npts; i++)
  {
    vgl_point_3d<double>  pi = def_ptset_all.p(i);
    vgl_vector_3d<double> ni = def_ptset_all.n(i);
    double min_dist = std::numeric_limits<double>::max();
    unsigned min_j = 0;
    double dp, dk, d;
    for (unsigned j = 0; j < nc; j++) {
      dp = vgl_distance(pi, cs_planes[j]);
      dk = vgl_distance(pi, pset[j]);
      d = dp + dk;
      if (d < min_dist) {
        min_dist = d;
        min_j = j;
      }
    }
    cs_ptsets[min_j].add_point_with_normal(pi, ni);
  }
  // construct new cross sections
  std::vector<bvgl_cross_section> def_cs;
  for (unsigned i = 0; i < nc; i++) {
    bvgl_cross_section cs(tset[i], pset[i], cs_planes[i], cs_ptsets[i]);
    def_cs.push_back(cs);
  }

  boxm2_vecf_orbicularis_oris target(deformed_axis, def_cs, this->cross_section_interval_);
  return target;
}

// deform the original orbicularis oris using deformation gradient defined in parameters
boxm2_vecf_orbicularis_oris boxm2_vecf_orbicularis_oris::deform() const
{
  // compute the stretch along principal axes
  double s1 = params_.lambda_;
  double s2 = std::pow(params_.lambda_, -params_.gamma_);
  double s3 = 1 / (s1*s2);
  // compute the rotation matrix
  vgl_vector_3d<double> l1_unit = params_.principal_eigenvector_1_;
  normalize(l1_unit);
  vgl_vector_3d<double> l2_unit = params_.principal_eigenvector_2_;
  normalize(l2_unit);
  vgl_vector_3d<double> l3_unit = cross_product(l1_unit, l2_unit);
  normalize(l3_unit);
  vnl_matrix_fixed<double,3,3> rot_matrix;
  this->rotation_matrix(l1_unit, l2_unit, l3_unit, rot_matrix);
  std::cout << "rotation matrix:\n" << rot_matrix;
  std::cout << "deformation along principal axes: " << s1 << ',' << s2 << ',' << s3 << std::endl;
  vgl_rotation_3d<double> rot(rot_matrix.transpose());
  vgl_rotation_3d<double> inv_rot(rot_matrix);
  // check if rotation matrix is identity
  bool no_rotation = rot_matrix.is_identity(1E-3);

  vgl_cubic_spline_3d<double> axis = this->axis();
  // compute local origin from pointset
  vgl_pointset_3d<double> ptsets_all = this->aggregate_pointset();
  vgl_point_3d<double> c_3d;
  double cx = 0.0, cy = 0.0, cz = 0.0;
  unsigned n_pts = ptsets_all.npts();
  for (unsigned i = 0; i < n_pts; i++) {
    vgl_point_3d<double> pt = ptsets_all.p(i);
    cx += pt.x();  cy += pt.y();  cz += pt.z();
  }
  cx /= n_pts;
  cy /= n_pts;
  cz /= n_pts;
  c_3d.set(cx, cy, cz);
  // apply deformation on axis
  std::vector<vgl_point_3d<double> > ori_pts = axis.knots();
  std::vector<vgl_point_3d<double> > deformed_knots;
  auto n_knots = static_cast<unsigned>(ori_pts.size());
  for (unsigned i = 0; i < n_knots; i++) {
    vgl_vector_3d<double> ori_lp = ori_pts[i] - c_3d;
    vgl_vector_3d<double> rot_lp = ori_lp;
    // rotate if necessary
    if (!no_rotation)
      rot_lp = rot*ori_lp;
    // stretch at principal direction
    double sx, sy, sz;
    sx = s1 * rot_lp.x();
    sy = s2 * rot_lp.y();
    sz = s3 * rot_lp.z();
    vgl_point_3d<double> deformed_lp(sx, sy, sz);
#if 0  // should not rotate it back, the rotation here is actual deformation, e.g., the desired oom is not fully symmetric along yz plane.
    // rotate it back if necessary
    if (!no_rotation)
      deformed_lp = inv_rot*stretched_lp;
#endif
    vgl_point_3d<double> def_p(deformed_lp.x()+c_3d.x(), deformed_lp.y()+c_3d.y(), deformed_lp.z()+c_3d.z());
    deformed_knots.push_back(def_p);
  }
  vgl_cubic_spline_3d<double> deformed_axis(deformed_knots);

  // deform all points in pointsets
  vgl_pointset_3d<double> ptsets_deform;
  for (unsigned i = 0; i < n_pts; i++) {
    // operate on point
    vgl_point_3d<double>  ori_p = ptsets_all.p(i);
    vgl_vector_3d<double> rot_lp = ori_p - c_3d;
    if (!no_rotation)
      rot_lp = rot*(ori_p - c_3d);
    double sx, sy, sz;
    sx = s1 * rot_lp.x();
    sy = s2 * rot_lp.y();
    sz = s3 * rot_lp.z();
    vgl_point_3d<double> deformed_lp(sx, sy, sz);
    vgl_point_3d<double> def_p(deformed_lp.x()+c_3d.x(), deformed_lp.y()+c_3d.y(), deformed_lp.z()+c_3d.z());
    // operate on surface normal
    vgl_vector_3d<double> ori_n = ptsets_all.n(i);
    vgl_vector_3d<double> rot_n = ori_n;
    if (!no_rotation)
      rot_n = rot*ori_n;
    double snx, sny, snz;
    snx = s1 * rot_n.x();
    sny = s2 * rot_n.y();
    snz = s3 * rot_n.z();
    vgl_vector_3d<double> def_n(snx, sny, snz);
    normalize(def_n);
    ptsets_deform.add_point_with_normal(def_p, def_n);
  }

  // create new cross section based on new axis
  std::vector<vgl_pointset_3d<double> > cs_ptsets;
  std::vector<vgl_plane_3d<double> > cross_planes;
  std::vector<double> tset;
  std::vector<vgl_point_3d<double> > pset;
  double mt = deformed_axis.max_t();
  for (double t = 0.0; t <= mt; t += this->cross_section_interval_) {
    vgl_point_3d<double> p = deformed_axis(t);
    vgl_plane_3d<double> norm_plane = deformed_axis.normal_plane(t);
    cs_ptsets.emplace_back();
    cross_planes.push_back(norm_plane);
    tset.push_back(t);
    pset.push_back(p);
  }

  // scan through the deformed pointset and assign each point to a cross section
  auto nc = static_cast<unsigned>(cs_ptsets.size());
  for (unsigned i = 0; i < n_pts; i++)
  {
    vgl_point_3d<double>  pi = ptsets_deform.p(i);
    vgl_vector_3d<double> ni = ptsets_deform.n(i);
    double min_dist = std::numeric_limits<double>::max();
    unsigned min_j = 0;
    double dp, dk, d;
    for (unsigned j = 0; j < nc; j++) {
      dp = vgl_distance(pi, cross_planes[j]);
      dk = vgl_distance(pi, pset[j]);
      d = dp + dk;
      if (d < min_dist) {
        min_dist = d;
        min_j = j;
      }
    }
    cs_ptsets[min_j].add_point_with_normal(pi, ni);
  }
  // construct new cross sections
  std::vector<bvgl_cross_section> deformed_cs;
  for (unsigned i = 0; i < nc; i++) {
    bvgl_cross_section cs(tset[i], pset[i], cross_planes[i], cs_ptsets[i]);
    deformed_cs.push_back(cs);
  }

  // return deformed orbicularis oris
  boxm2_vecf_orbicularis_oris target(deformed_axis, deformed_cs, this->cross_section_interval_);
  target.set_params(this->params_);
  std::cout << "number of deformed point set: " << ptsets_deform.npts() << std::endl;
  return target;
}

boxm2_vecf_orbicularis_oris boxm2_vecf_orbicularis_oris::circular_deform() const
{
  // compute the stretch along principal axes
  double s1 = params_.lambda_;
  double s2 = std::pow(params_.lambda_, -params_.gamma_);
  double s3 = 1 / (s1*s2);
  // compute the ankle point
  double ankle_dist = 25.0;  // experimental setting
  vgl_point_3d<double> c_3d;
  vgl_point_3d<double> ankle_pt;
  vgl_pointset_3d<double> ptsets_all = this->aggregate_pointset();
  unsigned npts = ptsets_all.npts();
  double cx = 0.0, cy = 0.0, cz = 0.0;
  double max_z = std::numeric_limits<double>::min();
  for (unsigned i = 0; i < npts; i++) {
    vgl_point_3d<double> pt = ptsets_all.p(i);
    cx += pt.x();  cy += pt.y();  cz += pt.z();
    if (pt.z() > max_z)
      max_z = pt.z();
  }
  cx /= npts;
  cy /= npts;
  cz /= npts;
  c_3d.set(cx, cy, max_z);
  ankle_pt.set(cx, cy, max_z + ankle_dist);

  // deform the axis
  // at each knot, the principal contraction axis, l1, follows the vector from the knot to the ankle point
  // the other two principal axes are define on the plane that perpendicular to l1
  // l2 = cross_product(e3, l1) --> l2 will stay close to e2
  // l3 = cross_product(l1, l2)
  std::cout << "deformation along principal axes: " << s1 << ',' << s2 << ',' << s3 << std::endl;
  std::cout << "deformation origin: " << c_3d << std::endl;
  std::cout << "ankle point: " << ankle_pt << std::endl;
  vgl_vector_3d<double> e1(1.0, 0.0, 0.0);
  vgl_vector_3d<double> e2(0.0, 1.0, 0.0);
  vgl_vector_3d<double> e3(0.0, 0.0, 1.0);
  std::vector<vgl_point_3d<double> > deformed_knots;
  vgl_cubic_spline_3d<double> ori_axis = this->axis();
  double n_knots = ori_axis.n_knots();
  for (double t = 0.0; t < n_knots; t+=1.0)
  {
    vgl_point_3d<double> ori_p = ori_axis(t);
    // define the principal axes
    vgl_vector_3d<double> l1 = ankle_pt - ori_p;
    normalize(l1);
    vgl_vector_3d<double> l2 = normalized(cross_product(e3, l1));
    vgl_vector_3d<double> l3 = normalized(cross_product(l1, l2));
    // compute the rotation matrix
    vnl_matrix_fixed<double, 3, 3> rot_matrix;
    this->rotation_matrix(l1, l2, l3, rot_matrix);
    vgl_rotation_3d<double> rot(rot_matrix);
    vgl_rotation_3d<double> inv_rot(rot_matrix.transpose());
    // deform the point
    vgl_vector_3d<double> ori_lp = ori_p - c_3d;
    vgl_vector_3d<double> rot_lp = rot*ori_lp;
    // stretch along principal axes
    double sx, sy, sz;
    sx = s1 * rot_lp.x();
    sy = s2 * rot_lp.y();
    sz = s3 * rot_lp.z();
    vgl_point_3d<double> deformed_lp(sx, sy, sz);
    // rotate back
    vgl_point_3d<double> inv_rot_p = inv_rot * deformed_lp;
    vgl_point_3d<double> deformed_p(inv_rot_p.x() + c_3d.x(), inv_rot_p.y() + c_3d.y(), inv_rot_p.z() + c_3d.z());
    deformed_knots.push_back(deformed_p);
#if 0
    std::cout << "************\n";
    std::cout << "t: " << t << " -- "
             << "sx: " << sx << ", sy: " << sy << ", sz: " << sz << " -- "
             << "original_point: " << ori_p.x() << "," << ori_p.y() << "," << ori_p.z() << " -- "
             << "rotated_local : " << rot_lp.x() << "," << rot_lp.y() << "," << rot_lp.z() << " -- "
             << "deformed_point: " << deformed_p.x() << "," << deformed_p.y() << "," << deformed_p.z()
             << std::endl;
#endif
  }
  vgl_cubic_spline_3d<double> deformed_axis(deformed_knots);

  // deform all points
  vgl_pointset_3d<double> def_ptset_all;
  for (unsigned i = 0; i < npts; i++) {
    vgl_point_3d<double>  ori_p = ptsets_all.p(i);
    vgl_vector_3d<double> ori_n = ptsets_all.n(i);
    // compute the rotation matrix
    vgl_vector_3d<double> l1 = ankle_pt - ori_p;
    normalize(l1);
    vgl_vector_3d<double> l2 = normalized(cross_product(e3, l1));
    vgl_vector_3d<double> l3 = normalized(cross_product(l1, l2));
    // compute the rotation matrix
    vnl_matrix_fixed<double, 3, 3> rot_matrix;
    this->rotation_matrix(l1, l2, l3, rot_matrix);
    vgl_rotation_3d<double> rot(rot_matrix);
    vgl_rotation_3d<double> inv_rot(rot_matrix.transpose());
    // deform the point
    vgl_vector_3d<double> ori_lp = ori_p - c_3d;
    vgl_vector_3d<double> rot_lp = rot*ori_lp;
    double sx, sy, sz;
    sx = s1 * rot_lp.x();
    sy = s2 * rot_lp.y();
    sz = s3 * rot_lp.z();
    vgl_point_3d<double> deformed_lp(sx, sy, sz);
    // rotate back
    vgl_point_3d<double> inv_rot_p = inv_rot * deformed_lp;
    vgl_point_3d<double> def_p(inv_rot_p.x() + c_3d.x(), inv_rot_p.y() + c_3d.y(), inv_rot_p.z() + c_3d.z());
    // deform the normal direction
    vgl_vector_3d<double> rot_n = rot*ori_n;
    double snx, sny, snz;
    snx = s1 * rot_n.x();  sny = s2 * rot_n.y();  snz = s3 * rot_n.z();
    // rotate it back
    vgl_vector_3d<double> def_ln(snx, sny, snz);
    vgl_vector_3d<double> inv_n = inv_rot*def_ln;
    vgl_vector_3d<double> def_n(inv_n.x() + c_3d.x(), inv_n.y() + c_3d.y(), inv_n.z() + c_3d.z());
    normalize(def_n);
    def_ptset_all.add_point_with_normal(def_p, def_n);
  }
  // create new cross section
  std::vector<vgl_pointset_3d<double> > cs_ptsets;
  std::vector<vgl_plane_3d<double> > cs_planes;
  std::vector<double> tset;
  std::vector<vgl_point_3d<double> > pset;
  double mt = deformed_axis.max_t();
  for (double t = 0.0; t <= mt; t += this->cross_section_interval_) {
    vgl_point_3d<double> p = deformed_axis(t);
    vgl_plane_3d<double> norm_plane = deformed_axis.normal_plane(t);
    cs_ptsets.emplace_back();
    cs_planes.push_back(norm_plane);
    tset.push_back(t);
    pset.push_back(p);
  }
  // scan through the deformed pointset and assign each point to a cross section
  auto nc = static_cast<unsigned>(cs_ptsets.size());
  for (unsigned i = 0; i < npts; i++)
  {
    vgl_point_3d<double>  pi = def_ptset_all.p(i);
    vgl_vector_3d<double> ni = def_ptset_all.n(i);
    double min_dist = std::numeric_limits<double>::max();
    unsigned min_j = 0;
    double dp, dk, d;
    for (unsigned j = 0; j < nc; j++) {
      dp = vgl_distance(pi, cs_planes[j]);
      dk = vgl_distance(pi, pset[j]);
      d = dp + dk;
      if (d < min_dist) {
        min_dist = d;
        min_j = j;
      }
    }
    cs_ptsets[min_j].add_point_with_normal(pi, ni);
  }
  // construct new cross sections
  std::vector<bvgl_cross_section> def_cs;
  for (unsigned i = 0; i < nc; i++) {
    bvgl_cross_section cs(tset[i], pset[i], cs_planes[i], cs_ptsets[i]);
    def_cs.push_back(cs);
  }

  boxm2_vecf_orbicularis_oris target(deformed_axis, def_cs, this->cross_section_interval_);
  return target;
}

bool boxm2_vecf_orbicularis_oris::inverse_vector_field(vgl_point_3d<double> const&  /*target_pt*/, vgl_vector_3d<double>&  /*inv_vf*/) const
{
  return true;
}

// display method
// write out the axis spline into ascii file, along with their tangent
void boxm2_vecf_orbicularis_oris::display_axis_spline_ascii(std::ofstream& ostr, unsigned const& r, unsigned const& g, unsigned const& b) const
{
  double nd = this->axis().max_t();
  for (double t = 0; t < nd; t+=0.2) {
    vgl_point_3d<double> p = this->axis()(t);
    vgl_vector_3d<double> n = this->axis().tangent(t);
    ostr << std::setprecision(10) << p.x() << ',' << std::setprecision(10) << p.y() << ',' << std::setprecision(10) << p.z() << ','
         << std::setprecision(10) << n.x() << ',' << std::setprecision(10) << n.y() << ',' << std::setprecision(10) << n.z() << ','
         << r << ',' << g << ',' << b << '\n';
  }
  ostr.close();
  return;
}

void boxm2_vecf_orbicularis_oris::display_cross_section_pointsets_ascii(std::ofstream& ostr, unsigned const& r, unsigned const& g, unsigned const& b) const
{
  vgl_pointset_3d<double> ptsets = this->aggregate_pointset();
  unsigned n_pts = ptsets.npts();
  for (unsigned i = 0; i < n_pts; i++) {
    vgl_point_3d<double>  p = ptsets.p(i);
    vgl_vector_3d<double> n = ptsets.n(i);
    ostr << std::setprecision(10) << p.x() << ',' << std::setprecision(10) << p.y() << ',' << std::setprecision(10) << p.z() << ','
         << std::setprecision(10) << n.x() << ',' << std::setprecision(10) << n.y() << ',' << std::setprecision(10) << n.z() << ','
         << r << ',' << g << ',' << b << '\n';
  }
  ostr.close();
  return;
}
