#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_svd.h>
#include <vcl_iostream.h>
#include <bnl/bnl_parabolic_interpolator.h>

void 
bnl_parabolic_interpolator::add_data_point(const double p, const double v)
{
  p_.push_back(p);
  v_.push_back(v);
}

void bnl_parabolic_interpolator::clear()
{
  p_.clear();
  v_.clear();
}

int bnl_parabolic_interpolator::n_points()
{
  return p_.size();
}

void bnl_parabolic_interpolator::fill_scatter_matrix()
{
  int n = this->n_points();
  double p4 = 0, p3 =0, p2 = 0, p2v=0, pv=0, v2=0, p =0, v=0;
  for (int i=0; i<n; i++)
    {
      double pi = p_[i], vi = v_[i];
      double x2 = pi*pi;
      double x3 = x2*pi;
      double x4 = x3*pi;
      double xvi = pi*vi;
      double x2vi = pi*xvi;
      double vi2 = vi*vi;
      p2 += x2;
      p3 += x3;
      p4 += x4;
      pv += xvi;
      p2v += x2vi;
      v2 += vi2;
      p += pi;
      v += vi;
    }
  //solution vector is in the order
  // [Vxx Vx V0 1]
  s_ = vnl_matrix<double>(4,4);
  s_.put(0,0,p4);s_.put(0,1,p3);s_.put(0,2,-p2v);s_.put(0,3,p2);
  s_.put(1,0,p3);s_.put(1,1,p2);s_.put(1,2,-pv);s_.put(1,3,p);
  s_.put(2,0,-p2v);s_.put(2,1,-pv);s_.put(2,2,v2);s_.put(2,3,-v);
  s_.put(3,0,p2);s_.put(3,1,p);s_.put(3,2,-v);s_.put(3,3,n);
}

bool bnl_parabolic_interpolator::solve()
{
  if (this->n_points() < 3)
    return false;
  this->fill_scatter_matrix();
  vnl_svd<double> svd(s_);
  vnl_vector<double> nv = svd.nullvector();
  vcl_cout << "NV: " << nv << '\n';
  if (nv[0] > -1e-6)
    return false;//Vxx is too small
  p_ext_ = -nv[1]/(2.0*nv[0]);
  return true;
}

void bnl_parabolic_interpolator::print()
{
  vcl_cout << "P / V\n";
  for (int i = 0; i<this->n_points(); i++)
    vcl_cout << p_[i] << '\t' << v_[i] << '\n';
  vcl_cout << vcl_flush;
}

