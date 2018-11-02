//:
// \file
// \brief Tool to write eps files showing modes of model
// \author Tim Cootes

#include <sstream>
#include <fstream>
#include <string>
#include <iostream>
#include <algorithm>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_exception.h>
#include <vul/vul_arg.h>
#include <vul/vul_string.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_quick_file.h>

#include <msm/msm_shape_model.h>
#include <msm/msm_shape_instance.h>
#include <msm/msm_curve.h>

#include <msm/msm_add_all_loaders.h>
#include <msm/utils/msm_draw_shape_to_eps.h>
#include <msm/utils/msm_shape_mode_view.h>

/*
Parameter file format:
<START FILE>
//: File to load model from
shape_model_path: shape_model.bfs

curves_path: face_front.crvs

// Maximum number of shape modes
max_modes: 99

//: Number of SDs to vary mode by
n_sds: 3.0

//: Current number of shapes per mode
n_per_mode: 3

//: When true, generate 2*n_per_mode frames of a movie for each mode
make_movie: false

//: When true, overlap all the shapes to highlight changes
//  Shapes on one side of mean are drawn with dashes, on the other with dots.
overlap_shapes: false

//: Radius of points to display (if <0, then don't draw points)
point_radius: 2

//: Line width
line-width: 1

line_colour: black
point_colour: red


//: Colour of background (or "none" for transparent)
background_colour: white

// Approximate width of region to display shape
width: 200

// Optional paths to text files containing a matrix and vector
// defining a linear subspace of the parameter space.
// When supplied, the tool will generate the shapes by
// moving along the modes defined in the subspace.
// A subspace model of the form x=mean+(PQ)b is created,
// where P is the matrix of original modes, Q is the loaded matrix
// defining the subspace.  so each column defines a subspace mode.
// subspace_mode_path: subspace_mode_matrix.txt
// subspace_var_path: subspace_var_vector.txt

base_name: mode
output_dir: ./

<END FILE>
*/

void print_usage()
{
  std::cout << "msm_draw_shape_modes -p param_file [-s shape_model.msm] [-c curves.crvs] [-c base_name]\n"
           << "Tool to write eps files showing modes of model.\n"
           << std::endl;

  vul_arg_display_usage_and_exit();
}

//: Structure to hold parameters
struct tool_params
{
  //: Path to shape model
  std::string shape_model_path;
  std::string curves_path;

  //: Maximum number of shape modes
  unsigned max_modes;

  //: Number of SDs to vary mode by
  double n_sds;

  //: Current number of shapes per mode
  unsigned n_per_mode;

  //: When true, generate 2*n_per_mode frames of a movie for each mode
  bool make_movie;

  //: When true, overlap all the shapes to highlight changes
  bool overlap_shapes;

  //: Approximate width of region to display shape
  double width;

  //: Optional path to text file containing a mode matrix
  std::string subspace_mode_path;

  //: Optional path to text files containing a variance vector
  std::string subspace_var_path;

  //: Base name for output
  std::string base_name;

  //: Directory to save eps files
  std::string output_dir;

  //: Line colour
  std::string line_colour;

  //: Point colour
  std::string point_colour;

  //: Colour of background (or "none" for transparent)
  std::string background_colour;


  //: Radius of points to display (if <0, then don't draw points)
  double point_radius;

  //: Line width
  double line_width;

  //: Parse named text file to read in data
  //  Throws a mbl_exception_parse_error if fails
  void read_from_file(const std::string& path);
};

//: Parse named text file to read in data
//  Throws a mbl_exception_parse_error if fails
void tool_params::read_from_file(const std::string& path)
{
  std::ifstream ifs(path.c_str());
  if (!ifs)
  {
    std::string error_msg = "Failed to open file: "+path;
    throw (mbl_exception_parse_error(error_msg));
  }

  mbl_read_props_type props = mbl_read_props_ws(ifs);

  curves_path=props["curves_path"];
  n_per_mode=vul_string_atoi(props.get_optional_property("n_per_mode","3"));
  max_modes=vul_string_atoi(props.get_optional_property("max_modes","5"));
  overlap_shapes=vul_string_to_bool(props.get_optional_property("overlap_shapes","false"));
  make_movie=vul_string_to_bool(props.get_optional_property("make_movie","false"));
  n_sds=vul_string_atof(props.get_optional_property("n_sds","3.0"));
  point_radius=vul_string_atof(props.get_optional_property("point_radius","1.5"));
  line_width=vul_string_atof(props.get_optional_property("line_width","1"));
  width=vul_string_atof(props.get_optional_property("width","100"));
  line_colour=props.get_optional_property("line_colour","black");
  point_colour=props.get_optional_property("point_colour","red");
  background_colour=props.get_optional_property("background_colour","white");
  base_name=props.get_optional_property("base_name","./");
  output_dir=props.get_optional_property("output_dir","./");
  shape_model_path=props.get_optional_property("shape_model_path",
                                               "shape_aam.bfs");

  if (make_movie) overlap_shapes=true;

  subspace_mode_path = props["subspace_mode_path"];
  if (subspace_mode_path=="-") subspace_mode_path="";

  subspace_var_path = props["subspace_var_path"];
  if (subspace_var_path=="-") subspace_var_path="";

  // Don't look for unused props so can use a single common parameter file.
}


