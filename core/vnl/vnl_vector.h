#ifndef vnl_vector_h_
#define vnl_vector_h_
#ifdef __GNUC__
#pragma interface
#endif

// .NAME vnl_vector
// .INCLUDE vnl/vnl_vector.h
// .FILE vnl_vector.txx
//
// .SECTION Description
// The vnl_vector<T> class implements one-dimensional arithmetic
// vectors to be used with the vnl_matrix<T> class. vnl_vector<T>
// has size fixed by constructor time or changed by assignment
// operator.
// For faster, non-mallocing vectors with size known at compile
// time, use vnl_vector_fixed* or vnl_T_n (e.g. vnl_double_3).

#include <vcl/vcl_iosfwd.h>
#include <vnl/vnl_error.h>
#include <vnl/vnl_c_vector.h>

template <class T> class vnl_vector;
template <class T> class vnl_matrix;

//--------------------------------------------------------------------------------

template <class T> T             dot_product (vnl_vector<T> const&, vnl_vector<T> const&);
template <class T> T             inner_product (vnl_vector<T> const&, vnl_vector<T> const&);
template <class T> T             bracket (vnl_vector<T> const &,
					  vnl_matrix<T> const &, 
					  vnl_vector<T> const &);
template <class T> T             cos_angle(vnl_vector<T> const&, vnl_vector<T> const& );
template <class T> double        angle (vnl_vector<T> const&, vnl_vector<T> const&);
template <class T> vnl_matrix<T> outer_product (vnl_vector<T> const&, vnl_vector<T> const&);
template <class T> vnl_vector<T> operator+(T const, vnl_vector<T> const&);
template <class T> vnl_vector<T> operator-(T const, vnl_vector<T> const&);
template <class T> vnl_vector<T> operator*(T const, vnl_vector<T> const&);
template <class T> vnl_vector<T> operator* (vnl_matrix<T> const& m, vnl_vector<T> const& v);
template <class T> vnl_vector<T> element_product(vnl_vector<T> const&,vnl_vector<T> const&);
template <class T> vnl_vector<T> element_quotient(vnl_vector<T> const&,vnl_vector<T> const&);
template <class T> T             cross_2d (vnl_vector<T> const&, vnl_vector<T> const&);
template <class T> vnl_vector<T> cross_3d (vnl_vector<T> const&, vnl_vector<T> const&); 

//----------------------------------------------------------------------

struct vnl_vector_grab_tag { };

template<class T>
class vnl_vector {
public:
  friend class vnl_matrix<T>;

  vnl_vector ();                                               // empty vector.
  vnl_vector (unsigned len);                                   // n-vector.
  vnl_vector (unsigned len, T const& v0);                      // n-vector of vals.
  //vnl_vector (unsigned int len, int n, T v00, ...);          // Opt. values
  vnl_vector (unsigned len, int n, T const values[]);          // safer
  vnl_vector (T const&, T const&, T const&);                   // 3-vector (x,y,z).
  vnl_vector (T const* data_block,unsigned int n);             // n-vector from a block of data.
  vnl_vector (vnl_vector<T> const&);                           // from another vector
  vnl_vector (vnl_vector<T> &that, vnl_vector_grab_tag)
    : num_elmts(that.num_elmts), data(that.data) 
    { that.num_elmts=0; that.data=0; }
  ~vnl_vector();
  
  // -- Return the length, number of elements, dimension of this vector.
  unsigned size() const { return num_elmts; }
  
  //
  inline void put (unsigned int i, T const&);                  // assign value
  inline T get (unsigned int i) const;                         // get value
  
  void fill (T const&);     // set elements to value
  void copy_in(T const *);  // from array[] to vector.
  void copy_out(T *) const; // from vector to array[].
  inline void set (T const *d) { copy_in(d); }
  
  // Return reference to the element at specified index. O(1).
  // No range check is performed.
  T       & operator() (unsigned int i) { return data[i]; }
  T const & operator() (unsigned int i) const { return data[i]; }
  T       & operator[] (unsigned int i) { return data[i]; }
  T const & operator[] (unsigned int i) const { return data[i]; }

  // assignment
  inline vnl_vector<T>& operator= (T const&v) { fill(v); return *this; } // assignment from scalar.
  vnl_vector<T>& operator= (vnl_vector<T> const& rhs);

