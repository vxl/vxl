// This is gel/vdgl/vdgl_digital_region.cxx
#include "vdgl_digital_region.h"
//:
// \file

#include <vcl_cmath.h> // for fabs and sqrt
#include <vcl_cassert.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_float_3.h>

#ifndef MAX_ROUNDOFF
#define MAX_ROUNDOFF .000025
#endif

#define near_zero(a) (vcl_fabs (a) < MAX_ROUNDOFF)

vsol_spatial_object_2d* vdgl_digital_region::clone() const
{
  return new vdgl_digital_region(*this);
}

//-------------------------------------------------------------------------
// Constructors
//
vdgl_digital_region::vdgl_digital_region(vdgl_digital_region const& r)
  : vsol_region_2d(r),
    npts_(0), pixel_size_(1.f), xp_(0), yp_(0), pix_(0),
    max_(0), min_((unsigned short)(-1)), xo_(0.f), yo_(0.f),
    io_(0.f), io_stdev_(0.0f), pix_index_(0),
    fit_valid_(false), scatter_matrix_valid_(false),
    X2_(0), Y2_(0), I2_(0), XY_(0), XI_(0), YI_(0), error_(0), sigma_sq_(0)
{
  for (unsigned int i = 0; i<r.Npix(); ++i)
    this->IncrementMeans(r.Xj()[i], r.Yj()[i], r.Ij()[i]);
  this->InitPixelArrays();
  for (unsigned int i = 0; i<r.Npix(); i++)
    this->InsertInPixelArrays(r.Xj()[i], r.Yj()[i], r.Ij()[i]);
  this->ComputeIntensityStdev();
}

vdgl_digital_region::vdgl_digital_region(int npts, const float* xp, const float* yp,
                                         const unsigned short *pix)
  : vsol_region_2d(),
    npts_(0), pixel_size_(1.f), xp_(0), yp_(0), pix_(0),
    max_(0), min_((unsigned short)(-1)), xo_(0.f), yo_(0.f),
    io_(0.f), io_stdev_(0.0f), pix_index_(0),
    fit_valid_(false), scatter_matrix_valid_(false),
    X2_(0), Y2_(0), I2_(0), XY_(0), XI_(0), YI_(0), error_(0), sigma_sq_(0)
{
  assert(npts > 0);
  for (int i = 0; i<npts; i++)
    this->IncrementMeans(xp[i], yp[i], pix[i]);
  this->InitPixelArrays();
  for (unsigned int i = 0; i<npts_; i++)
    this->InsertInPixelArrays(xp[i], yp[i], pix[i]);
  this->ComputeIntensityStdev();
}

//-------------------------------------------------------------------------
//
// Destructor.
//
vdgl_digital_region::~vdgl_digital_region()
{
  delete[] xp_;
  delete[] yp_;
  delete[] pix_;
}

//-------------------------------------------------------
//: The X pixel coordinate
float vdgl_digital_region::X() const
{
  if (pix_index_<0 || pix_index_ >= (int)npts_)
    return 0.0f;
  else
    return xp_[pix_index_];
}

//-------------------------------------------------------
//: The Y pixel coordinate
float vdgl_digital_region::Y() const
{
  if (pix_index_<0 || pix_index_ >= (int)npts_)
    return 0.0f;
  else
    return yp_[pix_index_];
}

//-------------------------------------------------------
//: The pixel Intensity
unsigned short vdgl_digital_region::I() const
{
  if (pix_index_<0 || pix_index_ >= (int)npts_)
    return (unsigned short)0;
  else
    return pix_[pix_index_];
}

//-------------------------------------------------------
//: Modify the X pixel coordinate
void vdgl_digital_region::set_X(float x)
{
  if (pix_index_<0 || pix_index_ >= (int)npts_)
    return;
  xp_[pix_index_]=x;
}

