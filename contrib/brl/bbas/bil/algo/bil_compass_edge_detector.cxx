// This is brl/bbas/bil/algo/bil_compass_edge_detector.cxx
#include <vector>
#include <iostream>
#include <cmath>
#include "bil_compass_edge_detector.h"
//:
// \file

#include <bil/algo/bil_nms.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h> // for vnl_math::pi
#include <vil/vil_convert.h>

#define MAX_VAL 255.0

// Integrate a circle of radius r centered at the origin from
// Xlow to Xhigh in Quadrant I.
// Since this function is only interested in square pixels, the Y value
// is included so that the area below Y is ignored.
double CArea(double Xhigh, double Xlow, double Y, double r)
{
  return 0.5 * ((Xhigh * std::sqrt(r*r - Xhigh * Xhigh) + r*r * std::asin(Xhigh / r)) -
                (Xlow  * std::sqrt(r*r - Xlow * Xlow)   + r*r * std::asin(Xlow  / r))   )
         - Y * (Xhigh - Xlow);
}

//: Compute the weighted masks for each wedge.
// Note: r is the radius of the circle, and n is the number of wedges.
// The result is an array of length ceil(r) * ceil(r) * n where the
// entries state how much area of each wedge of radius r (1/n of the quarter-circle)
// is inside each pixel.
//
// The pixels are in Quadrant I, listed in column-major order
// (just as we would get by calling a MATLAB routine).
// This is admittedly a mess, as there are at least 27 different cases for a
// circle and a radial line and a square to interact.
//
// This function has been reproduced exactly from Ruzon's code.
// I didn't wanna waste my time attempting to replicate it.
double* MakeQtrMask(double r, int n_wedges)
{
  // size of the quadrant
  int R = (int) std::ceil(r);

  // allocate the array to hold the mask
  auto* mask = new double[R*R*n_wedges];

  // initialize
  for (int i=0; i<R*R*n_wedges; ++i)
    mask[i] = 0.0;

  int ULC=0, URL=0, URH=0, LRC=0, LLL=0, LLH=0;
  int InCircle, NoLine, LowLine, HighLine, LowIntersect, HighIntersect;
  double mlow, mhigh, lowangle, highangle;
  double CA, BA, AA, LA, BXC=0, BXL, BXH, TXC=0, TXL, TXH, LYC, LYL=0, LYH;
  double RYC, RYL=0, RYH, AAC=0, BAC=0, AAN=0, BAN=0, XLC, YLC, XHC, YHC;

  // Iterate over the lower left hand corner of each pixel
  for (int x = 0; x <= R - 1; ++x) {
    for (int y = R - 1; y >= 0; --y) {
      /* Start by computing the pixel's area in the circle */
      if (x * x + y * y >= r * r) /* Pixel entirely outside circle */
        continue;

      if ((x+1) * (x+1) + (y+1) * (y+1) <= r * r) { /* Pixel entirely inside */
        CA = 1.0;
        InCircle = 1;
      }
      else { /* Tricky part; circle intersects pixel */
        ULC = x * x + (y+1) * (y+1) <= r * r;
        LRC = (x+1) * (x+1) + y * y <= r * r;
        BXC = std::sqrt(r * r - y * y);
        TXC = std::sqrt(r * r - (y+1) * (y+1));
        if (!ULC && !LRC)
          CA = CArea(BXC, x, y, r);
        else if (ULC && !LRC)
          CA = CArea(BXC, TXC, y, r) + TXC - x;
        else if (!ULC && LRC)
          CA = CArea(x + 1, x, y, r);
        else /* if (ULC && LRC) */
          CA = CArea(x + 1, TXC, y, r) + TXC - x;
        InCircle = 0;  /* Therefore, it must be on the border */
      }

      // Check through each wedge
      for (int i = 0; i < n_wedges; ++i) {
        /* Compute area above lower radial line of wedge */
        lowangle = i * vnl_math::pi / (2 * n_wedges);
        mlow = std::tan(lowangle);
        TXL = (y+1)/mlow;
        BXL = y/mlow;
        if (TXL <= x)
          AA = 0.0;
        else if (i == 0 || BXL >= x+1)
          AA = 1.0;
        else {
          LLL = BXL > x;
          URL = TXL > x+1;
          LYL = mlow * x;
          RYL = mlow * (x + 1);
          if (LLL && URL)
            AA = 1 - 0.5 * (RYL-y) * (x+1-BXL);
          else if (!LLL && URL)
            AA = 0.5 * (2*(y+1)-LYL-RYL);
          else if (LLL && !URL)
            AA = 0.5 * (TXL+BXL-2*x);
          else
            AA = 0.5 * (y+1-LYL) * (TXL-x);
        }
        LowLine = AA < 1.0 && AA > 0.0;

        /* Compute area below upper radial line of wedge */
        /* The cases are reversed from the lower line cases */
        highangle = (i+1) * vnl_math::pi / (2 * n_wedges);
        mhigh = std::tan(highangle);
        TXH = (y+1)/mhigh;
        BXH = y/mhigh;
        RYH = mhigh*(x+1);
        LYH = mhigh*x;
        if (i == n_wedges-1 || TXH <= x)
          BA = 1.0;
        else if (BXH >= x+1)
          BA = 0.0;
        else {
          LLH = BXH < x;
          URH = TXH < x+1;
          if (LLH && URH)
            BA = 1 - 0.5 * (y+1-LYH) * (TXH-x);
          else if (!LLH && URH)
            BA = 1 - 0.5 * (BXH+TXH-2*x);
          else if (LLH && !URH)
            BA = 0.5 * (LYH+RYH-2*y);
          else /* if (!LLH && !URH) */
            BA = 0.5 * (RYH-y) * (x+1-BXH);
        }
        HighLine = BA < 1.0 && BA > 0.0;
        LA = BA + AA - 1.0;
        if (LA == 0.0) /* Pixel not in wedge */
          continue;
        NoLine = LA == 1.0;

        /* Finish the cases we know about so far */
        if (InCircle) {
          mask[i * R*R + x * R + R - 1 - y] = LA;
          continue;
        }
        else if (NoLine) {
          mask[i * R*R + x * R + R - 1 - y] = CA;
          continue;
        }

        /* We can now assert (~InCircle && (HighLine || LowLine)) */
        /* But this does not ensure the circular arc intersects the line */
        LYC = std::sqrt(r * r - x * x);
        RYC = std::sqrt(r * r - (x+1) * (x+1));
        LowIntersect = LowLine &&
          ((!ULC && !LRC && ((LLL && BXL < BXC) || (!LLL && LYL < LYC))) ||
          (!ULC && LRC) || (ULC && !LRC) ||
          (ULC && LRC && ((!URL && TXL >= TXC) || (URL && RYL >= RYC))));

        HighIntersect = HighLine &&
          ((!ULC && !LRC && ((!LLH && BXH < BXC) || (LLH && LYH < LYC))) ||
          (!ULC && LRC) || (ULC && !LRC) ||
          (ULC && LRC && ((URH && TXH >= TXC) || (!URH && RYH >= RYC))));

        /* Recompute BA and AA (now BAC and AAC) given the intersection */
        if (LowIntersect) {
          XLC = std::cos(lowangle) * r;
          YLC = std::sin(lowangle) * r;
          if (!LRC && LLL)
            AAC = CA - 0.5 * (XLC - BXL) * (YLC - y) - CArea(BXC, XLC, y, r);
          else if (!LRC && !LLL)
            AAC = CA - 0.5 * (XLC - x) * (YLC + LYL - 2 * y) -
                  CArea(BXC, XLC, y, r);
          else if (LRC && LLL)
            AAC = CArea(XLC, x, y, r) - 0.5 * (YLC - y) * (XLC - BXL);
          else /* if (LRC && !LLL) */
            AAC = CA - CArea(x+1, XLC, y, r) -
                  0.5 * (YLC + LYL - 2 * y) * (XLC - x);
        }

        if (HighIntersect) {
          XHC = std::cos(highangle) * r;
          YHC = std::sin(highangle) * r;
          if (!LRC && !LLH)
            BAC = 0.5 * (XHC - BXH) * (YHC - y) + CArea(BXC, XHC, y, r);
          else if (!LRC && LLH)
            BAC = 0.5 * (XHC - x) * (YHC + LYH - 2 * y) +
                  CArea(BXC, XHC, y, r);
          else if (LRC && LLH)
            BAC = CArea(x+1, XHC, y, r) +
                  0.5 * (YHC + LYH - 2 * y) * (XHC - x);
          else /* if (LRC && !LLH) */
            BAC = CArea(x+1, XHC, y, r) + 0.5 * (YHC - y) * (XHC - BXH);
        }

        /* Compute area for a few more cases */
        if (LowIntersect && !HighLine) {
          mask[i * R*R + x * R + R - 1 - y] = AAC;
          continue;
        }
        else if (HighIntersect && !LowLine) {
          mask[i * R*R + x * R + R - 1 - y] = BAC;
          continue;
        }
        else if (HighIntersect && LowIntersect) {
          mask[i * R*R + x * R + R - 1 - y] = AAC + BAC - CA;
          continue;
        }

        // Here we can assert
        // ~InCircle && (HighLine || LowLine) && !LowIntersect && !HighIntersect
        // There are still many possible answers.  Start by computing BAN and AAN
        // (N for No Intersection).
        if (LowLine && !LowIntersect) {
          if (!ULC && !LLL)
            AAN = 0;
          else if (!LRC && LLL)
            AAN = CA;
          else if (LRC && URL && LLL)
            AAN = CA - 0.5 * (RYL - y) * (x+1 - BXL);
          else if (ULC && URL && !LLL)
            AAN = CA - 0.5 * (RYL + LYL - 2 * y);
          else /* if (ULC && !URL) */
            AAN = AA;
        }

        if (HighLine && !HighIntersect) {
          if (!ULC && LLH)
            BAN = CA;
          else if (!LRC && !LLH)
            BAN = 0;
          else if (LRC && !URH && !LLH)
            BAN = BA;
          else if (ULC && !URH && LLH)
            BAN = 0.5 * (RYL + LYL - 2 * y);
          else if (ULC && URH)
            BAN = CA + BA - 1;
        }

        if (LowLine && !LowIntersect && HighLine && !HighIntersect) {
          mask[i * R*R + x * R + R - 1 - y] = AAN + BAN - CA;
          continue;
        }
        else if (LowIntersect && HighLine && !HighIntersect) {
          mask[i * R*R + x * R + R - 1 - y] = AAC + BAN - CA;
          continue;
        }
        else if (LowLine && !LowIntersect && HighIntersect) {
          mask[i * R*R + x * R + R - 1 - y] = AAN + BAC - CA;
          continue;
        }
        else if (LowLine && !LowIntersect) {
          mask[i * R*R + x * R + R - 1 - y] = AAN;
          continue;
        }
        else if (HighLine && !HighIntersect) {
          mask[i * R*R + x * R + R - 1 - y] = BAN;
          continue;
        }
        else {
#ifdef DEBUG
           std::cout << "Big nasty horrible bug just happened\n";
#endif // DEBUG
          mask[i * R*R + x * R + R - 1 - y] = 0.0;
        }
      }
    }
  }

  // finally the masks are ready
  return mask;
}

