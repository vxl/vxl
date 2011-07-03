#ifndef bsta_joint_histogram_txx_
#define bsta_joint_histogram_txx_
//:
// \file
#include "bsta_joint_histogram.h"

#include <vcl_cmath.h> // for log()
#include <vcl_iostream.h>
#include "bsta_gauss.h"
#include <vnl/vnl_math.h> // for log2e == 1/vcl_log(2.0)
template <class T>
bsta_joint_histogram<T>::bsta_joint_histogram()
  : volume_valid_(false), volume_(0),
    nbins_a_(1), nbins_b_(1),
    range_a_(0), range_b_(0),
    delta_a_(0), delta_b_(0),
    min_a_(0), max_a_(0),
    min_b_(0), max_b_(0),
    min_prob_(0),
    counts_(1, 1, T(0))
{
  bsta_joint_histogram_base::type_ = bsta_joint_histogram_traits<T>::type();
}

template <class T>
bsta_joint_histogram<T>::bsta_joint_histogram(const T range,
                                              const unsigned int nbins,
                                              const T min_prob)
  : volume_valid_(false), volume_(0), nbins_a_(nbins), nbins_b_(nbins),
    range_a_(range), range_b_(range),delta_a_(0),delta_b_(0), min_a_(0),
    max_a_(range), min_b_(0), max_b_(range), min_prob_(min_prob),
    counts_(nbins, nbins, T(0))
{
  bsta_joint_histogram_base::type_ = bsta_joint_histogram_traits<T>::type();
  if (nbins_a_>0&&nbins_b_>0)
  {
    delta_a_ = range_a_/nbins_a_;
    delta_b_ = range_b_/nbins_b_;
  }
}

template <class T>
bsta_joint_histogram<T>::bsta_joint_histogram(const T range_a,
                                              const unsigned int nbins_a,
                                              const T range_b,
                                              const unsigned int nbins_b,
                                              const T min_prob)
  : volume_valid_(false), volume_(0), nbins_a_(nbins_a), nbins_b_(nbins_b),
    range_a_(range_a), range_b_(range_b),delta_a_(0),delta_b_(0),min_a_(0),
    max_a_(range_a), min_b_(0), max_b_(range_b), min_prob_(min_prob),
    counts_(nbins_a, nbins_b, T(0))
{
  bsta_joint_histogram_base::type_ = bsta_joint_histogram_traits<T>::type();
  if (nbins_a_>0&&nbins_b_>0)
  {
    delta_a_ = range_a_/nbins_a_;
    delta_b_ = range_b_/nbins_b_;
  }
}

template <class T>
bsta_joint_histogram<T>::bsta_joint_histogram(const T min_a, const T max_a,
                                              const unsigned int nbins_a,
                                              const T min_b, const T max_b,
                                              const unsigned int nbins_b,
                                              const T min_prob)
  : volume_valid_(false), volume_(0), nbins_a_(nbins_a), nbins_b_(nbins_b),
    min_a_(min_a), max_a_(max_a), min_b_(min_b), max_b_(max_b),
    min_prob_(min_prob), counts_(nbins_a, nbins_b, T(0))
{
  bsta_joint_histogram_base::type_ = bsta_joint_histogram_traits<T>::type();
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
}

template <class T>
void bsta_joint_histogram<T>::upcount(T a, T mag_a,
                                      T b, T mag_b)
{
  if (a<min_a_||a>max_a_)
    return;
  if (b<min_b_||b>max_b_)
    return;
  int bin_a =-1, bin_b = -1;
  for (unsigned int i = 0; i<nbins_a_; i++)
    if ((i+1)*delta_a_>=(a-min_a_))
    {
      bin_a = i;
      break;
    }
  for (unsigned int i = 0; i<nbins_b_; i++)
    if ((i+1)*delta_b_>=(b-min_b_))
    {
      bin_b = i;
      break;
    }
  if (bin_a<0||bin_b<0) return;
  T v = counts_[bin_a][bin_b]+ mag_a + mag_b;
  counts_.put(bin_a, bin_b, v);
  volume_valid_ = false;
}