//-------------------------------------------------------
//: Modify the Y pixel coordinate
void vdgl_digital_region::set_Y(float y)
{
  if (pix_index_<0 || pix_index_ >= (int)npts_)
    return;
  yp_[pix_index_]=y;
}

//-------------------------------------------------------
//: Modify the pixel intensity
void vdgl_digital_region::set_I(unsigned short I)
{
  if (pix_index_<0 || pix_index_ >= (int)npts_)
    return;
  pix_[pix_index_]=I;
}

//--------------------------------------------------------
//:
//  Initialize the region for accepting a stream of pixels
//  through repeated calls to ::IncrementMeans(..)
void vdgl_digital_region::ResetPixelData()
{
  npts_ = 0;
  delete [] xp_; xp_ = NULL;
  delete [] yp_; yp_ = NULL;
  delete [] pix_; pix_ = NULL;
  xo_ = yo_ = io_ = io_stdev_=0.0f;
}

//-----------------------------------------------------------------
//:
//    Used to scan through a set of pixels and acquire mean values
//    for coordinates and intensity.  This approach is necessary to
//    accumulate scatter matrices which have zero mean. One scans
//    the region pixels twice. First to get the means and and number of
//    region pixels and then to insert the pixel data into fixed arrays
void vdgl_digital_region::IncrementMeans(float x, float y,
                                         unsigned short pix)
{
  ++npts_;
  xo_ += x;
  yo_ += y;
  io_ += pix;
}

//-----------------------------------------------------------------
//:
// Calculate the standard deviation of intensity for the region.
//
float vdgl_digital_region::ComputeIntensityStdev()
{
  io_stdev_ = 0.0f; // start from scratch each time
  float mean = this->Io(); // get the mean.
  for (unsigned int i=0; i<npts_; i++) {
    io_stdev_ += (pix_[i]-mean)*(pix_[i]-mean);
  }
  io_stdev_ = io_stdev_ * 1.0f/(npts_ - 1.0f);
  io_stdev_ = vcl_sqrt(io_stdev_);
  return io_stdev_;
}

//-----------------------------------------------------------------
//:
// Now we have the number of pixels so we can create the storage arrays.
void vdgl_digital_region::InitPixelArrays()
{
  assert(npts_ > 0);
  delete [] xp_;  xp_ = new float[npts_];
  delete [] yp_;  yp_ = new float[npts_];
  delete [] pix_; pix_ = new unsigned short[npts_];
  pix_index_ = 0;
  min_ = (unsigned short)(-1);
  max_ = 0;
}

//------------------------------------------------------------------------
//: Insert pixel data into the face arrays.
void vdgl_digital_region::InsertInPixelArrays(float x, float y,
                                              unsigned short pix)
{
  assert(npts_ > 0);
  if (pix_index_<0||pix_index_>(int)npts_) return;
  xp_[pix_index_] = x; yp_[pix_index_] = y;
  pix_[pix_index_] = pix;
  if (pix<min_) min_ = pix;
  if (pix>max_) max_ = pix;
  ++pix_index_;
}

float vdgl_digital_region::Xo() const
{
  assert(npts_ > 0);
  return xo_/npts_;
}

float vdgl_digital_region::Yo() const
{
  assert(npts_ > 0);
  return yo_/npts_;
}

float vdgl_digital_region::Io() const
{
  assert(npts_ > 0);
  return io_/npts_;
}

float vdgl_digital_region::Io_sd() const
{
  assert(npts_ > 0);
  return io_stdev_;
}

//: Individual scatter matrix elements
double vdgl_digital_region::X2() const
{
  if (!scatter_matrix_valid_)
    this->ComputeScatterMatrix();
  return X2_;
}

double vdgl_digital_region::Y2() const
{
  if (!scatter_matrix_valid_)
    this->ComputeScatterMatrix();
  return Y2_;
}

double vdgl_digital_region::XY() const
{
  if (!scatter_matrix_valid_)
    this->ComputeScatterMatrix();
  return XY_;
}