//: This routine creates the mask that is one-quarter of the size of the circle and \p n_wedges deep.
// It also creates a mask sum over all the wedges and computes the number of
// pixels with non-zero weights.
//
double CreateMask(double sigma, int n_wedges, int masksz, int weight_type, double **mask, double **sum)
{
  // Create a normalized Gaussian mask in Quadrant I
  // Note: this mask is in column order
  auto* gauss = new double[masksz*masksz];
  for (int i = 0; i < masksz; ++i) {
    for (int j = 0; j < masksz; ++j)
    {
      double r = std::sqrt((masksz-i-0.5) * (masksz-i-0.5) + (j+0.5) * (j+0.5));

      // Various pixel weighting masks
      if (weight_type==1)
        gauss[j*masksz+i] = r * std::exp(-(r*r)/(2*sigma*sigma));  // RAYLEIGH
      else if (weight_type==2)
        gauss[j*masksz+i] = std::exp(-(r*r)/(2*sigma*sigma)); // GAUSSIAN
      else
        gauss[j*masksz+i] = 1 - r / masksz;  // LINEAR
    }
  }

  // compute a quarter mask for a compass operator with the given parameters
  *mask = MakeQtrMask(sigma * 3.0, n_wedges);

  // also compute the mask sum over all the wedges
  *sum = new double[masksz*masksz];
  for (int i=0; i<masksz*masksz; ++i) // initialize it
    (*sum)[i] = 0.0;

  for (int i = 0; i < masksz*masksz*n_wedges; ++i) {
    if (weight_type>0) // weight the wedge masks by the weighting function
      (*mask)[i] *= gauss[i % (masksz*masksz)];

    // compute the sum
    (*sum)[i % (masksz*masksz)] += (*mask)[i];
  }

  // Also compute sum of weights over one wedge of the circle.
  double wedge_wt = 0.0;
  for (int i = 0; i < masksz*masksz; ++i)
    wedge_wt += (*sum)[i];
  wedge_wt /= n_wedges;

  return wedge_wt;
}

