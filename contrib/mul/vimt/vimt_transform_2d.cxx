// This is mul/vimt/vimt_transform_2d.cxx
//:
//  \file

#include <iostream>
#include <cmath>
#include <cstdlib>
#include "vimt_transform_2d.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vsl/vsl_indent.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_det.h>
#include <vnl/vnl_math.h>

vnl_matrix<double> vimt_transform_2d::matrix() const
{
    vnl_matrix<double> M(3,3);
    matrix(M);
    return M;
}

void vimt_transform_2d::matrix(vnl_matrix<double>& M) const
{
    M.set_size(3,3);
    double**m_data = M.data_array();
    m_data[0][0]=xx_;   m_data[0][1]=xy_;   m_data[0][2]=xt_;
    m_data[1][0]=yx_;   m_data[1][1]=yy_;   m_data[1][2]=yt_;
    m_data[2][0]=tx_;   m_data[2][1]=ty_;   m_data[2][2]=tt_;
}

//=======================================================================
// Define the transform in terms of a 3x3 homogeneous matrix.
vimt_transform_2d& vimt_transform_2d::set_matrix(const vnl_matrix<double>& M)
{
    if (M.rows()!=3 || M.cols()!=3)
    {
        std::cerr<<"vimt_transform_2d::set_matrix(mat): mat not 3x3\n";
        std::abort();
    }

    form_=Affine;
    xx_=M[0][0]; xy_=M[0][1]; xt_=M[0][2];
    yx_=M[1][0]; yy_=M[1][1]; yt_=M[1][2];
    tx_=M[2][0]; ty_=M[2][1]; tt_=M[2][2];

    return *this;
}


void vimt_transform_2d::params_of(vnl_vector<double>& v, Form form) const
{
    double *v_data;
    switch (form)
    {
        case (Identity):
            v.set_size(0);
            break;
        case (Translation):
            v.set_size(2);
            v(0)=xt_; v(1)=yt_;
            break;
        case (ZoomOnly):
            v.set_size(4);
            v(0)=xx_; v(1)=yy_;
            v(2)=xt_; v(3)=yt_;
            break;
        case (RigidBody):
            v.set_size(3);
            v(0)=std::atan2(-xy_,xx_); // Angle
            v(1)=xt_; v(2)=yt_;
            break;
        case (Reflection):
            v.set_size(4);
            v_data = v.begin();
            v_data[0]=xx_; v_data[1]=xy_;
            v_data[2]=xt_; v_data[3]=yt_;
            break;
        case (Similarity):
            v.set_size(4);
            v_data = v.begin();
            v_data[0]=xx_; v_data[1]=xy_;
            v_data[2]=xt_; v_data[3]=yt_;
            break;
        case (Affine):
            v.set_size(6);
            v_data = v.begin();
            v_data[0]=xx_; v_data[1]=xy_; v_data[2]=xt_;
            v_data[3]=yx_; v_data[4]=yy_; v_data[5]=yt_;
            break;
        case (Projective):
            v.set_size(9);
            v_data = v.begin();
            v_data[0]=xx_; v_data[1]=xy_; v_data[2]=xt_;
            v_data[3]=yx_; v_data[4]=yy_; v_data[5]=yt_;
            v_data[6]=tx_; v_data[7]=ty_; v_data[8]=tt_;
            break;
        default:
            std::cerr<<"vimt_transform_2d::params() Unexpected form: "<<int(form)<<'\n';
            std::abort();
    }
}

