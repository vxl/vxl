#include "vmal_dense_matching.h"
#include <vtol/vtol_edge_2d.h>
#include <vmal/vmal_convert_vtol.h>
#include <vmal/vmal_operators.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_int_3.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_inverse.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_save.h>

//#include <vcl_cstdio.h>
#include <vcl_cmath.h>

vmal_dense_matching::vmal_dense_matching(const vnl_double_3x3 & H0,
                                         const vnl_double_3x3 & H1)
{
  _H0=H0;
  _H1=H1;
}

vmal_dense_matching::~vmal_dense_matching()
{
}

void vmal_dense_matching::set_fmatrix(const vnl_double_3x3 & F)
{
  _F=F;
  _type=1; //1 for fundamental matrix
}

void vmal_dense_matching::set_hmatrix(const vnl_double_3x3 & H)
{
  _H=H;
  _type=2; //2 for homograpgy
}

// Between two set of lines in 2 images that are matched, it compute the best lines
// using the fundamental constraint.
void vmal_dense_matching::refine_lines_using_F(vmal_multi_view_data_edge_sptr mvd_edge,
                                               vmal_multi_view_data_edge_sptr res)
{
  //We assume that the lines have been sorted. It means that, for example, the
  //first end-point of the first segment correspond to the first end-point of
  //the second segment.
  if (_type==1)
  {
    if (mvd_edge->get_nb_views()>1)
    {
      vnl_double_3* lines0_p;
      vnl_double_3* lines0_q;
      vnl_double_3* lines1_p;
      vnl_double_3* lines1_q;

      vcl_vector<vtol_edge_2d_sptr> tmp_lines0;
      vcl_vector<vtol_edge_2d_sptr> tmp_lines1;

      mvd_edge->get(0,1,tmp_lines0,tmp_lines1);
      int numlines=tmp_lines0.size();

      convert_lines_double_3(tmp_lines0, lines0_p, lines0_q);
      convert_lines_double_3(tmp_lines1, lines1_p, lines1_q);
      double threshold=0.1745;//0.08727;

      for (int i=0;i<numlines;i++)
      {
        //compute the 4 epipolars lines, each linked to an end-point of
        //the 2 segments
        vnl_double_3 epi_line1p=_F*lines0_p[i];
        vnl_double_3 epi_line1q=_F*lines0_q[i];
        vnl_double_3 epi_line0p=(_F.transpose()*lines1_p[i]);
        vnl_double_3 epi_line0q=(_F.transpose()*lines1_q[i]);

        //variables to represent the end-points
        vnl_double_3 pt0p=lines0_p[i];
        vnl_double_3 pt0q=lines0_q[i];
        vnl_double_3 pt1p=lines1_p[i];
        vnl_double_3 pt1q=lines1_q[i];

        vnl_double_3 inter1p;
        vnl_double_3 inter1q;

        double alpha,beta;

        if (vmal_operators::line_cross_seg(lines1_p[i], lines1_q[i], epi_line1p, inter1p,alpha))
          if (vmal_operators::line_cross_seg(lines1_p[i], lines1_q[i], epi_line1q, inter1q,beta))
          {
            //1 case: the first segment is "included" in the second segment
            vmal_operators::line_cross_seg(lines0_p[i], lines0_q[i], epi_line0p, pt0p,alpha);
            vmal_operators::line_cross_seg(lines0_p[i], lines0_q[i], epi_line0q, pt0q,beta);
          }
          else
          {
            //2 case: the segments share a part.
            vmal_operators::line_cross_seg(lines0_p[i], lines0_q[i], epi_line0p, pt0p, alpha);
            pt1q=inter1q;
          }
        else
          if (vmal_operators::line_cross_seg(lines1_p[i], lines1_q[i], epi_line1q, inter1q, beta))
          {
            //3 case:  the segments share a part.
            vmal_operators::line_cross_seg(lines0_p[i], lines0_q[i], epi_line0q, pt0q, beta);
            pt1p=inter1p;
          }
          else
          {
            //4 case: the second segment is "included" in the first
            pt1p=inter1p;
            pt1q=inter1q;
          }

        if (alpha>threshold && beta>threshold)
        {
          res->new_track();
          vtol_edge_2d_sptr out0=new vtol_edge_2d(pt0p[0],pt0p[1],pt0q[0],pt0q[1]);
          vtol_edge_2d_sptr out1=new vtol_edge_2d(pt1p[0],pt1p[1],pt1q[0],pt1q[1]);
          res->set(0,out0);
          res->set(1,out1);
          res->close_track();
        }
      }
    }
  }
  else
    vcl_cerr<<"Error: you must set the Fundamental matrix to use this method.\n";
}

