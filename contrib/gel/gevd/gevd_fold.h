#ifndef gevd_fold_h_
#define gevd_fold_h_
//:
// \file
// \brief detection of fold profiles in the intensity image
//
// Operator to implement Canny edge detector which finds elongated
// fold/roof contours with ddG. Then junctions are found by extending
// from end points of dangling contours.
//
// The recipe is:
//    -  Convolution with Gaussian with sigma typically = 1,
//       to well-condition the surface before taking second derivative.
//       Result is a smoothed image.
//
//    -  Convolution with Laplacian, or second difference.
//       Result is a gradient image. Canny proves that first-derivative
//       of the Gaussian is the optimum filter to detect elongated
//       step profiles.
//
//    -  Optionally estimate sensor/texture sigma, if given noise
//       sigma is a negative interpolation factor -k in range -[0 1].
//       noise_sigma = (1-k)*sensor_sigma + k*texture_sigma.
//       Sensor and texture sigmas are estimated from the histogram
//       of weak step edges, detected in an ROI centered on gradient image.
//       (see Step constructor documentation in Step.C - JLM)
//
//    -  Non Maximum suppression in the direction of local Hessian,
//       to detect pixels with maximum local curvature, above a priori
//       noise response. Result is connected contours, width < 2,
//       broken only at junctions with weaker chains.
//       Also obtain subpixel accuracy normal to the contour.
//
//    -  Optionally extend from end points of contours to search for
//       maximum curvature in the direction normal to the dangling contours,
//       above some factor of the noise response.
//       Result is a simple detection of all strong junctions.
//
// Input: Intensity image, smoothing sigma, sensor/texture noise sigma
//        and threshold factors for detecting contour/junction edge elements.
//
// Output: Magnitude, direction, location of fold pixels, forming
//         a connected network of contours.
//
// Complexity: O(|pixels|) time and space for convolutions.
//             O(|edgels|) time for iterative extension to recover junctions.
//
// \verbatim
// Authors
//  John Canny      (1986) SM Thesis
//  Chris Connolly  (1987) use directional 1st-difference
//  Van-Duc Nguyen  (1989) add subpixel location, extension to find junctions
//  Arron Heller    (1992) translate from CLOS to C++
//  Van-Duc Nguyen  (1995) add noise estimation, use FloatOperators.
//  Joe Mundy       (1997) Added strength and direction arrays for transfer to
//                         edgel chains. The flag "transfer" controls the 
//                         setting of these arrays. Note difference with
//                         gevd_step.h.. Didn't want to affect global use of
//                         DetectEdgels.
//
// \endverbatim
//-----------------------------------------------------------------------------

#include <vcl_iosfwd.h>
class gevd_bufferxy;

class gevd_fold
{
 public:
  gevd_fold(float smooth_sigma=1,       //!< spatial smoothing [0.5 2.0]
            float noise_sigma=-0.5,     //!< sensor/texture intensity noise -[0 1]
            float contour_factor=1.0,   //!< threshold factor for contour edgels
            float junction_factor=1.5); //!< threshold factor for junction edgels
  ~gevd_fold();

  static gevd_bufferxy* null_bufferxy;

  bool DetectEdgels(const gevd_bufferxy& image, //!< float image
                    gevd_bufferxy*& edgels, //!< strength = dG * I
                    gevd_bufferxy*& direction, //!< direction % PI/4
                    gevd_bufferxy*& locationx, //!< subpixel loc
                    gevd_bufferxy*& locationy,
                    bool peaks_only = false,
                    bool valleys_only = false,
                    bool transfer = false, //if true, fill mag and angle arrays
                    gevd_bufferxy*& mag = null_bufferxy, // d2G magnitude
                    gevd_bufferxy*& angle = null_bufferxy); //Gradient orientation
  int RecoverJunctions(const gevd_bufferxy& image, //!< iterative extension
                       gevd_bufferxy& edgels, //!< from end points of contours
                       gevd_bufferxy& direction,
                       gevd_bufferxy& locationx, gevd_bufferxy& locationy,
                       int*& junctionx, int*& junctiony);

  float NoiseSigma() const;     //!< query stored/estimated noise sigma
  float NoiseResponse() const;  //!< response of noise sigma to filter ddG
  float NoiseThreshold(bool shortp=false) const; //!< elongated/directional?

  static float NoiseResponseToFilter(const float noiseSigma,
                                     const float smoothSigma,
                                     const float filterFactor);

  friend vcl_ostream& operator<<(vcl_ostream& os, const gevd_fold& st);
  friend vcl_ostream& operator<<(vcl_ostream& os, gevd_fold& st);

 protected:
  float smoothSigma;                   //!< spatial smoothing
  float noiseSigma;                    //!< sensor/texture noise
  float contourFactor, junctionFactor; //!< threshold factor for edgels
  float filterFactor;                  //!< factor in convolution filter
};

#endif // gevd_step_h_
