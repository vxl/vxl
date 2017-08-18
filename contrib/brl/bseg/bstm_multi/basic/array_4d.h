#ifndef bstm_multi_basic_array_4d_h_
#define bstm_multi_basic_array_4d_h_

#include <vcl_cstddef.h>
#include <vcl_cstdlib.h>

#include <vgl/vgl_vector_3d.h>
#include <vnl/vnl_vector_fixed.h>

typedef vnl_vector_fixed<vcl_size_t, 4> index_4d;

// \brief a non-owning wrapper around a buffer that provides access using 4D
// coordinates. Intended for representing space-time blocks. Indexing is done in
// row-major order.
template <class T> class array_4d {
public:
  typedef T *iterator;
  typedef const T *const_iterator;

  array_4d() : buffer_(VXL_NULLPTR), x_(0), y_(0), z_(0), t_(0) {}
  array_4d(T *buffer, vcl_size_t x, vcl_size_t y, vcl_size_t z, vcl_size_t t)
      : buffer_(buffer), x_(x), y_(y), z_(z), t_(t) {}
  // no delegating constructors before C++11 :-(
  array_4d(T *buffer, const index_4d &dims)
      : buffer_(buffer), x_(dims[0]), y_(dims[1]), z_(dims[2]), t_(dims[3]) {}
  array_4d(T *buffer,
           const vcl_pair<vgl_vector_3d<unsigned>, unsigned> &num_regions)
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
  vcl_size_t size() const { return x_ * y_ * z_ * t_; }
  vcl_size_t size(vcl_size_t dim) const { return size_[dim]; }
  vcl_size_t x() const { return x_; }
  vcl_size_t y() const { return y_; }
  vcl_size_t z() const { return z_; }
  vcl_size_t t() const { return t_; }

  /* Iterators */
  iterator begin() { return buffer_; }
  iterator end() { return begin() + size(); }
  const_iterator cbegin() { return buffer_; }
  const_iterator cend() { return end(); }

  /* Accessors */
  T &operator[](vcl_size_t idx) { return buffer_[idx]; }
  const T &operator[](vcl_size_t idx) const { return buffer_[idx]; }

  T &operator()(const index_4d &coords) {
    return buffer_[index_from_coords(coords)];
  }
  const T &operator()(const index_4d &coords) const {
    return buffer_[index_from_coords(coords)];
  }

  T &operator()(vcl_size_t i, vcl_size_t j, vcl_size_t k, vcl_size_t ti) {
    return buffer_[index_from_coords(i, j, k, ti)];
  }
  const T &
  operator()(vcl_size_t i, vcl_size_t j, vcl_size_t k, vcl_size_t ti) const {
    return buffer_[index_from_coords(i, j, k, ti)];
  }

  /* Util */
  vcl_size_t
  index_from_coords(vcl_size_t i, vcl_size_t j, vcl_size_t k, vcl_size_t ti) {
    return ((i * size_[1] + j) * size_[2] + k) * size_[3] + ti;
  }
  vcl_size_t index_from_coords(const index_4d &coords) {
    return index_from_coords(coords[0], coords[1], coords[2], coords[3]);
  }

  index_4d coords_from_index(vcl_size_t idx) {
    index_4d coords;
    ldiv_t div = vcl_div(static_cast<long>(idx), (y_ * z_ * t_));
    coords[0] = div.quot;
    div = vcl_div(div.rem, (z_ * t_));
    coords[1] = div.quot;
    div = vcl_div(div.rem, t_);
    coords[2] = div.quot;
    coords[3] = div.rem;
    return coords;
  }

private:
  T *buffer_;

  // can access size either by numerical index, or by name.
  union {
    vcl_size_t size_[4];
    struct {
      vcl_size_t x_, y_, z_, t_;
    };
  };
};

#endif // bstm_multi_basic_array_4d_h_