//=======================================================================
vimt_transform_2d& vimt_transform_2d::simplify(double tol /*=1e-10*/)
{
    double r;
    double sx, sy;
    double det;
    switch (form_)
    {
        case Affine:
        {
            r = std::atan2(-xy_,xx_);
            double matrix_form[]= {xx_, yx_, xy_, yy_};
            vnl_matrix_fixed<double, 2, 2> X(matrix_form);
            vnl_matrix_fixed<double, 2, 2> S2 = X.transpose() * X;
            // if X=R*S then X'X = S'*R'*R*S
            // if R is a rotation matrix then R'*R=I and so X'X = S'*S = [s_x^2 0 0; 0 s_y^2 0]
            if (S2(0,1)*S2(0,1) + S2(1,0)*S2(1,0) >= tol*tol*6)
                return *this;

            // mirroring if det is negative;
            double mirror=vnl_math::sgn(vnl_det(X[0], X[1]));

            sx = std::sqrt(std::abs(S2(0,0))) * mirror;
            sy = std::sqrt(std::abs(S2(1,1))) * mirror;
            if (vnl_math::sqr(sx-sy) < tol*tol)
                return this->set_similarity(sx, r, xt_, yt_ ).simplify();
            else if (r*r < tol*tol)
                return this->set_zoom_only(sx, sy, xt_, yt_).simplify();
            else if (vnl_math::sqr(std::abs(r) - vnl_math::pi)< tol)
                return this->set_zoom_only(-sx, -sy, xt_, yt_).simplify();
            else
                return *this;
        }
        case Similarity:
            r = std::atan2(-xy_,xx_);

            det=+xx_*yy_-yx_*xy_;
            sx=std::sqrt(xx_*xx_ + yx_*yx_)* vnl_math::sgn(det);

            if (r*r < tol*tol)
                return this->set_zoom_only(sx, xt_, yt_).simplify();
            else if (vnl_math::sqr(std::abs(r) - vnl_math::pi)< tol)
                return this->set_zoom_only(-sx, xt_, yt_).simplify();
            else if (vnl_math::sqr(sx-1.0) < tol*tol)
                return this->set_rigid_body(r, xt_, yt_).simplify();
            else
                return *this;

        case RigidBody:
            r = std::atan2(-xy_,xx_);

            if (r*r < tol*tol)
                return this->set_translation(xt_, yt_).simplify();
            else
                return *this;
        case ZoomOnly:
            if (vnl_math::sqr(xx_-1.0) + vnl_math::sqr(yy_-1.0) < tol*tol)
                return set_translation(xt_, yt_);
            else
                return *this;
        case Translation:
            if (xt_*xt_+yt_*yt_<tol*tol)
                return set_identity();
            else
                return *this;
        case Identity:
            return *this;
        default:
            std::cerr << "vimt3d_transform_3d::simplify() Unexpected form:" <<  form_ << '\n';
            std::abort();
    }
    return *this;
}

void vimt_transform_2d::setCheck(int n1,int n2,const char* str) const
{
    if (n1==n2) return;
    std::cerr<<"vimt_transform_2d::set() "<<n1<<" parameters required for "
            <<str<<". Passed "<<n2<<'\n';
    std::abort();
}

vimt_transform_2d& vimt_transform_2d::set(const vnl_vector<double>& v, Form form)
{
    int n=v.size();
    const double* v_data = v.begin();

    switch (form)
    {
        case (Identity):
            return set_identity();
        case (Translation):
            setCheck(2,n,"Translation");
            return set_translation(v_data[0],v_data[1]);
        case (ZoomOnly):
            setCheck(4,n,"ZoomOnly");
            return set_zoom_only(v_data[0],v_data[1],v_data[2],v_data[3]);
        case (RigidBody):
            setCheck(3,n,"RigidBody");
            return set_rigid_body(v_data[0],v_data[1],v_data[2]);
        case (Reflection):
            setCheck(4,n,"Reflection");
            form_ = Reflection;
            inv_uptodate_=false;
            xx_ = v_data[0]; xy_ = v_data[1];
            yx_ = xy_; yy_ = -xx_;
            xt_ = v_data[2]; yt_ = v_data[3];
            return *this;
        case (Similarity):
            setCheck(4,n,"Similarity");
            form_ = Similarity;
            inv_uptodate_=false;
            xx_ = v_data[0]; xy_ = v_data[1];
            yx_ = -xy_; yy_=xx_;
            xt_ = v_data[2]; yt_ = v_data[3];
            return *this;
        case (Affine):
            setCheck(6,n,"Affine");
            form_ = Affine;
            inv_uptodate_=false;
            xx_ = v_data[0]; xy_ = v_data[1]; xt_ = v_data[2];
            yx_ = v_data[3]; yy_ = v_data[4]; yt_ = v_data[5];
            return *this;
        case (Projective):
            setCheck(9,n,"Projective");
            form_ = Projective;
            inv_uptodate_=false;
            xx_ = v_data[0]; xy_ = v_data[1]; xt_ = v_data[2];
            yx_ = v_data[3]; yy_ = v_data[4]; yt_ = v_data[5];
            tx_ = v_data[6]; ty_ = v_data[7]; tt_ = v_data[8];
            return *this;
        default:
            std::cerr<<"vimt_transform_2d::set() Unexpected form: "<<int(form)<<'\n';
            std::abort();
    }
    return *this;
}


