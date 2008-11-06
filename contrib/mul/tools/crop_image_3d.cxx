//: \file
//  \author Kevin de Souza
//  \date 31 January 2008
//  \brief Program to crop a 3D image down to a specified bounding box.

#include <vcl_exception.h>
#include <vcl_iostream.h>
#include <vul/vul_arg.h>
#include <mbl/mbl_log.h>
#include <vimt3d/vimt3d_add_all_loaders.h>
#include <vimt3d/vimt3d_load.h>
#include <vil3d/vil3d_save.h>
#include <vil3d/vil3d_new.h>
#include <vil3d/vil3d_crop.h>
#include <vul/vul_file.h>
#include <vul/vul_string.h>
#include <vimt3d/vimt3d_vil3d_v3i.h>
#include <vgl/vgl_point_3d.h>

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
  bool use_millimeters=false;
  unsigned i0=0, j0=0, k0=0;
  unsigned ni=10, nj=10, nk=10;
  double fx0=0, fy0=0, fz0=0;
  double fx1=1, fy1=1, fz1=1;

  // Parse the program arguments
  vul_arg<vcl_string> img_src(0, "input image filename");
  vul_arg<vcl_string> img_dst(0, "output image filename");
  vul_arg<vcl_vector<unsigned> > bbi("-bbi", "bounding box (image coords: i0,j0,k0,i1,j1,k1) of crop region");
  vul_arg<vcl_vector<double> > bbf("-bbf", "bounding box (image fraction e.g. 0.2,0.2,0.2,0.75,0.75,0.75) of crop region");
  vul_arg_parse(argc, argv);

  // Log the program arguments
  MBL_LOG(INFO, logger(), "crop_image_3d: ");
  MBL_LOG(INFO, logger(), "  img_src: " << img_src());
  MBL_LOG(INFO, logger(), "  img_dst: " << img_dst());
  if (bbi.set())
    MBL_LOG(INFO, logger(), "  bbi: " << bbi.print_value(logger().log(mbl_logger::INFO)));
  if (bbf.set())
    MBL_LOG(INFO, logger(), "  bbf: " << bbf.print_value(logger().log(mbl_logger::INFO)));

  if (bbi.set() && bbf.set())
  {
    vcl_cerr << "ERROR: do not specify both -bbi and -bbf options together" << vcl_endl;
    return 1;
  }

  // Validate the bbi argument - should be 6 unsigneds, specifying
  // lower corner (i0,j0,k0) and upper corner (i1,j1,k1) of included voxels
  if (bbi.set())
  {
    if (bbi().size() != 6)
    {
      vcl_cerr << "ERROR: bbi argument should contain exactly 6 unsigneds\n";
      return 1;
    }

    if (bbi()[0] >= bbi()[3] || bbi()[1] >= bbi()[4] || bbi()[2] >= bbi()[5])
    {
      vcl_cerr << "ERROR: bbi argument should indicate the lower and upper corners of a 3D box with strictly positive width, height and depth\n";
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
      vcl_cerr << "ERROR: bbf argument should contain exactly 6 floats\n";
      return 1;
    }

    if (bbf()[0] >= bbf()[3] || bbf()[1] >= bbf()[4] || bbf()[2] >= bbf()[5])
    {
      vcl_cerr << "ERROR: bbf argument should indicate the lower and upper corners of a 3D box with strictly positive width, height and depth within [0,1]\n";
      return 1;
    }
    fx0 = bbf()[0]; fy0 = bbf()[1]; fz0 = bbf()[2];    
    fx1 = bbf()[3]; fy1 = bbf()[4]; fz1 = bbf()[5];
  }

  // Determine the output filetype
  vcl_string filetype = vul_file::extension(img_dst());
  vul_string_left_trim(filetype, ".");
  if (filetype.empty()) filetype = "v3i";

  vimt3d_add_all_loaders();

  vil3d_image_resource_sptr ir = vil3d_load_image_resource(img_src().c_str());
  if (!ir)
  {
    vcl_cerr << "ERROR: Failed to load input image resource\n";
    return 1;
  }
  MBL_LOG(INFO, logger(), "Loaded input image_resource");

  MBL_LOG(INFO, logger(), "Input image: ");
  MBL_LOG(INFO, logger(), "  ni: " << ir->ni());
  MBL_LOG(INFO, logger(), "  nj: " << ir->nj());
  MBL_LOG(INFO, logger(), "  nk: " << ir->nk());
  MBL_LOG(INFO, logger(), "  np: " << ir->nplanes());

  vimt3d_transform_3d tr = vimt3d_load_transform(ir, use_millimeters);
  MBL_LOG(INFO, logger(), "Loaded input image transform");
  
  if (bbf.set())
  {
    // Convert image fraction values to voxel numbers 
    // Round lower bounds down
    i0 = static_cast<unsigned>(vcl_floor((ir->ni()-1)*fx0));
    j0 = static_cast<unsigned>(vcl_floor((ir->nj()-1)*fy0));
    k0 = static_cast<unsigned>(vcl_floor((ir->nk()-1)*fz0));
    // Round upper bounds up
    unsigned i1 = static_cast<unsigned>(vcl_ceil((ir->ni()-1)*fx1));
    unsigned j1 = static_cast<unsigned>(vcl_ceil((ir->nj()-1)*fy1));
    unsigned k1 = static_cast<unsigned>(vcl_ceil((ir->nk()-1)*fz1));
    ni = i1 - i0 + 1;
    nj = j1 - j0 + 1;
    nk = k1 - k0 + 1;
  }

  if (logger().level()>=mbl_logger::DEBUG)
  {
    vgl_point_3d<double> img_orig_wc = tr.inverse()(vgl_point_3d<double>(0,0,0));
    MBL_LOG(DEBUG, logger(), "Image origin in world coords: " << img_orig_wc);
  }

  if (i0 >= ir->ni() || j0 >= ir->nj() || k0 > ir->nk())
  {
    vcl_cerr << "ERROR: Crop region bbox lower corner is outside input image.\n";
    return 2;
  }
  if (i0+ni > ir->ni())
  {
    MBL_LOG(WARN, logger(), "Crop region bbox upper corner i was outside input image; truncating to fit.");
    vcl_cerr << "WARNING: Crop region bbox upper corner i was outside input image; truncating to fit.\n";
    ni = ir->ni()-i0;
  }
  if (j0+nj > ir->nj())
  {
    MBL_LOG(WARN, logger(), "Crop region bbox upper corner j was outside input image; truncating to fit.");
    vcl_cerr << "WARNING: Crop region bbox upper corner j was outside input image; truncating to fit.\n";
    nj = ir->nj()-j0;
  }
  if (k0+nk > ir->nk())
  {
    MBL_LOG(WARN, logger(), "Crop region bbox upper corner k was outside input image; truncating to fit.");
    vcl_cerr << "WARNING: Crop region bbox upper corner k was outside input image; truncating to fit.\n";
    nk = ir->nk()-k0;
  }

  // Get a view of the cropped region
  vil3d_image_view_base_sptr ivbp = ir->get_copy_view(i0, ni, j0, nj, k0, nk);

  // Create output image resource
  vil3d_image_resource_sptr ir2 = vil3d_new_image_resource(
    img_dst().c_str(), ni, nj, nk, ivbp->nplanes(), ivbp->pixel_format(), filetype.c_str());
  if (!ir2)
  {
    MBL_LOG(ERR, logger(), "Failed to create output image resource");
    vcl_cerr << "ERROR: Failed to create output image resource\n";
    return 2;
  }
  MBL_LOG(INFO, logger(), "Created output image_resource");

  // Modify transform to account for the implicit translation
  vimt3d_transform_3d transl;
  transl.set_translation(-double(i0), -double(j0), -double(k0));
  tr = transl*tr;
  dynamic_cast<vimt3d_vil3d_v3i_image &>(*ir2).set_world2im(tr);
  
  if (logger().level()>=mbl_logger::DEBUG)
  {
    vgl_point_3d<double> img_orig_wc = tr.inverse()(vgl_point_3d<double>(0,0,0));
    MBL_LOG(DEBUG, logger(), "Image origin in world coords: " << img_orig_wc);
  }

  // Write the image to file
  bool succ = ir2->put_view(*ivbp);
  if (!succ)
  {
    MBL_LOG(ERR, logger(), "Failed to put_view into output image resource");
    vcl_cerr << "ERROR: Failed to put_view into output image resource\n";
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
