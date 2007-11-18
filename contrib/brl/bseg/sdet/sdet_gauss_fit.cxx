// This is brl/bseg/sdet/sdet_gauss_fit.cxx
#include <sdet/sdet_gauss_fit.h>
//:
// \file

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cmath.h>
#include <vgl/vgl_point_3d.h>
#include <vnl/vnl_math.h>

// for new matrix operations
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_inverse.h>

#include <vnl/algo/vnl_levenberg_marquardt.h>

#define JIL_DIAGNOSTICS
// -----------------------------------------------------------------
// constructor
sdet_adjust_lsqr::
    sdet_adjust_lsqr( vcl_vector<vgl_point_3d<double> > const& img_pts,
                      unsigned int num_unknowns, unsigned int num_residuals,
                      int n_peaks)
            : vnl_least_squares_function(num_unknowns, num_residuals,
                                         vnl_least_squares_function::no_gradient)
            , img_pts_(img_pts)
            , num_pixels_(img_pts_.size())
            , n_peaks_(n_peaks)
{
}

// -----------------------------------------------------------------
// MAIN method: function "f" to calculate errors
// The virtual least-squares cost function -
//        calculates the fit of pixels to 2-d gaussian curve

// The 1 + n+peaks*6 unknowns to fit:
//                 [0] plane = zero plane (floor) of gaussian distributions, same for all peaks
//        Each peak has the following 6 parameters
//                 [1] peak  = peak value of gaussian distribution
//                 [2] x_bar = x location (in pixels) of peak
//                 [3] y_bar = y location (in pixels) of peak
//            And the 2x2 covariance matrix (the 2 off-diagonals are equal)
//                 [4] var_x = variance in x direction [0,0]
//                 [5] var_y = variance in y direction [1,1]
//                 [6] covar = off-diagonal covariance values [0,1] = [1,0]

// The fit_error residual vector elements are mis-fit errors for each image point
//    (NOTE: the errors are set to the absolute values (not squares) of the differences)

void sdet_adjust_lsqr::f( vnl_vector<double> const& unknowns,
                          vnl_vector<double> & fit_error)
{
  // set the parameters of the 2-d gaussain from "unknowns" and
  //   calculate the pixel point predictions using these values

  double pi = vnl_math::pi;
  vnl_vector<double> fit_value(num_pixels_); // the gaussian predictions of image intensity
  vnl_vector<double> sum(num_pixels_); // the sum of gaussians from all peaks
  // zero out all sums
  for (unsigned i=0; i < num_pixels_; i++)  {
    sum[i] = 0.0;
  }

  // Multiple peaks complicate this calculation, as the contribution of all the
  //   peaks must be added together to get the predicted value at a pixel

  // Loop through the peaks to add up predicted value for each pixel
  for (int j=0; j < n_peaks_; j++)
  {
    vnl_matrix_fixed<double,2,2> V;
    V(0,0) = unknowns[6*j+4];                    // the x variance
    V(0,1) = unknowns[6*j+6];                    // the covariance (equal)
    V(1,0) = unknowns[6*j+6];                    // the covariance (equal)
    V(1,1) = unknowns[6*j+5];                    // the y variance

    vnl_matrix_fixed<double,2,2> Vinv = vnl_inverse(V);

    for (unsigned i=0; i < img_pts_.size(); i++)
    {
      double peak_delta = unknowns[6*j+1] - unknowns[0]; //predicted height of peak above bkgd

      vnl_matrix_fixed<double,2,1> D;                    // the pixel's xi-xbari matrix
      D(0,0) = img_pts_[i].x() - unknowns[6*j+2];        // pixel x loc - peak x loc
      D(1,0) = img_pts_[i].y() - unknowns[6*j+3];        // pixel y loc - peak y loc

      vnl_matrix_fixed<double,1,2> Dtrans = D.transpose();    // the xi-xbari matrix transpose
      double exponent = ((-1.0 * Dtrans * Vinv * D)(0,0))/2.0;    // exponent of e

      //value of gaussian at x,y, a matrix operation
      double gaussian = peak_delta * vcl_exp(exponent);

      // add up value of summed components of multiple peaks at eacy image point
      sum[i] += gaussian;
    }
  }

  // finally, add "floor" value to sum
  for (unsigned i=0; i < img_pts_.size(); i++)
    fit_value[i] = sum[i] + unknowns[0];    // add value of pixel above "floor"

#if 0 // Debug:  print out values for this step
  vcl_cout << "plane(" << unknowns[0] << "),  peak(" << unknowns[1] << "),\n"
           << "x(" << unknowns[2] << "),  y(" << unknowns[3] << ")\n"
           << "var x(" << unknowns[4] << "),  var y(" << unknowns[5] << ")\n"
           << "covar(" << unknowns[6] << ')' << vcl_endl;
#endif

// calculate the fit_error values of the current fit
  for (unsigned i = 0; i < img_pts_.size(); ++i)
  {
    double e = (fit_value[i] - img_pts_[i].z());
    fit_error[i] = vcl_fabs(e);
#if 0 // Debug:  print out values of fit error for each point
    vcl_cout << "fiterror[" << i << ']' << fit_error[i] << vcl_endl;
#endif
  }
}


