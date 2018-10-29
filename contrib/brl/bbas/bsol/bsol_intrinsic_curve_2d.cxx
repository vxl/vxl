// This is brl/bbas/bsol/bsol_intrinsic_curve_2d.cxx
#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include "bsol_intrinsic_curve_2d.h"
//:
// \file
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_digital_curve_2d.h>
#include <vcl_cassert.h>
#include <vcl_compiler.h>
#include <vnl/vnl_math.h>


//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Default Constructor
//---------------------------------------------------------------------------
bsol_intrinsic_curve_2d::bsol_intrinsic_curve_2d()
{
  storage_=new std::vector<vsol_point_2d_sptr>();
  isOpen_=true;
}

//---------------------------------------------------------------------------
//: Constructor from a std::vector of points
//---------------------------------------------------------------------------

bsol_intrinsic_curve_2d::bsol_intrinsic_curve_2d(const std::vector<vsol_point_2d_sptr> &new_vertices)
{
  storage_=new std::vector<vsol_point_2d_sptr>(new_vertices);
  isOpen_=true;
}

//: Constructor from a vsol_polyline_2d_sptr
bsol_intrinsic_curve_2d::bsol_intrinsic_curve_2d(const vsol_polyline_2d_sptr poly)
{
  storage_ = new std::vector<vsol_point_2d_sptr>();
  for (unsigned i = 0; i < poly->size(); i++)
    storage_->push_back(poly->vertex(i));
  isOpen_=true;
}
bsol_intrinsic_curve_2d::bsol_intrinsic_curve_2d(const vsol_polygon_2d_sptr poly)
{
  storage_ = new std::vector<vsol_point_2d_sptr>();
  for (unsigned i = 0; i < poly->size(); i++)
    storage_->push_back(poly->vertex(i));
  isOpen_=false;
}


