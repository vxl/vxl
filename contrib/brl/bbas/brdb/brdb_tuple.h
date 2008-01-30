// This is brl/bbas/brdb/brdb_tuple.h
#ifndef brdb_tuple_h_
#define brdb_tuple_h_
//:
// \file
// \brief A database tuple (table row)
// \author Matthew Leotta
// \date Tue Jan 25 2005
//
// \verbatim
//  Modifications
// \endverbatim

//
// updated by Yong Zhao
// Apr 4th, 2007
// Make it work with the whole database initially based on Matt's sketch.


#include <vcl_vector.h>
#include <brdb/brdb_value.h>
#include <vbl/vbl_ref_count.h>
#include <brdb/brdb_tuple_sptr.h>
#include <vsl/vsl_binary_io.h>

//: A database tuple
class brdb_tuple : public vbl_ref_count
{
  //======================= Constructors / Destructors ========================
 public: 
  //: Default Constructor (0-tuple)
  brdb_tuple() {}

  //: Constructor for a 1-tuple
  template< class T1 >
  explicit brdb_tuple(const T1& value1         ) : values_(1) 
  {
    values_[0] = new brdb_value_t<T1>(value1);
  }

  //: Constructor for a 2-tuple
  template< class T1, class T2 >
  brdb_tuple(const T1& value1, const T2& value2) : values_(2)
  {
    values_[0] = new brdb_value_t<T1>(value1);
    values_[1] = new brdb_value_t<T2>(value2);
  }

  //: Constructor for a 3-tuple
  template< class T1, class T2, class T3 >
  brdb_tuple(const T1& value1, const T2& value2,
                 const T3& value3                  ) : values_(3)
  {
    values_[0] = new brdb_value_t<T1>(value1);
    values_[1] = new brdb_value_t<T2>(value2);
    values_[2] = new brdb_value_t<T3>(value3);
  }

  //: Constructor for a 4-tuple
  template< class T1, class T2, class T3, class T4 >
  brdb_tuple(const T1& value1, const T2& value2,
                 const T3& value3, const T4& value4) : values_(4)
  {
    values_[0] = new brdb_value_t<T1>(value1);
    values_[1] = new brdb_value_t<T2>(value2);
    values_[2] = new brdb_value_t<T3>(value3);
    values_[3] = new brdb_value_t<T4>(value4);
  }

  //: Constructor for a 5-tuple
  template< class T1, class T2, class T3, class T4, class T5 >
  brdb_tuple(const T1& value1, const T2& value2,
                 const T3& value3, const T4& value4,
                 const T5& value5                  ) : values_(5)
  {
    values_[0] = new brdb_value_t<T1>(value1);
    values_[1] = new brdb_value_t<T2>(value2);
    values_[2] = new brdb_value_t<T3>(value3);
    values_[3] = new brdb_value_t<T4>(value4);
    values_[4] = new brdb_value_t<T5>(value5);
  }

  //: Constructor for a 6-tuple
  template< class T1, class T2, class T3, class T4, class T5, class T6 >
  brdb_tuple(const T1& value1, const T2& value2,
                 const T3& value3, const T4& value4,
                 const T5& value5, const T6& value6) : values_(6)
  {
    values_[0] = new brdb_value_t<T1>(value1);
    values_[1] = new brdb_value_t<T2>(value2);
    values_[2] = new brdb_value_t<T3>(value3);
    values_[3] = new brdb_value_t<T4>(value4);
    values_[4] = new brdb_value_t<T5>(value5);
    values_[5] = new brdb_value_t<T6>(value6);
  }

  //: Constructor using a vector of db value references
  explicit brdb_tuple(const vcl_vector<brdb_value*>& values);

  //: Prototype factory using a vector type name
  static brdb_tuple_sptr make_prototype(const vcl_vector<vcl_string>& types);

  //: Copy Constructor
  brdb_tuple(const brdb_tuple& other);

