#ifndef bsta_histogram_hxx_
#define bsta_histogram_hxx_
//:
// \file
#include <iostream>
#include <cmath>
#include <limits>
#include "bsta_histogram.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include "bsta_gauss.h"
#include <vnl/vnl_math.h> // for log2e == 1/std::log(2.0)

template <class T>
bsta_histogram<T>::bsta_histogram()
  : area_valid_(false), area_(0), nbins_(0), range_(0),
    delta_(0),min_prob_(0), min_(0), max_(0)
{
  bsta_histogram_base::type_ = bsta_histogram_traits<T>::type();
}

template <class T>
bsta_histogram<T>::bsta_histogram(const T range, const unsigned int nbins,
                                  const T min_prob)
  : area_valid_(false), area_(0), nbins_(nbins), range_(range),
    delta_(0),min_prob_(min_prob), min_(0), max_(range)
{
  bsta_histogram_base::type_ = bsta_histogram_traits<T>::type();
  if (nbins>0)
  {
    delta_ = range_/nbins;
    counts_.resize(nbins, T(0));
  }
}

template <class T>
bsta_histogram<T>::bsta_histogram(const T min, const T max,
                                  const unsigned int nbins,
                                  const T min_prob)
  : area_valid_(false), area_(0), nbins_(nbins), delta_(0),
    min_prob_(min_prob), min_(min), max_(max)
{
  bsta_histogram_base::type_ = bsta_histogram_traits<T>::type();
  if (nbins>0)
  {
    range_ = max-min;
    delta_ = range_/nbins;
    counts_.resize(nbins, T(0));
  }
  else
  {
    range_ = 0;
    delta_ = 0;
  }
}

template <class T>
bsta_histogram<T>::bsta_histogram(const unsigned int nbins, const T min, const T delta,
                                  const T min_prob)
 : area_valid_(false), area_(0), nbins_(nbins), delta_(delta),
    min_prob_(min_prob), min_(min), max_(min+nbins*delta)
{
  bsta_histogram_base::type_ = bsta_histogram_traits<T>::type();
  if (nbins>0)
  {
    range_ = max_-min_;
    counts_.resize(nbins, T(0));
  }
  else
   range_ = 0;
}

template <class T>
bsta_histogram<T>::bsta_histogram(const T min, const T max,
                                  std::vector<T> const& data, const T min_prob)
  : area_valid_(false), area_(0), delta_(0), min_prob_(min_prob),
    min_(min), max_(max), counts_(data)
{
  bsta_histogram_base::type_ = bsta_histogram_traits<T>::type();
  nbins_ = data.size();
  range_ = max-min;
  if (nbins_>0)
    delta_ = range_/nbins_;
  else
    delta_ = 0;
}

template <class T>
void bsta_histogram<T>::upcount(T x, T mag)
{
  if (x<min_||x>max_)
    return;
  for (unsigned int i = 0; i<nbins_; ++i)
    if (T((i+1)*delta_) + min_ >= x)
    {
      counts_[i] += mag;
      break;
    }
  area_valid_ = false;
}

template <class T>

int bsta_histogram<T>::bin_at_val(T x) const
{
  if (x<min_||x>max_)
    return -1;
  for (unsigned int i = 0; i<nbins_; ++i)
    if (T((i+1)*delta_) + min_ >= x)
    {
      return i;
    }
  return -1;
}

template <class T>
void bsta_histogram<T>::compute_area() const
{
  area_ = 0;
  for (unsigned int i = 0; i < nbins_; ++i)
    area_ += counts_[i];
  area_valid_ = true;
}

