#ifndef bstm_multi_basic_array_4d_h_
#define bstm_multi_basic_array_4d_h_

#include <vcl_cstddef.h>
#include <vcl_cstdlib.h>

struct vec4 {
  vec4() : i(0), j(0), k(0), t(0) {}
  vec4(vcl_size_t x, vcl_size_t y, vcl_size_t z, vcl_size_t t1)
      : i(x), j(y), k(z), t(t1) {}
  union {
    struct {
      vcl_size_t i, j, k, t;
    };
    vcl_size_t size[4];
  };
};

// implements lexicographical comparison

int cmp(vcl_size_t a, vcl_size_t b) { return (b < a) - (a < b); }

int cmp(const vec4 &v1, const vec4 &v2) {
  int c;
  for (int i = 0; i < 3; ++i) {
    if ((c = cmp(v1.size[i], v2.size[i])) != 0) {
      return c;
    }
  }
  return cmp(v1.size[3], v2.size[3]);
}

bool operator==(const vec4 &v1, const vec4 &v2) { return cmp(v1, v2) == 0; }
bool operator!=(const vec4 &v1, const vec4 &v2) { return cmp(v1, v2) != 0; }
bool operator<(const vec4 &v1, const vec4 &v2) { return cmp(v1, v2) < 0; }
bool operator>(const vec4 &v1, const vec4 &v2) { return cmp(v1, v2) > 0; }
bool operator<=(const vec4 &v1, const vec4 &v2) { return cmp(v1, v2) <= 0; }
bool operator>=(const vec4 &v1, const vec4 &v2) { return cmp(v1, v2) >= 0; }

// \brief a non-owning wrapper around a buffer that provides access using 4D
// coordinates. Intended for representing space-time blocks.
template <class T> class array_4d {
public:
  typedef T *iterator;
  typedef const T *const_iterator;

  array_4d() : buffer_(VXL_NULLPTR), x_(0), y_(0), z_(0), t_(0) {}
  array_4d(T *buffer, vcl_size_t x, vcl_size_t y, vcl_size_t z, vcl_size_t t)
      : buffer_(buffer), x_(x), y_(y), z_(z), t_(t) {}
  // no delegating constructors before C++11 :-(
  array_4d(T *buffer, const vec4 &dims)
      : buffer_(buffer), x_(dims.i), y_(dims.j), z_(dims.k), t_(dims.t) {}

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

  T &operator()(vec4 coords) {
    return buffer_[index_from_coords(coords.i, coords.j, coords.k, coords.t)];
  }
  const T &operator()(vec4 coords) const {
    return buffer_[index_from_coords(coords.i, coords.j, coords.k, coords.t)];
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

  vec4 coords_from_index(vcl_size_t idx) {
    vec4 coords;
    ldiv_t div = vcl_div(static_cast<long>(idx), (y_ * z_ * t_));
    coords.i = div.quot;
    div = vcl_div(div.rem, (z_ * t_));
    coords.j = div.quot;
    div = vcl_div(div.rem, t_);
    coords.k = div.quot;
    coords.t = div.rem;
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
