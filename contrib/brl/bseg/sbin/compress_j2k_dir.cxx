#include <string>
#include <iostream>
#include <cstdlib>
#include <vul/vul_file.h>
#include <vul/vul_timer.h>
#include <vul/vul_file_iterator.h>
#include <vil/vil_load.h>
#include <vil/file_formats/vil_j2k_image.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//assumes that the base image is in the dir and the dir is
//named for the image without the extension.
static bool compress_images(std::string const& in_image_dir,
                            std::string const& out_image_dir,
                            const unsigned compression_ratio)
{
  std::string slash;
  //generate the temporary dir
#ifdef _WIN32
  slash =  "\\";
#else
  slash = "/";
#endif
  bool found_file = false;
  std::string glob = in_image_dir + slash + "*.*";
  if (!vul_file::is_directory(out_image_dir))
    if (!vul_file::make_directory(out_image_dir)){
      std::cout << "Unable to find or make directory\n";
      return false;
    }
  for (vul_file_iterator fn = glob.c_str(); fn; ++fn)
  {
    char const* in_file = fn();
    if (vul_file::is_directory(in_file)||!vil_load_image_resource(in_file))
      continue;
    found_file = true;
    std::string file_noext = vul_file::strip_extension(in_file);
    std::string file_no_dir = vul_file::strip_directory(file_noext);
    std::string file_j2k = file_no_dir + ".j2k";
    std::string out_file = out_image_dir + slash + file_j2k;
    std::cout << " Compressing " << in_file << " to " << out_file
             << '\n' << std::flush;
    vul_timer t;
    if (! vil_j2k_image::s_encode_jpeg2000(in_file, out_file.c_str(),
                                           compression_ratio,
                                           1024, true)){
      std::cout << " Compression failed for " << in_file  << '\n';
      return false;
    }
    std::cout << " Compressed image in " << t.real()/1000 << " seconds\n"
             << std::flush;
  }
  if (!found_file)
  {
    std::cout << "Unable to find image file\n";
      return false;
  }
  return true;
}

int main(int argc,char * argv[])
{
  if (argc<4)
  {
    std::cout<<"Usage : compress_j2k_dir.exe in_image_dir out_image_dir compression_ratio\n";
    return -1;
  }
  else
  {
    std::string in_image_dir(argv[1]);
    std::string out_image_dir(argv[2]);
    unsigned compression_ratio = std::atoi(argv[3]);
    std::cout << "in dir = " << in_image_dir << std::endl
             << "out_dir =" << out_image_dir << std::endl
             << "target compression ratio " << compression_ratio << std::endl;
    if (!compress_images(in_image_dir, out_image_dir, compression_ratio))
      return -1;
    else
      return 0;
  }
}
