// This is gel/gevd/gevd_fold.cxx
#include "gevd_fold.h"
//:
// \file
// Use 8 directions, with 45 degree angle in between them.
//
//\endverbatim

#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <gevd/gevd_noise.h>
#include <gevd/gevd_float_operators.h>
#include <gevd/gevd_pixel.h>
#ifdef DEBUG
# include <vul/vul_timer.h>
#endif

gevd_bufferxy* gevd_fold::null_bufferxy = 0;

const byte TWOPI = 8, FULLPI = 4, HALFPI = 2;
const int DIS[] = { 1, 1, 0,-1,-1,-1, 0, 1, // 8-connected neighbors
                    1, 1, 0,-1,-1,-1, 0, 1, // wrapped by 2PI to
                    1, 1, 0,-1,-1,-1, 0, 1};// avoid modulo operations.
const int DJS[] = { 0, 1, 1, 1, 0,-1,-1,-1,
                    0, 1, 1, 1, 0,-1,-1,-1,
                    0, 1, 1, 1, 0,-1,-1,-1};
const int RDS[] = {0,-1, 1,-2, 2,-3, 3,-4, 4,-5, 5}; // radial search

// const byte DIR0 = 8, DIR1 = 9, DIR2 = 10, DIR3 = 11;
const int FRAME = 4; // 3 for NMS and extension, 4 for contour

gevd_fold::gevd_fold(float smooth_sigma, // width of filter dG
                     float noise_sigma,   // sensor/texture intensity noise -[0 1]
                     float contour_factor, float junction_factor)
  : smoothSigma(smooth_sigma), noiseSigma(noise_sigma),
    contourFactor(contour_factor), junctionFactor(junction_factor),
    filterFactor(6)              // factor from gevd_float_operators::Hessian
{
  if (smoothSigma < 0.5)        // no guarrantee for 2-pixel separation
    vcl_cerr << "gevd_fold::gevd_fold -- too small smooth_sigma: "
             << smoothSigma << vcl_endl;
  if (smoothSigma > 2)          // smooth out too much the junctions
    vcl_cerr << "gevd_fold::gevd_fold -- too large smooth_sigma: "
             << smoothSigma << vcl_endl;
  if (noiseSigma < -1) {
    vcl_cerr << "gevd_fold::gevd_fold -- noiseSigma out of range -[0 1]: "
             << noiseSigma << ". Reset to -1.\n";
    noiseSigma = -1;
  }

  //vcl_cout << "Init Step\n" << *this << vcl_endl;
}


