//: \file
//  \brief Program to tile several 2D images onto a single large 2D image

#include <exception>
#include <iostream>
#include <cstdlib>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_arg.h>
#include <mbl/mbl_log.h>
#include <vil/vil_convert.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_view_as.h>
#include <vil/algo/vil_tile_images.h>
#include <vimt/vimt_load.h>
#include <vimt/vimt_add_all_binary_loaders.h>


//=========================================================================
//: Static function to create a static logger when first required
static mbl_logger& logger()
{
  static mbl_logger l("mul.tools.tile_images_2d");
  return l;
}


//========================================================================
//: Read a list of filenames from a file
// \param filename The name of a file containing a list of filenames
// \retval list The list of filenames (any previous contents will be overwritten)
static bool load_file_list(const std::string& filename,
                           std::vector<std::string>& list)
{
  list.clear();

  std::ifstream ifs(filename.c_str());
  if (!ifs || ifs.bad())
    return false;

  while (!ifs.eof() && ifs.good())
  {
    std::string str;
    ifs >> str;
    if (ifs.good() && !str.empty())
      list.push_back(str);
  }

  ifs.close();
  return true;
}


//========================================================================
//: Load a 2D image
// \note Currently only implemented for 3-plane, byte images -> rgb<byte>
static unsigned load_image_2d(const std::string& filename,
                              vil_image_view<vil_rgb<vxl_byte> >& img_rgb)
{
  vil_image_resource_sptr ir = vil_load_image_resource(filename.c_str());
  if (!ir)
    return 1;

  MBL_LOG(DEBUG, logger(), "pixel_type: " << ir->pixel_format());
  MBL_LOG(DEBUG, logger(), "size: " << ir->ni() << 'x' << ir->nj() << " pixels x " << ir->nplanes() << "planes");

  if (ir->pixel_format()==VIL_PIXEL_FORMAT_BYTE &&  ir->nplanes()==3)
  {
    vil_image_view<vxl_byte> img = vil_convert_cast(vxl_byte(), ir->get_view());
    img_rgb = vil_view_as_rgb(img);
    MBL_LOG(DEBUG, logger(), "Loaded " << filename << " 3-plane image<byte> into image<vil_rgb<byte> >");
  }
  else
  {
    std::cerr << "Image type not supported: "
             << "format=" << ir->pixel_format()
             << ", nplanes=" << ir->nplanes() << std::endl;
    return 2;
  }

  return 0;
}


//========================================================================
// Actual main function
//========================================================================
int main2(int argc, char*argv[])
{
  vimt_add_all_binary_loaders();

  // Parse the program arguments
  vul_arg<std::string> image_list_file(nullptr, "file containing a list of input image filenames");
  vul_arg<std::string> output_image_filename(nullptr, "output image filename");
  vul_arg_parse(argc, argv);

  // Load the list of image filenames
  std::vector<std::string> image_list;
  if (!load_file_list(image_list_file(), image_list))
  {
    std::cerr << "Failed to load image list file\n";
    return 1;
  }

  // Load the images
  const unsigned nimgs = image_list.size();
  std::vector<vil_image_view<vil_rgb<vxl_byte> > > imgs(nimgs);
  for (unsigned t=0; t<nimgs; ++t)
  {
    load_image_2d(image_list[t], imgs[t]);
  }
  MBL_LOG(DEBUG, logger(), "Loaded " << nimgs << " images");

  // Check that images are all the same size
  const unsigned ni = imgs[0].ni();
  const unsigned nj = imgs[0].nj();
  const unsigned np = imgs[0].nplanes();
  for (unsigned t=1; t<nimgs; ++t)
  {
    if (imgs[t].ni()!=ni || imgs[t].nj()!=nj || imgs[t].nplanes()!=np)
    {
      std::cerr << "ERROR: Images are not all the same size\n";
      return 2;
    }
  }
  MBL_LOG(DEBUG, logger(), "Images are all the same size");

  // Tile the images into a single big image
  vil_image_view<vil_rgb<vxl_byte> > big_image;
  vil_tile_images(big_image, imgs);
  MBL_LOG(DEBUG, logger(), "Tiled the images");

  // Save the big image
  vil_save(big_image, output_image_filename().c_str());
  MBL_LOG(DEBUG, logger(), "Saved the tiled image");

  return 0;
}


//========================================================================
// Exception-handling wrapper around main function
//========================================================================
int main(int argc, char*argv[])
{
  try
  {
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
