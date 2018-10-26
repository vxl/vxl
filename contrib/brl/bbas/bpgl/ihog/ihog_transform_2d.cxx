#include "ihog_transform_2d.h"
//:
// \file
#include <vnl/vnl_inverse.h>
#include <vgl/io/vgl_io_h_matrix_2d.h>

void ihog_transform_2d::b_write(vsl_b_ostream& bfs) const
{
    vsl_b_write(bfs,version_no());
    vsl_b_write(bfs,int(form_));
    vsl_b_write(bfs,vgl_h_matrix_2d<double>(t12_matrix_));
}

void ihog_transform_2d::b_read(vsl_b_istream& bfs)
{
    if (!bfs) return;

    short version;
    vsl_b_read(bfs,version);
    int f;
    switch (version) {
    case 1:
        vsl_b_read(bfs,f); form_=Form(f);
        vsl_b_read(bfs, (vgl_h_matrix_2d<double>&)(*this));
        break;
    default:
        std::cerr << "I/O ERROR: ihog_transform_2d::b_read(vsl_b_istream&)\n"
                 << "           Unknown version number "<< version << '\n';
        bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
        return;
    }
}

vgl_point_2d<double>  ihog_transform_2d::origin() const
{
  return {t12_matrix_[2][2]==1?t12_matrix_[0][2]:t12_matrix_[0][2]/t12_matrix_[2][2],t12_matrix_[2][2]==1?t12_matrix_[1][2]:t12_matrix_[1][2]/t12_matrix_[2][2]};
}

void ihog_transform_2d::set_origin(const vgl_point_2d<double> & p )
{
    t12_matrix_[0][2] = p.x()*t12_matrix_[2][2];
    t12_matrix_[1][2] = p.y()*t12_matrix_[2][2];
    if (form_ == Identity) form_=Translation;
}

void ihog_transform_2d::set_projective(const vnl_double_3x3& m)
{
  t12_matrix_=m;
}

void ihog_transform_2d::set_zoom_only(double s_x, double s_y, double t_x, double t_y)
{
    // form_=ZoomOnly;
    set_identity();
    t12_matrix_[0][0]=s_x;
    t12_matrix_[1][1]=s_y;
    set_translation(t_x,t_y);
    form_ = Affine;
#if 0
    xx_=s_x;   yy_=s_y;   tt_=1.0;
    xt_=t_x;   yt_=t_y;
    xy_=yx_=tx_=ty_=0.0;
    inv_uptodate_=false;
#endif
}

void ihog_transform_2d::params_of(vnl_vector<double>& v, Form form) const
{
    double *v_data;
    switch (form)
    {
        case (Identity):
            v.set_size(0);
            break;
        case (Translation):
            v.set_size(2);
            v(0)=t12_matrix_[0][2]; v(1)=t12_matrix_[1][2];
            break;
        case (RigidBody):
            v.set_size(3);
            v(0)=std::atan2(-t12_matrix_[0][1],t12_matrix_[0][0]); // Angle
            v(1)=t12_matrix_[0][2]; v(2)=t12_matrix_[1][2];
            break;
        case (Affine):
            v.set_size(6);
            v_data = v.begin();
            v_data[0]=t12_matrix_[0][0]; v_data[1]=t12_matrix_[0][1]; v_data[2]=t12_matrix_[0][2];
            v_data[3]=t12_matrix_[1][0]; v_data[4]=t12_matrix_[1][1]; v_data[5]=t12_matrix_[1][2];
            break;
        case (Projective):
            v.set_size(9);
            v_data = v.begin();
            v_data[0]=t12_matrix_[0][0]; v_data[1]=t12_matrix_[0][1]; v_data[2]=t12_matrix_[0][2];
            v_data[3]=t12_matrix_[1][0]; v_data[4]=t12_matrix_[1][1]; v_data[5]=t12_matrix_[1][2];
            v_data[6]=t12_matrix_[2][0]; v_data[7]=t12_matrix_[2][1]; v_data[8]=t12_matrix_[2][2];
            break;
        default:
            std::cerr<<"ihog_transform_2d::params() Unexpected form: "<<int(form)<<'\n';
    }
}

void ihog_transform_2d::set(const vnl_vector<double>& v, Form form)
{
    int n=v.size();
    const double* v_data = v.begin();

    switch (form)
    {
        case (Identity):
            set_identity();
            break;
        case (Translation):
            if (n!=2) return;
            set_translation(v_data[0],v_data[1]);
            break;
        case (RigidBody):
            if (n!=3) return;
            set_rigid_body(v_data[0],v_data[1],v_data[2]);
            break;
        case (Affine):
            if (n!=6) return;
            {
              double xx = v_data[0], xy = v_data[1], xt = v_data[2],
                     yx = v_data[3], yy = v_data[4], yt = v_data[5];
              vnl_double_2x3 M23(xx,xy,xt,yx,yy,yt);
              vgl_h_matrix_2d<double>::set_affine(M23); }
#if 0
            xx_ = v_data[0]; xy_ = v_data[1]; xt_ = v_data[2];
            yx_ = v_data[3]; yy_ = v_data[4]; yt_ = v_data[5];
            inv_uptodate_=false;
#endif
            form_ = Affine;
            break;
        case (Projective):
            if (n!=9) return;//setCheck(9,n,"Projective");
            { t12_matrix_=vnl_double_3x3(v_data); }
#if 0
            xx_ = v_data[0]; xy_ = v_data[1]; xt_ = v_data[2];
            yx_ = v_data[3]; yy_ = v_data[4]; yt_ = v_data[5];
            tx_ = v_data[6]; ty_ = v_data[7]; tt_ = v_data[8];
            inv_uptodate_=false;
#endif
            form_ = Projective;
            break;
        default:
            std::cerr<<"ihog_transform_2d::set() Unexpected form: "<<int(form)<<'\n';
    }
}

