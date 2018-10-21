// This is gel/vmal/vmal_refine_lines.cxx
#include "vmal_refine_lines.h"

#include <vmal/vmal_operators.h>
#include <vnl/vnl_inverse.h>

vmal_refine_lines::vmal_refine_lines() = default;

vmal_refine_lines::~vmal_refine_lines() = default;

//Need to be improve. Perhaps a normalization of the lines would give better results
void vmal_refine_lines::refine_lines_f(vnl_double_3 &line0p, vnl_double_3 &line0q,
                                       vnl_double_3 &line1p, vnl_double_3 &line1q,
                                       const vnl_double_3x3 & F,
                                       vnl_double_3 &r_line0p, vnl_double_3 &r_line0q,
                                       vnl_double_3 &r_line1p, vnl_double_3 &r_line1q)
{
  vnl_double_3 epi_line1p=F*line0p;
  vnl_double_3 epi_line1q=F*line0q;
  vnl_double_3 epi_line0p=(F.transpose()*line1p);
  vnl_double_3 epi_line0q=(F.transpose()*line1q);

  r_line0p=line0p;
  r_line0q=line0q;
  r_line1p=line1p;
  r_line1q=line1q;

  vnl_double_3 inter1p;
  vnl_double_3 inter1q;

  double alpha,beta;

  if (vmal_operators::line_cross_seg(line1p, line1q,
                                     epi_line1p, inter1p,alpha))
    if (vmal_operators::line_cross_seg(line1p, line1q,
                                       epi_line1q, inter1q,beta))
    {
      //1 case: the first segment is "included" in the second segment
      vmal_operators::line_cross_seg(line0p, line0q,
                                     epi_line0p, r_line0p,alpha);
      vmal_operators::line_cross_seg(line0p, line0q,
                                     epi_line0q, r_line0q,beta);
    }
    else
    {
      //2 case: the segments share a part.
      vmal_operators::line_cross_seg(line0p, line0q,
                                     epi_line0p, r_line0p, alpha);
      r_line1q=inter1q;
    }
  else
    if (vmal_operators::line_cross_seg(line1p, line1q,
                        epi_line1q, inter1q, beta))
    {
      //3 case:  the segments share a part.
      vmal_operators::line_cross_seg(line0p, line0q,
                                     epi_line0q, r_line0q, beta);
      r_line1p=inter1p;
    }
    else
    {
      //4 case: the second segment is "included" in the first
      r_line1p=inter1p;
      r_line1q=inter1q;
    }
}

// Between two lines in 2 images that are matched, it compute the best lines
// using the homography
void vmal_refine_lines::refine_lines_max_h(vnl_double_3 &line0p, vnl_double_3 &line0q,
                                           vnl_double_3 &line1p, vnl_double_3 &line1q,
                                           const vnl_double_3x3 & H,
                                           vnl_double_3 &r_line0p, vnl_double_3 &r_line0q,
                                           vnl_double_3 &r_line1p, vnl_double_3 &r_line1q)
{
  vnl_double_3x3 HI=vnl_inverse(H);

  vnl_double_3 h_line1_p=H*line0p;
  vnl_double_3 h_line1_q=H*line0q;
  vnl_double_3 h_line0_p=HI*line1p;
  vnl_double_3 h_line0_q=HI*line1q;

  r_line0p=line0p;
  r_line0q=line0q;
  r_line1p=line1p;
  r_line1q=line1q;

  vnl_double_3 inter1p;
  vnl_double_3 inter1q;

  if (vmal_operators::project_point(h_line1_p,line1p,line1q,inter1p))
    if (vmal_operators::project_point(h_line1_q,line1p,line1q,inter1q))
    {
      //Case 1
      vmal_operators::project_point(h_line0_p,line0p,line0q,r_line0p);
      vmal_operators::project_point(h_line0_q,line0p,line0q,r_line0q);
    }
    else
    {
      //Case 2
      vmal_operators::project_point(h_line0_p,line0p,line0q,r_line0p);
      r_line1q=inter1q;
    }
  else
    if (vmal_operators::project_point(h_line1_q,line1p,line1q,inter1q))
    {
      //Case 3
      r_line1p=inter1p;
      vmal_operators::project_point(h_line0_q,line0p,line0q,r_line0q);
    }
    else
    {
      //Case 4
      r_line1p=inter1p;
      r_line1q=inter1q;
    }
}

//Refine a pair of lines: it means that it keeps the common part of the two lines
//using the homography.
void vmal_refine_lines::refine_lines_min_h(vnl_double_3 &line0p, vnl_double_3 &line0q,
                                           vnl_double_3 &line1p, vnl_double_3 &line1q,
                                           const vnl_double_3x3 &H,
                                           vnl_double_3 &r_line0p, vnl_double_3 &r_line0q,
                                           vnl_double_3 &r_line1p, vnl_double_3 &r_line1q)
{
  vnl_double_3x3 HI=vnl_inverse(H);

  r_line0p=line0p;
  r_line0q=line0q;
  r_line1p=line1p;
  r_line1q=line1q;

  vnl_double_3 h_line1_p=H*line0p;
  vnl_double_3 h_line1_q=H*line0q;
  vnl_double_3 h_line0_p=HI*line1p;
  vnl_double_3 h_line0_q=HI*line1q;

  vnl_double_3 inter1p;
  vnl_double_3 inter1q;

  if (vmal_operators::project_point(h_line1_p,line1p,line1q,inter1p))
  {
    if (vmal_operators::project_point(h_line1_q,line1p,line1q,inter1q))
    {
      //Case 1
      r_line1p=inter1p;
      r_line1q=inter1q;
    }
    else
    {
      //Case 2
      vmal_operators::project_point(h_line0_q,line0p,line0q,r_line0q);
      r_line1p=inter1p;
    }
  }
  else
  {
    if (vmal_operators::project_point(h_line1_q,line1p,line1q,inter1q))
    {
      //Case 3
      vmal_operators::project_point(h_line0_p,line0p,line0q,r_line0p);
      r_line1q=inter1q;
    }
    else
    {
      //Case 4
      vmal_operators::project_point(h_line0_p,line0p,line0q,r_line0p);
      vmal_operators::project_point(h_line0_q,line0p,line0q,r_line0q);
    }
  }
}
