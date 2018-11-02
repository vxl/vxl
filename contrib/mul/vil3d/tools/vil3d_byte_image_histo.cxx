//:
// \file
// \brief Tool to load in a 3D image (assumed to be bytes) and compute histogram
// \author Tim Cootes

#include <iostream>
#include <fstream>
#include <vul/vul_arg.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil3d/vil3d_property.h>
#include <vil3d/vil3d_load.h>
#include <vil3d/vil3d_convert.h>
#include <vil3d/vil3d_slice.h>
#include <vil/vil_convert.h>
#include <vil/vil_save.h>
#include <vil/vil_crop.h>
#include <vil/vil_resample_bilin.h>
#include <vnl/vnl_math.h>
#include <vxl_config.h> // For vxl_byte
#include <vil3d/algo/vil3d_histogram.h>

void print_usage()
{
  std::cout<<"Tool to load in a 3D byte image and compute histogram."<<std::endl;
  std::cout<<"Image cast to bytes.  Histogram saved as text to named file."<<std::endl;
  std::cout<<"Format of file: Each line has intensity_value count"<<std::endl;
  vul_arg_display_usage_and_exit();
}


int main(int argc, char** argv)
{
  vul_arg<std::string> image_path("-i","3D image filename");
  vul_arg<std::string> output_path("-o","Output file for (text) histogram","image_histo.txt");

  vul_arg_parse(argc,argv);

  if (image_path()=="")
  {
    print_usage();
    return 0;
  }

  // Attempt to load in the 3D image
  vil3d_image_resource_sptr im_res = vil3d_load_image_resource(image_path().c_str());
  if (im_res==nullptr)
  {
    std::cerr<<"Failed to load in image from "<<image_path()<<'\n';
    return 1;
  }

  // Read in voxel size if available
  float width[3] = { 1.0f, 1.0f, 1.0f };
  im_res->get_property(vil3d_property_voxel_size, width);
  std::cout<<"Voxel sizes: "
          <<width[0]<<" x "<<width[1]<<" x "<<width[2]<<std::endl;
  std::cout<<"Each voxel has volume: "<<width[0]*width[1]*width[2]<<std::endl;

//  vil3d_image_view_base_sptr im_ptr = im_res->get_view();

  // Load the image data and cast to byte
  vil3d_image_view<vxl_byte> image3d
    = vil3d_convert_cast(vxl_byte(),im_res->get_view());

  if (image3d.size()==0)
  {
    std::cerr<<"Failed to load in image from "<<image_path()<<'\n';
    return 1;
  }
  std::cout<<"Image3D: "<<image3d<<std::endl;

  std::vector<double> histo(256);
  vil3d_histogram_byte(image3d,histo);

  std::ofstream ofs(output_path().c_str());
  if (!ofs)
  {
    std::cout<<"Couldn't open "<<output_path()<<" for output."<<std::endl;
    return 1;
  }
  for (unsigned i=0;i<histo.size();++i)
    ofs<<i<<" "<<histo[i]<<std::endl;
  ofs.close();
  std::cout<<"Histogram saved to "<<output_path()<<std::endl;

  return 0;
}