vimt_transform_2d& vimt_transform_2d::set_identity()
{
    if (form_==Identity) return *this;
    form_=Identity;
    inv_uptodate_=false;
    xx_=yy_=tt_=1.0;
    xy_=xt_=yx_=yt_=tx_=ty_=0.0;
    return *this;
}

vimt_transform_2d& vimt_transform_2d::set_translation(double t_x, double t_y)
{
    if (t_x==0 && t_y==0)
        return set_identity();
    else
    {
        form_=Translation;
        inv_uptodate_=false;
        xx_=yy_=tt_=1.0;
        xy_=0.0;
        yx_=0.0;
        tx_=ty_=0.0;
        xt_=t_x;
        yt_=t_y;
        return *this;
    }
}

vimt_transform_2d& vimt_transform_2d::set_origin( const vgl_point_2d<double> & p )
{
    if (form_ == Identity) form_=Translation;
    inv_uptodate_=false;
    xt_ = p.x()*tt_;
    yt_ = p.y()*tt_;
    return *this;
}

vimt_transform_2d& vimt_transform_2d::set_zoom_only(double s_x, double s_y, double t_x, double t_y)
{
    form_=ZoomOnly;
    inv_uptodate_=false;
    xx_=s_x;   yy_=s_y;   tt_=1.0;
    xt_=t_x;   yt_=t_y;
    xy_=yx_=tx_=ty_=0.0;
    return *this;
}

//: reflect about a line though the points m1, and m2
vimt_transform_2d& vimt_transform_2d::set_reflection( const vgl_point_2d<double> & m1, const vgl_point_2d<double> & m2)
{
    form_=Reflection;

    assert (m1 != m2);
    const double m1x = m1.x();
    const double m1y = m1.y();
    const double m2x = m2.x();
    const double m2y = m2.y();
    const double dx = m2x - m1x;
    const double dy = m2y - m1y;
    const double dx2dy2 = dx*dx + dy*dy;

// after plugging all the equations for mirroring into matlab symbolic calculator,
// I had to rearrange the equations to avoid divide-by-zero. See notebook for details. IMS

    xx_ = (dx*dx - dy*dy) / dx2dy2;

    xy_ = 2.0*dx*dy / dx2dy2;

    xt_ = (2.0*m1x*dy*dy - 2.0*m1y*dx*dy) / dx2dy2;

    yx_ = 2.0*dx*dy / dx2dy2;

    yy_ = (dy*dy - dx*dx) / dx2dy2;

    yt_ = (2.0*m1y*dx*dx - 2.0*m1x*dx*dy) / dx2dy2;

    tx_ = ty_ = 0.0;
    tt_ = 1.0;

    return *this;
}

