// This is bsol_intrinsic_curve_2d.cxx
#include "bsol_intrinsic_curve_2d.h"
//:
// \file

#include <vsol/vsol_point_2d.h>

//*****************************************************************************
// External declarations for implementation
//*****************************************************************************
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vcl_fstream.h>
#include <vnl/vnl_math.h>


//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Default Constructor
//---------------------------------------------------------------------------
bsol_intrinsic_curve_2d::bsol_intrinsic_curve_2d()
{
  storage_=new vcl_vector<vsol_point_2d_sptr>();
}

//---------------------------------------------------------------------------
//: Constructor from a vcl_vector of points
//---------------------------------------------------------------------------

bsol_intrinsic_curve_2d::bsol_intrinsic_curve_2d(const vcl_vector<vsol_point_2d_sptr> &new_vertices)
{
  storage_=new vcl_vector<vsol_point_2d_sptr>(new_vertices);
}

//---------------------------------------------------------------------------
// Copy constructor
//---------------------------------------------------------------------------
bsol_intrinsic_curve_2d::bsol_intrinsic_curve_2d(const bsol_intrinsic_curve_2d &other)
{
  storage_=new vcl_vector<vsol_point_2d_sptr>(*other.storage_);
  for (unsigned int i=0;i<storage_->size();++i)
    (*storage_)[i]=new vsol_point_2d(*((*other.storage_)[i]));
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
vsol_spatial_object_2d_sptr bsol_intrinsic_curve_2d::clone(void) const
{
  return new bsol_intrinsic_curve_2d (*this);
}

//***************************************************************************
// Access
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the first point of `this'
//---------------------------------------------------------------------------
vsol_point_2d_sptr bsol_intrinsic_curve_2d::p0(void) const
{
  return p0_;
}

//---------------------------------------------------------------------------
//: Return the last point of `this'
//---------------------------------------------------------------------------
vsol_point_2d_sptr bsol_intrinsic_curve_2d::p1(void) const
{
  return p1_;
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
  return
   obj.spatial_type() == vsol_spatial_object_2d::CURVE &&
   ((vsol_curve_2d const&)obj).curve_type() == vsol_curve_2d::POLYLINE
  ? *this == (bsol_intrinsic_curve_2d const&) (bsol_intrinsic_curve_2d const&) obj
  : false;
}

//***************************************************************************
// Internal status setting functions
//***************************************************************************
void bsol_intrinsic_curve_2d::clear (void)
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
//: Return the real type of a conic. It is a CURVE
//---------------------------------------------------------------------------
vsol_spatial_object_2d::vsol_spatial_object_2d_type
bsol_intrinsic_curve_2d::spatial_type(void) const
{
  return CURVE;
}

//---------------------------------------------------------------------------
//: Return the curvature of the vertex `i'
//---------------------------------------------------------------------------
double bsol_intrinsic_curve_2d::curvature (const int i) const
{		
	assert(valid_index(i));
	return curvature_[i];
}

//---------------------------------------------------------------------------
//: Return the angle of the vertex `i'
//---------------------------------------------------------------------------
double bsol_intrinsic_curve_2d::angle (const int i) const
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
void bsol_intrinsic_curve_2d::add_vertex(const vsol_point_2d_sptr &new_p, bool bRecomputeProterties)
{
  storage_->push_back(new_p);
}

//---------------------------------------------------------------------------
//: Remove one vertex from the intrinsic curve
//---------------------------------------------------------------------------
void bsol_intrinsic_curve_2d::remove_vertex(const int i, bool bRecomputeProterties)
{
	assert (valid_index(i));
	storage_->erase (storage_->begin() + i);
}

void bsol_intrinsic_curve_2d::modify_vertex (const int i, double x, double y, bool bRecomputeProterties)
{
	assert (valid_index(i));
	(*storage_)[i]->set_x (x);
	(*storage_)[i]->set_y (y);
}

//insert into i-1
void bsol_intrinsic_curve_2d::insert_vertex (const int i, double x, double y, bool bRecomputeProterties)
{
	assert (valid_index(i));
	vsol_point_2d_sptr pt = new vsol_point_2d (x,y);
	vcl_vector< vsol_point_2d_sptr >::iterator it = storage_->begin();
	it += i;
	storage_->insert (it, pt);
}

void bsol_intrinsic_curve_2d::readCONFromFile (vcl_string fileName)
{

	double x, y;
	char buffer[2000];
   int nPoints;

	//clear the existing curve data
	if (size() !=0)
		clear();

	//1)If file open fails, return.
	vcl_ifstream fp(fileName.c_str(), vcl_ios::in);
	if (!fp){
		vcl_cout<<" : Unable to Open "<<fileName<<vcl_endl;
		return;
	}

	//2)Read in file header.
   fp.getline (buffer,2000); //CONTOUR
   fp.getline (buffer,2000); //OPEN/CLOSE
   fp >> nPoints;
   vcl_cout << "Number of Points from Contour:" << nPoints << vcl_endl;
  
	for (int i=0;i<nPoints;i++) {
		fp >> x >> y;
		add_vertex (x,y);
	}

	fp.close();
	computeProperties();
}


//Compute arclength and normalized arc length
void bsol_intrinsic_curve_2d::computeArcLength()
{
  double px,py;
  double cx,cy;
  double dL;
  int i;

  //Compute arc length
  arcLength_.clear();
  s_.clear();
  length_=0;
  arcLength_.push_back(0.0);
  s_.push_back(0.0);
  
  px=(*storage_)[0]->x();
  py=(*storage_)[0]->y();
  for (i=1;i<size();i++){
    cx=(*storage_)[i]->x();
    cy=(*storage_)[i]->y();
    dL = vnl_math_hypot (cx-px,cy-py);
    length_ += dL;
    arcLength_.push_back(length_);
	 s_.push_back(dL);
    px=cx;
    py=cy;
  }

  assert (s_.size()==arcLength_.size());
  
  //Deal with the last point for a closed curve separately.
  //if (!_isOpen){
  //  px=(*storage_)[size()-1].x();
  //  py=(*storage_)[size()-1].y();
  //  cx=(*storage_)[0].x();
  //  cy=(*storage_)[0].y();
  //  dL=vcl_sqrt(pow(cx-px,2)+pow(cy-py,2));
  //  length_ += dL;
  //  arcLength_[0]=length_;
  //}

  //Compute normalized arc length 
  normArcLength_.clear();
  for (i=0;i<size();i++)
    normArcLength_.push_back(arcLength_[i]/length_);
}

//Compute curvature. Assumes derivative computation has been done.
void bsol_intrinsic_curve_2d::computeCurvatures()
{
  double pdx,pdy;
  double cdx,cdy;
  double d2x,d2y;
  double dL,K;
  int i;

  //Compute curvature
  curvature_.clear();
  curvature_.push_back(0.0);
  totalCurvature_=0.0;

  for (i=1;i<size();i++){
    pdx=dx_[i-1];
    pdy=dy_[i-1];
    cdx=dx_[i];
    cdy=dy_[i];
    dL=arcLength_[i]-arcLength_[i-1];
    if (dL > ZERO_TOLERANCE){
      d2x=(cdx-pdx)/dL;
      d2y=(cdy-pdy)/dL;
    }
    else
      d2x=d2y=0;
    if (vcl_fabs(cdx) < ZERO_TOLERANCE && vcl_fabs(cdy) < ZERO_TOLERANCE)
      K=0;
    else
      K=(d2y*cdx-d2x*cdy)/pow((pow(cdx,2)+pow(cdy,2)),3/2);
    //vcl_cout << d2x << " " << d2y << " " << dL << " " << cdx << " " << cdy << " " << K << vcl_endl;
    //printf("%6.3f %6.3f %6.3f %6.3f %6.3f %6.3f\n",d2x,d2y,dL,cdx,cdy,K);
    curvature_.push_back(K);
    totalCurvature_+=K;
  }

  //Deal with the last point for a closed curve separately.
  //if (!_isOpen){
  //  pdx=dx_[size()-1];
  //  pdy=dy_[size()-1];
  //  cdx=dx_[0];
  //  cdy=dy_[0];
  //  dL=arcLength_[0]-arcLength_[size()-1];
  //  if (dL > ZERO_TOLERANCE ){
  //    d2x=(cdx-pdx)/dL;
  //    d2y=(cdy-pdy)/dL;
  //  }
  //  else
  //    d2x=d2y=0;
  //  if (vcl_fabs(cdx) < ZERO_TOLERANCE && vcl_fabs(cdy) < ZERO_TOLERANCE)
  //    K=0;
  //  else
  //    K=(d2y*cdx-d2x*cdy)/pow((pow(cdx,2)+pow(cdy,2)),3/2);
  //  curvature_[0]=K;
  //  
  //}
}

//Compute derivatives
void bsol_intrinsic_curve_2d::computeDerivatives()
{
  double px,py;
  double cx,cy;
  double dL;
  int i;

  //Compute derivatives
  dx_.clear();
  dx_.push_back(0.0);
  dy_.clear();
  dy_.push_back(0.0);
  
  px=(*storage_)[0]->x();
  py=(*storage_)[0]->y();
  for (i=1;i<size();i++){
    cx=(*storage_)[i]->x();
    cy=(*storage_)[i]->y();
    dL=vcl_sqrt(pow(cx-px,2)+pow(cy-py,2));
    if (dL > ZERO_TOLERANCE){
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
  
    //Deal with the last point for a closed curve separately.
  //if (!_isOpen){
  //  px=(*storage_)[size()-1].x();
  //  py=(*storage_)[size()-1].y();
  //  cx=(*storage_)[0].x();
  //  cy=(*storage_)[0].y();
  //  dL=vcl_sqrt(pow(cx-px,2)+pow(cy-py,2));
  //  dx_[0]=(cx-px)/dL;
  //  dy_[0]=(cy-py)/dL;
  //}
}

//Compute angles
void bsol_intrinsic_curve_2d::computeAngles()
{
  double px,py;
  double cx,cy;
  double theta;
  int i;

  angle_.clear();
  angle_.push_back(0.0);
  totalAngleChange_=0.0;
  
  px=(*storage_)[0]->x();
  py=(*storage_)[0]->y();
  for (i=1;i<size();i++){
    cx=(*storage_)[i]->x();
    cy=(*storage_)[i]->y();
    theta=vcl_atan2(cy-py,cx-px);
    angle_.push_back(theta);
    px=cx;
    py=cy;
  }
  //Deal with the last point for a closed curve separately.
  //if (!_isOpen){
  //  px=(*storage_)[size()-1]->x();
  //  py=(*storage_)[size()-1]->y();
  //  cx=(*storage_)[0]->x();
  //  cy=(*storage_)[0]->y();
  //  theta=vcl_atan2(cy-py,cx-px);
  //  angle_[0]=theta;
  //}

  if (size()>2){
    angle_[0]=angle_[1];
    for (i=1;i<angle_.size();i++){
      //vcl_cout << angle_[i] << " " << angle_[i-1] << vcl_endl;
      totalAngleChange_ += vcl_fabs(angle_[i]-angle_[i-1]);
    }
  }
}

//Public function that calls the private functions to
//compute the various curve properties.
void bsol_intrinsic_curve_2d::computeProperties()
{
	if (size() ==0)
		return;

  computeArcLength();
  computeDerivatives();
  computeCurvatures();
  computeAngles();
}




#if 0


// Default Constructor:
bsol_intrinsic_curve_2d::bsol_intrinsic_curve_2d()
{
	vcl_vector< vsol_point_2d > a;
	vcl_vector<double> b;
	
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
	_isOpen=true;
};

// Constructor: From an array of points.
bsol_intrinsic_curve_2d::bsol_intrinsic_curve_2d(vsol_point_2d *pt, int size, bool isOpen)
{
  size()=size;
  _isOpen=isOpen;
  for (int i=0;i<size;i++) {
    (*storage_).push_back(pt[i]);
  }
  computeProperties();
}

//Constructor: From an array of x and y coords.
bsol_intrinsic_curve_2d::bsol_intrinsic_curve_2d(double *x, double *y, int size, bool isOpen)
{
  size()=size;
  _isOpen=isOpen;
  for (int i=0;i<size;i++){
    vsol_point_2d pt(x[i],y[i]); 
    (*storage_).push_back(pt);
  }
  computeProperties();
}

//Copy constructor.
bsol_intrinsic_curve_2d::bsol_intrinsic_curve_2d(const bsol_intrinsic_curve_2d &rhs)
{
  if (this != &rhs){
    (*storage_) = rhs.(*storage_);
	 s_ = rhs.s_;
    arcLength_ = rhs.arcLength_;
    normArcLength_ = rhs.normArcLength_;
    dx_ = rhs.dx_;
    dy_ = rhs.dy_;
    curvature_ = rhs.curvature_;
    angle_ = rhs.angle_;

    size()=rhs.size();
    _isOpen=rhs._isOpen;
    totalCurvature_=  rhs.totalCurvature_;
    totalAngleChange_=rhs.totalAngleChange_;
    length_=rhs.length_;
  }
  //return *this;
}

// '=' Assignment operator
bsol_intrinsic_curve_2d& bsol_intrinsic_curve_2d::operator= (const bsol_intrinsic_curve_2d &rhs)
{
	if (this != &rhs) {
		(*storage_)				= rhs.(*storage_);
		s_						= rhs.s_;
		arcLength_			= rhs.arcLength_;
		normArcLength_		= rhs.normArcLength_;
		dx_					= rhs.dx_;
		dy_					= rhs.dy_;
		curvature_			= rhs.curvature_;
		angle_				= rhs.angle_;
		
		size()			= rhs.size();
		_isOpen				= rhs._isOpen;
		length_				= rhs.length_;
		totalCurvature_	= rhs.totalCurvature_;
		totalAngleChange_	= rhs.totalAngleChange_;
	
		computeProperties();
	}
	return *this;
}


/*template <class double,class double>
void bsol_intrinsic_curve_2d<double,double>::readDataFromFile (vcl_string fileName)
{
}*/

//TBS's .CON reader!
// Read the curve data from a Raph contour file. Assumes that there
// is only one contour per file.
/*
CONTOUR
OPEN (or CLOSE)
20 (numPoints)
x1 y1 x2 y2 x3 y3 ....
*/
void bsol_intrinsic_curve_2d::readDataFromFile (vcl_string fileName)
{
	//clear the existing curve data
	if (size() !=0)
		clear();

	vcl_ifstream infp(fileName.c_str(), vcl_ios::in);

	if (!infp){
	  vcl_cout << " Error opening file  " << fileName << vcl_endl;
	  exit(1);
	}

	char lineBuffer[2000]; //200
	infp.getline(lineBuffer,2000);
	if (strncmp(lineBuffer,"CONTOUR",7)){
	  vcl_cerr << "Invalid File " << fileName.c_str() << vcl_endl;
	  vcl_cerr << "Should be CONTOUR " << lineBuffer << vcl_endl;
	  exit(1);
	}
	
	char openFlag[2000];
	infp.getline(openFlag,2000);
	if (!strncmp(openFlag,"OPEN",4))
	  _isOpen = true;
	else if (!strncmp(openFlag,"CLOSE",5))
	  _isOpen = false;
	else{
	  vcl_cerr << "Invalid File " << fileName.c_str() << vcl_endl;
	  vcl_cerr << "Should be OPEN/CLOSE " << openFlag << vcl_endl;
	  exit(1);
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
      
void bsol_intrinsic_curve_2d::readDataFromVector (vcl_vector<vcl_pair<double,double> > v)
{
	
	int numOfPoints=v.size();
	
	double x,y;
	for (int i=0;i<numOfPoints;i++){
		x=v[i].first;
		y=v[i].second;
		 
		add_vertex(x,y);
	}
	
	computeProperties();
}
  

#endif