#include <string>
#include <vector>
#include <iostream>
#include <limits>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_math.h>
#include <vil/vil_image_view.h>

static void filenames_from_directory(std::string const& dirname,
                                     std::vector<std::string>& filenames)
{
  std::string s(dirname);
  s += "/*.*";
  for (vul_file_iterator fit = s;fit; ++fit) {
    // check to see if file is a directory.
    if (vul_file::is_directory(fit()))
      continue;
    filenames.emplace_back(fit());
  }
}

static bool negate_images(std::string const& image_indir,
                          std::string const& image_outdir)
{
  std::vector<std::string> in_filenames;
  filenames_from_directory(image_indir, in_filenames);
  unsigned n_infiles = in_filenames.size();
  std::string file;
  for (unsigned int i=0; i<n_infiles; ++i)
  {
    bool no_valid_image = true;
    vil_image_resource_sptr imgr;
    while (no_valid_image)
    {
        file = in_filenames[i];
        imgr = vil_load_image_resource(file.c_str());
        no_valid_image = !imgr||imgr->ni()==0||imgr->nj()==0;
        if (no_valid_image&&i<n_infiles)
            i++;
        if (i>=n_infiles)
            return false;
    }
    if (!imgr)
      return false;
    vil_pixel_format fmt = imgr->pixel_format();
    vil_image_resource_sptr outr;
    switch (fmt)
    {
#define NEGATE_CASE(FORMAT, T) \
     case FORMAT: { \
      vil_image_view<T> view = imgr->get_copy_view(); \
      T mxv = std::numeric_limits<T>::max(); \
      vil_math_scale_and_offset_values(view, -1.0, mxv); \
      outr = vil_new_image_resource_of_view(view);  \
      break; \
                  }
     NEGATE_CASE(VIL_PIXEL_FORMAT_BYTE, vxl_byte);
     NEGATE_CASE(VIL_PIXEL_FORMAT_UINT_32, vxl_uint_32);
     NEGATE_CASE(VIL_PIXEL_FORMAT_UINT_16, vxl_uint_16);
     NEGATE_CASE(VIL_PIXEL_FORMAT_INT_16, vxl_int_16);
     NEGATE_CASE(VIL_PIXEL_FORMAT_FLOAT, float);
     NEGATE_CASE(VIL_PIXEL_FORMAT_DOUBLE, double);
#undef NEGATE_CASE
     default:
      std::cout << "Unknown image format\n";
      return false;
    }
    std::string infname = vul_file::strip_directory(file);
    std::string outname = image_outdir+ '/' + infname ;
    vil_save_image_resource(outr, outname.c_str(), "tiff");
  }
  return true;
}

int main(int argc,char * argv[])
{
    if (argc!=3)
    {
        std::cout<<"Usage : negate.exe image_in_dir image_out_dir\n";
        return -1;
    }
    else
    {
        std::string image_indir(argv[1]);
        std::string image_outdir(argv[2]);
        if (!negate_images(image_indir, image_outdir))
        {
          std::cout << "Negation failed\n";
          return -1;
        }
        return 0;
    }
}
