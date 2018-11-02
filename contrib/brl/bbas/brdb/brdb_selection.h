#ifndef brdb_selection_h_
#define brdb_selection_h_
//:
// \file
// \a selection which is the immediate result of a selection operation
// \author Yong Zhao
// \date Tue Mar 23, 2007
//
// \verbatim
//  Modifications
//   May 3, 2007   Matt Leotta   Updated to use the query as a binary tree
// \endverbatim


#include <vector>
#include <iostream>
#include <set>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_ref_count.h>
#include <brdb/brdb_relation_sptr.h>
#include <brdb/brdb_selection_sptr.h>
#include <brdb/brdb_value.h>
#include <brdb/brdb_tuple_sptr.h>
#include <brdb/brdb_query_aptr.h>


typedef std::set<std::vector<brdb_tuple_sptr>::iterator> selection_t;

class brdb_selection : public vbl_ref_count
{
 public:

  //: Constructor
  brdb_selection(const brdb_relation_sptr& relation, brdb_query_aptr query);

  //: Constructor
  brdb_selection(const brdb_selection_sptr& selection, brdb_query_aptr query);

  //: destructor
  ~brdb_selection() override;

  //: get the iterator pointing to the beginning of the selection set
  selection_t::const_iterator begin();

  //: get the iterator pointing to the ending of the selection set
  selection_t::const_iterator end();

  //: return true if selection is empty
  bool empty();

  //: update the selected tuple (suppose there is only one tuple been selected)
  bool update_selected_tuple(const brdb_tuple_sptr& new_tuple);

  //: update the selected tuple with one new attribute value (suppose there is only one tuple been selected)
  bool update_selected_tuple(const std::string& attribute_name, const brdb_value& value);

  //: Convenience function for updating the selected tuple with one new attribute value (suppose there is only one tuple been selected)
  //  \retval true if successful
  //  \retval false if invalid index or invalid type
  template<class T>
  bool update_selected_tuple_value(const std::string& attribute_name, const T& value )
  {
    return this->update_selected_tuple(attribute_name, static_cast<const brdb_value&>(brdb_value_t<T>(value)));
  }

  //: get value from selected tupe
  bool get_value(const std::string& attribute_name, brdb_value& value);

  //: get value pointer from selected tupe
  bool get_value(const std::string& attribute_name, brdb_value_sptr& value);

  //: convenient function to get value from selected tupe
  template<class T>
  bool get(const std::string& attribute_name, T& value)
  {
    brdb_value_t<T> val;
    if (! this->get_value(attribute_name, static_cast<brdb_value&>(val)))
      return false;
    else
    {
      value = val.value();
      return true;
    }
  }

  //: function to get value from selected tupes with an index (say, the value of ith selected tuples)
  bool get_value(const std::string& attribute_name,  unsigned int index, brdb_value& value);

  //: convenient function to get value from selected tupes with an index (say, the value of ith selected tuples)
  template<class T>
  bool get(const std::string& attribute_name, unsigned int index, T& value)
  {
    brdb_value_t<T> val;
    if (! this->get_value(attribute_name, index, static_cast<brdb_value&>(val)))
      return false;
    else
    {
      value = val.value();
      return true;
    }
  }

  //: get sql view (independent relation) from selection
  brdb_relation_sptr get_sqlview();

  //: apply AND operation on two selections, return the resulting selection
  brdb_selection_sptr selection_and(const brdb_selection_sptr& s);

  //: apply OR operation on two selections, return the resulting selection
  brdb_selection_sptr selection_or(const brdb_selection_sptr& s);

  //: apply XOR operation on two selections, return the resulting selection
  brdb_selection_sptr selection_xor(const brdb_selection_sptr& s);

  //: apply NOT operation on selection, return the resulting selection
  brdb_selection_sptr selection_not();

  //: print selection
  void print();

  //: get relation
  brdb_relation_sptr get_relation() const;

  //: delete the selected tuples
  void delete_tuples();

  //: return the size of selection;
  unsigned int size();

 private:
  //: Constructor with no query
  brdb_selection(const brdb_relation_sptr& relation);

  //: check timestamp of selection and update selection if needed;
  void check_and_update();

  //: apply a query to produce a selected set
  bool produce(const brdb_query_aptr& q, selection_t& s);

  //: apply a query to refine a selected set
  bool refine(const brdb_query_aptr& q, selection_t& s);

  //: see whether a tuple exists in this selection;
  bool tuple_exist(const brdb_tuple_sptr& tuple);

  //: see whether the selection contains a relation iterator
  bool contains(const std::vector<brdb_tuple_sptr>::iterator& relation_itr) const;

 private:
  //: time stamp
  unsigned long time_stamp_;

  //: the set of iterators to selected tuples
  selection_t selected_set_;

  //: the actual relation on which this selection is applied;
  brdb_relation_sptr relation_;

  //: store the original query which generate this selection, need this information for update selection
  brdb_query_aptr query_;
};


//: and operator
inline brdb_selection_sptr operator & (const brdb_selection_sptr& lhs,
                                       const brdb_selection_sptr& rhs)
{
  return lhs->selection_and(rhs);
}

//: or operator
inline brdb_selection_sptr operator | (const brdb_selection_sptr& lhs,
                                       const brdb_selection_sptr& rhs)
{
  return lhs->selection_or(rhs);
}

//: xor operator
inline brdb_selection_sptr operator ^ (const brdb_selection_sptr& lhs,
                                       const brdb_selection_sptr& rhs)
{
  return lhs->selection_xor(rhs);
}

//: not operator
inline brdb_selection_sptr operator ~ (const brdb_selection_sptr& rhs)
{
  return rhs->selection_not();
}

#endif // brdb_selection_h_
