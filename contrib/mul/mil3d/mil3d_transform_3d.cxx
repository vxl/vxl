// This is mul/mil3d/mil3d_transform_3d.cxx
#include "mil3d_transform_3d.h"
//:
// \file

#include <vcl_cstdlib.h>
#include <vcl_cassert.h>
#include <vsl/vsl_indent.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_math.h>

//=======================================================================

//=======================================================================
// Method: matrix
//=======================================================================

vnl_matrix<double> mil3d_transform_3d::matrix() const
{
  vnl_matrix<double> M(4,4);
  matrix(M);
  return M;
}

//=======================================================================
// Method: matrix
//=======================================================================

void mil3d_transform_3d::matrix(vnl_matrix<double>& M) const
{
  M.set_size(4,4);
  double**m_data = M.data_array();
  m_data[0][0]=xx_; m_data[0][1]=xy_; m_data[0][2]=xz_; m_data[0][3]=xt_;
  m_data[1][0]=yx_; m_data[1][1]=yy_; m_data[1][2]=yz_; m_data[1][3]=yt_;
  m_data[2][0]=zx_; m_data[2][1]=zy_; m_data[2][2]=zz_; m_data[2][3]=zt_;
  m_data[3][0]=tx_; m_data[3][1]=ty_; m_data[3][2]=tz_; m_data[3][3]=tt_;
}

//=======================================================================
// Method: angles
//=======================================================================

void mil3d_transform_3d::angles(double& phi_x, double& phi_y, double& phi_z) const
{
  phi_x=vcl_atan2(-yz_,zz_);
  phi_y=vcl_atan2(-xz_*vcl_cos(phi_x),zz_);
  phi_z=vcl_atan2(-xy_,xx_);

  // the angles may be wrong by +-vnl_math::pi - we can
  // only tell by checking against the signs
  // of the original entries in the rotation matrix
  if (vcl_fabs(vcl_sin(phi_y) + xz_) > 1e-6)
  if (phi_y > 0)
    phi_y=phi_y - vnl_math::pi;
  else
    phi_y=phi_y + vnl_math::pi;

  const double cos_y = vcl_cos(phi_y);

  if (vcl_fabs(vcl_sin(phi_x)*cos_y + yz_) > 1e-6 ||
      vcl_fabs(vcl_cos(phi_x)*cos_y - zz_) > 1e-6)
  {
    if (phi_x > 0)
      phi_x=phi_x - vnl_math::pi;
    else
      phi_x=phi_x + vnl_math::pi;
  }

  if (vcl_fabs(vcl_cos(phi_z)*cos_y - xx_) > 1e-6 ||
      vcl_fabs(vcl_sin(phi_z)*cos_y + xy_) > 1e-6)
  {
    if (phi_z > 0)
      phi_z=phi_z - vnl_math::pi;
    else
      phi_z=phi_z + vnl_math::pi;
  }

      // now compress the angles towards zero as much as possible
      // (we can add +-vnl_math::pi to each angle without changing
      // the rotation matrix)
  int count = 0;
  if (vcl_fabs(phi_x) > vnl_math::pi/2) ++count;
  if (vcl_fabs(phi_y) > vnl_math::pi/2) ++count;
  if (vcl_fabs(phi_z) > vnl_math::pi/2) ++count;

  if (count > 1)
  {
    if (phi_x > 0)
      phi_x=phi_x - vnl_math::pi;
    else
      phi_x=phi_x + vnl_math::pi;

    phi_y=-phi_y;
    if (phi_y > 0)
      phi_y=phi_y - vnl_math::pi;
    else
      phi_y=phi_y + vnl_math::pi;

    if (phi_z > 0)
      phi_z=phi_z - vnl_math::pi;
    else
      phi_z=phi_z + vnl_math::pi;
  }
}

//=======================================================================
// Method: params
//=======================================================================