bool
gevd_fold::DetectEdgels(const gevd_bufferxy& image,
                        gevd_bufferxy*& contour, gevd_bufferxy*& direction,
                        gevd_bufferxy*& locationx, gevd_bufferxy*& locationy,
                        bool peaks_only,
                        bool valleys_only,
                        bool transfer, //compute mag and angle? default=false
                        gevd_bufferxy*& mag, gevd_bufferxy*& angle)
{
  //vcl_cout << "*** Detect step profiles with second-derivative of Gaussian"
  //         << *this
  //         << vcl_endl;
  if (image.GetBitsPixel() != bits_per_float) {
    vcl_cerr << "gevd_fold::DetectEdgels requires float image\n";
    return false;
  }

  // -tpk @@ missing check if the requested buffer size is too small to contain the convolution operations

  // 1. Smooth image to regularize data, before taking derivatives
  gevd_bufferxy* smooth = NULL;      // Gaussian smoothed image
  // use float to avoid overflow/truncation
  filterFactor = gevd_float_operators::Gaussian((gevd_bufferxy&)image, // well-condition before
                                                smooth, smoothSigma); // 2nd-difference

  // 2. Use 2nd-difference to estimate local curvature, filter is ddG.
  gevd_bufferxy *curvature = NULL;
  // need to make new arrays since later NonMaximumSupression clears
  // locationx locationy
  gevd_bufferxy *dirx = gevd_float_operators::SimilarBuffer(image);
  gevd_bufferxy *diry = gevd_float_operators::SimilarBuffer(image);
  filterFactor *= gevd_float_operators::Hessian(*smooth, // 2nd-order difference
                                                 curvature, dirx, diry); // mult factor returned
  //If only peaks or valleys are asked for
  for (int j = 0; j < image.GetSizeY(); j++)
    for (int i = 0; i < image.GetSizeX(); i++)
      if ( (peaks_only   && floatPixel(*diry, i, j)<0) ||
           (valleys_only && floatPixel(*diry, i, j)>0)
         )
        floatPixel(*curvature, i, j) = 0;

  delete smooth;

  // 2.5 JLM - Fill the theta array for use in outputting continuous digital curve
  //           directions later.  The angle definition here is consistent with
  //           EdgeDetector, i.e. angle = (180/PI)*atan2(dI/dy, dI/dx);
  if (transfer) //Fill magnitude and angle arrays needed by EdgelChain const.
  {
    mag = gevd_float_operators::SimilarBuffer(image);
    angle = gevd_float_operators::SimilarBuffer(image);
    const float kdeg = 180.0/vnl_math::pi;
    for (int j = 0; j < image.GetSizeY(); j++)
      for (int i = 0; i < image.GetSizeX(); i++)
        if ((floatPixel(*mag, i, j) = floatPixel(*curvature, i, j)))
            floatPixel(*angle, i, j) = kdeg*vcl_atan2(floatPixel(*diry, i, j),
                                                      floatPixel(*dirx, i, j));
          else
            floatPixel(*angle, i, j) = 0;
  }


  // 3. Estimate sensor/texture sigmas from histogram of weak step edgels
  if (noiseSigma <= 0)  {
    int nedgel = 0;             // all edgels in ROI at center of image
    float* edgels = gevd_noise::EdgelsInCenteredROI(*curvature, *dirx, *diry, nedgel);
    if (edgels) {
      gevd_noise noise(edgels, nedgel); // histogram of weak edgels only
      delete [] edgels;
      float sensorNoise, textureNoise;
      if (noise.EstimateSensorTexture(sensorNoise, textureNoise)) {
        const float k = -noiseSigma; // given linear interpolation factor
        noiseSigma = ((1-k)*sensorNoise + k*textureNoise) /
          NoiseResponseToFilter(1, smoothSigma, filterFactor);
      } else {
        vcl_cout << "Can not estimate sensor & texture noise\n";
        noiseSigma = 1;         // reasonable default for 8-bit
      }
    } else {
      vcl_cout << "Not enough edge elements to estimate noise\n";
      noiseSigma = 1;
    }
    //vcl_cout << "Set noise sigma = " << noiseSigma << vcl_endl;
  }

  // 4. Find contour pixels as local maxima along slope direction
  //
  //                  [i,j+1]
  //                    ^
  //                    |
  // Note that [i-1 ,j] -> [i+1,j] indicates the sign of dirx and diry.
  //                    |          That is, if the intensities at the arrows
  //                 [i, j-1]      are larger than that at (i,j) then dirx or
  //                               diry are positive.  Again shown for a
  //                               right-handed (i,j) system -- JLM
  //
  // Thus for the following contour:
  //          ^ j            Normal Direction
  //          |________            ^
  //    light |        |           |    dirx = 0 and diry = +.  The direction
  //        __|________|__Contour__|    code returned by NonMaximumSupression
  //          |xxxxxxxx|                is 10 -- modulo 8 is 2, as shown.
  //    dark  |xxxxxxxx|
  //           ----------> i
  //
  // -----------------------------------------------------------------

  gevd_float_operators::NonMaximumSuppression(*curvature, *dirx, *diry,
                                              NoiseThreshold(), // above noise
                                              contour, direction,
                                              locationx, locationy);
  delete curvature; delete dirx; delete diry;
  gevd_float_operators::FillFrameX(*contour, 0, FRAME); // erase pixels in frame border
  gevd_float_operators::FillFrameY(*contour, 0, FRAME);
  return true;
}

//:
// Return -/+ PI/2, to encode the existence of an end point
// on the left/right side of the current contour point i,j.
static int
LeftXorRightEnd(const gevd_bufferxy& contour,
                int i, int j, // pixel on contour
                int dir) // normal to contour
{
  int di = DIS[dir], dj = DJS[dir];
  bool normalp = (floatPixel(contour, i - di, j - dj) ||
                  floatPixel(contour, i + di, j + dj));
  if (normalp)                  // Substitute neighbor
    return 0;                   // for left or right side.
  bool leftp = false;
  int ndir = dir - HALFPI;      // left ndir
  for (int n = 0; n < 3; ++n) { // 3 neighbors
    int theta = ndir + RDS[n];
    if (floatPixel(contour, i+DIS[theta], j+DJS[theta])) {
      leftp = true;             // found neighbor on left side
      break;
    }
  }
  bool rightp = false;
  ndir = dir + HALFPI;          // right ndir
  for (int n = 0; n < 3; ++n) { // 3 neighbors
    int theta = ndir + RDS[n];
    if (floatPixel(contour, i+DIS[theta], j+DJS[theta])) {
    rightp = true;            // found neighbor on right side
      break;
    }
  }
  return (leftp? 0: -HALFPI) + (rightp? 0: HALFPI); // increment from dir
}

