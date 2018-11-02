#ifndef bsta_joint_histogram_3d_hxx_
#define bsta_joint_histogram_3d_hxx_
//:
// \file
#include <iostream>
#include <cmath>
#include "bsta_joint_histogram_3d.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cstdlib>//for div
#include "bsta_gauss.h"
#include <vnl/vnl_math.h> // for log2e == 1/std::log(2.0)
template <class T>
bsta_joint_histogram_3d<T>::bsta_joint_histogram_3d()
  : volume_valid_(false), volume_(0),
    nbins_a_(1), nbins_b_(1), nbins_c_(1),
    range_a_(0), range_b_(0), range_c_(0),
    delta_a_(0), delta_b_(0), delta_c_(0),
    min_a_(0), max_a_(0),
    min_b_(0), max_b_(0),
    min_c_(0), max_c_(0),
    min_prob_(0),
    counts_(1, 1, 1, T(0))
{
  bsta_joint_histogram_3d_base::type_ = bsta_joint_histogram_3d_traits<T>::type();
}

template <class T>
bsta_joint_histogram_3d<T>::bsta_joint_histogram_3d(const T range,
                                                    const unsigned nbins,
                                                    const T min_prob)
  : volume_valid_(false), volume_(0), nbins_a_(nbins), nbins_b_(nbins),
    nbins_c_(nbins), range_a_(range), range_b_(range), range_c_(range),
    delta_a_(0),delta_b_(0), delta_c_(0),
    min_a_(0), max_a_(range),
    min_b_(0), max_b_(range),
    min_c_(0), max_c_(range),
    min_prob_(min_prob),
    counts_(nbins, nbins, nbins, T(0))
{
  bsta_joint_histogram_3d_base::type_ = bsta_joint_histogram_3d_traits<T>::type();

  if (nbins_a_>0&&nbins_b_>0&&nbins_c_>0)
  {
    delta_a_ = range_a_/nbins_a_;
    delta_b_ = range_b_/nbins_b_;
    delta_c_ = range_c_/nbins_c_;
  }
}


template <class T>
bsta_joint_histogram_3d<T>::bsta_joint_histogram_3d(const T range_a,
                                                    const unsigned nbins_a,
                                                    const T range_b,
                                                    const unsigned nbins_b,
                                                    const T range_c,
                                                    const unsigned nbins_c,
                                                    const T min_prob)
  : volume_valid_(false), volume_(0), nbins_a_(nbins_a), nbins_b_(nbins_b),
    nbins_c_(nbins_c), range_a_(range_a), range_b_(range_b),range_c_(range_c),
    delta_a_(0),delta_b_(0),delta_c_(0),
    min_a_(0), max_a_(range_a), min_b_(0), max_b_(range_b),
    min_c_(0), max_c_(range_c), min_prob_(min_prob),
    counts_(nbins_a, nbins_b, nbins_c, T(0))
{
  bsta_joint_histogram_3d_base::type_ = bsta_joint_histogram_3d_traits<T>::type();

  if (nbins_a_>0&&nbins_b_>0&&nbins_c_>0)
  {
    delta_a_ = range_a_/nbins_a_;
    delta_b_ = range_b_/nbins_b_;
    delta_c_ = range_c_/nbins_c_;
  }
}

template <class T>
bsta_joint_histogram_3d<T>::bsta_joint_histogram_3d(const T min, const T max,
                                                    const unsigned nbins,
                                                    const T min_prob)
 : volume_valid_(false), volume_(0), nbins_a_(nbins), nbins_b_(nbins),
    nbins_c_(nbins), min_a_(min), max_a_(max), min_b_(min), max_b_(max),
    min_c_(min), max_c_(max), min_prob_(min_prob),
    counts_(nbins, nbins, nbins, T(0))
{
  bsta_joint_histogram_3d_base::type_ = bsta_joint_histogram_3d_traits<T>::type();

  if (nbins>0) {
    range_a_ = max-min;
    delta_a_ = range_a_/nbins;
    range_b_ = range_a_;
    delta_b_ = delta_a_;
    range_c_ = range_a_;
    delta_c_ = delta_a_;
  }
  else {
    range_a_ = 0;
    delta_a_ = 0;
    range_b_ = 0;
    delta_b_ = 0;
    range_c_ = 0;
    delta_c_ = 0;
  }
}

