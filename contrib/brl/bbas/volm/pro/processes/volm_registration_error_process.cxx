// This is brl/bbas/volm/pro/processes/volm_registration_error_process.cxx
#include <iostream>
#include <ios>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Process to evaluate the projection improvement using OSM road
//
// \author Yi Dong
// \date March 10, 2015
// \verbatim
//  Modifications
//
// \endverbatim
//

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_vector_io.h>
#include <vsol/vsol_spatial_object.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_curve_2d_sptr.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <bsol/bsol_algs.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_closest_point.h>

//: Take pre-created projection geometry (vsol binary file) to compute the deviation from ground truth
namespace volm_registration_error_process_globals
{
  constexpr unsigned n_inputs_ = 6;
  constexpr unsigned n_outputs_ = 4;

  //: return the closest, along with the normal components, from a point to a line segment
  double closest_distance(vsol_point_2d_sptr const& p, std::vector<vsol_point_2d_sptr> const& point_set,
                          double& vec_x, double& vec_y);

  double closest_distance(std::vector<vsol_point_2d_sptr> const& p_set1, std::vector<vsol_point_2d_sptr> const& p_set2,
                          std::vector<std::vector<double> >& vec_dist);
}

bool volm_registration_error_process_cons(bprb_func_process& pro)
{
  using namespace volm_registration_error_process_globals;

  std::vector<std::string> input_types(n_inputs_);
  input_types[0] = "vcl_string";    // ground truth file
  input_types[1] = "vcl_string";    // projection file using geo-registered camera
  input_types[2] = "vcl_string";    // projection file using original camera
  input_types[3] = "double";;       // image gsd
  input_types[4] = "vcl_string";    // output file to store the shift vectors from geo-registered camera
  input_types[5] = "vcl_string";    // output file to store the shift vectors from original camera
  std::vector<std::string> output_types(n_outputs_);
  output_types[0] = "double";      // average shift of geo-registered camera
  output_types[1] = "double";      // standard deviation of shift using geo-registered camera
  output_types[2] = "double";      // average shift of original camera
  output_types[3] = "double";      // standard deviation of shift using original camera
  return pro.set_input_types(input_types) && pro.set_output_types(output_types);
}