//: Find best fold extension from end point, which has largest local maximum curvature.
// Search all 3x3=9 neighboring locations/directions.
// Return location, direction and strength of this extension pixel.
//
static float
BestFoldExtension(const gevd_bufferxy& smooth,
                  int i, int j,
                  int ndir, // tangential dir to neighbor
                  float threshold,
                  int& best_i, int& best_j, // pixel
                  int& best_d, float& best_l) // direction + subloc
{
  float best_s = threshold;     // insure greater response
  const int direc = ndir + HALFPI; // fold direction
  for (int n = 0; n < 3; n++) { // all 3 neighboring pixels
    int ntheta = ndir + RDS[n];
    int ni = i + DIS[ntheta];
    int nj = j + DJS[ntheta];
    float pix = floatPixel(smooth, ni, nj); // center
    for (int d = 0; d < 3; d++) { // all 3 neighboring directions
      int dir = direc + RDS[d];
      int di = DIS[dir];
      int dj = DJS[dir];
      float pix_m = floatPixel(smooth, ni-di, nj-dj);
      float pix_p = floatPixel(smooth, ni+di, nj+dj);
      float curvature = (float)vcl_fabs(pix_p + pix_m - 2*pix);
      float max_s = (dir%HALFPI)? best_s*2: best_s;
      if (curvature > max_s) {      // find best strength
        int di2 = 2*di;
        int dj2 = 2*dj;
        if (curvature > vcl_fabs(pix + floatPixel(smooth, ni-di2, nj-dj2)
                                 - 2 * pix_m) &&
            curvature > vcl_fabs(pix + floatPixel(smooth, ni+di2, nj+dj2)
                                 - 2 * pix_p)) {
          best_i = ni;
          best_j = nj;
          best_s = (dir%HALFPI)? curvature/2 : curvature;
          best_d = dir%FULLPI + TWOPI; // in range [0 FULLPI) + TWOPI
        }
      }
    }
  }
  if (best_s > threshold) {     // interpolate with parabola
    float pix = floatPixel(smooth, best_i, best_j);
    int di = DIS[best_d], dj = DJS[best_d];
    int di2 = 2*di, dj2 = 2*dj;
    float s_m = (float)vcl_fabs(pix + floatPixel(smooth, best_i-di2, best_j-dj2)
                                - 2*floatPixel(smooth, best_i-di, best_j-dj));
    float s_p = (float)vcl_fabs(pix + floatPixel(smooth, best_i+di2, best_j+dj2)
                                - 2*floatPixel(smooth, best_i+di, best_j+dj));
    if (best_d%HALFPI) {
      s_m /= (float)2.0;
      s_p /= (float)2.0;
    }
    best_l = gevd_float_operators::InterpolateParabola(s_m, best_s, s_p, best_s);
    return best_s;
  } else                        // not found
    return 0;
}


