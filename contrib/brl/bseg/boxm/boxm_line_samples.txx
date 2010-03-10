#ifndef boxm_line_samples_txx_
#define boxm_line_samples_txx_

#include <boxm/boxm_line_samples.h>

template <class T>
void boxm_line_samples<T>::insert(int num_obs, T x, T y, T theta)
{
  int index;
  
  // insert the item into the smallest index
  if (smallest_index_ == -1)  // the vector is empty
    index = 0;
  else 
    index =  smallest_index_;
     
  num_obs_[index] = num_obs;
  x_[index] = x;
  y_[index] = y;
  theta_[index] = theta;
  
  index=0;
  int smallest_obs=1000000;
  for (unsigned i=0; i<num_obs_.size(); i++) {
    if (num_obs_[i] < smallest_obs) {
      smallest_obs = num_obs_[i];
      index = i;
    }
  }
  smallest_index_=index; 
}


template <class T>
bool boxm_line_samples<T>::operator==(const boxm_line_samples<T> &rhs) const
{ 
  if (this==&rhs)
    return true;
  else {
    for (unsigned i=0; i<num_obs_.size(); i++) {
      if ((num_obs_[i] != rhs.num_obs_[i]) || (x_[i] != rhs.x_[i]) || (y_[i] != rhs.y_[i]) || (theta_[i] != rhs.theta_[i]))
        return false;
    }
    return true;
  }
  return false;
}

template <class T>
void boxm_line_samples<T>::print(vcl_ostream& os) 
{ 
  os << "(Number of Lines=" << num_obs_.size() << ')' << vcl_endl;  
  for (unsigned i=0; i<num_obs_.size(); i++) {
    os << i << " number of observations=" << num_obs_[i] << " (" << x_[i] << "," << y_[i] << "," << theta_[i] << vcl_endl;
  }
}

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_line_samples<T> const &sample)
{
  vsl_b_write(os, sample.version_no());
  vsl_b_write(os, sample.smallest_index_);
  vsl_b_write(os, sample.num_obs_.size());
  for (unsigned i=0; i<sample.num_obs_.size(); i++) {
    vsl_b_write(os, sample.num_obs_[i]);
    vsl_b_write(os, sample.x_[i]);
    vsl_b_write(os, sample.y_[i]);
    vsl_b_write(os, sample.theta_[i]);
  }
}

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_line_samples<T> const * &sample)
{
  if (sample) {
    vsl_b_write(os, *sample);
  }
}

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_line_samples<T> &sample)
{
  if (!is) return;

  short version;
  vsl_b_read(is,version);
  switch (version)
  {
    case 1:
      unsigned size;
      vsl_b_read(is, sample.smallest_index_);
      vsl_b_read(is, size);
      sample.num_obs_.resize(size);
      sample.x_.resize(size);
      sample.y_.resize(size);
      sample.theta_.resize(size);
      for (unsigned i=0; i<size; i++) {
        vsl_b_read(is, sample.num_obs_[i]);
        vsl_b_read(is, sample.x_[i]);
        vsl_b_read(is, sample.y_[i]);
        vsl_b_read(is, sample.theta_[i]);
      }
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, boxm_line_samples<T>&)\n"
               << "           Unknown version number "<< version << '\n';
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      break;
  }
}

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_line_samples<T> *&sample)
{
  vsl_b_read(is, *sample);
}

template <class T>
vcl_ostream& operator << (vcl_ostream& os, boxm_line_samples<T>& sample)
{
  sample.print(os);
  return os;
}

#define BOXM_LINE_SAMPLES_INSTANTIATE(T) \
template class boxm_line_samples<T >; \
template void vsl_b_write(vsl_b_ostream &, boxm_line_samples<T > const &); \
template void vsl_b_write(vsl_b_ostream &, boxm_line_samples<T > const *&); \
template void vsl_b_read(vsl_b_istream &, boxm_line_samples<T > &); \
template void vsl_b_read(vsl_b_istream &, boxm_line_samples<T > *&);\
template vcl_ostream& operator << (vcl_ostream&, boxm_line_samples<T >&)

#endif
