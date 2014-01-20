//:
// \file
// \brief Tool to compute shape parameters for each set of points
// \author Tim Cootes

#include <mbl/mbl_read_props.h>
#include <mbl/mbl_exception.h>
#include <mbl/mbl_parse_colon_pairs_list.h>
#include <vul/vul_arg.h>
#include <vcl_sstream.h>
#include <vcl_fstream.h>
#include <vcl_string.h>
#include <vsl/vsl_quick_file.h>
#include <vul/vul_file.h>
#include <msm/msm_shape_model.h>
#include <msm/msm_shape_instance.h>
#include <msm/msm_add_all_loaders.h>

#include <mbl/mbl_stats_1d.h>
/*
Parameter file format:
<START FILE>
//: File to containing shape model
shape_model_path: shape_model.bfs

output_path: shape_params.txt

//: Directory to save best fit points
//  If parameter is not set then do not write best fit points.
out_points_dir: /home/bestfit_points/

image_dir: /home/images/
points_dir: /home/points/
images: {
  image1.pts : image1.jpg
  image2.pts : image2.jpg
}
<END FILE>

Note: You can use the same file as was used to build the model, defining the output
      path using the -o command line option.
*/

void print_usage()
{
  vcl_cout << "msm_get_shape_params -p param_file\n"
           << "Compute shape parameters for each set of points.\n"
           << "Loads in named model and each set of points\n"
           << "Fits model to each set of points and saves\n"
           << "pose and shape parameters to a text file.\n"
           << "One row per shape.\n"
           << "Also save best fit points for every image.\n"
           << vcl_endl;

  vul_arg_display_usage_and_exit();
}

//: Structure to hold parameters
struct tool_params
{
  //: File containing the shape model
  vcl_string shape_model_path;

  //: File to save parameters to
  vcl_string output_path;

  //: Directory containing images
  vcl_string image_dir;

  //: Directory containing points
  vcl_string points_dir;

  //: Directory to save best fit points
  vcl_string out_points_dir;

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

  shape_model_path=props.get_required_property("shape_model_path");

  image_dir=props.get_optional_property("image_dir","./");
  points_dir=props.get_optional_property("points_dir","./");
  output_path=props.get_optional_property("output_path",
                                          "shape_params.txt");
  out_points_dir=props.get_optional_property("out_points_dir","");

  mbl_parse_colon_pairs_list(props.get_required_property("images"),
                             points_names,image_names);

  // Don't look for unused props so can use a single common parameter file.
}

void load_shapes(const vcl_string& points_dir,
                 const vcl_vector<vcl_string>& filenames,
                 vcl_vector<msm_points>& shapes)
{
  unsigned n=filenames.size();

  shapes.resize(n);
  for (unsigned i=0;i<n;++i)
  {
    vcl_string path = points_dir+"/"+filenames[i];
    if (!shapes[i].read_text_file(path))
    {
      mbl_exception_parse_error x("Failed to load points from "+path);
      mbl_exception_error(x);
    }
  }
}


int main(int argc, char** argv)
{
  vul_arg<vcl_string> param_path("-p","Parameter filename");
  vul_arg<vcl_string> out_path("-o","Output path (over-riding param file)");
  vul_arg<bool> no_pose("-no_pose","Don't display pose",false);
  vul_arg<bool> rel_params("-rel_p","Record params[i]/sd[i]",false);
  vul_arg<bool> use_pts_name("-use_pts_name","Include name of points at beginning of the line",false);
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
    vcl_cerr<<"Error: "<<e.what()<<'\n';
    return 1;
  }

  if (out_path()!="") params.output_path = out_path();

  msm_shape_model shape_model;

  if (!vsl_quick_file_load(shape_model,params.shape_model_path))
  {
    vcl_cerr<<"Failed to load shape model from "
            <<params.shape_model_path<<'\n';
    return 2;
  }

  msm_shape_instance sm_instance(shape_model);

  // Load in all the shapes
  vcl_vector<msm_points> shapes;
  load_shapes(params.points_dir,params.points_names,shapes);

  // Open the text file for output
  vcl_ofstream ofs(params.output_path.c_str());
  if (!ofs)
  {
    vcl_cerr<<"Failed to open "<<params.output_path <<" for output.\n";
    return 1;
  }

  bool write_bestfitpts = true;
  if (params.out_points_dir == "")
    write_bestfitpts = false;
  else
    vcl_cout << "Write best fit points to " << params.out_points_dir << vcl_endl;
  
  vnl_vector<double> sd = shape_model.mode_var();
  for (unsigned i=0;i<sd.size();++i) sd[i]=vcl_sqrt(sd[i]);
  mbl_stats_1d mahal_stats;
  
  for (unsigned i=0;i<shapes.size();++i)
  {
    sm_instance.fit_to_points(shapes[i]);

    if (use_pts_name()) ofs<<params.points_names[i]<<' ';

    if (!no_pose())
    {
      // Write pose parameters
      for (unsigned j=0;j<sm_instance.pose().size();++j)
        ofs<<sm_instance.pose()[j]<<' ';
    }

    // Write shape parameters
    vnl_vector<double> b = sm_instance.params();
    double M=0.0;  // Mahalanobis distance
    for (unsigned j=0;j<b.size();++j)
    {
      if (rel_params())
        ofs<<b[j]/sd[j]<<' ';
      else
        ofs<<b[j]<<' ';
      M += b[j]*b[j]/(sd[j]*sd[j]);
    }
    mahal_stats.obs(M);
    
    ofs<<vcl_endl;

    // Write best fit points
    if (write_bestfitpts)
    {
      // Check that the directory exists (points_name may include a dir)
      vcl_string out_pts_dir = vul_file::dirname(params.out_points_dir+"/"+params.points_names[i]);
      if (!vul_file::is_directory(out_pts_dir))
      {
        vcl_cout<<"Directory "<<out_pts_dir
                <<" does not exist. Creating it."<<vcl_endl;
        if (!vul_file::make_directory_path(out_pts_dir))
        {
          vcl_cerr<<"Unable to create it.\n";
          return 12;
        }
      }
      sm_instance.points().write_text_file(params.out_points_dir+"/"+params.points_names[i]);
    }
  }
  ofs.close();
  
  vcl_cout<<"Statistics of Mahalanobis distance: "<<mahal_stats<<vcl_endl;

  vcl_cout<<"Wrote parameters for "<<shapes.size()<<" shapes to "<<params.output_path<<vcl_endl;
  if (use_pts_name()) vcl_cout<<"Each line starts with filename."<<vcl_endl;
  if (!no_pose()) vcl_cout<<"First "<<sm_instance.pose().size()<<" values are pose."<<vcl_endl;
  vcl_cout<<"Next "<<sm_instance.params().size()<<" values are shape params";
  if (rel_params()) vcl_cout<<" in units of SD.";
  vcl_cout<<vcl_endl;

  return 0;
}
