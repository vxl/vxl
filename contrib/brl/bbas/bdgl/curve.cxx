#include<bdgl/curve.h>
#include<vcl_string.h>
#include<vcl_utility.h>
#include<vcl_cmath.h>
#include<vcl_fstream.h>
#include<vcl_cstring.h>
#include<vcl_cstdlib.h> // for exit()
#include<vgl/vgl_point_2d.h>
#include<vnl/vnl_double_2.h>
#define ZERO_TOLERANCE 1E-1

// Default Constructor:

Curve ::Curve()
{
  vcl_vector< vgl_point_2d<double> > a;
  vcl_vector<double> b;

  ptArray_=a;
  arcLength_ = b;
  normArcLength_ = b;
  dx_ = b;
  dy_ = b;
  curvature_ = b;
  angle_ = b;
  numPoints_=0;
  length_=0.0;
  totalCurvature_=0.0;
  totalAngleChange_=0.0;
  isOpen_=true;
}


//Copy constructor.
Curve::Curve(const Curve &rhs)
{
  if (this != &rhs)
  {
    ptArray_ = rhs.ptArray_;
    arcLength_ = rhs.arcLength_;
    normArcLength_ = rhs.normArcLength_;
    dx_ = rhs.dx_;
    dy_ = rhs.dy_;
    curvature_ = rhs.curvature_;
    angle_ = rhs.angle_;

    numPoints_=rhs.numPoints_;
    isOpen_=rhs.isOpen_;
    totalCurvature_=  rhs.totalCurvature_;
    totalAngleChange_=rhs.totalAngleChange_;
    length_=rhs.length_;
  }
  //return *this;
}


// Assignment operator
Curve &
Curve::operator=(const Curve &rhs)
{
  if (this != &rhs)
  {
   //vcl_cout<<"\n the size is "<<rhs.ptArray_.size()<<"\n";
	  ptArray_ = rhs.ptArray_;
   arcLength_ = rhs.arcLength_;
   normArcLength_ = rhs.normArcLength_;
   dx_ = rhs.dx_;
   dy_ = rhs.dy_;
   curvature_ = rhs.curvature_;
   angle_ = rhs.angle_;

   numPoints_ = rhs.numPoints_;
   isOpen_ = rhs.isOpen_;
   length_ = rhs.length_;
   totalCurvature_=  rhs.totalCurvature_;
   totalAngleChange_=rhs.totalAngleChange_;
  }
  return *this;
}


//Print x,y coordinates of all points.
void Curve::printElems()
{
  vcl_cout << "Num Points " << numPoints_ <<'\n';
  vcl_vector<vgl_point_2d<double> >::iterator iter;
  for (iter=ptArray_.begin();iter!=ptArray_.end();iter++)
    vcl_cout<<'\n'<<iter->x()<<'\t'<<iter->y();
}


//Add a point to the end of the list of points.
void Curve::append(vgl_point_2d<double> pt)
{
  ptArray_.push_back(pt);
  numPoints_++;
}


//Add a point  (x,y) to the end.
void Curve::append(double x,double y)
{
  vgl_point_2d<double> pt(x,y);
  ptArray_.push_back(pt);
  numPoints_++;
}

//Compute arclength and normalized arc length
void Curve::computeArcLength()
{
  double px,py;
  double cx,cy;
  double dL;
  int i;

  //Compute arc length
  arcLength_.clear();
  length_=0;
  arcLength_.push_back(0.0);

  px=ptArray_[0].x();
  py=ptArray_[0].y();
  for (i=1;i<numPoints_;i++)
  {
    cx=ptArray_[i].x();
    cy=ptArray_[i].y();
    dL=vcl_sqrt((float)(vcl_pow(cx-px,2)+vcl_pow(cy-py,2)));
    length_ += dL;
    arcLength_.push_back(length_);
    px=cx;
    py=cy;
  }

  //Deal with the last point for a closed curve separately.
  if (!isOpen_)
  {
    px=ptArray_[numPoints_-1].x();
    py=ptArray_[numPoints_-1].y();
    cx=ptArray_[0].x();
    cy=ptArray_[0].y();
    dL=vcl_sqrt(vcl_pow(cx-px,2)+vcl_pow(cy-py,2));
    length_ += dL;
    arcLength_[0]=length_;
  }

  //Compute normalized arc length
  normArcLength_.clear();
  for (i=0;i<numPoints_;i++)
    normArcLength_.push_back(arcLength_[i]/length_);
}

