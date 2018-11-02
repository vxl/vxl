// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for
// Air Force Research Laboratory, 2005.

#ifndef VIL_NITF2_FIELD_FUNCTOR_H
#define VIL_NITF2_FIELD_FUNCTOR_H

class vil_nitf2_field;
class vil_nitf2_enum_values;
class vil_nitf2_index_vector;

#include <map>
#include <string>
#include <utility>
#include <vector>
#include "vil_nitf2_field_sequence.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//-----------------------------------------------------------------------------
//:
// \file
// \brief Functors used by NITF classes
//
// Base class for functors that define a function that takes a
// vil_nitf2_field_sequence and attempts to compute an "out" parameter of
// type T. The function also returns a bool that specifies whether the
// value could be computed.
//
// These functors are used to evaluate NITF field tags and invariably
// call vil_nitf2_field_sequence::get_value(). When calling this method,
// please be sure to set the argument ignore_extra_indexes to true.
// This will allow the functor to be used within a repeat loop
// and reference any preceding tag, inside or outside the repeat loop.
// For example, to define a field sequence like this:
//   FIELD A;
//   REPEAT i=1..N
//     FIELD B(i)
//     FIELD C(i) exists if B(i) > 0
//     FIELD D(i) exists if A > 0
// the same conditional functor can be used in the definitions of fields
// C and D by simply providing the name of the appropriate tag.  When
// the functor is evaluated, the current index (i) is used to fetch the
// tag. By calling get_value() with ignore_extra_indexes set to true,
// the value of A can be fetched without causing an error.

template<typename T>
class vil_nitf2_field_functor
{
 public:
  virtual bool operator() (vil_nitf2_field_sequence* record,
                           const vil_nitf2_index_vector& indexes, T& out_value) = 0;
  virtual ~vil_nitf2_field_functor() = default;

  // Virtual copy method
  virtual vil_nitf2_field_functor<T>* copy() const = 0;
};

//:
// Functor vil_nitf2_field_value defines a function that sets its out parameter
// to a value of a field from a field sequence. The function returns whether
// the field was found. The functor is instantiated with the field's tag.
// The field sequence is passed to the function.
//
// You can override any value by specifying an overrideMap.  For example,
// if 0 was a special value that actually meant 1 but all other values
// (2, 3, 4...) actually meant when they were you could get that effect like this:
// <code>
// std::map< int, int > overrides;
// overrides.insert( std::make_pair( 0, 1 ) );
// new vil_nitf2_field_value( "FIELD_NAME", overrides );
// </code>
template<typename T>
class vil_nitf2_field_value : public vil_nitf2_field_functor<T>
{
 public:
  vil_nitf2_field_value(std::string tag) : tag(std::move(tag)) {}

  vil_nitf2_field_value(std::string tag, std::map<T, T> overrideMap )
    : tag(std::move(tag)), overrides(std::move( overrideMap )) {}

  vil_nitf2_field_functor<T>* copy() const override {
    return new vil_nitf2_field_value(tag, overrides); }

  bool operator() (vil_nitf2_field_sequence* record,
                   const vil_nitf2_index_vector& indexes, T& value) override
  {
    bool success = record->get_value(tag, indexes, value, true);
    if ( success ) {
      //check to see if this value is overridden or not
      typename std::map<T, T>::const_iterator it = overrides.find( value );
      if ( it != overrides.end() ){
        //found override, use it
        value = (*it).second;
      }
    }
    return success;
  }

 private:
  std::string tag;
  std::map<T, T> overrides;
};

//:
// Functor vil_nitf2_multiply_field_values defines a function that sets its out
// parameter to the product of the values of two fields. The predicate
// returns true iff both fields are found or the arg 'use_zero_if_tag_not_found';
// in the latter case, the out parameter is set to 0.
//
class vil_nitf2_multiply_field_values : public vil_nitf2_field_functor<int>
{
 public:
  vil_nitf2_multiply_field_values(std::string  tag_1,
                                  std::string  tag_2,
                                  bool use_zero_if_tag_not_found = false)
    : tag_1(std::move(tag_1)),
      tag_2(std::move(tag_2)),
      use_zero_if_tag_not_found(use_zero_if_tag_not_found) {}

  vil_nitf2_field_functor<int>* copy() const override {
    return new vil_nitf2_multiply_field_values(
      tag_1, tag_2, use_zero_if_tag_not_found);
  }

  bool operator() (vil_nitf2_field_sequence* record,
                   const vil_nitf2_index_vector& indexes, int& value) override;

 private:
  std::string tag_1;
  std::string tag_2;
  bool use_zero_if_tag_not_found;
};

//:
// Functor vil_nitf2_max_field_value_plus_offset_and_threshold defines a
// function that sets its out parameter to either the value of a specified
// field plus an offset, or a specified minimum value, whichever is greater.
// The function returns whether the field was found.
//
class vil_nitf2_max_field_value_plus_offset_and_threshold : public vil_nitf2_field_functor<int>
{
 public:
  vil_nitf2_max_field_value_plus_offset_and_threshold(
    std::string tag, int offset, int min_threshold = 0, int tag_factor = 1 )
    : tag(std::move(tag)), offset(offset), min_threshold(min_threshold), tag_factor( tag_factor ) {}

  vil_nitf2_field_functor<int>* copy() const override {
    return new vil_nitf2_max_field_value_plus_offset_and_threshold(
      tag, offset, min_threshold, tag_factor); }

  bool operator() (vil_nitf2_field_sequence* record,
                   const vil_nitf2_index_vector& indexes, int& value) override;