vimt_transform_2d& vimt_transform_2d::set_rigid_body(double theta, double t_x, double t_y)
{
    if (theta==0.0)
        return set_translation(t_x,t_y);
    else
    {
        form_=RigidBody;
        inv_uptodate_=false;
        double a=std::cos(theta);
        double b=std::sin(theta);
        xx_=a;   yx_=b;   tx_=0.0;
        xy_=-b;  yy_=a;   ty_=0.0;
        xt_=t_x; yt_=t_y; tt_=1.0;
        return *this;
    }
}

vimt_transform_2d& vimt_transform_2d::set_similarity(double s, double theta, double t_x, double t_y)
{
    if (s==1.0)
        return set_rigid_body(theta,t_x,t_y);
    else
    {
        form_=Similarity;
        inv_uptodate_=false;
        double a=s*std::cos(theta);
        double b=s*std::sin(theta);
        xx_=a;   yx_=b;   tx_=0.0;
        xy_=-b;  yy_=a;   ty_=0.0;
        xt_=t_x; yt_=t_y; tt_=1.0;
        return *this;
    }
}

//: Sets Euclidean transformation.
vimt_transform_2d& vimt_transform_2d::set_similarity(const vgl_point_2d<double> & dx,
                                                     const vgl_point_2d<double> & t)
{
    form_=Similarity;
    inv_uptodate_=false;
    xx_ = dx.x();  yx_ = dx.y(); tx_=0.0;
    xy_ = -dx.y(); yy_ = dx.x(); ty_=0.0;
    xt_ = t.x();   yt_ = t.y();  tt_=1.0;
    return *this;
}

//: Sets Euclidean transformation.
vimt_transform_2d& vimt_transform_2d::set_similarity(const vgl_vector_2d<double> & dx,
                                                     const vgl_point_2d<double> & t)
{
    form_=Similarity;
    inv_uptodate_=false;
    xx_ = dx.x();  yx_ = dx.y(); tx_=0.0;
    xy_ = -dx.y(); yy_ = dx.x(); ty_=0.0;
    xt_ = t.x();   yt_ = t.y();  tt_=1.0;
    return *this;
}


vimt_transform_2d& vimt_transform_2d::set_affine(const vnl_matrix<double>& M23)  // 2x3 matrix
{
    if ((M23.rows()!=2) || (M23.columns()!=3))
    {
        std::cerr<<"vimt_transform_2d::affine : Expect 2x3 matrix, got "<<M23.rows()<<" x "<<M23.columns()<<'\n';
        std::abort();
    }

    const double *const *m_data=M23.data_array();

    if (m_data[0][0]*m_data[1][1] < m_data[0][1]*m_data[1][0])
    {
        std::cerr << "vimt_transform_2d::affine :\n"
                 << "sub (2x2) matrix should have positive determinant\n";
        std::abort();
    }

    form_=Affine;
    inv_uptodate_=false;
    xx_=m_data[0][0];   xy_=m_data[0][1]; xt_=m_data[0][2];
    yx_=m_data[1][0];   yy_=m_data[1][1]; yt_=m_data[1][2];
    tx_=0.0;            ty_=0.0;          tt_=1.0;
    return *this;
}

//: Sets to be 2D affine transformation T(x,y)=p+x.u+y.v
vimt_transform_2d& vimt_transform_2d::set_affine(const vgl_point_2d<double> & p,
                                                 const vgl_vector_2d<double> & u,
                                                 const vgl_vector_2d<double> & v)
{
    form_=Affine;
    inv_uptodate_=false;
    xt_ = p.x();
    yt_ = p.y();
    xx_ = u.x();
    yx_ = u.y();
    xy_ = v.x();
    yy_ = v.y();
    return *this;
}