  //: Destructor
  virtual ~brdb_tuple();



  //========================= Accessors / Modifiers ==========================
 public:
  //: Assignment operator
  brdb_tuple& operator = (const brdb_tuple& rhs);

  //: Return the number of attributes (i.e. the size)
  unsigned int arity() const { return values_.size(); }

  //: Returns true if a value is null
  bool is_null(unsigned int index) const
  {
    assert(index < values_.size());
    return !(values_[index]);
  }

  //: Return a reference to an element of the tuple
  // \note this reference becomes invalid when the tuple is deleted
  brdb_value& operator[] (unsigned int index)
  {
    assert(index < values_.size());
    return *values_[index];
  }

  //: Return a const reference to an element of the tuple
  // \note this reference becomes invalid when the tuple is deleted
  const brdb_value& operator[] (unsigned int index) const
  {
    assert(index < values_.size());
    return *values_[index];
  }

  //: Set a value by index with type checking
  //  \retval true if successful
  //  \retval false if invalid index or invalid type
  bool set_value(unsigned int index, const brdb_value& value);

  //: Convenience function for setting a value by index
  //  \retval true if successful
  //  \retval false if invalid index or invalid type
  template<class T>
  bool set( unsigned int index , const T& value )
  {
    return this->set_value(index, static_cast<const brdb_value&>
                                  (brdb_value_t<T>(value))    );
  }


  //: Get a value by index with type checking
  //  \retval true if successful
  //  \retval false if invalid index or invalid type
  bool get_value(unsigned int index, brdb_value& value) const;

  //: Convenience function for getting a value by index
  //  \retval true if successful
  //  \retval false if invalid index or invalid type
  template<class T>
  bool get( unsigned int index , T& value ) const
  {
    brdb_value_t<T> val(value);
    if(!this->get_value(index, static_cast<brdb_value&>(val)))
      return false;
    value = val.value();
    return true;
  }

  //: add a value into the tuple
  bool add_value(const brdb_value& value);

  //: Convenience function for adding a value
  template<class T>
  bool add(const T& value)
  {
    return this->add_value(static_cast<const brdb_value&>(brdb_value_t<T>(value)) );
  }

  //: print all values of tuple
  void print() const;

  //: binary io read values only
  // read values from the stream only (assumes arity and types are initialized)
  void b_read_values(vsl_b_istream& is);

  //: binary io write values only
  // write values to the stream only
  void b_write_values(vsl_b_ostream& os) const;


  //============================== Data Members ===============================
 private:
  //: The values of the attributes
  vcl_vector<brdb_value*> values_;

};


//============================= Tuple Ordering Functors =======================


//: Abstract base class for tuple binary predicates
class brdb_tuple_order
{
 public:
  //: Main function
  virtual bool operator() (const brdb_tuple_sptr& lhs,
                           const brdb_tuple_sptr& rhs) const = 0;

  virtual ~brdb_tuple_order(){}

 protected:
  brdb_tuple_order() {}
};


//: Tuple "less than" binary predicate
class brdb_tuple_less : public brdb_tuple_order
{
 public:
  //: Constructor
  brdb_tuple_less(unsigned int index): index_(index) {}
  //: Main function
  bool operator() (const brdb_tuple_sptr& lhs,
                   const brdb_tuple_sptr& rhs) const
  {
    return (*lhs)[index_] < (*rhs)[index_];
  }

 protected:
  //: The index to sort by
  int index_;
};


//: Tuple "greater than" binary predicate
class brdb_tuple_greater : public brdb_tuple_order
{
 public:
  //: Constructor
  brdb_tuple_greater(unsigned int index): index_(index) {}
  //: Main function
  bool operator() (const brdb_tuple_sptr& lhs,
                   const brdb_tuple_sptr& rhs) const
  {
    return (*lhs)[index_] > (*rhs)[index_];
  }

 protected:
  //: The index to sort by
  int index_;
};





#endif // brdb_tuple_h_
