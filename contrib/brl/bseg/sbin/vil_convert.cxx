#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <vcl_compiler.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view_base.h>

static void filenames_from_directory(std::string const& dirname,
                                     std::vector<std::string>& filenames)
{
  std::string s(dirname);
  s += "/*.*";
  for (vul_file_iterator fit = s;fit; ++fit) {
    // check to see if file is a directory.
    if (vul_file::is_directory(fit()))
      continue;
    filenames.push_back(fit());
  }
}

static bool convert_images(std::string const& image_indir,
                           std::string const& image_outdir,
                           const unsigned blocksize,
                           std::string const& basename)
{
  std::vector<std::string> in_filenames;
  filenames_from_directory(image_indir, in_filenames);
  unsigned n_infiles = in_filenames.size();
  std::string file;
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
    std::string infname = vul_file::strip_directory(file);
    infname = vul_file::strip_extension(infname);
    std::string outname;
    if (basename=="")
      outname = image_outdir+ '/' + infname + ".tif";
    else
    {
        std::stringstream cs;
        cs << cnt;
        outname = image_outdir+ '/' + basename + '.' + cs.str().c_str() + ".tif";
    }
    if (blocksize>0)
    {
      vil_blocked_image_resource_sptr bim =
        vil_new_blocked_image_resource(outname.c_str(),
                                       imgr->ni(), imgr->nj(),
                                       imgr->nplanes(),
                                       imgr->pixel_format(),
                                       blocksize, blocksize,
                                       "tiff");
      vil_image_view_base_sptr view = imgr->get_view();
      if (view)
        bim->vil_image_resource::put_view(*view);
      continue;
    }
    vil_save_image_resource(imgr, outname.c_str(), "tiff");
  }
  return true;
}

int main(int argc,char * argv[])
{
    if (argc<3)
    {
      std::cout<<"Usage : vil_convert.exe image_in_dir image_out_dir blocksize basename\n";
      return -1;
    }
    else
    {
      std::string image_indir(argv[1]);
      std::string image_outdir(argv[2]);

      unsigned blocksize = 0;
      if (argc==4)
        blocksize = std::atoi(argv[3]);
      std::string basename = "";
      if (argc==5)
        basename = argv[4];
      if (!convert_images(image_indir, image_outdir, blocksize, basename))
      {
        std::cout << "Convert failed\n";
        return -1;
      }
      return 0;
    }
}