template <class T>
bsta_joint_histogram_3d<T>::bsta_joint_histogram_3d(const T min_a, const T max_a,
                                                    const unsigned nbins_a,
                                                    const T min_b, const T max_b,
                                                    const unsigned nbins_b,
                                                    const T min_c, const T max_c,
                                                    const unsigned nbins_c,
                                                    const T min_prob)
  : volume_valid_(false), volume_(0), nbins_a_(nbins_a), nbins_b_(nbins_b),
    nbins_c_(nbins_c), min_a_(min_a), max_a_(max_a),
    min_b_(min_b), max_b_(max_b),
    min_c_(min_c), max_c_(max_c),
    min_prob_(min_prob),
    counts_(nbins_a, nbins_b, nbins_c, T(0))
{
  bsta_joint_histogram_3d_base::type_ = bsta_joint_histogram_3d_traits<T>::type();

  if (nbins_a>0) {
    range_a_ = max_a-min_a;
    delta_a_ = range_a_/nbins_a;
  }
  else {
    range_a_ = 0;
    delta_a_ = 0;
  }
  if (nbins_b>0) {
    range_b_ = max_b-min_b;
    delta_b_ = range_b_/nbins_b;
  }
  else {
    range_b_ = 0;
    delta_b_ = 0;
  }
  if (nbins_c>0) {
    range_c_ = max_c-min_c;
    delta_c_ = range_c_/nbins_c;
  }
  else {
    range_c_ = 0;
    delta_c_ = 0;
  }
}

template <class T>
void bsta_joint_histogram_3d<T>::upcount(T a, T mag_a,
                                         T b, T mag_b,
                                         T c, T mag_c)
{
  int bin_a = -1, bin_b = -1, bin_c = -1;

  if(!this->bin_at_val(a, b, c, bin_a, bin_b, bin_c))
    return;

  counts_[bin_a][bin_b][bin_c] += mag_a + mag_b + mag_c;
  volume_valid_ = false;
}

template <class T>
bool bsta_joint_histogram_3d<T>::bin_at_val(
  const T a, const T b, const T c,
  int& ia, int& ib, int& ic) const
{
  if (a<min_a_||a>max_a_)
    return false;
  if (b<min_b_||b>max_b_)
    return false;
  if (c<min_c_||c>max_c_)
    return false;

  ia = -1, ib = -1, ic = -1;
  for (unsigned i = 0; i<nbins_a_; i++)
    if ((i+1)*delta_a_>=(a-min_a_))
    {
      ia = i;
      break;
    }
  for (unsigned i = 0; i<nbins_b_; i++)
    if ((i+1)*delta_b_>=(b-min_b_))
    {
      ib = i;
      break;
    }
  for (unsigned i = 0; i<nbins_c_; i++)
    if ((i+1)*delta_c_>=(c-min_c_))
    {
      ic = i;
      break;
    }
  if (ia<0||ib<0||ic<0) return false;

  return true;
}

template <class T>
void bsta_joint_histogram_3d<T>::compute_volume() const
{
  volume_=0;
  for (unsigned a = 0; a<nbins_a_; a++)
    for (unsigned b =0; b<nbins_b_; b++)
      for (unsigned c =0; c<nbins_c_; c++)
        volume_ += counts_[a][b][c];
  volume_valid_ = true;
}

template <class T>
T bsta_joint_histogram_3d<T>::p(unsigned a, unsigned b, unsigned c) const
{
  if (a>=nbins_a_)
    return 0;
  if (b>=nbins_b_)
    return 0;
  if (c>=nbins_c_)
    return 0;
  if (!volume_valid_)
    compute_volume();
  if (volume_ == T(0))
    return 0;
  else
    return counts_[a][b][c]/volume_;
}

