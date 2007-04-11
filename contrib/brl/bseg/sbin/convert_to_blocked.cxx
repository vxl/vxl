#include <vcl_string.h>
#include <vcl_vector.h>
#include<vcl_cstdio.h>
#include <vul/vul_sprintf.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vil/vil_load.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>

static void filenames_from_directory(vcl_string const& dirname,
                                     vcl_vector<vcl_string>& filenames)
{  vcl_string s(dirname);
 s += "/*.*";
 for (vul_file_iterator fit = s;fit; ++fit) {
   // check to see if file is a directory.
   if (vul_file::is_directory(fit()))
     continue;
   filenames.push_back(fit());
 }
}

static bool convert_to_blocked(vcl_string const& image_indir,
                               vcl_string const& image_outdir,
                               const unsigned bsize){
  vcl_vector<vcl_string> in_filenames;
  filenames_from_directory(image_indir, in_filenames);
  unsigned n_infiles = in_filenames.size();
  unsigned infile_counter = 0, frame = 0;
  vcl_string outfile = image_outdir + "/blk";
  vil_image_resource_sptr imgr;
  for (;infile_counter<n_infiles; ++infile_counter)
  {
    bool no_valid_image = true;
    while (no_valid_image)
    {
      if (infile_counter>=n_infiles)
        return false;
      imgr =
        vil_load_image_resource(in_filenames[infile_counter++].c_str());
      no_valid_image = !imgr||imgr->ni()==0||imgr->nj()==0;
    }
    frame++;
    //now we have a valid image file, convert it to blocked
    //the output file path

    vcl_string outname = vul_sprintf("%s%05d.%s", outfile.c_str()
                                     ,frame,"tif");
    vil_blocked_image_resource_sptr bimage;
    if (bsize>0)
      bimage = vil_new_blocked_image_facade(imgr, bsize, bsize);
    else
      bimage = vil_new_blocked_image_facade(imgr);
    vil_blocked_image_resource_sptr outimgr =
      vil_new_blocked_image_resource(outname.c_str(),
                                     bimage->ni(), bimage->nj(),
                                     bimage->nplanes(),
                                     bimage->pixel_format(),
                                     bimage->size_block_i(),
                                     bimage->size_block_j(),
                                     "tiff");
    for (unsigned j = 0; j<bimage->n_block_j(); ++j)
      for (unsigned i = 0; i<bimage->n_block_i(); ++i)
      {
        vil_image_view_base_sptr blk = bimage->get_block(i,j);
        if (!blk)
          return false;
        if (!outimgr->put_block(i, j, *blk))
          return false;
      }
  }
  return true;
}

int main(int argc,char * argv[])
{
  if (argc<3)
  {
    vcl_cout<<"Usage : convert_to_blocked.exe image_in_dir image_out_dir [block_size]\n";
    return -1;
  }
  vcl_string image_indir(argv[1]);
  vcl_string image_outdir(argv[2]);
  unsigned block_size = 0;//use default value
  if (argc==4)
    block_size = static_cast<unsigned>(atoi(argv[3]));
  if (!convert_to_blocked(image_indir, image_outdir, block_size))
  {
    vcl_cout << "Conversion failed\n";
    return -1;
  }
  return 0;
}
