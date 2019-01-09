//:
// \file
// \brief Tool to crop images to region around points
// For each input image and points, generates an output image and associated points
// Crops to a region around the points, and optionally rescales.
// Output image is saved as a byte image, with range stretched to [0,255]
// \author Tim Cootes

#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_exception.h>
#include <mbl/mbl_parse_colon_pairs_list.h>
#include <vul/vul_arg.h>
#include <vul/vul_string.h>
#include <vul/vul_file.h>
#include <msm/msm_points.h>
#include <vimt/vimt_image_2d_of.h>
#include <vgl/vgl_intersection.h>
#include <vil/vil_crop.h>
#include <vil/vil_convert.h>
#include <vil/vil_save.h>
#include <vnl/vnl_math.h>
#include <vimt/vimt_load.h>

/*
Parameter file format:
<START FILE>

// Where to save new images
out_image_dir: cropped_images

// Where to save new points
out_points_dir: cropped_points

//: Border width (as proportion of size)
border_width: 0.2


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
  std::cout << "msm_crop_images -p param_file\n"
          << "Tool to crop images to region around points.\n"
          << "For each input image and points, generates an output image and associated points.\n"
          << "Crops to a region around the points, and optionally rescales.\n"
          << "Output image is saved as a byte image, with range stretched to [0,255].\n"
          << std::endl;

  vul_arg_display_usage_and_exit();
}

//: Structure to hold parameters
struct tool_params
{
  // Where to save new images
  std::string out_image_dir;

  //: Optional new extension for image path (enabling saving as new type)
  // If "-" then set to empty.
  std::string im_ext_str;

  // Where to save new points
  std::string out_points_dir;

  //: Border width (as proportion of size)
  double border_width;

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

  out_image_dir=props.get_optional_property("out_image_dir","./cropped_images");
  out_points_dir=props.get_optional_property("out_points_dir","./cropped_points");
  im_ext_str=props.get_optional_property("im_ext_str","");
  if (im_ext_str=="-") im_ext_str="";
  image_dir=props.get_optional_property("image_dir","./");
  border_width=vul_string_atof(props.get_optional_property("border_width","0.1"));
  points_dir=props.get_optional_property("points_dir","./");

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

void make_dir_if_needed(std::string path)
{
  std::string dir=vul_file::dirname(path);
  if (!vul_file::is_directory(dir))
  {
    std::cout<<"Directory "<<dir<<" does not exist. Creating it."<<std::endl;
    if (!vul_file::make_directory_path(dir))
    {
      std::cout<<"Unable to create directory."<<std::endl;
    }
  }
}

int main(int argc, char** argv)
{
  vul_arg<std::string> param_path("-p","Parameter filename");
  vul_arg_parse(argc,argv);

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


  msm_points points;
  vimt_image_2d_of<float> image;
  unsigned n=params.image_names.size();
  for (unsigned i=0;i<n;++i)
  {
    std::string pts_path = params.points_dir+"/"+params.points_names[i];
    if (!points.read_text_file(pts_path))
    {
      std::cerr<<"Failed to load points from "+pts_path<<std::endl;
      return 1;
    }

    std::string image_path = params.image_dir+"/"+params.image_names[i];
    vimt_load_to_float(image_path,image,1.0);

    // Project points into image frame
    points.transform_by(image.world2im());
    vgl_box_2d<double> box=points.bounds();

    box.scale_about_centroid(1.0+params.border_width);

    const vil_image_view<float>& im=image.image();

    // Ensure box still in image
    box=vgl_intersection(box,vgl_box_2d<double>(0,im.ni()-1,  0,im.nj()-1));

    // Crop image to this box.
    unsigned ilo=vnl_math::rnd(box.min_x());
    unsigned ihi=vnl_math::rnd(box.max_x());
    unsigned jlo=vnl_math::rnd(box.min_y());
    unsigned jhi=vnl_math::rnd(box.max_y());
    vil_image_view<float> crop_im = vil_crop(im,ilo,1+ihi-ilo, jlo, 1+jhi-jlo);

    vil_image_view<vxl_byte> patch;
    vil_convert_stretch_range(crop_im,patch);

    std::string out_im_path=params.out_image_dir+"/"+params.image_names[i]+params.im_ext_str;
    make_dir_if_needed(out_im_path);
    if (!vil_save(patch,out_im_path.c_str()))
    {
      std::cerr<<"Failed to save image to "<<out_im_path<<std::endl;
      return 2;
    }

    // Compute point positions in new image
    points.translate_by(-double(ilo),-double(jlo));

    std::string out_pts_path=params.out_points_dir+"/"+params.points_names[i];
    make_dir_if_needed(out_pts_path);
    if (!points.write_text_file(out_pts_path))
    {
      std::cerr<<"Failed to write new points to "<<out_pts_path<<std::endl;
      return 3;
    }

    std::cout<<"Saved cropped image to "<<out_im_path<<" and points to "<<out_pts_path<<std::endl;
  }

  return 0;
}
