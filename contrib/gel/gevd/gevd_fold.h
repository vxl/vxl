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
//  Authors
//   John Canny      (1986) SM Thesis
//   Chris Connolly  (1987) use directional 1st-difference
//   Van-Duc Nguyen  (1989) add subpixel location, extension to find junctions
//   Arron Heller    (1992) translate from CLOS to C++
//   Van-Duc Nguyen  (1995) add noise estimation, use FloatOperators.
//   Joe Mundy       (1997) Added strength and direction arrays for transfer to
//                          edgel chains. The flag "transfer" controls the
//                          setting of these arrays. Note difference with
//                          gevd_step.h.. Didn't want to affect global use of
//                          DetectEdgels.
// \endverbatim
//-----------------------------------------------------------------------------

#include <vcl_iosfwd.h>
class gevd_bufferxy;

class gevd_fold
{
 public:
  //: Save parameters and create workspace for detecting fold profiles.
  // High frequency features are smoothed away by smooth_sigma.
  // Texture or white noise less than noise_sigma are not detected.
  // smooth_sigma = 0.5-2.0, 1.0 insures separation of independent folds >= 2.
  // noise_sigma = is standard deviation of intensity, in a uniform region.
  // Optionally estimate sensor/texture sigma, if given noise sigma
  // is a negative factor -k between -[0 1]. In this case,
  // noise_sigma = (1-k)*sensor_sigma + k*texture_sigma.
  // Response of white noise to the filter ddG is computed, and
  // then multiplied by contour_factor/junction_factor to obtain
  // thresholds for detecting contour/junction edges.
  //
  gevd_fold(float smooth_sigma=1,       //!< spatial smoothing [0.5 2.0]
            float noise_sigma=-0.5,     //!< sensor/texture intensity noise -[0 1]
            float contour_factor=1.0,   //!< threshold factor for contour edgels
            float junction_factor=1.5); //!< threshold factor for junction edgels

  //: Free space allocated for detecting fold profiles.  Does nothing.
  ~gevd_fold() {}

  static gevd_bufferxy* null_bufferxy;

  //: Detect fold profiles with ddG edge detector.
  // The image is convolved with a Gaussian to smooth away
  // high frequency noise, and insure separation of fold responses.
  // Then the largest absolute eigenvalue, and corresponding eigenvector
  // of the local Hessian are computed
  // using second difference [+1 -2 +1].
  // Optionally estimate sensor/texture sigma and set threshold.
  // Finally, non maximum suppression is done to find strict
  // local maxima of curvature.
  // The edge detector finds elongated contours only.
  // These contours are typically broken at junctions because non
  // maximum suppression is done along only the strongest direction.
  // Return contour (float), direction (byte), location (float) images.
  // Return true if no exception.
  // J. Canny, A Computational Approach to Edge Detection,
  // IEEE Trans on PAMI, vol 8, no 6, Nov 1986.
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

  //:
  // Find junctions by searching for extensions of contours from
  // their dangling end points. Non maximum suppression insures that
  // contours have width < 2, and so we can find the left/right neighbors,
  // and deduce end points. By using a minimally smoothed image,
  // we find fold profiles up to joining with a stronger contour, thus
  // recovering the missing junction caused by NMS along only 1 direction.
  // The junctions are returned but are not set in the contour image,
  // to prevent incorrect tracing of stronger contours first.
  int RecoverJunctions(const gevd_bufferxy& image, //!< iterative extension
                       gevd_bufferxy& edgels, //!< from end points of contours
                       gevd_bufferxy& direction,
                       gevd_bufferxy& locationx, gevd_bufferxy& locationy,
                       int*& junctionx, int*& junctiony);

  //: query stored/estimated noise sigma
  // Return the standard deviation of raw noise, in the original image,
  // either estimated or given by the user. If the noise has not been
  // estimated, return 0.
  float NoiseSigma() const;

  //: response of noise sigma to filter ddG
  // Compute response of white noise through the filter ddG, or
  // second-derivative of the Gaussian. Using a threshold of 3 times
  // this noise response would eliminate 99% of the noise edges.
  float NoiseResponse() const;

  //: elongated/directional?
  // Return threshold for detecting contour or junction,
  // which is response of white gaussian noise, noise_sigma,
  // to fold edge detector, i.e. second-order derivative of Gaussian,
  // smooth_sigma.
  // noise_sigma can be estimated by finding the standard deviation
  // in a region of constant intensity, and no texture patterns.
  // Use short_factor*noise_sigma and smooth_sigma/2, when detecting
  // junctions, to account for multiple responses to fold edge detector.
  float NoiseThreshold(bool shortp=false) const;

  //:
  // Compute response of white noise through the filter ddG, or
  // second-derivative of the Gaussian. Using a threshold of 3 times
  // this noise response would eliminate 99% of the noise edges.
  static float NoiseResponseToFilter(float noiseSigma,
                                     float smoothSigma,
                                     float filterFactor);

  friend vcl_ostream& operator<<(vcl_ostream& os, const gevd_fold& st);
  friend vcl_ostream& operator<<(vcl_ostream& os, gevd_fold& st);

 protected:
  float smoothSigma;                   //!< spatial smoothing
  float noiseSigma;                    //!< sensor/texture noise
  float contourFactor, junctionFactor; //!< threshold factor for edgels
  float filterFactor;                  //!< factor in convolution filter
};

#endif // gevd_fold_h_
