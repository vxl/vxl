#if 0
#include <math/math.h>
#include <Numerics/SVD.h>
#include <Basics/Histogram.h>
#include <SpatialBasics/tolerance.h>
#include <DigitalGeometry/IntensityCoef.h>
#include <DigitalGeometry/DigitalRegion.h>
#endif

#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_svd.h>
#include "vdgl_digital_region.h"


#ifndef MAX_ROUNDOFF
#define MAX_ROUNDOFF .000025
#define ANGLE_FUZZ .0003926754
#endif

#define near_zero(a) (fabs (a) < MAX_ROUNDOFF)

//-----------------------------------------------------------
//  -- Initialize region data
void vdgl_digital_region::init()
{
  _npts = 0;   
  _pixel_size=1.0;
  _max = 0;
  _min = (unsigned short) -1;
  _xp = NULL; _yp = NULL; _zp = NULL; _pix = NULL;
  _xo = 0; _yo = 0; _zo = 0; _io = 0;
  _X2 = 0;  _Y2 = 0;  _I2 = 0;
  _XY = 0;  _XI = 0;  _YI = 0;
  _Xi = 0; _Yi = 0; _Ii = 0;
  _error = 0; _sigma_sq = 0;
  _fit_valid = false;
  _scatter_matrix_valid = false;
  _Si = new vnl_matrix<double>(3, 3, 0.0); 
  _pi = new vnl_matrix<double>(3, 1, 0.0);        
  _mi = new vnl_matrix<double>(3, 3, 0.0);        
#if 0
  _hist = NULL;
#endif
}
//-------------------------------------------------------------------------
// -- Constructors
//
vdgl_digital_region::vdgl_digital_region()

{
  this->init();
}
vdgl_digital_region::vdgl_digital_region(const int npts, const float* xp, const float* yp,
                             const unsigned short *pix)
{
  this->init();
  for(int i = 0; i<npts; i++)
    this->IncrementMeans(xp[i], yp[i], pix[i]);
  this->InitPixelArrays();
  for(int i = 0; i<_npts; i++)
    this->InsertInPixelArrays(xp[i], yp[i], pix[i]);
}
vdgl_digital_region::vdgl_digital_region(const int npts, const float* xp, const float* yp,
                             const float* zp, const unsigned short *pix)
{
  this->init();
  for(int i = 0; i<npts; i++)
    this->IncrementMeans(xp[i], yp[i], zp[i], pix[i]);
  this->InitPixelArrays();
  for(int i = 0; i<_npts; i++)
    this->InsertInPixelArrays(xp[i], yp[i], zp[i], pix[i]);
}
//-------------------------------------------------------------------------
//
// -- Destructor.
//
vdgl_digital_region::~vdgl_digital_region()
{
  delete [] _xp;
  delete [] _yp;
  delete [] _zp;
  delete [] _pix;
  delete(_pi);
  delete(_Si);
  delete(_mi);
}

#if 0
//--------------------------------------------------------
// -- The Histogram is lazily evaluated. If a Histogram is present,
//    it is not recomputed unless force is true;
Histogram_ref vdgl_digital_region::GetHistogram(bool force)
{
  if(!_npts) return NULL;
  int res = int(_max-_min);
  if(_hist&&!force)
    return _hist;
  Histogram* h;
  if (res)
    h = new Histogram(res, _min, _max);
  else
    h = new Histogram(1, _min, _max);
  for(int i =0; i<_npts; i++)
    h->UpCount(_pix[i]);
  _hist = h;
  return h;
}
#endif

//------------------------------------------------------------
// -- reset the iterator for accessing region pixels
//
void vdgl_digital_region::reset()
{
  _pix_index = -1;
}

//------------------------------------------------------------
// -- check if iterator is finished - colin found a major bug here
//    we were skipping pixel[0] for the entire existence of this class
//
bool vdgl_digital_region::next()
{
  _pix_index++;
  return _pix_index<_npts;
}

//-------------------------------------------------------
// -- The X pixel coordinate
float vdgl_digital_region::X()
{
  if(_pix_index<0)
    return 0.0;
  return _xp[_pix_index];
}