void mil3d_transform_3d::params(vnl_vector<double>& v) const
{
  switch (form_)
  {
   case Identity:
    assert(tx_==0 && ty_==0 && tz_==0 && tt_==1);
    assert(xy_==0 && xz_==0 && yx_==0 && yz_==0 && zx_==0 && zy_==0);
    assert(xx_==1 && yy_==1 && zz_==1);
    assert(xt_==0 && yt_==0 && zt_==0);
    v.set_size(0);
    break;
   case Translation:
    assert(tx_==0 && ty_==0 && tz_==0 && tt_==1);
    assert(xy_==0 && xz_==0 && yx_==0 && yz_==0 && zx_==0 && zy_==0);
    assert(xx_==1 && yy_==1 && zz_==1);
    if (v.size()!=3) v.set_size(3);
    v[0]=xt_; v[1]=yt_; v[2]=zt_;
    break;
   case ZoomOnly:
    assert(tx_==0 && ty_==0 && tz_==0 && tt_==1);
    assert(xy_==0 && xz_==0 && yx_==0 && yz_==0 && zx_==0 && zy_==0);
    if (v.size()!=6) v.set_size(6);
    v[0]=xx_; v[1]=yy_; v[2]=zz_;
    v[3]=xt_; v[4]=yt_; v[5]=zt_;
    break;
   case RigidBody:
    assert(tx_==0 && ty_==0 && tz_==0 && tt_==1);
    if (v.size()!=6) v.set_size(6);
    angles(v[0],v[1],v[2]);
    v[3]=xt_; v[4]=yt_; v[5]=zt_;
    break;
   case Similarity:
    assert(tx_==0 && ty_==0 && tz_==0 && tt_==1);
    assert(xx_==yy_ && yy_==zz_);
    if (v.size()!=7) v.set_size(7);
    v[0]=xx_; // scaling factor
    angles(v[1],v[2],v[3]);
    v[4]=xt_; v[5]=yt_; v[6]=zt_;
    break;
   case Affine:
    assert(tx_==0 && ty_==0 && tz_==0 && tt_==1);
    if (v.size()!=9) v.set_size(9);
    v[0]=xx_; // scaling factor
    v[1]=yy_; // scaling factor
    v[2]=zz_; // scaling factor
    angles(v[3],v[4],v[5]);
    v[6]=xt_; v[7]=yt_; v[8]=zt_;
    break;
   case Projective:
    vcl_cerr<<"mil3d_transform_3d::params() Unexpected form Projective\n";
    break;
   case Undefined:
    vcl_cerr<<"mil3d_transform_3d::params() Unexpected form Undefined\n";
    break;
   default:
    vcl_cerr<<"mil3d_transform_3d::params() Unexpected form: "
            <<int(form_)<<vcl_endl;
    vcl_abort();
  }
}

//=======================================================================
// Method: setCheck
//=======================================================================

void mil3d_transform_3d::setCheck(int n1,int n2,const char* str) const
{
  if (n1==n2) return;
  vcl_cerr<<"mil3d_transform_3d::set() "<<n1<<" parameters required for "
          <<str<<". Passed "<<n2<<vcl_endl;
  vcl_abort();
}

//=======================================================================
// Method: set
//=======================================================================

void mil3d_transform_3d::set(const vnl_vector<double>& v, Form form)
{
  int n=v.size();

  switch (form)
  {
   case Identity:
    set_identity();
    break;
   case Translation:
    setCheck(3,n,"Translation");
    set_translation(v[0],v[1],v[2]);
    break;
   case ZoomOnly:
    setCheck(6,n,"ZoomOnly");
    set_zoom_only(v[0],v[1],v[2],
                  v[3],v[4],v[5]);
    break;
   case RigidBody:
    setCheck(6,n,"RigidBody");
    set_rigid_body(v[0],v[1],v[2],
                   v[3],v[4],v[5]);
    break;
   case Similarity:
    setCheck(7,n,"Similarity");
    set_similarity(v[0],v[1],v[2],
                   v[3],v[4],v[5], v[6]);
    form_ = Similarity;
    inv_uptodate_=false;
    break;
   case Affine:
    setCheck(9,n,"Affine");
    set_affine(v[0],v[1],v[2],
               v[3],v[4],v[5],
               v[6],v[7],v[8]);
    form_ = Affine;
    inv_uptodate_=false;
    break;
   case Projective:
    vcl_cerr<<"mil3d_transform_3d::set() Unexpected form Projective\n";
    break;
   case Undefined:
    vcl_cerr<<"mil3d_transform_3d::set() Unexpected form Undefined\n";
    break;
   default:
    vcl_cerr<<"mil3d_transform_3d::set() Unexpected form: "
            <<int(form)<<vcl_endl;
    vcl_abort();
  }
}


//=======================================================================
// Method: identity
//=======================================================================

void mil3d_transform_3d::set_identity()
{
  if (form_==Identity) return;
  form_=Identity;

  xx_=yy_=zz_=tt_=1;
  xy_=xz_=xt_=0;
  yx_=yz_=yt_=0;
  zx_=zy_=zt_=0;
  tx_=ty_=tz_=0;
  inv_uptodate_=false;
}

//=======================================================================
// Method: translation
//=======================================================================