bool volm_registration_error_process(bprb_func_process& pro)
{
  using namespace volm_registration_error_process_globals;

  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong Inputs!!!\n";
    return false;
  }
  vsl_add_to_binary_loader(vsol_polyline_2d());

  // get the inputs
  unsigned in_i = 0;
  std::string gt_file = pro.get_input<std::string>(in_i++);
  std::string cor_file = pro.get_input<std::string>(in_i++);
  std::string ori_file = pro.get_input<std::string>(in_i++);
  auto res = pro.get_input<double>(in_i++);
  std::string cor_out_file = pro.get_input<std::string>(in_i++);
  std::string ori_out_file = pro.get_input<std::string>(in_i++);

  // load the geometry
  if (!vul_file::exists(gt_file)) {
    std::cerr << pro.name() << ": can not find file: " << gt_file << std::endl;
    return false;
  }
  if (!vul_file::exists(cor_file)) {
    std::cerr << pro.name() << ": can not find file: " << cor_file << std::endl;
    return false;
  }
  if (!vul_file::exists(ori_file)) {
    std::cerr << pro.name() << ": can not find file: " << ori_file << std::endl;
    return false;
  }
  std::vector<vsol_spatial_object_2d_sptr> gt_sos_in;
  std::vector<vsol_spatial_object_2d_sptr> cor_sos_in;
  std::vector<vsol_spatial_object_2d_sptr> ori_sos_in;
  vsl_b_ifstream istr_gt(gt_file);
  if (! !istr_gt) {
    vsl_b_read(istr_gt, gt_sos_in);
    istr_gt.close();
  }
  vsl_b_ifstream istr_cor(cor_file);
  if (! !istr_cor) {
    vsl_b_read(istr_cor, cor_sos_in);
    istr_cor.close();
  }
  vsl_b_ifstream istr_ori(ori_file);
  if (! !istr_ori) {
    vsl_b_read(istr_ori, ori_sos_in);
    istr_ori.close();
  }
  //gt_sos_in.erase(gt_sos_in.begin()+148);
  //cor_sos_in.erase(cor_sos_in.begin()+148);
  if ( gt_sos_in.size() != cor_sos_in.size() || gt_sos_in.size() != ori_sos_in.size() || cor_sos_in.size() != ori_sos_in.size()) {
    std::cerr << pro.name() << ": inconsistency in between loaded projection geometries!!!\n";
    return false;
  }

  // cast to poly line
  std::vector<vsol_polyline_2d_sptr> gt_lines;
  std::vector<vsol_polyline_2d_sptr> cor_lines;
  std::vector<vsol_polyline_2d_sptr> ori_lines;
  std::vector<vsol_spatial_object_2d_sptr>::iterator vit;
  for (vit = gt_sos_in.begin(); vit != gt_sos_in.end(); ++vit)
    gt_lines.emplace_back((*vit)->cast_to_curve()->cast_to_polyline());
  for (vit = cor_sos_in.begin(); vit != cor_sos_in.end(); ++vit)
    cor_lines.emplace_back((*vit)->cast_to_curve()->cast_to_polyline());
  for (vit = ori_sos_in.begin(); vit != ori_sos_in.end(); ++vit)
    ori_lines.emplace_back((*vit)->cast_to_curve()->cast_to_polyline());

  // calculate the distance for each line
  unsigned n_lines = gt_lines.size();
  std::vector<std::vector<std::vector<double> > > cor_vectors;
  std::vector<std::vector<std::vector<double> > > ori_vectors;
  std::vector<double> cor_distance;
  std::vector<double> ori_distance;
  for (unsigned i = 0; i < n_lines; i++)
  {
    std::vector<std::vector<double> > cor_line_vector;
    std::vector<std::vector<double> > ori_line_vector;

    std::vector<vsol_point_2d_sptr> cor_points;
    std::vector<vsol_point_2d_sptr> ori_points;
    std::vector<vsol_point_2d_sptr> gt_points;
    for (unsigned gp = 0; gp < gt_lines[i]->size(); gp++) {
      gt_points.push_back(gt_lines[i]->vertex(gp));
      cor_points.push_back(cor_lines[i]->vertex(gp));
      ori_points.push_back(ori_lines[i]->vertex(gp));
    }
    double cd = closest_distance(cor_points, gt_points, cor_line_vector);
    double od = closest_distance(ori_points, gt_points, ori_line_vector);
    cd *= res;
    od *= res;
    std::cout << "line " << i << " has " << cor_line_vector.size() << " points and average distance is " << cd << " (correction) and "
             << od << " (original) " << std::endl;

#if 0
    vsol_polyline_2d_sptr cor_line = cor_lines[i];
    vsol_polyline_2d_sptr ori_line = ori_lines[i];
    // calculate the shift from corrected projection to ground truth
    unsigned n_cpts = cor_line->size();
    double cd = 0;
    for (unsigned cp = 0; cp < n_cpts; cp++) {
      if (i == 4 && cp == 2)
        int tmp = 1;
      double dist = 0, vec_x = 0, vec_y = 0;
      dist = closest_distance(cor_line->vertex(cp), gt_points, vec_x, vec_y);
      cd += dist;
      std::vector<double> pt_vect;
      pt_vect.push_back(vec_x);  pt_vect.push_back(vec_y);  pt_vect.push_back(dist);
      cor_line_vector.push_back(pt_vect);
    }
    cd /= n_cpts;
    cd *= res;
    // calculate the shift from original projection to ground truth
    unsigned n_opts = ori_line->size();
    double od = 0;
    for (unsigned op = 0; op < n_opts; op++) {
      double dist = 0, vec_x = 0, vec_y = 0;
      dist = closest_distance(ori_line->vertex(op), gt_points, vec_x, vec_y);
      od += dist;
      std::vector<double> pt_vect;
      pt_vect.push_back(vec_x);  pt_vect.push_back(vec_y), pt_vect.push_back(dist);
      ori_line_vector.push_back(pt_vect);
    }
    od /= n_opts;
    od *= res;
#endif
    // scale by the gsd resolution
    cor_distance.push_back(cd);
    ori_distance.push_back(od);
    cor_vectors.push_back(cor_line_vector);
    ori_vectors.push_back(ori_line_vector);
  }
  // calculate statistics
  double sum_cor = 0.0, sumsq_cor = 0.0;
  double sum_ori = 0.0, sumsq_ori = 0.0;
  for (unsigned i = 0; i < n_lines; i++)
  {
    std::cout << "line: " << i << ", cor_d = " << cor_distance[i] << " meter, ori_di = " << ori_distance[i] << " meter" << std::endl;
    sum_cor += cor_distance[i];
    sumsq_cor += cor_distance[i]*cor_distance[i];
    sum_ori += ori_distance[i];
    sumsq_ori += ori_distance[i]*ori_distance[i];
  }
  double cor_mean, cor_std, ori_mean, ori_std;
  cor_mean = sum_cor / n_lines;
  ori_mean = sum_ori / n_lines;

  cor_std = std::sqrt((sumsq_cor - cor_mean*cor_mean*n_lines) / (n_lines-1));
  ori_std = std::sqrt((sumsq_ori - ori_mean*ori_mean*n_lines) / (n_lines-1));

  // write out the distance vectors

  if (cor_out_file.compare("") != 0)
  {
    std::cout << "write correction result into " << cor_out_file << std::endl;
    std::ofstream ofs(cor_out_file.c_str());
    ofs << "line_id    x    y    distance\n";
    ofs.setf(std::ios::left);
    ofs.precision(5);
    ofs.setf(std::ios::showpoint);
    for (unsigned i = 0; i < n_lines; i++)
      for (unsigned p = 0; p < cor_vectors[i].size(); p++)
        ofs << i << ' ' << cor_vectors[i][p][0] << ' ' << cor_vectors[i][p][1] << ' ' << cor_vectors[i][p][2] << '\n';
    ofs.close();
  }
  if (ori_out_file.compare("") != 0)
  {
    std::cout << "write original result into " << cor_out_file << std::endl;
    std::ofstream ofs(ori_out_file.c_str());
    ofs << "line_id    x    y    distance\n";
    ofs.setf(std::ios::left);
    ofs.precision(5);
    for (unsigned i = 0; i < n_lines; i++)
      for (unsigned p = 0; p < ori_vectors[i].size(); p++)
        ofs << i << ' ' << ori_vectors[i][p][0] << ' ' << ori_vectors[i][p][1] << ' ' << ori_vectors[i][p][2] << '\n';
    ofs.close();
  }

  int out_i = 0;
  pro.set_output_val<double>(out_i++, cor_mean);
  pro.set_output_val<double>(out_i++, cor_std);
  pro.set_output_val<double>(out_i++, ori_mean);
  pro.set_output_val<double>(out_i++, ori_std);
  return true;
}

