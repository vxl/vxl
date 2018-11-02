//: \file
//  \author Kevin de Souza
//  \date 24 May 2010
//  \brief Program to crop a 2D image down to a specified bounding box.

#include <iostream>
#include <exception>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_arg.h>
#include <mbl/mbl_log.h>
#include <vimt/vimt_add_all_binary_loaders.h>
#include <vimt/vimt_load.h>
#include <vimt/vimt_save.h>
#include <vil/vil_new.h>
#include <vul/vul_file.h>
#include <vul/vul_string.h>
#include <vgl/vgl_point_2d.h>


//=========================================================================
// Static function to create a static logger when first required
//=========================================================================
static mbl_logger& logger()
{
  static mbl_logger l("mul.tools.crop_image_2d");
  return l;
}


//========================================================================
// Actual main function
//========================================================================
int main2(int argc, char*argv[])
{
  unsigned i0=0, j0=0;
  unsigned ni=10, nj=10;
  double fx0=0, fy0=0;
  double fx1=1, fy1=1;
  double x0=0, y0=0;
  double x1=100, y1=100;

  vul_arg_base::set_help_description(
    "DESCRIPTION:\n"
    "Crop a 2D image down to a specified bounding box.\n"
    "The specified bounding box may be expanded slightly to enclose whole voxels.\n"
  );

  // Parse the program arguments
  vul_arg<std::string> img_src(nullptr, "input image filename");
  vul_arg<std::string> img_dst(nullptr, "output image filename");
  vul_arg<std::vector<unsigned> > bbi("-bbi", "bounding box in image coords (i0,j0,i1,j1)");
  vul_arg<std::vector<double> > bbf("-bbf", "bounding box in image fraction e.g. 0.2,0.2,0.75,0.75");
  vul_arg<std::vector<double> > bbw("-bbw", "bounding box in world coords (x0,y0,x1,y1)");
  vul_arg<bool> use_mm("-mm", "World coords are in units of millimetres (default=metres)", false);
  vul_arg_parse(argc, argv);

  // Log the program arguments
  if (logger().level() >= mbl_logger::INFO)
  {
    MBL_LOG(INFO, logger(), "crop_image_2d: ");
    MBL_LOG(INFO, logger(), "  img_src: " << img_src());
    MBL_LOG(INFO, logger(), "  img_dst: " << img_dst());
    MBL_LOG(INFO, logger(), "  use_mm: " << use_mm());
    if (bbi.set())
    {
      std::ostream& log = logger().log(mbl_logger::INFO); // construct a new log message
      log << "  bbi: "; bbi.print_value(log);
      log << std::endl; // terminate log message
    }
    if (bbf.set())
    {
      std::ostream& log = logger().log(mbl_logger::INFO); // construct a new log message
      log << "  bbf: "; bbf.print_value(log);
      log << std::endl; // terminate log message
    }
    if (bbw.set())
    {
      std::ostream& log = logger().log(mbl_logger::INFO); // construct a new log message
      log << "  bbw: "; bbw.print_value(log);
      log << std::endl; // terminate log message
    }
  }

  // Count the number of bbox options specified - should be exactly 1.
  unsigned nbb=0;
  if (bbi.set()) nbb++;
  if (bbf.set()) nbb++;
  if (bbw.set()) nbb++;
  if (nbb!=1)
  {
    std::cerr << "ERROR: specify exactly 1 of the -bbi, -bbf or -bbw options.\n";
    return 1;
  }

  // Validate the bbi argument - should be 4 unsigneds, specifying
  // lower corner (i0,j0) and upper corner (i1,j1) of included voxels
  if (bbi.set())
  {
    if (bbi().size() != 4)
    {
      std::cerr << "ERROR: -bbi argument should contain exactly 4 unsigneds\n";
      return 1;
    }

    if (bbi()[0] >= bbi()[2] || bbi()[1] >= bbi()[3])
    {
      std::cerr << "ERROR: -bbi argument should indicate the lower and upper corners of a 2D box with strictly positive width and height.\n";
      return 1;
    }

    i0 = bbi()[0]; j0 = bbi()[1];
    ni = bbi()[2]-i0+1; nj = bbi()[3]-j0+1;
  }

  // Validate the bbf argument - should be 4 doubles, specifying
  // lower corner fraction (fx0,fy0) and upper corner (fx1,fy1) of included voxels
  if (bbf.set())
  {
    if (bbf().size() != 4)
    {
      std::cerr << "ERROR: -bbf argument should contain exactly 4 floats\n";
      return 1;
    }

    if (bbf()[0] >= bbf()[2] || bbf()[1] >= bbf()[3])
    {
      std::cerr << "ERROR: -bbf argument should indicate the lower and upper corners of a 2D box with strictly positive width and height within [0,1]\n";
      return 1;
    }
    fx0 = bbf()[0]; fy0 = bbf()[1];
    fx1 = bbf()[2]; fy1 = bbf()[3];
  }

  // Validate the bbw argument - should be 4 doubles, specifying
  // lower corner (x0,y0) and upper corner (x1,y1) in world coords
  if (bbw.set())
  {
    if (bbw().size() != 4)
    {
      std::cerr << "ERROR: -bbw argument should contain exactly 4 floats\n";
      return 1;
    }

    if (bbw()[0] >= bbw()[2] || bbw()[1] >= bbw()[3])
    {
      std::cerr << "ERROR: -bbw argument should indicate the lower and upper corners of a 2D box with strictly positive width and height.\n";
      return 1;
    }
    x0 = bbw()[0]; y0 = bbw()[1];
    x1 = bbw()[2]; y1 = bbw()[3];
  }

  // Determine the output filetype
  std::string filetype = vul_file::extension(img_dst());
  vul_string_left_trim(filetype, ".");
  if (filetype.empty()) filetype = "v2i";

  vimt_add_all_binary_loaders();

  vil_image_resource_sptr ir = vil_load_image_resource(img_src().c_str());
  if (!ir)
  {
    std::cerr << "ERROR: Failed to load input image resource\n";
    return 1;
  }
  MBL_LOG(INFO, logger(), "Loaded input image_resource");

  MBL_LOG(INFO, logger(), "Input image: ");
  MBL_LOG(INFO, logger(), "  ni: " << ir->ni());
  MBL_LOG(INFO, logger(), "  nj: " << ir->nj());
  MBL_LOG(INFO, logger(), "  np: " << ir->nplanes());

  const float unit_scaling = use_mm() ? 1000.0f : 1.0f;
  vimt_transform_2d w2i = vimt_load_transform(ir, unit_scaling);
  MBL_LOG(INFO, logger(), "Loaded input image transform");

  if (bbf.set())
  {
    // Convert image fraction values to voxel numbers

    // Round lower bounds down
    i0 = static_cast<unsigned>(std::floor((ir->ni()-1)*fx0));
    j0 = static_cast<unsigned>(std::floor((ir->nj()-1)*fy0));
    // Round upper bounds up
    auto i1 = static_cast<unsigned>(std::ceil((ir->ni()-1)*fx1));
    auto j1 = static_cast<unsigned>(std::ceil((ir->nj()-1)*fy1));
    ni = i1 - i0 + 1;
    nj = j1 - j0 + 1;
  }
  if (bbw.set())
  {
    // Convert world coords values to voxel numbers
    vgl_point_2d<double> imlo = w2i(vgl_point_2d<double>(x0,y0));
    vgl_point_2d<double> imhi = w2i(vgl_point_2d<double>(x1,y1));
    // Round lower bounds down
    i0 = static_cast<unsigned>(std::floor(imlo.x()));
    j0 = static_cast<unsigned>(std::floor(imlo.y()));
    // Round upper bounds up
    auto i1 = static_cast<unsigned>(std::ceil(imhi.x()));
    auto j1 = static_cast<unsigned>(std::ceil(imhi.y()));
    ni = i1 - i0 + 1;
    nj = j1 - j0 + 1;
  }


  if (logger().level()>=mbl_logger::DEBUG)
  {
    vgl_point_2d<double> img_orig_wc = w2i.inverse()(vgl_point_2d<double>(0,0));
    MBL_LOG(DEBUG, logger(), "Image origin in world coords: " << img_orig_wc);
  }

  if (i0 >= ir->ni() || j0 >= ir->nj())
  {
    std::cerr << "ERROR: Crop region bbox lower corner is outside input image.\n";
    return 2;
  }
  if (i0+ni > ir->ni())
  {
    MBL_LOG(WARN, logger(), "Crop region bbox upper corner i was outside input image; truncating to fit.");
    std::cerr << "WARNING: Crop region bbox upper corner i was outside input image; truncating to fit.\n";
    ni = ir->ni()-i0;
  }
  if (j0+nj > ir->nj())
  {
    MBL_LOG(WARN, logger(), "Crop region bbox upper corner j was outside input image; truncating to fit.");
    std::cerr << "WARNING: Crop region bbox upper corner j was outside input image; truncating to fit.\n";
    nj = ir->nj()-j0;
  }

  // Get a view of the cropped region
  vil_image_view_base_sptr ivbp = ir->get_copy_view(i0, ni, j0, nj);

  // Create output image resource
  vil_image_resource_sptr ir2 = vil_new_image_resource(
    img_dst().c_str(), ni, nj, ivbp->nplanes(), ivbp->pixel_format(), filetype.c_str());
  if (!ir2)
  {
    MBL_LOG(ERR, logger(), "Failed to create output image resource");
    std::cerr << "ERROR: Failed to create output image resource\n";
    return 2;
  }
  MBL_LOG(INFO, logger(), "Created output image_resource");

  // Modify transform to account for the implicit translation
  vimt_transform_2d transl;
  transl.set_translation(-double(i0), -double(j0));
  w2i = transl*w2i;
  vimt_save_transform(ir2, w2i, use_mm());

  if (logger().level()>=mbl_logger::DEBUG)
  {
    vgl_point_2d<double> img_orig_wc = w2i.inverse()(vgl_point_2d<double>(0,0));
    MBL_LOG(DEBUG, logger(), "Image origin in world coords: " << img_orig_wc);
  }

  // Write the image to file
  bool succ = ir2->put_view(*ivbp);
  if (!succ)
  {
    MBL_LOG(ERR, logger(), "Failed to put_view into output image resource");
    std::cerr << "ERROR: Failed to put_view into output image resource\n";
    return 3;
  }
  MBL_LOG(INFO, logger(), "Copied cropped image to output image_resource");
  MBL_LOG(INFO, logger(), "Output image: ");
  MBL_LOG(INFO, logger(), "  ni: " << ir2->ni());
  MBL_LOG(INFO, logger(), "  nj: " << ir2->nj());
  MBL_LOG(INFO, logger(), "  np: " << ir2->nplanes());

  return 0;
}


//========================================================================
// Exception-handling wrapper around main function
//========================================================================
int main(int argc, char*argv[])
{
  // Initialize the logger
  mbl_logger::root().load_log_config_file();

  try
  {
    main2(argc, argv);
  }
  catch (std::exception& e)
  {
    std::cout << "Caught exception " << e.what() << std::endl;
    return 3;
  }
  catch (...)
  {
    std::cout << "Caught unknown exception" << std::endl;
    return 3;
  }

  return 0;
}
