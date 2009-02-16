#ifndef bsta_joint_histogram_txx_
#define bsta_joint_histogram_txx_

#include "bsta_joint_histogram.h"

#include <vcl_cmath.h> // for log()
#include <vcl_iostream.h>
#include "bsta_gauss.h"
#include <vnl/vnl_math.h> // for log2e == 1/vcl_log(2.0)

template <class T>
bsta_joint_histogram<T>::bsta_joint_histogram(const T range,
                                              const unsigned int nbins,
                                              const T min_prob)
  : volume_valid_(false), volume_(0), nbins_(nbins), range_(range), delta_(0)
{
  if (nbins>0)
  {
    delta_ = range_/nbins;
    counts_.resize(nbins, nbins);
    counts_.fill(T(0));
  }
  min_prob_ = min_prob;
}

template <class T>
void bsta_joint_histogram<T>::upcount(T a, T mag_a,
                                      T b, T mag_b)
{
  if (a<0||a>range_)
    return;
  if (b<0||b>range_)
    return;
  int bin_a =0, bin_b = 0;
  for (unsigned int i = 0; i<nbins_; i++)
    if ((i+1)*delta_>=a)
    {
      bin_a = i;
      break;
    }
  for (unsigned int i = 0; i<nbins_; i++)
    if ((i+1)*delta_>=b)
    {
      bin_b = i;
      break;
    }
  T v = counts_[bin_a][bin_b]+ mag_a + mag_b;
  counts_.put(bin_a, bin_b, v);
  volume_valid_ = false;
}

template <class T>
void bsta_joint_histogram<T>::compute_volume() const
{
  volume_=0;
  for (unsigned int a = 0; a<nbins_; a++)
    for (unsigned int b =0; b<nbins_; b++)
      volume_ += counts_[a][b];
  volume_valid_ = true;
}

template <class T>
T bsta_joint_histogram<T>::p(unsigned int a, unsigned int b) const
{
  if (a>=nbins_)
    return 0;
  if (b>=nbins_)
    return 0;
  if (!volume_valid_)
    compute_volume();
  if (volume_ == T(0))
    return 0;
  else
    return counts_[a][b]/volume_;
}

