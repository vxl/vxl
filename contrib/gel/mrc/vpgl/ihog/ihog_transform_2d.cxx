#include "ihog_transform_2d.h"
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_inverse.h>

void ihog_transform_2d::b_write(vsl_b_ostream& bfs) const
{
    vsl_b_write(bfs,version_no());
    vsl_b_write(bfs,int(form_));
    const vgl_h_matrix_2d<double> m(t12_matrix_);
    vsl_b_write(bfs,&m);
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
        vgl_h_matrix_2d<double>::b_read(bfs);
        break;
    default:
        vcl_cerr << "I/O ERROR: ihog_transform_2d::b_read(vsl_b_istream&)\n"
                 << "           Unknown version number "<< version << '\n';
        bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
        return;
    }

    //inv_uptodate_ = false;
}

vgl_point_2d<double>  ihog_transform_2d::origin() const
{ 
  return vgl_point_2d<double> (t12_matrix_[2][2]==1?t12_matrix_[0][2]:t12_matrix_[0][2]/t12_matrix_[2][2],t12_matrix_[2][2]==1?t12_matrix_[1][2]:t12_matrix_[1][2]/t12_matrix_[2][2]); 
}

void ihog_transform_2d::set_origin(const vgl_point_2d<double> & p )
{
    t12_matrix_[0][2] = p.x()*t12_matrix_[2][2];
    t12_matrix_[1][2] = p.y()*t12_matrix_[2][2];
    if (form_ == Identity) form_=Translation;
    //inv_uptodate_=false;
}

void ihog_transform_2d::set_projective(const vnl_matrix<double>& m)
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
   // xx_=s_x;   yy_=s_y;   tt_=1.0;
   // xt_=t_x;   yt_=t_y;
   // xy_=yx_=tx_=ty_=0.0;
   // inv_uptodate_=false;
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
            v(0)=vcl_atan2(-t12_matrix_[0][1],t12_matrix_[0][0]); // Angle
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
            vcl_cerr<<"ihog_transform_2d::params() Unexpected form: "<<int(form)<<vcl_endl;
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
            if (n!=2) return;//setCheck(2,n,"Translation");
            set_translation(v_data[0],v_data[1]);
            break;
        case (RigidBody):
            if (n!=3) return;//setCheck(3,n,"RigidBody");
            set_rigid_body(v_data[0],v_data[1],v_data[2]);
            break;
        case (Affine):
          { if (n!=6) return;//setCheck(6,n,"Affine");
            vnl_matrix<double> M23(2,3,6,v_data);
            vgl_h_matrix_2d<double>::set_affine(M23);
           // xx_ = v_data[0]; xy_ = v_data[1]; xt_ = v_data[2];
           // yx_ = v_data[3]; yy_ = v_data[4]; yt_ = v_data[5];
            form_ = Affine;
           // inv_uptodate_=false;
            break;}
        case (Projective):
          { if (n!=9) return;//setCheck(9,n,"Projective");
            vnl_matrix_fixed<double,3,3> M33(v_data);
            t12_matrix_=M33;
            //xx_ = v_data[0]; xy_ = v_data[1]; xt_ = v_data[2];
            //yx_ = v_data[3]; yy_ = v_data[4]; yt_ = v_data[5];
            //tx_ = v_data[6]; ty_ = v_data[7]; tt_ = v_data[8];
            form_ = Projective;
            //inv_uptodate_=false;
            break;}
        default:
            vcl_cerr<<"ihog_transform_2d::set() Unexpected form: "<<int(form)<<vcl_endl;
    }
}

