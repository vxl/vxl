// This is brl/bbas/bsol/bsol_intrinsic_curve_3d.cxx
#include "bsol_intrinsic_curve_3d.h"
//:
// \file

#include <vsol/vsol_point_3d.h>
#include <vcl_cassert.h>
#include <vcl_cstdio.h>
#include <vcl_cmath.h>
#include <vcl_cstring.h>
#include <vnl/vnl_math.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Default Constructor
//---------------------------------------------------------------------------
bsol_intrinsic_curve_3d::bsol_intrinsic_curve_3d()
{
  storage_=new vcl_vector<vsol_point_3d_sptr>();
  computeProperties();
}

//---------------------------------------------------------------------------
//: Constructor from a vcl_vector of points
//---------------------------------------------------------------------------

bsol_intrinsic_curve_3d::bsol_intrinsic_curve_3d(const vcl_vector<vsol_point_3d_sptr> &new_vertices)
{
  storage_=new vcl_vector<vsol_point_3d_sptr>(new_vertices);
  computeProperties();
}

//---------------------------------------------------------------------------
// Copy constructor
//---------------------------------------------------------------------------
bsol_intrinsic_curve_3d::bsol_intrinsic_curve_3d(const bsol_intrinsic_curve_3d &other)
  : vsol_curve_3d(other)
{
  storage_=new vcl_vector<vsol_point_3d_sptr>(*other.storage_);
  for (unsigned int i=0; i<storage_->size(); ++i)
    (*storage_)[i]=new vsol_point_3d(*((*other.storage_)[i]));
  computeProperties();
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
bsol_intrinsic_curve_3d::~bsol_intrinsic_curve_3d()
{
  delete storage_;
}

//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_3d* bsol_intrinsic_curve_3d::clone(void) const
{
  return new bsol_intrinsic_curve_3d(*this);
}

//***************************************************************************
// Comparison
//***************************************************************************

//---------------------------------------------------------------------------
//: Has `this' the same points than `other' in the same order ?
//---------------------------------------------------------------------------
bool bsol_intrinsic_curve_3d::operator==(const bsol_intrinsic_curve_3d &other) const
{
  // quick return if possible:
  if (this==&other)
    return true;
  if (size() != other.size())
    return false;

  // run through other list to find the point matching the first point of this:
  vcl_vector<vsol_point_3d_sptr>::const_iterator i1 = storage_->begin(),
                                                 i2 = other.storage_->begin();
  while (i2!=other.storage_->end() && *(*i1)!=*(*i2))
    ++i2;
  if (i2==other.storage_->end())
    return false;

  // now run through both lists in sync and compare points:
  while (++i2,++i1!=storage_->end()) {
    if (i2==other.storage_->end()) i2 = other.storage_->begin();
    if (*(*i1)!=*(*i2)) return false;
  }

  return true;
}

//: spatial object equality
bool bsol_intrinsic_curve_3d::operator==(const vsol_spatial_object_3d& obj) const
{
  return
    obj.spatial_type() == vsol_spatial_object_3d::CURVE &&
    ((vsol_curve_3d const&)obj).curve_type() == vsol_curve_3d::DIGITAL_CURVE
    ? *this == static_cast<bsol_intrinsic_curve_3d const&>(obj)
    : false;
}

//***************************************************************************
// Status setting
//***************************************************************************

//: Compute the arcLength_[], s_[], curvature_[], angle_[]
void bsol_intrinsic_curve_3d::computeProperties()
{
  if (size() ==0)
    return;

  // 0) initialize starting point and end point.
  p0_ = (*storage_)[1];
  p1_ = (*storage_)[storage_->size()-1];

  // 1) Reset datastructures
  arcLength_.clear();
  s_.clear();
  phi_.clear();
  phis_.clear();
  phiss_.clear();
  theta_.clear();
  thetas_.clear();
  thetass_.clear();
  Tangent_.clear();
  Normal_.clear();
  Binormal_.clear();
  curvature_.clear();
  torsion_.clear();

  // 2) Setup the starting conditions
  arcLength_.push_back(-1);
  s_.push_back(-1);
  phi_.push_back(-1);
  phis_.push_back(-1); phis_.push_back(-1);
  phiss_.push_back(-1); phiss_.push_back(-1);
  theta_.push_back(-1);
  thetas_.push_back(-1); thetas_.push_back(-1);
  thetass_.push_back(-1); thetass_.push_back(-1);
  Tangent_.push_back(NULL);
  Normal_.push_back(NULL); Normal_.push_back(NULL);
  Binormal_.push_back(NULL); Binormal_.push_back(NULL);
  curvature_.push_back(-1.0); curvature_.push_back(-1.0);
  torsion_.push_back(-1); torsion_.push_back(-1);

  //The third order terms.
  if (size()>2) {
    phiss_.push_back(-1);
    thetass_.push_back(-1);
    torsion_.push_back(-1);
  }

  // 3) Compute the first derivative: arc length and angle.
  double prev_x = (*storage_)[0]->x();
  double prev_y = (*storage_)[0]->y();
  double prev_z = (*storage_)[0]->z();
  double length = 0;
  for (unsigned int i=1; i<size(); ++i)
  {
    double cur_x=(*storage_)[i]->x();
    double cur_y=(*storage_)[i]->y();
    double cur_z=(*storage_)[i]->z();
    double cur_dx = cur_x - prev_x;
    double cur_dy = cur_y - prev_y;
    double cur_dz = cur_z - prev_z;
    double dL = vcl_sqrt(cur_dx*cur_dx + cur_dy*cur_dy + cur_dz*cur_dz);
    s_.push_back(dL);
    length += dL;
    arcLength_.push_back(length);

    double phi = vcl_acos(cur_dz/dL);
    phi_.push_back(phi);
    double dLxy = dL*vcl_sin(phi);
    double theta = vcl_acos(cur_dx/dLxy);
    theta_.push_back(theta);

    vgl_vector_3d<double>* tangent = new vgl_vector_3d<double>(cur_dx, cur_dy, cur_dz);
    normalize(*tangent); //normalize the tangent vector.
    Tangent_.push_back(tangent);

    prev_x = cur_x;
    prev_y = cur_y;
    prev_z = cur_z;
  }
  assert (s_.size() == size());
  assert (arcLength_.size() == size());
  assert (phi_.size() == size());
  assert (theta_.size() == size());
  assert (Tangent_.size() == size());

  // 4) Compute the second derivative: phi_s, theta_s, normal, binormal, curvature
  totalCurvature_ = 0;
  totalAngleChange_ = 0;
  for (unsigned int i=2; i<size(); ++i)
  {
    double phis = (phi_[i] - phi_[i-1])/s_[i];
    phis_.push_back(phis);
    double thetas = (theta_[i] - theta_[i-1])/s_[i];
    thetas_.push_back(thetas);
    double curvature = vnl_math_hypot(phis, vcl_sin(phi_[i])*thetas);
    curvature_.push_back(curvature);
    totalCurvature_ += curvature;
    totalAngleChange_ += vcl_fabs(curvature);

    double cos_phi = vcl_cos(phi_[i]);
    double sin_phi = vcl_sin(phi_[i]);
    double cos_theta = vcl_cos(theta_[i]);
    double sin_theta = vcl_sin(theta_[i]);
    double normalx = cos_phi * cos_theta * phis -
                sin_phi * sin_theta * thetas;
    double normaly = cos_phi * sin_theta * phis -
                sin_phi * cos_theta * thetas;
    double normalz = - vcl_sin(phi_[i]) * phis;
    vgl_vector_3d<double>* normal = new vgl_vector_3d<double>(normalx, normaly, normalz);
    normalize(*normal); //normalize the normal vector.
    Normal_.push_back(normal);

    vgl_vector_3d<double>* binormal = new vgl_vector_3d<double>;
    *binormal = cross_product(*(Tangent_[i]), *normal);
    normalize(*binormal); //normalize the binormal vector.
    Binormal_.push_back(binormal);
  }
  assert (phis_.size() == size());
  assert (thetas_.size() == size());
  assert (curvature_.size() == size());
  assert (Normal_.size() == size());
  assert (Binormal_.size() == size());

  // 5) Compute the third derivative: phi_ss, theta_ss, torsion
  for (unsigned int i=3; i<size(); ++i) {
    double phiss = (phis_[i] - phis_[i-1])/s_[i];
    phiss_.push_back(phiss);
    double thetass = (thetas_[i] - thetas_[i-1])/s_[i];
    thetass_.push_back(thetass);

    //compute torsion. (this is an very noisy approach, without ENO).
#if 0
    double sin_phi = vcl_sin(phi_[i]);
    double cos_phi = vcl_cos(phi_[i]);
    double torsion_n = 2*cos_phi*thetas_[i]*phis_[i]*phis_[i] +
                 sin_phi*phis_[i]*thetass_[i] +
                 sin_phi*thetas_[i]*
                  ( -phiss_[i] + sin_phi*cos_phi*thetas_[i]*thetas_[i] );
    double torsion_d = phis_[i]*phis_[i] + sin_phi*sin_phi*thetas_[i]*thetas_[i];
    double torsion = torsion_n / torsion_d;
    torsion_.push_back(torsion);
#else // 0
    //torsion = -N dot B'. B' = (B[i]-B[i-1])/s[i]
    vgl_vector_3d<double> bp = (*Binormal_[i] - *Binormal_[i-1])/s_[i];
    normalize(bp);
    double torsion = - dot_product(*Normal_[i], bp);
    torsion_.push_back(torsion);
#endif // 0
  }
  assert (phiss_.size() == size());
  assert (thetass_.size() == size());
  assert (torsion_.size() == size());
}

#if 0
//: Compute the arcLength_[], s_[], curvature_[], angle_[]
void bsol_intrinsic_curve_3d::computeProperties_old()
{
  if (size() ==0)
    return;
  // initialize starting point and end point.
  p0_ = (*storage_)[1];
  p1_ = (*storage_)[storage_->size()-1];

  // 1)reset datastructures
  arcLength_.clear();
  s_.clear();
  curvature_.clear();
  angle_.clear();

  // 2)the arc length of the first point is defined to be zero.
  arcLength_.push_back(0.0);
  s_.push_back(0.0);
  angle_.push_back(0.0);
  totalAngleChange_ = 0.0;
  curvature_.push_back(0.0);
  totalCurvature_ = 0.0;

  vgl_vector_3d<double>* tangent = new vgl_vector_3d<double>;
  Tangent_.push_back(tangent);

  // 3)compute arc length for all other vertices
  double length = 0;
  double prev_x = (*storage_)[0]->x();
  double prev_y = (*storage_)[0]->y();
  double prev_z = (*storage_)[0]->z();
  double prev_dx = 0;
  double prev_dy = 0;
  double prev_dz = 0;
  double prev_dL = 0;

  for (unsigned int i=1; i<size(); ++i)
  {
    double cur_x=(*storage_)[i]->x();
    double cur_y=(*storage_)[i]->y();
    double cur_z=(*storage_)[i]->z();

    double cur_dx = cur_x-prev_x;
    double cur_dy = cur_y-prev_y;
    double cur_dz = cur_z-prev_z;

    tangent = new vgl_vector_3d<double>(cur_dx, cur_dy, cur_dz);
    normalize(*tangent); //normalize the tangent vector.
    Tangent_.push_back(tangent);

    double dL = vnl_math_hypot(cur_dx, cur_dy);
    length += dL;
    arcLength_.push_back(length);
    s_.push_back(dL);

     double theta = vcl_atan2(cur_dy, cur_dx);
    angle_.push_back(theta);

    double K;
    if (dL > ZERO_TOLERANCE) {
      double cdx = cur_dx/dL;
      double cdy = cur_dy/dL;
      double pdx, pdy;
      if (prev_dL > ZERO_TOLERANCE) {
        pdx = prev_dx/prev_dL;
        pdy = prev_dy/prev_dL;
      }
      else {
        pdx = 0;
        pdy = 0;
      }
      double d2x = (cdx-pdx)/dL;
      double d2y = (cdy-pdy)/dL;
      K = (d2y*cdx-d2x*cdy) / vcl_pow(cdx*cdx+cdy*cdy,3/2);
    }
    else {
      K = 0;
    }
    curvature_.push_back(K);
    totalCurvature_ += K;

    prev_x = cur_x;
    prev_y = cur_y;
    prev_z = cur_z;
    prev_dx = cur_dx;
    prev_dy = cur_dy;
    prev_dz = cur_dz;
    prev_dL = dL;
  }//end for i

  // 4) Compute the Normal vector. Note that N_i = T_i+1 - T_i
  vgl_vector_3d<double>* normal;
  for (unsigned int i=0; i+1<size(); ++i) {
    normal = new vgl_vector_3d<double>(*Tangent_[i+1] - *Tangent_[i]);
    Normal_.push_back(normal);
  }
  normal = new vgl_vector_3d<double>;
  Normal_.push_back(normal);

  // 5) Compute the Binormal vector. B_i = T_i * N_i
  for (unsigned int i=0; i<size(); ++i) {
    vgl_vector_3d<double>* binormal = new vgl_vector_3d<double>(cross_product(*Tangent_[i], *Normal_[i]));
    Binormal_.push_back(binormal);
  }

  // 6)Deal with the beginning part of the curve.
  //   set angle_[0] to angle_[1], and compute totalAngleChange
  if (size() > 1) {
    Tangent_[0] = Tangent_[1];
    angle_[0] = angle_[1];
    for (unsigned int i=1; i<size(); ++i)
      totalAngleChange_ += vcl_fabs(angle_[i]-angle_[i-1]);

    // 5-2)Deal with the ending part of the curve.
    Normal_[0] = Normal_[1];
    Normal_[size()-1] = Normal_[size()-2];
    // 5-3)Deal with the ending part of the curve.
    Binormal_[0] = Binormal_[1];
    Binormal_[size()-1] = Binormal_[size()-2];
  }
}
#endif

void bsol_intrinsic_curve_3d::clear(void)
{
  for (unsigned int i=0; i<size(); ++i) {
    delete Tangent_[i];
  }

  //reset the storage_
  delete storage_;
  storage_=new vcl_vector<vsol_point_3d_sptr>();

  arcLength_.clear();
  s_.clear();
  phi_.clear();
  phis_.clear();
  phiss_.clear();
  theta_.clear();
  thetas_.clear();
  thetass_.clear();
  Tangent_.clear();
  Normal_.clear();
  Binormal_.clear();
  curvature_.clear();
  torsion_.clear();
  totalCurvature_ = 0;
  totalAngleChange_ = 0;
}

bool bsol_intrinsic_curve_3d::LoadCON3File(vcl_string fileName)
{
  vcl_FILE* fp;
  char buffer[128];

  if ((fp = vcl_fopen(fileName.c_str(), "r")) == NULL) {
    vcl_fprintf(stderr, "ERROR: Can't open input .con3 file %s.\n", fileName.c_str());
    return false;
  }
  CON3Filename_ = fileName;

  vcl_fscanf(fp, "CONTOUR\n");
  vcl_fscanf(fp, "%s\n", buffer);
  if (vcl_strcmp(buffer, "OPEN")==0) //the same
    isOpen_ = true;
  else //"CLOSE"
    isOpen_ = false;
  int numPoints=0;
  vcl_fscanf(fp, "%d\n", &numPoints);
  if (numPoints<=0)
    vcl_fprintf(stderr, "WARNING: First line of file %s (number of points) should be strictly positive.\n",
                fileName.c_str());

  for (int i=0; i<numPoints; ++i) {
    double x, y, z;
    vcl_fscanf(fp, "%lf", &x);
    vcl_fscanf(fp, "%lf", &y);
    vcl_fscanf(fp, "%lf", &z);
    add_vertex(x, y, z);
  }

  //Note here that if the numPoints is wrong, the rest of the file is not read.
  vcl_fclose(fp);

  computeProperties();

  return true;
}

bool bsol_intrinsic_curve_3d::SaveCON3File(vcl_string fileName)
{
  vcl_FILE* fp;

  if ((fp = vcl_fopen(fileName.c_str(), "w")) == NULL) {
    vcl_fprintf(stderr, "ERROR( bsol_intrinsic_curve_3d::SaveCON3File): Can't open output .con3 file %s.\n", fileName.c_str());
    return false;
  }

  vcl_fprintf(fp, "CONTOUR\n");
  if (isOpen_)
    vcl_fprintf(fp, "OPEN\n");
  else
    vcl_fprintf(fp, "CLOSE\n");

  vcl_fprintf(fp, "%d\n", size());

  for (unsigned int i=0; i<size(); ++i) {
    vcl_fprintf(fp, "%.10f %.10f %.10f\n", x(i), y(i), z(i));
  }

  vcl_fclose(fp);
  return true;
}

//---------------------------------------------------------------------------
//: Compute the bounding box of `this'
//---------------------------------------------------------------------------
void bsol_intrinsic_curve_3d::compute_bounding_box(void) const
{
  set_bounding_box((*storage_)[0]->x(), (*storage_)[0]->y(), (*storage_)[0]->z());
  for (unsigned int i=1; i<storage_->size(); ++i)
    add_to_bounding_box((*storage_)[i]->x(), (*storage_)[i]->y(), (*storage_)[i]->z());
}