// -------------------------------------------------------------------------
// Initialize the process

static bool init(vcl_vector<vgl_point_3d<double> > img_pts, vcl_vector<double>& peak,
                 double& plane, vcl_vector<double>& ux, vcl_vector<double>& uy,
                 vcl_vector<double>& sxx, vcl_vector<double>& syy, vcl_vector<double>& sxy,
                 vcl_vector<vsol_point_2d_sptr> ps_list, int n_peaks, double xmin, double ymin)
{
  //find the smallest intensity value in polygon
  plane = 65000.;
  for (unsigned i=0; i<img_pts.size(); i++)
  {
    if ( img_pts[i].z() < plane )
    plane = img_pts[i].z();
  }

  // With multiple peaks, finding the largest is more complex,
  // Let's assume that the largest value around each peak will occur near the
  //   selected position.  So let's just look in that local area for a couple pixels.

  for (int j=0; j < n_peaks; j++)
  {
    peak.push_back(0.0);                // create vector elements for this peak
    ux.push_back(0.0);
    uy.push_back(0.0);
    sxx.push_back(0.0);
    syy.push_back(0.0);
    sxy.push_back(0.0);

    double xf = ps_list[j]->x() - xmin;            // "picked" values of x & y for this peak
    double yf = ps_list[j]->y() - ymin;

    for ( unsigned i=0; i<img_pts.size(); i++)
    {
      double dx = img_pts[i].x() - xf;
      double dy = img_pts[i].y() - yf;
      // is this point within 2 pixels of selected peak's "picked" location??
      if (dx >= -2. && dx <= 2. && dy >= -2. && dy <= 2.)
      if ( img_pts[i].z() > peak[j] )  {
        peak[j] = img_pts[i].z();
        ux[j] = img_pts[i].x();                // these are our rough guesses
        uy[j] = img_pts[i].y();
      }
    }

    // Compute various moments of the "Gaussian" distribution
    // For multiple peaks, the moments become much more difficult to calculate.
    // Let's see if a simplifying assumption can be used and still converge.  This
    //   assumption will be that the highest pixel value arpund the peak will be a
    //   good starting point for the peak and x,y values.  We will assume the variances
    //   will be about half a pixel with covariance = 0. (see above and below)

#if 0 // Original code, now commented out
    double sum = 0.0;
    ux = 0.0;
    uy = 0.0;
    for (unsigned i=0; i<img_pts.size(); i++)
    {
      vgl_point_3d<double>& p = img_pts[i];
      double w = p.z()-plane;
      sum += w;
      double x = p.x(), y = p.y();
      ux += w*x;
      uy += w*y;
      sxx += x*x*w;
      syy += y*y*w;
      sxy += x*y*w;
    }

    if (sum<=0) return false;
    ux /= sum;
    uy /= sum;
    sxx = 0;
    syy = 0;
    sxy = 0;

    for (unsigned i=0; i<img_pts.size(); i++)
    {
      vgl_point_3d<double>& p = img_pts[i];
      double w = p.z()-plane;
      double x = p.x(), y = p.y();
      sxx += (x-ux)*(x-ux)*w;
      syy += (y-uy)*(y-uy)*w;
      sxy += (x-ux)*(y-uy)*w;
    }

    sxx /= sum;
    syy /= sum;
    sxy /= sum;
#endif // 0

    sxx[j] = 0.5;                // more rough guesses
    syy[j] = 0.5;
    sxy[j] = 0.0;
  }                            // end of for j loop
  return true;
}


// ------------------------------------------------------------
//: Adjust the parameters of the 2d gaussian
//    Returns adjusted fit parameters