template <class T>
void bsta_joint_histogram<T>::compute_volume() const
{
  volume_=0;
  for (unsigned int a = 0; a<nbins_a_; a++)
    for (unsigned int b =0; b<nbins_b_; b++)
      volume_ += counts_[a][b];
  volume_valid_ = true;
}

template <class T>
T bsta_joint_histogram<T>::p(unsigned int a, unsigned int b) const
{
  if (a>=nbins_a_)
    return 0;
  if (b>=nbins_b_)
    return 0;
  if (!volume_valid_)
    compute_volume();
  if (volume_ == T(0))
    return 0;
  else
    return counts_[a][b]/volume_;
}

template <class T>
T bsta_joint_histogram<T>::p(T a, T b) const
{
  if (a<min_a_||a>max_a_)
    return 0;
  if (b<min_b_||b>max_b_)
    return 0;
  if (!volume_valid_)
    compute_volume();
  if (volume_ == T(0))
    return 0;
  unsigned r = 0, c = 0;
  bool found = false;
  for (unsigned ia = 0; (ia<nbins_a_)&&!found; ++ia)
    if ((ia+1)*delta_a_>=(a-min_a_)) {
      r = ia;
      found = true;
    }
  if (!found)
    return 0;
  found = false;
  for (unsigned ib = 0; (ib<nbins_b_)&&!found; ++ib)
    if ((ib+1)*delta_b_>=(b-min_b_)) {
      c = ib;
      found = true;
    }
  if (!found)
    return false;
  return counts_[r][c]/volume_;
}

//: The average and variance bin value for row a using counts to compute probs
//  T avg_and_variance_bin_for_row_a(const unsigned int a) const;
template <class T>
bool bsta_joint_histogram<T>::avg_and_variance_bin_for_row_a(const unsigned int a, T & avg, T & var) const
{
  if (a >= nbins_a_)
    return false;

  T sum = 0;
  for (unsigned int b =0; b<nbins_b_; b++)
    sum += counts_[a][b];

  if (sum <= 0)
    return false;

  avg = 0;
  for (unsigned int b =0; b<nbins_b_; b++)
    avg += ((b+1)*delta_b_/2)*(counts_[a][b]/sum);

  var = 0;
  for (unsigned int b =0; b<nbins_b_; b++) {
    T dif = (b+1)*delta_b_/2-avg;
    var += vcl_pow(dif, T(2.0))*(counts_[a][b]/sum);
  }

  return true;
}

template <class T>
T bsta_joint_histogram<T>::volume() const
{
  if (!volume_valid_)
    compute_volume();
  return volume_;
}

template <class T>
T bsta_joint_histogram<T>::entropy() const
{
  T ent = 0;
  for (unsigned int i = 0; i<nbins_a_; ++i)
    for (unsigned int j = 0; j<nbins_b_; ++j)
    {
      T pij = this->p(i,j);
      if (pij>min_prob_)
        ent -= pij*T(vcl_log(pij));
    }
  ent *= (T)vnl_math::log2e;
  return ent;
}

template <class T>
T bsta_joint_histogram<T>::mutual_information() const
{
  T mutual_information = T(0);

  //calculate marginal distributions
  vcl_vector<T> pa(nbins_a_,T(0)),pb(nbins_b_,T(0));
  for (unsigned a = 0; a < nbins_a_; ++a)
    for (unsigned b = 0; b < nbins_b_; ++b)
    {
      pa[a] += this->p(a,b);
      pb[b] += this->p(a,b);
    }

  //calculate mutual information in base 10
  for (unsigned a = 0; a < nbins_a_; ++a)
    for (unsigned b = 0; b < nbins_b_; ++b)
      if (p(a,b) != 0 && pa[a] != 0 && pb[b] != 0)
        mutual_information+=this->p(a,b)*(vcl_log(this->p(a,b)) - (vcl_log(pa[a]) + vcl_log(pb[b])) );

  //convert mutual information to base 2
  mutual_information *= (T)vnl_math::log2e;

  return mutual_information;
}

