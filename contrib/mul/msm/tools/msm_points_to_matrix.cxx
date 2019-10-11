//:
// \file
// \brief Create a single matrix (text file) containing all points
// Each row contains the point coordinates from one shape (x0,y0,x1,y1...)
// \author Tim Cootes

#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_exception.h>
#include <mbl/mbl_parse_colon_pairs_list.h>
#include <mbl/mbl_parse_int_list.h>
#include <vul/vul_arg.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_quick_file.h>
#include <vul/vul_file.h>
#include <vul/vul_string.h>
#include <msm/msm_reflect_shape.h>


/*
Parameter file format:
<START FILE>

//: Define renumbering required under reflection
//  If defined, a reflected version of each shape is included in build
reflection_symmetry: { 7 6 5 4 3 2 1 0 }

//: When true, only use reflection. When false, use both reflection and original.
only_reflect: false


image_dir: /home/images/
points_dir: /home/points/
images: {
  image1.pts : image1.jpg
  image2.pts : image2.jpg
}
<END FILE>

*/

void print_usage()
{
  std::cout << "msm_points_to_matrix -l image_list.txt -o pts_matrix.txt [-c]\n"
           << "Load in all sets of points in image list.\n"
           << "Saves to single file, one line per shape.\n"
           << "Format per line: x0 y0 x1 y1 ...\n"
           << "If -c flag, then translates so each is shape centred on the origin.\n"
           << std::endl;

  vul_arg_display_usage_and_exit();
}

//: Structure to hold parameters
struct tool_params
{
  //: Define renumbering required under reflection
  //  If defined, a reflected version of each shape is included in build
  std::vector<unsigned> reflection_symmetry;

  //: When true, only use reflection. When false, use both reflection and original.
  bool only_reflect;

  //: Directory containing images
  std::string image_dir;

  //: Directory containing points
  std::string points_dir;

  //: List of image names
  std::vector<std::string> image_names;

  //: List of points file names
  std::vector<std::string> points_names;

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

  image_dir=props.get_optional_property("image_dir","./");
  points_dir=props.get_optional_property("points_dir","./");

  std::string ref_sym_str=props.get_optional_property("reflection_symmetry","-");
  reflection_symmetry.resize(0);
  if (ref_sym_str!="-")
  {
    std::stringstream ss(ref_sym_str);
    mbl_parse_int_list(ss, std::back_inserter(reflection_symmetry),
                       unsigned());
  }

  only_reflect=vul_string_to_bool(props.get_optional_property("only_reflect","false"));

  mbl_parse_colon_pairs_list(props.get_required_property("images"),
                             points_names,image_names);

  // Don't look for unused props so can use a single common parameter file.
}

void load_shapes(const std::string& points_dir,
                 const std::vector<std::string>& filenames,
                 std::vector<msm_points>& shapes)
{
  unsigned n=filenames.size();

  shapes.resize(n);
  for (unsigned i=0;i<n;++i)
  {
    std::string path = points_dir+"/"+filenames[i];
    if (!shapes[i].read_text_file(path))
    {
      mbl_exception_parse_error x("Failed to load points from "+path);
      mbl_exception_error(x);
    }
  }
}


int main(int argc, char** argv)
{
  vul_arg<std::string> list_path("-l","Image list");
  vul_arg<std::string> out_path("-o","Output path (over-riding param file)");
  vul_arg<bool> centre_shapes("-c","Centre shapes to origin.",false);
  vul_arg_parse(argc,argv);

  if (list_path()=="")
  {
    print_usage();
    return 0;
  }

  tool_params params;
  try
  {
    params.read_from_file(list_path());
  }
  catch (mbl_exception_parse_error& e)
  {
    std::cerr<<"Error: "<<e.what()<<'\n';
    return 1;
  }

  // Load in all the shapes
  std::vector<msm_points> shapes;
  load_shapes(params.points_dir,params.points_names,shapes);

  std::vector<std::string> points_names=params.points_names;
  if (params.reflection_symmetry.size()>0)
  {
    // Use reflections
    msm_points ref_points;
    unsigned n=shapes.size();
    for (unsigned i=0;i<n;++i)
    {
      msm_reflect_shape_along_x(shapes[i],params.reflection_symmetry,
                                ref_points,shapes[i].cog().x());
      if (params.only_reflect) shapes[i]=ref_points;
      else
      {
        shapes.push_back(ref_points);
        points_names.push_back(points_names[i]+"-ref");
      }
    }
  }

  if (centre_shapes())
  {
    for (unsigned i=0;i<shapes.size();++i)
    {
      vgl_point_2d<double> cog=shapes[i].cog();
      shapes[i].translate_by(-cog.x(),-cog.y());
    }
  }

  // Open the text file for output
  std::ofstream ofs(out_path().c_str());
  if (!ofs)
  {
    std::cerr<<"Failed to open "<<out_path()<<" for output.\n";
    return 1;
  }

  for (unsigned i=0;i<shapes.size();++i)
  {
    for (unsigned j=0;j<shapes[i].size();++j)
      ofs<<shapes[i][j].x()<<" "<<shapes[i][j].y()<<" ";
    ofs<<std::endl;
  }
  ofs.close();

  std::cout<<"Wrote "<<shapes.size()<<" shapes to "<<out_path()<<std::endl;
  if (centre_shapes())
    std::cout<<"Shapes centred on the origin."<<std::endl;

  return 0;
}