vnl_vector<double> sdet_gauss_fit::adjust( vcl_vector<vgl_point_3d<double> > img_pts,
                                           vcl_vector<vsol_point_2d_sptr> ps_list,
                                           int n_peaks, vcl_ofstream& outfile,
                                           double xmin, double ymin)
{
  unsigned num_pixels = img_pts.size();
  unsigned num_unknowns = 6*n_peaks + 1;            // zero level + (6 * number of peaks)
  unsigned num_residuals = num_pixels;

  // Do some calculations to find reasonable values to start with

  // We want to be able to draw a polygon on the image and fit only the
  //   pixels in that polygon.  So here the pixels and their x,y location
  //   in the image are stored in a vcl_vector<vgl_point_3d<double> img_pnts

  double plane;
  vcl_vector<double> peak;
  vcl_vector<double> ux;
  vcl_vector<double> uy;
  vcl_vector<double> sxx;
  vcl_vector<double> syy;
  vcl_vector<double> sxy;

  if (!init(img_pts, peak, plane, ux, uy, sxx, syy, sxy, ps_list, n_peaks, xmin, ymin))
  {
    outfile << "ERROR!! sdet_gauss_fit::adjust(), Cannot init()" << vcl_endl;
    vcl_cerr << "ERROR!! sdet_gauss_fit::adjust(), Cannot init()" << vcl_endl;
  }

  for (int i=0; i < n_peaks; i++)
  {
#if 0
    outfile << "Init values:\n-----------------------------------------------\n Peak "
            << i+1 << "\n -------------------------\nplane= " << plane << vcl_endl
            << "peak=" << peak[i] << ",  ux=" << ux[i]
            << ",  uy=" << uy[i] << ",  sxx=" << sxx[i] << ",  syy=" << syy[i]
            << ",  sxy=" << sxy[i] << vcl_endl;
    outfile.flush();
#endif
    vcl_cout << "Init values:\n-----------------------------------------------\n Peak "
             << i+1 << "\n -------------------------\nplane= " << plane << vcl_endl
             << "peak=" << peak[i] << ",  ux=" << ux[i]
             << ",  uy=" << uy[i] << ",  sxx=" << sxx[i] << ",  syy=" << syy[i]
             << ",  sxy=" << sxy[i] << vcl_endl;
  }

  // Initialize the least squares function
  sdet_adjust_lsqr lsf(img_pts, num_unknowns, num_residuals, n_peaks);

  // --------
  // Create the Levenberg Marquardt minimizer
  vnl_levenberg_marquardt levmarq(lsf);

#ifdef JIL_DIAGNOSTICS
  levmarq.set_verbose(true);
  levmarq.set_trace(true);
#endif

  //Use the default values - Leaving these calls for potential future need
#if 0
  // Set the x-tolerance.  When the length of the steps taken in X (variables)
  // are no longer than this, the minimization terminates.
  levmarq.set_x_tolerance(1e-10);

  // Set the epsilon-function.  This is the step length for FD Jacobian.
  levmarq.set_epsilon_function(1);

  // Set the f-tolerance.  When the successive RMS errors are less than this,
  // minimization terminates.
  levmarq.set_f_tolerance(1e-15);
#endif

  // Set the maximum number of iterations
  levmarq.set_max_function_evals(1000);

  // Create an INITIAL values of parameters with which to start
  vnl_vector<double> unknowns(num_unknowns);

  unknowns[0] = plane;
  for (int i=0; i < n_peaks; i++)
  {
    unknowns[1+6*i] = peak[i];
    unknowns[2+6*i] = ux[i];
    unknowns[3+6*i] = uy[i];
    unknowns[4+6*i] = sxx[i];
    unknowns[5+6*i] = syy[i];
    unknowns[6+6*i] = sxy[i];

    outfile << "max pixel for peak " << i+1 << " = " << peak[i] << vcl_endl;
  }

  vcl_cout << "\nStarting Levenberg-Marquardt minimizer, may take a few seconds per iteration"
           << vcl_endl;

  // Minimize the error and get best correspondence vertices and translations
  levmarq.minimize(unknowns);

  // Summarize the results
  levmarq.diagnose_outcome();
#if 0
  outfile << "Min error of " << levmarq.get_end_error()
          << " at the following local minima : " << vcl_endl;
#endif
  vcl_cout << "Min error of " << levmarq.get_end_error()
           << " at the following local minima : " << vcl_endl;

  // print out parameter fit results
#if 0
  outfile << "  -----------------------------\nFitted parameters:\n------------" << vcl_endl;
#endif
  vcl_cout << "  -----------------------------\nFitted parameters:\n------------" << vcl_endl;

  for (unsigned i=0; i<num_unknowns; i++)
  {
#if 0
    outfile << "unknowns[" << i << "]= " << unknowns[i] << vcl_endl;
    if (i%6==4||i%6==5) vcl_cout << "  sd = " << vcl_sqrt(unknowns[i]) << vcl_endl;
#endif

    vcl_cout << "unknowns[" << i << "]= " << unknowns[i] << vcl_endl;
    if (i%6==4||i%6==5) vcl_cout << "  sd = " << vcl_sqrt(unknowns[i]) << vcl_endl;

    if ((i % 6) == 0)  {
#if 0
      outfile << "------------" << vcl_endl;
#endif
      vcl_cout << "------------" << vcl_endl;
    }
  }
  outfile.flush();
  vcl_cout.flush();

  return unknowns;
}

