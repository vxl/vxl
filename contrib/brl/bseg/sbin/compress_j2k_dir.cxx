#include <vul/vul_file.h>
#include <vul/vul_timer.h>
#include <vul/vul_file_iterator.h>
#include <vil/vil_load.h>
#include <vil/file_formats/vil_j2k_image.h>
#include <vcl_string.h>
#include <vcl_cstdlib.h>

//assumes that the base image is in the dir and the dir is
//named for the image without the extension.
static bool compress_images(vcl_string const& in_image_dir,
                            vcl_string const& out_image_dir,
                            const unsigned compression_ratio)
{
  vcl_string slash;
  //generate the temporary dir
#ifdef VCL_WIN32
  slash =  "\\";
#else
  slash = "/";
#endif
  bool found_file = false;
  vcl_string glob = in_image_dir + slash + "*.*";
  if (!vul_file::is_directory(out_image_dir))
    if (!vul_file::make_directory(out_image_dir)){
      vcl_cout << "Unable to find or make directory\n";
      return false;
    }
  for (vul_file_iterator fn = glob.c_str(); fn; ++fn)
  {
    char const* in_file = fn();
    if (vul_file::is_directory(in_file)||!vil_load_image_resource(in_file))
      continue;
    found_file = true;
    vcl_string file_noext = vul_file::strip_extension(in_file);
    vcl_string file_no_dir = vul_file::strip_directory(file_noext);
    vcl_string file_j2k = file_no_dir + ".j2k";
    vcl_string out_file = out_image_dir + slash + file_j2k;
    vcl_cout << " Compressing " << in_file << " to " << out_file
             << '\n' << vcl_flush;
    vul_timer t;
    if (! vil_j2k_image::s_encode_jpeg2000(in_file, out_file.c_str(),
                                           compression_ratio,
                                           1024, true)){
      vcl_cout << " Compression failed for " << in_file  << '\n';
      return false;
    }
    vcl_cout << " Compressed image in " << t.real()/1000 << " seconds\n"
             << vcl_flush;
  }
  if (!found_file)
  {
    vcl_cout << "Unable to find image file\n";
      return false;
  }
  return true;
}

int main(int argc,char * argv[])
{
  if (argc<4)
  {
    vcl_cout<<"Usage : compress_j2k_dir.exe in_image_dir out_image_dir compression_ratio\n";
    return -1;
  }
  else
  {
    vcl_string in_image_dir(argv[1]);
    vcl_string out_image_dir(argv[2]);
    unsigned compression_ratio = vcl_atoi(argv[3]);
    vcl_cout << "in dir = " << in_image_dir << vcl_endl
             << "out_dir =" << out_image_dir << vcl_endl
             << "target compression ratio " << compression_ratio << vcl_endl;
    if (!compress_images(in_image_dir, out_image_dir, compression_ratio))
      return -1;
    else
      return 0;
  }
}
