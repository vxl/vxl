#ifndef bvpl_kernel_iterator_h_
#define bvpl_kernel_iterator_h_
//:
// \file
// \brief A class for iterating over the kernel values, paired with the position and value
//
// \author Gamze Tunali
// \date May 28, 2009
// \verbatim
//  Modifications
//   <None yet>
// \endverbatim

#include <utility>
#include <vector>
#include <iostream>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vgl/vgl_point_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class bvpl_kernel_dispatch
{
 public:
  bvpl_kernel_dispatch(float c) { c_=c; }
  ~bvpl_kernel_dispatch() = default;
  float c_;
};

class bvpl_kernel_iterator: public vbl_ref_count
{
 public:
  typedef std::pair<vgl_point_3d<int>, bvpl_kernel_dispatch> kernel_pair;

  bvpl_kernel_iterator() : cur_index_(0) {}

  bvpl_kernel_iterator(std::vector<kernel_pair> kernel_vals):kernel_vals_(std::move(kernel_vals)), cur_index_(0) {}

  ~bvpl_kernel_iterator() override= default;

  void insert(vgl_point_3d<int> index, bvpl_kernel_dispatch val) {kernel_vals_.emplace_back(index,val); }

  vgl_point_3d<int> index() const { return kernel_vals_[cur_index_].first; }

  bvpl_kernel_iterator& begin() { cur_index_ = 0; return *this;}

  bvpl_kernel_iterator& operator++() { if (cur_index_ != kernel_vals_.size()) ++cur_index_; return *this;}

  bvpl_kernel_iterator& operator--() { if (cur_index_ != 0) --cur_index_; return *this;}

  bvpl_kernel_dispatch operator*() const { return kernel_vals_[cur_index_].second; }

  bool isDone() { if (cur_index_ == kernel_vals_.size()) return true; return false; }

 private:
  std::vector<std::pair<vgl_point_3d<int>, bvpl_kernel_dispatch> > kernel_vals_;
  unsigned cur_index_;
};

typedef vbl_smart_ptr<bvpl_kernel_iterator> bvpl_kernel_iterator_sptr;

#endif
