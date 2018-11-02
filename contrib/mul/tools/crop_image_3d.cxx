//: \file
//  \author Kevin de Souza
//  \date 31 January 2008
//  \brief Program to crop a 3D image down to a specified bounding box.

#include <exception>
#include <iostream>
#include <algorithm>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_arg.h>
#include <mbl/mbl_log.h>
#include <vimt3d/vimt3d_add_all_loaders.h>
#include <vimt3d/vimt3d_load.h>
#include <vimt3d/vimt3d_save.h>
#include <vil3d/vil3d_save.h>
#include <vil3d/vil3d_new.h>
#include <vil3d/vil3d_crop.h>
#include <vul/vul_file.h>
#include <vul/vul_string.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>


//=========================================================================
// Static function to create a static logger when first required
//=========================================================================
static mbl_logger& logger()
{
  static mbl_logger l("mul.tools.crop_image_3d");
  return l;
}


//========================================================================
// Actual main function
//========================================================================
int main2(int argc, char*argv[])
{
  unsigned i0=0, j0=0, k0=0;
  unsigned ni=10, nj=10, nk=10;
  double fx0=0, fy0=0, fz0=0;
  double fx1=1, fy1=1, fz1=1;
  double x0=0, y0=0, z0=0;
  double x1=100, y1=100, z1=100;

  vul_arg_base::set_help_description(
    "DESCRIPTION:\n"
    "Crop a 3D image down to a specified bounding box.\n"
    "The specified bounding box may be expanded slightly to enclose whole voxels.\n"
  );

  // Parse the program arguments
  vul_arg<std::string> img_src(nullptr, "input image filename");
  vul_arg<std::string> img_dst(nullptr, "output image filename");
  vul_arg<std::vector<unsigned> > bbi("-bbi", "bounding box in image coords (i0,j0,k0,i1,j1,k1)");
  vul_arg<std::vector<double> > bbf("-bbf", "bounding box in image fraction e.g. 0.2,0.2,0.2,0.75,0.75,0.75");
  vul_arg<std::vector<double> > bbw("-bbw", "bounding box in world coords (x0,y0,z0,x1,y1,z1)");
  vul_arg<std::vector<double> > cw("-cw", "crop width in world distances (xlo_d,ylo_d,zlo_d,xhi_d,yhi_d,zhi_d)");
  vul_arg<bool> use_mm("-mm", "World coords are in units of millimetres (default=metres)", false);
  vul_arg<bool> ignore_bounds_errors("-ib", "adjust any bounding box values outside image to image edge", false);
  vul_arg_parse(argc, argv);

    // Log the program arguments
  if (logger().level() >= mbl_logger::INFO)
  {
    MBL_LOG(INFO, logger(), "crop_image_3d: ");
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
    if (cw.set())
    {
      std::ostream& log = logger().log(mbl_logger::INFO); // construct a new log message
      log << "  cw: "; cw.print_value(log);
      log << std::endl; // terminate log message
    }
  }

  // Count the number of bbox options specified - should be exactly 1.
  unsigned nbb=0;
  if (bbi.set()) nbb++;
  if (bbf.set()) nbb++;
  if (bbw.set()) nbb++;
  if (cw.set()) nbb++;
  if (nbb!=1)
  {
    std::cerr << "ERROR: specify exactly 1 of the -bbi, -bbf, -bbw, or -cw options." << std::endl;
    return 1;
  }

  // Validate the bbi argument - should be 6 unsigneds, specifying
  // lower corner (i0,j0,k0) and upper corner (i1,j1,k1) of included voxels
  if (bbi.set())
  {
    if (bbi().size() != 6)
    {
      std::cerr << "ERROR: -bbi argument should contain exactly 6 unsigneds\n";
      return 1;
    }

    if (bbi()[0] >= bbi()[3] || bbi()[1] >= bbi()[4] || bbi()[2] >= bbi()[5])
    {
      std::cerr << "ERROR: -bbi argument should indicate the lower and upper corners of a 3D box with strictly positive width, height and depth\n";
      return 1;
    }

    i0 = bbi()[0]; j0 = bbi()[1]; k0 = bbi()[2];
    ni = bbi()[3]-i0+1; nj = bbi()[4]-j0+1; nk = bbi()[5]-k0+1;
  }

  // Validate the bbf argument - should be 6 doubles, specifying
  // lower corner fraction (fx0,fy0,fz0) and upper corner (fx1,fy1,fz1) of included voxels
  if (bbf.set())
  {
    if (bbf().size() != 6)
    {
      std::cerr << "ERROR: -bbf argument should contain exactly 6 floats\n";
      return 1;
    }

    if (bbf()[0] >= bbf()[3] || bbf()[1] >= bbf()[4] || bbf()[2] >= bbf()[5])
    {
      std::cerr << "ERROR: -bbf argument should indicate the lower and upper corners of a 3D box with strictly positive width, height and depth within [0,1]\n";
      return 1;
    }
    fx0 = bbf()[0]; fy0 = bbf()[1]; fz0 = bbf()[2];
    fx1 = bbf()[3]; fy1 = bbf()[4]; fz1 = bbf()[5];
  }

  // Validate the bbw argument - should be 6 doubles, specifying
  // lower corner (x0,y0,z0) and upper corner (x1,y1,z1) in world coords
  if (bbw.set())
  {
    if (bbw().size() != 6)
    {
      std::cerr << "ERROR: -bbw argument should contain exactly 6 floats\n";
      return 1;
    }

    if (bbw()[0] >= bbw()[3] || bbw()[1] >= bbw()[4] || bbw()[2] >= bbw()[5])
    {
      std::cerr << "ERROR: -bbw argument should indicate the lower and upper corners of a 3D box with strictly positive width, height and depth.\n";
      return 1;
    }
    x0 = bbw()[0]; y0 = bbw()[1]; z0 = bbw()[2];
    x1 = bbw()[3]; y1 = bbw()[4]; z1 = bbw()[5];
  }
  // Validate the cw argument - should be 6 doubles, specifying
  // lower corner shift up (x0,y0,z0) and upper corner shift down (x1,y1,z1) in world distances
  if (cw.set())
  {
    if (cw().size() != 6)
    {
      std::cerr << "ERROR: -cw argument should contain exactly 6 floats\n";
      return 1;
    }

    if (cw()[0] < 0 || cw()[1] || cw()[2] < 0 || cw()[3]<0 || cw()[4]<0 || cw()[5]<0)
    {
      std::cerr << "ERROR: -cw argument should specify cropping widths on each face of the bounding box. Negative values are not allowed.\n";
      return 1;
    }

    x0 = cw()[0]; y0 = cw()[1]; z0 = cw()[2];
    x1 = cw()[3]; y1 = cw()[4]; z1 = cw()[5];
  }

  // Determine the output filetype
  std::string filetype = vul_file::extension(img_dst());
  vul_string_left_trim(filetype, ".");
  if (filetype.empty()) filetype = "v3i";

  vimt3d_add_all_loaders();

  vil3d_image_resource_sptr ir = vil3d_load_image_resource(img_src().c_str());
  if (!ir)
  {
    std::cerr << "ERROR: Failed to load input image resource\n";
    return 1;
  }
  MBL_LOG(INFO, logger(), "Loaded input image_resource");

  MBL_LOG(INFO, logger(), "Input image: ");
  MBL_LOG(INFO, logger(), "  ni: " << ir->ni());
  MBL_LOG(INFO, logger(), "  nj: " << ir->nj());
  MBL_LOG(INFO, logger(), "  nk: " << ir->nk());
  MBL_LOG(INFO, logger(), "  np: " << ir->nplanes());

  vimt3d_transform_3d w2i = vimt3d_load_transform(ir, use_mm());
  MBL_LOG(INFO, logger(), "Loaded input image transform");

  if (bbf.set())
  {
    if (ignore_bounds_errors())
    {
      if (fx0 < 0) fx0=0;
      if (fy0 < 0) fy0=0;
      if (fz0 < 0) fz0=0;
      if (fx0 > 1) fx1=1;
      if (fy0 > 1) fy1=1;
      if (fz0 > 1) fz1=1;
    }

    // Convert image fraction values to voxel numbers

    // Round lower bounds down
    i0 = static_cast<unsigned>(std::floor((ir->ni()-1)*fx0));
    j0 = static_cast<unsigned>(std::floor((ir->nj()-1)*fy0));
    k0 = static_cast<unsigned>(std::floor((ir->nk()-1)*fz0));
    // Round upper bounds up
    auto i1 = static_cast<unsigned>(std::ceil((ir->ni()-1)*fx1));
    auto j1 = static_cast<unsigned>(std::ceil((ir->nj()-1)*fy1));
    auto k1 = static_cast<unsigned>(std::ceil((ir->nk()-1)*fz1));
    ni = i1 - i0 + 1;
    nj = j1 - j0 + 1;
    nk = k1 - k0 + 1;
  }
  if (bbw.set())
  {
    // Convert world coords values to voxel numbers
    vgl_point_3d<double> imlo = w2i(vgl_point_3d<double>(x0,y0,z0));
    vgl_point_3d<double> imhi = w2i(vgl_point_3d<double>(x1,y1,z1));
    imhi.set(imhi.x()*0.999999, imhi.y()*0.999999, imhi.z()*0.999999);
    if (ignore_bounds_errors())
    {
      imlo.set(std::max<double>(imlo.x(),0),
        std::max<double>(imlo.y(),0),
        std::max<double>(imlo.z(),0) );
      imhi.set(std::min<double>(imhi.x(),ir->ni()-1),
        std::min<double>(imhi.y(),ir->nj()-1),
        std::min<double>(imhi.z(),ir->nk()-1) );
    }
    // Round lower bounds down
    i0 = static_cast<unsigned>(std::floor(imlo.x()));
    j0 = static_cast<unsigned>(std::floor(imlo.y()));
    k0 = static_cast<unsigned>(std::floor(imlo.z()));
    // Round upper bounds up
    auto i1 = static_cast<unsigned>(std::ceil(imhi.x()));
    auto j1 = static_cast<unsigned>(std::ceil(imhi.y()));
    auto k1 = static_cast<unsigned>(std::ceil(imhi.z()));


    ni = i1 - i0 + 1;
    nj = j1 - j0 + 1;
    nk = k1 - k0 + 1;
  }
  if (cw.set())
  {

    // Convert world coords values to voxel numbers
    vgl_box_3d<double> bb;
    bb.add(w2i.inverse()(0,0,0));
    bb.add(w2i.inverse()(ir->ni(),ir->nj(),ir->nk()));
    bb.set_min_x(bb.min_x()+x0);
    bb.set_min_y(bb.min_y()+y0);
    bb.set_min_z(bb.min_z()+z0);
    bb.set_max_x(bb.max_x()-x1);
    bb.set_max_y(bb.max_y()-y1);
    bb.set_max_z(bb.max_z()-z1);

    if (bb.is_empty())
    {
      std::cerr << "ERROR: -cw argument should specify cropping widths that do not overlap.\n";
      return 1;
    }


    vgl_point_3d<double> imhi = w2i(bb.max_point());
    imhi.set(imhi.x()*0.999999, imhi.y()*0.999999, imhi.z()*0.999999);
    vgl_point_3d<double> imlo = w2i(bb.min_point());

    // Round lower bounds down
    i0 = static_cast<unsigned>(std::max(0.0,std::floor(imlo.x())));
    j0 = static_cast<unsigned>(std::max(0.0,std::floor(imlo.y())));
    k0 = static_cast<unsigned>(std::max(0.0,std::floor(imlo.z())));
    // Round upper bounds up
    auto i1 = static_cast<unsigned>(std::ceil(imhi.x()));
    auto j1 = static_cast<unsigned>(std::ceil(imhi.y()));
    auto k1 = static_cast<unsigned>(std::ceil(imhi.z()));


    ni = i1 - i0;
    nj = j1 - j0;
    nk = k1 - k0;
  }
  if (ignore_bounds_errors())
  {
    if (i0+ni > ir->ni()) ni=ir->ni() - i0;
    if (j0+nj > ir->nj()) nj=ir->nj() - j0;
    if (k0+nk > ir->nk()) nk=ir->nk() - k0;
  }


  if (logger().level()>=mbl_logger::DEBUG)
  {
    vgl_point_3d<double> img_orig_wc = w2i.inverse()(vgl_point_3d<double>(0,0,0));
    MBL_LOG(DEBUG, logger(), "Image origin in world coords: " << img_orig_wc);
  }

  if (i0 >= ir->ni() || j0 >= ir->nj() || k0 > ir->nk())
  {
    std::cerr << "ERROR: Crop region bbox lower corner is outside input image " <<
      vul_file::strip_directory(img_src()) << "\n";
    return 2;
  }
  if (i0+ni > ir->ni())
  {
    std::cerr << "WARNING: Crop region bbox upper corner i was outside input image " <<
      vul_file::strip_directory(img_src()) << "; truncating to fit.\n";
    ni = ir->ni()-i0;
  }
  if (j0+nj > ir->nj())
  {
    std::cerr << "WARNING: Crop region bbox upper corner j was outside input image " <<
      vul_file::strip_directory(img_src()) << "; truncating to fit.\n";
    nj = ir->nj()-j0;
  }
  if (k0+nk > ir->nk())
  {
    std::cerr << "WARNING: Crop region bbox upper corner k was outside input image " <<
      vul_file::strip_directory(img_src()) << "; truncating to fit.\n";
    nk = ir->nk()-k0;
  }

  // Get a view of the cropped region
  vil3d_image_view_base_sptr ivbp = ir->get_copy_view(i0, ni, j0, nj, k0, nk);

  // Create output image resource
  vil3d_image_resource_sptr ir2 = vil3d_new_image_resource(
    img_dst().c_str(), ni, nj, nk, ivbp->nplanes(), ivbp->pixel_format(), filetype.c_str());
  if (!ir2)
  {
    std::cerr << "ERROR: Failed to create output image resource\n";
    return 2;
  }
  MBL_LOG(INFO, logger(), "Created output image_resource");

  // Modify transform to account for the implicit translation
  vimt3d_transform_3d transl;
  transl.set_translation(-double(i0), -double(j0), -double(k0));
  w2i = transl*w2i;
  vimt3d_save_transform(ir2, w2i, use_mm());

  if (logger().level()>=mbl_logger::DEBUG)
  {
    vgl_point_3d<double> img_orig_wc = w2i.inverse()(vgl_point_3d<double>(0,0,0));
    MBL_LOG(DEBUG, logger(), "Image origin in world coords: " << img_orig_wc);
  }

  // Write the image to file
  bool succ = ir2->put_view(*ivbp);
  if (!succ)
  {
    std::cerr << "ERROR: Failed to put_view into output image resource\n";
    return 3;
  }
  MBL_LOG(INFO, logger(), "Copied cropped image to output image_resource");
  MBL_LOG(INFO, logger(), "Output image: ");
  MBL_LOG(INFO, logger(), "  ni: " << ir2->ni());
  MBL_LOG(INFO, logger(), "  nj: " << ir2->nj());
  MBL_LOG(INFO, logger(), "  nk: " << ir2->nk());
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
    std::cout << "Caught unknown exception " << std::endl;
    return 3;
  }

  return 0;
}