// This routine abstracts a little of the complexity of placing pixels and
// their weights into the histograms
void AddWeight(vil_image_view<vxl_byte>& image, double weight,
               int row, int col, bil_bin hist[])
{
  // read image intensity at this point
  vxl_byte intensity = image(col, row);

  // quantize this intensity value to the specified bins
  int min_index = (int) std::floor(intensity*(NBINS - 1)/MAX_VAL);

  // accumulate it at the correct bin
  hist[min_index].weight += weight;
  hist[min_index].wsum += weight*intensity;
  // hist[min_index].value is not computed
}

// create the histograms for each wedge from the image intensities
void CreateWedgeHistograms(vil_image_view<vxl_byte>& image, double *mask,
                           int r, int c,
                           int masksz, int n_wedges, bil_bin* hist)
{
  // initialize the histograms to zero
  for (int i=0; i<4*n_wedges*NBINS; ++i) {
    hist[i].value = 0.0;
    hist[i].weight = 0.0;
    hist[i].wsum = 0.0;
  }

  for (int k = 0; k < n_wedges; ++k) {
    for (int i = 0; i < masksz; ++i) {
      for (int j = 0; j < masksz; ++j)
      {
        int index = k*masksz*masksz+j*masksz+i;
        if (mask[index] > 0) {
          AddWeight(image, mask[index], r-masksz+i,   c+j,          hist + k*NBINS);
          AddWeight(image, mask[index], r-j-1,        c-masksz+i,   hist + (n_wedges+k)*NBINS);
          AddWeight(image, mask[index], r+masksz-i-1, c-j-1,        hist + (2*n_wedges+k)*NBINS);
          AddWeight(image, mask[index], r+j,          c+masksz-i-1, hist + (3*n_wedges+k)*NBINS);
        }
      }
    }
  }
}


