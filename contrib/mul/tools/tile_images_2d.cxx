//: \file
//  \brief Program to tile several 2D images onto a single large 2D image

#include <vcl_exception.h>
#include <vcl_cstdlib.h>
#include <vcl_iostream.h>
#include <vul/vul_arg.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
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
static bool load_file_list(const vcl_string& filename,
                           vcl_vector<vcl_string>& list)
{
  list.clear();

  vcl_ifstream ifs(filename.c_str());
  if (!ifs || ifs.bad())
    return false;

  while (!ifs.eof() && ifs.good())
  {
    vcl_string str;
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
static unsigned load_image_2d(const vcl_string& filename,
                              vil_image_view<vil_rgb<vxl_byte> >& img_rgb)
{
  vil_image_resource_sptr ir = vil_load_image_resource(filename.c_str());
  if (!ir) 
    return 1;

  MBL_LOG(DEBUG, logger(), "pixel_type: " << ir->pixel_format());
  MBL_LOG(DEBUG, logger(), "size: " << ir->ni() << 'x' 
    << ir->nj() << " pixels x " << ir->nplanes() << "planes");

  if (ir->pixel_format()==VIL_PIXEL_FORMAT_BYTE &&  ir->nplanes()==3)
  {
    vil_image_view<vxl_byte> img = vil_convert_cast(vxl_byte(), ir->get_view());
    img_rgb = vil_view_as_rgb(img);
    MBL_LOG(DEBUG, logger(), "Loaded " << filename 
      << " 3-plane image<byte> into image<vil_rgb<byte> >");
  }
  else
  {
    vcl_cerr << "Image type not supported: " 
      << "format=" << ir->pixel_format() 
      << ", nplanes=" << ir->nplanes() << vcl_endl;
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
  vul_arg<vcl_string> image_list_file(0, "file containing a list of input image filenames");
  vul_arg<vcl_string> output_image_filename(0, "output image filename");
  vul_arg_parse(argc, argv);

  // Load the list of image filenames
  vcl_vector<vcl_string> image_list;
  if (!load_file_list(image_list_file(), image_list))
  {
    vcl_cerr << "Failed to load image list file" << vcl_endl;
    return 1;
  }

  // Load the images
  const unsigned nimgs = image_list.size();
  vcl_vector<vil_image_view<vil_rgb<vxl_byte> > > imgs(nimgs);
  for (unsigned i=0; i<nimgs; ++i)
  {
    load_image_2d(image_list[i], imgs[i]);
  }

  // Tile the images into a single big image
  vil_image_view<vil_rgb<vxl_byte> > big_image;
  vil_tile_images(big_image, imgs);

  // Save the big image
  vil_save(big_image, output_image_filename().c_str());

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