//-------------------------------------------------------
// -- The Y pixel coordinate
float vdgl_digital_region::Y()
{
  if(_pix_index<0)
    return 0.0;
  return _yp[_pix_index];
}

//-------------------------------------------------------
// -- The Z pixel coordinate
float vdgl_digital_region::Z()
{
  if(_pix_index<0)
    return 0.0;
  return _zp[_pix_index];
}

//-------------------------------------------------------
// -- The pixel Intensity
unsigned short vdgl_digital_region::I()
{
  if(_pix_index<0)
    return 0;
  return _pix[_pix_index];
}
//--------------------------------------------------------
// -- this method initializes the region for accepting a 
//    stream of pixels through repeated calls to ::IncrementMeans(..)
void vdgl_digital_region::ResetPixelData()
{
  _npts = 0;
  delete [] _xp;
  delete [] _yp;
  delete [] _zp;
  delete [] _pix;
  _xp = NULL; _yp = NULL; _zp = NULL; _pix = NULL;
  _xo = 0; _yo = 0; _zo = 0; _io = 0;
}
//---------------------------------------------------------
// -- The 2-d version
void vdgl_digital_region::IncrementMeans(float x, float y, unsigned short pix)
{
  this->IncrementMeans(x, y, 0, pix);
}
//-----------------------------------------------------------------
// -- Used to scan through a set of pixels and acquire mean values
//    for coordinates and intensity.  This approach is neccessary to
//    accumulate scatter matrices which have zero mean. One scans
//    the region pixels twice. First to get the means and and number of
//    region pixels and then to insert the pixel data into fixed arrays
void vdgl_digital_region::IncrementMeans(float x, float y, float z, 
                                   unsigned short pix)
{
  _npts++;
  _xo += x;
  _yo += y;
  _zo += z;
  _io += pix;
}
//-----------------------------------------------------------------
// -- Now we have the number of pixels so we can create the storage
//    arrays.
void vdgl_digital_region::InitPixelArrays()
{
  delete [] _xp;
  delete [] _yp;
  delete [] _zp;
  delete [] _pix;
  if(!_npts)
    {
      vcl_cout << "In vdgl_digital_region::ResetPixelArrays() - no pixels"
	   << vcl_endl;
      return;
    }
  _xp = new float[_npts];
  _yp = new float[_npts];
  _zp = new float[_npts];

  _pix = new unsigned short[_npts];
  _pix_index = 0;
  _min = (unsigned short) -1;
  _max = 0;
}
//--------------------------------------------------------------------
// -- the 2-d version
void vdgl_digital_region::InsertInPixelArrays(float x, float y, 
                                        unsigned short pix)
{
  this->InsertInPixelArrays(x, y, 0, pix);
}

//------------------------------------------------------------------------
// -- Insert pixel data into the face arrays.
//
void vdgl_digital_region::InsertInPixelArrays(float x, float y, float z, 
                                        unsigned short pix)
	
{
  if(!_npts)
    {
      vcl_cout << "In vdgl_digital_region::InsertIntoPixelArrays() - shouldn't happen"
	   << vcl_endl;
      return;
    }
  if(_pix_index<0||_pix_index>_npts) return;      
  _xp[_pix_index] = x; _yp[_pix_index] = y;
  _zp[_pix_index] = z; _pix[_pix_index] = pix;
  if(pix<_min)
    _min = pix;
  if(pix>_max)
    _max = pix;
  _pix_index++;
}

float vdgl_digital_region::Xo()
{  
  if(_npts)
    return _xo/_npts;
  else
    return 0;
}

float vdgl_digital_region::Yo()
{  
  if(_npts)
    return _yo/_npts;
  else
    return 0;
}

float vdgl_digital_region::Zo()
{  
  if(_npts)
    return _zo/_npts;
  else
    return 0;
}

