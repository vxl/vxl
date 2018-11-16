//:
// \file
// \brief Tool to write eps files showing a set of shapes defined by shape parameters
// Parameters loaded in from a text file, with one vector per line
// \author Tim Cootes

#include <sstream>
#include <fstream>
#include <string>
#include <sstream>
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

/*
Parameter file format:
<START FILE>
//: File to load model from
shape_model_path: shape_model.msm

curves_path: face_front.crvs

//: Path to text file containing shape parameters (one vector per line)
shape_params_path: shape_params.txt

//: Radius of points to display (if <0, then don't draw points)
point_radius: 2

//: Line width
line_width: 1

line_colour: black
point_colour: red

//: Colour of background (or "none" for transparent)
background_colour: white

// Approximate width of region to display shape
width: 200

// Path to save EPS file to
out_path: shapes.eps
<END FILE>

shape_params.txt should include shape parameters, in order, on each line.
Blank lines are ignored. For instance:
<START FILE>
-0.5
+0.5

0 -0.3
0 + 0.3

0 0 -0.2
0 0 +0.2
<END FILE>
*/

void print_usage()
{
  std::cout << "msm_draw_shapes -p param_file [-s shape_model.msm] [-c curves.crvs] [-o shapes.eps]\n"
           << "Tool to write eps files showing shapes for given shape parameters.\n"
           << std::endl;

  vul_arg_display_usage_and_exit();
}

//: Structure to hold parameters
struct tool_params
{
  //: Path to shape model
  std::string shape_model_path;
  std::string curves_path;
  std::string shape_params_path;

  //: Approximate width of region to display shape
  double width;

  //: Base name for output
  std::string out_path;

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
  shape_params_path=props["shape_params_path"];
  point_radius=vul_string_atof(props.get_optional_property("point_radius","1.5"));
  line_width=vul_string_atof(props.get_optional_property("line_width","1"));
  width=vul_string_atof(props.get_optional_property("width","100"));
  line_colour=props.get_optional_property("line_colour","black");
  point_colour=props.get_optional_property("point_colour","red");
  background_colour=props.get_optional_property("background_colour","white");
  out_path=props.get_optional_property("out_path","shapes.eps");
  shape_model_path=props.get_optional_property("shape_model_path",
                                               "shape_model.msm");

  // Don't look for unused props so can use a single common parameter file.
}


bool load_shape_params(std::string path,
                       std::vector<vnl_vector<double> >& b)
{
  std::ifstream ifs(path.c_str());
  if (!ifs)
  {
    std::cerr<<"Failed to open "<<path<<std::endl;
    return false;
  }

  std::string line;
  while(std::getline(ifs,line))
  {
    if (line=="") continue;
    std::stringstream ss(line);
    std::vector<double> x;
    ss>>std::ws;
    double v;
    while (!ss.eof())
    {
      ss>>v>>std::ws;
      x.push_back(v);
    }
    if (x.size()>0)
    {
      vnl_vector<double> b_new(x.size());
      for (unsigned i=0;i<x.size();++i) b_new[i]=x[i];
      b.push_back(b_new);
    }
  }

  return true;
}


int main(int argc, char** argv)
{
  vul_arg<std::string> param_path("-p","Parameter filename");
  vul_arg<std::string> model_path("-s","Shape model (overriding that in parameter file)");
  vul_arg<std::string> curves_path("-c","Curves path (overriding that in parameter file)");
  vul_arg<std::string> out_path("-o","Output path (overriding that in parameter file)");
  vul_arg<std::string> sp_path("-sp","Shape parameter path (overriding that in parameter file)");
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
  if (out_path()!="") params.out_path=out_path();
  if (sp_path()!="") params.shape_params_path=sp_path();

  msm_shape_model shape_model;

  if (!vsl_quick_file_load(shape_model,params.shape_model_path))
  {
    std::cerr<<"Failed to load shape model from "
            <<params.shape_model_path<<'\n';
    return 2;
  }

  std::cout<<"Model: "<<shape_model<<std::endl;

  std::cerr<<"First mode SDs are ";
  for (unsigned i=0;i<8;++i)
    if (i<shape_model.n_modes())
      std::cout<<std::sqrt(shape_model.mode_var()[i])<<' ';
  std::cout<<std::endl;


  msm_curves curves;
  if (!curves.read_text_file(params.curves_path))
    std::cerr<<"Failed to read in curves from "<<params.curves_path<<'\n';

  // Load in shape parameters from a text file
  std::vector<vnl_vector<double> > b;
  if (!load_shape_params(params.shape_params_path,b))
    return 3;

  // Generate the shapes and compute bounding box
  unsigned n_shapes=b.size();
  std::cout<<"Loaded data for "<<n_shapes<<" shapes."<<std::endl;

  std::vector<msm_points> points(n_shapes);

  // First compute model shapes and largest bounding box
  msm_shape_instance sm_inst(shape_model);

  vgl_box_2d<double> bbox;
  for (unsigned i=0;i<n_shapes;++i)
  {
    sm_inst.set_params(b[i]);
    points[i]=sm_inst.points();
    if (i==0)
      bbox=points[i].bounds();
    else
    {
      bbox.add(points[i].bounds());
    }
  }

  // Estimate box size for each shape.
  double w_width = params.width/n_shapes;
  double w_height = w_width*bbox.height()/bbox.width();
  double s=0.95*w_width/bbox.width();

  // Scale shapes to fit in this box
  for (unsigned i=0;i<n_shapes;++i)
    points[i].scale_by(s);

  // Centre of scaled bounding box
  vgl_point_2d<double > bbox_c(bbox.centroid().x()*s,
                               bbox.centroid().y()*s);

  vgl_point_2d<double> view_centre(0.5*w_width,0.5*w_height);
  vgl_vector_2d<double> t=view_centre-bbox_c;

  // Translate each example to create a row
  // Assume each model centred on origin
  for (unsigned i=0;i<n_shapes;++i)
  {
    // Centre each example along a row
    points[i].translate_by(t.x()+i*w_width,t.y());
  }

  mbl_eps_writer writer(params.out_path.c_str(),
                        params.width,w_height);

  if (params.background_colour!="none")
  {
    writer.set_colour(params.background_colour);
    writer.draw_background_box();
  }

  for (unsigned i=0;i<n_shapes;++i)
  {
    writer.set_colour(params.point_colour);
    if (params.point_radius>0)
      msm_draw_points_to_eps(writer,points[i],
                             params.point_radius);
    writer.set_colour(params.line_colour);
    writer.set_line_width(params.line_width);
    msm_draw_shape_to_eps(writer,points[i],curves);
  }
  writer.close();
  std::cout<<"Saved to "<<params.out_path<<std::endl;

  return 0;
}
