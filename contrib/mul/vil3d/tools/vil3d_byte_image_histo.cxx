//:
// \file
// \brief Tool to load in a 3D image (assumed to be bytes) and compute histogram
// \author Tim Cootes

#include <vul/vul_arg.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
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
  vcl_cout<<"Tool to load in a 3D byte image and compute histogram."<<vcl_endl;
  vcl_cout<<"Image cast to bytes.  Histogram saved as text to named file."<<vcl_endl;
  vcl_cout<<"Format of file: Each line has intensity_value count"<<vcl_endl;
  vul_arg_display_usage_and_exit();
}


int main(int argc, char** argv)
{
  vul_arg<vcl_string> image_path("-i","3D image filename");
  vul_arg<vcl_string> output_path("-o","Output file for (text) histogram","image_histo.txt");

  vul_arg_parse(argc,argv);

  if (image_path()=="")
  {
    print_usage();
    return 0;
  }

  // Attempt to load in the 3D image
  vil3d_image_resource_sptr im_res = vil3d_load_image_resource(image_path().c_str());
  if (im_res==VXL_NULLPTR)
  {
    vcl_cerr<<"Failed to load in image from "<<image_path()<<'\n';
    return 1;
  }

  // Read in voxel size if available
  float width[3] = { 1.0f, 1.0f, 1.0f };
  im_res->get_property(vil3d_property_voxel_size, width);
  vcl_cout<<"Voxel sizes: "
          <<width[0]<<" x "<<width[1]<<" x "<<width[2]<<vcl_endl;
  vcl_cout<<"Each voxel has volume: "<<width[0]*width[1]*width[2]<<vcl_endl;

//  vil3d_image_view_base_sptr im_ptr = im_res->get_view();

  // Load the image data and cast to byte
  vil3d_image_view<vxl_byte> image3d
    = vil3d_convert_cast(vxl_byte(),im_res->get_view());

  if (image3d.size()==0)
  {
    vcl_cerr<<"Failed to load in image from "<<image_path()<<'\n';
    return 1;
  }
  vcl_cout<<"Image3D: "<<image3d<<vcl_endl;

  vcl_vector<double> histo(256);
  vil3d_histogram_byte(image3d,histo);

  vcl_ofstream ofs(output_path().c_str());
  if (!ofs)
  {
    vcl_cout<<"Couldn't open "<<output_path()<<" for output."<<vcl_endl;
    return 1;
  }
  for (unsigned i=0;i<histo.size();++i)
    ofs<<i<<" "<<histo[i]<<vcl_endl;
  ofs.close();
  vcl_cout<<"Histogram saved to "<<output_path()<<vcl_endl;

  return 0;
}
