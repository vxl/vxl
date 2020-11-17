//:
// \file
// \brief Tool to write eps files showing a set of shapes defined by shape parameters
// Parameters loaded in from a text file, with one vector per line.
// Includes option (save_as_sequence) to save each shape (one line of text file) as
// a separate file (outfile_XXXX.eps), which can then be used to make animations.
// \author Tim Cootes

#include <sstream>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_exception.h>
#include <mbl/mbl_parse_string_list.h>
#include "vul/vul_arg.h"
#include "vul/vul_string.h"
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vsl/vsl_quick_file.h"

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

// When defined, cycle through these colours when drawing each shape
point_colours: { red green blue }

// When defined, cycle through these colours when drawing each shape
line_colours: { red green blue }

// When defined, cycle through these styles when drawing each shape
// Options: dots1, dots2, solid, dashes3-1, dashes3-2, dashes2-3, dashes1-3
// dashesX-Y  X=length of dash, Y=length of space
line_styles: { dots solid dashes3-2 }

//: Colour of background (or "none" for transparent)
background_colour: white

//: When true, overlap all the shapes (all have same centre point)
overlap_shapes: false

//: When true, save each shape to a separate file (out_path1234.eps)
save_as_sequence: false

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

  // When defined, cycle through these colours when drawing each shape
  std::vector<std::string> line_colours;

  // When defined, cycle through these styles when drawing each shape
  std::vector<std::string> line_styles;

  // When defined, cycle through these colours when drawing each shape
  std::vector<std::string> point_colours;


  //: Point colour
  std::string point_colour;

  //: Colour of background (or "none" for transparent)
  std::string background_colour;


  //: Radius of points to display (if <0, then don't draw points)
  double point_radius;

  //: Line width
  double line_width;

  //: When true, overlap all the shapes (all have same centre point)
  bool overlap_shapes;

  //: When true, save each shape to a separate file (out_path1234.eps)
  bool save_as_sequence;

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
  overlap_shapes=vul_string_to_bool(props.get_optional_property("overlap_shapes","false"));
  save_as_sequence=vul_string_to_bool(props.get_optional_property("save_as_sequence","false"));
  if (save_as_sequence) overlap_shapes=true;  // Don't apply offset to video frames

  std::string point_colours_str=props.get_optional_property("point_colours","");
  if (!point_colours_str.empty()) {
    std::stringstream lcss(point_colours_str);
    mbl_parse_string_list(lcss,point_colours);

    std::cout<<"PointColours: ";
    for (unsigned i=0;i<line_colours.size();++i) std::cout<<point_colours[i]<<" ";
    std::cout<<std::endl;
  }

  if (point_colours.empty()) {
    point_colours.resize(1);
    point_colours[0]=point_colour;
  }

  std::string line_colours_str=props.get_optional_property("line_colours","");
  if (!line_colours_str.empty()) {
    std::stringstream lcss(line_colours_str);
    mbl_parse_string_list(lcss,line_colours);

    std::cout<<"LineColours: ";
    for (unsigned i=0;i<line_colours.size();++i) std::cout<<line_colours[i]<<" ";
    std::cout<<std::endl;
  }

  if (line_colours.empty()) {
    line_colours.resize(1);
    line_colours[0]=line_colour;
  }

  std::string line_styles_str=props.get_optional_property("line_styles","");
  if (line_styles_str!="")
  {
    std::stringstream lsss(line_styles_str);
    mbl_parse_string_list(lsss,line_styles);

    std::cout<<"LineStyles: ";
    for (unsigned i=0;i<line_styles.size();++i) std::cout<<line_styles[i]<<" ";
    std::cout<<std::endl;
  }

  if (line_styles.size()<1)
  {
    line_styles.resize(1);
    line_styles[0]="solid";
  }

  out_path=props.get_optional_property("out_path","shapes.eps");
  shape_model_path=props.get_optional_property("shape_model_path",
                                               "shape_model.msm");

  // Don't look for unused props so can use a single common parameter file.
}


