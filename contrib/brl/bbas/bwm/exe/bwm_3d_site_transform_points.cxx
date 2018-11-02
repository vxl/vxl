//:
// \file
#include <vector>
#include <set>
#include <iostream>
#include <fstream>
#include <string>
#include <bwm/bwm_observer_cam.h>
#include <bwm/bwm_observer_mgr.h>
#include <bwm/bwm_3d_corr.h>
#include <bwm/bwm_3d_corr_sptr.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vpgl/algo/vpgl_ortho_procrustes.h>

#include <rply.h>   //.ply parser


//helper class to read in bb from file
class ply_points_reader
{
 public:
  std::vector<vnl_vector_fixed<double,3> > all_points;
  vnl_vector_fixed<double,3> p;
  std::vector<int > vertex_indices;
};

//: Call-back function for a "vertex" element
int plyio_vertex_cb(p_ply_argument argument)
{
  long index;
  void* temp;
  ply_get_argument_user_data(argument, &temp, &index);

  ply_points_reader* parsed_ply =  (ply_points_reader*) temp;

  switch (index)
  {
    case 0: // "x" coordinate
      parsed_ply->p[0] = ply_get_argument_value(argument);
      break;
    case 1: // "y" coordinate
      parsed_ply->p[1] = ply_get_argument_value(argument);
      break;
    case 2: // "z" coordinate
     parsed_ply->p[2] = ply_get_argument_value(argument);
     // Instert into vector
     parsed_ply->all_points.push_back(parsed_ply->p);
     break;
    default:
      assert(!"This should not happen: index out of range");
  }
  return 1;
}


void readPointsFromPLY(const std::string& filename, std::vector<vnl_vector_fixed<double,3> > &all_points)
{
  ply_points_reader parsed_ply;
  parsed_ply.all_points = all_points;

  p_ply ply = ply_open(filename.c_str(), nullptr, 0, nullptr);
  if (!ply) {
    std::cout << "File " << filename << " doesn't exist.";
  }
  if (!ply_read_header(ply))
    std::cout << "File " << filename << " doesn't have header.";

  // vertex
  int  nvertices =
  ply_set_read_cb(ply, "vertex", "x", plyio_vertex_cb, (void*) (&parsed_ply), 0);
  ply_set_read_cb(ply, "vertex", "y", plyio_vertex_cb, (void*) (&parsed_ply), 1);
  ply_set_read_cb(ply, "vertex", "z", plyio_vertex_cb, (void*) (&parsed_ply), 2);

  std::cerr << nvertices << " points\n";

  // Read DATA
  ply_read(ply);

  // CLOSE file
  ply_close(ply);

  all_points=parsed_ply.all_points;
}


//: Write points to a PLY file
void writePointsToPLY(const std::string& ply_file_out, std::vector<vnl_vector_fixed<double,3> > &all_points)
{
    // OPEN output file
  p_ply oply = ply_create(ply_file_out.c_str(), PLY_ASCII, nullptr, 0, nullptr);

  std::cerr << "  saving " << ply_file_out << " :\n";

  // HEADER SECTION
  // vertex
  ply_add_element(oply, "vertex", all_points.size());
  ply_add_scalar_property(oply, "x", PLY_DOUBLE); //PLY_FLOAT
  ply_add_scalar_property(oply, "y", PLY_DOUBLE); //PLY_FLOAT
  ply_add_scalar_property(oply, "z", PLY_DOUBLE); //PLY_FLOAT
  // comment
  ply_add_comment(oply, "created by bwm_transform_site_3d");
  // object info
  ply_add_obj_info(oply, "a vector of vnl_vector_fixed<double,3> object");
  // end header
  ply_write_header(oply);

  // DATA SECTION
  // save min and max boint of the box to ply file
  for (unsigned pi=0; pi<all_points.size(); ++pi) {
    vnl_vector_fixed<double,3> p = all_points[pi];
    ply_write(oply, p[0]);
    ply_write(oply, p[1]);
    ply_write(oply, p[2]);
  }
 // CLOSE PLY FILE
  ply_close(oply);
}

