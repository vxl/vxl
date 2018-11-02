#ifndef bstm_multi_basic_array_4d_h_
#define bstm_multi_basic_array_4d_h_

#include <iostream>
#include <cstddef>
#include <cstdlib>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgl/vgl_vector_3d.h>
#include <vnl/vnl_vector_fixed.hxx>

typedef vnl_vector_fixed<std::size_t, 4> index_4d;

// \brief a non-owning wrapper around a buffer that provides access using 4D
// coordinates. Intended for representing space-time blocks. Indexing is done in
// row-major order.
template <class T> class array_4d {
public:
  typedef T *iterator;
  typedef const T *const_iterator;

  array_4d() : buffer_(nullptr), x_(0), y_(0), z_(0), t_(0) {}
  array_4d(T *buffer, std::size_t x, std::size_t y, std::size_t z, std::size_t t)
      : buffer_(buffer), x_(x), y_(y), z_(z), t_(t) {}
  // no delegating constructors before C++11 :-(
  array_4d(T *buffer, const index_4d &dims)
      : buffer_(buffer), x_(dims[0]), y_(dims[1]), z_(dims[2]), t_(dims[3]) {}
  array_4d(T *buffer,
           const std::pair<vgl_vector_3d<unsigned>, unsigned> &num_regions)
      : buffer_(buffer)
      , x_(num_regions.first.x())
      , y_(num_regions.first.y())
      , z_(num_regions.first.z())
      , t_(num_regions.second) {}

  array_4d<T> &operator=(const array_4d<T> &that) {
    buffer_ = that.buffer_;
    x_ = that.x_;
    y_ = that.y_;
    z_ = that.z_;
    t_ = that.t_;
    return *this;
  }

  /*  Observers */
  std::size_t size() const { return x_ * y_ * z_ * t_; }
  std::size_t size(std::size_t dim) const { return size_[dim]; }
  std::size_t x() const { return x_; }
  std::size_t y() const { return y_; }
  std::size_t z() const { return z_; }
  std::size_t t() const { return t_; }

  /* Iterators */
  iterator begin() { return buffer_; }
  iterator end() { return begin() + size(); }
  const_iterator cbegin() const { return buffer_; }
  const_iterator cend() const { return end(); }

  /* Accessors */
  T &operator[](std::size_t idx) { return buffer_[idx]; }
  const T &operator[](std::size_t idx) const { return buffer_[idx]; }

  T &operator()(const index_4d &coords) {
    return buffer_[index_from_coords(coords)];
  }
  const T &operator()(const index_4d &coords) const {
    return buffer_[index_from_coords(coords)];
  }

  T &operator()(std::size_t i, std::size_t j, std::size_t k, std::size_t ti) {
    return buffer_[index_from_coords(i, j, k, ti)];
  }
  const T &
  operator()(std::size_t i, std::size_t j, std::size_t k, std::size_t ti) const {
    return buffer_[index_from_coords(i, j, k, ti)];
  }

  /* Util */
  std::size_t
  index_from_coords(std::size_t i, std::size_t j, std::size_t k, std::size_t ti) const {
    return ((i * size_[1] + j) * size_[2] + k) * size_[3] + ti;
  }
  std::size_t index_from_coords(const index_4d &coords)  const {
    return index_from_coords(coords[0], coords[1], coords[2], coords[3]);
  }

  index_4d coords_from_index(std::size_t idx) const {
    index_4d coords;
    ldiv_t div = std::ldiv((idx), (y_ * z_ * t_));
    coords[0] = div.quot;
    div = std::div(div.rem, (z_ * t_));
    coords[1] = div.quot;
    div = std::div(div.rem, t_);
    coords[2] = div.quot;
    coords[3] = div.rem;
    return coords;
  }

private:
  T *buffer_;

  // can access size either by numerical index, or by name.
  union {
    std::size_t size_[4];
    struct {
      std::size_t x_, y_, z_, t_;
    };
  };
};

#endif // bstm_multi_basic_array_4d_h_