//Compute curvature. Assumes derivative computation has been done.
void Curve::computeCurvatures()
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

  for (i=1;i<numPoints_;i++)
  {
    pdx=dx_[i-1];
    pdy=dy_[i-1];
    cdx=dx_[i];
    cdy=dy_[i];
    dL=arcLength_[i]-arcLength_[i-1];
    if (dL > ZERO_TOLERANCE)
    {
      d2x=(cdx-pdx)/dL;
      d2y=(cdy-pdy)/dL;
    }
    else
      d2x=d2y=0;
    if (vcl_fabs(cdx) < ZERO_TOLERANCE && vcl_fabs(cdy) < ZERO_TOLERANCE)
      K=0;
    else
      K=(d2y*cdx-d2x*cdy)/vcl_pow((vcl_pow(cdx,2)+vcl_pow(cdy,2)),3/2);
    curvature_.push_back(K);
    totalCurvature_+=K;
  }

  //Deal with the last point for a closed curve separately.
  if (!isOpen_)
  {
    pdx=dx_[numPoints_-1];
    pdy=dy_[numPoints_-1];
    cdx=dx_[0];
    cdy=dy_[0];
    dL=arcLength_[0]-arcLength_[numPoints_-1];
    if (dL > ZERO_TOLERANCE )
    {
      d2x=(cdx-pdx)/dL;
      d2y=(cdy-pdy)/dL;
    }
    else
      d2x=d2y=0;
    if (vcl_fabs(cdx) < ZERO_TOLERANCE && vcl_fabs(cdy) < ZERO_TOLERANCE)
      K=0;
    else
      K=(d2y*cdx-d2x*cdy)/vcl_pow((vcl_pow(cdx,2)+vcl_pow(cdy,2)),3/2);
    curvature_[0]=K;
  }
}

void Curve::computeDerivatives()
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

  px=ptArray_[0].x();
  py=ptArray_[0].y();
  for (i=1;i<numPoints_;i++)
  {
    cx=ptArray_[i].x();
    cy=ptArray_[i].y();
    dL=vcl_sqrt(vcl_pow(cx-px,2)+vcl_pow(cy-py,2));
    if (dL > ZERO_TOLERANCE)
    {
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
  if (!isOpen_)
  {
    px=ptArray_[numPoints_-1].x();
    py=ptArray_[numPoints_-1].y();
    cx=ptArray_[0].x();
    cy=ptArray_[0].y();
    dL=vcl_sqrt(vcl_pow(cx-px,2)+vcl_pow(cy-py,2));
    dx_[0]=(cx-px)/dL;
    dy_[0]=(cy-py)/dL;
  }
}

//Compute angles
void Curve::computeAngles()
{
  double px,py;
  double cx,cy;
  double theta;

  angle_.clear();
  angle_.push_back(0.0);
  totalAngleChange_=0.0;

  px=ptArray_[0].x();
  py=ptArray_[0].y();
  for (int i=1;i<numPoints_;i++)
  {
    cx=ptArray_[i].x();
    cy=ptArray_[i].y();
    theta=vcl_atan2(cy-py,cx-px);
    angle_.push_back(theta);
    px=cx;
    py=cy;
  }
  //Deal with the last point for a closed curve separately.
  if (!isOpen_)
  {
    px=ptArray_[numPoints_-1].x();
    py=ptArray_[numPoints_-1].y();
    cx=ptArray_[0].x();
    cy=ptArray_[0].y();
    theta=vcl_atan2(cy-py,cx-px);
    angle_[0]=theta;
  }

  if (numPoints_>2)
  {
    angle_[0]=angle_[1];
    for (unsigned int i=1;i<angle_.size();i++)
      totalAngleChange_+=vcl_fabs(angle_[i]-angle_[i-1]);
  }
}

//Public function that calls the private functions to
//compute the various curve properties.
void Curve::computeProperties()
{
  computeArcLength();
  computeDerivatives();
  computeCurvatures();
  computeAngles();
}

// Read the curve data from a Raph contour file. Assumes that there
// is only one contour per file.
/*
CONTOUR
OPEN (or CLOSE)
20 (numPoints)
x1 y1 x2 y2 x3 y3 ....
*/
void Curve::readDataFromFile(vcl_string fileName)
{
  vcl_ifstream infp(fileName.c_str(),vcl_ios::in);
  if (!infp)
  {
    vcl_cout << " Error opening file  " << fileName << '\n';
    vcl_exit(1);
  }

  char magicNum[200];

  infp.getline(magicNum,200);
  if (vcl_strncmp(magicNum,"CONTOUR",7))
  {
    vcl_cerr << "Invalid File " << fileName.c_str() << '\n'
             << "Should be CONTOUR " << magicNum <<'\n';
    vcl_exit(1);
  }

  char openFlag[200];
  infp.getline(openFlag,200);
  if (!vcl_strncmp(openFlag,"OPEN",4))
    isOpen_ = true;
  else if (!vcl_strncmp(openFlag,"CLOSE",5))
    isOpen_ = false;
  else{
    vcl_cerr << "Invalid File " << fileName.c_str() << '\n'
             << "Should be OPEN/CLOSE " << openFlag << '\n';
    vcl_exit(1);
  }

  int i,numOfPoints;
  infp >> numOfPoints;

  double x,y;
  for (i=0;i<numOfPoints;i++)
  {
    infp >> x >> y;
    append(x,y);
  }
  infp.close();
  computeProperties();
}


void Curve::readDataFromVector(vcl_vector<vcl_pair<double,double> > v)
{
  int numOfPoints=v.size();
  ptArray_.clear();
  double x,y;
  for (int i=0;i<numOfPoints;i++)
  {
    x=v[i].first;
    y=v[i].second;
    append(x,y);
  }
  
  computeProperties();
}
void Curve::readDataFromVector(vcl_vector<vgl_point_2d<double> > v)
{
  int numOfPoints=v.size();
  ptArray_.clear();
  double x,y;
  for (int i=0;i<numOfPoints;i++)
  {
    x=v[i].x();
    y=v[i].y();
    append(x,y);
  }
  computeProperties();
}
