#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <cmath>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_arg.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view.h>

#include <vsol/vsol_box_2d.h>
#include <vgl/vgl_point_2d.h>
#include <bwm/video/bwm_video_site_io.h>
#include <bwm/video/bwm_video_corr.h>

int main(int argc, char** argv)
{
  //Input arguments
  vul_arg<std::string> site   ("-sitename", "Site Filename", "");
  vul_arg<std::string> gtdir  ("-gtdir", "Directory to store the ground truth", "");
  vul_arg<int>        radiusx("-radiusx", "Radius along X direction", 3);
  vul_arg<int>        radiusy("-radiusy", "Radius along Y direction", 3);
  vul_arg_parse(argc, argv);

  bwm_video_site_io video_site;

  video_site.open(site());
  std::string video_path=video_site.video_path();
  std::cout<<"Video Path "<<video_path<<std::endl;

  int rx=radiusx();
  int ry=radiusy();

  std::string frame_glob=vul_file::dirname(video_path)+"/*.??*";
  std::vector<bwm_video_corr_sptr> corrs=video_site.corrs();
  vul_file_iterator img_file_it(frame_glob.c_str());
  std::vector<std::string> img_files;
  while (img_file_it) {
    std::string imgName(img_file_it());
    img_files.push_back(imgName);
    ++img_file_it;
  }
  std::sort(img_files.begin(), img_files.end());

  for (unsigned i=0;i<img_files.size();i++)
  {
    vil_image_resource_sptr img_ptr=vil_load_image_resource(img_files[i].c_str());
    vil_image_view<unsigned char> outimg(img_ptr->ni(),img_ptr->nj());
    outimg.fill(0);
    for (auto & corr : corrs)
    {
      vgl_point_2d<double> point2d;
      if (corr->match(i,point2d))
      {
        int u=(int)std::floor(point2d.x());
        int v=(int)std::floor(point2d.y());

        for (int i=u-rx;i<=u+rx;i++)
          for (int j=v-ry;j<=v+ry;j++)
            if (i>=0 && j>=0 && i<(int)outimg.ni() && j<(int)outimg.nj())
              outimg(i,j)=255;
      }
    }
    std::string outfile=gtdir()+"/"+vul_file::strip_extension(vul_file::basename(img_files[i]))+".tiff";
    vil_save(outimg,outfile.c_str());
  }
  return 0;
}