void mil3d_transform_3d::set_translation(double t_x, double t_y, double t_z)
{
  if (t_x==0 && t_y==0 && t_z==0)
    set_identity();
  else
  {
    form_=Translation;
    xx_=yy_=yy_=tt_=1;
    xy_=xz_=yx_=yz_=zx_,zy_=0;
    tx_=ty_=tz_=0;
    xt_=t_x;
    yt_=t_y;
    zt_=t_z;
  }

  inv_uptodate_=false;
}

//=======================================================================
// Method: origin
//=======================================================================

void mil3d_transform_3d::origin(const vgl_point_3d<double> & p)
{
  xt_=p.x()*tt_;
  yt_=p.y()*tt_;
  zt_=p.z()*tt_;

  if (form_ == Identity) form_=Translation;

  inv_uptodate_=false;
}

//=======================================================================
// Method: zoomonly
//=======================================================================

void mil3d_transform_3d::set_zoom_only(double s_x, double s_y, double s_z,
                                       double t_x, double t_y, double t_z)
{
  form_=ZoomOnly;
  tx_=ty_=tz_=0;
  xy_=xz_=yx_=yz_=zx_=zy_=0;
  xx_=s_x;
  yy_=s_y;
  zz_=s_z;
  xt_=t_x;
  yt_=t_y;
  zt_=t_z;

  inv_uptodate_=false;
}

//=======================================================================
// Method: setRotMat
//=======================================================================

void mil3d_transform_3d::setRotMat(double r_x, double r_y, double r_z)
{
  double sinx=vcl_sin(r_x);
  double siny=vcl_sin(r_y);
  double sinz=vcl_sin(r_z);
  double cosx=vcl_cos(r_x);
  double cosy=vcl_cos(r_y);
  double cosz=vcl_cos(r_z);

  // set R_mat = Rx*Ry*Rz
  xx_ =  cosy*cosz;
  xy_ = -cosy*sinz;
  xz_ = -siny;
  yx_ =  cosx*sinz - sinx*siny*cosz;
  yy_ =  cosx*cosz + sinx*siny*sinz;
  yz_ = -sinx*cosy;
  zx_ =  sinx*sinz + cosx*siny*cosz;
  zy_ =  sinx*cosz - cosx*siny*sinz;
  zz_ =  cosx*cosy;
}

//=======================================================================
// Method: set_rigid_body
//=======================================================================

void mil3d_transform_3d::set_rigid_body(double r_x, double r_y, double r_z,
                                        double t_x, double t_y, double t_z)
{
  if (r_x==0.0 && r_y==0.0 && r_z==0.0) {
    set_translation(t_x,t_y,t_z);
  }
  else
  {
    form_=RigidBody;

    setRotMat(r_x,r_y,r_z);
    xt_=t_x;
    yt_=t_y;
    zt_=t_z;
  }

  inv_uptodate_=false;
}

//=======================================================================
// Method: Similarity
//=======================================================================

void mil3d_transform_3d::set_similarity(double s,
                                        double r_x, double r_y, double r_z,
                                        double t_x, double t_y, double t_z)
{
  if (s==1.0)
    set_rigid_body(r_x,r_y,r_z,t_x,t_y,t_z);
  else
  {
    form_=Similarity;

    setRotMat(r_x,r_y,r_z);
    xx_=yy_=zz_=s;
    xt_=t_x;
    yt_=t_y;
    zt_=t_z;
  }
  inv_uptodate_=false;
}

//=======================================================================
// Method: affine
//=======================================================================

void mil3d_transform_3d::set_affine(double s_x, double s_y, double s_z,
                                    double r_x, double r_y, double r_z,
                                    double t_x, double t_y, double t_z)
{
  form_=Affine;

  // set rotation matrix as for rigid body
  setRotMat(r_x,r_y,r_z);

  // the take account of scaling
  xx_*=s_x;
  yy_*=s_y;
  zz_*=s_z;

  xt_=t_x;
  yt_=t_y;
  zt_=t_z;
  tx_=ty_=tz_=0.0; tt_=1.0;
  inv_uptodate_=false;
}

//=======================================================================
// Method: projective
//=======================================================================

void mil3d_transform_3d::set_projective(vnl_matrix<double> const& M)
{
  assert(M.rows()==4 && M.cols()==4);
  form_=Projective;
  xx_ = M(0,0); xy_ = M(0,1); xz_ = M(0,2); xt_ = M(0,3);
  yx_ = M(1,0); yy_ = M(1,1); yz_ = M(1,2); yt_ = M(1,3);
  zx_ = M(2,0); zy_ = M(2,1); zz_ = M(2,2); zt_ = M(2,3);
  tx_ = M(3,0); ty_ = M(3,1); tz_ = M(3,2); tt_ = M(3,3);
  inv_uptodate_=false;
}

//=======================================================================
// Method: operator()
//=======================================================================

