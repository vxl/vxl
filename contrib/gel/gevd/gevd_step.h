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
#ifndef _Step_h_
#define _Step_h_
//
// .NAME Step - detection of step profiles in the intensity image
// .LIBRARY Detection
// .HEADER Segmentation package
// .INCLUDE Detection/Step.h
// .FILE Step.h
// .FILE Step.C
//
// .SECTION Description
// Operator to implement Canny edge detector which finds elongated
// step contours with dG. Then junctions are found by extending
// from end points of dangling contours.
// The recipe is:
//
//    1. Convolution with Gaussian with sigma typically = 1,
//       to well-condition the surface before taking first derivative.
//       Result is a smoothed image.
//
//    2. Convolution with first derivative, or first difference.
//       Result is a gradient image. Canny proves that first-derivative
//       of the Gaussian is the optimum filter to detect elongated
//       step profiles.
//
//    3. Optionally estimate sensor/texture sigma, if given noise
//       sigma is a negative interpolation factor -k in range -[0 1].
//       noise_sigma = (1-k)*sensor_sigma + k*texture_sigma.
//       Sensor and texture sigmas are estimated from the histogram
//       of weak step edges, detected in an ROI centered on gradient image.
//       (see Step constructor documentation in Step.C - JLM)
//
//    4. Non Maximum suppression in the direction of local gradient,
//       to detect pixels with maximum local slope, above apriori
//       noise response. Result is connected contours, width < 2,
//       broken only at junctions with weaker chains.
//       Also obtain subpixel accuracy normal to the contour.
//
//    5. Optionally extend from end points of contours to search for
//       maximum slope in the direction normal to the dangling contours,
//       above some factor of the noise response.
//       Result is a simple detection of all strong junctions.
//
// Input: Intensity image, smoothing sigma, sensor/texture noise sigma
//       and threshold factors for detecting contour/junction edge elements.
//
// Ouput: Magnitude, direction, location of step pixels, forming
//        a connected network of contours.
//
// Complexity: O(|pixels|) time and space for convolutions.
//             O(|edgels|) time for iterative extension to recover junctions.
//
// .EXAMPLE ../Examples/step.C
// .SECTION Authors
//  John Canny      (1986) SM Thesis
//  Chris Connolly  (1987) use directional 1st-difference
//  Van-Duc Nguyen  (1989) add subpixel location, extension to find junctions
//  Arron Heller    (1992) translate from CLOS to C++
//  Van-Duc Nguyen  (1995) add noise estimation, use FloatOperators.
//  Joe Mundy       (1997) expanded comments on some methods.  Added gradient
//                         magnitde, grad_mag,  and graident direction angle
//                         buffer, angle, to cache between edgel detection phases.
//-----------------------------------------------------------------------------

#include <vcl_iostream.h>
class gevd_bufferxy;


class gevd_step
{
public:
  gevd_step(float smooth_sigma=1,       // spatial smoothing [0.5 2.0]
            float noise_sigma=-0.5,     // sensor/texture intensity noise -[0 1]
            float contour_factor=1.0,   // threshold factor for contour edgels
            float junction_factor=1.5); // threshold factor for junction edgels
  ~gevd_step();

  bool DetectEdgels(const gevd_bufferxy& image, // float image
                    gevd_bufferxy*& edgels, // strength = dG * I
                    gevd_bufferxy*& direction, // direction % PI/4
                    gevd_bufferxy*& locationx, // subpixel loc
                    gevd_bufferxy*& locationy,
                    gevd_bufferxy*& grad_mag, //Gradient magnitude
                    gevd_bufferxy*& angle);    //Gradient orientation
  int RecoverJunctions(const gevd_bufferxy& image, // iterative extension
                       gevd_bufferxy& edgels, // from end points of contours
                       gevd_bufferxy& direction,
                       gevd_bufferxy& locationx, gevd_bufferxy& locationy,
                       int*& junctionx, int*& junctiony);

  float NoiseSigma() const;     // query stored/estimated noise sigma
  float NoiseResponse() const;  // response of noise sigma to filter ddG
  float NoiseThreshold(bool shortp=false) const; // elongated/directional?

  static float NoiseResponseToFilter(const float noiseSigma,
                                     const float smoothSigma,
                                     const float filterFactor);

  friend vcl_ostream& operator << (vcl_ostream& os, const gevd_step& st);
  friend vcl_ostream& operator << (vcl_ostream& os, gevd_step& st);

protected:
  float smoothSigma;                   // spatial smoothing
  float noiseSigma;                    // sensor/texture noise
  float contourFactor, junctionFactor; // threshold factor for edgels
  float filterFactor;                  // factor in convolution filter
};

#endif