// -------------------------------------------------------
// Calculate the paremeters for the ellipse
// Argument is fitted 2-d gaussian parameters, returns ellipse angle and parameters

vnl_vector<double> sdet_gauss_fit::calculate_ellipse( vnl_vector<double> result, float x,
                                                      float y, int i, int n_peaks,
                                                      vcl_ofstream& outfile)
{
  // First check to be sure n_peaks agrees with the length of result
  vnl_vector<double> params(3*n_peaks);

  double pi = vnl_math::pi;
  double theta = 0.0;                  // angle from x axis to ellipse major axis
  double a = 0.0;                      // ellipse major axis std deviation
  double b = 0.0;                      // ellipse minor axis std deviation
  double sxx = result[6*i+4];          // copy result data into local variables
  double syy = result[6*i+5];
  double sxy = result[6*i+6];

  // take care of special cases where sxy = 0 or sxx = syy
  // First calculate the appropriate boolians about relationship of s(nn)
  bool xxgtyy = (sxx > syy);
  bool xxeqyy = (vcl_fabs(sxx - syy) < 0.0001);
  bool xygt0 = (sxy > 0.0001);
  bool xyeq0 = (vcl_fabs(sxy) < 0.0001);

  // Case 7
  if (xyeq0 && xxeqyy)
    theta = 0.0;                    // 0 degrees, along x axis
  // Case 2
  else if (xxeqyy && xygt0)
    theta = pi/4.0;                        // 45 degrees, diagonal
  // Case 3
  else if (xxeqyy && !xygt0)
    theta = 3.0*pi/4.0;                    // 135 degrees, diagonal
  // the other normal cases
  else
  {
    theta = 0.5 * vcl_atan(2.0 * sxy/(sxx - syy));

    // Case 1
    if (xxgtyy && xygt0) {}                // theta is correct
    // Case 4
    else if (xxgtyy && !xygt0)
      theta = theta + pi;                // theta + 180 degrees
    // Case 5&6
    else if (!xxgtyy)
      theta = theta + pi/2.0;            // theta + 90 degrees
  }

  double angle = theta*180./pi;

  // now calculate the one sigma length of the major and minor axis
  double sqrt2 = vcl_sqrt(2.0);
  double factor = vcl_sqrt(sxx*sxx + syy*syy + 4.0*sxy*sxy - 2.0*sxx*syy);
  a = vcl_sqrt(sxx + syy + factor)/sqrt2;
  b = vcl_sqrt(sxx + syy - factor)/sqrt2;

  // print out and write results to a file too
  outfile << "--------------------\nresults for peak " << i+1 << vcl_endl;
  vcl_cout << "--------------------\nresults for peak " << i+1 << vcl_endl;

  double bkgd = result[0];
  double peak = result[6*i+1];
  double d_peak = peak - bkgd;

  outfile << "bkgd= " << bkgd << ",  peak= " << peak << ",  d_peak= "
          << d_peak << vcl_endl
          << "x= " << x << ",  y= " << y << vcl_endl;
  vcl_cout << "bkgd= " << bkgd << ",  peak= " << peak << ",  d_peak= "
           << d_peak << vcl_endl
           << "x= " << x << ",  y= " << y << vcl_endl;
#if 0
  outfile << "sxx= " << sxx << ",  syy= " << syy << ",  sxy= " << sxy
          << ",  factor= " << factor << vcl_endl;
  vcl_cout << "sxx= " << sxx << ",  syy= " << syy << ",  sxy= " << sxy
           << ",  factor= " << factor << vcl_endl
           << "sin(theta)= " << vcl_sin(theta) << ",   cos(theta)= "
           << vcl_cos(theta) << vcl_endl;
#endif

  outfile << "theta= " << theta << ",   angle= " << angle
          << ",   a= " << a << ",   b= " << b << vcl_endl;
  vcl_cout << "theta= " << theta << ",   angle= " << angle
           << ",   a= " << a << ",   b= " << b << vcl_endl;
  outfile.flush();
  vcl_cout.flush();

  params[0] = theta;
  params[1] = a;
  params[2] = b;

    return params;
}
