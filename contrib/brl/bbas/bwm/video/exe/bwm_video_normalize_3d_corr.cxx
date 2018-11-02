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
#include <vil/vil_math.h>
#include <vil/vil_convert.h>
#include <vsol/vsol_box_2d.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#ifdef BWM_HAS_PROJECT_MESHES
  #include <bwm/bwm_observer_cam.h>
#endif
#include <vpgl/vpgl_perspective_camera.h>

float avg_intensity(vil_image_view<float> & img, int rx, int ry, int u, int v)
{
    float avgintensity=0.0;
    for (int m=u-rx;m<=u+rx;++m)
        for (int p=v-ry;p<=v+ry;++p)
            avgintensity+=img(m,p);

    return avgintensity/(2*rx+1.0f)/(2*ry+1.0f);
}

bool compute_correspondences(std::vector<std::string> img_files,
                             std::vector<std::string> cam_files,
                             std::vector<std::string> obj_files,
                             std::vector<std::vector<bool> > & mask,
                             std::vector<float> & corr_int)
{
    for (unsigned i=0; i<img_files.size(); ++i)
    {
        vil_image_view_base_sptr img_ptr=vil_load(img_files[i].c_str());
        std::vector<bool>  mask_j(obj_files.size(),false);
        if (vil_image_view<unsigned short> *img_byte = dynamic_cast<vil_image_view<unsigned short>*>(img_ptr.ptr()))
        {
            vil_image_view<float>* floatimg = new vil_image_view<float>(img_byte->ni(), img_byte->nj(), 1);
            vil_convert_stretch_range_limited<unsigned short>(*img_byte, *floatimg, (unsigned short)29000, (unsigned short) 34000, 0.0f, 1.0f);

            std::ifstream ifs(cam_files[i].c_str());
            if (!ifs.is_open()) {
                std::cerr << "Failed to open file " << cam_files[i] << '\n';
                return false;
            }

            vpgl_perspective_camera<double>* cam = new vpgl_perspective_camera<double>();
            ifs >> *cam;
            ifs.close();
            std::vector<vgl_polygon<double> > poly_2d_list;
#ifdef BWM_HAS_PROJECT_MESHES
            bwm_project_meshes(obj_files, cam, poly_2d_list);
#endif
            //scan through polygons, retrieve contained pixels
            for (unsigned j=0; j < poly_2d_list.size(); ++j)
            {
                vgl_polygon<double> this_poly = poly_2d_list[j];
                vgl_polygon_scan_iterator<double> psi(this_poly);

                float meanintensity=0.0;
                float countintensity=0.0;
                //save the polygon x-y coordinates
                for (psi.reset(); psi.next();)
                {
                    unsigned int y = psi.scany();
                    if (y<floatimg->nj())
                        for (unsigned int x = psi.startx(); (int) x<= psi.endx() && x<floatimg->ni(); ++x)
                        {
                            meanintensity+=(*floatimg)(x,y);
                            ++countintensity;
                            mask_j[j]=true;
                        }
                }
                if (mask_j[j])
                    corr_int.push_back(meanintensity/countintensity);
            }
        }
        mask.push_back(mask_j);
    }
    return true;
}