template <class T>
T bsta_joint_histogram_3d<T>::p(T a, T b, T c) const
{
  if (!volume_valid_) {
    compute_volume();
  }
  int bina, binb, binc;
  if(!bin_at_val(a,b,c,bina,binb,binc))
    return 0;
  return counts_[bina][binb][binc]/volume_;
}


template <class T>
void bsta_joint_histogram_3d<T>::parzen(const T sigma)
{
  if (sigma<=0)
    return;
  double sd = (double)sigma;
  vbl_array_3d<double> in(nbins_a_, nbins_b_, nbins_c_), out;
  for (unsigned int a = 0; a<nbins_a_; a++)
    for (unsigned int b = 0; b<nbins_b_; b++)
      for (unsigned int c = 0; c<nbins_c_; c++)
      in[a][b][c] = (double)counts_[a][b][c];

  bsta_gauss::bsta_3d_gaussian(sd, in, out);

  for (unsigned int a = 0; a<nbins_a_; a++)
    for (unsigned int b = 0; b<nbins_b_; b++)
      for (unsigned int c = 0; c<nbins_c_; c++)
        counts_[a][b][c] = (T)out[a][b][c];
}

//: The average and variance bin value for row a using counts to compute probs
//  T avg_and_variance_bin_for_row_a(const unsigned a) const;
#if 0
template <class T>
bool bsta_joint_histogram_3d<T>::avg_and_variance_bin_for_row_a(const unsigned a, T & avg, T & var) const
{
  if (a >= nbins_a_)
    return false;

  T sum = 0;
  for (unsigned b =0; b<nbins_b_; b++)
    sum += counts_[a][b];

  if (sum <= 0)
    return false;

  avg = 0;
  for (unsigned b =0; b<nbins_b_; b++)
    avg += ((b+1)*delta_b_/2)*(counts_[a][b]/sum);

  var = 0;
  for (unsigned b =0; b<nbins_b_; b++) {
    T dif = (b+1)*delta_b_/2-avg;
    var += std::pow(dif, T(2.0))*(counts_[a][b]/sum);
  }

  return true;
}
#endif
template <class T>
T bsta_joint_histogram_3d<T>::volume() const
{
  if (!volume_valid_)
    compute_volume();
  return volume_;
}

template <class T>
T bsta_joint_histogram_3d<T>::entropy() const
{
  T ent = 0;
  for (unsigned i = 0; i<nbins_a_; ++i)
    for (unsigned j = 0; j<nbins_b_; ++j)
      for (unsigned k = 0; k<nbins_c_; ++k)
      {
        T pijk = this->p(i,j,k);
        if (pijk>min_prob_)
          ent -= pijk*T(std::log(pijk));
      }
  ent *= (T)vnl_math::log2e;
  return ent;
}

#if 0
template <class T>
void bsta_joint_histogram_3d<T>::parzen(const T sigma)
{
  if (sigma<=0)
    return;
  double sd = (double)sigma;
  vbl_array_2d<double> in(nbins_a_, nbins_b_), out;
  for (unsigned row = 0; row<nbins_a_; row++)
    for (unsigned col = 0; col<nbins_b_; col++)
      in[row][col] = (double)counts_[row][col];

  bsta_gauss::bsta_2d_gaussian(sd, in, out);

  for (unsigned row = 0; row<nbins_a_; row++)
    for (unsigned col = 0; col<nbins_b_; col++)
      counts_[row][col] = (T)out[row][col];
}
#endif

template <class T>
T bsta_joint_histogram_3d<T>::get_count(T a, T b, T c) const
{
  T pv = this->p(a,b,c);
  if (volume_valid_)
    return pv*volume_;
  return pv*this->volume();
}

