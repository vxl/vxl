//: \file
//  \author Ian Scott
//  \date 8 April 2008
//  \brief Program to print info about an image.

#include <vcl_exception.h>
#include <vcl_iostream.h>
#include <vul/vul_arg.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <mbl/mbl_log.h>
#include <vil/vil_load.h>
#include <vimt/vimt_transform_2d.h>
#include <vimt/vimt_load_transform.h>
#include <vimt/vimt_add_all_binary_loaders.h>
#include <vimt3d/vimt3d_load.h>
#include <vimt3d/vimt3d_transform_3d.h>
#include <vimt3d/vimt3d_add_all_loaders.h>

static unsigned try_3d_image(const char * filename, float unit_scaling)
{
  try
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
      vcl_cerr << "Can only cope with \"-s 1\" or \"-s 1000\" for 3D images.\n";
      vcl_exit(3);
    }

    vgl_vector_3d<double> voxel = w2i.inverse().delta(vgl_point_3d<double>(0,0,0), vgl_vector_3d<double>(1,1,1));

    vgl_point_3d<double> world_min_point = w2i.inverse().origin();
    vgl_point_3d<double> world_max_point = w2i.inverse()(ir->ni()+0.999, ir->nj()+0.999, ir->nk()+0.999);
    vcl_cout << "size: " << ir->ni() << 'x' << ir->nj() << 'x' << ir->nk() << " voxels x " << ir->nplanes() << "planes\n"
             << "size: " << ir->ni()*voxel.x() << 'x' << ir->nj()*voxel.y() << 'x' << ir->nk()*voxel.z()
             << " in units of " << 1.0/unit_scaling << "m\n"
             << "voxel size: " << voxel.x() << 'x' << voxel.y() << 'x' << voxel.z()
             << " in units of " << 1.0/unit_scaling << "m\n"
             << "world_origin: " << w2i.origin().x() << 'x' << w2i.origin().y() << 'x' << w2i.origin().z() << " voxels\n"
             << "world bounds: [" << world_min_point.x() << ',' << world_min_point.y() << ',' << world_min_point.z() << "] -> ["
             << world_max_point.x() << ',' << world_max_point.y() << ',' << world_max_point.z() << "]\n"
             << "voxel_type: " << ir->pixel_format() << '\n';
    return 0;
  }
  catch (vcl_exception &e)
  {
    vcl_cerr << e.what();
    return 1;
  }
}


static unsigned try_2d_image(const char * filename, float unit_scaling)
{
  try
  {
    vil_image_resource_sptr ir = vil_load_image_resource(filename);
    if (!ir) return 1;
    vimt_transform_2d w2i = vimt_load_transform(ir, unit_scaling);
    vgl_vector_2d<double> pixel = w2i.inverse().delta(vgl_point_2d<double>(0,0), vgl_vector_2d<double>(1,1));
    vcl_cout << "size: " << ir->ni() << 'x' << ir->nj() << " pixels x " << ir->nplanes() << "planes\n"
             << "size: " << ir->ni()*pixel.x() << 'x' << ir->nj()*pixel.y()
             << " in units of " << 1.0/unit_scaling << "m\n"
             << "pixel size: " << pixel.x() << 'x' << pixel.y()
             << " in units of " << 1.0/unit_scaling << "m\n"
             << "world_origin: " << w2i.origin().x() << 'x' << w2i.origin().y() << " pixels\n"
             << "pixel_type: " << ir->pixel_format() << '\n';
    return 0;
  }
  catch (vcl_exception &e)
  {
    vcl_cerr << e.what();
    return 1;
  }
}
//========================================================================
// Actual main function
//========================================================================
int main2(int argc, char*argv[])
{
  vimt_add_all_binary_loaders();
  vimt3d_add_all_loaders();

  // Parse the program arguments
  vul_arg<vcl_string> img_src(0, "input image filename");
  vul_arg<float> unit_scaling("-s", "Unit scaling (1000 for mm)", 1000);
  vul_arg_parse(argc, argv);

  if (try_3d_image(img_src().c_str(), unit_scaling()) == 0)
    return 0;

  return try_2d_image(img_src().c_str(), unit_scaling());
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
  catch (vcl_exception& e)
  {
    vcl_cout << "caught exception " << e.what() << vcl_endl;
    return 3;
  }
  catch (...)
  {
    vcl_cout << "caught unknown exception " << vcl_endl;
    return 3;
  }


  return 0;
}