// This code normalizes images (adjust gain and offset) based on 3-d ocrrespondences and cameras.
int main(int argc, char** argv)
{
    //Input arguments
    vul_arg<std::string> camdir   ("-camdir", "Camera Directory", "");
    vul_arg<std::string> imgdir   ("-imgdir", "Image Directory", "");
    vul_arg<std::string> objdir   ("-objdir", "Mesh Directory", "");
    vul_arg<std::string> outdir   ("-outdir", "Output Directory", "");
    vul_arg_parse(argc, argv);

    std::string frame_glob=vul_file::dirname(imgdir())+"/*.???";
    std::string cam_glob=vul_file::dirname(camdir())+"/*.???";
    std::string obj_glob=vul_file::dirname(objdir())+"/*.???";

    vul_file_iterator img_file_it(frame_glob.c_str());
    vul_file_iterator cam_file_it(cam_glob.c_str());
    vul_file_iterator obj_file_it(obj_glob.c_str());

    std::vector<std::string> img_files;
    std::vector<std::string> cam_files;
    while (img_file_it && cam_file_it) {
        std::string imgname(img_file_it());
        std::string camname(cam_file_it());
        img_files.push_back(imgname);
        cam_files.push_back(camname);
        ++img_file_it;
        ++cam_file_it;
    }
    std::sort(img_files.begin(), img_files.end());
    std::sort(cam_files.begin(), cam_files.end());
    std::vector<std::string> obj_files;
    while (obj_file_it) {
        std::string objname(obj_file_it());
        obj_files.push_back(objname);
        ++obj_file_it;
    }

    std::vector<std::vector<bool> > mask;
    std::vector<float> corr_int;

    // project polygons in the images and record corresponding intensities.
    if (!compute_correspondences(img_files,cam_files,obj_files,mask,corr_int))
        return -1;

    std::vector<float> mean_ref_intensity(obj_files.size(),0.0);
    std::vector<float> stdev_ref_intensity(obj_files.size(),0.0);
    std::vector<float> mean_ref_count(obj_files.size(),0.0);
    int pointindex=0;
    for (unsigned i=0;i<mask.size();++i)
    {
        for (unsigned j=0;j<mask[i].size();++j)
        {
            if (mask[i][j])
            {
                float curr_intensity=corr_int[pointindex++];
                mean_ref_intensity[j]+=curr_intensity;
                stdev_ref_intensity[j]+=(curr_intensity*curr_intensity);
                mean_ref_count[j]++;
            }
        }
    }
    for (unsigned j=0;j<mean_ref_intensity.size();++j)
    {
        mean_ref_intensity[j]/=mean_ref_count[j];
        stdev_ref_intensity[j]=std::sqrt((stdev_ref_intensity[j]/mean_ref_count[j]-mean_ref_intensity[j]*mean_ref_intensity[j]));
        std::cout<<"Mean "<<mean_ref_intensity[j]<<" Std "<<stdev_ref_intensity[j]<<std::endl;
    }
    pointindex=0;
    for (unsigned i=0;i<img_files.size();++i)
    {
        vil_image_view_base_sptr img_ptr=vil_load(img_files[i].c_str());
        std::string imgname=outdir()+"/"+vul_file::basename(img_files[i]);
        if (vil_image_view<unsigned short> *img_byte = dynamic_cast<vil_image_view<unsigned short>*>(img_ptr.ptr()))
        {
            vil_image_view<float>* floatimg = new vil_image_view<float>(img_byte->ni(), img_byte->nj(), 1);
            vil_convert_stretch_range_limited<unsigned short>(*img_byte, *floatimg, (unsigned short)5500, (unsigned short) 6600, 0.0f, 1.0f);

            float summui=0.0;  float summuixi=0.0;
            float sumxi=0.0;   float sumxi2=0.0;

            float count=0.0;
            for (unsigned j=0;j<mask[i].size();++j)
            {
                // check for top right quarter.
                if (!mask[i][j]) continue;
                // avg intensity over a neighborhood.
                float curr_pt_intensity=corr_int[pointindex++];
                summuixi+=curr_pt_intensity*mean_ref_intensity[j];
                summui+=mean_ref_intensity[j];
                sumxi+=curr_pt_intensity;
                sumxi2+=curr_pt_intensity*curr_pt_intensity;
                ++count;
            }
            // if count >2 then compute gain and offset.
            if (count>=2)
            {
                float a=(summuixi-summui*sumxi/count)/(sumxi2-sumxi*sumxi/count);
                float b =(summui-a*sumxi)/count;
                std::cout<<"# of correspondences are "<<count<<"a(gain) "<<a<< " b(offset) "<<b<<std::endl;
                vil_math_scale_and_offset_values<float>(*floatimg,a,b);
                vil_convert_stretch_range_limited<float>(*floatimg, *img_byte, 0.0f, 1.0f, (unsigned short)0, (unsigned short)(256*256-1));
                vil_save(*img_byte,imgname.c_str());
                std::cout<<" ======="<<std::endl;
            }
            else
                std::cout<<" Only "<<count << " correspondences"<<std::endl;
        }
    }

    return 0;
}