template <class T>
void bsta_joint_histogram_3d<T>::
bin_max_count(unsigned& ia, unsigned& ib, unsigned & ic) const
{
  ia = 0; ib = 0; ic = 0;
  for(unsigned i = 0; i < this->counts_.get_row1_count(); ++i)
    for(unsigned j = 0; j < this->counts_.get_row2_count(); ++j)
      for(unsigned k = 0; k < this->counts_.get_row3_count(); ++k)
        if( this->counts_[i][j][k] > this->counts_[ia][ib][ic] )
          ia = i, ib = j, ic = k;
}

template <class T>
void bsta_joint_histogram_3d<T>::clear()
{
  volume_valid_ = false;
  volume_ = 0;
  counts_.fill(T(0));
}

template <class T>
void bsta_joint_histogram_3d<T>::print(std::ostream& os) const
{
  for (unsigned a = 0; a<nbins_a_; a++)
    for (unsigned b = 0; b<nbins_b_; b++)
      for (unsigned c = 0; c<nbins_c_; c++)
        if (p(a,b,c) > 0)
          os << "p[" << a << "][" << b << "][" << c << "]=" << p(a,b,c) << '\n';
}

template <class T>
void bsta_joint_histogram_3d<T>::print_to_vrml(std::ostream& os,
                                               bool relative_prob_scale,
                                               T red, T green, T blue) const
{
  // we need to scale the display, find magnitude of largest value
  T max = (T)0;
  T min_delta = delta_a_;
  if (delta_b_<min_delta)
    min_delta = delta_b_;
  if (delta_c_<min_delta)
    min_delta = delta_c_;
  T rad_scale = min_delta/2;
  if (relative_prob_scale){
  for (unsigned a = 0; a<nbins_a_; a++)
    for (unsigned b = 0; b<nbins_b_; b++)
      for (unsigned c = 0; c<nbins_c_; c++){
        T v = p(a,b,c);
        if (v > max)
          max = v;
      }
  if (max <= T(0)) return;
  rad_scale = min_delta/max/2;
  }
  os << "#VRML V2.0 utf8\n"
     << "Group { children [\n";

  for (unsigned a = 0; a<nbins_a_; a++)
  {
    for (unsigned b = 0; b<nbins_b_; b++)
    {
      for (unsigned c = 0; c<nbins_c_; c++)
      {
        T v = p(a,b,c);
        if (v>min_prob_)
          os << "Transform {\n"
             << "  translation " << a*delta_a_ << ' ' << b*delta_b_
             << ' ' << c*delta_c_ << '\n'
             << "  children Shape {\n"
             << "    geometry Sphere { radius " <<  rad_scale*v << "}\n"
             << "    appearance DEF A1 Appearance {"
             << "      material Material {\n"
             << "        diffuseColor " << red << ' '
             << green << ' ' << blue << '\n'
             << "        emissiveColor .3 0 0\n"
             << "      }\n"
             << "    }\n"
             << "  }\n"
             << "}\n";
      }
    }
  }
  // The bounding box for the 3-d grid
  os << "Transform {\n"
     << "  translation " << delta_a_*(nbins_a_-1)/2.0f << ' '
     << delta_b_*(nbins_b_-1)/2.0f << ' ' << delta_c_*(nbins_c_-1)/2.0f << '\n'
     << "  children Shape {\n"
     << "    geometry Box { size " << delta_a_*(nbins_a_-1)
     << ' ' << delta_b_*(nbins_b_-1)
     << ' ' << delta_c_*(nbins_c_-1) << " }\n"
     << "    appearance Appearance {\n"
     << "      material Material {\n"
     << "       diffuseColor 1 1 1\n"
     << "       transparency 0.8\n"
     << "    }\n"
     << "  }\n"
     << "}\n"
  // the "a" axis
     << "Transform {\n"
     << "  translation " << 0.0 << ' '
     << -delta_b_*(nbins_b_-1)/2.0f <<' '<<-delta_c_*(nbins_c_-1)/2.0f << '\n'
     << "  children Shape {\n"
     << "    geometry Box { size " << delta_a_*(nbins_a_-1)
     << ' ' << 0.05*min_delta
     << ' ' << 0.05*min_delta << " }\n"
     << "    appearance Appearance {\n"
     << "      material Material {\n"
     << "       emissiveColor 1 0 0\n"
     << "       transparency 0.0\n"
     << "    }\n"
     << "  }\n"
     << "}\n"
  // the "b" axis
     << "Transform {\n"
     << "  translation " << -delta_a_*(nbins_a_-1)/2.0f << ' '
     << delta_b_*(nbins_b_-1)/2.0f <<' '<< 0.0 << '\n'
     << "  children Shape {\n"
     << "    geometry Box { size " << 0.05*min_delta
     << ' ' << delta_b_*(nbins_b_-1)
     << ' ' << 0.05*min_delta << " }\n"
     << "    appearance Appearance {\n"
     << "      material Material {\n"
     << "       emissiveColor 0 1 0\n"
     << "       transparency 0.0\n"
     << "    }\n"
     << "  }\n"
     << "}\n"
  // the "c" axis
     << "Transform {\n"
     << "  translation " << 0.0 << ' '
     << -delta_b_*(nbins_b_-1)/2.0f <<' '<< delta_c_*(nbins_c_-1)/2.0f << '\n'
     << "  children Shape {\n"
     << "    geometry Box { size " << 0.05*min_delta
     << ' ' << 0.05*min_delta
     << ' ' << delta_c_*(nbins_c_-1)  << " }\n"
     << "    appearance Appearance {\n"
     << "      material Material {\n"
     << "       emissiveColor 0 0 1\n"
     << "       transparency 0.0\n"
     << "    }\n"
     << "  }\n"
     << "}\n"
  // the background
     << "Background { skyColor 1 1 1 }\n"
     << "NavigationInfo { type \"EXAMINE\" }\n"
     << "] }\n";
}

