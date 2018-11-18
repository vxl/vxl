#include <iostream>
#include "brdb_database.h"
//:
// \file
// \brief A database
// \author Yong Zhao
// \date Tue Mar 23, 2007
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <brdb/brdb_tuple.h>
#include <brdb/brdb_relation.h>
#include <brdb/brdb_selection.h>
#include <brdb/brdb_query.h>

//: Constructor - create an empty database;
brdb_database::brdb_database()
= default;

//: Constructor - create a database populated with relations
brdb_database::brdb_database(std::vector<brdb_relation_sptr> relations, std::vector<std::string> relation_names)
{
  // make sure the size of relations is same with size of names;
  assert(relations.size() == relation_names.size());

  // check that all names are unique
  assert(std::set<std::string>(relation_names.begin(), relation_names.end()).size() == relation_names.size());

  // fill the relations_
  auto relations_itr = relations.begin();
  auto relation_name_itr = relation_names.begin();
  for (; (relations_itr != relations.end()) && (relation_name_itr != relation_names.end()); ++relations_itr, ++relation_name_itr)
  {
    relations_[*relation_name_itr] = *relations_itr;
  }

  // check the final relations size;
  assert(relations_.size() == relation_names.size());
}

//: get the number of relations
int
brdb_database::size() const
{
  return relations_.size(); // because vcl_mat will crash if it is empty
}

//: clear the database
void
brdb_database::clear()
{
  relations_.clear();
}


//: check whether a relation exists
bool
brdb_database::exists(const std::string& name) const
{
  if (empty())
    return false;

  auto itr = relations_.find(name);
  return itr != relations_.end();
}


//: check whether a relation exists with the given attribute
bool
brdb_database::exists(const std::string& relation_name,
                      const std::string& attribute_name) const
{
  if (empty())
    return false;

  auto itr = relations_.find(relation_name);
  if (itr == relations_.end())
    return false;

  return itr->second->exists(attribute_name);
}


//: add a tuple into a relation
bool
brdb_database::add_tuple(const std::string& name, const brdb_tuple_sptr& new_tuple)
{
  auto itr = relations_.find(name);
  if (itr == relations_.end())
  {
    std::cerr << "Database warning: trying to add new tuple to an unknown relation: "
             << name << std::endl;
    return false;
  }

  return itr->second->add_tuple(new_tuple);
}


//: remove one relation
bool
brdb_database::remove_relation(const std::string& name)
{
  auto itr = relations_.find(name);
  if (itr == relations_.end())
  {
    std::cerr << "Database warning: trying to delete a relation that does not exist: "
             << name << std::endl;
    return false;
  }

  relations_.erase(itr);
  return true;
}


//: clear one relation
bool
brdb_database::clear_relation(const std::string& name)
{
  auto itr = relations_.find(name);
  if (itr == relations_.end())
  {
    std::cerr << "Database warning: trying to clear a relation that does not exist: "
             << name << std::endl;
    return false;
  }

  itr->second->clear();
  return true;
}


//: add new relation to database
bool
brdb_database::add_relation(const std::string& name, const brdb_relation_sptr& new_relation)
{
  //std::cout << "Adding relation " << name << '\n';

  auto itr = relations_.find(name);
  if (itr != relations_.end())
  {
    std::cerr << "Database warning: trying to add a new relation " << name
             << " which has a conflicting name with an existing relation\n";
    return false;
  }
  relations_[name] = new_relation;
  return true;
}


//: get a relation by name
brdb_relation_sptr
brdb_database::get_relation(const std::string& name) const
{
  auto itr = relations_.find(name);
  if (itr == relations_.end())
    return nullptr;
  return itr->second;
}


//: print the whole database
void
brdb_database::print() const
{
  std::cout << "\n<<<<<<<<<<<<<<<<<<<<---- Printing database ---->>>>>>>>>>>>>>>>>>>>>>>\n";
  auto itr = relations_.begin();
  for (; itr != relations_.end(); itr++)
  {
    std::cout << "______________________________________________________________________\n"
             << "Relation name: " << (*itr).first << std::endl;
    (*itr).second->print();
  }
  std::cout << "\n<<<<<<<<<<<<<<<<<<<<<<<<<<---- end ---->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n" << std::flush;
}


