// This is gel/vmal/vmal_operators.cxx
#include <iostream>
#include <cmath>
#include "vmal_operators.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_cross.h>

vmal_operators::vmal_operators() = default;

vmal_operators::~vmal_operators() = default;

//Project the point (x0,y0) on the line [(ax,ay),(bx,by)]. (x,y) are
//the coordinates of the projected. If (x,y) does not belong to the
//segment, it sets (x,y) to (-1,-1). The function returns the distance
//between the point and its projected.
double vmal_operators::project_point(double x0,double y0,
                                     double ax,double ay,
                                     double bx,double by,
                                     double *x,double *y)
{
  *x=0;
  *y=0;
  double n1=bx-ax;
  double n2=by-ay;
  double norm=n1*n1+n2*n2;
  if (norm!=0)
  {
    double resy=-(ax*n1*n2-ay*n1*n1-x0*n1*n2-y0*n2*n2)/norm;
    double resx= (ax*n2*n2-ay*n1*n2+x0*n1*n1+y0*n1*n2)/norm;

    if (((ax-resx)*(bx-resx)+(ay-resy)*(by-resy))>0)
    {
      *x=-1;
      *y=-1;
    }
    else
    {
      *x=resx;
      *y=resy;
    }
      return std::sqrt((resx-x0)*(resx-x0)+(resy-y0)*(resy-y0));
  }

  return -1;
}


bool vmal_operators::project_point(vnl_double_3 &x,
                                   vnl_double_3 &a,
                                   vnl_double_3 &b,
                                   vnl_double_3 &px)
{
  double ax=a[0]/a[2];
  double ay=a[1]/a[2];
  double bx=b[0]/b[2];
  double by=b[1]/b[2];
  double x0=x[0]/x[2];
  double y0=x[1]/x[2];

  double n1=bx-ax;
  double n2=by-ay;
  double norm=n1*n1+n2*n2;

  double resy=-(ax*n1*n2-ay*n1*n1-x0*n1*n2-y0*n2*n2)/norm;
  double resx= (ax*n2*n2-ay*n1*n2+x0*n1*n1+y0*n1*n2)/norm;
  px[0]=resx;
  px[1]=resy;
  px[2]=1;

  return ((ax-resx)*(bx-resx)+(ay-resy)*(by-resy))<=0;
}

//
bool vmal_operators::cross_seg(double f1x,double f1y,double f2x,double f2y,//first segment
                               double s1x,double s1y,double s2x,double s2y)//second segment
{
  //compute the directors vectors
  double nsx=s2x-s1x;
  double nsy=s2y-s1y;

  double nfx=f2x-f1x;
  double nfy=f2y-f1y;

  double df=-f1x*nfy+f1y*nfx;
  double ds=-s1x*nsy+s1y*nsx;

  //compute the determinant
  double det=nsy*nfx-nsx*nfy;
  if (det==0)
  return false;

  //compute the intersection point
  double interx=(-nfx*ds+nsx*df)/det;
  double intery=(nsy*df-ds*nfy)/det;

  // test if the intersection point belongs to the segments
  return ((((s1x-interx)*(s2x-interx)+(s1y-intery)*(s2y-intery)) < 0) &&
      (((f1x-interx)*(f2x-interx)+(f1y-intery)*(f2y-intery)) < 0));
}


bool vmal_operators::line_cross_seg(vnl_double_3 start_seg,
                                    vnl_double_3 end_seg,
                                    vnl_double_3 & line_equ,
                                    vnl_double_3 & inter,
                                    double &alpha)
{
  start_seg[0]=start_seg[0]/start_seg[2];
  start_seg[1]=start_seg[1]/start_seg[2];
  start_seg[2]=1;

  end_seg[0]=end_seg[0]/end_seg[2];
  end_seg[1]=end_seg[1]/end_seg[2];
  end_seg[2]=1;

  vnl_double_3 seg_equ;
  seg_equ[0]=end_seg[1]-start_seg[1];
  seg_equ[1]=-(end_seg[0]-start_seg[0]);
  seg_equ[2]=-(seg_equ[0]*start_seg[0]+
         seg_equ[1]*start_seg[1]);

  inter=vnl_cross_3d(line_equ,seg_equ);
  inter[0]=inter[0]/inter[2];
  inter[1]=inter[1]/inter[2];
  inter[2]=1;

  vnl_double_2 vect_line(-line_equ[1], line_equ[0]);
  vnl_double_2 vect_seg(-seg_equ[1], -seg_equ[0]);

  alpha=angle(vect_line,vect_seg);

  return ((start_seg[0]-inter[0])*(end_seg[0]-inter[0])+
          (start_seg[1]-inter[1])*(end_seg[1]-inter[1])) < 0;
}
