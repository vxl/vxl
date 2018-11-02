// This is brl/bbas/brdb/brdb_value.h
#ifndef brdb_value_h_
#define brdb_value_h_
//:
// \file
// \brief The database value class
// \author Matt Leotta
// \date January 24, 2005
//
// \verbatim
//  Modifications
//   Apr 4, 2007 - Yong Zhao - Make it work with the whole database initially based on Matt's sketch.
// \endverbatim

#include <string>
#include <map>
#include <iostream>
#include <utility>
#include <brdb/brdb_value_sptr.h>
#include <cassert>
#include <vbl/vbl_ref_count.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>

// forward declaration
template< class T > class brdb_value_t;

//: This abstract class is the base class for database values
class brdb_value : public vbl_ref_count
{
 public:

  //: Destructor
  ~brdb_value() override = default;

  //: Return the actual value
  template< class T >
  T val() const
  {
    const brdb_value_t<T>* type_val = dynamic_cast<const brdb_value_t<T>*>(this);
    assert(type_val);
    return type_val->value();
  }


  //: Assignment operator
  template< class T >
  brdb_value& operator = (const T& rhs)
  {
    assert(this->is_a() == brdb_value_t<T>::type());
    this->assign(brdb_value_t<T>(rhs));
    return *this;
  }

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual brdb_value* clone() const = 0;

  //: Return the string identifying this class
  virtual std::string is_a() const = 0;

  //: Test for equality under polymorphism
  virtual bool eq(const brdb_value& other) const = 0;

  //: Test for inequality (less than) under polymorphism
  virtual bool lt(const brdb_value& other) const = 0;

  //: Assign the value of /p other to this if the types are the same
  virtual bool assign(const brdb_value& other) = 0;

  //: Print out the value
  virtual void print() const = 0;

  //: Return a const reference to the global registry of database value classes
  static std::map<std::string, const brdb_value*> const & registry() { return mut_registry(); }

  //: Create static instances of this struct to register a database value class
  struct registrar{
    registrar(const brdb_value* exemplar);
  };

  friend struct registrar;

  //---------------------------------------------------------------------------
  // Binary I/O functions

  //: binary io read value only
  //  Handles only the value (without version or type info)
  virtual void b_read_value(vsl_b_istream&)
  {
    std::cout << "Warning: calling binary read on parent value class, this value is not being read" << std::endl;
  }

  //: binary io write value only
  //  Handles only the value (without version or type info)
  virtual void b_write_value(vsl_b_ostream&) const
  {
    std::cout << "Warning: calling binary write on parent value class, this value is not being saved" << std::endl;
  }


 protected:
  //: Constructor
  brdb_value() = default;
  //: Copy Constructor
  brdb_value(const brdb_value&) : vbl_ref_count() {}

 private:
  //: Return a reference to the global registry of database value classes
  static std::map<std::string, const brdb_value*> & mut_registry();
};

//: Equals operator
inline bool operator == (const brdb_value& lhs,
                         const brdb_value& rhs)
{
  assert(lhs.is_a() == rhs.is_a());
  return lhs.eq(rhs);
}

//: Not Equal operator
inline bool operator != (const brdb_value& lhs,
                         const brdb_value& rhs)
{
  assert(lhs.is_a() == rhs.is_a());
  return !lhs.eq(rhs);
}

//: Less than operator
inline bool operator < (const brdb_value& lhs,
                        const brdb_value& rhs)
{
  assert(lhs.is_a() == rhs.is_a());
  return lhs.lt(rhs);
}
//: Less than operator
inline bool operator <= (const brdb_value& lhs,
                         const brdb_value& rhs)
{
  assert(lhs.is_a() == rhs.is_a());
  return !rhs.lt(lhs);
}

//: Greater than operator
inline bool operator > (const brdb_value& lhs,
                        const brdb_value& rhs)
{
  assert(lhs.is_a() == rhs.is_a());
  return rhs.lt(lhs);
}

//: Greater than or equal to operator
inline bool operator >= (const brdb_value& lhs,
                         const brdb_value& rhs)
{
  assert(lhs.is_a() == rhs.is_a());
  return !lhs.lt(rhs);
}

//: A templated database value class
template< class T >
class brdb_value_t : public brdb_value
{
 public:
  //: Default Constructor
  brdb_value_t<T>() = default;