float vdgl_digital_region::Io()
{  
  if(_npts)
    return _io/_npts;
  else
    return 0;
}
// -- Individual scatter matrix elements
double vdgl_digital_region::X2()
{
  if(!_scatter_matrix_valid)
    this->ComputeScatterMatrix();
  return _X2;
}
double vdgl_digital_region::Y2()
{
  if(!_scatter_matrix_valid)
    this->ComputeScatterMatrix();
  return _Y2;
}
double vdgl_digital_region::XY()
{
  if(!_scatter_matrix_valid)
    this->ComputeScatterMatrix();
  return _XY;
}
double vdgl_digital_region::I2()
{
  if(!_scatter_matrix_valid)
    this->ComputeScatterMatrix();
  return _I2;
}
double vdgl_digital_region::XI()
{
if(!_scatter_matrix_valid)
    this->ComputeScatterMatrix();
 return _XI;
}
double vdgl_digital_region::YI()
{
if(!_scatter_matrix_valid)
    this->ComputeScatterMatrix();
 return _YI;
}
double vdgl_digital_region::Xi()
{
if(!_scatter_matrix_valid)
    this->ComputeScatterMatrix();
 return _Xi;
}
double vdgl_digital_region::Yi()
{
if(!_scatter_matrix_valid)
    this->ComputeScatterMatrix();
 return _Yi;
}
double vdgl_digital_region::Ii()
{
if(!_scatter_matrix_valid)
  this->ComputeScatterMatrix();
 return _Ii;
}
//------------------------------------------------------------
// --Compute the diameter from the scatter matrix
float vdgl_digital_region::Diameter()
{
  // make sure the scatter matrix is valid
  if(!_scatter_matrix_valid)
    this->ComputeScatterMatrix();
  if(this->Npix()<4)
    return 1.0;
  // construct the lower right 2x2 matrix of S, s.
  vnl_matrix<double> s(2, 2, 0.0);
  for(int r = 1; r<=2; r++)
    for(int c = 1; c<=2; c++)
      s(r-1,c-1) = (*_Si)(r,c);
  //Compute SVD of s to get diameter
  vnl_svd<double> svd(s);
  if(svd.rank()!=2)
    return float(sqrt(this->area()));
  //The factor of two is to estimate the extreme limit of the distribution
  double r = 2*sqrt(fabs(svd.W(0))+ fabs(svd.W(1)));
  return float(2*r);//diameter
}

//------------------------------------------------------------
// --Compute the aspect ratio from the scatter matrix
float vdgl_digital_region::AspectRatio()
{
  // make sure the scatter matrix is valid
  if(!_scatter_matrix_valid)
    this->ComputeScatterMatrix();
  if(this->Npix()<4)
    return 1.0;
  // construct the lower right 2x2 matrix of S, s.
  vnl_matrix<double> s(2, 2, 0.0);
  for(int r = 1; r<=2; r++)
    for(int c = 1; c<=2; c++)
      s(r-1,c-1) = (*_Si)(r,c);
  //Compute SVD of s to get aspect ratio
  vnl_svd<double> svd(s);
  if(svd.rank()!=2)
    return 1.0;
  return (float)sqrt(svd.W(0)/svd.W(1));
}

#if 0
//------------------------------------------------------------
// --Compute the principal orientation of the region. major_axis is
//   a 2-d vector representing the orientation.
void vdgl_digital_region::PrincipalOrientation(vcl_vector<float>& major_axis)
{
  // make sure the scatter matrix is valid
  if(!_scatter_matrix_valid)
    this->ComputeScatterMatrix();
  if(this->Npix()<4)
    {
      vcl_cout << "In vdgl_digital_region::PrincipalOrientation(..) Npts<4"
           << vcl_endl;
      major_axis.x()=1.0; major_axis.y()=0.0;
      return;
    }
  // construct the lower right 2x2 matrix of S, s.
  vnl_matrix<double> s(2, 2, 0.0);
  for(int r = 1; r<=2; r++)
    for(int c = 1; c<=2; c++)
      s(r-1,c-1) = (*_Si)(r,c);
  //Compute SVD of s to get aspect ratio
  SVD svd(s);
  if(svd.rank()!=2)
    {
      vcl_cout << "In vdgl_digital_region::PrincipalOrientation(..) Insufficient rank"
           << vcl_endl;
      major_axis.x()=1.0; major_axis.y()=0.0;
      return;
    }
  vnl_matrix<double> v = svd.V();
  //2 sigma gives a good estimate of axis length (sigma = principal eigenvalue)
  double r = 2*sqrt(fabs(svd.W(0)));
  major_axis.x()=float(v(0,0)*r);
  major_axis.y()=float(v(1,0)*r);
}

