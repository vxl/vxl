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
#ifndef _gevd_detector_h_
#define _gevd_detector_h_
//
// .NAME gevd_detector - non-display-based interface class
// .LIBRARY Detection
// .HEADER Segmentation package
// .INCLUDE Detection/gevd_detector.h
// .FILE gevd_detector.h
// .FILE gevd_detector.C
//
// .SECTION Description
//
// An interface to Van-Duc's Canny code. No display is involved.
// The Canny has two major phases: 1) detect step edgels, 2)Follow contours
// and construct a topological network. Each phase has a number of
//  parameters which are defined as follows.
//  Step Detection:
//
//  float contourFactor, junctionFactor: Scale factors for determining the
//                                       gradient threshold. Nominally 1.0.
//                                       contourFactor is in effect for edgels
//                                       on contours (boundaries).
//                                       junctionFactor is in effect during the
//                                       extension of contours at endpoints.
//                                       To extend contours aggressively, use a
//                                       low value of junctionFactor, i.e., .5.
//
//  float noiseThreshold: A weighting factor that determines the relative
//                        proportion of sensor noise level and texture noise level
//                        as measured in a ROI in the center of the image. The
//                        nominal value of -.5 gives equal weight to both. If the
//                        value is positive, then a default noise threshold of 1.0
//                        is assigned.
//
//  float filterFactor:  An overall scale factor for determining gradient threshold.
//                       Nominally 2.0.
//
//  bool  junctionp:  If true, then recover junctions by extending contours.
//                    Nominally true.
//
//  Contour Following:
//  float hysteresisFactor:     A scale factor which is multiplied by the
//                              image noise level to determine the minimum
//                              gradient threshold in following an edgel contour.
//                              Nominally 2.0.
//
//  int minLength:              The minimum length contour to constructed.
//
//  float minJump:              A scale factor which is multiplied by the
//                              image noise level to determine the gradient
//                              threshold at a junction. Nominally 1.0.
//
//  float maxGap:               The width of a gap which can be crossed in
//                              forming a junction with another edgel contour.
//                              Nominally sqrt(5) = 2.24.
//
//  bool spacingp:              If true, then equalize the sub-pixel locations
//                              of each edgel by averaging the adjacent left
//                              a right neighbor locations. Nominally true.
//
//  bool borderp:               If true, insert virtual contours at the border
//                              to close regions. Nominally false.
//
//
// .SECTION Author:
//             Jane S. Liu - 3/27/95
//             GE Corporate Research and Development
//
// .SECTION Modifications :
//             JLM - May 1997
//             Added extra interface for parameters not intitially
//             provided by Jane.These parameters are needed to get
//             satisfactory boundary closure. Also expanded comments.
//             JLM - November 1997
//             Moved most parameters up to gevd_detectorParams in
//             order to unify the use of parameters.
//-----------------------------------------------------------------------------

class gevd_bufferxy;

#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vil/vil_image.h>

#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_edge_2d.h>
#include "gevd/gevd_detector_params.h"

class gevd_detector : public gevd_detector_params
{
public:
  // So far, not all parameters are included in the constructor.  These seem to
  // be the most important in controlling performance - JLM
  //
  gevd_detector(gevd_detector_params& params);
  gevd_detector(vil_image, float smoothSigma = 1.0, float noiseSigma =2.0,
           float contourFactor = 1.0, float junctionFactor = 1.5,
           int minLength = 6, float maxGap = 2.23606, float minJump=1.0);
  ~gevd_detector();

  // External interfaces
  //Step contour detection - various return formats

  void DoContourDetector(vcl_vector<vtol_edge_2d_sptr >*);

  // New versions using the parameter block

  vcl_vector<vtol_edge_2d *>  DoContourDetector(vil_image image);
  void  DoContourDetector(vil_image image, vcl_vector<vtol_edge_2d_sptr >& edgels);

  //Fold contour detection
  void DoFoldContourDetector(vil_image image, vcl_vector<vtol_edge_2d_sptr >& edgels);

  //Corner detection using curvature on edgel chains
  //GEOFF  void  DoCornerDetector(vil_image image, IUPointGroup& corners);

  //Corner detection using curvature on edgel chains
  void  DoBreakCorners(vcl_vector<vtol_edge_2d_sptr >& in_edgels, vcl_vector<vtol_edge_2d_sptr >& out_edgels);

  // internal interfaces
  bool DoContour();
  bool DoFoldContour();
  bool DoCorner( float angle = 10,      // smallest angle at corner
                 float separation = 1,  // |mean1-mean2|/sigma
                 int length = 5,        // min length to find cornersxo
                 int cycle = 2,         // number of corners in a cycle
                 int ndimension = 2     // number of dimension
                 );
  bool DoStep();
  bool DoFold();

  gevd_bufferxy* GetBufferFromImage();

  vcl_vector<vtol_vertex_2d_sptr> *GetVertices() {return vertices;}
  vcl_vector<vtol_edge_2d_sptr> *GetEdges() {return edges;}
  void SetImage(vil_image img) {image = img;}

  void print(vcl_ostream &strm=vcl_cout) const;


protected:
  void UnProtectLists();
  void ClearData(); // clear buffer

protected:
  vil_image image;

  float noise; // noise estimation/threshold

  gevd_bufferxy *edgel,                      // output from DoStep
    *direction, *locationx, *locationy, *grad_mag, *angle; // detect step/fold
  int *junctionx, *junctiony, njunction; // junctions found
  vcl_vector<vtol_vertex_2d_sptr >* vertices; // network of linked edges/vertices
  vcl_vector<vtol_edge_2d_sptr >* edges;

  float filterFactor;     // factor in convolution filter
  float hysteresisFactor; // hysteresis factor
  float noiseThreshold;
};

#endif // _gevd_detector_h_
