#ifndef bsta_joint_histogram_txx_
#define bsta_joint_histogram_txx_

#include <vcl_cmath.h> // for log()
#include <vcl_iostream.h>
#include <bsta/bsta_gauss.h>
#include <bsta/bsta_joint_histogram.h>

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
    if ((i+1)*delta_>a)
    {
      bin_a = i;
      break;
    }
  for (unsigned int i = 0; i<nbins_; i++)
    if ((i+1)*delta_>b)
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
        ent -= pij*vcl_log(pij);
    }
  ent/= vcl_log(2.0);
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
    ent = -vcl_log(sum)/vcl_log(2.0);
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
void bsta_joint_histogram<T>::print() const
{
  for (unsigned int a = 0; a<nbins_; a++)
    for (unsigned int b = 0; b<nbins_; b++)
      if (p(a,b) > 0)
        vcl_cout << "p[" << a << "][" << b << "]=" << p(a,b) << '\n';
}

#undef BSTA_JOINT_HISTOGRAM_INSTANTIATE
#define BSTA_JOINT_HISTOGRAM_INSTANTIATE(T) \
template class bsta_joint_histogram<T >

#endif // bsta_joint_histogram_txx_
