//:
// \file
// \brief a brdb_query to be submitted to brdb_database_manager
// \author Yong Zhao
// \date Tue Mar 23, 2007
//
// \verbatim
//  Modifications
//   (none yet)
// \endverbatim

#include <brdb/brdb_query.h>
#include <vcl_cassert.h>


//: Return the complement query comparison type
brdb_query::comp_type operator ~(brdb_query::comp_type type)
{
  return static_cast<brdb_query::comp_type>(2*(type/2) + 1-(type%2));
}


//: Return the complement query
brdb_query_aptr operator ~(const brdb_query_aptr& q)
{
  if (!q.get())
    return brdb_query_aptr(NULL);

  return q->complement();
}


//: Create the union of two queries (takes ownership)
brdb_query_aptr operator &(brdb_query_aptr q1, brdb_query_aptr q2)
{
  if (!q1.get()){
    if (!q2.get())
      return brdb_query_aptr(NULL);
    else
      return q2;
  }
  else if (!q2.get())
    return q1;

  return brdb_query_aptr(new brdb_query_and(q1,q2));
}


//: Create the intersection of two queries (takes ownership)
brdb_query_aptr operator |(brdb_query_aptr q1, brdb_query_aptr q2)
{
  if (!q1.get()){
    if (!q2.get())
      return brdb_query_aptr(NULL);
    else
      return q2;
  }
  else if (!q2.get())
    return q1;

  return brdb_query_aptr(new brdb_query_or(q1,q2));
}

//============================= brdb_query_branch =============================

brdb_query_branch::brdb_query_branch(brdb_query_aptr q1, brdb_query_aptr q2)
  : first_(q1), second_(q2) // assumes ownership of these two queries
{
  assert(first_.get() && second_.get());
}


//: Copy constructor
brdb_query_branch::brdb_query_branch(const brdb_query_branch& other)
  : first_(other.first_->clone()),
    second_(other.second_->clone())
{
}


//: Assignment operator
brdb_query_branch&
brdb_query_branch::operator = (const brdb_query_branch& rhs)
{
  this->first_ = rhs.first_->clone();
  this->second_ = rhs.second_->clone();
  return *this;
}


//============================== brdb_query_and ===============================

//: Constructor from two auto pointers (takes ownership)
brdb_query_and::brdb_query_and(brdb_query_aptr q1, brdb_query_aptr q2)
  : brdb_query_branch(q1,q2)
{
}

//: Constructor from two queries (makes copies)
brdb_query_and::brdb_query_and(const brdb_query& q1, const brdb_query& q2)
  : brdb_query_branch(q1.clone(),q2.clone())
{
}


brdb_query_aptr
brdb_query_and::clone() const
{
  return brdb_query_aptr(new brdb_query_and(*this));
}


brdb_query_aptr
brdb_query_and::complement() const
{
  brdb_query_aptr q1c = first_->complement();
  brdb_query_aptr q2c = second_->complement();
  return brdb_query_aptr(new brdb_query_or(q1c,q2c));
}

//============================== brdb_query_or ================================

//: Constructor from two auto pointers (takes ownership)
brdb_query_or::brdb_query_or(brdb_query_aptr q1, brdb_query_aptr q2)
  : brdb_query_branch(q1,q2)
{
}

//: Constructor from two queries (makes copies)
brdb_query_or::brdb_query_or(const brdb_query& q1, const brdb_query& q2)
  : brdb_query_branch(q1.clone(),q2.clone())
{
}


brdb_query_aptr
brdb_query_or::clone() const
{
  return brdb_query_aptr(new brdb_query_or(*this));
}


brdb_query_aptr
brdb_query_or::complement() const
{
  brdb_query_aptr q1c = first_->complement();
  brdb_query_aptr q2c = second_->complement();
  return brdb_query_aptr(new brdb_query_and(q1c,q2c));
}


//============================= brdb_query_comp ===============================


//: make a query on a certain attribute, with a certain type of comparison to a value
brdb_query_comp::brdb_query_comp(const vcl_string& attribute_name,
                                 const brdb_query::comp_type& type,
                                 vcl_auto_ptr<brdb_value> value)
  : attribute_name_(attribute_name),
    comparison_type_(type),
    value_(value)
{
}


//: Copy Constructor
brdb_query_comp::brdb_query_comp(const brdb_query_comp& other)
  : attribute_name_(other.attribute_name_),
    comparison_type_(other.comparison_type_),
    value_(other.value_->clone())
{
}


brdb_query_aptr
brdb_query_comp::complement() const
{
  return brdb_query_aptr(new brdb_query_comp(attribute_name_, ~comparison_type_,
                                             vcl_auto_ptr<brdb_value>(value_->clone())));
}


//: clone this query
brdb_query_aptr
brdb_query_comp::clone() const
{
  return brdb_query_aptr(new brdb_query_comp(*this));
}

//: Assignment operator
brdb_query_comp&
brdb_query_comp::operator = (const brdb_query_comp& rhs)
{
  this->attribute_name_ = rhs.attribute_name_;
  this->comparison_type_ = rhs.comparison_type_;
  this->value_ = vcl_auto_ptr<brdb_value>(rhs.value_->clone());
  return *this;
}

//: whether some value passes the constraint
bool
brdb_query_comp::pass(const brdb_value& input_value) const
{
  switch (comparison_type_)
  {
   case EQ:
    return input_value == (*value_);
   case GT:
    return input_value > (*value_);
   case GEQ:
    return input_value >= (*value_);
   case LT:
    return input_value < (*value_);
   case LEQ:
    return input_value <= (*value_);
   case NEQ:
    return input_value != (*value_);
   case ALL:
    return true;
   case NONE:
    return false;
   default:
    assert(!"nonexisting operator; use EQ, NEQ, GT, GEQ, LT, LEQ, ALL or NONE");
    return false;
  }
}
