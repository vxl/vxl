//:
// \file
#include "sdet_detector_params.h"
#include <vcl_string.h>
#include <vcl_sstream.h>

//------------------------------------------------------------------------
// Constructors
//

sdet_detector_params::sdet_detector_params(const sdet_detector_params& dp)
{
  InitParams(dp.smooth, dp.noise_weight, dp.noise_multiplier,
             dp.automatic_threshold, dp.aggressive_junction_closure,
             dp.minLength, dp.maxGap, dp.minJump, dp.contourFactor,
             dp.junctionFactor, dp.junctionp, dp.spacingp, dp.borderp,
             dp.peaks_only, dp.valleys_only,
             dp.corner_angle, dp.separation, dp.min_corner_length,
             dp.cycle, dp.ndimension);
}

sdet_detector_params::sdet_detector_params(float smooth_sigma, float noise_w,
                                           float noise_m, bool automatic_t,
                                           int aggressive_jc, int minl,
                                           float maxgp, float minjmp,
                                           float contour_f, float junction_f,
                                           bool recover_j, bool equal_spacing,
                                           bool follow_b,
                                           bool peaks_only,
                                           bool valleys_only,
                                           float ang, float sep, int min_corner_len,
                                           int cyc, int ndim)
{
  InitParams(smooth_sigma, noise_w, noise_m, automatic_t,
             aggressive_jc, minl, maxgp, minjmp,
             contour_f, junction_f, recover_j, equal_spacing,
             follow_b, peaks_only, valleys_only,
             ang, sep, min_corner_len,
             cyc, ndim);
}

void sdet_detector_params::InitParams(float smooth_sigma, float noise_w,
                                      float noise_m, bool automatic_t,
                                      int aggressive_jc, int minl,
                                      float maxgp, float minjmp,
                                      float contour_f, float junction_f,
                                      bool recover_j, bool equal_spacing,
                                      bool follow_b,
                                      bool only_peaks,
                                      bool only_valleys,
                                      float ang, float sep, int min_corner_len,
                                      int cyc, int ndim)
{
  //Step contour parameters
  smooth = smooth_sigma;
  noise_weight = noise_w;
  noise_multiplier = noise_m;
  automatic_threshold = automatic_t;
  aggressive_junction_closure = aggressive_jc;
  minLength = minl;
  spacingp = equal_spacing;
  borderp = follow_b;
  junctionp = recover_j;
  // Fold Parameters
  peaks_only = only_peaks;
  valleys_only = only_valleys;
  //Corner parameters
  corner_angle = ang;
  separation = sep;
  min_corner_length = min_corner_len;
  cycle = cyc;
  ndimension = ndim;
  // The remaining parameters are set according to the state of
  // Agressive junction closure.  If the value is <0 then the
  // state of junction parameters is derived from the constructor
  // arguments.  If the value is >0 then the variable is assumed to
  // be a bool and the parameters are determined from computation.

  if (aggressive_junction_closure<0)
    {
      junctionp = recover_j;
      contourFactor = contour_f;
      junctionFactor = junction_f;
      maxGap = maxgp;
      minJump = minjmp;
    }

  // Perform the sanity check anyway.
  SanityCheck();
}

void sdet_detector_params::set_noise_weight(float nw)
{
  noise_weight = nw;
}

void sdet_detector_params::set_noise_multiplier(float nm)
{
  noise_multiplier = nm;
}

void sdet_detector_params::set_automatic_threshold(bool at)
{
  automatic_threshold = at;
}

void sdet_detector_params::set_aggressive_junction_closure(int ajc)
{
  aggressive_junction_closure = ajc;
}

void sdet_detector_params::set_close_borders(bool cb)
{
  borderp = cb;
}


