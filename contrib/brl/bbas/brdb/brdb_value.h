// This is brl/bbas/brdb/brdb_value.h
#ifndef brdb_value_h_
#define brdb_value_h_

//:
// \file
// \brief The database value class
// \author Matt Leotta
// \date 1/24/05
//
// \verbatim
//  Modifications
// \endverbatim

//
// updated by Yong Zhao
// Apr 4th, 2007
// Make it work with the whole database initially based on Matt's sketch.

#include <brdb/brdb_value_sptr.h>
#include <vcl_string.h>
#include <vcl_map.h>
#include <vcl_cassert.h>
#include <vbl/vbl_ref_count.h>
#include <vcl_iostream.h>
#include <vsl/vsl_binary_io.h>

// forward declaration
template< class T > class brdb_value_t;


//: This abstract class is the base class for database values
class brdb_value : public vbl_ref_count
{
 public:
  
  //: Destructor
  virtual ~brdb_value() {}

  //: Conversion operator
  template< class T >
  operator T () const
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
  virtual vcl_string is_a() const = 0;

  //: Test for equality under polymorphism
  virtual bool eq(const brdb_value& other) const = 0;

  //: Test for inequality (less than) under polymorphism
  virtual bool lt(const brdb_value& other) const = 0;
 
  //: Assign the value of /p other to this if the types are the same
  virtual bool assign(const brdb_value& other) = 0;

  //: Print out the value
  virtual void print() const = 0;

  //: Return a const reference to the global registry of database value classes
  static vcl_map<vcl_string, const brdb_value*> const & registry() { return mut_registry(); }

  //: Create static instances of this struct to register a database value class
  struct registrar{
    registrar(const brdb_value* exemplar);
  };

  friend struct registrar;


  //---------------------------------------------------------------------------
  // Binary I/O functions


  //: binary io read value only
  // handles only the value (without version or type info)
  virtual void b_read_value(vsl_b_istream& is) = 0;

  //: binary io write value only
  // handles only the value (without version or type info)
  virtual void b_write_value(vsl_b_ostream& os) const = 0;


 protected:
  //: Constructor
  brdb_value() {}
  //: Copy Constructor
  brdb_value(const brdb_value&) {}

 private:  
  //: Return a reference to the global registry of database value classes
  static vcl_map<vcl_string, const brdb_value*> & mut_registry();

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
  brdb_value_t<T>() {}
   
  //: Constructor
  explicit brdb_value_t<T>(const T& value)
   : value_(value) {}

  //: Return the string identifying this class
  virtual vcl_string is_a() const { return type_string_; }

  static vcl_string const& type() { return type_string_; }

  //: Clone
  virtual brdb_value * clone() const { return new brdb_value_t<T>(*this); }

  //: Test for equality under polymorphism
  virtual bool eq(const brdb_value& other) const;

  //: Test for inequality (less than) under polymorphism
  virtual bool lt(const brdb_value& other) const;

  //: Assign the value of /p other to this if the types are the same
  virtual bool assign(const brdb_value& other);

  //: Return the string identifying this class
  virtual void print() const { vcl_cout << value_ << "   ";}


  //: Return the value
  T value() const { return value_; }
  
  //: Conversion operator
  operator T () const { return value_; }

  //: Assignment operator
  brdb_value_t<T>& operator = (const T& rhs) { value_ = rhs; return *this; }
  //: Assignment operator
  brdb_value_t<T>& operator = (const brdb_value_t<T>& rhs) { value_ = rhs.value_; return *this; }

  //---------------------------------------------------------------------------
  // Binary I/O functions


  //: binary io read value only
  // handles only the value (without version or type info)
  virtual void b_read_value(vsl_b_istream& is);

  //: binary io write value only
  // handles only the value (without version or type info)
  virtual void b_write_value(vsl_b_ostream& os) const;

 private:
  //: The stored data
  T value_;

  //: The type identifier string for this class
  const static vcl_string type_string_;

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
