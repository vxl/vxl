//This is brl/bbas/bvrml/pro/processes/bvrml_ply_processes.cxx
#include <string>
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file
//
#include <bprb/bprb_parameters.h>

#include <cassert>
#ifdef DEBUG
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#endif

#include <brdb/brdb_value.h>
#include <bvrml/bvrml_write.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_sphere_3d.h>
#include <vul/vul_awk.h>

#include <rply.h>   //.ply parser

class bvrml_point_cov
{
 public:
  float p_[3];
  float axes_[3];
  float LE_;
  float CE_;
  float angle_;
  float prob_;
};

typedef std::pair<vgl_point_3d<float>, std::vector<bvrml_point_cov> > point_pair;

class bvrml_filter_fileio_parsed_ply
{
 public:
  float p[3];  //holds location
  float prob;
  float angle; //holds color
  float axes[3];
  float LE;
  float CE;
  float dist_thres;

  //accumulated data
  std::vector<point_pair> data;
};

bool bvrml_load_points_ply(const std::string &ply_file, float dist_thres,
                           std::vector<point_pair> &data);

//: Call-back function for a "vertex" element
int bvrml_plyio_vertex_cb_(p_ply_argument argument);


//: sets input and output types
bool bvrml_filtered_ply_process_cons(bprb_func_process& pro)
{
  //inputs
  std::vector<std::string> input_types_(5);
  input_types_[0] = "vcl_string";  // vrml file name
  input_types_[1] = "vcl_string";  // ply file
  input_types_[2] = "vcl_string";  // the point coordinates to be used to filter points in the ply file
  input_types_[3] = "float"; // distance threshold for proximity based filtering
  input_types_[4] = "bool"; // pass true if just to display nearest

  //output
  std::vector<std::string> output_types_(0);

  bool good = pro.set_input_types(input_types_) &&
              pro.set_output_types(output_types_);
  return good;
}

bool bvrml_filtered_ply_process(bprb_func_process& pro)
{
  // check number of inputs
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << ": Invalid inputs" << std::endl;
    return false;
  }

  std::string fname = pro.get_input<std::string>(0);
  std::string ply_file = pro.get_input<std::string>(1);
  std::string point_file = pro.get_input<std::string>(2);
  auto dist_thres = pro.get_input<float>(3);
  bool nearest = pro.get_input<bool>(4);

  std::ofstream ofs(fname.c_str(), std::ios::app);

  std::vector<point_pair> data;

  std::ifstream ifs(point_file.c_str());
  vul_awk awk(ifs);
  for (; awk; ++awk)
  {
    std::string pt_line = awk.line();
    std::stringstream ss(pt_line);
    float x,y,z;
    ss >> x; ss >> y; ss >> z;
    std::cout << "read: " << x << ' ' << y << ' ' << z << '\n';
    vgl_point_3d<float> pt(x,y,z);
    std::vector<bvrml_point_cov> tmp;
    data.emplace_back(pt, tmp);
  }
  //: now filter the ply points while reading them
  if (!bvrml_load_points_ply(ply_file, dist_thres, data))
    return false;

  double avg_dist = 0.0;
  double avg_LE = 0.0;
  double avg_CE = 0.0;
  unsigned cnt = 0;
  //: now write the filtered points to vrml
  for (auto & i : data) {
    vgl_point_3d<float> pt = i.first;
    std::vector<bvrml_point_cov> fpts = i.second;
    //vgl_sphere_3d<float> sp(pt.x(), pt.y(), pt.z(), 0.5f);
    vgl_sphere_3d<float> sp(pt.x(), pt.y(), pt.z(), 0.1f);
    bvrml_write::write_vrml_sphere(ofs, sp, 1.0, 0.0, 0.0, 0.0f);
    std::vector<bvrml_point_cov> fpts_to_write;
    if (nearest) {
      double dist_min = 10000000.0;
      bvrml_point_cov pc_min;
      vgl_point_3d<float> pt_min;
      for (auto pc : fpts) {
        vgl_point_3d<float> pt_f(pc.p_[0], pc.p_[1], pc.p_[2]);
        vgl_vector_3d<float> diff_vec = pt_f-pt;
        double dist = diff_vec.length();
        if (dist < dist_min) {
          dist_min = dist;
          pc_min = pc;
          pt_min = pt_f;
        }
      }
      fpts_to_write.push_back(pc_min);
      avg_dist += dist_min;
      avg_LE += pc_min.LE_;
      avg_CE += pc_min.CE_;
      ++cnt;
      std::cout << "pt: " << pt << std::endl
               << "nearest pt: " << pt_min << std::endl
               << "dist min: " << dist_min << std::endl;
    }
    else
      fpts_to_write = fpts;
    for (auto pc : fpts_to_write) {
      vgl_sphere_3d<float> fsp(pc.p_[0], pc.p_[1], pc.p_[2], 0.1f);
      bvrml_write::write_vrml_sphere(ofs, fsp, 0.0, 0.0, 1.0, 0.0f);
      //: also put a LE & CE indicator cylinder
      vgl_point_3d<double> pt(pc.p_[0], pc.p_[1], pc.p_[2]);
      vgl_vector_3d<double> dir(0.0, 0.0, 1.0);
      //: create a cylinder that encapsulates the CE (circular error) and LE (linear error)
      float radius = pc.CE_;
      float height = pc.LE_; // make the major axis height
      std::cout << "CE: " << pc.CE_ << " height: " << pc.LE_ << std::endl;
      bvrml_write::write_vrml_cylinder(ofs, pt, dir, radius, height, 0.0, 1.0, 0.0);
    }
  }
  if (nearest) {
    avg_dist /= cnt;
    avg_LE /= cnt;
    avg_CE /= cnt;
    std::cout << "there were " << cnt << " nearest pts, avg dist: " << avg_dist << " avg LE: " << avg_LE << " avg CE: " << avg_CE << std::endl;
  }

  // CLOSE file
  ofs.close();

  return true;
}


