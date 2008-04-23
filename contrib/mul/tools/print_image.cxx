//: \file
//  \author Ian Scott
//  \date 8 April 2008
//  \brief Program to print info about an image.

#include <vcl_exception.h>
#include <vcl_iostream.h>
#include <vul/vul_arg.h>
#include <mbl/mbl_log.h>
#include <vil/vil_load.h>
#include <vimt/vimt_image_2d.h>
#include <vimt/vimt_load_transform.h>
#include <vimt/vimt_add_all_binary_loaders.h>



//========================================================================
// Actual main function
//========================================================================
int main2(int argc, char*argv[])
{ 
  vimt_add_all_binary_loaders();

  // Parse the program arguments
  vul_arg<vcl_string> img_src(0, "input image filename");
  vul_arg<float> unit_scaling("-s", "Unit scaling (1000 for mm)", 1000);
  vul_arg_parse(argc, argv); 


  vil_image_resource_sptr ir = vil_load_image_resource(img_src().c_str());
  vimt_transform_2d w2i = vimt_load_transform(ir, unit_scaling());
  vgl_vector_2d<double> pixel = w2i.inverse().delta(vgl_point_2d<double>(0,0), vgl_vector_2d<double>(1,1));
  vcl_cout << "size: " << ir->ni() << "x" << ir->nj() << " pixels x " << ir->nplanes()
    << "planes\n";
  vcl_cout << "size: " << ir->ni()*pixel.x() << "x" << ir->nj()*pixel.y() << " in units of " <<
    1.0/unit_scaling() << "m\n";
  vcl_cout << "pixel size: " << pixel.x() << "x" << pixel.y() << " in units of " <<
    1.0/unit_scaling() << "m\n";
  vcl_cout << "world_origin: " << w2i.origin().x() << "x" << w2i.origin().y() << " pixels\n";
  vcl_cout << "pixel_type: " << ir->pixel_format() << "\n";
  return 0;
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