//: compute the optimal orientation and edge strength from the vector of edge strengths computed at discrete orientations
void compute_strength_and_orient(const double* dist, int n_orient, float& strength, double& orientation)
{
  double wedgesize = vnl_math::pi/n_orient;

  // Compute Minimum and Maximum EMD values
  double maxEMD = 0.0, minEMD = 1.0;
  int strindex = 0;
  for (int i = 0; i < n_orient; ++i) {
    if (dist[i] > maxEMD) {
      maxEMD = dist[i];
      strindex = i;
    }
    if (dist[i] < minEMD) {
      minEMD = dist[i];
    }
  }

  // Fit a parabola to the edge strengths at different orientations to
  // determine the subpixel orientation and max edge strength.

  // The strength and orientation of an edge lie not at the
  // maximum EMD value but rather at the vertex of the parabola that runs
  // through the maximum and the two points on either side.

  // The first
  // computation of orientation assumes the maximum is the y-intercept.
  // After computing the strength, we adjust the orientation.

  double maxEMDori = strindex * wedgesize;

  double a = dist[strindex];
  double b = dist[(strindex+n_orient-1) % n_orient];
  double c = dist[(strindex+n_orient+1) % n_orient];

  double d = (b + c - 2 * a);
  if (std::fabs(d) > 1e-3) { // not degenerate
    double x = (wedgesize/2)*(b - c)/d;
    strength = float(a + x*(c - b)/(2*wedgesize) + x*x*d/(2*wedgesize*wedgesize));
    orientation = std::fmod(maxEMDori + x + vnl_math::twopi, vnl_math::pi);
  }
  else { // Uncertainty bounds
    strength = float(a);
    orientation = maxEMDori;
  }
}

