// This is gel/gevd/gevd_step.cxx
#include "gevd_step.h"
//:
// \file
// Use 8 directions, with 45 degree angle in between them.
// The array DIS gives the i-component of the directions
// The array DJS gives the j-component of the directions.
// Shown here for a right-handed coordinate system for (i,j)
// in accordance with standard texts. -- JLM
//\verbatim
//               j
//         ______|_____________
//        |      |      |      |
//        |  3   |  2   |  1 <------ Direction Code
//        |______|______|______|     for 8-connected
//        |      |      |      |     neighbors
//        |  4   |(i,j) |  0   |
//        |______|______|______|___ i
//        |      |      |      |
//        |  5   |  6   |  7   |
//        |______|______|______|
//
//\endverbatim

#include <vcl_vector.h>
#include <vnl/vnl_math.h>
#include <gevd/gevd_noise.h>
#include <gevd/gevd_float_operators.h>
#include <gevd/gevd_pixel.h>
#ifdef DEBUG
# include <vul/vul_timer.h>
#endif

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

//: Save parameters and create workspace for detecting step profiles.
// High frequency features are smoothed away by smooth_sigma.
// Texture or white noise less than noise_sigma are not detected.
// smooth_sigma = 0.5-2.0, 1.0 insures separation of independent steps >= 2.
//
// If noise_sigma > 0, noise_sigma = is interpreted as the standard deviation
//    of intensity z(x,y) in a uniform region.
// If -1 <= noise_sigma <= 0, then noise_sigma is interpreted as a factor -k,
// where k is a weighting factor between two estimates: sensorNoise and textureNoise.
//
//  noiseSigma = ((1-k)*sensorNoise + k*textureNoise)/K
//
// The values of sensorNoise and textureNoise are computed from the shape of
// the histogram of gradient magnitude values, in the neighborhood of low gradients.
// The value of K is computed in NoiseResponseToFilter and defined by:
//
//  1/(smoothSigma^1.5)*(.5/M_PI^.25)*filterFactor.
//
// Given the defaults:  smoothSigma =1 and filterFactor = 2, K = .75
//
// The threshold used to delete weak edges is modified depending on the
// context.  For edgels along a contour, shortp = false, and the threshold
// is given by:
//
//    NoiseThreshold(shortp) = 3*K*noiseSigma
//
// For edgels at junctions, shortp = true, the threshold is computed
// using different factors.  The difference is controlled by the parameters:
// contour_factor and junction_factor.  smoothSigma when extending the
// boundary at junctions is 1/2 the value along contours.
//
gevd_step::gevd_step(float smooth_sigma, // width of filter dG
                     float noise_sigma,   // sensor/texture intensity noise -[0 1]
                     float contour_factor, float junction_factor)
  :smoothSigma(smooth_sigma), noiseSigma(noise_sigma),
   contourFactor(contour_factor), junctionFactor(junction_factor),
   filterFactor(2)              // factor from gevd_float_operators::Gradient
{
  if (smoothSigma < 0.5)        // no guarrantee for 2-pixel separation
    vcl_cerr << "gevd_step::gevd_step -- too small smooth_sigma: "
             << smoothSigma << vcl_endl;
  if (smoothSigma > 3)          // smooth out too much the junctions
    vcl_cerr << "gevd_step::gevd_step -- too large smooth_sigma: "
             << smoothSigma << vcl_endl;
  if (noiseSigma < -1) {
    vcl_cerr << "gevd_step::gevd_step -- noiseSigma out of range -[0 1]: "
             << noiseSigma << ". Reset to -1.\n";
    noiseSigma = -1;
  }

  //vcl_cout << "Init Step\n" << *this << vcl_endl;
}


//: Free space allocated for detecting step profiles.  Does nothing.
gevd_step::~gevd_step() {}

