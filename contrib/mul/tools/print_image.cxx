//: \file
//  \author Ian Scott
//  \date 8 April 2008
//  \brief Program to print info about an image.

#include <exception>
#include <iostream>
#include <cstdlib>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_arg.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <mbl/mbl_log.h>
#include <vil/vil_load.h>
#include <vimt/vimt_transform_2d.h>
#include <vimt/vimt_load.h>
#include <vimt/vimt_add_all_binary_loaders.h>
#include <vimt3d/vimt3d_load.h>
#include <vimt3d/vimt3d_transform_3d.h>
#include <vimt3d/vimt3d_add_all_loaders.h>


//: Load the image data and determine the intensity range
static void get_intensity_range_2d(const vil_image_resource_sptr& ir, double& min, double& max)
{
  vil_image_view<double> iv_double = vil_convert_cast(double(), ir->get_view());
  vil_math_value_range(iv_double, min, max);
}


//: Load the image data and determine the intensity range
static void get_intensity_range_3d(const vil3d_image_resource_sptr& ir, double& min, double& max)
{
  vil3d_image_view<double> iv_double = vil3d_convert_cast(double(), ir->get_view());
  vil3d_math_value_range(iv_double, min, max);
}


//: Calculate pixel size from transform of any form
// Adapted from vimt_image_2d.cxx
vgl_vector_2d<double> pixel_size_from_transform(const vimt_transform_2d& w2i)
{
  const vimt_transform_2d& i2w = w2i.inverse();
  vgl_point_2d<double> p(0,0);
  vgl_vector_2d<double> i(1,0);
  vgl_vector_2d<double> j(0,1);
  double dx = i2w.delta(p, i).length();
  double dy = i2w.delta(p, j).length();
  return {dx, dy};
}


//: Calculate voxel size from transform of any form
// Adapted from vimt3d_image_3d.cxx
static vgl_vector_3d<double> voxel_size_from_transform(const vimt3d_transform_3d& w2i)
{
  const vimt3d_transform_3d& i2w = w2i.inverse();
  vgl_point_3d<double> p(0,0,0);
  vgl_vector_3d<double> i(1,0,0);
  vgl_vector_3d<double> j(0,1,0);
  vgl_vector_3d<double> k(0,0,1);
  double dx = i2w.delta(p, i).length();
  double dy = i2w.delta(p, j).length();
  double dz = i2w.delta(p, k).length();
  return {dx, dy, dz};
}


//: Try to load a 3D image
static unsigned try_3d_image(const char * filename, float unit_scaling, bool range)
{
  vil3d_image_resource_sptr ir = vil3d_load_image_resource(filename);
  if (!ir) return 1;

  vimt3d_transform_3d w2i;
  if (unit_scaling == 1000.0)
    w2i = vimt3d_load_transform(ir, true);
  else if (unit_scaling == 1.0)
    w2i = vimt3d_load_transform(ir, false);
  else
  {
    std::cerr << "Can only cope with \"-s 1\" or \"-s 1000\" for 3D images.\n";
    std::exit(3);
  }

  vgl_vector_3d<double> voxel = voxel_size_from_transform(w2i);
  vgl_point_3d<double> world_min_point = w2i.inverse().origin();
  vgl_point_3d<double> world_max_point = w2i.inverse()(ir->ni()+0.999, ir->nj()+0.999, ir->nk()+0.999);
  vgl_point_3d<double> world_centre = world_min_point + (world_max_point-world_min_point)/2.0;
  std::cout << "size: " << ir->ni() << 'x' << ir->nj() << 'x' << ir->nk() << " voxels x " << ir->nplanes() << "planes\n"
           << "size: " << ir->ni()*voxel.x() << 'x' << ir->nj()*voxel.y() << 'x' << ir->nk()*voxel.z()
           << " in units of " << 1.0/unit_scaling << "m\n"
           << "voxel size: " << voxel.x() << 'x' << voxel.y() << 'x' << voxel.z()
           << " in units of " << 1.0/unit_scaling << "m\n"
           << "world_origin: " << w2i.origin().x() << 'x' << w2i.origin().y() << 'x' << w2i.origin().z() << " voxels\n"
           << "world bounds: [" << world_min_point.x() << ',' << world_min_point.y() << ',' << world_min_point.z() << "] -> ["
           << world_max_point.x() << ',' << world_max_point.y() << ',' << world_max_point.z() << "]\n"
           << "world centre: " << world_centre.x() << ',' << world_centre.y() << ',' << world_centre.z() << "\n"
           << "voxel_type: " << ir->pixel_format() << "\n"
           << "transform: " << w2i
           << std::endl;

  if (range)
  {
    double min,max;
    get_intensity_range_3d(ir, min, max);
    std::cout << "intensity range: " << min << " to " << max << std::endl;
  }
  return 0;
}