vgl_point_3d<double> mil3d_transform_3d::operator()(double x, double y, double z) const
{
  switch (form_)
  {
   case Identity :
    return vgl_point_3d<double>(x,y,z);
   case Translation :
    return vgl_point_3d<double>(x+xt_,y+yt_,z+zt_);
   case ZoomOnly :
      return vgl_point_3d<double>(x*xx_+xt_,
                                  y*yy_+yt_,
                                  z*zz_+zt_);
   case RigidBody :
   case Similarity :
   case Affine :
    return vgl_point_3d<double>(x*xx_+y*xy_+z*xz_+xt_,
                                x*yx_+y*yy_+z*yz_+yt_,
                                x*zx_+y*zy_+z*zz_+zt_);
   case Projective:
    {
      double x2 = x*xx_+y*xy_+z*xz_+xt_,
             y2 = x*yx_+y*yy_+z*yz_+yt_,
             z2 = x*zx_+y*zy_+z*zz_+zt_,
             t2 = x*tx_+y*ty_+z*tz_+tt_;
      return vgl_point_3d<double>(x2/t2,y2/t2,z2/t2);
    }
   case Undefined:
    vcl_cerr<<"mil3d_transform_3d::operator() Unexpected form Undefined\n";
    vcl_abort();
   default:
    vcl_cerr<<"mil3d_transform_3d::operator() : Unrecognised form: "
            <<int(form_)<<vcl_endl;
    vcl_abort();
  }

  return vgl_point_3d<double>(); // To keep over-zealous compilers happy
}

//=======================================================================
// Method: delta
//=======================================================================

vgl_vector_3d<double> mil3d_transform_3d::delta(vgl_point_3d<double> p, vgl_vector_3d<double> dp) const
{
  switch (form_)
  {
   case Identity :
   case Translation:
    return dp;
   case ZoomOnly :
    return vgl_vector_3d<double>(dp.x()*xx_,
                                 dp.y()*yy_,
                                 dp.z()*zz_);
   case RigidBody :
   case Similarity :
   case Affine : // return value is independent of p since in general dp determines p for a given transformation
    return vgl_vector_3d<double> (
      dp.x()*xx_+dp.y()*xy_+dp.z()*xz_,
      dp.x()*yx_+dp.y()*yy_+dp.z()*yz_,
      dp.x()*zx_+dp.y()*zy_+dp.z()*zz_
     );
   default: // general projective transformation
    {
      double lambda = tx_*p.x()+ty_*p.y()+tz_*p.z()+tt_;
      double lamb_d = tx_*dp.x()+ty_*dp.y()+tz_*dp.z();
      double x = dp.x()*xx_+dp.y()*xy_+dp.z()*xz_;
      double y = dp.x()*yx_+dp.y()*yy_+dp.z()*yz_;
      double z = dp.x()*zx_+dp.y()*zy_+dp.z()*zz_;
      if (lambda==1 && lamb_d==0)
        return vgl_vector_3d<double>(x,y,z); // quick return if possible
      vgl_point_3d<double> tp = (*this)(p);
      x -= tp.x() * lamb_d;
      y -= tp.y() * lamb_d;
      z -= tp.z() * lamb_d;
      x /= lambda+lamb_d; // may be division by 0 - in that case delta=infinity
      y /= lambda+lamb_d;
      z /= lambda+lamb_d;
      return vgl_vector_3d<double>(x,y,z);
    }
  }

  return vgl_vector_3d<double>(); // To keep over-zealous compilers happy
}


//=======================================================================
// Method: inverse
//=======================================================================

mil3d_transform_3d mil3d_transform_3d::inverse() const
{
  if (!inv_uptodate_) calcInverse();

  mil3d_transform_3d inv;

  inv.xx_ = xx2_; inv.xy_ = xy2_; inv.xz_ = xz2_; inv.xt_ = xt2_;
  inv.yx_ = yx2_; inv.yy_ = yy2_; inv.yz_ = yz2_; inv.yt_ = yt2_;
  inv.zx_ = zx2_; inv.zy_ = zy2_; inv.zz_ = zz2_; inv.zt_ = zt2_;
  inv.tx_ = tx2_; inv.ty_ = ty2_; inv.tz_ = tz2_; inv.tt_ = tt2_;

  inv.xx2_ = xx_; inv.xy2_ = xy_; inv.xz2_ = xz_; inv.xt2_ = xt_;
  inv.yx2_ = yx_; inv.yy2_ = yy_; inv.yz2_ = yz_; inv.yt2_ = yt_;
  inv.zx2_ = zx_; inv.zy2_ = zy_; inv.zz2_ = zz_; inv.zt2_ = zt_;
  inv.tx2_ = tx_; inv.ty2_ = ty_; inv.tz2_ = tz_; inv.tt2_ = tt_;

  inv.form_ = form_;
  inv.inv_uptodate_ = true;

  return inv;
}

