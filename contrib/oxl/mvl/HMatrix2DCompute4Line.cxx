// This is oxl/mvl/HMatrix2DCompute4Line.cxx
#include "HMatrix2DCompute4Line.h"
//
#include <mvl/HMatrix2D.h>
#include <mvl/HomgLine2D.h>
#include <mvl/ProjectiveBasis2D.h>

bool
HMatrix2DCompute4Line::compute_l(const LineArray & lines1,
                                 const LineArray & lines2,
                                 HMatrix2D* H)
{
  PointArray points1(4,HomgPoint2D()),points2(4,HomgPoint2D());
  for (int i=0;i<4;i++) {
    points1[i].set(lines1[i].x(),lines1[i].y(),lines1[i].w());
    points2[i].set(lines2[i].x(),lines2[i].y(),lines2[i].w());
  }
  ProjectiveBasis2D basis1(points1);
  ProjectiveBasis2D basis2(points2);

  H->set((basis1.get_T().get_inverse().get_matrix() * basis2.get_T_matrix()).transpose());
  return true;
}
