#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_cstdlib.h>
// not used? #include <vcl_sstream.h>
#include <vpl/vpl.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vil/vil_load.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_pixel_traits.h>
#include <vil/vil_new.h>

static void filenames_from_directory(vcl_string const& dirname,
                                     vcl_vector<vcl_string>& filenames)
{
  vcl_string s(dirname);
  s += "/*.*";
  for (vul_file_iterator fit = s;fit; ++fit) {
    // check to see if file is a directory.
    if (vul_file::is_directory(fit()))
      continue;
    filenames.push_back(fit());
  }
}

static bool generate_pyramids(vcl_string const& image_indir,
                              vcl_string const& image_outdir,
                              const unsigned nlevels,
                              const unsigned blocksize)
{
  vcl_string slash;
  //generate the temporary dir
#ifdef VCL_WIN32
  slash =  "\\";
#else
  slash = "/";
#endif
  vcl_string tempdir = image_outdir + slash + "temp";
  vul_file::make_directory(tempdir.c_str());
  vcl_vector<vcl_string> in_filenames;
  filenames_from_directory(image_indir, in_filenames);
  unsigned n_infiles = in_filenames.size();
  vcl_string file;
  unsigned cnt = 0;
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
    cnt++;
    vcl_string infname = vul_file::strip_directory(file);
    infname = vul_file::strip_extension(infname);
    vcl_string outname;
    outname = image_outdir+ '/' + infname + ".tif";
    vil_blocked_image_resource_sptr bim;
    if (blocksize>0)
      bim = vil_new_blocked_image_facade(imgr, blocksize, blocksize);
    else
      bim = vil_new_blocked_image_facade(imgr, 256, 256);

    vil_pyramid_image_resource_sptr pir =
      vil_new_pyramid_image_from_base(outname.c_str(), bim.ptr(), nlevels, "tiff", tempdir.c_str());
  }
  vpl_rmdir(tempdir.c_str());
  return true;
}

int main(int argc,char * argv[])
{
    if (argc<4)
    {
      vcl_cout<<"Usage : generate_tiff_pyramids.exe base_image_in_dir pyr_image_out_dir nlevels blocksize\n";
      return -1;
    }
    else
    {
      vcl_string image_indir(argv[1]);
      vcl_string image_outdir(argv[2]);

      unsigned nlevels = vcl_atoi(argv[3]);
      if (nlevels<2)
      {
        vcl_cout << "Must have at least 2 levels\n";
        return 0;
      }
      unsigned blocksize = 0;
      if (argc==5)
        blocksize = vcl_atoi(argv[4]);
      if (!generate_pyramids(image_indir, image_outdir, nlevels, blocksize))
      {
        vcl_cout << "Generate Pyramids failed\n";
        return -1;
      }
      return 0;
    }
}
