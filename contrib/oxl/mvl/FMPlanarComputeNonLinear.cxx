// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//                               
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//       
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>
//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation "FMPlanarComputeNonLinear.h"
#endif
//
// Class: FMPlanarComputeNonLinear
// Author: Martin Armsrong Oxford
// Created: Nov 96
// Modifications:
//
//-----------------------------------------------------------------------------

#include <mvl/PairSetCorner.h>
#include <mvl/FMatrixPlanar.h>
#include <mvl/FMPlanarComputeNonLinear.h>
#include <mvl/FMPlanarNonLinFun.h>

// -- Constructor.  The parameter outlier_threshold is not currently used, but
// may be in future if this is converted to a Huber function.
FMPlanarComputeNonLinear::FMPlanarComputeNonLinear(const ImageMetric* image_metric1, const ImageMetric* image_metric2, double outlier_threshold):
  FMatrixCompute()
{
  _image_metric1 = image_metric1;
  _image_metric2 = image_metric2;
  _outlier_distance_squared = outlier_threshold * outlier_threshold;
}

// -- Compute from given PairMatchSetCorner
bool FMPlanarComputeNonLinear::compute_planar(PairMatchSetCorner& matches, FMatrixPlanar* F)
{
  PairSetCorner inliers(matches);
  return compute_planar(inliers.points1, inliers.points2, F);
}

// -- Compute from given pair of vcl_vector<HomgPoint2D>
bool FMPlanarComputeNonLinear::compute_planar(vcl_vector<HomgPoint2D>& points1,
					      vcl_vector<HomgPoint2D>& points2, FMatrixPlanar* F)
{
  cout << "FMPlanarComputeNonLinear: Fitting planar-motion F matrix [e1]_x [l]_x [e2]_x\n";
  FMPlanarNonLinFun computor(_image_metric1, _image_metric2, _outlier_distance_squared, points1, points2);
  return computor.compute(F);
}

bool FMPlanarComputeNonLinear::compute(PairMatchSetCorner& matches, FMatrix* F)
{
  FMatrixPlanar fplanar;
  fplanar.init(*F);
  if (!compute_planar(matches, &fplanar))
    return false;
  
  // Slice Fplanar into F
  *F = fplanar;
  return true;
}

bool FMPlanarComputeNonLinear::compute(vcl_vector<HomgPoint2D>& points1, vcl_vector<HomgPoint2D>& points2, FMatrix* F)
{
  FMatrixPlanar fplanar;
  fplanar.init(*F);
  if (!compute_planar(points1, points2, &fplanar))
    return false;
  
  // Slice Fplanar into F
  *F = fplanar;
  return true;
}

