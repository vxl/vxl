// This is gel/vdgl/vdgl_digital_region.h
#ifndef vdgl_digital_region_h_
#define vdgl_digital_region_h_
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
//   Joe Mundy November 27, 1999
//   GE Corporate Research and Development.
//
// \verbatim
// Modifications
//   8-May-2002 - Peter Vanroose - now inherits from vsol_region_2d
//  15-May-2002 - Peter Vanroose - inconsistency Xi() versus Ix() removed
//                (There were three pairs of data members both referring to
//                 intensity information, but only one of them was updated.)
//   8-Jan-2003 - Peter Vanroose - added is_convex() (virtual of vsol_region_2d)
// \endverbatim
//-----------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vnl/vnl_double_3x3.h>
#include <vsol/vsol_region_2d.h>

class vdgl_digital_region : public vsol_region_2d
{
 public:

  // Constructors/Initializers/Destructors---------------------------------
  vdgl_digital_region();
  vdgl_digital_region(int npts, const float* xp, const float* yp, const unsigned short *pix);
  vdgl_digital_region(int npts, const float* xp, const float* yp, const float* zp, const unsigned short *pix);
  vdgl_digital_region(vdgl_digital_region const& r);
  ~vdgl_digital_region();

  // Data Access-----------------------------------------------------------
  // Data storage for the pixel arrays
  void ResetPixelData();
  void IncrementMeans(float x, float y, unsigned short pix);
  void IncrementMeans(float x, float y, float z, unsigned short pix);
  void InitPixelArrays();
  void InsertInPixelArrays(float x, float y, unsigned short pix);
  void InsertInPixelArrays(float x, float y, float z, unsigned short pix);

  //: The region pixel coordinates and intensities
  float const* Xj() const {return xp_;}
  float const* Yj() const {return yp_;}
  float const* Zj() const {return zp_;}
  unsigned short const* Ij() const {return pix_;}
  int Npix() const {return npts_;}

  //: The size of a region pixel in image pixel units
  //(due to expanded resolution processing)
  void set_pixel_size(float pixel_size){pixel_size_= pixel_size;}
  float get_pixel_size() const {return pixel_size_;}

  // Min and Max region intensities
  float get_min() const {return min_;}
  float get_max() const {return max_;}
  // Access to the pixels interior to the region
  void reset() const; //!< Reset pixel iterator
  bool next() const;  //!< Increment to next pixel
  float X() const;    //!< The x pixel coordinate
  float Y() const;    //!< The y pixel coordinate
  //:should deprecate the Z access since intensity face is strictly 2d
  float Z() const;    //!< The z pixel coordinate
  unsigned short I() const;//!< The pixel intensity

  void set_X(float x);    //!< change x pixel coordinate
  void set_Y(float y);    //!< change y pixel coordinate
  void set_I(unsigned short I);    //!< change pixel intensity

  // The mean geometric and intensity values of the region
  float Xo() const; //!< The mean X value of the region
  float Yo() const; //!< The mean Y value of the region
  float Zo() const; //!< The mean Z value of the region
  float Io() const; //!< The mean intensity value of the region
  float Io_sd() const; //!< The intensity standard deviation value of the region
  float ComputeIntensityStdev(); //!< Compute the intensity stdev for the region

  // Scatter Matrix Values
  double X2() const; //!< The second order X moment of the region
  double Y2() const; //!< The second order Y moment of the region
  double XY() const; //!< The second order X,Y moment of the region
  double I2() const; //!< The second order intensity moment of the region
  double XI() const; //!< The second order X,intensity moment of the region
  double YI() const; //!< The second order Y,intensity moment of the region
  // Quantities computable from the region scatter matrix
  float Diameter() const;
  float AspectRatio() const;
  // distinguish from vtol_face::area()
  virtual double area() const { return npts_*pixel_size_*pixel_size_; }

  //: Return true if this region is convex
  virtual bool is_convex() const { return false; } // virtual of vsol_region_2d

#if 0
  void PrincipalOrientation(vcl_vector<float>& major_axis);
#endif
  //Get Fitted Plane Coefficients
  double Ix() const;  //!< First derivative of intensity wrt x
  double Iy() const;  //!< First derivative of intensity wrt y
  double Var() const {return sigma_sq_;} //!< The plane fitting error.
  float Ir() const;   //!< The pixel intensity with the plane subtracted

  // Utility Methods
  void DoPlaneFit() const; //!< Fit a plane to the region intensities
  void PrintFit() const;

  virtual vsol_spatial_object_2d* clone() const;

  //: Return a platform independent string identifying the class
  vcl_string is_a() const;

 protected:
  // Members
  int npts_;                //!< Number of pixels in the region
  float pixel_size_;        //!< Image pixel size in fractions of a pixel
  float *xp_, *yp_, *zp_;   //!< The location of each pixel
  unsigned short *pix_;     //!< The pixel intensity
  float max_, min_;         //!< Upper and lower bounds
  float xo_, yo_, zo_, io_; //!< Mean Values
  float io_stdev_;          //!< Intensity standard deviation for region
  mutable int pix_index_;   //!< Index in pixel array (iterator)
  void ComputeScatterMatrix() const; // mutable
  void IncrByXYI(double x, double y, int intens) const; // mutable
  double ComputeSampleResidual() const; // mutable
  //members
  mutable bool fit_valid_;           //!< Has a plane fit been done?
  mutable bool scatter_matrix_valid_;//!< Is the scatter matrix current?
  mutable double Ix_;                //!< dI/dx
  mutable double Iy_;                //!< dI/dy
  //: The sums of various monomials to form the scatter matrix
  mutable double X2_,Y2_,I2_,XY_,XI_,YI_;
  mutable double error_, sigma_sq_;  //!< fitting errors
  mutable vnl_double_3x3 Si_;        //!< scatter matrix
};

#endif // vdgl_digital_region_h_