void draw_mode(msm_shape_mode_view& mode_view,
               const msm_curves& curves,
               tool_params& params)
{
  mode_view.compute_shapes();
  unsigned m = mode_view.mode();
  vgl_box_2d<int> win_box = mode_view.display_window();

  std::stringstream ss;
  ss<<params.output_dir<<'/'<<params.base_name<<"_s"<<m<<".eps";
  mbl_eps_writer writer(ss.str().c_str(),
                        win_box.width(),win_box.height());

  if (params.background_colour!="none")
  {
    writer.set_colour(params.background_colour);
    writer.draw_background_box();
  }

  unsigned n_shapes = mode_view.points().size();
  for (unsigned i=0;i<n_shapes;++i)
  {
    writer.set_colour(params.point_colour);
    if (params.point_radius>0)
      msm_draw_points_to_eps(writer,mode_view.points()[i],
                             params.point_radius);
    writer.set_colour(params.line_colour);
    writer.set_line_width(params.line_width);
    if (params.overlap_shapes)
    {
      // Use dashes for one side of mean, dots for the other
      if (i<n_shapes/2) writer.ofs()<<"[3 2] 0 setdash"<<std::endl;  // Dashes
      else
      if (i==n_shapes/2) writer.ofs()<<"[1 0] 0 setdash"<<std::endl;  // Solid
      else
        writer.ofs()<<"[1 2] 0 setdash"<<std::endl;  // Dots
    }
    msm_draw_shape_to_eps(writer,mode_view.points()[i],curves);
  }
  writer.close();
  std::cout<<"Saved to "<<ss.str()<<std::endl;
}

//: Write a set of eps files defining a movie of the mode.
void draw_mode_frames(msm_shape_mode_view& mode_view,
                      const msm_curves& curves,
                      tool_params& params)
{
  mode_view.compute_shapes();
  unsigned m = mode_view.mode();
  vgl_box_2d<int> win_box = mode_view.display_window();

  unsigned n_shapes = mode_view.points().size();
  for (unsigned i=0;i<2*n_shapes-2;++i)
  {
    // f goes from 0 - (n_shapes-1), then back down to 1
    unsigned f=i;
    if (i>=n_shapes) f=2*n_shapes-2-i;

    std::stringstream ss;
    ss<<params.output_dir<<'/'<<params.base_name<<"_s"<<m<<'_';
    if (i<10) ss<<'0';
    ss<<i<<".eps";
    mbl_eps_writer writer(ss.str().c_str(),
                          win_box.width(),win_box.height());

    if (params.background_colour!="none")
    {
      writer.set_colour(params.background_colour);
      writer.draw_background_box();
    }

    writer.set_colour(params.point_colour);
    if (params.point_radius>0)
      msm_draw_points_to_eps(writer,mode_view.points()[f],
                             params.point_radius);
    writer.set_colour(params.line_colour);
    writer.set_line_width(params.line_width);
    msm_draw_shape_to_eps(writer,mode_view.points()[f],curves);

    writer.close();
  }
  std::cout<<"Saved "<<2*n_shapes-2<<" frames to "
          <<params.base_name<<"_s"<<m<<"_XX.eps"<<std::endl;
}