  vnl_vector<T>& operator+= (T const);
  vnl_vector<T>& operator-= (T const);
  vnl_vector<T>& operator*= (T const);
  vnl_vector<T>& operator/= (T const);

  vnl_vector<T>& operator+= (vnl_vector<T> const&);
  vnl_vector<T>& operator-= (vnl_vector<T> const&);
  
  vnl_vector<T>& pre_multiply (vnl_matrix<T> const&);          // v = M * v
  vnl_vector<T>& post_multiply (vnl_matrix<T> const&);         // v = v * M
  vnl_vector<T>& operator*= (vnl_matrix<T> const&);            // v = v * M

  vnl_vector<T> operator- () const;                            // negation and
  vnl_vector<T> operator+ (T const) const;                     // all binary operations
  vnl_vector<T> operator* (T const) const;                     // return by values.
  vnl_vector<T> operator/ (T const) const;

  vnl_vector<T> operator- (T const) const;

  vnl_vector<T> operator+ (vnl_vector<T> const& ) const;
  vnl_vector<T> operator- (vnl_vector<T> const& ) const;
  vnl_vector<T> operator* (vnl_matrix<T> const& ) const;

  //--------------------------------------------------------------------------------

  // -- access the contiguous block storing the elements in the vector. O(1).
  inline T const* data_block () const { return data; }
  inline T      * data_block () { return data; }

  // iterators
  typedef T element_type;
  typedef T       *iterator;
  inline iterator begin() { return data; }
  inline iterator end() { return data+num_elmts; }
  typedef T const *const_iterator;
  inline const_iterator begin() const { return data; }
  inline const_iterator end() const { return data+num_elmts; }

  //
  vnl_vector<T> apply(T (*f)(T)) const;
  vnl_vector<T> apply(T (*f)(T const&)) const;

  // subvectors
  vnl_vector<T> extract (unsigned int len, unsigned int start=0) const;
  vnl_vector<T>& update (vnl_vector<T> const&, unsigned int start=0);

  // norms etc
  typedef typename vnl_c_vector<T>::abs_t abs_t;
  abs_t squared_magnitude() const { return vnl_c_vector<T>::two_nrm2(begin(), size()); }
  abs_t magnitude() const { return two_norm(); }
  abs_t one_norm() const { return vnl_c_vector<T>::one_norm(begin(), size()); }
  abs_t two_norm() const { return vnl_c_vector<T>::two_norm(begin(), size()); }
  abs_t inf_norm() const { return vnl_c_vector<T>::inf_norm(begin(), size()); }
  abs_t rms     () const { return vnl_c_vector<T>::rms_norm(begin(), size()); }
  T min_value () const { return vnl_c_vector<T>::min_value(begin(), size()); }
  T max_value () const { return vnl_c_vector<T>::max_value(begin(), size()); }
  T mean() const { return vnl_c_vector<T>::mean(begin(), size()); }

  // mutators
  vnl_vector<T>& normalize() { vnl_c_vector<T>::normalize(begin(), size()); return *this; } // v /= v.magnitude()
  void flip();
  
  // coordinates along 4 axes. no boundary checks.
  inline T& x() const { return data[0]; }
  inline T& y() const { return data[1]; }
  inline T& z() const { return data[2]; }
  inline T& t() const { return data[3]; }

  // these check bounds :
  inline void set_x(T const&);
  inline void set_y(T const&);
  inline void set_z(T const&);
  inline void set_t(T const&);

  //
  void assert_size(unsigned sz) const;
  void assert_finite() const;
  bool is_finite() const;

  // comparison
  bool operator== (vnl_vector<T> const& that) const { return this->operator_eq(that); }
  bool operator_eq (vnl_vector<T> const&) const;