#if 0 // function commented out
//: Compute intensity histogram gradient using the compass operator
// Note: # of orientations = 2*n_wedges
void bil_compute_compass_gradient( vil_image_view<vxl_byte>& image, int spacing, int n_wedges,
                                   int weight_type, double sigma, int dist_op, bool SG_filter,
                                   vil_image_view<float>& hist_grad,
                                   vil_image_view<double>& hist_ori)
{
  // convert to grayscale
  vil_image_view<vxl_byte> img;
  if (image.nplanes() == 3) {
    vil_convert_planes_to_grey(image, img );
  }
  else if (image.nplanes() == 1) {
    img = image;
  }

  // determine some relevant parameters
  int n_orient = 2*n_wedges; // number of orientations
  int masksz = (int) std::ceil(3 * sigma); // mask size

  // allocate space for histogram gradients at various orientations
  std::vector<vil_image_view<double> > hist_dist(n_orient);
  for (int i=0; i<n_orient; ++i) {
    hist_dist[i].set_size(img.ni(), img.nj());
    hist_dist[i].fill(0.0);
  }

  // create the wedge masks
  double *mask, *masksum;
  double wedge_wt = CreateMask(sigma, n_wedges, masksz, weight_type, &mask, &masksum);

  // allocate space for the histograms
  dbdet_bin* wHist = new dbdet_bin[4*n_wedges*NBINS];       // All wedge histograms
  dbdet_signature hist1, hist2;         // Semi-circle histograms
  dbdet_signature hist1norm, hist2norm; // normalized histograms

  // loop over all the pixels in the image
  for (unsigned x = masksz; x < img.ni()-masksz; x+= spacing) {
    for (unsigned y = masksz; y < img.nj()-masksz; y+=spacing)
    {
      // Create wedge histograms (output in hist)
      CreateWedgeHistograms(img, mask, y, x, masksz, n_wedges, wHist);

      // Compute initial histogram sums (i.e., or the first orientation)
      // Note: half of the wedges contribute towards one histogram
      //      and the rest contribute towards the other
      for (int i = 0; i < NBINS; ++i)
      {
        hist1.bins[i].weight = 0.0;
        hist1.bins[i].wsum = 0.0;
        for (int j = 0; j < 2*n_wedges; ++j) {
          hist1.bins[i].weight += wHist[j * NBINS + i].weight;
          hist1.bins[i].wsum   += wHist[j * NBINS + i].wsum;
        }

        hist2.bins[i].weight = 0.0;
        hist2.bins[i].wsum = 0.0;
        for (int j = 2*n_wedges; j < 4*n_wedges; ++j) {
          hist2.bins[i].weight += wHist[j * NBINS + i].weight;
          hist2.bins[i].wsum   += wHist[j * NBINS + i].wsum;
        }
      }

      // Loop over every orientation
      for (int i = 0; i < n_orient; ++i) {
        // Normalize the histograms
        for (int j = 0; j < NBINS; ++j) {
          hist1norm.bins[j].value  = hist1.bins[j].wsum / hist1.bins[j].weight;
          hist2norm.bins[j].value  = hist2.bins[j].wsum / hist2.bins[j].weight;

          // Normalize by the number of wedges
          hist1norm.bins[j].weight = hist1.bins[j].weight / (wedge_wt * n_orient);
          hist2norm.bins[j].weight = hist2.bins[j].weight / (wedge_wt * n_orient);
        }

        // compute distance between the normalized histograms
        double d = 0.0;
        if (dist_op==0)
          d = dbdet_chi_sq_dist(hist1norm.bins, hist2norm.bins); // compute chi^2 dist
        else if (dist_op==1)
          d = dbdet_bhat_dist(hist1norm.bins, hist2norm.bins);   // compute Bhattacharyya dist
        else if (dist_op==2)
          d = dbdet_gray_EMD(hist1norm.bins, hist2norm.bins);    // compute EMD

        // record this distance as the contrast between the two halves of the compass
        hist_dist[i](x,y) = d;

        // Update the histograms except for the last iteration
        //
        // Note: This is an optimized process for computing histograms at each of the orientations
        //       Instead of accumulating the values for the entire semicircles at each orientation,
        //       just add one wedge and remove another wedge to get the histogram for the next orientation
        if (i < n_orient - 1) {
          for (int j = 0; j < NBINS; ++j)
          {
            hist1.bins[j].weight += - wHist[i * NBINS + j].weight                                // remove this wedge
                                    + wHist[((i+n_orient) % (4*n_wedges)) * NBINS + j].weight;   // add this wedge

            hist1.bins[j].wsum   += - wHist[i * NBINS + j].wsum                                  // remove this wedge
                                    + wHist[((i+n_orient) % (4*n_wedges)) * NBINS + j].wsum;     // add this wedge

            hist2.bins[j].weight += - wHist[((i+n_orient) % (4*n_wedges)) * NBINS + j].weight    // remove this wedge
                                    + wHist[i * NBINS + j].weight;                               // add this wedge

            hist2.bins[j].wsum   += - wHist[((i+n_orient) % (4*n_wedges)) * NBINS + j].wsum      // remove this wedge
                                    + wHist[i * NBINS + j].wsum;                                 // add this wedge
          }
        }
      }
    }
  }

  // garbage collection
  delete mask, masksum;
  delete wHist;

  // allocate space for computing the final contrast magnitude and orientation
  hist_ori.set_size(img.ni(), img.nj());
  hist_ori.fill(0.0);
  hist_grad.set_size(img.ni(), img.nj());
  hist_grad.fill(0.0);

  // Filter the responses at each orientation using Savistzky-Golay filtering
  //  Allocate space for the filtered responses
  std::vector<vil_image_view<double> > filt_hist_dist(n_orient);

  // loop over all the pixels in the image to compute NMS over orientations
  double* dist = new double[n_orient];

  for (unsigned x = masksz; x < img.ni()-masksz; x+= spacing) {
    for (unsigned y = masksz; y < img.nj()-masksz; y+=spacing)
    {
        for (int i=0; i<n_orient; ++i)
          dist[i] = hist_dist[i](x,y);
      // compute edge strength and orientation at the current pixel
      compute_strength_and_orient(dist, n_orient, hist_grad(x,y), hist_ori(x,y));
    }
  }

  delete dist;
}
#endif // 0


