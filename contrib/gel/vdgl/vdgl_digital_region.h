//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//                               
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//       
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>
#ifndef _DigitalRegion_h
#define _DigitalRegion_h

//-----------------------------------------------------------------------------
//
// .NAME DigitalRegion - A representation of the digital interior of  
//                       a region.  Analogous to EdgelChain.
//                      
// .LIBRARY DigitalGeometry
// .HEADER  SpatialObjects package
// .INCLUDE DigitalGeometry/DigitalRegion.h
// .FILE DigitalRegion.h
// .FILE DigitalRegion.C
//
// .SECTION Description:
//  Maintains the discrete geometry and intensity data of a Face, a 2-d
//  analog of EdgelChain. So far the class is a very simple group of 1-d
//  arrays for holding the x, y, z, pixel locations and the intensity.
//  The class maintains 2d/3d pixels in the style of TargetJr and could be 
//  used to represent 3-d intensity points
// .SECTION Author:
//             Joe Mundy November 27, 1999
//             GE Corporate Research and Development.
// .SECTION Modifications <none>
//-----------------------------------------------------------------------------

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_math.h>     // for sqrt(2)
#include <vgl/vgl_point_2d.h>
#include <vtol/vtol_face_2d.h>

class vdgl_digital_region
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
  float* Xj() {return _xp;}
  float* Yj() {return _yp;}
  float* Zj() {return _zp;}
  unsigned short* Ij() {return _pix;}
  int Npix(){return _npts;}

  //The size of a region pixel in image pixel units 
  //(due to expanded resolution processing)
  void set_pixel_size(float pixel_size){_pixel_size= pixel_size;}
  float get_pixel_size(){return _pixel_size;}

  // Min and Max region intensities
  float get_min() {return _min;}
  float get_max() {return _max;}
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
  float area(){return _npts*_pixel_size*_pixel_size;}

  float AspectRatio();
  void PrincipalOrientation(vcl_vector<float>& major_axis);
  //Get Fitted Plane Coefficients
  double Ix();  //First derivative of intensity wrt x
  double Iy();  //First derivative of intensity wrt y
  double Var(){return _sigma_sq;} //The plane fitting error.
#if 0
  IntensityCoef_ref GetIntCoef();//A package of coefficients
#endif
  float Ir();        //The pixel intensity with the plane subtracted

  // Utility Methods
  //UtilityMethods
  void DoPlaneFit(); //Fit a plane to the region intensities
  void PrintFit();

 protected:
  //
  void init();              //Set inital region data
  // Members
  int _npts;                 //Number of pixels in the region
  float _pixel_size;           //Image pixel size in fractions of a pixel
  float *_xp, *_yp, *_zp;     //The location of each pixel
  unsigned short *_pix;      //The pixel intensity
  float _max, _min;          //Upper and lower bounds
  float _xo, _yo, _zo, _io;  //Mean Values
  int _pix_index;            //Index in pixel array
  void ComputeScatterMatrix();
  void IncrByXYI(double x, double y, int intens);
  void SolveForPlane();    
  double ComputeResidual(vnl_matrix<double>& pvect);
  double ComputeSampleResidual();
#if 0
  bool Transform(CoolTransform const& t);
#endif
  //members
  bool _fit_valid;           //Has a plane fit been done?
  bool _scatter_matrix_valid; //Is the scatter matrix current?
  double _Ix;                //dI/dx 
  double _Iy;                //dI/dy 
  double _X2,_Y2,_I2;        //The sums of various monomials
  double _XY,_XI,_YI;        // To form the scatter matrix
  double _Xi, _Yi, _Ii;      //
  double _error, _sigma_sq;  //fitting errors
  vnl_matrix<double> *_Si, *_pi, *_mi; //Scatter matrices and etc.
#if 0
  Histogram_ref _hist;//A histogram of the region intensity
#endif
};

#endif
