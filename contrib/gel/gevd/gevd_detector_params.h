#ifndef gevd_detector_params_h_
#define gevd_detector_params_h_
//:
// \file
// \brief non-display-based interface class
//
// The parameter mixin for VanDuc's edge detector.
//
// - float smooth : The standard deviation of the Gaussian smoothing kernel.
//
// - float noise_weight: A weighting factor that determines the relative
//                     proportion of sensor noise level and texture noise level
//                     as measured in a ROI in the center of the image. The
//                     nominal value of 0.5 gives equal weight to both.
//
// - float noise_multiplier: Overall scale factor for noise
//
// - bool automatic_threshold: If true then the noise level is determined from
//                            image measurements.
//
// - float filterFactor:  An overall scale factor for determining
//                       gradient threshold Nominally 2.0.
//
// - float contourFactor, junctionFactor: Scale factors for determining the
//                                       gradient threshold. Nominally 1.0.
//                                       contourFactor is in effect for edgels
//                                       on contours (boundaries).
//                                       junctionFactor is in effect during the
//                                       extension of contours at endpoints.
//                                       To extend contours aggressively, use a
//                                       low value of junctionFactor, i.e., .5.
//
// - bool  junctionp:  If true, then recover junctions by extending contours.
//                    Nominally true.
//
// - Contour Following:
// - float hysteresisFactor:     A scale factor which is multiplied by the
//                              image noise level to determine the minimum
//                              gradient threshold in following an edgel contour.
//                              Nominally 2.0.
//
// - int minLength:              The minimum length contour to constructed.
//
// - float minJump:              A scale factor which is multiplied by the
//                              image noise level to determine the gradient
//                              threshold at a junction. Nominally 1.0.
//
// - float maxGap:               The width of a gap which can be crossed in
//                              forming a junction with another edgel contour.
//                              Nominally sqrt(5) = 2.24.
//
// - bool spacingp:              If true, then equalize the sub-pixel locations
//                              of each edgel by averaging the adjacent left
//                              a right neighbor locations. Nominally true.
//
// - bool borderp:               If true, insert virtual contours at the border
//                              to close regions. Nominally false.
//
//
// \author Joseph L. Mundy - GE Corporate Research and Development
// \date   November 1997
//-----------------------------------------------------------------------------

#include "gevd_param_mixin.h"

class gevd_detector_params : public gevd_param_mixin
{
 public:

  gevd_detector_params(float smooth_sigma = 1.0, float noise_w = -0.5,
                       float noise_m = 2, bool automatic_t = false,
                       int aggressive_jc = 1, int minl = 4,
                       float maxgp = 4, float minjmp = 0.1,
                       float contour_f = 2.0, float junction_f = 1.0,
                       bool recover_j = true, bool equal_spacing=true,
                       bool follow_b = true,
                       bool peaks_only=false,
                       bool valleys_only=false,
                       float ang = 10, float sep = 1, int min_corner_len = 5,
                       int cyc = 2, int ndim = 2);

  gevd_detector_params(const gevd_detector_params& old_params);
  ~gevd_detector_params() {}

  bool SanityCheck();
  void Describe(ParamModifier& mod);

  void set_noise_weight(float noise_weight);
  void set_noise_multiplier(float noise_multiplier);
  void set_automatic_threshold(bool automatic_threshold);
  void set_aggressive_junction_closure(int aggressive_junction_closure);
  void set_close_borders(bool close_borders);

 protected:
  void InitParams(float smooth_sigma, float noise_w,
                  float noise_m, bool automatic_t,
                  int aggressive_jc, int minl,
                  float maxgp, float minjmp,
                  float contour_f, float junction_f,
                  bool recover_j, bool equal_spacing,
                  bool follow_b,
                  bool peaks_only,
                  bool valleys_only,
                  float ang, float sep, int min_corner_len,
                  int cyc, int ndim);

 public:
  //
  // Parameters for detecting edgel chains
  //
  float smooth; // !< Smoothing kernel sigma
  float noise_weight; //!< The weight between sensor noise and texture noise
  float noise_multiplier; // !< The overal noise threshold scale factor
  bool   automatic_threshold; // !< Determine the threshold values from image
  int aggressive_junction_closure; //!< Close junctions agressively
  int minLength;                // !< minimum chain length
  float contourFactor;  //!< Threshold along contours
  float junctionFactor; //!< Threshold at junctions
  float filterFactor;   // !< ratio of sensor to texture noise
  bool junctionp; // !< recover missing junctions
  float minJump;  // !< change in strength at junction
  float maxGap;   // !< Bridge small gaps up to max_gap across.
  bool spacingp;  // !< equalize spacing?
  bool borderp;   // !< insert virtual border for closure?
  //
  // Fold detection parameters
  //
  bool peaks_only; //!< Only return peaks, d^2I/dn^2 < 0, n is normal dir to ridge
  bool valleys_only; //!< Only return valeys, d^2I/dn^2 > 0
  //
  // Parameters for corner detection on edgel chains
  //
  float corner_angle; // !< smallest angle at corner
  float separation; // !< |mean1-mean2|/sigma
  int min_corner_length; // !< min length to find corners
  int cycle; // !< number of corners in a cycle
  int ndimension; // !< spatial dimension of edgel chains.
};

#endif // gevd_detector_params_h_