template <class T>
T bsta_joint_histogram<T>::renyi_entropy() const
{
  T ent = 0, sum = 0;
  for (unsigned int i = 0; i<nbins_a_; ++i)
    for (unsigned int j = 0; j<nbins_b_; ++j)
    {
      T pij = this->p(i,j);
      sum += pij*pij;
    }
  if (sum>min_prob_)
    ent = - T(vcl_log(sum))*(T)vnl_math::log2e;
  return ent;
}

template <class T>
T bsta_joint_histogram<T>::entropy_marginal_a() const
{
  T ent = 0;
  vcl_vector<T> counts_a(nbins_a_, T(0));
  T count_a_sum = T(0);
  for (unsigned int i = 0; i<nbins_a_; ++i)
    for (unsigned int j = 0; j <nbins_b_; ++j)
    {
      counts_a[i] += this->get_count(i,j);
      count_a_sum += this->get_count(i,j);
    }

  for (unsigned int i = 0; i <nbins_a_; ++i) {
    T pi = counts_a[i]/count_a_sum;
    if (pi>min_prob_)
      ent -= pi*T(vcl_log(pi));
  }
  ent *= (T)vnl_math::log2e;
  return ent;
}

template <class T>
void bsta_joint_histogram<T>::parzen(const T sigma)
{
  if (sigma<=0)
    return;
  double sd = (double)sigma;
  vbl_array_2d<double> in(nbins_a_, nbins_b_), out;
  for (unsigned int row = 0; row<nbins_a_; row++)
    for (unsigned int col = 0; col<nbins_b_; col++)
      in[row][col] = (double)counts_[row][col];

  bsta_gauss::bsta_2d_gaussian(sd, in, out);

  for (unsigned int row = 0; row<nbins_a_; row++)
    for (unsigned int col = 0; col<nbins_b_; col++)
      counts_[row][col] = (T)out[row][col];
}

template <class T>
T bsta_joint_histogram<T>::get_count(T a, T b) const
{
  T pv = this->p(a,b);
  if (volume_valid_)
    return pv*volume_;
  return pv*this->volume();
}

template <class T>
void bsta_joint_histogram<T>::clear()
{
  volume_valid_ = false;
  volume_ = 0;
  counts_.fill(T(0));
}

template <class T>
void bsta_joint_histogram<T>::print(vcl_ostream& os) const
{
  for (unsigned int a = 0; a<nbins_a_; a++)
    for (unsigned int b = 0; b<nbins_b_; b++)
      if (p(a,b) > 0)
        os << "p[" << a << "][" << b << "]=" << p(a,b) << '\n';
}