//: get all the relation names
std::set<std::string>
brdb_database::get_all_relation_names() const
{
  std::set<std::string> names;

  for (const auto & relation : relations_)
  {
    names.insert(relation.first);
  }

  return names;
}


//: SQL join of two relations in the database, returning the result
brdb_relation_sptr
brdb_database::join(const std::string& r1, const std::string& r2) const
{
  auto find_r1 = relations_.find(r1);
  auto find_r2 = relations_.find(r2);

  if (find_r1 == relations_.end() || find_r2 == relations_.end())
  {
    std::cerr << "Database warning: trying to join relation that does not exist in database: "
             << r1 << " or " << r2 << std::endl;
    return nullptr;
  }

  return brdb_join(find_r1->second, find_r2->second);
}


//: SQL join of two relations in the database, adding the result to the database
bool
brdb_database::join(const std::string& r1, const std::string& r2, const std::string& result )
{
  auto itr = relations_.find(result);
  if (itr != relations_.end()){
    return false;
  }

  brdb_relation_sptr new_relation = this->join(r1,r2);
  if (!new_relation)
    return false;

  return this->add_relation(result, new_relation);
}


//: submit a query, get a selection which contains the result of query.
brdb_selection_sptr
brdb_database::select(const std::string& relation_name, brdb_query_aptr q) const
{
  if (!exists(relation_name)){
    std::cerr << "Database warning: trying to select in a nonexisting relation: "
             << relation_name << std::endl;
    return nullptr;
  }

  brdb_relation_sptr relation = this->get_relation(relation_name);

  return new brdb_selection(relation, std::move(q) );
}


//: merge another database into this one
bool
brdb_database::merge(const brdb_database_sptr& other)
{
  // disallow self merging
  if (this == other.ptr())
    return false;

  // simple copying case
  if (this->empty())
  {
    this->relations_ = other->relations_;
    return true;
  }

  typedef std::map<std::string, brdb_relation_sptr> r_map;

  // copy the current relations so that the original relations
  // are only modified if all merging is successful.
  r_map new_relations(this->relations_);

  // for each relation in the other database
  for (auto & relation : other->relations_)
  {
    // look for a relation with the same name in this database
    auto ti = new_relations.find(relation.first);
    // if not found
    if (ti == this->relations_.end())
    {
      new_relations.insert(relation);
      continue;
    }

    // test for compatibility of relations
    if (!ti->second->is_compatible(relation.second))
      return false;

    // copy the old relation and merge tuples
    // if later merging fails, the original tuple is not modified
    brdb_relation_sptr new_relation = new brdb_relation(*ti->second);
    if (!new_relation->merge(relation.second))
      return false;

    new_relations[ti->first] = new_relation;
  }

  // everything has succeeded so make the new relations official
  this->relations_ = new_relations;
  return true;
}


//: binary IO write
void brdb_database::b_write(vsl_b_ostream os)
{
  // first write the version number;
  unsigned int ver = 1;
  vsl_b_write(os, ver);

  // then write the size of the database
  unsigned int database_size = this->size();
  vsl_b_write(os, database_size);

  // the write each relation and it name
  if (database_size != 0)
  {
    for (std::map<std::string, brdb_relation_sptr>::const_iterator itr = relations_.begin();
         itr != relations_.end(); ++itr)
    {
      // first write the relation name
      std::string relation_name((*itr).first);
      vsl_b_write(os, relation_name);

      // then write the relation
      brdb_relation_sptr current_relation = (*itr).second;
      current_relation->b_write(os);
    }
  }
}


//: binary IO read
void brdb_database::b_read(vsl_b_istream is)
{
  // first clear the database;
  this->clear();

  // first read the version number;
  unsigned int ver;
  vsl_b_read(is, ver);

  // then read the size of the database
  unsigned int database_size;
  vsl_b_read(is, database_size);

  // the write each relation and it name
  if (database_size != 0)
  {
    for (unsigned int i=0; i<database_size; i++)
    {
      // first read the relation name
      std::string relation_name;
      vsl_b_read(is, relation_name);

      // then read the relation
      brdb_relation_sptr current_relation = new brdb_relation();
      current_relation->b_read(is);

      // then add this relation into the database
      this->add_relation(relation_name, current_relation);
    }
  }
}