//: Try to load a 2D image
static unsigned try_2d_image(const char * filename, float unit_scaling, bool range)
{
  vil_image_resource_sptr ir = vil_load_image_resource(filename);
  if (!ir) return 1;

  vimt_transform_2d w2i = vimt_load_transform(ir, unit_scaling);
  //vgl_vector_2d<double> pixel = w2i.inverse().delta(vgl_point_2d<double>(0,0), vgl_vector_2d<double>(1,1));
  vgl_vector_2d<double> pixel = pixel_size_from_transform(w2i);
  vgl_point_2d<double> world_min_point = w2i.inverse().origin();
  vgl_point_2d<double> world_max_point = w2i.inverse()(ir->ni()+0.999, ir->nj()+0.999);
  vgl_point_2d<double> world_centre = world_min_point + (world_max_point-world_min_point)/2.0;
  std::cout << "size: " << ir->ni() << 'x' << ir->nj() << " pixels x " << ir->nplanes() << "planes\n"
           << "size: " << ir->ni()*pixel.x() << 'x' << ir->nj()*pixel.y()
           << " in units of " << 1.0/unit_scaling << "m\n"
           << "pixel size: " << pixel.x() << 'x' << pixel.y()
           << " in units of " << 1.0/unit_scaling << "m\n"
           << "world_origin: " << w2i.origin().x() << 'x' << w2i.origin().y() << " pixels\n"
           << "world bounds: [" << world_min_point.x() << ',' << world_min_point.y() << "] -> ["
           << world_max_point.x() << ',' << world_max_point.y() << "]\n"
           << "world centre: " << world_centre.x() << ',' << world_centre.y() << "\n"
           << "pixel_type: " << ir->pixel_format() << "\n"
           << "transform: " << w2i
           << std::endl;

  if (range)
  {
    double min,max;
    get_intensity_range_2d(ir, min, max);
    std::cout << "intensity range: " << min << " to " << max << std::endl;
  }

  return 0;
}


//========================================================================
// Actual main function
//========================================================================
int main2(int argc, char*argv[])
{
  vimt_add_all_binary_loaders();
  vimt3d_add_all_loaders();

  // Parse the program arguments
  vul_arg<std::string> img_src(nullptr, "input image filename");
  vul_arg<float> unit_scaling("-s", "Unit scaling (1000 for mm)", 1000);
  vul_arg<bool> range("-r", "Determine intensity range", false);
  vul_arg<bool> only_3d("-3", "Only try to load 3d image", false);
  vul_arg<bool> only_2d("-2", "Only try to load 2d image", false);
  vul_arg_parse(argc, argv);


  if (only_3d() && only_2d())
  {
    vul_arg_display_usage_and_exit("Can't have both \"-2\" and \"-3\"\n");
    return 1;
  }
  else if (only_3d())
  {
    return try_3d_image(img_src().c_str(), unit_scaling(), range());
  }
  else if (only_2d())
  {
    return try_2d_image(img_src().c_str(), unit_scaling(), range());
  }
  else
  {
    try
    {
      if (try_3d_image(img_src().c_str(), unit_scaling(), range()) == 0)
        return 0;
    }
    catch (std::exception& e)
    {
      std::cout << "caught exception " << e.what() << std::endl;
    }
    return try_2d_image(img_src().c_str(), unit_scaling(), range());
  }
}


//========================================================================
// Exception-handling wrapper around main function
//========================================================================
int main(int argc, char*argv[])
{
  try
  {
    // Initialize the logger
    mbl_logger::root().load_log_config_file();
    main2(argc, argv);
  }
  catch (std::exception& e)
  {
    std::cout << "caught exception " << e.what() << std::endl;
    return 3;
  }
  catch (...)
  {
    std::cout << "caught unknown exception " << std::endl;
    return 3;
  }

  return 0;
}