bool load_shape_params(const std::string& path,
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
    if (line.empty())
      continue;
    std::stringstream ss(line);
    std::vector<double> x;
    ss>>std::ws;
    double v;
    while (!ss.eof())
    {
      ss>>v>>std::ws;
      x.push_back(v);
    }
    if (!x.empty()) {
      vnl_vector<double> b_new(x.size());
      for (unsigned i=0;i<x.size();++i) b_new[i]=x[i];
      b.push_back(b_new);
    }
  }

  return true;
}

// Return eps command to achieve given line style
std::string line_style_command(std::string str)
{
  // Cycle through styles
  if (str=="dots1")
    return std::string("[0.5 1] 0 setdash");
  else
  if (str=="dots2")
    return std::string("[0.5 2] 0 setdash");
  else
  if (str=="dashes3-1")
    return std::string("[3 1] 0 setdash");  // Dashes
  else
  if (str=="dashes3-2")
    return std::string("[3 2] 0 setdash");  // Dashes
  else
  if (str=="dashes2-3")
    return std::string("[2 3] 0 setdash");  // Dashes
  else
  if (str=="dashes1-3")
    return std::string("[1 3] 0 setdash");  // Dashes

  return std::string("[1 0] 0 setdash");  // Solid
}

std::string make_filename(std::string basename, unsigned i)
{
  std::stringstream ss;
  ss<<basename;
  if (i<10) ss<<"0";
  if (i<100) ss<<"0";
  if (i<1000) ss<<"0";
  ss<<i<<".eps";
  return ss.str();
}

void save_as_sequence(tool_params& params,
                      const std::vector<msm_points>& points,
                      double w_height,
                      const msm_curves& curves)
{
  unsigned n_shapes=points.size();
  for (unsigned i=0;i<n_shapes;++i)
  {
    std::string filepath=  make_filename(params.out_path,i);

    mbl_eps_writer writer(filepath.c_str(),
                          params.width,w_height);

    if (params.background_colour!="none")
    {
      writer.set_colour(params.background_colour);
      writer.draw_background_box();
    }

      // Cycle through colours for points
    writer.set_colour(params.point_colours[0]);

    if (params.point_radius>0)
      msm_draw_points_to_eps(writer,points[i],
                             params.point_radius);

      // Cycle through colours for lines
    writer.set_colour(params.line_colours[0]);
    writer.set_line_width(params.line_width);

    writer.ofs()<<line_style_command(params.line_styles[0])<<std::endl;

    msm_draw_shape_to_eps(writer,points[i],curves);
    writer.close();
  }
  std::cout<<"Saved sequence to "<<params.out_path<<"????.eps"<<std::endl;
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

  if (param_path().empty()) {
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

  if (!model_path().empty())
    params.shape_model_path = model_path();
  if (!curves_path().empty())
    params.curves_path = curves_path();
  if (!out_path().empty())
    params.out_path = out_path();
  if (!sp_path().empty())
    params.shape_params_path = sp_path();

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
  if (params.overlap_shapes) w_width=params.width;
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

  // Horizontal distance between shapes is either 0 or w_width
  double x_step=(params.overlap_shapes?0:w_width);

  // Translate each example to create a row
  // Assume each model centred on origin
  for (unsigned i=0;i<n_shapes;++i)
  {
    // Centre each example along a row
    points[i].translate_by(t.x()+i*x_step,t.y());
  }

  if (params.save_as_sequence)
  {
    save_as_sequence(params,points,w_height,curves);
    return 0;
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
      // Cycle through colours for points
    writer.set_colour(params.point_colours[i%params.point_colours.size()]);

    if (params.point_radius>0)
      msm_draw_points_to_eps(writer,points[i],
                             params.point_radius);

      // Cycle through colours for lines
    writer.set_colour(params.line_colours[i%params.line_colours.size()]);
    writer.set_line_width(params.line_width);

    writer.ofs()<<line_style_command(params.line_styles[i%params.line_styles.size()])<<std::endl;

    msm_draw_shape_to_eps(writer,points[i],curves);
  }
  writer.close();
  std::cout<<"Saved to "<<params.out_path<<std::endl;

  return 0;
}