  // I/O
  bool read_ascii(istream& s);
  static vnl_vector<T> read(istream& s);

protected:
  unsigned num_elmts;           // Number of elements
  T* data;                      // Pointer to the vnl_vector

#if VCL_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD
# define v vnl_vector<T>
# define m vnl_matrix<T>
  friend T      dot_product      VCL_NULL_TMPL_ARGS (v const&, v const&);
  friend T      inner_product    VCL_NULL_TMPL_ARGS (v const&, v const&);
  friend T      bracket          VCL_NULL_TMPL_ARGS (v const&, m const&, v const&);
  friend T      cos_angle        VCL_NULL_TMPL_ARGS (v const&, v const&);
  friend double angle            VCL_NULL_TMPL_ARGS (v const&, v const&);
  friend m      outer_product    VCL_NULL_TMPL_ARGS (v const&, v const&);
  friend v      operator+        VCL_NULL_TMPL_ARGS (T const,  v const&);
  friend v      operator-        VCL_NULL_TMPL_ARGS (T const,  v const&);
  friend v      operator*        VCL_NULL_TMPL_ARGS (T const,  v const&);
  friend v      operator*        VCL_NULL_TMPL_ARGS (m const&, v const&);
  friend v      element_product  VCL_NULL_TMPL_ARGS (v const&, v const&);
  friend v      element_quotient VCL_NULL_TMPL_ARGS (v const&, v const&);
  friend T      cross_2d         VCL_NULL_TMPL_ARGS (v const&, v const&);
  friend v      cross_3d         VCL_NULL_TMPL_ARGS (v const&, v const&); 
# undef v
# undef m
#endif
};

//--------------------------------------------------------------------------------
//
// Definitions of inline functions
//

// -- Gets the element at specified index and return its value. O(1).
// Range check is performed.

template <class T>
inline T vnl_vector<T>::get (unsigned int index) const {
#if ERROR_CHECKING
  if (index >= this->num_elmts)		// If invalid index specified
    vnl_error_vector_index ("get", index);	// Raise exception
#endif
  return this->data[index];
}

// -- Puts the value at specified index. O(1).
// Range check is performed.

template <class T>
inline void vnl_vector<T>::put (unsigned int index, T const& value) {
#if ERROR_CHECKING
  if (index >= this->num_elmts)		// If invalid index specified
    vnl_error_vector_index ("put", index); // Raise exception
#endif
  this->data[index] = value;	// Assign data value
}

// -- Mutates lhs vector to substract all its elements with value. O(n).

template<class T>
inline vnl_vector<T>& vnl_vector<T>::operator-= (T const value) {
  return *this += (- value);
}

// -- Mutates lhs vector to stores its post-multiplication with
// matrix m: v = v * m. O(m*n).

template<class T>
inline vnl_vector<T>& vnl_vector<T>::operator*= (vnl_matrix<T> const& m) {
   return this->post_multiply(m);
}


// -- Returns new vector with elements of v added with value. O(n).

template<class T>
inline vnl_vector<T> operator+ (T const value, vnl_vector<T> const& v) {
  return v + value;
}

// -- Returns new vector with elements of v substracted with value. O(n).


template<class T>
inline vnl_vector<T> vnl_vector<T>::operator-(T const value) const {
  return (*this) + (- value);
}

// -- Returns new vector with value substracted with elements of v. O(n).

template<class T>
inline vnl_vector<T> operator- (T const value, vnl_vector<T> const& v) {
  return (- v) + value;
}

// -- Returns new vector with elements of v multiplied with value. O(n).

template<class T>
inline vnl_vector<T> operator* (T const value, vnl_vector<T> const& v) {
  return v * value;
}

// set_x(Type) --

template<class T>
inline void vnl_vector<T>::set_x(T const& xx){
  if (size() >= 1)
    data[0] = xx;
}


// set_y(Type) --
template<class T>
inline void vnl_vector<T>::set_y(T const& yy){
  if (size() >= 2)
    data[1] = yy;
}

// set_z(Type) --

template<class T>
inline void vnl_vector<T>::set_z(T const& zz){
  if (size() >= 3)
    data[2] = zz;
}

// set_t(Type) -- Sets the coordinates of a vector with
// a check for valid length. O(1).

template<class T>
inline void vnl_vector<T>::set_t(T const& tt){
  if (size() >= 4)
    data[3] = tt;
}

// -- Read/write vector from/to an istream :
template <class T> vcl_ostream& operator<< (vcl_ostream &, vnl_vector<T> const&);
template <class T> vcl_istream& operator>> (vcl_istream &, vnl_vector<T>      &);

#endif