  //: Constructor
  explicit brdb_value_t<T>(T  value)
   : value_(std::move(value)) {}

  //: Return the string identifying this class
  std::string is_a() const override { return get_type_string(); }

  static std::string const& type() { return get_type_string(); }

  //: Clone
  brdb_value * clone() const override { return new brdb_value_t<T>(*this); }

  //: Test for equality under polymorphism
  bool eq(const brdb_value& other) const override;

  //: Test for inequality (less than) under polymorphism
  bool lt(const brdb_value& other) const override;

  //: Assign the value of /p other to this if the types are the same
  bool assign(const brdb_value& other) override;

  //: Return the string identifying this class
  void print() const override { std::cout << value_ << "   ";}

  //: Return the value
  T value() const { return value_; }

  //: Conversion operator
  operator T() const { return value_; }

  //: Assignment operator
  brdb_value_t<T>& operator = (const T& rhs) { value_ = rhs; return *this; }
  //: Assignment operator
  brdb_value_t<T>& operator = (const brdb_value_t<T>& rhs) { value_ = rhs.value_; return *this; }

  //---------------------------------------------------------------------------
  // Binary I/O functions

  //: binary io read value only
  //  Handles only the value (without version or type info)
  void b_read_value(vsl_b_istream& is) override;

  //: binary io write value only
  //  Handles only the value (without version or type info)
  void b_write_value(vsl_b_ostream& os) const override;

 private:
  //: The stored data
  T value_;

  //: The type identifier string for this class
  static const std::string& get_type_string();
};

template< class T >
inline bool operator == (const brdb_value_t<T>& lhs,
                         const brdb_value_t<T>& rhs)
{
  return lhs.value() == rhs.value();
}

template< class T >
inline bool operator == (const T& lhs,
                         const brdb_value_t<T>& rhs)
{
  return lhs == rhs.value();
}

template< class T >
inline bool operator == (const brdb_value_t<T>& lhs,
                         const T& rhs)
{
  return lhs.value() == rhs;
}

template< class T >
inline bool operator != (const brdb_value_t<T>& lhs,
                         const brdb_value_t<T>& rhs)
{
  return lhs.value() != rhs.value();
}

template< class T >
inline bool operator != (const T& lhs,
                         const brdb_value_t<T>& rhs)
{
  return lhs != rhs.value();
}

template< class T >
inline bool operator != (const brdb_value_t<T>& lhs,
                         const T& rhs)
{
  return lhs.value() != rhs;
}

template< class T >
inline bool operator <= (const brdb_value_t<T>& lhs,
                         const brdb_value_t<T>& rhs)
{
  return lhs.value() <= rhs.value();
}

template< class T >
inline bool operator <= (const T& lhs,
                         const brdb_value_t<T>& rhs)
{
  return lhs <= rhs.value();
}

template< class T >
inline bool operator <= (const brdb_value_t<T>& lhs,
                         const T& rhs)
{
  return lhs.value() <= rhs;
}

template< class T >
inline bool operator < (const brdb_value_t<T>& lhs,
                        const brdb_value_t<T>& rhs)
{
  return lhs.value() < rhs.value();
}

template< class T >
inline bool operator < (const T& lhs,
                        const brdb_value_t<T>& rhs)
{
  return lhs < rhs.value();
}

template< class T >
inline bool operator < (const brdb_value_t<T>& lhs,
                        const T& rhs)
{
  return lhs.value() < rhs;
}

template< class T >
inline bool operator >= (const brdb_value_t<T>& lhs,
                         const brdb_value_t<T>& rhs)
{
  return lhs.value() >= rhs.value();
}

template< class T >
inline bool operator >= (const T& lhs,
                         const brdb_value_t<T>& rhs)
{
  return lhs >= rhs.value();
}

template< class T >
inline bool operator >= (const brdb_value_t<T>& lhs,
                         const T& rhs)
{
  return lhs.value() >= rhs;
}

template< class T >
inline bool operator > (const brdb_value_t<T>& lhs,
                        const brdb_value_t<T>& rhs)
{
  return lhs.value() > rhs.value();
}

template< class T >
inline bool operator > (const T& lhs,
                        const brdb_value_t<T>& rhs)
{
  return lhs > rhs.value();
}

template< class T >
inline bool operator > (const brdb_value_t<T>& lhs,
                        const T& rhs)
{
  return lhs.value() > rhs;
}

#endif // brdb_value_h_
