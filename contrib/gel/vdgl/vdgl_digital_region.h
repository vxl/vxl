#ifndef vdgl_digital_region_h_
#define vdgl_digital_region_h_

//-----------------------------------------------------------------------------
//:
// \file
// \brief A representation of the digital interior of a region.
//
//  Maintains the discrete geometry and intensity data of a Face, a 2-d
//  analog of EdgelChain. So far the class is a very simple group of 1-d
//  arrays for holding the x, y, z, pixel locations and the intensity.
//  The class maintains 2d/3d pixels in the style of TargetJr and could be
//  used to represent 3-d intensity points
//
// \author
//             Joe Mundy November 27, 1999
//             GE Corporate Research and Development.
//
// \verbatim
// Modifications
//  8-May-2002 - Peter Vanroose - now inherits from vsol_region_2d
// \endverbatim
//-----------------------------------------------------------------------------

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vsol/vsol_region_2d.h>

class vdgl_digital_region : public vsol_region_2d
{
  // PUBLIC INTERFACE----------------------------------------------------------

public:

  // Constructors/Initializers/Destructors---------------------------------
  vdgl_digital_region();
  vdgl_digital_region(const int npts, const float* xp, const float* yp,
                      const unsigned short *pix);
  vdgl_digital_region(const int npts, const float* xp, const float* yp,
                      const float* zp, const unsigned short *pix);
  ~vdgl_digital_region();

  // Data Access-----------------------------------------------------------
  // Data storage for the pixel arrays
  void ResetPixelData();
  void IncrementMeans(float x, float y, unsigned short pix);
  void IncrementMeans(float x, float y, float z, unsigned short pix);
  void InitPixelArrays();
  void InsertInPixelArrays(float x, float y, unsigned short pix);
  void InsertInPixelArrays(float x, float y, float z, unsigned short pix);

  //The region pixel coordinates and intensities
  float* Xj() {return xp_;}
  float* Yj() {return yp_;}
  float* Zj() {return zp_;}
  unsigned short* Ij() {return pix_;}
  int Npix(){return npts_;}

  //The size of a region pixel in image pixel units
  //(due to expanded resolution processing)
  void set_pixel_size(float pixel_size){pixel_size_= pixel_size;}
  float get_pixel_size(){return pixel_size_;}

  // Min and Max region intensities
  float get_min() {return min_;}
  float get_max() {return max_;}
  //Access to the pixels interior to the region
  void reset();      //Reset pixel iterator
  bool next();       //Increment to next pixel
  float X();           //The x  pixel coordinate
  float Y();           //The y pixel coordinate
  float Z();           //The z pixel coordinate
  unsigned short I();//The pixel intensity
  //The mean geometric and intensityvalues of the region
  float Xo();
  float Yo();
  float Zo();
  float Io();
#if 0
  //Histogram of face intensity intensity
  Histogram_ref GetHistogram(bool force = false);
  Histogram_ref GetResidualHistogram();
#endif
  //Scatter Matrix Values
  double X2();
  double Y2();
  double XY();
  double I2();
  double XI();
  double YI();
  double Xi();
  double Yi();
  double Ii();
  //Quantities computable from the region scatter matrix
  float Diameter();
      //distinguish from Face::Area()
  virtual double area() const {return npts_*pixel_size_*pixel_size_;}

  float AspectRatio();
#if 0
  void PrincipalOrientation(vcl_vector<float>& major_axis);
#endif
  //Get Fitted Plane Coefficients
  double Ix();  //First derivative of intensity wrt x
  double Iy();  //First derivative of intensity wrt y
  double Var(){return sigma_sq_;} //The plane fitting error.
#if 0
  IntensityCoef_ref GetIntCoef();//A package of coefficients
#endif
  float Ir();        //The pixel intensity with the plane subtracted

  // Utility Methods
  void DoPlaneFit(); //Fit a plane to the region intensities
  void PrintFit();

  virtual vsol_spatial_object_2d_sptr clone() const;

 protected:
  //
  void init();              //Set inital region data
  // Members
  int npts_;                //Number of pixels in the region
  float pixel_size_;        //Image pixel size in fractions of a pixel
  float *xp_, *yp_, *zp_;   //The location of each pixel
  unsigned short *pix_;     //The pixel intensity
  float max_, min_;         //Upper and lower bounds
  float xo_, yo_, zo_, io_; //Mean Values
  int pix_index_;           //Index in pixel array
  void ComputeScatterMatrix();
  void IncrByXYI(double x, double y, int intens);
  void SolveForPlane();
  double ComputeResidual(vnl_matrix<double>& pvect);
  double ComputeSampleResidual();
#if 0
  bool Transform(CoolTransform const& t);
#endif
  //members
  bool fit_valid_;           //Has a plane fit been done?
  bool scatter_matrix_valid_; //Is the scatter matrix current?
  double Ix_;                //dI/dx
  double Iy_;                //dI/dy
  double X2_,Y2_,I2_;        //The sums of various monomials
  double XY_,XI_,YI_;        // To form the scatter matrix
  double Xi_, Yi_, Ii_;      //
  double error_, sigma_sq_;  //fitting errors
  vnl_matrix<double> *Si_, *pi_, *mi_; //Scatter matrices and etc.
#if 0
  Histogram_ref hist_;//A histogram of the region intensity
#endif
};

#endif // vdgl_digital_region_h_
