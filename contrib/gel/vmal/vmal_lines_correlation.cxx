// This is gel/vmal/vmal_lines_correlation.cxx
#include "vmal_lines_correlation.h"

#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3x3.h>
#include <vcl_cmath.h>
#include <vcl_cstdlib.h> // for vcl_abs(int)

vmal_lines_correlation::vmal_lines_correlation():
delta_(5),radius_(5.0)
{
}

vmal_lines_correlation::vmal_lines_correlation(int delta, double radius):
delta_(delta),radius_(radius)
{
}

vmal_lines_correlation::~vmal_lines_correlation()
{
}

double vmal_lines_correlation::find_min_corr(vnl_double_3 &line0p, vnl_double_3 &line0q,
                                             vnl_double_3 &line1p, vnl_double_3 &line1q,
                                             vil1_memory_image_of<vxl_byte> &image0,
                                             vil1_memory_image_of<vxl_byte> &image1,
                                             vnl_double_3 &trans)
{
  double res, min_res=-1.0;
  int i;
  vnl_double_3 min_trans;
  for (i=-delta_;i<=delta_;i++)
  {
    res=lines_correlation(line0p, line0q,
                          line1p, line1q,
                          image0,  image1,
                          trans,i);
    if ((res<min_res) || (min_res==-1.0))
    {
      min_res=res;
      min_trans=trans;
    }
  }

  trans=min_trans;
  return min_res;
}

double vmal_lines_correlation::lines_correlation(vnl_double_3 &line0_p, vnl_double_3 &line0_q,
                                                 vnl_double_3 &line1_p, vnl_double_3 &line1_q,
                                                 vil1_memory_image_of<vxl_byte> &image0,
                                                 vil1_memory_image_of<vxl_byte> &image1,
                                                 vnl_double_3 &trans, int bias)
{
  //compute the director vector of the segments
  vnl_double_2 tan0(line0_q[0]-line0_p[0], line0_q[1]-line0_p[1]);
//vnl_double_2 tan1(line1_q[0]-line1_p[0], line1_q[1]-line1_p[1]);

  //compute the normal vector to the segments and multiply it by the radius
  vnl_double_2 norm0(-line0_p[1]+line0_q[1], -line0_q[0]+line0_p[0]);
  norm0=norm0.normalize()*radius_;
  vnl_double_2 norm1(-line1_p[1]+line1_q[1], -line1_q[0]+line1_p[0]);
  norm1=norm1.normalize()*radius_;
  //Now norm and tan define the seaching box

  //compute the transformation from the searching box to the image reference.
  vnl_double_2 t0(line0_p[0]-norm0[0],line0_p[1]-norm0[1]);
  vnl_double_3x3 r0;
  r0.set_identity();
  double costheta0=norm0[0]/radius_;
  double sintheta0=norm0[1]/radius_;
  r0[0][0]=costheta0; r0[0][1]=-sintheta0;
  r0[1][0]=sintheta0; r0[1][1]=costheta0;

  //compute the vector that will slide the first line to find the best match between
  //the two.
  trans=r0*vnl_double_3(0.0, bias, 1.0);

  r0[0][2]=t0[0];
  r0[1][2]=t0[1];

  vnl_double_2 t1(line1_p[0]-norm1[0],line1_p[1]-norm1[1]);
  vnl_double_3x3 r1;
  r1.set_identity();
  double costheta1=norm1[0]/radius_;
  double sintheta1=norm1[1]/radius_;
  r1[0][0]=costheta1; r1[0][1]=-sintheta1; r1[0][2]=t1[0];
  r1[1][0]=sintheta1; r1[1][1]=costheta1;  r1[1][2]=t1[1];

  int num_pixel_width=(int)(2*radius_)+1;
  int num_pixel_height=(int)(tan0.magnitude())+1;

  double sum=0;
  vnl_double_3 pixel0;
  vnl_double_3 pixel1;
  for (int i=0;i<num_pixel_width; i++)
    for (int j=0;j<num_pixel_height;j++)
    {
      vnl_double_3 cur_pt(i, j, 1);
      pixel0=(r0*cur_pt)+trans;
      pixel1=(r1*cur_pt);
      unsigned char value0;
      unsigned char value1;
      if (interpol_pixel(pixel0, pixel1, image0, image1, value0, value1))
        sum+=vcl_abs(value0-value1);
    }

  return sum/(num_pixel_height*num_pixel_height);
}

bool vmal_lines_correlation::interpol_pixel(vnl_double_3 &pixel0, vnl_double_3 &pixel1,
                                            vil1_memory_image_of<vxl_byte> &image0,
                                            vil1_memory_image_of<vxl_byte> &image1,
                                            unsigned char &value0, unsigned char &value1)
{
  int h=image0.height();
  int w=image0.width();
  int top=(int)pixel0[1]+1;
  int right=(int)pixel0[0]+1;
  int left=(int)pixel0[0];
  int bottom=(int)pixel0[1];

  if ((top <0) || (top >=h) || (bottom<0) || (bottom>=h) ||
      (left<0) || (left>=w) || (right <0) || (right >=w))
    return false;

  unsigned char p3=image0[top][right];
  unsigned char p2=image0[top][left];
  unsigned char p1=image0[bottom][right];
  unsigned char p0=image0[bottom][left];


  double dx=pixel0[0]-left;
  double dy=pixel0[1]-bottom;

  value0= (unsigned char) (0.5 + dx*dy*p3+(1.0-dx)*dy*p2+(1.0-dy)*dx*p1+(1.0-dx)*(1.0-dy)*p0);

  top=(int)pixel1[1]+1;
  right=(int)pixel1[0]+1;
  left=(int)pixel1[0];
  bottom=(int)pixel1[1];

  if ((top <0) || (top >=h) || (bottom<0) || (bottom>=h) ||
      (left<0) || (left>=w) || (right <0) || (right >=w))
    return false;

  p3=image1[top][right];
  p2=image1[top][left];
  p1=image1[bottom][right];
  p0=image1[bottom][left];


  dx=pixel1[0]-left;
  dy=pixel1[1]-bottom;

  value1= (unsigned char) (0.5 + dx*dy*p3+(1.0-dx)*dy*p2+(1.0-dy)*dx*p1+(1.0-dx)*(1.0-dy)*p0);

  return true;
}