double vdgl_digital_region::I2() const
{
  if (!scatter_matrix_valid_)
    this->ComputeScatterMatrix();
  return I2_;
}

double vdgl_digital_region::XI() const
{
  if (!scatter_matrix_valid_)
    this->ComputeScatterMatrix();
  return XI_;
}

double vdgl_digital_region::YI() const
{
  if (!scatter_matrix_valid_)
    this->ComputeScatterMatrix();
  return YI_;
}

//------------------------------------------------------------
//: Compute the diameter from the scatter matrix
float vdgl_digital_region::Diameter() const
{
  // make sure the scatter matrix is valid
  if (!scatter_matrix_valid_)
    this->ComputeScatterMatrix();
  if (this->Npix() < 4)
    return 1.0f;
  // construct the lower right 2x2 matrix of S, s.
  vnl_matrix<double> s(2, 2, 0.0);
  for (int r = 1; r<=2; r++)
    for (int c = 1; c<=2; c++)
      s(r-1,c-1) = Si_(r,c);
  //Compute SVD of s to get diameter
  vnl_svd<double> svd(s);
  if (svd.rank()!=2)
    return float(vcl_sqrt(this->area()));
  //The factor of two is to estimate the extreme limit of the distribution
  double radius = 2*vcl_sqrt(vcl_fabs(svd.W(0))+ vcl_fabs(svd.W(1)));
  return float(2*radius);//diameter
}

//------------------------------------------------------------
//: Compute the aspect ratio from the scatter matrix
float vdgl_digital_region::AspectRatio() const
{
  // make sure the scatter matrix is valid
  if (!scatter_matrix_valid_)
    this->ComputeScatterMatrix();
  if (this->Npix() < 4)
    return 1.0f;
  // construct the lower right 2x2 matrix of S, s.
  vnl_matrix<double> s(2, 2, 0.0);
  for (int r = 1; r<=2; r++)
    for (int c = 1; c<=2; c++)
      s(r-1,c-1) = Si_(r,c);
  //Compute SVD of s to get aspect ratio
  vnl_svd<double> svd(s);
  if (svd.rank()!=2)
    return 1.0f;
  return (float)vcl_sqrt(svd.W(0)/svd.W(1));
}

#if 0 // function commented out
//------------------------------------------------------------
//: Compute the principal orientation of the region.
//   major_axis is a 2-d vector representing the orientation.
void vdgl_digital_region::PrincipalOrientation(vcl_vector<float>& major_axis)
{
  // make sure the scatter matrix is valid
  if (!scatter_matrix_valid_)
    this->ComputeScatterMatrix();
  if (this->Npix() < 4)
  {
    vcl_cout << "In vdgl_digital_region::PrincipalOrientation(..) Npts<4\n";
    major_axis.x()=1.0; major_axis.y()=0.0;
    return;
  }
  // construct the lower right 2x2 matrix of S, s.
  vnl_matrix<double> s(2, 2, 0.0);
  for (int r = 1; r<=2; r++)
    for (int c = 1; c<=2; c++)
      s(r-1,c-1) = Si_(r,c);
  //Compute SVD of s to get aspect ratio
  SVD svd(s);
  if (svd.rank()!=2)
  {
    vcl_cout << "In vdgl_digital_region::PrincipalOrientation(..) Insufficient rank\n";
    major_axis.x()=1.0; major_axis.y()=0.0;
    return;
  }
  vnl_matrix<double> v = svd.V();
  //2 sigma gives a good estimate of axis length (sigma = principal eigenvalue)
  double radius = 2*vcl_sqrt(vcl_fabs(svd.W(0)));
  major_axis.x()=float(v(0,0)*radius);
  major_axis.y()=float(v(1,0)*radius);
}
#endif // 0

