#ifndef mbl_stochastic_data_collector_txx_
#define mbl_stochastic_data_collector_txx_

//:
//  \file

#include <vcl_string.h>
#include <vsl/vsl_indent.h>
#include <mbl/mbl_stochastic_data_collector.h>
#include <vnl/vnl_math.h>

//=======================================================================

template <class T>
mbl_stochastic_data_collector<T>::
mbl_stochastic_data_collector()
: samples_(0), nPresented_(0), rand(9667566)
{
}

//=======================================================================

template <class T>
mbl_stochastic_data_collector<T>::
mbl_stochastic_data_collector(unsigned n):
samples_(n), nPresented_(0), rand(9667566)
{
}

//=======================================================================

template <class T>
mbl_stochastic_data_collector<T>::
~mbl_stochastic_data_collector()
{
}
//=======================================================================

//: Clear any stored data
template <class T>
void mbl_stochastic_data_collector<T>::clear()
{
  nPresented_ = 0;
}

//=======================================================================

//: Set number of samples to be stored
// If not set, the value defaults to 1000.
// Calling this function implicity calls clean().
template <class T>
void mbl_stochastic_data_collector<T>::set_n_samples(int n)
{
  nPresented_ = 0;
  samples_.resize(n);
}

//=======================================================================

//: Stochastically record given vector.
// The vector will be recorded, and saved with a probabililty equal to that of
// every other vector presented to this function.
// returns true if it actually stored the value.
template <class T>
void mbl_stochastic_data_collector<T>::record(const T& v)
{
  if (nPresented_ < samples_.size())
  {
    samples_[nPresented_] = v;
    nPresented_ ++;
  }
  else
  {
    double nSamples = samples_.size();
    // recalculate probability from scratch each time to avoid accumulation of rounding errors.
    double prob = nSamples * nSamples / (nPresented_ * (nSamples+1));
    nPresented_ ++;
    if (rand.drand64() < prob)
      samples_[rand.lrand32(samples_.size() - 1)] = v;
  }
}

//=======================================================================

//: Force recording of this given value
// This does not increment n_presented()
// Used with next(), to avoid calculating values that will not be stored.
template <class T>
void mbl_stochastic_data_collector<T>::force_record(const T& v)
{
  if (nPresented_ < samples_.size())
    samples_[nPresented_-1] = v;
  else
    samples_[rand.lrand32(samples_.size() - 1)] = v;
}

//=======================================================================

//: Will decide whether to store the next value
// This will increment n_presented()
// \return true if you should call record_definite() with the next value.
template <class T>
bool mbl_stochastic_data_collector<T>::store_next()
{
  if (nPresented_ < samples_.size())
  {
    nPresented_ ++;
    return true;
  }
  else
  {
    double nSamples = samples_.size();
    // recalculate probability from scratch each time
    //   to avoid accumulation of rounding errors.
    double prob = nSamples * nSamples / (nPresented_ * (nSamples+1));
    nPresented_ ++;
    if (rand.drand64() < prob)
    {
      return true;
    }
    else return false;
  }
}

//=======================================================================

//: Return object describing stored data
template <class T>
mbl_data_wrapper<T>& mbl_stochastic_data_collector<T>::data_wrapper()
{
  v_data_.set(&samples_[0], vnl_math_min((unsigned long) samples_.size(), nPresented_));
  return v_data_;
}

//=======================================================================

//: Reseed the internal random number generator.
template <class T>
void mbl_stochastic_data_collector<T>::reseed (unsigned long seed)
{
  rand.reseed(seed);
}

//=======================================================================

template <class T>
vcl_string mbl_stochastic_data_collector<T>::is_a() const
{
  return vcl_string("mbl_stochastic_data_collector<T>");
}

//=======================================================================

template <class T>
bool mbl_stochastic_data_collector<T>::is_class(vcl_string const& s) const
{
  return s==mbl_stochastic_data_collector<T>::is_a() || mbl_data_collector<T>::is_class(s);
}

//=======================================================================

template <class T>
short mbl_stochastic_data_collector<T>::version_no() const
{
  return 1;
}

//=======================================================================

template <class T>
mbl_data_collector_base* mbl_stochastic_data_collector<T>::clone() const
{
  return new mbl_stochastic_data_collector<T>(*this);
}

//=======================================================================

template <class T>
void mbl_stochastic_data_collector<T>::print_summary(vcl_ostream& os) const
{
  os << "number stored: " << samples_.size() << " number presented: " <<
    nPresented_ << vcl_endl;
}

//=======================================================================

template <class T>
void mbl_stochastic_data_collector<T>::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs, version_no());

  vsl_b_write(bfs, samples_);
  vsl_b_write(bfs, nPresented_);
}

//=======================================================================

template <class T>
void mbl_stochastic_data_collector<T>::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
  case (1):
    vsl_b_read(bfs, samples_);
    vsl_b_read(bfs, nPresented_);
    break;
  default:
    vcl_cerr << "I/O ERROR: mbl_stochastic_data_collector<T::b_read(vsl_b_istream&) \n";
    vcl_cerr << "           Unknown version number "<< version << "\n";
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

#define MBL_STOCHASTIC_DATA_COLLECTOR_INSTANTIATE(T) \
template class mbl_stochastic_data_collector< T >

#endif // mbl_data_collector_txx_