int
gevd_fold::RecoverJunctions(const gevd_bufferxy& image,
                            gevd_bufferxy& contour, gevd_bufferxy& direction,
                            gevd_bufferxy& locationx, gevd_bufferxy& locationy,
                            int*& junctionx, int*& junctiony)
{
#if defined(DEBUG)
  vul_timer t;
#endif
  if (image.GetBitsPixel() != bits_per_float) {
    vcl_cerr << "gevd_fold::RecoverJunction requires float image\n";
    return false;
  }
  const int rmax = 1+FRAME;     // 1 + kernel radius of BestStepExtension
  const int kmax = int(4 * smoothSigma + 0.5) + 2; // gap = 2
  const int xmax = image.GetSizeX()-rmax-1; // fill fold direction
  const int ymax = image.GetSizeY()-rmax-1;
#ifdef DEBUG
  vcl_cout << "RecoverJunctions: rmax, kmax, xmax, ymax:" << rmax << ' ' << kmax << ' ' << xmax << ' ' << ymax << '\n';
#endif
  // 1. Find end points of dangling contours
  //const int length0 = xmax/kmax*ymax/kmax/4;// 25% size
  //const float growth = 2;     // growth ratio of the arrays

  vcl_vector<int> ndir; //  ndir.set_growth_ratio(growth);
  vcl_vector<int> xloc; //  xloc.set_growth_ratio(growth); // dynamic array instead of long lists
  vcl_vector<int> yloc; //  yloc.set_growth_ratio(growth);
  int xdir;
  for (int y = rmax; y <= ymax; y++) // find end points of long contours
    for (int x = rmax; x <= xmax; x++) // inside image border - rmax
      if (floatPixel(contour, x, y) && // on contour
          (xdir = LeftXorRightEnd(contour, x, y, // left xor right neighbor
                                  bytePixel(direction, x, y))) != 0)
      {
        ndir.push_back(xdir);   // save end point of elongated contours
        xloc.push_back(x);
        yloc.push_back(y);
      }
  const int length = ndir.size();
  //vcl_cout << "% end pats = "     // trace allocated size
  //          << length*100 / float((xmax/kmax)*(ymax/kmax)) << vcl_endl;

  // 2. Extend from end points until they touch other contours
  gevd_bufferxy* smooth = NULL;
  gevd_float_operators::Gaussian((gevd_bufferxy&)image, smooth, smoothSigma/2); // avoid oversmoothing
  const bool shortp = true;     // short contours
  const float threshold = NoiseThreshold(shortp);
  float curvature, loc;
  int njunction = 0;            // number of junctions found
  for (int r = 1; r <= kmax; r++) { // breadth-first extension
    int ntouch = 0, nextension = 0;
    for (int i = 0; i < length; i++)
      if ((xdir = ndir[i]) != 0 && xdir != TWOPI) // still extension?
      {
        int x = xloc[i], y = yloc[i];
        int dir = bytePixel(direction, x, y); // direction of fold
        curvature = BestFoldExtension(*smooth,
                                  x, y, dir + xdir,     // current end pt
                                  threshold,
                                  x, y, dir, loc);
        if (curvature) {                 // next end point
          xloc[i] = x, yloc[i] = y; // update new end point
          xdir = LeftXorRightEnd(contour,    // mark completed if
                                 x, y, dir); // another contour is reached,
                                             // indicated by xdir==0.
          if (xdir)
          {
            if (x < rmax || x > xmax || // check for reaching border
                y < rmax || y > ymax)
              xdir = 0;               // junction with virtual border
            else
              nextension++;
            floatPixel(contour, x, y) = curvature; // still disconnected
          }
          else
          {           // touching another contour
            ntouch++;
            xdir = TWOPI;     // mark junction, without linking chains
          }
          ndir[i] = xdir;
          bytePixel(direction, x, y) = byte(dir);
          floatPixel(locationx, x, y) = loc*DIS[dir];
          floatPixel(locationy, x, y) = loc*DJS[dir];
        } else                  // no further extension found
          ndir[i] = 0;
      }
    //vcl_cout << "Touch " << ntouch << " contours.\n";
    // vcl_cout << "Will extend " << nextension << " contours.\n";
    njunction += ntouch;
    if (!nextension) break;     // all either junction or termination
  }
  delete smooth;

  // 3. Return the end points or junctions found
  if (junctionx) delete [] junctionx;
  if (junctiony) delete [] junctiony;
  junctionx = new int[njunction];
  junctiony = new int[njunction];
  for (int i = 0, j = 0; i < length; i++)
    if (ndir[i] == TWOPI) {
      junctionx[j] = xloc[i];
      junctiony[j] = yloc[i];
      j++;
    }
#if defined(DEBUG)
  vcl_cout << "Find " << length << " end points, and "
           << njunction << " junctions.\n"
           << "Recover " << 100.0*njunction/length
           << "% end points as junctions > "
           << threshold << ", in " << t.real() << " msecs.\n";
#endif
  return njunction;
}


float
gevd_fold::NoiseSigma() const
{
  return (noiseSigma <= 0)? 0: noiseSigma;
}


float
gevd_fold::NoiseResponse() const
{
  return NoiseResponseToFilter(noiseSigma,
                               smoothSigma, filterFactor);
}


float
gevd_fold::NoiseThreshold(bool shortp) const
{
  float factor = (shortp? junctionFactor : contourFactor);
  float smooth = (shortp? smoothSigma/2 : smoothSigma);
  return factor * 3 *          // 3*sigma for 99% removal confidence
         NoiseResponseToFilter(noiseSigma, smooth, filterFactor);
}


float
gevd_fold::NoiseResponseToFilter(float noiseSigma,
                                 float smoothSigma,
                                 float filterFactor)
{
  return noiseSigma /          // white noise
         (float)vcl_pow((double)smoothSigma, 2.5) * // size of filter ddG
         ((float)vcl_sqrt(0.375 / vcl_sqrt(vnl_math::pi))) *
         filterFactor;        // factor in Hessian image
}


//: Output a snapshot of current control parameters
vcl_ostream& operator<< (vcl_ostream& os, const gevd_fold& st)
{
  os << "Fold:\n"
     << "   smoothSigma " << st.smoothSigma << vcl_endl
     << "   noiseSigma " << st.noiseSigma << vcl_endl
     << "   contourFactor " << st.contourFactor << vcl_endl
     << "   junctionFactor " << st.junctionFactor << vcl_endl
     << "   filterFactor " << st.filterFactor << vcl_endl;
    return os;
}


//: Output a snapshot of current control parameters
vcl_ostream& operator<< (vcl_ostream& os, gevd_fold& st)
{
  os << "Fold:\n"
     << "   smoothSigma " << st.smoothSigma << vcl_endl
     << "   noiseSigma " << st.noiseSigma << vcl_endl
     << "   contourFactor " << st.contourFactor << vcl_endl
     << "   junctionFactor " << st.junctionFactor << vcl_endl
     << "   filterFactor " << st.filterFactor << vcl_endl;
    return os;
}
