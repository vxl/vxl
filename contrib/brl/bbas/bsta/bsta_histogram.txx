#ifndef bsta_histogram_txx_
#define bsta_histogram_txx_

#include <vcl_cmath.h> // for log()
#include <vcl_iostream.h>
#include <bsta/bsta_gauss.h>
#include <bsta/bsta_histogram.h>

#define LOG2 1.4426950408889634074 // == 1/vcl_log(2.0)

template <class T>
bsta_histogram<T>::bsta_histogram(const T range, const unsigned int nbins,
                                  const T min_prob)
  : area_valid_(false), area_(0), nbins_(nbins), range_(range), delta_(0)
{
  if (nbins>0)
  {
    delta_ = range_/nbins;
    counts_.resize(nbins, T(0));
  }
  min_prob_ = min_prob;
}

template <class T>
void bsta_histogram<T>::upcount(T x, T mag)
{
  if (x<0||x>range_)
    return;
  for (unsigned int i = 0; i<nbins_; i++)
    if ((i+1)*delta_>x)
    {
      counts_[i] += mag;
      break;
    }
  area_valid_ = false;
}

template <class T>
void bsta_histogram<T>::compute_area() const
{
  area_ =0;
  for (unsigned int i = 0; i<nbins_; i++)
    area_ += counts_[i];
  area_valid_ = true;
}

template <class T>
T bsta_histogram<T>::p(unsigned int bin) const
{
  if (bin>=nbins_)
    return 0;
  if (!area_valid_)
    compute_area();
  if (area_ == T(0))
    return 0;
  else
    return counts_[bin]/area_;
}

template <class T>
T bsta_histogram<T>::area() const
{
  if (!area_valid_)
    compute_area();
  return area_;
}

template <class T>
T bsta_histogram<T>::entropy() const
{
  T ent = 0;
  for (unsigned int i = 0; i<nbins_; ++i)
  {
    T pi = this->p(i);
    if (pi>min_prob_)
      ent -= pi*T(vcl_log(pi));
  }
  ent *= (T)LOG2;
  return ent;
}

template <class T>
T bsta_histogram<T>::renyi_entropy() const
{
  T sum = 0, ent = 0;
  for (unsigned int i = 0; i<nbins_; ++i)
  {
    T pi = this->p(i);
    sum += pi*pi;
  }
  if (sum>min_prob_)
    ent = - T(vcl_log(sum))*(T)LOG2;
  return ent;
}

template <class T>
void bsta_histogram<T>::parzen(const T sigma)
{
  if (sigma<=0)
    return;
  double sd = (double)sigma;
  vcl_vector<double> in(nbins_), out(nbins_);
  for (unsigned int i=0; i<nbins_; i++)
    in[i]=counts_[i];
  bsta_gauss::bsta_1d_gaussian(sd, in, out);
  for (unsigned int i=0; i<nbins_; i++)
    counts_[i]=(T)out[i];
}

template <class T>
void bsta_histogram<T>::print() const
{
  for (unsigned int i=0; i<nbins_; i++)
    if (p(i) > 0)
      vcl_cout << "p[" << i << "]=" << p(i) << '\n';
}

#undef BSTA_HISTOGRAM_INSTANTIATE
#define BSTA_HISTOGRAM_INSTANTIATE(T) \
template class bsta_histogram<T >

#endif // bsta_histogram_txx_
