#ifndef sdet_detector_h_
#define sdet_detector_h_
//:
// \file
// \brief non-display-based interface class
//
// An interface to Van-Duc's Canny code. No display is involved.
// The Canny has two major phases: 1) detect step edgels, 2)Follow contours
// and construct a topological network. Each phase has a number of
//  parameters which are defined as follows.
//  Step Detection:
//
// - float contourFactor, junctionFactor: Scale factors for determining the
//                                        gradient threshold. Nominally 1.0.
//                                        contourFactor is in effect for edgels
//                                        on contours (boundaries).
//                                        junctionFactor is in effect during the
//                                        extension of contours at endpoints.
//                                        To extend contours aggressively, use a
//                                        low value of junctionFactor, i.e., .5.
//
// - float noiseThreshold: A weighting factor that determines the relative
//                         proportion of sensor noise level and texture noise level
//                         as measured in a ROI in the center of the image. The
//                         nominal value of -.5 gives equal weight to both. If the
//                         value is positive, then a default noise threshold of 1.0
//                         is assigned.
//
// - float filterFactor:  An overall scale factor for determining gradient threshold.
//                        Nominally 2.0.
//
// - bool  junctionp:  If true, then recover junctions by extending contours.
//                     Nominally true.
//
// - Contour Following:
// - float hysteresisFactor:     A scale factor which is multiplied by the
//                               image noise level to determine the minimum
//                               gradient threshold in following an edgel contour.
//                               Nominally 2.0.
//
// - int minLength:              The minimum length contour to constructed.
//
// - float minJump:              A scale factor which is multiplied by the
//                               image noise level to determine the gradient
//                               threshold at a junction. Nominally 1.0.
//
// - float maxGap:               The width of a gap which can be crossed in
//                               forming a junction with another edgel contour.
//                               Nominally sqrt(5) = 2.24.
//
// - bool spacingp:              If true, then equalize the sub-pixel locations
//                               of each edgel by averaging the adjacent left
//                               a right neighbor locations. Nominally true.
//
// - bool borderp:               If true, insert virtual contours at the border
//                               to close regions. Nominally false.
//
// \author
//             Jane S. Liu - 3/27/95
//             GE Corporate Research and Development
//
// \verbatim
//  Modifications
//   JLM - May 1997
//             Added extra interface for parameters not initially
//             provided by Jane.These parameters are needed to get
//             satisfactory boundary closure. Also expanded comments.
//   JLM - November 1997
//             Moved most parameters up to sdet_detectorParams in
//             order to unify the use of parameters.
// \endverbatim
//-----------------------------------------------------------------------------

class gevd_bufferxy;

#include <vcl_vector.h>
#include <vcl_iosfwd.h>
#include <vil1/vil1_image.h>

#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include "sdet_detector_params.h"

class sdet_detector : public sdet_detector_params
{
 public:
  // So far, not all parameters are included in the constructor.  These seem to
  // be the most important in controlling performance - JLM
  //
  sdet_detector(sdet_detector_params& params);
  sdet_detector(vil1_image, float smoothSigma = 1.0, float noiseSigma =2.0,
                float contourFactor = 1.0, float junctionFactor = 1.5,
                int minLength = 6, float maxGap = 2.23606, float minJump=1.0);
  ~sdet_detector();

  // External interfaces
  //Step contour detection
  bool DoContour();

  //Fold contour detection
  void DoFoldContourDetector(vil1_image image, vcl_vector<vtol_edge_2d_sptr >& edgels);

  //Corner detection using curvature on edgel chains
  //GEOFF  void  DoCornerDetector(vil1_image image, IUPointGroup& corners);

  //Corner detection using curvature on edgel chains
  void  DoBreakCorners(vcl_vector<vtol_edge_2d_sptr >& in_edgels, vcl_vector<vtol_edge_2d_sptr >& out_edgels);

  // internal interfaces
  bool DoFoldContour();
  bool DoCorner( float angle = 10,      //!< smallest angle at corner
                 float separation = 1,  //!< |mean1-mean2|/sigma
                 int length = 5,        //!< min length to find cornersxo
                 int cycle = 2,         //!< number of corners in a cycle
                 int ndimension = 2);   //!< number of dimension
  bool DoStep();
  bool DoFold();

  gevd_bufferxy* GetBufferFromImage();

  vcl_vector<vtol_vertex_2d_sptr> *GetVertices() {return vertices;}
  vcl_vector<vtol_edge_2d_sptr> *GetEdges() {return edges;}
  void SetImage(vil1_image img) {image = img;}

  void print(vcl_ostream &strm=vcl_cout) const;

 protected:
  void ClearData(); //!< clear buffer

 protected:
  vil1_image image;

  float noise; //!< noise estimation/threshold

  gevd_bufferxy *edgel,                      //!< output from DoStep
    *direction, *locationx, *locationy, *grad_mag, *angle; //!< detect step/fold
  int *junctionx, *junctiony, njunction; //!< junctions found

  vcl_vector<vtol_vertex_2d_sptr >* vertices;//!< network of linked
  vcl_vector<vtol_edge_2d_sptr >* edges; //!< edges and vertices

  float filterFactor;     //!< factor in convolution filter
  float hysteresisFactor; //!< hysteresis factor
  float noiseThreshold;
};

#endif // sdet_detector_h_