 private:
  std::string tag;
  int offset;
  int min_threshold;
  int tag_factor;
};

//:
// Functor vil_nitf2_field_value_greater_than defines a comparison predicate that
// sets its out parameter to true if a specified field from a field
// sequence is found and its value is greater than a specified threshold.
// The predicate returns whether the field was found.
//
template<typename T>
class vil_nitf2_field_value_greater_than: public vil_nitf2_field_functor<bool>
{
 public:
  vil_nitf2_field_value_greater_than(std::string tag, T threshold)
    : tag(std::move(tag)), threshold(threshold) {}

  vil_nitf2_field_functor<bool>* copy() const override {
    return new vil_nitf2_field_value_greater_than(tag, threshold); }

  bool operator() (vil_nitf2_field_sequence* record,
                   const vil_nitf2_index_vector& indexes, bool& result) override {
    T value;
    if (record->get_value(tag, indexes, value, true)) {
      result = value > threshold;
      return true;
    }
    else {
      return false;
    }
  }
 private:
  std::string tag;
  T threshold;
};

//:
// Functor vil_nitf2_field_specified defines a comparison predicate that sets
// its out parameter to true iff the specified field is not blank.
//
class vil_nitf2_field_specified: public vil_nitf2_field_functor<bool>
{
 public:
  vil_nitf2_field_specified(std::string tag) : tag(std::move(tag)) {}

  vil_nitf2_field_functor<bool>* copy() const override {
    return new vil_nitf2_field_specified(tag); }

  bool operator() (vil_nitf2_field_sequence* record,
                   const vil_nitf2_index_vector& indexes, bool& result) override;

 private:
  std::string tag;
};

//:
// Functor vil_nitf2_field_value_one_of defines a predicate that sets its out
// parameter to true iff the value of the specified tag equals one of
// the elements of a std::vector of acceptable values.
//
template<typename T>
class vil_nitf2_field_value_one_of: public vil_nitf2_field_functor<bool>
{
 public:
  /// Constructor to specify a std::vector of acceptable values
  vil_nitf2_field_value_one_of(std::string tag, std::vector<T> acceptable_values)
    : tag(std::move(tag)), acceptable_values(std::move(acceptable_values)) {}

  /// Constructor to specify only one acceptable value
  vil_nitf2_field_value_one_of(std::string tag, T acceptable_value)
    : tag(std::move(tag)), acceptable_values(1, acceptable_value) {}

  vil_nitf2_field_functor<bool>* copy() const override {
    return new vil_nitf2_field_value_one_of(tag, acceptable_values); }

  bool operator() (vil_nitf2_field_sequence* record,
                   const vil_nitf2_index_vector& indexes, bool& result) override
  {
    result = false;
    T val;
    if (record->get_value(tag, indexes, val, true)) {
      typename std::vector<T>::iterator it;
      for (it = acceptable_values.begin(); it != acceptable_values.end(); ++it) {
        if ((*it) == val) {
          result = true;
          break;
        }
      }
      return true;
    }
    // Field not defined
    return false;
  }

 protected:
  std::string tag;
  std::vector<T> acceptable_values;
};

//:
// Functor vil_nitf2_choose_field_value defines a function that sets its out
// parameter to a value of one of two fields of a field sequence. The
// field chosen is determined by the evaluating the functor passed as
// argument 'choose_tag_1_predicate': tag_1 is chosen if it evaluates to true;
// tag_2, otherwise.
//
template<typename T>
class vil_nitf2_choose_field_value : public vil_nitf2_field_functor<T>
{
 public:
  /// Constructor. I take ownership of inDecider.
  vil_nitf2_choose_field_value(std::string  tag_1, std::string  tag_2,
                               vil_nitf2_field_functor<bool>* choose_tag_1_predicate)
    : tag_1(std::move(tag_1)), tag_2(std::move(tag_2)), choose_tag_1_predicate(choose_tag_1_predicate) {}

  vil_nitf2_field_functor<T>* copy() const override {
    return new vil_nitf2_choose_field_value(
      tag_1, tag_2, choose_tag_1_predicate->copy()); }

  ~vil_nitf2_choose_field_value() override {
    if (choose_tag_1_predicate) delete choose_tag_1_predicate;
  }

  bool operator() (vil_nitf2_field_sequence* record,
                   const vil_nitf2_index_vector& indexes, T& value) override {
    bool choose_tag_1;
    if ((*choose_tag_1_predicate)(record, indexes, choose_tag_1)) {
      if (choose_tag_1) return record->get_value(tag_1, indexes, value, true);
      else return record->get_value(tag_2, indexes, value, true);
    }
    else return false;
  }
 private:
  std::string tag_1;
  std::string tag_2;
  vil_nitf2_field_functor<bool>* choose_tag_1_predicate;
};

// Functor vil_nitf2_constant_functor defines a function that sets its
// output value to a constant that does not depend on the field sequence
// passed to it.
//
template<typename T>
class vil_nitf2_constant_functor : public vil_nitf2_field_functor<T>
{
 public:
  vil_nitf2_constant_functor(T value) : value_(value) {}

  vil_nitf2_constant_functor* copy() const override {
    return new vil_nitf2_constant_functor(value_);
  }

  ~vil_nitf2_constant_functor() override = default;

  bool operator() (vil_nitf2_field_sequence* /*record*/,
                  const vil_nitf2_index_vector& /*indexes*/, T& value) override {
    value = value_;
    return true;
  }

 private:
  T value_;
};

#endif // VIL_NITF2_FIELD_FUNCTOR_H
