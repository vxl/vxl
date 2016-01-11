//:
// \file
// \brief Tool to select larger shapes from image and point list files
// Loads in list of images and points file names.
// Outputs new list, containing the subset with shapes such that
// the separation between the reference points is larger than a threshold.
// \author Tim Cootes

#include <mbl/mbl_read_props.h>
#include <mbl/mbl_exception.h>
#include <mbl/mbl_parse_colon_pairs_list.h>
#include <vul/vul_arg.h>
#include <vcl_sstream.h>
#include <vcl_fstream.h>
#include <vcl_string.h>
#include <vsl/vsl_quick_file.h>
#include <msm/msm_points.h>
#include <msm/msm_add_all_loaders.h>

/*
Input file format:
<START FILE>
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
  vcl_cout << "msm_select_large_shapes -i image_list.txt -r0 0 -r1 1 -mins 30 -o new_list.txt\n"
           << "Loads images and point file names, and directory strings from input file.\n"
           << "Outputs new list, containing the subset with shapes such that\n"
           << "the separation between the reference points is larger than a threshold."
           << vcl_endl;

  vul_arg_display_usage_and_exit();
}

//: Structure to hold parameters
struct tool_params
{
  //: Directory containing images
  vcl_string image_dir;

  //: Directory containing points
  vcl_string points_dir;

  //: List of image names
  vcl_vector<vcl_string> image_names;

  //: List of points file names
  vcl_vector<vcl_string> points_names;

  //: Parse named text file to read in data
  //  Throws a mbl_exception_parse_error if fails
  void read_from_file(const vcl_string& path);
};

//: Parse named text file to read in data
//  Throws a mbl_exception_parse_error if fails
void tool_params::read_from_file(const vcl_string& path)
{
  vcl_ifstream ifs(path.c_str());
  if (!ifs)
  {
    vcl_string error_msg = "Failed to open file: "+path;
    throw (mbl_exception_parse_error(error_msg));
  }

  mbl_read_props_type props = mbl_read_props_ws(ifs);

  image_dir=props.get_optional_property("image_dir","");
  points_dir=props.get_optional_property("points_dir","");

  mbl_parse_colon_pairs_list(props.get_required_property("images"),
                             points_names,image_names);

  // Don't look for unused props so can use a single common parameter file.
}

int main(int argc, char** argv)
{
  vul_arg<vcl_string> in_path("-i","Input image + points file");
  vul_arg<vcl_string> out_path("-o","Output path");
  vul_arg<unsigned> ref_pt0("-r0","Index of reference point 0",0);
  vul_arg<unsigned> ref_pt1("-r1","Index of reference point 1",1);
  vul_arg<double> min_sep("-mins","Minimum separation of points required",30.0);
  vul_arg_parse(argc,argv);

  if (in_path()=="" || out_path()=="")
  {
    print_usage();
    return 0;
  }

  tool_params params;
  try
  {
    params.read_from_file(in_path());
  }
  catch (mbl_exception_parse_error& e)
  {
    vcl_cerr<<"Error: "<<e.what()<<'\n';
    return 1;
  }


  // Open the text file for output
  vcl_ofstream ofs(out_path().c_str());
  if (!ofs)
  {
    vcl_cerr<<"Failed to open "<<out_path() <<" for output.\n";
    return 1;
  }

  ofs<<"// Shapes such that separation between point "
     <<ref_pt0()<<" and "<<ref_pt1()<<" is at least "<<min_sep()<<vcl_endl;
  ofs<<"image_dir: "<<params.image_dir<<vcl_endl;
  ofs<<"points_dir: "<<params.points_dir<<vcl_endl;
  ofs<<"images: {"<<vcl_endl;

  unsigned n_selected=0;
  msm_points points;
  for (unsigned i=0;i<params.image_names.size();++i)
  {
    vcl_string pts_path=params.points_dir+"/"+params.points_names[i];
    if (!points.read_text_file(pts_path))
    {
      vcl_cerr<<"Failed to read points from "<<pts_path<<vcl_endl;
      return 1;
    }
    double s = (points[ref_pt1()]-points[ref_pt0()]).length();
    if (s<min_sep()) continue;  // Shape too small

    ofs<<"  "<<params.points_names[i]<<" : "<<params.image_names[i]<<vcl_endl;
    n_selected++;
  }
  ofs<<"}"<<vcl_endl;
  ofs.close();

  vcl_cout<<"Total number considered: "<<params.image_names.size()<<vcl_endl;
  vcl_cout<<"Wrote new list of "<<n_selected<<" shapes to "<<out_path()<<vcl_endl;

  return 0;
}