//: Create new model where modes are a subspace of the original model
//  Loads in a matrix Q defining modes, and vector V defining variance
//  Modes of new model are given by (PQ) where P are the modes of the
//  original.
msm_shape_model create_subspace_model(
            const msm_shape_model& shape_model,
            const std::string& mode_path,
            const std::string& var_path)
{
  // Attempt to load in modes
  vnl_matrix<double> Q;
  std::ifstream Qs(mode_path.c_str());
  if (!Qs)
  {
    std::cout<<"Failed to open "<<mode_path<<std::endl;
    std::abort();
  }

  if (!Q.read_ascii(Qs))
  {
    std::cerr<<"Failed to read matrix from "<<mode_path<<'\n';
    std::abort();
  }
  Qs.close();

  vnl_vector<double> new_var;
  std::ifstream Vs(var_path.c_str());
  if (!Vs)
  {
    std::cout<<"Failed to open "<<var_path<<std::endl;
    std::abort();
  }
  if (!new_var.read_ascii(Vs))
  {
    std::cerr<<"Failed to read vector from "<<var_path<<'\n';
    std::abort();
  }
  Vs.close();

std::cout<<"new_var: "<<new_var<<std::endl;

  std::cerr<<"Number of subspace modes = "<<Q.columns()<<'\n';

  if (Q.columns()!=new_var.size())
  {
    std::cerr<<"Number of variances = "<<new_var.size()<<'\n'
            <<"Numbers differ."<<'\n';
    std::abort();
  }

  if (Q.rows()>shape_model.n_modes())
  {
    std::cerr<<"More rows in matrix than number of modes available."<<'\n';
    std::abort();
  }

  const vnl_matrix<double>& allP = shape_model.modes();
  vnl_matrix<double> subP = allP.extract(allP.rows(),Q.rows());

  vnl_matrix<double> newP = subP * Q;

  msm_shape_model new_model;
  new_model.set(shape_model.mean_points(),newP,new_var,
                shape_model.default_pose(),
                shape_model.aligner(),
                shape_model.param_limiter());

  return new_model;
}

int main(int argc, char** argv)
{
  vul_arg<std::string> param_path("-p","Parameter filename");
  vul_arg<std::string> model_path("-s","Shape model (overriding that in parameter file)");
  vul_arg<std::string> curves_path("-c","Curves path (overriding that in parameter file)");
  vul_arg<std::string> base_name("-b","Base name for output (overriding that in parameter file)");
  vul_arg_parse(argc,argv);

  msm_add_all_loaders();

  if (param_path()=="")
  {
    print_usage();
    return 0;
  }

  tool_params params;
  try
  {
    params.read_from_file(param_path());
  }
  catch (mbl_exception_parse_error& e)
  {
    std::cerr<<"Error: "<<e.what()<<'\n';
    return 1;
  }

  if (model_path()!="") params.shape_model_path=model_path();
  if (curves_path()!="") params.curves_path=curves_path();
  if (base_name()!="") params.base_name=base_name();

  msm_shape_model shape_model;

  if (!vsl_quick_file_load(shape_model,params.shape_model_path))
  {
    std::cerr<<"Failed to load shape model from "
            <<params.shape_model_path<<'\n';
    return 2;
  }

  std::cout<<"Model: "<<shape_model<<std::endl;
  std::cerr<<"First mode variances are ";
  for (unsigned i=0;i<8;++i)
    if (i<shape_model.n_modes())
      std::cout<<shape_model.mode_var()[i]<<' ';
  std::cout<<std::endl;

  if (params.subspace_mode_path!="")
    shape_model = create_subspace_model(shape_model,
                                        params.subspace_mode_path,
                                        params.subspace_var_path);

  msm_curves curves;
  if (!curves.read_text_file(params.curves_path))
    std::cerr<<"Failed to read in curves from "<<params.curves_path<<'\n';

  msm_shape_mode_view mode_view;
  mode_view.set_shape_model(shape_model);
#if 0
  // Estimate aspect ratio
  vgl_box_2d<double> bounds = shape_model.mean_points().bounds();
  double asp=bounds.height()/(1e-3+bounds.width());

  int win_width;
  if (params.overlap_shapes)
    win_width=int(params.width);
  else
    win_width=int(params.width*params.n_per_mode);

  int win_height=int(asp*params.width);
  mode_view.set_display_window(vgl_box_2d<int>(0,win_width, 0,win_height));
#endif // 0
  mode_view.set_range(params.n_sds);
  mode_view.set_n_per_mode(params.n_per_mode);
  mode_view.set_overlap_shapes(params.overlap_shapes);
  mode_view.set_display_width(params.width);
#if 0
  vgl_box_2d<int> win_box = mode_view.display_window();
#endif // 0

  unsigned n_modes = std::min(params.max_modes,shape_model.n_modes());

  for (unsigned m=0;m<n_modes;++m)
  {
    mode_view.set_mode(1+m);

    if (params.make_movie)
      draw_mode_frames(mode_view,curves,params);
    else
      draw_mode(mode_view,curves,params);
  }
  return 0;
}
