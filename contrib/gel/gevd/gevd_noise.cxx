// This is gel/gevd/gevd_noise.cxx
#include "gevd_noise.h"
//:
// \file

//#include <ImageProcessing/FloatOperators.h>
//#include <ImageProcessing/pixel.h>

//#include <cool/Timer.h>
//#include <iostream.h>
#include <vcl_cmath.h> // for vcl_fabs()

#include "gevd_pixel.h"
#include "gevd_float_operators.h"


const int INVALID = -1;
const int KRADIUS = 10; // smooth raw histogram


//: Generate the histogram curve at low responses to estimate the sensor/texture noise in data.

gevd_noise::gevd_noise(const float* data, const int n, // data in typical region
                       const int number_of_bins) // granularity of histogram
  : hist(new float[number_of_bins]),
    nbin(number_of_bins)
{
  // 1. Find the mean value, to estimate the range
  float range = 0; int i;
  for (i = 0; i < n; i++)
    range += data[i];
  range /= n;                   // sensor < texture < mean
  binsize = range/(nbin-1);     // first guess value
#ifdef TRACE_DEBUG
  vcl_cout << "Find mean of h(x) at: " << range << vcl_endl;
#endif

  // 2. Search for visible peak in histogram
  for (;;) {                    // search for range of low responses
    for (i = 0; i < nbin; i++)  // clear hist curve
      hist[i] = 0;
    for (i = 0; i < n; i++) {   // create histogram with initial
      float strength = data[i]; // binsize and range
      if (strength < range)
        hist[int(strength/binsize)] += 1;
    }
    float peakh = 0; int peaki = INVALID; // find main peak of histogram
    for (i = 0; i < nbin; i++)
      if (hist[i] > peakh) {
        peakh = hist[i];
        peaki = i;
      }
#ifdef TRACE_DEBUG
    vcl_cout << "Find peak of h(x) at: " << peaki*binsize
             << "  " << peakh << vcl_endl;
#endif
    if (peaki < 10) {           // narrow range a whole lot
      range /= 10.0, binsize /= 10.0;
    } else if (peaki >= nbin-5) { // expand range a little
      range *= 1.5, binsize *= 1.5;
    } else {
      range = peaki*binsize*2;  // put peak near center of histogram
      binsize = range/(nbin-1);
      break;                    // range of histogram found
    }
  }

  // 3. Find smooth histogram around noise response
  for (i = 0; i < nbin; i++)    // clear hist curve
    hist[i] = 0;
  for (i = 0; i < n; i++) {     // create histogram with final
    float strength = data[i];   // binsize and range
    if (strength < range)
      hist[int(strength/binsize)] += 1;
  }
  gevd_float_operators::RunningSum(hist, hist, nbin, KRADIUS); // smooth by default
#ifdef TRACE_DEBUG
  for (i = 0; i < nbin; i++)    // points of smoothed h(x)
    vcl_cout << hist[i] << ' ';
  vcl_cout << vcl_endl << vcl_endl;
#endif
}

//: Free allocated space.

gevd_noise::~gevd_noise()
{
  delete [] hist;
}

//: Collect all edgels above zero, in an ROI at center of image.
// This utility function is used to collect edgels from a
// response image, i.e. gradient/hessian/laplacian, or other
// edge response image.

