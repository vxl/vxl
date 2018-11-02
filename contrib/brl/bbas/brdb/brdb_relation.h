// This is brl/bbas/brdb/brdb_relation.h
#ifndef brdb_relation_h_
#define brdb_relation_h_
//:
// \file
// \brief A database relation (table)
// \author Matthew Leotta
// \date Tue Jan 26 2005
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//
// updated by Yong Zhao
// Apr 4th, 2007
// Make it work with the whole database initially based on Matt's sketch.


#include <vector>
#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_ref_count.h>
#include <brdb/brdb_tuple_sptr.h>
#include <brdb/brdb_relation_sptr.h>
#include <vsl/vsl_binary_io.h>

// forward declarations
class brdb_value;


//: A database tuple
class brdb_relation : public vbl_ref_count
{
  //======================= Constructors / Destructors ========================
 public:
  // Default Constructor
  brdb_relation();

  //: Constructor - create an empty relation but define the columns
  brdb_relation( std::vector<std::string>  names,
                 std::vector<std::string>  types );

  //: Constructor - create a relation populated with tuples
  //  If types are not provided they will be inferred from the tuples.
  //  All tuples must have the same types and arity.
  brdb_relation( const std::vector<std::string>& names,
                 std::vector<brdb_tuple_sptr>  tuples,
                 std::vector<std::string>  types = std::vector<std::string>() );

  // Destructor
  ~brdb_relation() override;


  //========================= Accessors / Modifiers ===========================
  //: Return the number of tuples (i.e. the number of rows in the table)
  unsigned int size() const { return static_cast<unsigned>(tuples_.size()); }
  //: Return the number of attributes in a tuple (i.e. the number of columns in the table)
  unsigned int arity() const { return static_cast<unsigned>(names_.size()); }

  //: Return the name for \p index
  // \note returns the empty string if the index is out of range
  std::string name(unsigned int index) const;

  //: Return the index for the attribute with \p name
  // \note returns the arity (max index + 1) if name is not found
  unsigned int index(const std::string& name) const;

  //: Return the type by attribute name
  std::string type(const std::string& name) const;

  //: Return the type by index
  std::string type(unsigned int index) const;

  //: Return true if there is an attribute in the relation with such a name
  bool exists(const std::string& name) const;

  //: Return true if there are no tuples in the relation.
  bool empty() const {return tuples_.empty();}

  //: Sort the tuples by a certain attribute name
  bool order_by(const std::string& name, bool ascending=true);

  //: Sort the tuples by a certain attribute index
  bool order_by(unsigned int index, bool ascending=true);

  //: clear the relation
  // Keep the names and types, but remove all the tuples
  void clear();

  //: Add one tuple to relation
  bool add_tuple(const brdb_tuple_sptr& new_tuple);

  //: insert a tuple at certain position
  bool insert_tuple(const brdb_tuple_sptr& new_tuple, const std::vector<brdb_tuple_sptr>::iterator& pos);

  //: remove a tuple at certain position from the relation
  bool remove_tuple(const std::vector<brdb_tuple_sptr>::iterator& pos);

  //: Set a value by name
  bool set_value(std::vector<brdb_tuple_sptr>::iterator pos, const std::string& name, const brdb_value& value);

  //: Convenience function for setting a value by name
  template<class T>
  bool set( std::vector<brdb_tuple_sptr>::iterator pos, const std::string& name , const T& value );

  //: Get a value by name
  bool get_value(std::vector<brdb_tuple_sptr>::iterator pos, const std::string& name, brdb_value& value) const;

  //: Convenience function for getting a value by name
  template<class T>
  bool get(std::vector<brdb_tuple_sptr>::iterator pos, const std::string& name , const T& value ) ;

  //: print out the relation
  void print() const;

  //: Return an iterator to the beginning of the relation
  std::vector<brdb_tuple_sptr>::iterator begin() { return tuples_.begin(); }

  //: Return an iterator to the end of the relation
  std::vector<brdb_tuple_sptr>::iterator end() { return tuples_.end(); }

  //: binary io read
  void b_read(vsl_b_istream &is);

  //: binary io write
  void b_write(vsl_b_ostream &os) const;

  //: check whether the time stamp is the most updated, if not, return false, otherwise, return true
  bool check_timestamp(const unsigned& time_stamp_check) const;

  //: get time stamp;
  unsigned long get_timestamp() const {return this->time_stamp_; }

  //: check whether another relation is compatible with this relation
  bool is_compatible(const brdb_relation_sptr& other) const;

  //: if compatible, add tuples from the other relation into this one
  bool merge(const brdb_relation_sptr& other);

 private:
  //: Verify that the data stored in this class make a valid relation
  // \note called by the constructors
  bool is_valid() const;

  //: Verify that a tuple is valid relative to this relation
  bool is_valid(const brdb_tuple_sptr& tuple) const;

  //: update the timestamp of this relation
  void update_timestamp();

 private:
  //: The time stamp of this relation
  unsigned long time_stamp_;
  //: The names of the attributes
  std::vector<std::string> names_;
  //: The types of the attributes
  std::vector<std::string> types_;
  //: The tuples of the attributes
  std::vector<brdb_tuple_sptr> tuples_;
};


//: SQL join of two generic relations
brdb_relation_sptr brdb_join(const brdb_relation_sptr& r1, const brdb_relation_sptr& r2);


#endif // brdb_relation_h_
