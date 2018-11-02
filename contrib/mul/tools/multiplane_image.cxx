//: \file
//  \author Kevin de Souza
//  \date 30 October 2008
//  \brief Combine 2 images into a single image with multiple planes

#include <iostream>
#include <exception>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_arg.h>
#include <mbl/mbl_log.h>
#include <vil/vil_pixel_format.h>
#include <vimt3d/vimt3d_load.h>
#include <vimt3d/vimt3d_save.h>
#include <vimt3d/vimt3d_transform_3d.h>
#include <vimt3d/vimt3d_add_all_loaders.h>


//=========================================================================
// Static function to create a static logger when first required
//=========================================================================
static mbl_logger& logger()
{
  static mbl_logger l("mul.tools.multiplane_image");
  return l;
}


//========================================================================
//========================================================================
static bool load_image_resource_info(const std::string& filename,
                                     const bool use_mm,
                                     vil3d_image_resource_sptr& ir,
                                     vimt3d_transform_3d& w2i)
{
  ir = vil3d_load_image_resource(filename.c_str());
  if (!ir)
  {
    std::cerr << "ERROR: failed to load image resource for " << filename << '\n';
    return false;
  }
  w2i = vimt3d_load_transform(ir, use_mm);
  return true;
}


//========================================================================
// Actual main function
//========================================================================
int main2(int argc, char*argv[])
{
  vimt3d_add_all_loaders();

  // Parse the program arguments
  vul_arg<std::string> img1(nullptr, "input image1 filename");
  vul_arg<std::string> img2(nullptr, "input image2 filename");
  vul_arg<std::string> img_out(nullptr, "output image filename");
  vul_arg<bool> use_mm("-mm", "Image transform in millimetres (default=metres)", false);
  vul_arg<float> s1("-s1", "Scaling factor to multiply voxels in image1", 1.0f);
  vul_arg<float> s2("-s2", "Scaling factor to multiply voxels in image2", 1.0f);
  vul_arg_parse(argc, argv);

  // Load first image
  vil3d_image_resource_sptr ir1 = nullptr;
  vimt3d_transform_3d w2i;
  if (!load_image_resource_info(img1(), use_mm(), ir1, w2i))
    return 1;
  const unsigned ni = ir1->ni();
  const unsigned nj = ir1->nj();
  const unsigned nk = ir1->nk();
  const vil_pixel_format fmt = ir1->pixel_format();
  const unsigned np1 = ir1->nplanes();
  MBL_LOG(INFO, logger(), "Input image 1: ");
  MBL_LOG(INFO, logger(), "ni: " << ni);
  MBL_LOG(INFO, logger(), "nj: " << nj);
  MBL_LOG(INFO, logger(), "nk: " << nk);
  MBL_LOG(INFO, logger(), "np: " << np1);
  MBL_LOG(INFO, logger(), "pixfmt: " << fmt);
  MBL_LOG(INFO, logger(), "scaling: " << s1());

  // Second image must have the same dimensions, transform and pixel type
  vil3d_image_resource_sptr ir2 = nullptr;
  vimt3d_transform_3d w2i2;
  if (!load_image_resource_info(img2(), use_mm(), ir2, w2i2))
    return 1;
  const unsigned ni2 = ir2->ni();
  const unsigned nj2 = ir2->nj();
  const unsigned nk2 = ir2->nk();
  const vil_pixel_format fmt2 = ir2->pixel_format();
  const unsigned np2 = ir2->nplanes();
  MBL_LOG(INFO, logger(), "Input image 2: ");
  MBL_LOG(INFO, logger(), "ni: " << ni2);
  MBL_LOG(INFO, logger(), "nj: " << nj2);
  MBL_LOG(INFO, logger(), "nk: " << nk2);
  MBL_LOG(INFO, logger(), "np: " << np2);
  MBL_LOG(INFO, logger(), "pixfmt: " << fmt2);
  MBL_LOG(INFO, logger(), "scaling: " << s2());

  if (!(w2i2==w2i) || ni2!=ni || nj2!=nj || nk2!=nk || fmt2!=fmt)
  {
    std::cerr << "ERROR: input images must have same dimensions, transform and pixel type" << '\n';
    return 2;
  }

  // Output image will have the same dimensions, transform and pixel type as first input image.
  // Number of planes will be the total of both images
  const unsigned np = np1 + np2;
  vimt3d_image_3d_of<float> img(ni, nj, nk, np, w2i);
  MBL_LOG(INFO, logger(), "Output image: ");
  MBL_LOG(INFO, logger(), "ni: " << ni);
  MBL_LOG(INFO, logger(), "nj: " << nj);
  MBL_LOG(INFO, logger(), "nk: " << nk);
  MBL_LOG(INFO, logger(), "np: " << np);
  MBL_LOG(INFO, logger(), "pixfmt (hardcoded): float");

  vil3d_image_view<float> imgview;

  // Copy image1 data
  imgview = ir1->get_view();
  for (unsigned p=0; p<np1; ++p)
  {
    const unsigned pp = p;
    for (unsigned k=0; k<nk; ++k)
    {
      for (unsigned j=0; j<nj; ++j)
      {
        for (unsigned i=0; i<ni; ++i)
        {
          img.image()(i,j,k,pp) = s1() * imgview(i,j,k,p);
        }
      }
    }
  }

  // Copy image2 data
  imgview = ir2->get_view();
  for (unsigned p=0; p<np2; ++p)
  {
    const unsigned pp = np1 + p;
    for (unsigned k=0; k<nk2; ++k)
    {
      for (unsigned j=0; j<nj2; ++j)
      {
        for (unsigned i=0; i<ni2; ++i)
        {
          img.image()(i,j,k,pp) = s2() * imgview(i,j,k,p);
        }
      }
    }
  }

  // Write the combined image to disk
  vimt3d_save(img_out(), img, use_mm());
  std::cout << "Wrote combined image to file: " << img_out() << std::endl;

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