//: Detect step profiles with Canny edge detector.
// The image is convolved with a Gaussian to smooth away
// high frequency noise, and insure separation of step responses.
// Then local gradient magnitude and direction is detected
// using first difference [-1 0 +1].
// Optionally estimate sensor/texture sigma and set threshold.
// Finally, non maximum suppression is done to find strict
// local maxima of slope.
// The Canny edge detector finds elongated contours only.
// These contours are typically broken at junctions because non
// maximum suppression is done along only the strongest direction.
// Return contour (float), direction (byte), location (float) images.
// Return true if no exception.
// J. Canny, A Computational Approach to Edge Detection,
// IEEE Trans on PAMI, vol 8, no 6, Nov 1986.
bool
gevd_step::DetectEdgels(const gevd_bufferxy& image,
                        gevd_bufferxy*& contour, gevd_bufferxy*& direction,
                        gevd_bufferxy*& locationx, gevd_bufferxy*& locationy,
                        gevd_bufferxy*& grad_mag, gevd_bufferxy*& angle)
{
  //vcl_cout << "*** Detect step profiles with first-derivative of Gaussian"
  //         << *this
  //         << vcl_endl;
  if (image.GetBitsPixel() != bits_per_float) {
    vcl_cerr << "gevd_step::DetectEdgels requires float image\n";
    return false;
  }

  // -tpk @@ missing check if the requested buffer size is too small to contain the convolution operations

  // 1. Smooth image to regularize data, before taking derivatives
  gevd_bufferxy* smooth = NULL;      // Gaussian smoothed image
  // use float to avoid overflow/truncation
  filterFactor = gevd_float_operators::Gaussian((gevd_bufferxy&)image, // well-condition before
                                                smooth, smoothSigma); // 1st-difference

  // 2. Use 1st-difference to estimate local slope, filter is dG.
  gevd_bufferxy *slope = NULL, *dirx=NULL, *diry=NULL;
  filterFactor *= gevd_float_operators::Gradient(*smooth, // directional 1st-difference
                                                 slope, dirx, diry); // mult factor returned
  delete smooth;

  // 2.5 JLM - Fill the theta array for use in outputting continuous digital curve
  //           directions later.  The angle definition here is consistent with
  //           EdgeDetector, i.e. angle = (180/M_PI)*vcl_atan2(dI/dy, dI/dx);

  grad_mag = gevd_float_operators::SimilarBuffer(image);
  angle = gevd_float_operators::SimilarBuffer(image);
  const double kdeg = 180*vnl_math::one_over_pi;
  for (int j = 0; j < image.GetSizeY(); j++)
    for (int i = 0; i < image.GetSizeX(); i++)
      if ((floatPixel(*grad_mag, i, j) = floatPixel(*slope, i, j)))
        floatPixel(*angle, i, j) = float(kdeg*vcl_atan2(floatPixel(*diry, i, j),
                                                        floatPixel(*dirx, i, j)));
      else
        floatPixel(*angle, i, j) = 0;


  // 3. Estimate sensor/texture sigmas from histogram of weak step edgels
  if (noiseSigma <= 0)  {
    int nedgel = 0;             // all edgels in ROI at center of image
    float* edgels = gevd_noise::EdgelsInCenteredROI(*slope, *dirx, *diry,
                                                    nedgel);
    if ((edgels) && (nedgel > 0 )) {
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

  gevd_float_operators::NonMaximumSuppression(*slope, *dirx, *diry,
                                              NoiseThreshold(), // above noise
                                              contour, direction,
                                              locationx, locationy);
  delete slope; delete dirx; delete diry;
  gevd_float_operators::FillFrameX(*contour, 0, FRAME); // erase pixels in frame border
  gevd_float_operators::FillFrameY(*contour, 0, FRAME);
  return true;
}

//:
// Return -/+ PI/2, to encode the existence of an end point
// on the left/right side of the current contour point i,j.
// Note, in vil1, images have a left-handed (i,j) coordinate frame,
// i.e., i increases left to right and j increases downward in the image.
//
// The search proceeds as follows:
//
// 1)
//\verbatim
//        X
//    --(i,j)--
//        X    |
//             v  Normal to contour (dir)
//\endverbatim
// if either X is occupied then it must be a corner -- so do nothing.
//
// 2) Next, search along the contour to the left or right
//\verbatim
//      L         R
//      L--(i,j)--R
//      L    |    R
//           v Normal to contour (dir)
//
//    left       right      if any R position is occupied but not any L
//  neighbor    neighbor    return -2.
//                          if any L position is occupied but not any R
//                          return +2.  Otherwise return 0.
//\endverbatim
//  Thus, the direction code along the contour at the end of the contour
//  is returned.
static int
LeftXorRightEnd(const gevd_bufferxy& contour,
                int i, int j, // pixel on contour
                const int dir) // normal to contour
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

//: Find best step extension from end point, which has largest local maximum slope.
// Search all 3x3=9 neighboring locations/orientations
// in the direction of the contour.  If the best gradient along the contour
// extension is below the threshold, then return 0, otherwise
// return the location, direction and strength of the extension pixel.
//
static float
BestStepExtension(const gevd_bufferxy& smooth,
                  const int i, const int j,
                  const int ndir, // tangential dir to neighbor
                  const float threshold,
                  int& best_i, int& best_j, // pixel
                  int& best_d, float& best_l) // direction + subloc
{
  float best_s = threshold;     // insure greater response
  const int direc = ndir + HALFPI; // step direction
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
      float slope = (float)vcl_fabs(pix_p - pix_m);
      float max_s = (dir%HALFPI)? best_s*(float)vcl_sqrt(2.0): best_s;
      if (slope > max_s) {      // find best strength
        int di2 = 2*di;
        int dj2 = 2*dj;
        if (slope > vcl_fabs(pix - floatPixel(smooth, ni-di2, nj-dj2)) &&
            slope > vcl_fabs(pix - floatPixel(smooth, ni+di2, nj+dj2))) {
          best_i = ni;
          best_j = nj;
          best_s = (dir%HALFPI)? slope/(float)vcl_sqrt(2.0) : slope;
          best_d = dir%FULLPI + TWOPI; // in range [0 FULLPI) + TWOPI
        }
      }
    }
  }
  if (best_s > threshold) {     // interpolate with parabola
    float pix = floatPixel(smooth, best_i, best_j);
    int di2 = 2 * DIS[best_d], dj2 = 2 * DJS[best_d];
    float s_m = (float)vcl_fabs(pix - floatPixel(smooth, best_i-di2, best_j-dj2));
    float s_p = (float)vcl_fabs(pix - floatPixel(smooth, best_i+di2, best_j+dj2));
    if (best_d%HALFPI) {
      s_m /= (float)vcl_sqrt(2.0);
      s_p /= (float)vcl_sqrt(2.0);
    }
    best_l = gevd_float_operators::InterpolateParabola(s_m, best_s, s_p, best_s);
    return best_s;
  } else                        // not found
    return 0;
}