vimt_transform_2d& vimt_transform_2d::set_projective(const vnl_matrix<double>& M33)   // 3x3 matrix
{
    if ((M33.rows()!=3) || (M33.columns()!=3))
    {
        std::cerr<<"vimt_transform_2d::projective : Expect 3x3 matrix, got "<<M33.rows()<<" x "<<M33.columns()<<'\n';
        std::abort();
    }

    form_=Projective;
    inv_uptodate_=false;
    const double *const *m_data=M33.data_array();
    xx_=m_data[0][0];   xy_=m_data[0][1]; xt_=m_data[0][2];
    yx_=m_data[1][0];   yy_=m_data[1][1]; yt_=m_data[1][2];
    tx_=m_data[2][0];   ty_=m_data[2][1]; tt_=m_data[2][2];
    return *this;
}

vgl_point_2d<double>  vimt_transform_2d::operator()(double x, double y) const
{
    double z;
    switch (form_)
    {
        case Identity :
            return {x,y};
        case Translation :
            return {x+xt_,y+yt_};
        case ZoomOnly :
            return {x*xx_+xt_,y*yy_+yt_};
        case RigidBody :
        case Similarity :
        case Reflection :
        case Affine :
            return vgl_point_2d<double> (x*xx_+y*xy_+xt_,x*yx_+y*yy_+yt_);
        case Projective :
            z=x*tx_+y*ty_+tt_;
            if (z==0) return vgl_point_2d<double> (0,0);   // Or should it be inf,inf?
            else      return vgl_point_2d<double> ((x*xx_+y*xy_+xt_)/z,(x*yx_+y*yy_+yt_)/z);
        default:
            std::cerr<<"vimt_transform_2d::operator() : Unrecognised form:"<<int(form_)<<'\n';
            std::abort();
    }

    return {}; // To keep over-zealous compilers happy
}

vgl_vector_2d<double>  vimt_transform_2d::delta(const vgl_point_2d<double>& p, const vgl_vector_2d<double>& dp) const
{
    switch (form_)
    {
        case Identity :
        case Translation:
            return dp;
        case ZoomOnly :
            return {dp.x()*xx_,dp.y()*yy_};
        case RigidBody :
        case Similarity :
        case Reflection :
        case Affine :
            return {dp.x()*xx_+dp.y()*xy_,dp.x()*yx_+dp.y()*yy_};
        case Projective :
            return operator()(p+dp)-operator()(p);
        default:
            std::cerr<<"vimt_transform_2d::delta() : Unrecognised form:"<<int(form_)<<'\n';
            std::abort();
    }

    return {}; // To keep over-zealous compilers happy
}


vimt_transform_2d vimt_transform_2d::inverse() const
{
    if (!inv_uptodate_) calcInverse();

    vimt_transform_2d inv;

    inv.xx_ = xx2_; inv.xy_ = xy2_; inv.xt_ = xt2_;
    inv.yx_ = yx2_; inv.yy_ = yy2_; inv.yt_ = yt2_;
    inv.tx_ = tx2_; inv.ty_ = ty2_; inv.tt_ = tt2_;

    inv.xx2_ = xx_; inv.xy2_ = xy_; inv.xt2_ = xt_;
    inv.yx2_ = yx_; inv.yy2_ = yy_; inv.yt2_ = yt_;
    inv.tx2_ = tx_; inv.ty2_ = ty_; inv.tt2_ = tt_;

    inv.form_ = form_;
    inv.inv_uptodate_ = true;

    return inv;
}