float*
gevd_noise::EdgelsInCenteredROI(const gevd_bufferxy& magnitude,
                                const gevd_bufferxy& dirx, const gevd_bufferxy& diry,
                                int& nedgel, const int
#ifdef MINROI
                                roiArea
#endif
                               )
{
  nedgel = 0;
#ifdef MINROI
  int area = magnitude.GetSizeX() * magnitude.GetSizeY();
  if (area < roiArea) {
    vcl_cerr << "Image is smaller than minimum ROI\n";
    return NULL;
  }
  float k = vcl_sqrt(float(roiArea) / area); // reduction factor
#else
  float k = 1.0;
#endif
  const int sx = int(k*magnitude.GetSizeX()), sy = int(k*magnitude.GetSizeY());
  float* edgels = new float [sx*sy];
  const int xmin = (magnitude.GetSizeX() - sx) / 2;
  const int xmax = xmin + sx;
  const int ymin = (magnitude.GetSizeY() - sy) / 2;
  const int ymax = ymin + sy;
  int i, j; float strength, x_s, s_x, dx, dy, r;
  for (j = ymin; j < ymax; j++)
    for (i = xmin; i < xmax; i++) {
      strength = floatPixel(magnitude, i, j);
      if (strength) {
        dx = floatPixel(dirx, i, j);
        dy = floatPixel(diry, i, j);
        if (dy < 0) dx = -dx, dy = -dy; // modulo PI
        if (dx > 0) {           // which octant?
          if (dx > dy) {        // 0-45 degree
            r = dy / dx;
            x_s = (r*floatPixel(magnitude, i-1, j-1) +
                   (1-r)*floatPixel(magnitude, i-1, j));
            s_x = (r*floatPixel(magnitude, i+1, j+1) +
                   (1-r)*floatPixel(magnitude, i+1, j));
          } else {              // 45-90 degree
            r = dx / dy;
            x_s = (r*floatPixel(magnitude, i-1, j-1) +
                   (1-r)*floatPixel(magnitude, i, j-1));
            s_x = (r*floatPixel(magnitude, i+1, j+1) +
                   (1-r)*floatPixel(magnitude, i, j+1));
          }
        } else {
          dx = -dx;             // absolute value
          if (dy > dx) {        // 90-135 degree
            r = dx / dy;
            x_s = (r*floatPixel(magnitude, i-1, j+1) +
                   (1-r)*floatPixel(magnitude, i, j+1));
            s_x = (r*floatPixel(magnitude, i+1, j-1) +
                   (1-r)*floatPixel(magnitude, i, j-1));
          } else {              // 135-180 degree
            r = dy / dx;
            x_s = (r*floatPixel(magnitude, i+1, j-1) +
                   (1-r)*floatPixel(magnitude, i+1, j));
            s_x = (r*floatPixel(magnitude, i-1, j+1) +
                   (1-r)*floatPixel(magnitude, i-1, j));
          }
        }
        if (x_s < strength && strength > s_x)  // strict local maximum
          edgels[nedgel++] = strength;
      }
    }
  return edgels;
}

//:
// Fit a Raleigh distribution to the histogram curve of
// edgels with low magnitudes, h(x), to estimate the sensor noise,
// as would be zero-crossing of dh(x), and texture noise as the dominant
// peak in h(x). Setting the threshold at 3 times the sensor/texture
// noise would eliminate 99% of all noisy edges.
// The raw noise in the original image can be deduced from the filter
// used to convolve with the image.
// H. Voorhees & T. Poggio, Detecting Blobs as Textons in Natural Images,
// Proc. 1987 IU Workshop, Los Angeles, CA.

bool
gevd_noise::EstimateSensorTexture(float& sensor, float& texture) const
{
  // 1. Compute derivative of histogram, dh(x)
  float* dhist = new float[nbin];
#ifdef TRACE_DEBUG
  float mag = gevd_float_operators::Slope(hist, dhist, nbin);
  mag *= gevd_float_operators::RunningSum(dhist, dhist, nbin, KRADIUS);
  for (int i = 0; i < nbin; i++)        // points of smoothed dh(x)
    vcl_cout << dhist[i]/mag << ' ';
  vcl_cout << vcl_endl << vcl_endl;
#endif

  // 2. Estimate sensor from first downward slope of dh(x)
  if (!WouldBeZeroCrossing(dhist, nbin, sensor)) {
    vcl_cerr << "Can not estimate sensor\n";
    return false;
  }
  sensor *= binsize;

  // 3. Find texture as zero-crossing of dh(x)
  if (!RealZeroCrossing(dhist, nbin, texture)) {
    vcl_cerr << "Can not estimate texture\n";
    return false;
  }
  texture *= binsize;

  // 4. Check for consistency
  if (sensor == 0 ||    // not found
      sensor > texture) // or inconsistency
    sensor = texture;
  delete [] dhist;
  return true;
}