//-----------------------------------------------------------------------------
//
//: Checks that parameters are within acceptable bounds.
// This method is always called after a parameter modifier has changed the prms.
//
bool sdet_detector_params::SanityCheck()
{
  vcl_stringstream msg;
  bool valid = true;

  if (aggressive_junction_closure >0 )
  {
    junctionp = true;
    contourFactor = noise_multiplier;
    junctionFactor = .5f*noise_multiplier;
    maxGap = 4.f;
    minJump = .1f;
  }
  if (aggressive_junction_closure == 0)
  {
    contourFactor = noise_multiplier;
    junctionFactor = 1.5f*noise_multiplier;
    maxGap = 2.2f;
    minJump = 1.0f;
  }
  if (smooth <= 0)      // Standard deviation of the smoothing kernel
  {
    msg << "ERROR: Value of gaussian smoothing sigma is not positive: "
        << smooth << " <= 0\0";
    smooth = smooth==0 ? 1.0f : -smooth;
  }
  // MPP 2/11//2002
  // Invert noise_weight sign per Jim G.
  if (noise_weight > 0.0 || noise_weight < -1.0)   // Noise weighting factor
  {
    msg << "ERROR: Value of noise weight must be between -1 and 0, not "
        << noise_weight << "\0";
    noise_weight = -0.5f;
  }
  if (noise_multiplier <= 0)    // The over all noise scale factor
  {
    msg << "ERROR: Value of noise scale factor is not positive: "
        << noise_multiplier << " <= 0\0";
    noise_multiplier = noise_multiplier==0 ? 1.0f : -noise_multiplier;
  }
  if (minLength <= 3)   // Edgel chain length
  {
    msg << "ERROR: Value of minimum chain length is too low: "
        << minLength << " <= 3\0";
    minLength = 3;
  }
  if (maxGap <= 0)      // Chain gaps to jump
  {
    msg << "ERROR: Value of maximum gap is not positive: "
        << maxGap << " <= 0\0";
    maxGap = 2.2f;
  }
  if (minJump <= 0)     // Jump to close a junction
  {
    msg << "ERROR: Value of min jump junction is not positive: "
        << minJump << " <= 0\0";
    maxGap = 1.0f;
  }
  if (contourFactor <= 0)       // Threshold in following a contour
  {
    msg << "ERROR: Value of contour factor is not positive: "
        << contourFactor << " <= 0\0";
    contourFactor = 1.0f;
  }
  if (junctionFactor<= 0)       // Threshold in following a junction
  {
    msg << "ERROR: Value of junction factor is not positive: "
        << junctionFactor << " <= 0\0";
    maxGap = 1.5f;
  }
  if (peaks_only&&valleys_only)
  {
    msg << "ERROR: Can restrict to either peaks or valleys, not both\0";
    valid = false;
  }
  if (corner_angle < 5.0f)
  {
    msg << "ERROR: Value of corner angle is too low: "
        << corner_angle << " < 5\0";
    valid = false;
  }
  if (separation < 1.0f)
  {
    msg << "ERROR: Value of corner separation is too low: "
        << separation << " < 1\0";
    valid = false;
  }
  if (min_corner_length < 5)
  {
    msg << "ERROR: Value of minimum chain length too low: "
        << min_corner_length << " < 5\0";
    valid = false;
  }
  if (cycle > 10)
  {
    msg << "ERROR: Value of number of corners in a 1-cycle is too high: "
        << cycle << " > 10\0";
    valid = false;
  }
  if (ndimension > 3)
  {
    msg << "ERROR: Value of corner spatial dimension is too large: "
        << ndimension << " > 3\0";
    valid = false;
  }

  SetErrorMsg(msg.str().c_str());
  return valid;
}
vcl_ostream& operator << (vcl_ostream& os, const sdet_detector_params& dp)
{
  vcl_string sa, st;
  if(dp.aggressive_junction_closure>0)
    sa = "yes";
  else
    sa = "no";
  if(dp.automatic_threshold)
    st = "yes";
  else
    st = "no";
  os << "Edge Detector Params:" << vcl_endl;
  os << " Smooth Sigma " << dp.smooth << vcl_endl;
  os << " Noise Weight " << dp.noise_weight << vcl_endl;
  os << " Noise Multiplier " << dp.noise_multiplier << vcl_endl;
  os << " Automatic Threshold? " << st << vcl_endl;
  os << " Agressive Closure " << sa << vcl_endl;
  os << " Recover Junctions " << dp.junctionp << vcl_endl;
  os << " Minimum Chain Length " << dp.minLength << vcl_endl;
  os << " Peaks Only " << dp.peaks_only << vcl_endl;
  os << " Valleys Only " << dp.valleys_only << vcl_endl << vcl_endl;
  os << "Corner Detection Params: " << vcl_endl;
  os << " Corner Angle " << dp.corner_angle << vcl_endl;
  os << " Corner Separation " << dp.separation  << vcl_endl;
  os << " Min Corner Length " << dp.min_corner_length << vcl_endl;
  os << " Close borders " << dp.borderp << vcl_endl << vcl_endl;

  return os;
}
//------------------------------------------------------------
//: Describe the parameters to a parameter modifier.
void sdet_detector_params::Describe(ParamModifier& /* mod */)
{
//   static UIChoice JunctionClosure[] =
//   {
//     UIChoicePair("Default",-1),
//     UIChoicePair("Weak",0),
//     UIChoicePair("Aggressive",1),
//     UIChoice_finish
//   };

//   // These are the parameters which the user is likely to want to set.
//   mod.Name("EdgeDetector Parameters(VanDuc)");
//   mod.AddParam("Sigma", smooth);
//   mod.AddParam("Noise Weight", noise_weight);
//   mod.AddParam("Noise Multiplier", noise_multiplier);
//   mod.AddParam("Automatic Threshold", automatic_threshold,
//             ParamModifier::OnOff);
//   mod.AddParam("Junction Closure", aggressive_junction_closure,
//             JunctionClosure);
}
