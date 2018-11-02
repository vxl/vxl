#ifndef boxm_edge_tangent_sample_h_
#define boxm_edge_tangent_sample_h_
//:
// \file

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>
#include "boxm_plane_obs.h"
#include <boxm/boxm_aux_traits.h>


template <class T>
class boxm_edge_tangent_sample
{
 public:
  boxm_edge_tangent_sample() = default;
  ~boxm_edge_tangent_sample() = default;
  static short version_no() { return 1; }
  void print(std::ostream& os) const;

  //: adds observations
  void insert(boxm_plane_obs<T> const& obs) { obs_.push_back(obs); }

  //: insert a lis of observations
  void insert(std::vector<boxm_plane_obs<T> > const& obs) {obs_.insert(obs_.end(),obs.begin(),obs.end()); }

  //: returns the number of observations
  unsigned num_obs() const { return obs_.size(); }

  boxm_plane_obs<T> obs(unsigned int i) const {
    if (i < obs_.size()) return obs_[i];
    std::cerr << "boxm_edge_tangent_sample: " << i << " is out of index - size is " << obs_.size() << std::endl;
    return boxm_plane_obs<T>();
  }

  //: returns a basic value that represents this sample (mostly for raw file creation purposes)
  float basic_val(unsigned int i) {
    if (i < obs_.size()) return obs_[i].seg_len_;
    std::cerr << "boxm_edge_tangent_sample: " << i << "is out of index - size is " << obs_.size() << std::endl;
    return -1.0f;
  }

  std::vector<boxm_plane_obs<T> > obs_list() { return obs_; }

 private:
  std::vector<boxm_plane_obs<T> > obs_;
};

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_edge_tangent_sample<T> const &sample);

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_edge_tangent_sample<T> const * &sample);

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_edge_tangent_sample<T> &sample);

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_edge_tangent_sample<T> *&sample);

template <class T>
std::ostream&operator << (std::ostream& os, boxm_edge_tangent_sample<T>& sample);


#endif // boxm_edge_tangent_sample_h_