void ihog_transform_2d::set_rigid_body(double theta, double t_x, double t_y)
{
  if (theta==0.0)
    set_translation(t_x,t_y);
  else{
    set_rotation(theta);
    set_translation(t_x,t_y);
  }
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
void ihog_transform_2d::set_affine(const vnl_matrix<double>& M23)
{
  if ((M23.rows()!=2) || (M23.columns()!=3)) {
    vcl_cerr<<"vimt_transform_2d::affine : Expect 2x3 matrix, got "<<M23.rows()<<" x "<<M23.columns()<<vcl_endl;
    return;
  }

  const double *const *m_data=M23.data_array();

  if (m_data[0][0]*m_data[1][1] < m_data[0][1]*m_data[1][0]) {
      vcl_cerr << "vimt_transform_2d::affine :\n"
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
       // case ZoomOnly :
       //     return vgl_vector_2d<double> (dp.x()*t12_matrix_[0][0],dp.y()*t12_matrix_[1][1]);
        case RigidBody :
        case Affine :
            return vgl_vector_2d<double> (dp.x()*t12_matrix_[0][0]+dp.y()*t12_matrix_[0][1],dp.x()*t12_matrix_[1][0]+dp.y()*t12_matrix_[1][1]);
        case Projective :
            return operator()(p+dp)-operator()(p);
        default:
            vcl_cerr<<"ihog_transform_2d::delta() : Unrecognised form:"<<int(form_)<<vcl_endl;
    }

    return vgl_vector_2d<double> (); // To keep over-zealous compilers happy
}

ihog_transform_2d ihog_transform_2d::inverse() const
{
  vnl_matrix_fixed<double,3,3> result = vnl_inverse(t12_matrix_);

  return ihog_transform_2d(result,form_);
}
#if 0
//: Transform composition (L*R)(x) = L(R(x))
ihog_transform_2d ihog_transform_2d::operator*(const ihog_transform_2d& L, const ihog_transform_2d& R)
{
    // Default is identity_
    ihog_transform_2d T;
    vnl_matrix_fixed<double,3,3> m(L.get_matrix()*R.get_matrix());
    T.set(m);
    /*if (L.form() == ihog_transform_2d::Identity)
        return R;
    else
    if (R.form() == ihog_transform_2d::Identity)
        return L;
    else
    if (L.form() == ihog_transform_2d::Translation)
    {
        T = R;

        if (R.form() == ihog_transform_2d::Projective)
        {
            t12_matrix_[0][0] += L.get(0,2)*R.get(2,0));
            t12_matrix_[0][1] += L.get(0,2)*R.get(2,1));
            t12_matrix_[0][2] += L.get(0,2)*R.get(2,2));

            t12_matrix_[1][0] += L.get(1,2)*R.get(2,0));
            t12_matrix_[1][1] += L.get(1,2)*R.get(2,1));
            t12_matrix_[1][2] += L.get(1,2)*R.get(2,2));

        }
        else
        {
            t12_matrix_[0][2] += L.get(0,2);
            t12_matrix_[1][2] += L.yt_;
        }
    }
    else
    if (R.form() == ihog_transform_2d::Translation)
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
        if (R.form() == ihog_transform_2d::Projective ||
            L.form() == ihog_transform_2d::Projective)
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
            if (R.form() == ihog_transform_2d::Projective ||
                L.form() == ihog_transform_2d::Projective)
                T.form_ = ihog_transform_2d::Projective;
            else
            if (R.form() == ihog_transform_2d::Affine ||
                L.form() == ihog_transform_2d::Affine)
                T.form_ = ihog_transform_2d::Affine;
            else
            if (R.form() == ihog_transform_2d::Reflection ||
                L.form() == ihog_transform_2d::Reflection)
                T.form_ = ihog_transform_2d::Affine;
            else
            if (R.form() == ihog_transform_2d::Similarity ||
                L.form() == ihog_transform_2d::Similarity)
                T.form_ = ihog_transform_2d::Similarity;
            else
            if (R.form() == ihog_transform_2d::RigidBody ||
                L.form() == ihog_transform_2d::RigidBody)
            {
                if (R.form() == ihog_transform_2d::ZoomOnly)
                    if (R.xx_ == R.yy_)
                        T.form_ = ihog_transform_2d::Similarity;
                    else
                        T.form_ = ihog_transform_2d::Affine;
                else
                if (L.form() == ihog_transform_2d::ZoomOnly)
                    if (L.xx_ == L.yy_)
                        T.form_ = ihog_transform_2d::Similarity;
                    else
                        T.form_ = ihog_transform_2d::Affine;
                else
                    T.form_ = ihog_transform_2d::RigidBody;
            }
            else
            if (R.form() == ihog_transform_2d::ZoomOnly ||
                L.form() == ihog_transform_2d::ZoomOnly)
                T.form_ = ihog_transform_2d::ZoomOnly;
            else
                T.form_ = ihog_transform_2d::Translation;
        }

                // make sure det == 1 for rigid body (prevents
                // accumulated rounding errors)
        if (T.form_ == ihog_transform_2d::RigidBody)
        {
            double det = T.xx_*T.yy_ - T.xy_*T.yx_;
            T.xx_ /= det;
            T.xy_ /= det;
            T.yx_ /= det;
            T.yy_ /= det;
        }
    }

    T.inv_uptodate_ = false;*/

    return T;
}
#endif
vgl_point_2d<double> ihog_transform_2d::operator()(double x, double y) const
{
    double z;
    switch (form_)
    {
        case Identity :
            return vgl_point_2d<double> (x,y);
        case Translation :
            return vgl_point_2d<double> (x+t12_matrix_[0][2],y+t12_matrix_[1][2]);
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
            vcl_cerr<<"vimt_transform_2d::operator() : Unrecognised form:"<<int(form_)<<vcl_endl;
           
    }

    return vgl_point_2d<double> (); // To keep over-zealous compilers happy
}

ihog_transform_2d& ihog_transform_2d::operator=(const ihog_transform_2d& R)
{
  this->t12_matrix_ = R.get_matrix();
  this->form_ = R.form();
  return *this;
}