// ============================== PLY ==============================

bool bvrml_load_points_ply(const std::string &ply_file, float dist_thres,
                           std::vector<point_pair> &data)
{
  long nvertices;

  bvrml_filter_fileio_parsed_ply parsed_ply;
  parsed_ply.data = data;
  parsed_ply.dist_thres = dist_thres;

  p_ply ply = ply_open(ply_file.c_str(), nullptr, 0, nullptr);
  if (!ply){
    std::cerr << "Couldn't open ply file: " << ply_file << '\n';
    return false;
  }

  // Read HEADER
  if (!ply_read_header(ply)) return false;

  // vertex
  nvertices =
  ply_set_read_cb(ply, "vertex", "x",
                  bvrml_plyio_vertex_cb_, (void*) (&parsed_ply), 0);
  ply_set_read_cb(ply, "vertex", "y",
                  bvrml_plyio_vertex_cb_, (void*) (&parsed_ply), 1);
  ply_set_read_cb(ply, "vertex", "z",
                  bvrml_plyio_vertex_cb_, (void*) (&parsed_ply), 2);
  ply_set_read_cb(ply, "vertex", "axes_a",
                  bvrml_plyio_vertex_cb_, (void*) (&parsed_ply), 3);
  ply_set_read_cb(ply, "vertex", "axes_b",
                  bvrml_plyio_vertex_cb_, (void*) (&parsed_ply), 4);
  ply_set_read_cb(ply, "vertex", "axes_c",
                  bvrml_plyio_vertex_cb_, (void*) (&parsed_ply), 5);
  ply_set_read_cb(ply, "vertex", "LE",
                  bvrml_plyio_vertex_cb_, (void*) (&parsed_ply), 6);
  ply_set_read_cb(ply, "vertex", "CE",
                  bvrml_plyio_vertex_cb_, (void*) (&parsed_ply), 7);
  ply_set_read_cb(ply, "vertex", "prob",
                  bvrml_plyio_vertex_cb_, (void*) (&parsed_ply), 8);

  std::cout << "ply nvertices: " << nvertices << " points\n";

  // Read DATA
  if (!ply_read(ply))
    return false;

  // CLOSE file
  ply_close(ply);

  data=parsed_ply.data;
  return true;
}


//: Call-back function for a "vertex" element
int bvrml_plyio_vertex_cb_(p_ply_argument argument)
{
  long index;
  void* temp;
  ply_get_argument_user_data(argument, &temp, &index);

  auto* parsed_ply =  (bvrml_filter_fileio_parsed_ply*) temp;

  switch (index)
  {
    case 0: // "x" coordinate
      parsed_ply->p[0] = (float)ply_get_argument_value(argument);
      break;
    case 1: // "y" coordinate
      parsed_ply->p[1] = (float)ply_get_argument_value(argument);
      break;
    case 2: // "z" coordinate
      parsed_ply->p[2] = (float)ply_get_argument_value(argument);
      break;
    case 3: // "axes_a"
      parsed_ply->axes[0] = (float)ply_get_argument_value(argument);
      break;
    case 4: // "axes_b"
      parsed_ply->axes[1] = (float)ply_get_argument_value(argument);
      break;
    case 5: // "axes_c"
      parsed_ply->axes[2] = (float)ply_get_argument_value(argument);
      break;
    case 6: // "LE"
      parsed_ply->LE = (float)ply_get_argument_value(argument);
      break;
    case 7: // "CE"
      parsed_ply->CE = (float)ply_get_argument_value(argument);
      break;
    case 8:
    {
      parsed_ply->prob = (float)ply_get_argument_value(argument);
      // now check if this point needs to be collected
      vgl_point_3d<float> read_pt(parsed_ply->p[0], parsed_ply->p[1], parsed_ply->p[2]);
      std::vector<point_pair>& pp = parsed_ply->data;
      for (auto & i : pp) {
        vgl_vector_3d<float> dif = read_pt-i.first;
        auto dist = (float)dif.length();
        if (dist <= parsed_ply->dist_thres) {
          bvrml_point_cov pc;
          pc.p_[0] = parsed_ply->p[0];
          pc.p_[1] = parsed_ply->p[1];
          pc.p_[2] = parsed_ply->p[2];
          pc.axes_[0] = parsed_ply->axes[0];
          pc.axes_[1] = parsed_ply->axes[1];
          pc.axes_[2] = parsed_ply->axes[2];
          pc.LE_ = parsed_ply->LE;
          pc.CE_ = parsed_ply->CE;
          pc.prob_ = parsed_ply->prob;
          i.second.push_back(pc);
        }
      }
      break;
    }
    default:
      assert(!"This should not happen: index out of range");
      break;
  }
  return 1;
}