#endif

double vdgl_digital_region::Ix()
{
  if(!_fit_valid)
    this->DoPlaneFit();
  return _Ix;
}
double vdgl_digital_region::Iy()
{
  if(!_fit_valid)
    this->DoPlaneFit();
  return _Iy;
}


//--------------------------------------------------------------
// -- Update the scatter matrix elements.  The means are subtracted
//    from the input values before incrementing. Thus the scatter
//    matrix is formed with a coordinate system at the origin.
void vdgl_digital_region::IncrByXYI(double x, double y, int intens)
{ 
  _fit_valid=false;
  _scatter_matrix_valid=false;
  double dbx = x - this->Xo(), dby = y - this->Yo();
  double dint = (double)intens- this->Io();
  _X2 += dbx*dbx;  _Y2 += dby*dby;  _I2 += dint*dint;
  _XY += dbx*dby;  _XI += dbx*dint;  _YI += dby*dint;
}

//-------------------------------------------
// -- The scatter matrix (upper 3x3) is transformed to the origin
//    by subtracting off the means.
void vdgl_digital_region::ComputeScatterMatrix()
{
  if(!_npts)
    {
      vcl_cout << "In vdgl_digital_region::ComputeScatterMatrix() - no pixels" 
	   << vcl_endl;
      return;
    }
  _X2 = 0;  _Y2 = 0;  _I2 = 0;
  _XY = 0;  _XI = 0;  _YI = 0;

  for(this->reset(); this->next();)
    this->IncrByXYI(this->X(), this->Y(), this->I());


  //The Scatter Matrix
  //
  //   -             -
  //  | I^2  XI   YI  |
  //  |               |
  //  | XI   X^2  XY  |
  //  |               |
  //  | YI   XY   Y^2 |
  //   -             -
  //
  _Si->put(0,0, _I2/_npts);
  _Si->put(0,1, _XI/_npts);
  _Si->put(0,2, _YI/_npts);
  _Si->put(1,1, _X2/_npts);
  _Si->put(1,2, _XY/_npts);
  _Si->put(2,2, _Y2/_npts);
  _Si->put(1,0, _Si->get(0,1));
  _Si->put(2,0, _Si->get(0,2));
  _Si->put(2,1, _Si->get(1,2));
  _scatter_matrix_valid = true;
}

//--------------------------------------------------------------
// --Solve for the fitted plane. Closed form solution in terms of
//   the scatter matrix.
void vdgl_digital_region::SolveForPlane()
{
  _fit_valid = true;
  if(_npts < 4)
    {
      vcl_cout << " In vdgl_digital_region::SolveForPlane(..) too few points" 
	   << vcl_endl;
      _Ix = 0;
      _Iy = 0;
      _pi->put(0,0, 1.0);   _pi->put(1,0, -_Ix);   _pi->put(2,0, -_Iy);
      return;
    }
  double den = _Si->get(1,1)*_Si->get(2,2);
  den -= _Si->get(1,2)*_Si->get(1,2);
  if(near_zero(den))
    {
      vcl_cout << "In vdgl_digital_region::SolveForPlane(..) determinant near zero" 
           << vcl_endl;
      _Ix = 0;
      _Iy = 0;
      _pi->put(0,0, 1.0);   _pi->put(1,0, -_Ix);   _pi->put(2,0, -_Iy);
      return;
    }
  double adet = _Si->get(0,1)*_Si->get(2,2);
  adet -= _Si->get(0,2)*_Si->get(1,2);
  double bdet = _Si->get(1,1)*_Si->get(0,2);
  bdet -= _Si->get(0,1)*_Si->get(1,2);
  _Ix = adet/den;
  _Iy = bdet/den;
  _pi->put(0,0, 1.0);   _pi->put(1,0, -_Ix);   _pi->put(2,0, -_Iy);
}

//---------------------------------------------------------------
// -- Computes the square residual error using the scatter matrix
//
double vdgl_digital_region::ComputeResidual(vnl_matrix<double>& pvect)
{
  if(!_fit_valid)
    this->DoPlaneFit();
  vnl_matrix<double> residual(1,1);
  residual = pvect.transpose() * (*_Si) * (pvect);
  return residual.get(0,0);
}