//: Find would be zero-crossing of the derivative of the histogram from its downward curvature.
// This is sensor noise in the ROI.  Protected.

bool
gevd_noise::WouldBeZeroCrossing(const float* dhist, const int nbin,
                                float& index)
{
  int imax=INVALID, i1=INVALID, i2=INVALID, i;// x-coord of downward slope
  float maxdh = 0, dh1 = 0, dh2 = 0; // y-coord
  for (i = 0; i < nbin; i++) {
    if (dhist[i] > maxdh) {     // going upward to max
      maxdh = dhist[i];
      imax = i;                 // peak in dh(x)
    }
  }
  if (imax == INVALID)          // can not find maximum in dh(x)
    return false;
#ifdef TRACE_DEBUG
  vcl_cout << "Find max of dh(x) at: " << imax << vcl_endl;
#endif
  for (i = imax; i < nbin; i++) // going forward to min
    if (dhist[i] < 0.5*maxdh) {
      dh2 = dhist[i];           // second point on downward slope
      i2 = i;
      break;
    }
  if (i2 == INVALID)            // range of histogram is too small
    return false;
#ifdef TRACE_DEBUG
  vcl_cout << "Find end of downward dh(x) at: " << i2 << ", " << dh2 << vcl_endl;
#endif
  for (i = i2; i > 0; i--)              // going backward to max
    if (dhist[i] > 0.9*maxdh) {
      dh1 = dhist[i];           // first point on downward slope
      i1 = i;
      break;
    }
  if (i1 == INVALID)            // can not first pt of downward slope
    return false;
#ifdef TRACE_DEBUG
  vcl_cout << "Find start of downward dh(x) at: " << i1 << ", " << dh1 << vcl_endl;
#endif
  if (i1 >= i2 || dh1 <= dh2)   // downward slope is too short
    index = 0;
  else                          // from fitting the downward slope, find
    index = i2 + (i2-i1)*dh2/(dh1-dh2); // would-be zc
#ifdef TRACE_DEBUG
  vcl_cout << "Find would be zero-crossing dh(x) at: " << index << vcl_endl;
#endif
  return true;
}


//: Find real zero-crossing of the derivative of the histogram.
// This is texture noise in the ROI.  Protected.

bool
gevd_noise::RealZeroCrossing(const float* dhist, const int nbin,
                             float& index)
{
  int i3=INVALID, imin=INVALID, i;// x-coord right of zero-crossing
  float dh3 = 0, mindh = 0;             // y-coord
  for (i = 0; i < nbin; i++) {
    if (dhist[i] < mindh) {     // going downward to min
      mindh = dhist[i];
      imin = i;                 // bottom in dh(x)
    }
  }
  if (imin == INVALID)          // can not find minimum in dh(x)
    return false;
#ifdef TRACE_DEBUG
  vcl_cout << "Find min of dh(x) at: " << imin << vcl_endl;
#endif
  for (i = imin; i > 0; i--)    // going backward from minimum
    if (dhist[i] >= 0) {
      dh3 = dhist[i];           // second point on downward slope
      i3 = i;
      break;
    }
  if (i3 == INVALID)            // range of histogram is too small
    return false;
  index = (float)i3;
  if (dh3 > 0) {                // interpolate zero-crossing
    int i4 = i3+1;
    float dh4 = (float)vcl_fabs(dhist[i4]);
    index = (i3*dh4 + i4*dh3) / (dh3 + dh4);
  }
#ifdef TRACE_DEBUG
  vcl_cout << "Find real zero-crossing dh(x) at: " << index << vcl_endl;
#endif
  return true;
}