double vdgl_digital_region::Ix() const
{
  if (!fit_valid_)
    this->DoPlaneFit();
  return Ix_;
}

double vdgl_digital_region::Iy() const
{
  if (!fit_valid_)
    this->DoPlaneFit();
  return Iy_;
}


//--------------------------------------------------------------
//: Update the scatter matrix elements.
// The means are subtracted from the input values before incrementing.
// Thus the scatter matrix is formed with a coordinate system at the origin.
void vdgl_digital_region::IncrByXYI(double x, double y, int intens) const
{
  fit_valid_=false;
  scatter_matrix_valid_=false;
  double dbx = x - this->Xo(), dby = y - this->Yo();
  double dint = (double)intens- this->Io();
  X2_ += dbx*dbx;  Y2_ += dby*dby;  I2_ += dint*dint;
  XY_ += dbx*dby;  XI_ += dbx*dint;  YI_ += dby*dint;
}

//-------------------------------------------
//: The scatter matrix (upper 3x3) is transformed to the origin by subtracting off the means.
void vdgl_digital_region::ComputeScatterMatrix() const
{
  if (!npts_)
  {
    vcl_cout << "In vdgl_digital_region::ComputeScatterMatrix() - no pixels\n";
    return;
  }
  X2_ = 0;  Y2_ = 0;  I2_ = 0;
  XY_ = 0;  XI_ = 0;  YI_ = 0;

  for (this->reset(); this->next();)
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
  Si_(0,0) = I2_/npts_;
  Si_(1,0) = Si_(0,1) = XI_/npts_;
  Si_(2,0) = Si_(0,2) = YI_/npts_;
  Si_(1,1) = X2_/npts_;
  Si_(2,1) = Si_(1,2) = XY_/npts_;
  Si_(2,2) = Y2_/npts_;
  scatter_matrix_valid_ = true;
}

//---------------------------------------------------------------
//: Computes the squared deviation from the sample mean
//
double vdgl_digital_region::ComputeSampleResidual() const
{
  if (!fit_valid_)
    this->DoPlaneFit();
  return Si_(0,0) + Si_(1,1) + Si_(2,2);
}

//: Solve for the fitted plane.
//  Closed form solution in terms of the scatter matrix.
void vdgl_digital_region::DoPlaneFit() const
{
  assert(npts_ >= 4);
  fit_valid_ = true;

  if (!scatter_matrix_valid_)
    this->ComputeScatterMatrix();

  double den = Si_(1,1)*Si_(2,2)
             - Si_(1,2)*Si_(2,1);
  if (near_zero(den))
  {
    vcl_cout << "In vdgl_digital_region::SolveForPlane(..) determinant near zero\n";
    Ix_ = Iy_ = 0;
    error_ = Si_(0,0);
    sigma_sq_ = Si_(0,0) + Si_(1,1) + Si_(2,2);
    return;
  }
  double adet = Si_(0,1)*Si_(2,2)
              - Si_(0,2)*Si_(2,1);
  double bdet = Si_(0,2)*Si_(1,1)
              - Si_(0,1)*Si_(1,2);
  Ix_ = adet/den;
  Iy_ = bdet/den;
  error_ = Si_(0,0) - 2*Ix_*Si_(0,1) - 2*Iy_*Si_(0,2)
         + Ix_*Ix_*Si_(1,1) + 2*Ix_*Iy_*Si_(1,2) + Iy_*Iy_*Si_(2,2);
  sigma_sq_ =  this->ComputeSampleResidual();
}