//=======================================================================
// Method: calcInverse
//=======================================================================

void mil3d_transform_3d::calcInverse() const
{
  xx2_=yy2_=zz2_=tt2_=1;
  xy2_ = xz2_= xt2_ = yx2_ = yz2_= yt2_ = zx2_ = zy2_ = zt2_ = tx2_ = ty2_ = tz2_ = 0;

  switch (form_)
  {
   case Identity :
    break;
   case Translation :
    xt2_=-xt_;
    yt2_=-yt_;
    zt2_=-zt_;
    break;
   case ZoomOnly :
    xx2_=1.0/xx_;
    yy2_=1.0/yy_;
    zz2_=1.0/zz_;
    xt2_=-xt_/xx_;
    yt2_=-yt_/yy_;
    zt2_=-zt_/zz_;
    break;
   case RigidBody :
    // transpose x,y,z part
    xx2_=xx_;
    xy2_=yx_;
    xz2_=zx_;
    yx2_=xy_;
    yy2_=yy_;
    yz2_=zy_;
    zx2_=xz_;
    zy2_=yz_;
    zz2_=zz_;
    xt2_=-(xx2_*xt_ + xy2_*yt_ + xz2_*zt_);
    yt2_=-(yx2_*xt_ + yy2_*yt_ + yz2_*zt_);
    zt2_=-(zx2_*xt_ + zy2_*yt_ + zz2_*zt_);
    break;
   case Similarity :
   case Affine :
    {
      double det=-xx_*yy_*zz_+xx_*zy_*yz_+yx_*xy_*zz_-yx_*zy_*xz_-zx_*xy_*yz_+zx_*yy_*xz_;
      if (det==0)
      {
        vcl_cerr<<"mill_transform_3d() : No inverse exists for this affine transform (det==0)\n";
        vcl_abort();
      }

      xx2_=(-yy_*zz_+zy_*yz_)/det;
      xy2_=( xy_*zz_-zy_*xz_)/det;
      xz2_=(-xy_*yz_+yy_*xz_)/det;
      xt2_=(xy_*yz_*zt_-xy_*yt_*zz_-yy_*xz_*zt_+yy_*xt_*zz_+zy_*xz_*yt_-zy_*xt_*yz_)/det;

      yx2_=( yx_*zz_-zx_*yz_)/det;
      yy2_=(-xx_*zz_+zx_*xz_)/det;
      yz2_=( xx_*yz_-yx_*xz_)/det;
      yt2_=(-xx_*yz_*zt_+xx_*yt_*zz_+yx_*xz_*zt_-yx_*xt_*zz_-zx_*xz_*yt_+zx_*xt_*yz_)/det;

      zx2_=(-yx_*zy_+zx_*yy_)/det;
      zy2_=( xx_*zy_-zx_*xy_)/det;
      zz2_=(-xx_*yy_+yx_*xy_)/det;
      zt2_=( xx_*yy_*zt_-xx_*yt_*zy_-yx_*xy_*zt_+yx_*xt_*zy_+zx_*xy_*yt_-zx_*xt_*yy_)/det;

      break;
    }
   case Projective:
    {
      // subdivide m in four 2x2 matrices:
      double A[4], B[4], C[4], D[4];
      A[0] = xx_; A[1] = xy_; B[0] = xz_; B[1] = xt_;
      A[2] = yx_; A[3] = yy_; B[2] = yz_; B[3] = yt_;
      C[0] = zx_; C[1] = zy_; D[0] = zz_; D[1] = zt_;
      C[2] = tx_; C[3] = ty_; D[2] = tz_; D[3] = tt_;
      // Now solve the matrix eqns A*Ai+B*Ci=I, A*Bi+B*Di=0=C*Ai+D*Ci, C*Bi+D*Di=I:
      double det_B = B[0]*B[3] - B[1]*B[2];
      double det_D = D[0]*D[3] - D[1]*D[2];
      if (det_B==0 || det_D==0)
      {
        vcl_cerr<<"mill_transform_3d() : No inverse exists for this projective transform\n";
        vcl_abort();
      }
      // DC = inverse(D)*C
      double DC[4];
      DC[0]=(D[3]*C[0]-D[2]*C[2])/det_D;
      DC[1]=(D[3]*C[1]-D[2]*C[3])/det_D;
      DC[2]=(D[0]*C[2]-D[1]*C[0])/det_D;
      DC[3]=(D[0]*C[3]-D[1]*C[1])/det_D;
      // BA = inverse(B)*A
      double BA[4];
      BA[0]=(B[3]*A[0]-B[2]*A[2])/det_B;
      BA[1]=(B[3]*A[1]-B[2]*A[3])/det_B;
      BA[2]=(B[0]*A[2]-B[1]*A[0])/det_B;
      BA[3]=(B[0]*A[3]-B[1]*A[1])/det_B;
      // Ai = inverse(A-B*DC)
      double Ai[4];
      Ai[3]= A[0]-B[0]*DC[0]-B[1]*DC[2];
      Ai[2]=-A[1]+B[0]*DC[1]+B[1]*DC[3];
      Ai[1]=-A[2]+B[2]*DC[0]+B[3]*DC[2];
      Ai[0]= A[3]-B[2]*DC[1]-B[3]*DC[3];
      double det_Ai = Ai[0]*Ai[3]-Ai[1]*Ai[2];
      Ai[0]/=det_Ai; Ai[1]/=det_Ai; Ai[2]/=det_Ai; Ai[3]/=det_Ai;
      // Bi = inverse(C-D*BA)
      double Bi[4];
      Bi[3]= C[0]-D[0]*BA[0]-D[1]*BA[2];
      Bi[2]=-C[1]+D[0]*BA[1]+D[1]*BA[3];
      Bi[1]=-C[2]+D[2]*BA[0]+D[3]*BA[2];
      Bi[0]= C[3]-D[2]*BA[1]-D[3]*BA[3];
      double det_Bi = Bi[0]*Bi[3]-Bi[1]*Bi[2];
      Bi[0]/=det_Bi; Bi[1]/=det_Bi; Bi[2]/=det_Bi; Bi[3]/=det_Bi;
      // Ci = -DC*Ai
      double Ci[4];
      Ci[0]=-DC[0]*Ai[0]-DC[1]*Ai[2];
      Ci[1]=-DC[0]*Ai[1]-DC[1]*Ai[3];
      Ci[2]=-DC[2]*Ai[0]-DC[3]*Ai[2];
      Ci[3]=-DC[2]*Ai[1]-DC[3]*Ai[3];
      // Di = -BA*Bi
      double Di[4];
      Di[0]=-BA[0]*Bi[0]-BA[1]*Bi[2];
      Di[1]=-BA[0]*Bi[1]-BA[1]*Bi[3];
      Di[2]=-BA[2]*Bi[0]-BA[3]*Bi[2];
      Di[3]=-BA[2]*Bi[1]-BA[3]*Bi[3];
      // recompose the resulting 4x4 matrix:
      xx2_ = Ai[0]; xy2_ = Ai[1]; xz2_ = Bi[0]; xt2_ = Bi[1];
      yx2_ = Ai[2]; yy2_ = Ai[3]; yz2_ = Bi[2]; yt2_ = Bi[3];
      zx2_ = Ci[0]; zy2_ = Ci[1]; zz2_ = Di[0]; zt2_ = Di[1];
      tx2_ = Ci[2]; ty2_ = Ci[3]; tz2_ = Di[2]; tt2_ = Di[3];
      break;
    }
   case Undefined:
    vcl_cerr<<"mil3d_transform_3d::calcInverse() : Unexpected form Undefined\n";
    break;
   default:
    vcl_cerr<<"mil3d_transform_3d::calcInverse() : Unrecognised form: "<<int(form_)<<vcl_endl;
    vcl_abort();
  }

  inv_uptodate_=true;
}