#if 0
template <class T>
void bsta_joint_histogram_3d<T>::print_to_m(std::ostream& os) const
{
  os << "y = zeros(" << nbins_a_ << ", " << nbins_b_ << ");\n";
  for (unsigned a = 0; a<nbins_a_; a++) {
    for (unsigned b = 0; b<nbins_b_; b++) {
      if (p(a,b) > 0) {
        os << "y(" << a+1 << ", " << b+1 << ") = " << p(a,b) << "; ";
        //os << "y(" << a+1 << ", " << b+1 << ") = " << counts_[a][b] << "; ";
      }
    }
    //os << '\n';
  }
  //os << '\n';
  os << "bar3(y,'detached');\n";
}
#endif // 0

template <class T>
void bsta_joint_histogram_3d<T>::print_to_text(std::ostream& os) const
{
  os << "nbins_a \t nbins_b \t nbins_c\n"
     << nbins_a_ << '\t' << nbins_b_ << '\t' << nbins_c_ << '\n';

  for (unsigned a = 0; a<nbins_a_; a++)
  {
    for (unsigned b = 0; b<nbins_b_; b++)
    {
      for (unsigned c = 0; c<nbins_c_; c++)
      {
        os << get_count(a,b,c) << '\t';
      }
      os << '\n';
    }
    os << '\n';
  }
  os << "\n probs:\n";
  for (unsigned a = 0; a<nbins_a_; a++)
  {
    for (unsigned b = 0; b<nbins_b_; b++)
    {
      for (unsigned c = 0; c<nbins_c_; c++)
      {
        os << p(a,b,c) << '\t';
      }
      os << '\n';
    }
    os << '\n';
  }
}

#undef BSTA_JOINT_HISTOGRAM_3D_INSTANTIATE
#define BSTA_JOINT_HISTOGRAM_3D_INSTANTIATE(T) \
template class bsta_joint_histogram_3d<T >

#endif // bsta_joint_histogram_3d_hxx_