void vimt_transform_2d::calcInverse()  const
{
    xx2_ = yy2_ = tt2_ = 1;
    xy2_ = xt2_ = yx2_ = yt2_ = tx2_ = ty2_ = 0;

    switch (form_)
    {
        case Identity :
            break;
        case Translation :
            xt2_ = -xt_;
            yt2_ = -yt_;
            break;
        case ZoomOnly :
            assert(xx_ != 0 && yy_ != 0);
            xx2_=1.0/xx_;
            xt2_=-xt_/xx_;
            yy2_=1.0/yy_;
            yt2_=-yt_/yy_;
            break;
        case RigidBody :
            xx2_ = xx_; xy2_ = yx_;
            yx2_ = xy_; yy2_ = yy_;
            xt2_ = -(xx2_*xt_ + xy2_*yt_);
            yt2_ = -(yx2_*xt_ + yy2_*yt_);
            break;
        case Similarity :
        case Affine :
        {
            double det = xx_*yy_-xy_*yx_;
            if (det==0)
            {
                std::cerr<<"vimt_transform_2d::calcInverse() : No inverse exists for this affine transform (det==0)\n";
                std::abort();
            }
            xx2_=yy_/det;   xy2_=-xy_/det;
            yx2_=-yx_/det;   yy2_=xx_/det;
            xt2_=-xx2_*xt_-xy2_*yt_;
            yt2_=-yx2_*xt_-yy2_*yt_;
            break;
        }
        case Projective :
        {
            vnl_matrix<double> M(3,3),M_inv(3,3);
            matrix(M);
            M_inv = vnl_inverse(M);
            double **m_data=M_inv.data_array();
            xx2_=m_data[0][0];   xy2_=m_data[0][1]; xt2_=m_data[0][2];
            yx2_=m_data[1][0];   yy2_=m_data[1][1]; yt2_=m_data[1][2];
            tx2_=m_data[2][0];   ty2_=m_data[2][1]; tt2_=m_data[2][2];
            break;
        }
        default:
            std::cerr<<"vimt_transform_2d::calcInverse() : Unrecognised form:"<<int(form_)<<'\n';
            std::abort();
    }

    inv_uptodate_=true;
}

bool vimt_transform_2d::operator==(const vimt_transform_2d& t) const
{
    return
        xx_ == t.xx_ &&
        xy_ == t.xy_ &&
        xt_ == t.xt_ &&
        yx_ == t.yx_ &&
        yy_ == t.yy_ &&
        yt_ == t.yt_ &&
        tx_ == t.tx_ &&
        ty_ == t.ty_ &&
        tt_ == t.tt_;
}