//:
// Find junctions by searching for extensions of contours from
// their dangling end points. Non maximum suppression insures that
// contours have width < 2, and so we can find the left/right neighbors,
// and deduce end points. By using a minimally smoothed image,
// we find step profiles up to joining with a stronger contour, thus
// recovering the missing junction caused by NMS along only 1 direction.
// The junctions are returned but are not set in the contour image,
// to prevent incorrect tracing of stronger contours first.
// The search is extended outward for a distance of kmax which is currently
// 4*smoothSigma + 2.
int
gevd_step::RecoverJunctions(const gevd_bufferxy& image,
                            gevd_bufferxy& contour, gevd_bufferxy& direction,
                            gevd_bufferxy& locationx, gevd_bufferxy& locationy,
                            int*& junctionx, int*& junctiony)
{
#if defined(DEBUG)
  vul_timer t;
#endif
  if (image.GetBitsPixel() != bits_per_float) {
    vcl_cerr << "gevd_step::RecoverJunction requires float image\n";
    return false;
  }
  const int rmax = 1+FRAME;     // 1 + kernel radius of BestStepExtension
  const int kmax = int(4 * smoothSigma + 0.5) + 2; // gap = 2
  const int xmax = image.GetSizeX()-rmax-1; // fill step direction
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
  if (!length) return 0;        // no end points exist

  // 2. Extend from end points until they touch other contours
  gevd_bufferxy* smooth = NULL;
  gevd_float_operators::Gaussian((gevd_bufferxy&)image, smooth, smoothSigma/2); // avoid oversmoothing
  const bool shortp = true;     // short contours
  const float threshold = NoiseThreshold(shortp);
  float slope, loc;
  int njunction = 0;            // number of junctions found
  for (int r = 1; r <= kmax; r++) { // breadth-first extension
    int ntouch = 0, nextension = 0;
    for (int i = 0; i < length; i++)
      if ((xdir = ndir[i]) != 0 && xdir != TWOPI) // still extension?
      {
        int x = xloc[i], y = yloc[i];
        int dir = bytePixel(direction, x, y); // direction of step
        slope = BestStepExtension(*smooth,
                                  x, y, dir + xdir,     // current end pt
                                  threshold,
                                  x, y, dir, loc);
        if (slope) {                 // next end point
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
            floatPixel(contour, x, y) = slope; // still disconnected
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


//:
// Return the standard deviation of raw noise, in the original image,
// either estimated or given by the user. If the noise has not been
// estimated, return 0.
float
gevd_step::NoiseSigma() const
{
  return (noiseSigma <= 0)? 0: noiseSigma;
}


//:
// Compute response of white noise through the filter dG, or
// second-derivative of the Gaussian. Using a threshold of 3 times
// this noise response would eliminate 99% of the noise edges.
float
gevd_step::NoiseResponse() const
{
  return NoiseResponseToFilter(noiseSigma,
                               smoothSigma, filterFactor);
}


//:
// Return threshold for detecting contour or junction,
// which is response of white gaussian noise, noise_sigma,
// to step edge detector, i.e. first-order derivative of Gaussian,
// smooth_sigma.
// noise_sigma can be estimated by finding the standard deviation
// in a region of constant intensity, and no texture patterns.
// Use short_factor*noise_sigma and smooth_sigma/2, when detecting
// junctions, to account for multiple responses to step edge detector.
float
gevd_step::NoiseThreshold(bool shortp) const
{
  float factor = (shortp? junctionFactor : contourFactor);
  float smooth = (shortp? smoothSigma/2 : smoothSigma);
  return factor * 3 *          // 3*sigma for 99% removal confidence
         NoiseResponseToFilter(noiseSigma, smooth, filterFactor);
}


//:
// Compute response of white noise through the filter dG, or
// first-derivative of the Gaussian. Using a threshold of 3 times
// this noise response would eliminate 99% of the noise edges.
float
gevd_step::NoiseResponseToFilter(const float noiseSigma,
                                 const float smoothSigma,
                                 const float filterFactor)
{
  return noiseSigma /          // white noise
         (float)vcl_pow((double)smoothSigma, 1.5) * // size of filter dG
         (0.5f / (float)vcl_pow(vnl_math::pi, 0.25)) *
         filterFactor;        // multiplication factor
}


//: Output a snapshot of current control parameters
vcl_ostream& operator<< (vcl_ostream& os, const gevd_step& st)
{
  os << "Step:\n"
     << "   smoothSigma " << st.smoothSigma << vcl_endl
     << "   noiseSigma " << st.noiseSigma << vcl_endl
     << "   contourFactor " << st.contourFactor << vcl_endl
     << "   junctionFactor " << st.junctionFactor << vcl_endl
     << "   filterFactor " << st.filterFactor << vcl_endl;
    return os;
}


//: Output a snapshot of current control parameters
vcl_ostream& operator<< (vcl_ostream& os, gevd_step& st)
{
  os << "Step:\n"
     << "   smoothSigma " << st.smoothSigma << vcl_endl
     << "   noiseSigma " << st.noiseSigma << vcl_endl
     << "   contourFactor " << st.contourFactor << vcl_endl
     << "   junctionFactor " << st.junctionFactor << vcl_endl
     << "   filterFactor " << st.filterFactor << vcl_endl;
    return os;
}