template <class T>
void bsta_joint_histogram<T>::print_to_vrml(vcl_ostream& os) const
{
  // we need to scale the display, find magnitude of largest value
  T max = (T)0;
  for (unsigned int a = 0; a<nbins_a_; a++)
    for (unsigned int b = 0; b<nbins_b_; b++)
      if (p(a,b) > max)
        max = p(a,b);
  float avg = static_cast<float>(0.5*(nbins_a_ + nbins_b_));
  os << "#VRML V2.0 utf8\n"
     << "Group { children [\n";

  for (unsigned int a = 0; a<nbins_a_; a++)
  {
    for (unsigned int b = 0; b<nbins_b_; b++)
    {
      float height = (max > 0) ? float((p(a,b)/max)*avg) : 0.0f;
      os << "Transform {\n"
         << "  translation " << a << ' ' << b << ' ' << height << '\n'
         << "  children Shape {\n"
         << "    geometry Sphere { radius 0.2 }\n"
         << "    appearance DEF A1 Appearance {"
         << "      material Material {\n"
         << "        diffuseColor 1 0 0\n"
         << "        emissiveColor .3 0 0\n"
         << "      }\n"
         << "    }\n"
         << "  }\n"
         << "}\n"
         << "Transform {\n"
         << "  translation " << a << ' ' << b << ' ' << height/2.0 << '\n'
         << "  rotation 1 0 0 " << vnl_math::pi/2.0 << '\n'
         << "  children Shape {\n"
         << "    appearance USE A1\n"
         << "    geometry Cylinder { radius 0.05 height " << height << " }\n"
         << "  }\n"
         << "}\n";
    }
  }
  os << "Transform {\n"
     << "  translation " << (nbins_a_-1)/2.0f << ' ' << (nbins_b_-1)/2.0f << " 0\n"
     << "  children Shape {\n"
     << "    geometry Box { size " << nbins_a_-1 << ' ' << nbins_b_-1 << " 0.3 }\n"
     << "    appearance Appearance {\n"
     << "      material Material { diffuseColor 0.8 0.8 0.8 }\n"
     << "    }\n"
     << "  }\n"
     << "}\n"
  // draw a green box to designate the origin of the histogram
     << "Transform {\n"
     << "  translation 0.0 0.0 0.0\n"
     << "  children Shape {\n"
     << "    geometry Box { size " << "1.0 1.0 1.0 }\n"
     << "    appearance Appearance {\n"
     << "      material Material { diffuseColor 0.0 1.0 0.0 }\n"
     << "    }\n"
     << "  }\n"
     << "}\n"
  // draw a red box to designate the "a" axis  of the histogram
     << "Transform {\n"
     << "  translation " << (nbins_a_-1)/2.0f << " 0 0\n"
     << "  children Shape {\n"
     << "    geometry Box { size " << (nbins_a_-1) << " 0.5 0.5 }\n"
     << "    appearance Appearance {\n"
     << "      material Material { diffuseColor 1.0 0.0 0.0 }\n"
     << "    }\n"
     << "  }\n"
     << "}\n"
  // draw a cyan box to designate the "b" axis  of the histogram
     << "Transform {\n"
     << "  translation 0 " << (nbins_b_-1)/2.0f << " 0\n"
     << "  children Shape {\n"
     << "    geometry Box { size " << 0.5 << ' ' << (nbins_b_-1) << " 0.5 }\n"
     << "    appearance Appearance {\n"
     << "      material Material { diffuseColor 0.0 0.8 0.8 }\n"
     << "    }\n"
     << "  }\n"
     << "}\n"
  // draw background
     << "Background { skyColor 1 1 1 }\n"
     << "NavigationInfo { type \"EXAMINE\" }\n"
     << "] }\n";
}

template <class T>
void bsta_joint_histogram<T>::print_to_m(vcl_ostream& os) const
{
  os << "y = zeros(" << nbins_a_ << ", " << nbins_b_ << ");\n";
  for (unsigned int a = 0; a<nbins_a_; a++) {
    for (unsigned int b = 0; b<nbins_b_; b++) {
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

template <class T>
void bsta_joint_histogram<T>::print_to_text(vcl_ostream& os) const
{
  os << nbins_a_ << '\t' << nbins_b_ << '\n';
  for (unsigned int a = 0; a<nbins_a_; a++)
  {
    for (unsigned int b = 0; b<nbins_b_; b++)
    {
      os << get_count(a,b) << '\t';
    }
    os << '\n';
  }
  os << "\n probs:\n";
  for (unsigned int a = 0; a<nbins_a_; a++)
  {
    for (unsigned int b = 0; b<nbins_b_; b++)
    {
      os << p(a,b) << '\t';
    }
    os << '\n';
  }
}

#undef BSTA_JOINT_HISTOGRAM_INSTANTIATE
#define BSTA_JOINT_HISTOGRAM_INSTANTIATE(T) \
template class bsta_joint_histogram<T >

#endif // bsta_joint_histogram_txx_
