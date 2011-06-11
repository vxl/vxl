#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include <vcl_sstream.h>
#include <vcl_string.h>
#include <vcl_algorithm.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_arg.h>
#include <vcl_vector.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_convert.h>
#include <vsol/vsol_box_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vcl_cmath.h>
#include <bwm/video/bwm_video_site_io.h>
#include <bwm/video/bwm_video_corr.h>

float avg_intensity(vil_image_view<float> & img, int rx, int ry, int u, int v)
{
    float avgintensity=0.0;
    for (int m=u-rx;m<=u+rx;++m)
        for (int p=v-ry;p<=v+ry;++p)
            avgintensity+=img(m,p);

    return avgintensity/((2*rx+1)*(2*ry+1));
}

void compute_corr_intensities(vcl_vector<vcl_string> img_files,
                              vcl_vector<bwm_video_corr_sptr> corrs,
                              vcl_vector<float> & mus,
                              vcl_vector<float> & counts,
                              int rx, int ry)
{
    for (unsigned i=0;i<img_files.size();i++)
    {
        vil_image_view_base_sptr img_ptr=vil_load(img_files[i].c_str());
        if (vil_image_view<vxl_byte> *img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(img_ptr.ptr()))
        {
            vil_image_view<float>* floatimg = new vil_image_view<float>(img_byte->ni(), img_byte->nj(), 1);
            vil_convert_stretch_range_limited(*img_byte, *floatimg, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);

            unsigned ni=img_ptr->ni();
            unsigned nj=img_ptr->nj();
            for (unsigned j=0;j<corrs.size();j++)
            {
                // check for top right quarter.
                vgl_point_2d<double> point2d;
                if (!corrs[j]->match(i,point2d)) continue;

                int u=(int)vcl_floor(point2d.x());
                int v=(int)vcl_floor(point2d.y());

                if (u>=(int)ni/2+rx && u+rx<(int)ni && v>ry && v+ry<(int)nj)
                {
                    // average value of a corr throughout the sequence
                    mus[j]+=avg_intensity(*floatimg,rx,ry,u,v);
                    counts[j]++;
                }
            }
        }
    }
}

int main(int argc, char** argv)
{
    //Input arguments
    vul_arg<vcl_string> site   ("-sitename", "Site Filename", "");
    vul_arg<int>        radiusx("-radiusx", "Radius along X direction", 3);
    vul_arg<int>        radiusy("-radiusy", "Radius along Y direction", 3);
    vul_arg<vcl_string>        outdir("-outdir", "Output Directory", "");
    vul_arg_parse(argc, argv);

    bwm_video_site_io video_site;

    video_site.open(site());
    vcl_string video_path=video_site.video_path();
    vcl_cout<<"Video Path "<<video_path<<vcl_endl;

    int rx=radiusx();
    int ry=radiusy();

    vcl_string frame_glob=vul_file::dirname(video_path)+"/*.??*";
    vcl_vector<bwm_video_corr_sptr> corrs=video_site.corrs();
    vul_file_iterator img_file_it(frame_glob.c_str());
    vcl_vector<vcl_string> img_files;
    while (img_file_it) {
        vcl_string imgName(img_file_it());
        img_files.push_back(imgName);
        ++img_file_it;
    }
    vcl_sort(img_files.begin(), img_files.end());
    vcl_vector<float> mus(corrs.size(),0.0);
    vcl_vector<float> counts(corrs.size(),0.0);

    compute_corr_intensities(img_files, corrs, mus, counts,rx,ry);

    for (unsigned i=0;i<img_files.size();i++)
    {
        vil_image_view_base_sptr img_ptr=vil_load(img_files[i].c_str());
        vcl_string imgname=outdir()+"/"+vul_file::basename(img_files[i]);
        if (vil_image_view<vxl_byte> *img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(img_ptr.ptr()))
        {
            vil_image_view<float>* floatimg = new vil_image_view<float>(img_byte->ni(), img_byte->nj(), 1);
            vil_convert_stretch_range_limited(*img_byte, *floatimg, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);

            unsigned ni=img_ptr->ni();
            unsigned nj=img_ptr->nj();
            float count=0;
            float summui=0.0;  float summuixi=0.0;
            float sumxi=0.0;   float sumxi2=0.0;

            for (unsigned j=0;j<corrs.size();++j)
            {
                // check for top right quarter.
                vgl_point_2d<double> point2d;
                if (!corrs[j]->match(i,point2d)) continue;
                int u=(int)vcl_floor(point2d.x());
                int v=(int)vcl_floor(point2d.y());
                if (u>=(int)ni/2+rx && u+rx<(int)ni && v>ry && v+ry<(int)nj)
                {
                    // avg intensity over a neighborhood.
                    float avgintensity=avg_intensity(*floatimg,rx,ry,u,v);
                    summuixi+=avgintensity*mus[j]/counts[j];
                    summui+=mus[j]/counts[j];;
                    sumxi+=avgintensity;
                    sumxi2+=avgintensity*avgintensity;
                    count++;
                }
            }
            if (count>=2)
            {
                float a=(summuixi-summui*sumxi/count)/(sumxi2-sumxi*sumxi/count);
                float b =(summui-a*sumxi)/count;
                vcl_cout<<"Count is "<<count<<"a "<<a<< " b "<<b<<vcl_endl;

                vil_math_scale_and_offset_values<float>(*floatimg,a,b);
                vil_convert_stretch_range_limited(*floatimg, *img_byte, 0.0f, 1.0f, vxl_byte(0), vxl_byte(255));
                vil_save(*img_byte,imgname.c_str());
            }
            else
                vcl_cout<<" Only "<<count << " correspondences"<<vcl_endl;
        }
    }

    return 0;
}
