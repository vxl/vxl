// This is gel/vdgl/vdgl_digital_region.h
#ifndef vdgl_digital_region_h_
#define vdgl_digital_region_h_
//:
// \file
// \brief A representation of the digital interior of a region.
//
//  Maintains the discrete geometry and intensity data of a Face, a 2-d
//  analog of EdgelChain. So far the class is a very simple group of 1-d
//  arrays for holding the x and y pixel locations and the intensity.
//  The class maintains 2d pixels in the style of TargetJr and could be
//  used to represent 2-d intensity points
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
//  22-Sep-2004 - Peter Vanroose - removed 3D interface: class is intrinsically 2D
//  22-Sep-2004 - Peter Vanroose - added transform() (projective transformation)
//  22-Sep-2004 - Peter Vanroose - added histogram() and residual_histogram()
// \endverbatim
//-----------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vcl_string.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_float_3x3.h>
#include <vnl/vnl_float_2.h>
#include <vsol/vsol_region_2d.h>
#include <vsol/vsol_point_2d.h>
class vdgl_digital_region : public vsol_region_2d
{
 public:

  // Constructors/Initializers/Destructors---------------------------------
  vdgl_digital_region()
  : vsol_region_2d(),
    npts_(0), pixel_size_(1.f), xp_(0), yp_(0), pix_(0),
    max_(0), min_((unsigned short)(-1)), xo_(0.f), yo_(0.f),
    io_(0.f), io_stdev_(0.0f), pix_index_(0),
    fit_valid_(false), scatter_matrix_valid_(false),
    X2_(0), Y2_(0), I2_(0), XY_(0), XI_(0), YI_(0), error_(0), sigma_sq_(0) {}

  vdgl_digital_region(int npts, const float* xp, const float* yp, const unsigned short *pix);
  vdgl_digital_region(vdgl_digital_region const& r);
  ~vdgl_digital_region();

  // Data Access-----------------------------------------------------------
  // Data storage for the pixel arrays
  void ResetPixelData();
  void IncrementMeans(float x, float y, unsigned short pix);
  void InitPixelArrays();
  void InsertInPixelArrays(float x, float y, unsigned short pix);

  //: The region pixel coordinates and intensities
  float const* Xj() const {return xp_;}
  float const* Yj() const {return yp_;}
  unsigned short const* Ij() const {return pix_;}
  unsigned int Npix() const {return npts_;}

  //: The size of a region pixel in image pixel units
  //(due to expanded resolution processing)
  void set_pixel_size(float pixel_size){pixel_size_= pixel_size;}
  float get_pixel_size() const {return pixel_size_;}

  // Min and Max region intensities
  float get_min() const { return min_; }
  float get_max() const { return max_; }
  //: reset the iterator for accessing region pixels
  void reset() const { pix_index_ = -1; }
  //: increment to next pixel and check if iterator is finished
  bool next() const { ++pix_index_; return pix_index_<(int)npts_; }
  float X() const;    //!< The x pixel coordinate
  float Y() const;    //!< The y pixel coordinate
  unsigned short I() const;//!< The pixel intensity

  void set_X(float x);    //!< change x pixel coordinate
  void set_Y(float y);    //!< change y pixel coordinate
  void set_I(unsigned short I);    //!< change pixel intensity

  // The mean geometric and intensity values of the region
  float Xo() const; //!< The mean X value of the region
  float Yo() const; //!< The mean Y value of the region
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
  
  //: The centroid of the pointset 
  virtual vsol_point_2d_sptr centroid() const
    {return new vsol_point_2d(this->Xo(), this->Yo());}
  
  //: transform this region using the given 3x3 projective transformation matrix
  bool transform(vnl_float_3x3 const& t);

  //: Compute the intensity histogram
  //  The intensity ranges from get_min() to get_max().
  vcl_vector<unsigned int> histogram(int nbins);
  //: Compute the residual intensity histogram
  //  The intensity range is returned as the last two arguments.
  vcl_vector<unsigned int> residual_histogram(int nbins, float* min=0, float* max=0);

  //: Return true if this region is convex
  virtual bool is_convex() const { return false; } // virtual of vsol_region_2d

  void PrincipalOrientation(vnl_float_2& major_axis);

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
  vcl_string is_a() const { return vcl_string("vdgl_digital_region"); }

 protected:
  // Members
  unsigned int npts_;       //!< Number of pixels in the region
  float pixel_size_;        //!< Image pixel size in fractions of a pixel
  float *xp_, *yp_;         //!< The location of each pixel
  unsigned short *pix_;     //!< The pixel intensities
  float max_, min_;         //!< Upper and lower bounds
  float xo_, yo_, io_;      //!< Mean Values
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