template <class T>
T bsta_histogram<T>::cumulative_area(unsigned bin) const
{
  T area =0;
  for (unsigned int i = 0; i<bin; ++i)
    area += counts_[i];
  return area;
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
T bsta_histogram<T>::p(const T val) const
{
  if (val<min_||val>max_)
    return 0;
  if (!area_valid_)
    compute_area();
  if (area_ == T(0))
    return 0;
  else
    for (unsigned int i = 0; i<nbins_; ++i)
      if (T((i+1)*delta_) + min_ >= val)
        return counts_[i]/area_;
  return 0;
}

template <class T>
T bsta_histogram<T>::area() const
{
  if (!area_valid_)
    compute_area();
  return area_;
}

//: Mean of distribution
template <class T>
T bsta_histogram<T>::mean() const
{
  return mean(0, nbins_-1);
}

  //: Mean of distribution between bin indices
template <class T>
T bsta_histogram<T>::mean(const unsigned int lowbin, const unsigned int highbin) const
{
  assert(lowbin<=highbin);
  assert(highbin<nbins_);
  T sum = 0;
  T sumx = 0;
  for (unsigned i = lowbin; i<=highbin; ++i)
  {
    sum += counts_[i];
    sumx += (i*delta_ + min_)*counts_[i];
  }
  if (sum==0)
    return 0;
  T result = sumx/sum;
  return result;
}

template <class T>
T bsta_histogram<T>::mean_vals(const T low, const T high) const
{
  //find bin indices
  T tlow=low, thigh=high;
  if (tlow<min_) tlow = min_;
  if (thigh>max_) thigh = max_;
  unsigned low_bin=0, high_bin = 0;
  for (unsigned int i = 0; i<nbins_; ++i)
    if (T((i+1)*delta_) + min_ >= tlow) {
      low_bin = i;
      break;
    }
  for (unsigned int i = 0; i<nbins_; ++i)
    if (T((i+1)*delta_) + min_ >= thigh) {
      high_bin = i;
      break;
    }
  return this->mean(low_bin, high_bin);
}

  //: Variance of distribution
template <class T>
T bsta_histogram<T>::variance() const
{
  return variance(0, nbins_-1);
}

  //: Variance of distribution between bin indices
template <class T>
T bsta_histogram<T>::
variance(const unsigned int lowbin, const unsigned int highbin) const
{
  assert(lowbin<=highbin);
  assert(highbin<nbins_);
  T mean = this->mean(lowbin, highbin);
  mean -= min_;
  T sum = 0;
  T sumx2 = 0;
  for (unsigned i = lowbin; i<=highbin; ++i)
  {
    sum += counts_[i];
    sumx2 += (i*delta_-mean)*(i*delta_-mean)*counts_[i];
  }
  if (sum==0)
    return 0;
  else
    return sumx2/sum;
}

template <class T>
T bsta_histogram<T>::
variance_vals(const T low, const T high) const
{
  //find bin indices
  T tlow=low, thigh=high;
  if (tlow<min_) tlow = min_;
  if (thigh>max_) thigh = max_;
  unsigned low_bin=0, high_bin = 0;
  for (unsigned int i = 0; i<nbins_; ++i)
    if (T((i+1)*delta_) + min_ >= tlow) {
      low_bin = i;
      break;
    }
  for (unsigned int i = 0; i<nbins_; ++i)
    if (T((i+1)*delta_) + min_ >= thigh) {
      high_bin = i;
      break;
    }
  return this->variance(low_bin, high_bin);
}

template <class T>
T bsta_histogram<T>::entropy() const
{
  double ent = 0;
  for (unsigned int i = 0; i<nbins_; ++i)
  {
    double pi = this->p(i);
    if (pi>min_prob_)
      ent -= pi*std::log(pi);
  }
  ent *= vnl_math::log2e;
  return T(ent);
}

template <class T>
T bsta_histogram<T>::renyi_entropy() const
{
  double sum = 0, ent = 0;
  for (unsigned int i = 0; i<nbins_; ++i)
  {
    double pi = this->p(i);
    sum += pi*pi;
  }
  if (sum>min_prob_)
    ent = - std::log(sum)*vnl_math::log2e;
  return T(ent);
}

template <class T>
void bsta_histogram<T>::parzen(const T sigma)
{
  if (sigma<=0)
    return;
  double sd = (double)sigma;
  std::vector<double> in(nbins_), out(nbins_);
  for (unsigned int i=0; i<nbins_; ++i)
    in[i]=counts_[i];
  bsta_gauss::bsta_1d_gaussian(sd, in, out);
  for (unsigned int i=0; i<nbins_; ++i)
    counts_[i]=(T)out[i];
}

//The first non-zero bin starting at index = 0
template <class T>
unsigned bsta_histogram<T>::low_bin()
{
  unsigned lowbin=0;
  for (; lowbin<nbins_&&counts_[lowbin]==0; ++lowbin) /*nothing*/;
  return lowbin;
}

//The first non-zero bin starting at index = nbins-1
template <class T>
unsigned bsta_histogram<T>::high_bin()
{
  unsigned highbin=nbins_-1;
  for (; highbin>0&&counts_[highbin]==0; --highbin) /*nothing*/;
  return highbin;
}

// Fraction of area less than value
template <class T>
T bsta_histogram<T>::fraction_below(const T value) const
{
 if (value<min_||value>max_)
    return 0;
  if (!area_valid_)
    compute_area();
  if (area_ == T(0))
    return 0;
  T sum = 0, limit=(value-min_);
  for (unsigned int i = 0; i<nbins_; ++i)
    if (T((i+1)*delta_)<limit)
      sum+=counts_[i];
    else
      return sum/area_;
  return 0;
}

// Fraction of area greater than value
template <class T>
T bsta_histogram<T>::fraction_above(const T value) const
{
 if (value<min_||value>max_)
    return 0;
  if (!area_valid_)
    compute_area();
  if (area_ == T(0))
    return 0;
  T sum = 0, limit=(value-min_);
  for (unsigned int i = 0; i<nbins_; ++i)
    if (T((i+1)*delta_)>limit)
      sum+=counts_[i];
  return sum/area_;
}

// Value for area fraction below value
template <class T>
T bsta_histogram<T>::value_with_area_below(const T area_fraction) const
{
  if (area_fraction>T(1))
    return 0;
  if (!area_valid_)
    compute_area();
  if (area_ == T(0))
    return 0;
  T sum = 0;
  for (unsigned int i=0; i<nbins_; ++i)
  {
    sum += counts_[i];
    if (sum>=area_fraction*area_)
      return (i+1)*delta_+min_;
  }
  return 0;
}

// Value for area fraction above value
template <class T>
T  bsta_histogram<T>::value_with_area_above(const T area_fraction) const
{
  if (area_fraction>T(1))
    return 0;
  if (!area_valid_)
    compute_area();
  if (area_ == T(0))
    return 0;
  T sum = 0;
  for (unsigned int i=nbins_-1; i!=0; i--)
  {
    sum += counts_[i];
    if (sum>area_fraction*area_)
      return (i+1)*delta_+min_;
  }
  return 0;
}

//function to erase all bin counts
template <class T>
void bsta_histogram<T>::clear()
{
    area_valid_ = false;
    area_ = T(0);
    counts_.assign(nbins_,T(0));
}

template <class T>
void bsta_histogram<T>::print(std::ostream& os) const
{
  for (unsigned int i=0; i<nbins_; ++i)
    if (p(i) > 0)
      os << "p[" << i << "]=" << p(i) << '\n';
}

template <class T>
void bsta_histogram<T>::pretty_print(std::ostream& os) const
{
  os << "area valid: " << area_valid_ << '\n'
     << "area: " << area_ << '\n'
     << "number of bins: " <<  nbins_ << '\n'
     << "range: " << range_ << '\n'
     << "delta: " << delta_ << '\n'
     << "min_prob: " << min_prob_ << '\n'
     << "min: " << min_ << '\n'
     << "max: " << max_ << '\n'
     << "counts: ";
  for (unsigned i = 0; i < counts_.size() ; ++i)
    os << counts_[i] << ' ';
  os << '\n';
}

//: print as a matlab plot command
template <class T>
void bsta_histogram<T>::print_to_m(std::ostream& os) const
{
  os << "x = [" << min_;
  for (unsigned int i=1; i<nbins_; ++i)
    os << ", " << min_ + i*delta_;
  os << "];\n"
     << "y = [" << p((unsigned int)0);
  for (unsigned int i=1; i<nbins_; ++i)
    os << ", " << p(i);
  os << "];\n"
     << "bar(x,y,'r')\n";
}

//: print x and y arrays
template <class T>
void bsta_histogram<T>::print_to_arrays(std::ostream& os) const
{
  os << min_;
  for (unsigned int i=1; i<nbins_; ++i)
    os << ", " << min_ + i*delta_;
  os << '\n'
     << p((unsigned int)0);
  for (unsigned int i=1; i<nbins_; ++i)
    os << ", " << p(i);
  os << '\n';
}

template <class T>
void bsta_histogram<T>::print_vals_prob(std::ostream& os) const
{
  for (unsigned i = 0; i<nbins_; ++i)
    os << avg_bin_value(i) << ' ' << p(i) << '\n';
}

template <class T>
std::ostream& bsta_histogram<T>::write(std::ostream& s) const
{
  s << area_valid_ << ' '
    << area_ << ' '
    << nbins_ << ' '
    << range_ << ' '
    << delta_ << ' '
    << min_prob_ << ' '
    << min_ << ' '
    << max_ << ' ';
  for (unsigned i = 0; i < counts_.size() ; ++i)
    s << counts_[i] << ' ';

  return  s << '\n';
}

template <class T>
std::istream& bsta_histogram<T>::read(std::istream& s)
{
  s >> area_valid_
    >> area_
    >> nbins_
    >> range_
    >> delta_
    >> min_prob_
    >> min_
    >> max_;
  counts_.resize(nbins_);
  for (unsigned i = 0; i < counts_.size() ; ++i)
    s >> counts_[i] ;
  return  s;
}
template <class T>
T hist_intersect(bsta_histogram<T> const& ha, bsta_histogram<T> const& hb){
  unsigned na = ha.nbins(), nb = hb.nbins();
  if(na != nb){
    std::cout << "histograms do not have the same number of bins\n";
    return std::numeric_limits<T>::max();
  }
  T sum = T(0);
  for(unsigned i = 0; i<na; ++i){
    T pa = ha.p(i), pb = hb.p(i);
    if(pa<pb)
      sum += pa;
    else
      sum+= pb;
  }
  return sum;
}
template <class T>
T js_divergence(bsta_histogram<T> const& ha, bsta_histogram<T> const& hb){
  unsigned na = ha.nbins(), nb = hb.nbins();
  if(na != nb){
    std::cout << "histograms do not have the same number of bins\n";
    return std::numeric_limits<T>::max();
  }
  T sum = T(0);
  for(unsigned i = 0; i<na; ++i){
    T pa = ha.p(i), pb = hb.p(i);
    T pavg = (pa + pb)/T(2);
    T jsa = T(0), jsb = T(0);
    if(pa != T(0))
      jsa = pa*std::log(pa/pavg);
    if(pb != T(0))
      jsb = pb*std::log(pb/pavg);
    sum += jsa + jsb;
  }
  return sum/T(2);
}
template <class T>
T bhatt_distance(bsta_histogram<T> const& ha, bsta_histogram<T> const& hb) {
  unsigned na = ha.nbins(), nb = hb.nbins();
  if (na != nb) {
    std::cout << "histograms do not have the same number of bins\n";
    return std::numeric_limits<T>::max();
  }
  T sum = T(0);
  for (unsigned i = 0; i<na; ++i) {
    T pa = ha.p(i), pb = hb.p(i);
    if (pa<T(0) || pb<T(0)) {
      std::cout << "Fatal! - negative probablity\n";
      return std::numeric_limits<T>::max();
    }
    sum += sqrt(pa*pb);
  }
  if (sum<T(0)) {
    std::cout << "Fatal! - negative sum\n";
    return std::numeric_limits<T>::max();
  }
  return -log(sum);

}
template <class T>
bsta_histogram<T> scale(bsta_histogram<T> const& h, T s){
  T min = h.min(), max = h.max(), delta = h.delta();
  unsigned nbins = h.nbins();
  // create return hist with same number of bins and range
  bsta_histogram<T> hret(min, max, nbins);
  if(s>T(1)) s = T(1);
  for(T x = max-(delta/2.0); x>=(min+(delta/2.0)); x -= delta)
    {
      unsigned x_bin = static_cast<unsigned>(h.bin_at_val(x));
        T trans_x = (x-min)*s + min;
      unsigned trans_bin = static_cast<unsigned>(hret.bin_at_val(trans_x));
      T tbin_val = hret.avg_bin_value(trans_bin);
      //amount to distribute to adjacent bins
      T offset = T(2)*(trans_x - tbin_val)/delta;
      T x_counts = h.counts(x_bin);
      // cases
      if( offset <= T(0)){
        if(offset<-T(1)) offset = -T(1);
        if(trans_bin == 0){// 100% of the counts go in trans_bin
          T ret_counts = hret.counts(trans_bin);
          hret.set_count(trans_bin, ret_counts+x_counts);
        }else{// distribute to bin below
          T ret_counts = hret.counts(trans_bin);
          T ret_counts_minus = hret.counts(trans_bin-1);
          T counts_for_bin = (T(1)+offset)*x_counts;
          T counts_for_minus_bin = -offset*x_counts;
          hret.set_count(trans_bin, counts_for_bin+ret_counts);
          hret.set_count(trans_bin-1, counts_for_minus_bin+ret_counts_minus);
        }
      }else{ // offset > 0
        if(offset>T(1)) offset = T(1);
        if(trans_bin == nbins-1){ // 100% of counts go in trans_bin
          T ret_counts = hret.counts(trans_bin);
          hret.set_count(trans_bin, ret_counts+x_counts);
        }else{ // distribute to bin above
          T ret_counts = hret.counts(trans_bin);
          T ret_counts_plus = hret.counts(trans_bin+1);
          T counts_for_bin = (T(1)-offset)*x_counts;
          T counts_for_plus_bin = offset*x_counts;
          hret.set_count(trans_bin, counts_for_bin+ret_counts);
          hret.set_count(trans_bin+1, counts_for_plus_bin+ret_counts_plus);
        }
      }
    }
  return hret;
}
// find scale to minimize js_dirvergence
template <class T>
T minimum_js_divergence_scale(bsta_histogram<T> const& h_from, bsta_histogram<T> const& h_to,
                              T min_scale){
  T min_js_d = std::numeric_limits<T>::max(), scale_min = T(1);
  T ds = min_scale/T(10);
  for(T s = (T(1)-ds); s>=min_scale; s-=ds){
    bsta_histogram<T> hs = scale(h_from, s);
    T jsd = js_divergence(h_to, hs);
    if(jsd<min_js_d){
      scale_min = s;
      min_js_d = jsd;
    }
  }
  return scale_min;
}

template <class T>
bool merge_hists(bsta_histogram<T> const& ha, bsta_histogram<T> const& hb, bsta_histogram<T>& h_merged){
  // ha and hb must have the same number of bins and min max values
  if(ha.nbins() != hb.nbins()){
    std::cout << "Can't merge histogams with different numbers of bins" << std::endl;
    return false;
  }
  if(ha.min() != hb.min() || ha.max() != hb.max()){
    std::cout << "Can't merge histogams with different value ranges" << std::endl;
    return false;
  }
  bsta_histogram<T> temp(ha.min(), ha.max(), ha.nbins());
  for(unsigned i = 0; i<ha.nbins(); ++i){
    T counta = ha.counts(i), countb = hb.counts(i);
    temp.set_count(i, counta + countb);
  }
  h_merged = temp;
  return true;
}
  //: Write to stream
template <class T>
std::ostream& operator<<(std::ostream& s, bsta_histogram<T> const& h)
{
  return h.write(s);
}

//: Read from stream
template <class T>
std::istream& operator>>(std::istream& is, bsta_histogram<T>& h)
{
  return h.read(is);
}


#undef BSTA_HISTOGRAM_INSTANTIATE
#define BSTA_HISTOGRAM_INSTANTIATE(T) \
template class bsta_histogram<T >; \
template T js_divergence(bsta_histogram<T> const& , bsta_histogram<T> const&); \
template T bhatt_distance(bsta_histogram<T> const& , bsta_histogram<T> const&); \
template bsta_histogram<T> scale(bsta_histogram<T> const&, T ); \
template T minimum_js_divergence_scale(bsta_histogram<T> const&, bsta_histogram<T> const&, T); \
template T hist_intersect(bsta_histogram<T> const& ha, bsta_histogram<T> const& hb); \
template bool merge_hists(bsta_histogram<T> const& ha, bsta_histogram<T> const& hb, bsta_histogram<T>& h_merged); \
template std::istream& operator>>(std::istream&, bsta_histogram<T >&);  \
template std::ostream& operator<<(std::ostream&, bsta_histogram<T > const&)

#endif // bsta_histogram_hxx_