//: Transform composition (L*R)(x) = L(R(x))
vimt_transform_2d operator*(const vimt_transform_2d& L, const vimt_transform_2d& R)
{
                // Default is identity_
    vimt_transform_2d T;

    if (L.form() == vimt_transform_2d::Identity)
        return R;
    else
    if (R.form() == vimt_transform_2d::Identity)
        return L;
    else
    if (L.form() == vimt_transform_2d::Translation)
    {
        T = R;

        if (R.form() == vimt_transform_2d::Projective)
        {
            T.xx_ += L.xt_*R.tx_;
            T.xy_ += L.xt_*R.ty_;
            T.xt_ += L.xt_*R.tt_;

            T.yx_ += L.yt_*R.tx_;
            T.yy_ += L.yt_*R.ty_;
            T.yt_ += L.yt_*R.tt_;
        }
        else
        {
            T.xt_ += L.xt_;
            T.yt_ += L.yt_;
        }
    }
    else
    if (R.form() == vimt_transform_2d::Translation)
    {
        T = L;

        T.xt_ += L.xx_*R.xt_ +
                L.xy_*R.yt_;
        T.yt_ += L.yx_*R.xt_ +
                L.yy_*R.yt_;
        T.tt_ += L.tx_*R.xt_ +
                L.ty_*R.yt_;
    }
    else
    {
        if (R.form() == vimt_transform_2d::Projective ||
            L.form() == vimt_transform_2d::Projective)
        {
                            // full monty_...
            T.xx_ = L.xx_*R.xx_ + L.xy_*R.yx_ + L.xt_*R.tx_;
            T.xy_ = L.xx_*R.xy_ + L.xy_*R.yy_ + L.xt_*R.ty_;
            T.xt_ = L.xx_*R.xt_ + L.xy_*R.yt_ + L.xt_*R.tt_;
            T.yx_ = L.yx_*R.xx_ + L.yy_*R.yx_ + L.yt_*R.tx_;
            T.yy_ = L.yx_*R.xy_ + L.yy_*R.yy_ + L.yt_*R.ty_;
            T.yt_ = L.yx_*R.xt_ + L.yy_*R.yt_ + L.yt_*R.tt_;
            T.tx_ = L.tx_*R.xx_ + L.ty_*R.yx_ + L.tt_*R.tx_;
            T.ty_ = L.tx_*R.xy_ + L.ty_*R.yy_ + L.tt_*R.ty_;
            T.tt_ = L.tx_*R.xt_ + L.ty_*R.yt_ + L.tt_*R.tt_;
        }
        else
        {
                            // Affine, Similarity, Reflection
                            // ZoomOnly, RigidBody
            T.xx_ = L.xx_*R.xx_ + L.xy_*R.yx_;
            T.xy_ = L.xx_*R.xy_ + L.xy_*R.yy_;
            T.xt_ = L.xx_*R.xt_ + L.xy_*R.yt_ + L.xt_;
            T.yx_ = L.yx_*R.xx_ + L.yy_*R.yx_;
            T.yy_ = L.yx_*R.xy_ + L.yy_*R.yy_;
            T.yt_ = L.yx_*R.xt_ + L.yy_*R.yt_ + L.yt_;
        }

                            // now set the type using the type of L and R
        if (R.form() == L.form())
            T.form_ = R.form();
        else
        {
            if (R.form() == vimt_transform_2d::Projective ||
                L.form() == vimt_transform_2d::Projective)
                T.form_ = vimt_transform_2d::Projective;
            else
            if (R.form() == vimt_transform_2d::Affine ||
                L.form() == vimt_transform_2d::Affine)
                T.form_ = vimt_transform_2d::Affine;
            else
            if (R.form() == vimt_transform_2d::Reflection ||
                L.form() == vimt_transform_2d::Reflection)
                T.form_ = vimt_transform_2d::Affine;
            else
            if (R.form() == vimt_transform_2d::Similarity ||
                L.form() == vimt_transform_2d::Similarity)
                T.form_ = vimt_transform_2d::Similarity;
            else
            if (R.form() == vimt_transform_2d::RigidBody ||
                L.form() == vimt_transform_2d::RigidBody)
            {
                if (R.form() == vimt_transform_2d::ZoomOnly)
                    if (R.xx_ == R.yy_)
                        T.form_ = vimt_transform_2d::Similarity;
                    else
                        T.form_ = vimt_transform_2d::Affine;
                else
                if (L.form() == vimt_transform_2d::ZoomOnly)
                    if (L.xx_ == L.yy_)
                        T.form_ = vimt_transform_2d::Similarity;
                    else
                        T.form_ = vimt_transform_2d::Affine;
                else
                    T.form_ = vimt_transform_2d::RigidBody;
            }
            else
            if (R.form() == vimt_transform_2d::ZoomOnly ||
                L.form() == vimt_transform_2d::ZoomOnly)
                T.form_ = vimt_transform_2d::ZoomOnly;
            else
                T.form_ = vimt_transform_2d::Translation;
        }

                // make sure det == 1 for rigid body (prevents
                // accumulated rounding errors)
        if (T.form_ == vimt_transform_2d::RigidBody)
        {
            double det = T.xx_*T.yy_ - T.xy_*T.yx_;
            T.xx_ /= det;
            T.xy_ /= det;
            T.yx_ /= det;
            T.yy_ /= det;
        }
    }

    T.inv_uptodate_ = false;
    return T;
}