// the resulting similarity maps from the coordinate frame of pts1
// to the coordinate frame of pts0
static bool compute_similarity(vnl_matrix<double> const& pts0,
                               vnl_matrix<double> const& pts1,
                               vgl_rotation_3d<double>& R,
                               vnl_vector_fixed<double, 3>& t,
                               double& scale)
{
  vpgl_ortho_procrustes op(pts0, pts1);
  R = op.R();
  t = op.t();
  scale = op.s();
  if (! op.compute_ok()) return false;
  std::cout << "Ortho procrustes error "
           << std::sqrt(op.residual_mean_sq_error()) << '\n';
  return true;
}

// this executable finds a similarity transform, given a set of corresponding
// 3-d points. The similiarity transform is then applied to a directory of
// PLY files containing lists of 3d points that are transformed to the new coordinate system
int main(int argc, char** argv)
{
  //Get Inputs

  vul_arg<std::string> corrs_path   ("-corrs", "corr input file",  "");

  vul_arg<std::string> input_point_dir ("-in_point_dir","directory to get .ply files containing the points","");
  vul_arg<std::string> output_point_dir ("-out_point_dir","directory to store transformed points", "");

  if (argc != 7) {
    std::cout << "usage: bwm_3d_site_transform -corrs <corr file> -in_point_dir <dir> -out_point_dir <dir>\n";
    return -1;
  }

  vul_arg_parse(argc, argv);

  // verify input camera dir
  if (!vul_file::is_directory(input_point_dir().c_str()))
  {
    std::cout<<"Input directory does not exist"<<std::endl;
    return -1;
  }

  // verify output camera dir
  if (!vul_file::is_directory(output_point_dir().c_str()))
  {
    std::cout<<"Output directory does not exist"<<std::endl;
    return -1;
  }

  std::vector<bwm_3d_corr_sptr> corrs;
  bwm_observer_mgr::load_3d_corrs(corrs_path(), corrs);
  // assume correspondences between two sites only
  unsigned n = corrs.size();
  vnl_matrix<double> pts0(3,n), pts1(3,n);
  for (unsigned i = 0; i<n; ++i) {
    std::cout << *(corrs[i]);
    std::vector<vgl_point_3d<double> > match_pts = corrs[i]->matching_pts();
    pts0[0][i] = match_pts[0].x();  pts1[0][i] = match_pts[1].x();
    pts0[1][i] = match_pts[0].y();  pts1[1][i] = match_pts[1].y();
    pts0[2][i] = match_pts[0].z();  pts1[2][i] = match_pts[1].z();
  }
  vgl_rotation_3d<double> R;
  vnl_vector_fixed<double, 3> t;
  double scale;
  if (!compute_similarity(pts1, pts0, R, t, scale)) {
    std::cout << "similarity computation failed\n";
    return -1;
  }
  std::cout << "scale = " << scale << "\nR = " << R << "\nt = " << t << '\n';
  //transform the points
  std::string in_dir = input_point_dir() + "/*.ply";
  for (vul_file_iterator fn = in_dir.c_str(); fn; ++fn) {
    std::string f = fn();
    std::vector<vnl_vector_fixed<double,3> > points2transform;
    std::vector<vnl_vector_fixed<double,3> > transformed_points;
    readPointsFromPLY(f, points2transform);
    for (unsigned pi = 0; pi < points2transform.size(); ++pi) {
      vnl_vector_fixed<double, 3> new_p = scale*(R * (points2transform[pi])+ t);
      transformed_points.push_back(new_p);
    }
    std::cout << "Transformed Poins: " << points2transform.size() << '\n';
    std::string fname = vul_file::strip_directory(f.c_str());
    std::cout << fname << '\n';
    std::string out_dir = output_point_dir() + "/";
    std::string out_file = out_dir + fname;
    writePointsToPLY(out_file, transformed_points);
  }
  return 0;
}