// Between two set of lines in 2 images that are matched, it compute the best lines
// using the homography
void vmal_dense_matching::refine_lines_using_H(vmal_multi_view_data_edge_sptr mvd_edge,
                                               vmal_multi_view_data_edge_sptr res)
{
  //the second segment.
  if (_type==2)
  {
    if (mvd_edge->get_nb_views()>1)
    {
      vnl_double_3* lines0_p;
      vnl_double_3* lines0_q;
      vnl_double_3* lines1_p;
      vnl_double_3* lines1_q;

      vcl_vector<vtol_edge_2d_sptr> tmp_lines0;
      vcl_vector<vtol_edge_2d_sptr> tmp_lines1;

      mvd_edge->get(0,1,tmp_lines0,tmp_lines1);
      int numlines=tmp_lines0.size();

      convert_lines_double_3(tmp_lines0, lines0_p, lines0_q);
      convert_lines_double_3(tmp_lines1, lines1_p, lines1_q);

      vnl_double_3x3 HI=vnl_inverse(_H);
      for (int i=0;i<numlines;i++)
      {
        vnl_double_3 pt0p=lines0_p[i];
        vnl_double_3 pt0q=lines0_q[i];
        vnl_double_3 pt1p=lines1_p[i];
        vnl_double_3 pt1q=lines1_q[i];

        vnl_double_3 h_lines1_p=_H*lines0_p[i];
        vnl_double_3 h_lines1_q=_H*lines0_q[i];
        vnl_double_3 h_lines0_p=HI*lines1_p[i];
        vnl_double_3 h_lines0_q=HI*lines1_q[i];

        vnl_double_3 inter1p;
        vnl_double_3 inter1q;

        if (vmal_operators::project_point(h_lines1_p,lines1_p[i],lines1_q[i],inter1p))
        {
          if (vmal_operators::project_point(h_lines1_q,lines1_p[i],lines1_q[i],inter1q))
          {
            //Case 1
            vmal_operators::project_point(h_lines0_p,lines0_p[i],lines0_q[i],pt0p);
            vmal_operators::project_point(h_lines0_q,lines0_p[i],lines0_q[i],pt0q);
          }
          else
          {
            //Case 2
            vmal_operators::project_point(h_lines0_p,lines0_p[i],lines0_q[i],pt0p);
            pt1q=inter1q;
          }
        }
        else
        {
          if (vmal_operators::project_point(h_lines1_q,lines1_p[i],lines1_q[i],inter1q))
          {
            //Case 3
            pt1p=inter1p;
            vmal_operators::project_point(h_lines0_q,lines0_p[i],lines0_q[i],pt0q);
          }
          else
          {
            //Case 4
            pt1p=inter1p;
            pt1q=inter1q;
          }

        res->new_track();
        vtol_edge_2d_sptr out0=new vtol_edge_2d(pt0p[0],pt0p[1],pt0q[0],pt0q[1]);
        vtol_edge_2d_sptr out1=new vtol_edge_2d(pt1p[0],pt1p[1],pt1q[0],pt1q[1]);
        res->set(0,out0);
        res->set(1,out1);
        res->close_track();
      }
    }
  }
  else
    vcl_cerr<<"Error: you must set the Homography matrix to use this method.\n";
}


void vmal_dense_matching::disparity_map(vmal_multi_view_data_edge_sptr mvd_edge,
                                        int h,int w)
{
  vnl_double_3* lines0_p;
  vnl_double_3* lines0_q;
  vnl_double_3* lines1_p;
  vnl_double_3* lines1_q;

  int disparity, min_disparity=0,max_disparity=0;

  vcl_vector<vtol_edge_2d_sptr> tmp_lines0;
  vcl_vector<vtol_edge_2d_sptr> tmp_lines1;

  mvd_edge->get(0,1,tmp_lines0,tmp_lines1);
  int numlines=tmp_lines0.size();

  convert_lines_double_3(tmp_lines0, lines0_p, lines0_q);
  convert_lines_double_3(tmp_lines1, lines1_p, lines1_q);

  vnl_double_3x3 IH0=vnl_inverse(_H0);

  vnl_double_3 int_line0p;
  vnl_double_3 int_line0q;
  vnl_double_3 int_line1p;
  vnl_double_3 int_line1q;

  vnl_double_3 point0;
  vnl_double_3 point1;
  vnl_double_3 t_point0, t_point1;
  vnl_int_3 int_t_point0, int_t_point1;

  double a0,b0,a1,b1;
  double delta0x,delta0y,delta1x,delta1y;

  vnl_matrix<int> map(w,h);
  map.fill(0);

  for (int i=0;i<numlines;i++)
  {
    int_line0p[0]=vmal_round(lines0_p[i][0]);
    int_line0p[1]=vmal_round(lines0_p[i][1]);
    int_line0p[2]=1.0;
    int_line0q[0]=vmal_round(lines0_q[i][0]);
    int_line0q[1]=vmal_round(lines0_q[i][1]);
    int_line0q[2]=1.0;

    int_line1p[0]=vmal_round(lines1_p[i][0]);
    int_line1p[1]=vmal_round(lines1_p[i][1]);
    int_line1p[2]=1.0;
    int_line1q[0]=vmal_round(lines1_q[i][0]);
    int_line1q[1]=vmal_round(lines1_q[i][1]);
    int_line1q[2]=1.0;

    double num0=int_line0p[0]-int_line0q[0];
    double num1=int_line1p[0]-int_line1q[0];

    a0=(int_line0p[1]-int_line0q[1])/num0;
    a1=(int_line1p[1]-int_line1q[1])/num1;
    b0=int_line0p[1]-a0*int_line0p[0];
    b1=int_line1p[1]-a1*int_line1p[0];

    if ((int_line0q[0]-int_line0p[0])>0)
      delta0x=0.5;
    else
      delta0x=-0.5;
    if ((int_line0q[1]-int_line0p[1])>0)
      delta0y=0.5;
    else
      delta0y=-0.5;

    if ((int_line1q[0]-int_line1p[0])>0)
      delta1x=0.5;
    else
      delta1x=-0.5;
    if ((int_line1q[1]-int_line1p[1])>0)
      delta1y=0.5;
    else
      delta1y=-0.5;

    point0=int_line0p;
    point1=int_line1p;

    while (point0!=int_line0q)
    {
      if (num0!=0.0)
        if (vcl_fabs(((point0[0]+delta0x)*a0+b0)-point0[1])<=0.5)
          point0[0]+=delta0x*2;
        else
          point0[1]+=delta0y*2;
      else
        point0[1]+=delta0y*2;

      if (num1!=0.0) {
        if (vcl_fabs(((point1[0]+delta1x)*a1+b1)-point1[1])<=0.5)
          point1[0]+=delta1x*2;
        else
          point1[1]+=delta1y*2;
      }
      else
        point1[1]+=delta1y*2;

      t_point0=_H0*point0;
      t_point1=_H1*point1;
      int_t_point0[0]=(int)vmal_round(t_point0[0]/t_point0[2]);
      int_t_point0[1]=(int)vmal_round(t_point0[1]/t_point0[2]);
      int_t_point1[0]=(int)vmal_round(t_point1[0]/t_point1[2]);
      int_t_point1[1]=(int)vmal_round(t_point1[1]/t_point1[2]);

      //disparity=vmal_round_int(int_t_point1[1]-int_t_point0[1]);
      disparity=1;
      map(vmal_round_int(point0[0]),vmal_round_int(point0[1]))=disparity;
      if (disparity<min_disparity)
        min_disparity=disparity;
      if (disparity>max_disparity)
        max_disparity=disparity;
    }
  }
  vcl_cerr<<"Disparity min: "<<min_disparity<<vcl_endl;
  vcl_cerr<<"Disparity max: "<<max_disparity<<vcl_endl;

  //Save the matrix in a pgn image
  max_disparity-=min_disparity;
  unsigned char* buf=new unsigned char[w*h];
  for (int i=0; i<h; i++)
  {
    for (int j=0; j<w; j++)
    {
      int value=vmal_round_int((map(j,i)-min_disparity)*255/max_disparity);
      buf[i*w+j]=(unsigned char)value;
    }
  }
  vil1_memory_image_of<vxl_byte> ima(buf, w, h);
  vil1_save(ima, "lolo.pgm");
  delete [] buf;
}


double vmal_round(double a)
{
  return (a>0) ? double(int(a+0.5)) : double(int(a-0.5));
}

int vmal_round_int(double a)
{
  return (a>0) ? int(a+0.5) : int(a-0.5);
}