void vimt_transform_2d::print_summary(std::ostream& o) const
{
    vsl_indent_inc(o);
    switch (form_)
    {
        case Identity:
            o << "Identity";
            break;

        case Translation:
            o << "Translation (" << xt_ << ',' << yt_ << ')';
            break;

        case ZoomOnly:
            o << "ZoomOnly { ";
            if (xx_==yy_)
              o<<"s="<<xx_;
            else
              o<<"sx="<<xx_<<" sy="<<yy_;
            o<<" t=("<<xt_<<','<< yt_<<") }";
            break;

        case RigidBody:
            o << "RigidBody { A="<< std::atan2(yx_,xx_)
              << "t= (" << xt_ << ',' << yt_ <<") }";
            break;

        case Similarity:
            o << "Similarity {"
              << " s= " << std::sqrt(xx_*xx_+xy_*xy_)
              << " A= " << std::atan2(xy_,xx_)
              << " t= (" << xt_ << ',' << yt_ << " ) }";
            break;

        case Reflection:
            o << "Reflection\n"
              << vsl_indent()<< xx_ << ' ' << xy_ << '\n'
              << vsl_indent()<< yx_ << ' ' << yy_ << '\n'
              << vsl_indent()<< "translation = (" << xt_ << ',' << yt_ << ')';
            break;

        case Affine:
            o << "Affine\n"
              << vsl_indent()<< xx_ << ' ' << xy_ << '\n'
              << vsl_indent()<< yx_ << ' ' << yy_ << '\n'
              << vsl_indent()<< "translation = (" << xt_ << ',' << yt_ << ')';
            break;

        case Projective:
            o << "Projective\n"
              << vsl_indent()<< xx_ << ' ' << xy_ << ' ' << xt_ << '\n'
              << vsl_indent()<< yx_ << ' ' << yy_ << ' ' << yt_ << '\n'
              << vsl_indent()<< tx_ << ' ' << ty_ << ' ' << tt_;
            break;
        default:
            assert(!"Invalid form");
    }
    vsl_indent_dec(o);
}

std::ostream& operator<<( std::ostream& os, const vimt_transform_2d& t )
{
    vsl_indent_inc(os);
    t.print_summary(os);
    vsl_indent_dec(os);
    return os;
}

short vimt_transform_2d::version_no() const { return 1; }


void vimt_transform_2d::b_write(vsl_b_ostream& bfs) const
{
    vsl_b_write(bfs,version_no());
    vsl_b_write(bfs,int(form_));
    vsl_b_write(bfs,xx_); vsl_b_write(bfs,xy_); vsl_b_write(bfs,xt_);
    vsl_b_write(bfs,yx_); vsl_b_write(bfs,yy_); vsl_b_write(bfs,yt_);
    vsl_b_write(bfs,tx_); vsl_b_write(bfs,ty_); vsl_b_write(bfs,tt_);
}

void vimt_transform_2d::b_read(vsl_b_istream& bfs)
{
    if (!bfs) return;

    short version;
    vsl_b_read(bfs,version);
    int f;
    switch (version) {
    case 1:
        inv_uptodate_ = false;
        vsl_b_read(bfs,f); form_=Form(f);
        vsl_b_read(bfs,xx_); vsl_b_read(bfs,xy_); vsl_b_read(bfs,xt_);
        vsl_b_read(bfs,yx_); vsl_b_read(bfs,yy_); vsl_b_read(bfs,yt_);
        vsl_b_read(bfs,tx_); vsl_b_read(bfs,ty_); vsl_b_read(bfs,tt_);
        break;
    default:
        std::cerr << "I/O ERROR: vimt_transform_2d::b_read(vsl_b_istream&)\n"
                 << "           Unknown version number "<< version << '\n';
        bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    }
}

void vsl_b_read(vsl_b_istream& bfs,vimt_transform_2d& t)
{
    t.b_read(bfs);
}

void vsl_b_write(vsl_b_ostream& bfs,const vimt_transform_2d& t)
{
    t.b_write(bfs);
}

void vsl_print_summary(std::ostream& os,const vimt_transform_2d& t)
{
    //os << t.is_a() << ": ";
    vsl_indent_inc(os);
    t.print_summary(os);
    vsl_indent_dec(os);
}