void vdgl_digital_region::PrintFit() const
{
  if (!fit_valid_)
    this->DoPlaneFit();
  vcl_cout << "IntensityFit(In Plane Coordinates): "
           << "Number of Points =" <<  npts_ << vcl_endl
           << "Scatter Matrix:\n"
           << "X2 Y2 I2   " << X2() << ' ' << Y2() << ' ' << I2() << vcl_endl
           << "XY XI YI = " << XY() << ' ' << XI() << ' ' << YI() << vcl_endl
           << "Xo Yo Io   " << Xo() << ' ' << Yo() << ' ' << Io() << vcl_endl
           << "fitted Plane:\n"
           << "di/dx " << this->Ix() << vcl_endl
           << "di/dy " << this->Iy() << vcl_endl
           << "sample variance: " << this->Var()<< vcl_endl
           << "squared cost: " << error_ << vcl_endl
           << "average cost: " << vcl_sqrt(error_) << vcl_endl << vcl_endl;
}

#if 0
//-----------------------------------------------------------
//: Return a package of fitted coefficients
//
IntensityCoef_ref vdgl_digital_region::GetIntCoef() const
{
  if (!fit_valid_)
    this->DoPlaneFit();
  return new IntensityCoef(this->Npix(), float(this->Var()),
                           float(this->Io()), float(this->Ix()),
                           float(this->Iy()));
}
#endif

//-------------------------------------------------------
//: The Residual Intensity
float vdgl_digital_region::Ir() const
{
  if (pix_index_<0 || pix_index_ >= (int)npts_)
    return 0.0f;
  if (npts_<4)
    return 0.0f;

  if (!fit_valid_)
  {
    int initial_pix_index = pix_index_;//Save the current pix_index_ state
    this->DoPlaneFit();
    pix_index_ = initial_pix_index;//Restore the pix_index_ state
  }
  float val = float(this->I());
  float io = this->Io(),
        ix = float(this->Ix()), x = this->X(), xo = this->Xo(),
        iy = float(this->Iy()), y = this->Y(), yo = this->Yo();
  float plane_int = io + ix*(x - xo) + iy*(y-yo);

  return val - plane_int;
}

bool vdgl_digital_region::transform(vnl_float_3x3 const& t)
{
  //Transform the Pixels
  if (npts_ == 0)
    return false;
  for (unsigned int i=0; i<npts_; i++)
  {
    vnl_float_3 p = t * vnl_float_3(xp_[i], yp_[i], 1.0f);
    xp_[i]=p[0]/p[2];
    yp_[i]=p[1]/p[2];
  }
  //Transform the mean pixel position
  vnl_float_3 m = t * vnl_float_3(this->Xo(), this->Yo(), 1.0f);
  xo_ = m[0]/m[2]*npts_;
  yo_ = m[1]/m[2]*npts_;
  fit_valid_ = false;
  scatter_matrix_valid_=false;
  return true;
}

//--------------------------------------------------------
//: Compute the histogram
vcl_vector<unsigned int> vdgl_digital_region::histogram(int nbins)
{
  assert(nbins > 0);
  if (nbins == 1) return vcl_vector<unsigned int>(1, npts_);
  vcl_vector<unsigned int> hist(nbins, 0U);
  if (npts_ == 0) return hist;
  float res = max_-min_, step = res/nbins + 1e-6f;
  for (unsigned int i =0; i<npts_; ++i)
    ++hist[int((pix_[i]-min_)/step)];
  return hist;
}

vcl_vector<unsigned int> vdgl_digital_region::residual_histogram(int nbins,
                                                                 float* min,
                                                                 float* max)
{
  assert(nbins > 0);
  if (nbins == 1) return vcl_vector<unsigned int>(1, npts_);
  vcl_vector<unsigned int> hist(nbins, 0U);
  if (npts_ == 0) return hist;
  this->reset();
  float mini = this->Ir(), maxi=mini;
  while (this->next())
  {
    float ir = this->Ir();
    if (ir<mini) mini = ir;
    if (ir>maxi) maxi = ir;
  }
  if (min) *min = mini;
  if (max) *max = maxi;
  float res = maxi-mini, step = res/nbins + 1e-6f;
  for (this->reset(); this->next(); )
    ++hist[int((this->Ir()-mini)/step)];
  return hist;
}