//=======================================================================
// Method: operator==
//=======================================================================

bool mil3d_transform_3d::operator==(const mil3d_transform_3d& t) const
{
  return
    xx_ == t.xx_ &&
    yy_ == t.yy_ &&
    zz_ == t.zz_ &&
    tt_ == t.tt_ &&
    xy_ == t.xy_ &&
    xz_ == t.xz_ &&
    xt_ == t.xt_ &&
    yx_ == t.yx_ &&
    yz_ == t.yz_ &&
    yt_ == t.yt_ &&
    zx_ == t.zx_ &&
    zy_ == t.zy_ &&
    zt_ == t.zt_ &&
    tx_ == t.tx_ &&
    ty_ == t.ty_ &&
    tz_ == t.tz_ ;
}

//=======================================================================
// Method: operator*
//=======================================================================

mil3d_transform_3d operator*(const mil3d_transform_3d& L, const mil3d_transform_3d& R)
{
  mil3d_transform_3d T;

  if (L.form() == mil3d_transform_3d::Identity)
    return R;
  else
  if (R.form() == mil3d_transform_3d::Identity)
    return L;
  else
  {
  /// full multiplication - inefficient but works for
  // arbitrary 4*4 transformation matrix
    T.xx_ = L.xx_*R.xx_ + L.xy_*R.yx_+ L.xz_*R.zx_ + L.xt_*R.tx_;
    T.xy_ = L.xx_*R.xy_ + L.xy_*R.yy_+ L.xz_*R.zy_ + L.xt_*R.ty_;
    T.xz_ = L.xx_*R.xz_ + L.xy_*R.yz_+ L.xz_*R.zz_ + L.xt_*R.tz_;
    T.xt_ = L.xx_*R.xt_ + L.xy_*R.yt_+ L.xz_*R.zt_ + L.xt_*R.tt_;

    T.yx_ = L.yx_*R.xx_ + L.yy_*R.yx_+ L.yz_*R.zx_ + L.yt_*R.tx_;
    T.yy_ = L.yx_*R.xy_ + L.yy_*R.yy_+ L.yz_*R.zy_ + L.yt_*R.ty_;
    T.yz_ = L.yx_*R.xz_ + L.yy_*R.yz_+ L.yz_*R.zz_ + L.yt_*R.tz_;
    T.yt_ = L.yx_*R.xt_ + L.yy_*R.yt_+ L.yz_*R.zt_ + L.yt_*R.tt_;

    T.zx_ = L.zx_*R.xx_ + L.zy_*R.yx_+ L.zz_*R.zx_ + L.zt_*R.tx_;
    T.zy_ = L.zx_*R.xy_ + L.zy_*R.yy_+ L.zz_*R.zy_ + L.zt_*R.ty_;
    T.zz_ = L.zx_*R.xz_ + L.zy_*R.yz_+ L.zz_*R.zz_ + L.zt_*R.tz_;
    T.zt_ = L.zx_*R.xt_ + L.zy_*R.yt_+ L.zz_*R.zt_ + L.zt_*R.tt_;

    T.tx_ = L.tx_*R.xx_ + L.ty_*R.yx_+ L.tz_*R.zx_ + L.tt_*R.tx_;
    T.ty_ = L.tx_*R.xy_ + L.ty_*R.yy_+ L.tz_*R.zy_ + L.tt_*R.ty_;
    T.tz_ = L.tx_*R.xz_ + L.ty_*R.yz_+ L.tz_*R.zz_ + L.tt_*R.tz_;
    T.tt_ = L.tx_*R.xt_ + L.ty_*R.yt_+ L.tz_*R.zt_ + L.tt_*R.tt_;

    // now set the type using the type of L and R
    if (R.form() == L.form())
      T.form_ = R.form();
    else
    {
      if (R.form() == mil3d_transform_3d::Affine ||
          L.form() == mil3d_transform_3d::Affine)
        T.form_ = mil3d_transform_3d::Affine;
      else
      if (R.form() == mil3d_transform_3d::Similarity ||
          L.form() == mil3d_transform_3d::Similarity)
        T.form_ = mil3d_transform_3d::Similarity;
      else
      if (R.form() == mil3d_transform_3d::RigidBody ||
          L.form() == mil3d_transform_3d::RigidBody)
      {
        if (R.form() == mil3d_transform_3d::ZoomOnly)
        {
          if (R.xx_ == R.yy_ &&
              R.xx_ == R.zz_)
            T.form_ = mil3d_transform_3d::Similarity;
          else
            T.form_ = mil3d_transform_3d::Affine;
        }
        else
        if (L.form() == mil3d_transform_3d::ZoomOnly)
        {
          if (L.xx_ == L.yy_ &&
              L.xx_ == L.zz_)
            T.form_ = mil3d_transform_3d::Similarity;
          else
            T.form_ = mil3d_transform_3d::Affine;
        }
        else
          T.form_ = mil3d_transform_3d::RigidBody;
      }
      else
      if (R.form() == mil3d_transform_3d::ZoomOnly ||
          L.form() == mil3d_transform_3d::ZoomOnly)
        T.form_ = mil3d_transform_3d::ZoomOnly;
      else
        T.form_ = mil3d_transform_3d::Translation;
    }
  }

  T.inv_uptodate_ = false;

  return T;
}