//---------------------------------------------------------------
// -- Computes the squared deviation from the sample mean
//
double vdgl_digital_region::ComputeSampleResidual()
{
  if(!_fit_valid)
    this->DoPlaneFit();
  return _Si->get(0,0) + _Si->get(1,1) + _Si->get(2,2);
}

void vdgl_digital_region::DoPlaneFit()
{
  if(!_scatter_matrix_valid)
    this->ComputeScatterMatrix();
  
  this->SolveForPlane();
  _error =  this->ComputeResidual(*_pi);
  _sigma_sq =  this->ComputeSampleResidual();
}



void vdgl_digital_region::PrintFit()
{
  if(!_fit_valid)
    this->DoPlaneFit();
  vcl_cout << "IntensityFit(In Plane Coordinates): "
       << "Number of Points =" <<  _npts << vcl_endl
       << "Scatter Matrix: " << vcl_endl
       << "X2 Y2 I2 " << this->X2() << " " << this->Y2() 
       << " " << this->I2() << vcl_endl
       << "XY XI YI " << this->XY() << " " << this->XI() 
       << " " << this->YI() << vcl_endl
       << "Xo Yo Io " << this->Xo() << " " << this->Yo()
       << " " << this->Io() << vcl_endl
       << "fitted Plane: " << vcl_endl
       << "di/dx " << this->Ix() << vcl_endl
       << "di/dy " << this->Iy() << vcl_endl
       << "sample variance: " << this->Var()<< vcl_endl
       << "squared cost: " << _error << vcl_endl
       << "average cost: " << sqrt(_error) << vcl_endl << vcl_endl;
}

#if 0
//-----------------------------------------------------------
//  -- Return a package of fitted coeficients
//
IntensityCoef_ref vdgl_digital_region::GetIntCoef()
{
  if(!_fit_valid)
    this->DoPlaneFit();
  return new IntensityCoef(this->Npix(), float(this->Var()), 
                           float(this->Io()), float(this->Ix()),
                           float(this->Iy()));
}
#endif

//-------------------------------------------------------
// -- The Residual Intensity
float vdgl_digital_region::Ir()
{
  if(_pix_index<0)
    return 0.0;

  if(!_fit_valid)
    {
      int initial_pix_index = _pix_index;//Save the current _pix_index state
      this->DoPlaneFit();
      _pix_index = initial_pix_index;//Restore the _pix_index state
    }
  float val = float(this->I());
  float io = float(this->Io()), ix = float(this->Ix()),
    x = float(this->X()), xo = float(this->Xo());
  float iy = float(this->Iy()), y = float(this->Y()), yo = float(this->Yo());
  float plane_int = io + ix*(x - xo) + iy*(y-yo);
  float res = (val - plane_int);
  return res;
}

#if 0
Histogram_ref vdgl_digital_region::GetResidualHistogram()
{
  if(!_npts) return NULL;
  float min = vnl_math::maxfloat, max = -vnl_math::maxfloat;
  for(this->reset(); this->next();)
    {
      float ir = (float)this->Ir();
      if(ir<min)
	min = ir;
      if(ir>max)
	max = ir;
    }
  int res = int(max-min);
  Histogram* h = new Histogram(res, min, max);
  for(this->reset(); this->next();)
    h->UpCount(this->Ir());
  return h;
}

bool vdgl_digital_region::Transform(CoolTransform const& t)
{
  //Transform the Pixels
  int n = _npts;
  if(!_npts)
    return false;
  for(int i=0; i<n; i++)
    {
      vcl_vector<float> p(_xp[i], _yp[i], _zp[i]);
      vcl_vector<float>pt = p*t;
      _xp[i]=pt.x();
      _yp[i]=pt.y();
      _zp[i]=pt.z();
    }
  //Transform the mean pixel position
  vcl_vector<float> m(this->Xo(), this->Yo(), this->Zo());
  vcl_vector<float>mt = m*t;
  _xo = mt.x()*n;
  _yo = mt.y()*n;
  _zo = mt.z()*n;
  _fit_valid = false;
  _scatter_matrix_valid=false;
  return true;
}
#endif
