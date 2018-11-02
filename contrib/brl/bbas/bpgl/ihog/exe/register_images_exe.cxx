#include <string>
#include <vector>
#include <iostream>
#include <cstdio>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_bounding_box.h>
#include <vul/vul_sprintf.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vnl/vnl_matrix.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <ihog/ihog_minimizer.h>
#include <ihog/ihog_world_roi.h>
#include <ihog/ihog_transform_2d.h>
#include <ihog/ihog_transform_2d_sptr.h>
#include <ihog/ihog_sample_grid_bilin.h>


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

static bool read_homographies(std::string const& filename, std::vector<vnl_matrix<double> >& homographies)
{
  std::ifstream ifile(filename.c_str(),std::ios::in);
  std::cout<<"\n Reading Homographies "<<filename;

  if (!ifile)
  {
    std::cout<<"\n error opening file";
    std::cout.flush();
    return false;
  }
  char buffer[100];
  while (ifile.getline(buffer,100))
  {
    vnl_matrix<double> p(3,3);
    ifile>>p;
    std::cout<<p;
    homographies.push_back(p);
    ifile.getline(buffer,100);
  }
  std::cout.flush();
  return true;
}

static bool register_images(std::string const& homography_file,
                            std::string const& image_indir,
                            std::string const& image_outdir)
{
  int bimg_ni;
  int bimg_nj;

  int offset_i;
  int offset_j;

  vbl_bounding_box<double,2> box;
  std::vector<vnl_matrix<double> > homographies;
  read_homographies(homography_file, homographies);
  unsigned nframes = homographies.size();
  if (!nframes)
  {
    std::cout << "no transforms to use in registration\n";
    return false;
  }
  std::vector<std::string> in_filenames;
  filenames_from_directory(image_indir, in_filenames);
  unsigned n_infiles = in_filenames.size();
  unsigned infile_counter = 0;
  //read the first image
  bool no_valid_image = true;
  vil_image_resource_sptr imgr;
  while (no_valid_image)
  {
    imgr =
      vil_load_image_resource(in_filenames[infile_counter++].c_str());
    no_valid_image = !imgr||imgr->ni()==0||imgr->nj()==0;
    if (infile_counter>=n_infiles)
      return false;
  }
  unsigned ni =  imgr->ni(), nj =  imgr->nj();
  infile_counter = 0;//return to first frame

  std::vector<ihog_transform_2d > xforms;
  for (unsigned i=0;i<nframes;i++)
  {
    ihog_transform_2d p;
    vnl_double_2x3 M23(homographies[i][0][0],homographies[i][0][1],homographies[i][0][2],
                       homographies[i][1][0],homographies[i][1][1],homographies[i][1][2]);
    p.set_affine(M23);
    xforms.push_back(p);
    box.update(p(0,0).x(),p(0,0).y());
    box.update(p(0,nj).x(),p(0,nj).y());
    box.update(p(ni,0).x(),p(ni,0).y());
    box.update(p(ni,nj).x(),p(ni,nj).y());
  }

  bimg_ni=(int)std::ceil(box.max()[0]-box.min()[0]);
  bimg_nj=(int)std::ceil(box.max()[1]-box.min()[1]);

  offset_i=(int)std::ceil(0-box.min()[0]);
  offset_j=(int)std::ceil(0-box.min()[1]);

  std::string outfile = image_outdir + "/reg";

  for (unsigned frame = 0;frame<nframes; ++frame)
  {
    no_valid_image = true;
    while (no_valid_image)
    {
      imgr =
        vil_load_image_resource(in_filenames[infile_counter++].c_str());
      no_valid_image = !imgr||imgr->ni()==0||imgr->nj()==0;
      if (infile_counter>=n_infiles)
      {
        std::cout << "Number of homographies and input images do not match\n";
        return false;
      }
    }
    vil_image_view<float> curr_view =
      *vil_convert_cast(float(), imgr->get_view());
    ihog_transform_2d ftxform=xforms[frame].inverse();
    ihog_image<float> sample_im;

    vgl_point_2d<double> p(-offset_i,-offset_j);
    vgl_vector_2d<double> u(1,0);
    vgl_vector_2d<double> v(0,1);

    ihog_image<float> curr_img(curr_view,ftxform);
    ihog_resample_bilin(curr_img,sample_im,p,u,v,bimg_ni,bimg_nj);

    vil_image_view<vxl_byte> temp;
    vil_convert_stretch_range(sample_im.image(), temp);
    std::string outname = vul_sprintf("%s%05d.%s", outfile.c_str(),
                                     frame,
                                     "tif");
    vil_save(temp, outname.c_str());
  }
  return false;
}

int main(int argc,char * argv[])
{
  if (argc!=4)
  {
    std::cout<<"Usage : register_images.exe homography_file image_in_dir image_out_dir\n";
    return -1;
  }
  else
  {
    std::string homography_name(argv[1]);
    std::string image_indir(argv[2]);
    std::string image_outdir(argv[3]);
    if (!register_images(homography_name, image_indir, image_outdir))
    {
      std::cout << "Registration failed\n";
      return -1;
    }
    else
      return 0;
  }
}