//: The average and variance bin value for row a using counts to compute probs
//  T avg_and_variance_bin_for_row_a(const unsigned int a) const;
template <class T>
bool bsta_joint_histogram<T>::avg_and_variance_bin_for_row_a(const unsigned int a, T & avg, T & var) const
{
  if (a >= nbins_)
    return false;
    
  T sum = 0;
  for (unsigned int b =0; b<nbins_; b++)
    sum += counts_[a][b];
    
  if (sum <= 0)
    return false;
  
  avg = 0;
  for (unsigned int b =0; b<nbins_; b++)
    avg += ((b+1)*delta_/2)*(counts_[a][b]/sum);
  
  var = 0;
  for (unsigned int b =0; b<nbins_; b++) {
    T dif = (b+1)*delta_/2-avg;
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
  for (unsigned int i = 0; i<nbins_; ++i)
    for (unsigned int j = 0; j<nbins_; ++j)
    {
      T pij = this->p(i,j);
      if (pij>min_prob_)
        ent -= pij*T(vcl_log(pij));
    }
  ent *= (T)vnl_math::log2e;
  return ent;
}

template <class T>
T bsta_joint_histogram<T>::renyi_entropy() const
{
  T ent = 0, sum = 0;
  for (unsigned int i = 0; i<nbins_; ++i)
    for (unsigned int j = 0; j<nbins_; ++j)
    {
      T pij = this->p(i,j);
      sum += pij*pij;
    }
  if (sum>min_prob_)
    ent = - T(vcl_log(sum))*(T)vnl_math::log2e;
  return ent;
}

template <class T>
void bsta_joint_histogram<T>::parzen(const T sigma)
{
  if (sigma<=0)
    return;
  double sd = (double)sigma;
  vbl_array_2d<double> in(nbins_, nbins_), out;
  for (unsigned int row = 0; row<nbins_; row++)
    for (unsigned int col = 0; col<nbins_; col++)
      in[row][col] = (double)counts_[row][col];

  bsta_gauss::bsta_2d_gaussian(sd, in, out);

  for (unsigned int row = 0; row<nbins_; row++)
    for (unsigned int col = 0; col<nbins_; col++)
      counts_[row][col] = (T)out[row][col];
}

template <class T>
void bsta_joint_histogram<T>::print(vcl_ostream& os) const
{
  for (unsigned int a = 0; a<nbins_; a++)
    for (unsigned int b = 0; b<nbins_; b++)
      if (p(a,b) > 0)
        os << "p[" << a << "][" << b << "]=" << p(a,b) << '\n';
}

template <class T>
void bsta_joint_histogram<T>::print_to_vrml(vcl_ostream& os) const
{
  // we need to scale the display, find magnitude of largest value
  T max = (T)0;
  for (unsigned int a = 0; a<nbins_; a++)
    for (unsigned int b = 0; b<nbins_; b++)
      if (p(a,b) > max)
        max = p(a,b);
        
  os << "#VRML V2.0 utf8\n";
  os << "Group { children [\n";
  
  for (unsigned int a = 0; a<nbins_; a++) {
    for (unsigned int b = 0; b<nbins_; b++) {
      if (p(a,b) > 0) {
        float height = float((p(a,b)/max)*nbins_);
        os << "Transform {\n";
        os << "  translation " << a << " " << b << " " << height << vcl_endl;
        os << "  children Shape {\n";
        os << "    geometry Sphere { radius 0.2 }\n";
        os << "    appearance DEF A1 Appearance {";
        os << "      material Material {\n";
        os << "        diffuseColor 1 0 0\n";
        os << "        emissiveColor .3 0 0\n";
        os << "      }\n";
        os << "    }\n";
        os << "  }\n";
        os << "}\n";
        os << "Transform { \n";
        os << "  translation " << a << " " << b << " " << height/2.0 << "\n";
        os << "  rotation 1 0 0 " << vnl_math::pi/2.0 << " \n";
        os << "  children Shape { \n";
        os << "    appearance USE A1 \n"; 
        os << "    geometry Cylinder { radius 0.05 height " << height << " }\n";
        os << "  }\n";
        os << "}\n";
      }
    }
  }
  
  os << "Transform {\n";
  os << "  translation " << (nbins_-1)/2.0f << " " << (nbins_-1)/2.0f << " 0\n";
  os << "  children Shape {\n";
  os << "    geometry Box { size " << nbins_-1 << " " << nbins_-1 << " 0.3 } \n";
  os << "    appearance Appearance { \n";
  os << "      material Material { diffuseColor 0.8 0.8 0.8 } \n";
  os << "    } \n";
  os << "  } \n";
  os << "}\n";
  
  os << "Background { skyColor 1 1 1 }\n";
  os << "NavigationInfo { type \"EXAMINE\" }\n";
  os << "] }\n";
}

template <class T>
void bsta_joint_histogram<T>::print_to_m(vcl_ostream& os) const
{
  os << "y = zeros(" << nbins_ << ", " << nbins_ << ");\n";
  for (unsigned int a = 0; a<nbins_; a++) {
    for (unsigned int b = 0; b<nbins_; b++) {
      if (p(a,b) > 0) {
        //os << "y(" << a+1 << ", " << b+1 << ") = " << p(a,b) << "; ";
        os << "y(" << a+1 << ", " << b+1 << ") = " << counts_[a][b] << "; ";
      }
    }
    //os << "\n";
  }
  //os << "\n";
  os << "bar3(y,'detached');\n";
}

#undef BSTA_JOINT_HISTOGRAM_INSTANTIATE
#define BSTA_JOINT_HISTOGRAM_INSTANTIATE(T) \
template class bsta_joint_histogram<T >

#endif // bsta_joint_histogram_txx_
