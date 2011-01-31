#include "bil_detect_blur_scale.h"
//
#include <sdet/sdet_detector.h>
#include <vsol/vsol_digital_curve_2d_sptr.h>
#include <vsol/vsol_digital_curve_2d.h>
#include <vsol/vsol_point_2d.h>
#include <bsta/bsta_histogram.h>

#include <vnl/vnl_erf.h>
#include <vil/vil_convert.h>

bool inbounds(int x,int y,vil_image_resource_sptr & img)
{
    return x>=0 && y>=0 && x<int(img->ni()) && y<int(img->nj());
}

void bil_detect_blur_scale(vil_image_resource_sptr  & img,
                           int len_of_curves,
                           float & est_sigma)
{
    float sigma=2.0f;
    vcl_vector<vsol_digital_curve_2d_sptr> edges;

    sdet_detector det(img,sigma,2.0,1.0,1.5,len_of_curves,2.23606,1.0);
    det.set_close_borders(false);
    det.DoStep();
    det.DoContour();
    det.get_vsol_edges(edges);

    vil_image_view<unsigned char>  uimg=img->get_copy_view();
    vil_image_view<float> fimg(uimg.ni(),uimg.nj());

    vil_convert_stretch_range_limited<unsigned char>(uimg,fimg,0,255,0.0f,1.0f);
    float hist[8]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};

    for (unsigned i=0;i<edges.size();i++)
    {
        float prevx=0.0f, prevy=0.0f;
        for (unsigned j=0;j<edges[i]->size();j++)
        {
            if (j==0)
            {
                prevx=edges[i]->point(j)->x();
                prevy=edges[i]->point(j)->y();
                continue;
            }
            float x=edges[i]->point(j)->x();   float y=edges[i]->point(j)->y();
            float nx=-(prevy-y);float ny=(prevx-x);
            float len=vcl_sqrt(nx*nx+ny*ny);

            if (len<=0.0) continue;
            nx/=len;ny/=len;

            int cx=(int)vcl_floor(x+0.5);      int cy=(int)vcl_floor(y+0.5);
            if (!inbounds(cx,cy,img)) continue;

            int lcx=(int)vcl_floor(x-4*nx+0.5);int lcy=(int)vcl_floor(y-4*ny+0.5);
            if (!inbounds(lcx,lcy,img)) continue;

            int rcx=(int)vcl_floor(x+4*nx+0.5);int rcy=(int)vcl_floor(y+4*ny+0.5);
            if (!inbounds(rcx,rcy,img)) continue;

            float lval=fimg(lcx,lcy);
            float rval=fimg(rcx,rcy);
            float val=fimg(cx,cy);
            float r=vcl_fabs(rval-lval)/2;

            float minerr=1e5; float minsig=-1;
            for (float sig=1;sig<5;)
            {
                float err=0.0;
                for (int t=-5;t<=5;t++)
                {
                    int xt=(int)vcl_floor(x+t*nx+0.5);
                    int yt=(int)vcl_floor(y+t*ny+0.5);

                    if (inbounds(xt,yt,img))
                    {
                        float obs=fimg(xt,yt)-val;
                        float erf=r*vnl_erf((double)t/(double)sig/vcl_sqrt(2.0));
                        err+=(erf-obs)*(erf-obs);
                    }
                }
                if (err<minerr)
                {
                    minerr=err;
                    minsig=sig;
                }
                // reverse side
                err=0.0;
                for (int t=-4;t<=4;t++)
                {
                    int xt=(int)vcl_floor(x+t*nx+0.5);
                    int yt=(int)vcl_floor(y+t*ny+0.5);

                    if (inbounds(xt,yt,img))
                    {
                        float obs=fimg(xt,yt)-val;
                        float erf=r*vnl_erf((double)-t/(double)sig/vcl_sqrt(2.0));
                        err+=(erf-obs)*(erf-obs);
                    }
                }
                if (err<minerr)
                {
                    minerr=err;
                    minsig=sig;
                }

                sig+=0.5;
            }
            //vcl_cout<<" "<<minsig;
            hist[int(minsig*2)-2]++;

            prevx=x;
            prevy=y;
        }
    }
    float maxval=-1.0;
    for (unsigned cnt=0;cnt<8;cnt++)
    {
        if (hist[cnt]>maxval)
        {
            maxval=hist[cnt];
            est_sigma=(float)(cnt+2)/2;
        }
    }
}