void ihog_transform_2d::set_rigid_body(double theta, double t_x, double t_y)
{
  if (theta==0.0)
    set_translation(t_x,t_y);
  else {
    set_rotation(theta);
    set_translation(t_x,t_y);
  }
  form_=RigidBody;
}

//: Sets to be 2D affine transformation T(x,y)=p+x.u+y.v
void ihog_transform_2d::set_affine(const vgl_point_2d<double> & p,
                                   const vgl_vector_2d<double> & u,
                                   const vgl_vector_2d<double> & v)
{
  t12_matrix_[0][2] = p.x();
  t12_matrix_[1][2] = p.y();
  t12_matrix_[0][0] = u.x();
  t12_matrix_[1][0] = u.y();
  t12_matrix_[0][1] = v.x();
  t12_matrix_[1][1] = v.y();
  form_=Affine;
}

//: Sets to be 2D affine transformation using 2x3 matrix
void ihog_transform_2d::set_affine(const vnl_double_2x3& M23)
{
  if ((M23.rows()!=2) || (M23.columns()!=3)) {
    std::cerr<<"vimt_transform_2d::affine : Expect 2x3 matrix, got "<<M23.rows()<<" x "<<M23.columns()<<'\n';
    return;
  }

  if (M23[0][0]*M23[1][1] < M23[0][1]*M23[1][0]) {
    std::cerr << "vimt_transform_2d::affine :\n"
             << "sub (2x2) matrix should have positive determinant\n";
  }

  vgl_h_matrix_2d<double>::set_affine(M23);
  form_=Affine;
}

vgl_vector_2d<double>
ihog_transform_2d::delta(const vgl_point_2d<double>& p, const vgl_vector_2d<double>& dp) const
{
    switch (form_)
    {
        case Identity :
        case Translation:
            return dp;
#if 0
        case ZoomOnly :
            return vgl_vector_2d<double> (dp.x()*t12_matrix_[0][0],dp.y()*t12_matrix_[1][1]);
#endif
        case RigidBody :
        case Affine :
            return {dp.x()*t12_matrix_[0][0]+dp.y()*t12_matrix_[0][1],dp.x()*t12_matrix_[1][0]+dp.y()*t12_matrix_[1][1]};
        case Projective :
            return operator()(p+dp)-operator()(p);
        default:
            std::cerr<<"ihog_transform_2d::delta() : Unrecognised form: "<<int(form_)<<'\n';
    }

    return {}; // To keep over-zealous compilers happy
}

ihog_transform_2d ihog_transform_2d::inverse() const
{
  vnl_double_3x3 result = vnl_inverse(t12_matrix_);
  return ihog_transform_2d(result,form_);
}

vgl_point_2d<double> ihog_transform_2d::operator()(double x, double y) const
{
    double z;
    switch (form_)
    {
        case Identity :
            return {x,y};
        case Translation :
            return {x+t12_matrix_[0][2],y+t12_matrix_[1][2]};
        case RigidBody :
        case Affine :
            return vgl_point_2d<double> (x*t12_matrix_[0][0]+y*t12_matrix_[0][1]+t12_matrix_[0][2],
                                         x*t12_matrix_[1][0]+y*t12_matrix_[1][1]+t12_matrix_[1][2]);
        case Projective :
            z=x*t12_matrix_[2][0]+y*t12_matrix_[2][1]+t12_matrix_[2][2];
            if (z==0) return vgl_point_2d<double> (0,0);   // Or should it be inf,inf?
            else  return vgl_point_2d<double> ((x*t12_matrix_[0][0]+y*t12_matrix_[0][1]+t12_matrix_[0][2])/z,
                                               (x*t12_matrix_[1][0]+y*t12_matrix_[1][1]+t12_matrix_[1][2])/z);
        default:
            std::cerr<<"vimt_transform_2d::operator() : Unrecognised form: "<<int(form_)<<'\n';
    }

    return vgl_point_2d<double> (); // To keep over-zealous compilers happy
}

ihog_transform_2d& ihog_transform_2d::operator=(const ihog_transform_2d& R)
{
  this->t12_matrix_ = R.get_matrix();
  this->form_ = R.form();
  return *this;
}
