//:
// \file
#include "gevd_detector_params.h"

#include <vcl_strstream.h>
//#include <Basics/types.h> // Gets AttributeValuePair

//------------------------------------------------------------------------
// Constructors
//

gevd_detector_params::gevd_detector_params(const gevd_detector_params& dp)
{
  InitParams(dp.smooth, dp.noise_weight, dp.noise_multiplier,
             dp.automatic_threshold, dp.aggressive_junction_closure,
             dp.minLength, dp.maxGap, dp.minJump, dp.contourFactor,
             dp.junctionFactor, dp.junctionp, dp.spacingp, dp.borderp,
             dp.corner_angle, dp.separation, dp.min_corner_length,
             dp.cycle, dp.ndimension);
}

gevd_detector_params::gevd_detector_params(float smooth_sigma, float noise_w,
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

void gevd_detector_params::InitParams(float smooth_sigma, float noise_w,
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

void gevd_detector_params::set_noise_weight(float nw)
{
  noise_weight = nw;
}

void gevd_detector_params::set_noise_multiplier(float nm)
{
  noise_multiplier = nm;
}

void gevd_detector_params::set_automatic_threshold(bool at)
{
  automatic_threshold = at;
}

void gevd_detector_params::set_aggressive_junction_closure(int ajc)
{
  aggressive_junction_closure = ajc;
}

void gevd_detector_params::set_close_borders(bool cb)
{
  borderp = cb;
}


//-----------------------------------------------------------------------------
//
//: Checks that parameters are within acceptable bounds.
// This method is always called after a parameter modifier has changed the prms.
//
bool gevd_detector_params::SanityCheck()
{
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
      junctionp = true;
      contourFactor = noise_multiplier;
      junctionFactor = 1.5f*noise_multiplier;
      maxGap = 2.2f;
      minJump = 1.0f;
    }

  vcl_strstream msg;
  bool valid = true;
  if (smooth <= 0)      // Standard deviation of the smoothing kernel
  {
    msg<< "ERROR: Value of gaussian smoothing sigma is too low <=0" << vcl_ends;
    smooth = 1.0f;
    valid = false;
  }
  // MPP 2/11//2002
  // Invert noise_weight sign per Jim G.
  if (noise_weight > 0.0 || noise_weight < -1.0)   // Noise weighting factor
  {
    msg << "ERROR: Value of noise weight must be [-1.0 0.0]" << vcl_ends;
    noise_weight = -0.5f;
    valid = false;
  }
  if (noise_multiplier <= 0)    // The over all noise scale factor
  {
    msg << "ERROR: Value of noise scale factor is too low <=" << vcl_ends;
    noise_multiplier = 1.0f;
    valid = false;
  }

  if (minLength <= 3)   // Edgel chain length
  {
    msg << "ERROR: Value of minimum chain length is too low <= 3" << vcl_ends;
    minLength = 3;
    valid = false;
  }

  if (maxGap <= 0)      // Chain gaps to jump
  {
    msg << "ERROR: Value of maximum gap is too low <0" << vcl_ends;
    maxGap = 2.2f;
    valid = false;
  }

  if (minJump <= 0)     // Jump to close a junction
  {
    msg << "ERROR: Value of min jump junction is too low <0" << vcl_ends;
    maxGap = 1.0f;
    valid = false;
  }

  if (contourFactor <= 0)       // Threshold in following a contour
  {
    msg << "ERROR: Value of contour factor is too low <0" << vcl_ends;
    contourFactor = 1.0f;
    valid = false;
  }

  if (junctionFactor<= 0)       // Threshold in following a junction
  {
    msg << "ERROR: Value of junction factor is too low <0" << vcl_ends;
    maxGap = 1.5f;
    valid = false;
  }

  if (peaks_only&&valleys_only)
    {
    msg << "ERROR: Can restrict to either peaks or valleys, not both"
        << vcl_ends;
    valid = false;
    }

  if (corner_angle < 5.0f)
    {
      msg << "ERROR: Value of corner angle is too low <5" << vcl_ends;
    }

  if (separation < 1.0f)
    {
      msg << "ERROR: Value of corner separation is too low <1" << vcl_ends;
    }

  if (min_corner_length < 5)
    {
      msg << "ERROR: Value of minimum chain length too low <5" << vcl_ends;
    }

  if (cycle > 10)
    {
      msg << "ERROR: Value of number of corners in a 1-cycle is too "
          << "high > 10" << vcl_ends;
    }

  if (ndimension > 3)
    {
      msg << "ERROR: Value of corner spatial dimension is too large > 3"
          << vcl_ends;
    }
  msg << vcl_ends;
  SetErrorMsg(msg.str());
  delete [] msg.str();
  return valid;
}

//------------------------------------------------------------
//: Describe the parameters to a parameter modifier.
void gevd_detector_params::Describe(ParamModifier& mod)
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