//=======================================================================
// Method: version_no
//=======================================================================

short mil3d_transform_3d::version_no() const
{
  return 1;
}

//=======================================================================
// Method: is_a
//=======================================================================

vcl_string mil3d_transform_3d::is_a() const
{
  return vcl_string("mil3d_transform_3d");
}

//=======================================================================
// Method: is_class
//=======================================================================

bool mil3d_transform_3d::is_class(vcl_string const& s) const
{
  return s==is_a();
}

//=======================================================================
// Method: print
//=======================================================================

void mil3d_transform_3d::print_summary(vcl_ostream& o) const
{
  o << vsl_indent()<< "Form: ";
  vsl_indent_inc(o);
  switch (form_)
  {
   case Identity:
    o << "Identity";
    break;
   case Translation:
   {
    vnl_vector<double> p(3);
    params(p);
    o << "Translation (" << p(0) << ',' << p(1) << ',' << p(2) << ')';
    break;
   }
   case ZoomOnly:
   {
    vnl_vector<double> p(6);
    params(p);
    o << "ZoomOnly\n"
      << vsl_indent()<< "scale factor = (" << p(0) << ',' << p(1) << ',' << p(2) << ")\n"
      << vsl_indent()<< "translation  = (" << p(3) << ',' << p(4) << ',' << p(5) << ")\n";
    break;
   }
   case RigidBody:
   {
    vnl_vector<double> p(6);
    params(p);
    o << "RigidBody\n"
      << vsl_indent()<< "angles = " << p(0) << ',' << p(1) << ',' << p(2) << vcl_endl
      << vsl_indent()<< "translation = (" << p(3) << ',' << p(4) << ',' << p(5) << ")\n";
    break;
   }
   case Similarity:
   {
    vnl_vector<double> p(7);
    params(p);
    o << "Similarity\n"
      << vsl_indent()<< "scale factor = " << p(0) << vcl_endl
      << vsl_indent()<< "angles = " << p(1) << ',' << p(2) << ',' << p(3) << vcl_endl
      << vsl_indent()<< "translation = (" << p(4) << ',' << p(5) << ',' << p(5) << ")\n";
    break;
   }
   case Affine:
   {
    vnl_vector<double> p(9);
    params(p);
    o << "Affine\n"
      << vsl_indent()<< "scale factors = " << p(0) << ',' << p(1) << ',' << p(2) << vcl_endl
      << vsl_indent()<< "angles = " << p(3) << ',' << p(4) << ',' << p(5) << vcl_endl
      << vsl_indent()<< "translation = (" << p(6) << ',' << p(7) << ',' << p(8) << ")\n";
    break;
   }
   case Projective:
    o << "Projective\n";
    break;
   case Undefined:
    o << "Undefined\n";
    break;
  }
  vsl_indent_dec(o);
}