//: Detect edges using the compass operator
// Note: # of orientations = 2*n_wedges

vil_image_view<float> bil_detect_compass_edges(vil_image_view<vxl_byte>& image,
                                               int n_wedges,
                                               double sigma,   double threshold,
                                               vil_image_view<float>& hist_grad)
{
  // convert to grayscale
  vil_image_view<vxl_byte> img;
  if (image.nplanes() == 3) {
    vil_convert_planes_to_grey(image, img );
  }
  else if (image.nplanes() == 1) {
    img = image;
  }

  // determine some relevant parameters
  int n_orient = 2*n_wedges; // number of orientations
  int masksz = (int) std::ceil(3 * sigma); // mask size

  // allocate space for computing the final contrast magnitude and orientation
  vil_image_view<double> hist_ori(img.ni(), img.nj());
  hist_grad.set_size(img.ni(), img.nj());
  hist_grad.fill(0.0);

  // loop over all the pixels in the image to compute NMS over orientations
  auto* dist = new double[n_orient];

  // create the wedge masks
  double *mask, *masksum;
  double wedge_wt = CreateMask(sigma, n_wedges, masksz, 0, &mask, &masksum);

  // allocate space for the histograms
  auto* wHist = new bil_bin[4*n_wedges*NBINS];       // All wedge histograms
  bil_signature hist1, hist2;         // Semi-circle histograms
  bil_signature hist1norm, hist2norm; // normalized histograms

  bil_signature qhist1, qhist2, qhist3, qhist4;
  bil_signature qhist1_norm, qhist2_norm, qhist3_norm, qhist4_norm;

  // loop over all the pixels in the image
  for (unsigned x = masksz; x < img.ni()-masksz; ++x) {
    for (unsigned y = masksz; y < img.nj()-masksz; ++y)
    {
      // Create wedge histograms (output in hist)
      CreateWedgeHistograms(img, mask, y, x, masksz, n_wedges, wHist);

      // Compute initial histogram sums (i.e., or the first orientation)
      // Note: half of the wedges contribute towards one histogram
      //       and the rest contribute towards the other
      for (int i = 0; i < NBINS; ++i)
      {
        hist1.bins[i].weight = 0.0;
        hist1.bins[i].wsum = 0.0;
        for (int j = 0; j < 2*n_wedges; ++j) {
          hist1.bins[i].weight += wHist[j * NBINS + i].weight;
          hist1.bins[i].wsum   += wHist[j * NBINS + i].wsum;
        }

        hist2.bins[i].weight = 0.0;
        hist2.bins[i].wsum = 0.0;
        for (int j = 2*n_wedges; j < 4*n_wedges; ++j) {
          hist2.bins[i].weight += wHist[j * NBINS + i].weight;
          hist2.bins[i].wsum   += wHist[j * NBINS + i].wsum;
        }
      }

      // Loop over every orientation
      for (int i = 0; i < n_orient; ++i)
      {
        // Normalize the histograms
        for (int j = 0; j < NBINS; ++j) {
          hist1norm.bins[j].wsum  = hist1.bins[j].wsum;
          hist2norm.bins[j].wsum  = hist2.bins[j].wsum;

          hist1norm.bins[j].value  = hist1.bins[j].wsum / hist1.bins[j].weight;
          hist2norm.bins[j].value  = hist2.bins[j].wsum / hist2.bins[j].weight;

          // Normalize by the number of wedges
          hist1norm.bins[j].weight = hist1.bins[j].weight / (wedge_wt * n_orient);
          hist2norm.bins[j].weight = hist2.bins[j].weight / (wedge_wt * n_orient);
        }

        // compute distance between the normalized histograms
        double d = 0.0;
        // TO FIX: hardcoded
        int dist_op=2;
        if (dist_op==0)
          d = bil_chi_sq_dist(hist1norm.bins, hist2norm.bins); // compute chi^2 dist
        else if (dist_op==1)
          d = bil_bhat_dist(hist1norm.bins, hist2norm.bins);   // compute Bhattacharyya dist
        else if (dist_op==2)
          d = bil_gray_EMD(hist1norm.bins, hist2norm.bins);    // compute EMD

        dist[i] = d;

        // Update the histograms except for the last iteration
        //
        // Note: This is an optimized process for computing histograms at each of the orientations
        //       Instead of accumulating the values for the entire semicircles at each orientation,
        //       just add one wedge and remove another wedge to get the histogram for the next orientation
        if (i < n_orient - 1) {
          for (int j = 0; j < NBINS; ++j)
          {
            hist1.bins[j].weight += - wHist[i * NBINS + j].weight                                // remove this wedge
                                    + wHist[((i+n_orient) % (4*n_wedges)) * NBINS + j].weight;   // add this wedge

            hist1.bins[j].wsum   += - wHist[i * NBINS + j].wsum                                  // remove this wedge
                                    + wHist[((i+n_orient) % (4*n_wedges)) * NBINS + j].wsum;     // add this wedge

            hist2.bins[j].weight += - wHist[((i+n_orient) % (4*n_wedges)) * NBINS + j].weight    // remove this wedge
                                    + wHist[i * NBINS + j].weight;                               // add this wedge

            hist2.bins[j].wsum   += - wHist[((i+n_orient) % (4*n_wedges)) * NBINS + j].wsum      // remove this wedge
                                    + wHist[i * NBINS + j].wsum;                                 // add this wedge
          }
        }
      }

      // compute edge strength and orientation at the current pixel
      compute_strength_and_orient(dist, n_orient, hist_grad(x,y), hist_ori(x,y));
    }
  }

  // TODO: when spacing>1, the NMS has to be done on the coarser grid
  vil_image_view<double> dx(img.ni(), img.nj());
  vil_image_view<double> dy(img.ni(), img.nj());
    double* Ori = hist_ori.top_left_ptr();
  double* Gx = dx.top_left_ptr();
  double* Gy = dy.top_left_ptr();

  for (unsigned long i=0; i<hist_ori.size(); i++) {
    Gx[i] = std::sin(Ori[i]);
    Gy[i] = std::cos(Ori[i]);
  }
  bil_nms NMS(bil_nms_params(threshold, bil_nms_params::PFIT_3_POINTS), dx, dy, hist_grad);
  NMS.apply();

  // garbage collection
  delete mask;
  delete [] masksum;
  delete [] wHist;
  delete[] dist;
  vil_image_view<float> magimg=NMS.mag();

  return  magimg;
}


