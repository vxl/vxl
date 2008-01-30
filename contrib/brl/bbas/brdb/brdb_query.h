// This is brl/bbas/brdb/brdb_query.h
#ifndef brdb_query_h_
#define brdb_query_h_
//:
// \file
// \brief A database query (table row)
// \author Zhao Yong
// \date Mar 25, 2007
//
// \verbatim
//  Modifications
//    May 2, 2007   Matt Leotta   Converted the query to a query binary tree
// \endverbatim


#include <brdb/brdb_value.h>
#include <brdb/brdb_query_aptr.h>




//: A database query (abstract base class)
class brdb_query
{
 protected:
  //: Destructor - private to prevent allocation on the heap
  virtual ~brdb_query() {}
  friend class vcl_auto_ptr<brdb_query>;

 public:
  virtual brdb_query_aptr clone() const = 0;
  virtual brdb_query_aptr complement() const = 0;

  //: query comparison type enum
  typedef enum comp_type {
    NONE = 0, // take none of the values (logical inverse of ALL)
    ALL  = 1, // take all values, resgard compared value
    EQ   = 2, // ==
    NEQ  = 3, // !=
    GT   = 4, // >
    LEQ  = 5, // <=
    LT   = 6, // <
    GEQ  = 7, // >=
  };
};


//: Return the complement query comparison type
brdb_query::comp_type operator ~(brdb_query::comp_type type);

//: Return the complement query
brdb_query_aptr operator ~(const brdb_query_aptr& q);


//: A database query that contains two subqueries (abstract)
class brdb_query_branch : public brdb_query
{
 public:
  //: Constructor from two auto pointers (takes ownership)
  brdb_query_branch(brdb_query_aptr q1, brdb_query_aptr q2);

  //: Copy constructor
  brdb_query_branch(const brdb_query_branch& other);

  //: Assignment operator
  brdb_query_branch& operator = (const brdb_query_branch& rhs);

  //: return a const reference to the first sub-query
  const brdb_query_aptr& first() const { return first_; }
  //: return a const reference to the second sub-query
  const brdb_query_aptr& second() const { return second_; }

 protected:
  brdb_query_aptr first_;
  brdb_query_aptr second_;
};


//: A database query (the intersection of two queries)
class brdb_query_and : public brdb_query_branch
{
 public:
  //: Constructor from two auto pointers (takes ownership)
  brdb_query_and(brdb_query_aptr q1, brdb_query_aptr q2);

  //: Constructor from two queries (makes copies)
  brdb_query_and(const brdb_query& q1, const brdb_query& q2);

  brdb_query_aptr clone() const;

  //: Destructor
  ~brdb_query_and() {}

  brdb_query_aptr complement() const;

};


//: Create the union of two queries (takes ownership)
brdb_query_aptr operator &(brdb_query_aptr q1, brdb_query_aptr q2);


//: A database query (the union of two queries)
class brdb_query_or : public brdb_query_branch
{
 public:
  //: Constructor from two auto pointers (takes ownership)
  brdb_query_or(brdb_query_aptr q1, brdb_query_aptr q2);

  //: Constructor from two queries (makes copies)
  brdb_query_or(const brdb_query& q1, const brdb_query& q2);

  brdb_query_aptr clone() const;

  //: Destructor
  ~brdb_query_or() {}

  brdb_query_aptr complement() const;

};


//: Create the intersection of two queries (takes ownership)
brdb_query_aptr operator |(brdb_query_aptr q1, brdb_query_aptr q2);


//: A database query for comparisons
class brdb_query_comp : public brdb_query
{
 public: 

  //: make a query on a certain attribute, with a certain type of comparison to a value
  brdb_query_comp(const vcl_string& attribute_name,
                  const brdb_query::comp_type& type,
                  vcl_auto_ptr<brdb_value> value);

  //: Copy Constructor
  brdb_query_comp(const brdb_query_comp& other);

  //: Destructor
  ~brdb_query_comp() {}

  //: Assignment operator
  brdb_query_comp& operator = (const brdb_query_comp& rhs);

  //: clone this query
  brdb_query_aptr clone() const;

  //: produce the complement query
  brdb_query_aptr complement() const;

  //: get the value
  const brdb_value& value() const { return *value_; }

  //: get the name of the attribute
  vcl_string attribute_name() const { return attribute_name_; }

  //: get query comparison type
  brdb_query::comp_type comparison_type() const { return comparison_type_; }

  //: whether some value pass the constraint
  bool pass(const brdb_value& input_value) const;

  //============================== Data Members ===============================
 private:

  //: indicates the name of the attribute which this query constraint will be applied on
  vcl_string attribute_name_; 

  //: comparison_type_ indicates how the constraint will be;
  brdb_query::comp_type comparison_type_;

  //: the value which will be used by the constraints
  vcl_auto_ptr<brdb_value> value_;

};



template <class T>
brdb_query_aptr
    brdb_query_comp_new(const vcl_string& attribute_name,
                        const brdb_query::comp_type& type,
                        const T& value)
{
  return brdb_query_aptr(new brdb_query_comp(attribute_name, type,
                            vcl_auto_ptr<brdb_value>(new brdb_value_t<T>(value))));
}



#endif // brdb_query_h_