//=======================================================================
// Method: save
//=======================================================================

  // required if data is present in this class
void mil3d_transform_3d::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,int(form_));
  vsl_b_write(bfs,xx_); vsl_b_write(bfs,xy_); vsl_b_write(bfs,xz_); vsl_b_write(bfs,xt_);
  vsl_b_write(bfs,yx_); vsl_b_write(bfs,yy_); vsl_b_write(bfs,yz_); vsl_b_write(bfs,yt_);
  vsl_b_write(bfs,zx_); vsl_b_write(bfs,zy_); vsl_b_write(bfs,zz_); vsl_b_write(bfs,zt_);
  vsl_b_write(bfs,tx_); vsl_b_write(bfs,ty_); vsl_b_write(bfs,tz_); vsl_b_write(bfs,tt_);
}

//=======================================================================
// Method: load
//=======================================================================

  // required if data is present in this class
void mil3d_transform_3d::b_read(vsl_b_istream& bfs)
{
  short version;
  vsl_b_read(bfs,version);
  int f;
  switch (version)
  {
   case 1:
    vsl_b_read(bfs,f); form_=Form(f);
    vsl_b_read(bfs,xx_); vsl_b_read(bfs,xy_); vsl_b_read(bfs,xz_); vsl_b_read(bfs,xt_);
    vsl_b_read(bfs,yx_); vsl_b_read(bfs,yy_); vsl_b_read(bfs,yz_); vsl_b_read(bfs,yt_);
    vsl_b_read(bfs,zx_); vsl_b_read(bfs,zy_); vsl_b_read(bfs,zz_); vsl_b_read(bfs,zt_);
    vsl_b_read(bfs,tx_); vsl_b_read(bfs,ty_); vsl_b_read(bfs,tz_); vsl_b_read(bfs,tt_);
    break;
   default:
    vcl_cerr<<"mil3d_transform_3d::load : Illegal version number "<< version << '\n';
    vcl_abort();
  }

  inv_uptodate_ = 0;
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const mil3d_transform_3d& b)
{
  b.b_write(bfs);
}

//=======================================================================
// Associated function: operator>>
//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, mil3d_transform_3d& b)
{
  b.b_read(bfs);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os,const mil3d_transform_3d& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
  return os;
}

//================== end of mil3d_transform_3d.cxx ======================