//: Compute Chi^2 distance between two histograms
double bil_chi_sq_dist(const bil_bin hist1[], const bil_bin hist2[])
{
  double dist = 0.0;
  for (int i=0; i<NBINS; ++i)
    dist += (hist1[i].weight - hist2[i].weight)*(hist1[i].weight - hist2[i].weight)/(hist1[i].weight + hist2[i].weight + 2e-16);

  return dist;
}

//: Compute Bhattacharyya distance between two histograms
double bil_bhat_dist(const bil_bin hist1[], const bil_bin hist2[])
{
  double dist = 0.0;
  for (int i=0; i<NBINS; ++i)
    dist += hist1[i].weight*hist2[i].weight;

  return dist;
}

//: Compute EMD between two signatures (signature 1=dirt, signature 2 = hole)
//
// This implementation of the Earth Mover's Distance (EMD) is for one-
// dimensional datasets where the distance between clusters can be easily
// computed on the fly.
//
// implemented from the pseudocode in Ruzon's Thesis
double bil_gray_EMD(const bil_bin dirt[], const bil_bin hole[])
{
  int i = -1, j = -1;
  double leftoverdirt = 0.0, leftoverhole = 0.0, work = 0.0;
  double dirt_amt, hole_amt;

  while (true)
  {
    // Compute the amount of mass in the lowest numbered bin that hasn't
    // been moved yet from the piles of dirt
    if (leftoverdirt == 0.0)
    {
      // advance i to the next non-empty interval
      ++i;
      while (dirt[i].weight == 0.0 && i < NBINS) ++i;

      // if no more intervals
      if (i == NBINS)
        return work; // we're done
      else
        dirt_amt = dirt[i].weight;
    }
    else // use the amount that was left over from the last move
      dirt_amt = leftoverdirt;

    // Do the same for the holes
    if (leftoverhole == 0.0)
    {
      // advance j to the next non-empty interval
      ++j;
      while (hole[j].weight == 0.0 && j < NBINS) ++j;

      // if no more intervals
      if (j == NBINS)
        return work; // we're done
      else
        hole_amt = hole[j].weight;
    }
    else // use the amount that was left over from the last move
      hole_amt = leftoverhole;

    // Compute the work done moving the smaller amount of mass and decide
    // how much is left over in each bin.
    double massmoved = std::min(dirt_amt, hole_amt);

    work += massmoved * (1 - std::exp(-std::fabs(hole[j].value - dirt[i].value) / GAMMA));

    leftoverdirt = dirt_amt - massmoved;
    leftoverhole = hole_amt - massmoved;
  }
}