// return the closest, along with the normal components, from a point to a line segment
double volm_registration_error_process_globals::closest_distance(vsol_point_2d_sptr const& p, std::vector<vsol_point_2d_sptr> const& point_set,
                                                                 double& vec_x, double& vec_y)
{
  double d = 0.0;
  // find the closest points
  vsol_point_2d_sptr cp = bsol_algs::closest_point(p, point_set, d);
  // locate the neighbor points
  unsigned cid = 0;
  unsigned n_points = point_set.size();
  bool found = false;
  for (unsigned i = 0; (i < n_points && !found); i++) {
    if (cp == point_set[i])
      cid = i;
  }
  if (cid == 0)
  {
    vsol_point_2d_sptr cp_next = point_set[cid+1];
    //d = vgl_distance_to_linesegment(cp->x(), cp->y(), cp_next->x(), cp_next->y(), p->x(), p->y());
    // obtain the normal vector
    double ret_x, ret_y;
    vgl_closest_point_to_linesegment(ret_x, ret_y, cp->x(), cp->y(), cp_next->x(), cp_next->y(), p->x(), p->y());
    vgl_vector_2d<double> vect = vgl_point_2d<double>(ret_x, ret_y) - vgl_point_2d<double>(p->x(), p->y());
    d = vect.length();
    vec_x = vect.x();
    vec_y = vect.y();
  }
  else if (cid == n_points-1)
  {
    vsol_point_2d_sptr cp_prev = point_set[cid-1];
    //d = vgl_distance_to_linesegment(cp_prev->x(), cp_prev->y(), cp->x(), cp->y(), p->x(), p->y());
    double ret_x, ret_y;
    vgl_closest_point_to_linesegment(ret_x, ret_y, cp_prev->x(), cp_prev->y(), cp->x(), cp->y(), p->x(), p->y());
    vgl_vector_2d<double> vect = vgl_point_2d<double>(ret_x, ret_y) - vgl_point_2d<double>(p->x(), p->y());
    d = vect.length();
    vec_x = vect.x();
    vec_y = vect.y();
  }
  else {
    vsol_point_2d_sptr cp_prev = point_set[cid-1];
    vsol_point_2d_sptr cp_next = point_set[cid+1];
    double ret_x_prev, ret_y_prev, ret_x_next, ret_y_next;
    vgl_closest_point_to_linesegment(ret_x_prev, ret_y_prev, cp_prev->x(), cp_prev->y(), cp->x(), cp->y(), p->x(), p->y());
    vgl_closest_point_to_linesegment(ret_x_next, ret_y_next, cp->x(), cp->y(), cp_next->x(), cp_next->y(), p->x(), p->y());
    vgl_vector_2d<double> vect_prev = vgl_point_2d<double>(ret_x_prev, ret_y_prev) - vgl_point_2d<double>(p->x(), p->y());
    vgl_vector_2d<double> vect_next = vgl_point_2d<double>(ret_x_next, ret_y_next) - vgl_point_2d<double>(p->x(), p->y());
    if (vect_prev.length() <= vect_next.length()) {
      d = vect_prev.length();
      vec_x = vect_prev.x();
      vec_y = vect_next.y();
    }
    else {
      d = vect_next.length();
      vec_x = vect_next.x();
      vec_y = vect_next.y();
    }
  }
  return d;
}

double volm_registration_error_process_globals::closest_distance(std::vector<vsol_point_2d_sptr> const& p_set1, std::vector<vsol_point_2d_sptr> const& p_set2,
                                                                 std::vector<std::vector<double> >& vec_dist)
{
  double average_dist = 0;
  vec_dist.clear();
  if (p_set1.size() != p_set2.size()) {
    return average_dist;
  }
  auto n_pts = (unsigned)p_set1.size();
  for (unsigned i = 0; i < n_pts; i++) {
    double dx = p_set1[i]->x() - p_set2[i]->x();
    double dy = p_set1[i]->y() - p_set2[i]->y();
    double dist = std::sqrt( dx*dx + dy*dy);
    std::vector<double> pt_vec;
    pt_vec.push_back(dx);
    pt_vec.push_back(dy);
    pt_vec.push_back(dist);
    vec_dist.push_back(pt_vec);
    average_dist += dist;
  }
  average_dist /= n_pts;
  return average_dist;
}