//---------------------------------------------------------------------------
// Copy constructor
//---------------------------------------------------------------------------
bsol_intrinsic_curve_2d::bsol_intrinsic_curve_2d(const bsol_intrinsic_curve_2d &other)
  : vsol_curve_2d(other)
{
  storage_=new std::vector<vsol_point_2d_sptr>(*other.storage_);
  for (unsigned int i=0;i<storage_->size();++i)
    (*storage_)[i]=new vsol_point_2d(*((*other.storage_)[i]));

  isOpen_ = other.isOpen_;
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
bsol_intrinsic_curve_2d::~bsol_intrinsic_curve_2d()
{
  delete storage_;
}

//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_2d* bsol_intrinsic_curve_2d::clone(void) const
{
  return new bsol_intrinsic_curve_2d(*this);
}

//***************************************************************************
// Comparison
//***************************************************************************

//---------------------------------------------------------------------------
//: Has `this' the same points than `other' in the same order ?
//---------------------------------------------------------------------------
bool bsol_intrinsic_curve_2d::operator==(const bsol_intrinsic_curve_2d &other) const
{
  bool result = (this==&other);

  if (!result)
  {
    result = (storage_->size()==other.storage_->size());
    if (result)
    {
      vsol_point_2d_sptr p=(*storage_)[0];

      unsigned int i=0;
      for (result=false;i<storage_->size()&&!result;++i)
        result = (*p==*(*other.storage_)[i]);
      if (result)
      {
        for (int j=1;j<size()&&result;++i,++j)
        {
          if (i>=storage_->size()) i=0;
          result = ((*storage_)[i]==(*storage_)[j]);
        }
      }
    }
  }
  return result;
}

//: spatial object equality

bool bsol_intrinsic_curve_2d::operator==(const vsol_spatial_object_2d& obj) const
{
  return obj.is_a() == "bsol_intrinsic_curve_2d" &&
    operator==(static_cast<bsol_intrinsic_curve_2d const&>(obj));
}

//***************************************************************************
// Internal status setting functions
//***************************************************************************
void bsol_intrinsic_curve_2d::clear(void)
{
  s_.clear();
  arcLength_.clear();
  normArcLength_.clear();
  length_ = 0;
  curvature_.clear();
  angle_.clear();
  totalCurvature_ = 0;
  totalAngleChange_ = 0;
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the curvature of the vertex `i'
//---------------------------------------------------------------------------
double bsol_intrinsic_curve_2d::curvature(const int i) const
{
  assert(valid_index(i));
  return curvature_[i];
}

//---------------------------------------------------------------------------
//: Return the angle of the vertex `i'
//---------------------------------------------------------------------------
double bsol_intrinsic_curve_2d::angle(const int i) const
{
  assert(valid_index(i));
  return angle_[i];
}

//---------------------------------------------------------------------------
//: Compute the bounding box of `this'
//---------------------------------------------------------------------------
void bsol_intrinsic_curve_2d::compute_bounding_box(void) const
{
  set_bounding_box((*storage_)[0]->x(), (*storage_)[0]->y());
  for (unsigned int i=1;i<storage_->size();++i)
    add_to_bounding_box((*storage_)[i]->x(), (*storage_)[i]->y());
}


//***************************************************************************
// Status setting
//***************************************************************************

//---------------------------------------------------------------------------
//: Set the first point of the curve
// Require: in(new_p0)
//---------------------------------------------------------------------------
void bsol_intrinsic_curve_2d::set_p0(const vsol_point_2d_sptr &new_p0)
{
  p0_=new_p0;
  storage_->push_back(p0_);
}

//---------------------------------------------------------------------------
//: Set the last point of the curve
// Require: in(new_p1)
//---------------------------------------------------------------------------
void bsol_intrinsic_curve_2d::set_p1(const vsol_point_2d_sptr &new_p1)
{
  p1_=new_p1;
  storage_->push_back(p0_);
}

//---------------------------------------------------------------------------
//: Add another point to the curve
//---------------------------------------------------------------------------
void bsol_intrinsic_curve_2d::add_vertex(const vsol_point_2d_sptr &new_p, bool bRecomputeProperties)
{
  storage_->push_back(new_p);
  if (bRecomputeProperties) computeProperties();
}

//---------------------------------------------------------------------------
//: Remove one vertex from the intrinsic curve
//---------------------------------------------------------------------------
void bsol_intrinsic_curve_2d::remove_vertex(const int i, bool bRecomputeProperties)
{
  assert (valid_index(i));
  storage_->erase(storage_->begin() + i);
  if (bRecomputeProperties) computeProperties();
}

void bsol_intrinsic_curve_2d::modify_vertex(const int i, double x, double y, bool bRecomputeProperties)
{
  assert (valid_index(i));
  (*storage_)[i]->set_x(x);
  (*storage_)[i]->set_y(y);
  if (bRecomputeProperties) computeProperties();
}

//: insert into i-1
void bsol_intrinsic_curve_2d::insert_vertex(const int i, double x, double y, bool bRecomputeProperties)
{
  assert (valid_index(i));
  vsol_point_2d_sptr pt = new vsol_point_2d(x,y);
  auto it = storage_->begin();
  it += i;
  storage_->insert(it, pt);
  if (bRecomputeProperties) computeProperties();
}

void bsol_intrinsic_curve_2d::readCONFromFile(std::string fileName)
{
  double x, y;
  char buffer[2000];
  int nPoints;

  //clear the existing curve data
  if (size() !=0)
    clear();

  //1)If file open fails, return.
  std::ifstream fp(fileName.c_str(), std::ios::in);
  if (!fp) {
    std::cout<<" : Unable to Open "<<fileName<<std::endl;
    return;
  }

  //2)Read in file header.
  fp.getline(buffer,2000); //CONTOUR

  //fp.getline(buffer,2000); //OPEN/CLOSE
  //char openFlag[2000];
  std::string openFlag;
  //fp.getline(openFlag,2000);
  std::getline(fp, openFlag);
  //if (!vcl_Strncmp(openFlag,"OPEN",4))
  if (openFlag.find("OPEN",0) != std::string::npos)
    isOpen_ = true;
  //else if (!vcl_Strncmp(openFlag,"CLOSE",5))
  else if (openFlag.find("CLOSE",0) != std::string::npos)
    isOpen_ = false;
  else{
    std::cerr << "Invalid File " << fileName.c_str() << '\n'
             << "Should be OPEN/CLOSE " << openFlag << '\n';
    return;
  }

  fp >> nPoints;
  std::cout << "Number of Points from Contour: " << nPoints
           << "\nContour flag is "<< isOpen_ << " (1 for open, 0 for close)\n";

  for (int i=0;i<nPoints;i++) {
    fp >> x >> y;
    add_vertex(x,y);
  }

  fp.close();
  computeProperties();
}


//: Compute arc length and normalized arc length
void bsol_intrinsic_curve_2d::computeArcLength()
{
  //Compute arc length
  arcLength_.clear();
  s_.clear();
  length_=0;
  arcLength_.push_back(0.0);
  s_.push_back(0.0);

  double px=(*storage_)[0]->x();
  double py=(*storage_)[0]->y();
  for (int i=1;i<size();i++)
  {
    double cx=(*storage_)[i]->x();
    double cy=(*storage_)[i]->y();
    double dL = vnl_math::hypot(cx-px,cy-py);
    length_ += dL;
    arcLength_.push_back(length_);
    s_.push_back(dL);
    px=cx;
    py=cy;
  }

  assert (s_.size()==arcLength_.size());

// original code of TBS in /vision/projects/kimia/curve-matching/CurveMatch/CODE/CODE-IRIX6.5/Matching-Tek/c
// treats the starting point in no special way for closed curves, its arclength is simply 0 which is correct
#if 0 // commented out
  //Deal with the last point for a closed curve separately.
  if (!isOpen_)
  {
    px=(*storage_)[size()-1]->x();
    py=(*storage_)[size()-1]->y();
    double cx=(*storage_)[0]->x();
    double cy=(*storage_)[0]->y();
    double dL=std::sqrt(std::pow(cx-px,2)+std::pow(cy-py,2));
    length_ += dL;
    arcLength_[0]=length_;
  }
#endif // 0

  //Compute normalized arc length
  normArcLength_.clear();
  for (int i=0;i<size();i++)
    normArcLength_.push_back(arcLength_[i]/length_);

#ifdef DEBUG
  std::cout << "Norm arc length values:\n";
  for (int i = 0; i<size(); i++)
    std::cout << "normArcLength_[" << i << "]: " << normArcLength_[i] << std::endl;

  std::cout << "arc length values:\n";
  for (int i = 0; i<size(); i++)
    std::cout << "arcLength_[" << i << "]: " << arcLength_[i] << std::endl;
#endif
}

//: Compute curvature. Assumes derivative computation has been done.
void bsol_intrinsic_curve_2d::computeCurvatures()
{
  //Compute curvature
  curvature_.clear();
  curvature_.push_back(0.0);
  totalCurvature_=0.0;

  for (int i=1;i<size();i++)
  {
    double pdx=dx_[i-1];
    double pdy=dy_[i-1];
    double cdx=dx_[i];
    double cdy=dy_[i];
    double dL=arcLength_[i]-arcLength_[i-1];
    double d2x=0, d2y=0;
    if (dL > ZERO_TOLERANCE) {
      d2x=(cdx-pdx)/dL;
      d2y=(cdy-pdy)/dL;
    }
    double K = 0;
    if (std::fabs(cdx) >= ZERO_TOLERANCE || std::fabs(cdy) >= ZERO_TOLERANCE)
      K=(d2y*cdx-d2x*cdy)/std::pow((std::pow(cdx,2)+std::pow(cdy,2)),3/2);
#ifdef DEBUG
    std::cout << d2x << ' ' << d2y << ' ' << dL << ' ' << cdx << ' ' << cdy << ' ' << K << std::endl;
#endif
    curvature_.push_back(K);
    totalCurvature_+=K;
  }

#if 1 // commented out
  // Deal with the last point for a closed curve separately.
  if (!isOpen_)
  {
    double pdx=dx_[size()-1];
    double pdy=dy_[size()-1];
    double cdx=dx_[0];
    double cdy=dy_[0];
    double dL=arcLength_[0]-arcLength_[size()-1];
    double d2x, d2y;
    if (dL > ZERO_TOLERANCE ) {
      d2x=(cdx-pdx)/dL;
      d2y=(cdy-pdy)/dL;
    }
    else
      d2x=d2y=0;
    double K;
    if (std::fabs(cdx) < ZERO_TOLERANCE && std::fabs(cdy) < ZERO_TOLERANCE)
      K=0;
    else
      K=(d2y*cdx-d2x*cdy)/std::pow((std::pow(cdx,2)+std::pow(cdy,2)),3/2);
    curvature_[0]=K;
  }
#endif // 0
}

//: Compute derivatives
void bsol_intrinsic_curve_2d::computeDerivatives()
{
  //Compute derivatives
  dx_.clear();
  dx_.push_back(0.0);
  dy_.clear();
  dy_.push_back(0.0);

  double px=(*storage_)[0]->x();
  double py=(*storage_)[0]->y();
  for (int i=1;i<size();i++)
  {
    double cx=(*storage_)[i]->x();
    double cy=(*storage_)[i]->y();
    double dL=std::sqrt(std::pow(cx-px,2)+std::pow(cy-py,2));
    if (dL > ZERO_TOLERANCE) {
      dx_.push_back((cx-px)/dL);
      dy_.push_back((cy-py)/dL);
    }
    else{
      dx_.push_back(0.0);
      dy_.push_back(0.0);
    }
    px=cx;
    py=cy;
  }

#if 1 // commented out
  //Deal with the last point for a closed curve separately.
  if (!isOpen_)
  {
    double px=(*storage_)[size()-1]->x();
    double py=(*storage_)[size()-1]->y();
    double cx=(*storage_)[0]->x();
    double cy=(*storage_)[0]->y();
    double dL=std::sqrt(std::pow(cx-px,2)+std::pow(cy-py,2));
    dx_[0]=(cx-px)/dL;
    dy_[0]=(cy-py)/dL;
  }
#endif // 0
}

//: Compute angles
void bsol_intrinsic_curve_2d::computeAngles()
{
  angle_.clear();
  angle_.push_back(0.0);
  totalAngleChange_=0.0;

  double px=(*storage_)[0]->x();
  double py=(*storage_)[0]->y();
  for (int i=1;i<size();i++)
  {
    double cx=(*storage_)[i]->x();
    double cy=(*storage_)[i]->y();
    double theta=std::atan2(cy-py,cx-px);
    angle_.push_back(theta);
    px=cx;
    py=cy;
  }

  if (size()>2) {
    angle_[0]=angle_[1];
    for (unsigned int i=1;i<angle_.size();i++) {
#ifdef DEBUG
      std::cout << angle_[i] << ' ' << angle_[i-1] << std::endl;
#endif
      totalAngleChange_ += std::fabs(angle_[i]-angle_[i-1]);
    }
  }

//: IMPORTANT NOTE: in open curve matching giving the inputs as
//  OPEN curves or CLOSE curves (i.e. in .con file)
//  changes the cost computation
//  due to the following operation
//  In closed curve matching, input curves should always be given
//  as CLOSE curves.
#if 1
  //Deal with the last point for a closed curve separately.
  if (!isOpen_)
  {
    double px=(*storage_)[size()-1]->x();
    double py=(*storage_)[size()-1]->y();
    double cx=(*storage_)[0]->x();
    double cy=(*storage_)[0]->y();
    double theta=std::atan2(cy-py,cx-px);
    angle_[0]=theta;

#if 0 // commented out
    // TBS source code tests the distance between first and last points!!
    if (std::sqrt((cx-px)*(cx-px)+(cy-py)*(cy-py))<2)
      c->angle[0]=std::atan2(cy-py,cx-px);
#endif // 0
  }
#endif
}

//: Public function that calls the private functions to compute the various curve properties.
void bsol_intrinsic_curve_2d::computeProperties()
{
  if (size() ==0)
    return;

  computeArcLength();
  computeDerivatives();
  computeCurvatures();
  computeAngles();
}

//: Public function to upsample the current curve, it uses vsol_digital_curve and its interpolator
bool bsol_intrinsic_curve_2d::upsample(int new_size)
{
  if (size() >= new_size) {
    std::cout << "In bsol_intrinsic_curve_2d::upsample method: Curve size is larger than or equal to new_size already, exiting!\n";
    return true;
  }

  vsol_digital_curve_2d_sptr dc = new vsol_digital_curve_2d(*storage_);

  //: if curve is closed sample the portion between last point and first point
  if (!isOpen()) {
    dc->add_vertex((*storage_)[0]);
  }

  clear();
  storage_->clear();

  double T = dc->length()/new_size;
  std::cout << "T value for new_size is: " << T << std::endl;

  for (unsigned int i=1; i<dc->size(); ++i)
  {
    double len = ((dc->point(i))->get_p()-(dc->point(i-1))->get_p()).length();
    //: start with actual curve point
    storage_->push_back(dc->point(i-1));
    //: add first point with length T apart
    int j = 0;
    while (len - j*T >= (T+(T/2))) {
      std::cout << "i: " << i << " interpolating " << (i-1)+(j+1)*T/len << std::endl;
      storage_->push_back(new vsol_point_2d(dc->interp((i-1)+(j+1)*T/len)));
      j++;
    }
  }

  std::cout << "after upsampling bsol curve size: " << size() << " (should be " << new_size << ")\n";
  this->computeProperties();
  return true;
}

#if 0 // rest of file commented out

//: Default Constructor:
bsol_intrinsic_curve_2d::bsol_intrinsic_curve_2d()
{
  std::vector< vsol_point_2d > a;
  std::vector<double> b;

  (*storage_)=a;
  s_ = b;
  arcLength_ = b;
  normArcLength_ = b;
  dx_ = b;
  dy_ = b;
  curvature_ = b;
  angle_ = b;
  size()=0;
  length_=0.0;
  totalCurvature_=0.0;
  totalAngleChange_=0.0;
  isOpen_=true;
};

//: Constructor: From an array of points.
bsol_intrinsic_curve_2d::bsol_intrinsic_curve_2d(vsol_point_2d *pt, int size, bool isOpen)
{
  size()=size;
  isOpen_=isOpen;
  for (int i=0;i<size;i++) {
    (*storage_).push_back(pt[i]);
  }
  computeProperties();
}

//: Constructor: From an array of x and y coords.
bsol_intrinsic_curve_2d::bsol_intrinsic_curve_2d(double *x, double *y, int size, bool isOpen)
{
  size()=size;
  isOpen_=isOpen;
  for (int i=0;i<size;i++) {
    vsol_point_2d pt(x[i],y[i]);
    (*storage_).push_back(pt);
  }
  computeProperties();
}

// Copy constructor.
bsol_intrinsic_curve_2d::bsol_intrinsic_curve_2d(const bsol_intrinsic_curve_2d &rhs)
{
  if (this != &rhs)
  {
    (*storage_) = rhs.(*storage_);
    s_ = rhs.s_;
    arcLength_ = rhs.arcLength_;
    normArcLength_ = rhs.normArcLength_;
    dx_ = rhs.dx_;
    dy_ = rhs.dy_;
    curvature_ = rhs.curvature_;
    angle_ = rhs.angle_;

    size()=rhs.size();
    isOpen_=rhs.isOpen_;
    totalCurvature_=  rhs.totalCurvature_;
    totalAngleChange_=rhs.totalAngleChange_;
    length_=rhs.length_;
  }
}

// '=' Assignment operator
bsol_intrinsic_curve_2d& bsol_intrinsic_curve_2d::operator=(const bsol_intrinsic_curve_2d &rhs)
{
  if (this != &rhs) {
    (*storage_)    = rhs.(*storage_);
    s_             = rhs.s_;
    arcLength_     = rhs.arcLength_;
    normArcLength_ = rhs.normArcLength_;
    dx_            = rhs.dx_;
    dy_            = rhs.dy_;
    curvature_     = rhs.curvature_;
    angle_         = rhs.angle_;

    size()            = rhs.size();
    isOpen_           = rhs.isOpen_;
    length_           = rhs.length_;
    totalCurvature_   = rhs.totalCurvature_;
    totalAngleChange_ = rhs.totalAngleChange_;

    computeProperties();
  }
  return *this;
}


template <class double,class double>
void bsol_intrinsic_curve_2d<double,double>::readDataFromFile(std::string fileName)
{
}

//: TBS's .CON reader!
// Read the curve data from a Raph contour file.
// Assumes that there is only one contour per file.
/*
CONTOUR
OPEN (or CLOSE)
20 (numPoints)
x1 y1 x2 y2 x3 y3 ....
*/
void bsol_intrinsic_curve_2d::readDataFromFile(std::string fileName)
{
  //clear the existing curve data
  if (size() !=0)
    clear();

  std::ifstream infp(fileName.c_str(), std::ios::in);

  if (!infp) {
    std::cout << " Error opening file  " << fileName << std::endl;
    std::exit(1);
  }

  char lineBuffer[2000]; //200
  infp.getline(lineBuffer,2000);
  if (std::strncmp(lineBuffer,"CONTOUR",7)) {
    std::cerr << "Invalid File " << fileName.c_str() << '\n'
             << "Should be CONTOUR " << lineBuffer << '\n';
    std::exit(1);
  }

  char openFlag[2000];
  infp.getline(openFlag,2000);
  if (!std::strncmp(openFlag,"OPEN",4))
    isOpen_ = true;
  else if (!std::strncmp(openFlag,"CLOSE",5))
    isOpen_ = false;
  else{
    std::cerr << "Invalid File " << fileName.c_str() << '\n'
             << "Should be OPEN/CLOSE " << openFlag << '\n';
    std::exit(1);
  }

  int i,numOfPoints;
  infp >> numOfPoints;

  double x,y;
  for (i=0;i<numOfPoints;i++) {
    infp >> x >> y;
    add_vertex(x,y);
  }

  infp.close();
  computeProperties();
}

void bsol_intrinsic_curve_2d::readDataFromVector(std::vector<std::pair<double,double> > v)
{
  unsigned int numOfPoints=v.size();

  double x,y;
  for (unsigned int i=0;i<numOfPoints;i++) {
    x=v[i].first;
    y=v[i].second;

    add_vertex(x,y);
  }

  computeProperties();
}

#endif // 0
