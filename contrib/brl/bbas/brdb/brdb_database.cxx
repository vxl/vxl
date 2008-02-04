//:
// \file
// \brief A database
// \author Yong Zhao
// \date Tue Mar 23, 2007
//
// \verbatim
//  Modifications
// \endverbatim



#include <brdb/brdb_database.h>
#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <brdb/brdb_tuple.h>
#include <brdb/brdb_relation.h>
#include <brdb/brdb_selection.h>
#include <brdb/brdb_query.h>

//: Constructor - create an empty database;
brdb_database::brdb_database() 
{ 
}

//: Constructor - create a database populated with relations
brdb_database::brdb_database(vcl_vector<brdb_relation_sptr> relations, vcl_vector<vcl_string> relation_names) 
{
  // make sure the size of relations is same with size of names;
  assert(relations.size() == relation_names.size());

  // check that all names are unique
  assert(vcl_set<vcl_string>(relation_names.begin(), relation_names.end()).size() == relation_names.size());

  // fill the relations_
  vcl_vector<brdb_relation_sptr>::iterator relations_itr = relations.begin();
  vcl_vector<vcl_string>::iterator relation_name_itr = relation_names.begin();
  for(; (relations_itr != relations.end()) && (relation_name_itr != relation_names.end()); ++relations_itr, ++relation_name_itr)
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
brdb_database::exists(const vcl_string& name) const
{
  if(empty())
    return false;

  vcl_map<vcl_string, brdb_relation_sptr>::const_iterator itr = relations_.find(name);
  return (itr != relations_.end());
}


//: check whether a relation exists with the given attribute
bool
brdb_database::exists(const vcl_string& relation_name,
                      const vcl_string& attribute_name) const
{
  if(empty())
    return false;

  vcl_map<vcl_string, brdb_relation_sptr>::const_iterator itr = relations_.find(relation_name);
  if (itr == relations_.end())
    return false;

  return itr->second->exists(attribute_name);
}


//: add a tuple into a relation
bool 
brdb_database::add_tuple(const vcl_string& name, const brdb_tuple_sptr& new_tuple)
{
  vcl_map<vcl_string, brdb_relation_sptr>::iterator itr = relations_.find(name);
  if(itr == relations_.end())
  {
    vcl_cerr << "Database warning: trying to add new tuple to an unknown relation: "
             << name << vcl_endl;
    return false;
  }

  return itr->second->add_tuple(new_tuple);
}


//: remove one relation
bool
brdb_database::remove_relation(const vcl_string& name)
{
  vcl_map<vcl_string, brdb_relation_sptr>::iterator itr = relations_.find(name);
  if(itr == relations_.end())
  {
    vcl_cerr << "Database warning: trying to delete a relation that does not exist: "
             << name << vcl_endl;
    return false;
  }

  relations_.erase(itr);
  return true;
}


//: clear one relation
bool
brdb_database::clear_relation(const vcl_string& name)
{
  vcl_map<vcl_string, brdb_relation_sptr>::iterator itr = relations_.find(name);
  if(itr == relations_.end())
  {
    vcl_cerr << "Database warning: trying to clear a relation that does not exist: "
             << name << vcl_endl;
    return false;
  }

  itr->second->clear();
  return true;
}


//: add new relation to database
bool 
brdb_database::add_relation(const vcl_string& name, const brdb_relation_sptr& new_relation)
{
  vcl_cout << "Adding relation " << name << '\n';

  vcl_map<vcl_string, brdb_relation_sptr>::iterator itr = relations_.find(name);
  if(itr != relations_.end())
  {
    vcl_cerr << "Database warning: trying to add a new relation "
             << "which has an conflicting name with an existinig relation. "
             << name << vcl_endl;
    return false;
  }
  relations_[name] = new_relation;
  return true;
}


//: get a relation by name
brdb_relation_sptr 
brdb_database::get_relation(const vcl_string& name) const
{
  vcl_map<vcl_string, brdb_relation_sptr>::const_iterator itr = relations_.find(name);
  if(itr == relations_.end())
    return NULL;
  return itr->second;
}


//: print the whole database
void 
brdb_database::print() const
{
  vcl_cout << "\n<<<<<<<<<<<<<<<<<<<<---- Printing database ---->>>>>>>>>>>>>>>>>>>>>>>\n";
  vcl_map<vcl_string, brdb_relation_sptr>::const_iterator itr = relations_.begin();
  for(; itr != relations_.end(); itr++)
  {
    vcl_cout << "______________________________________________________________________\n";
    vcl_cout << "Relation name: " << (*itr).first << vcl_endl;
    (*itr).second->print();
  }
  vcl_cout << "\n<<<<<<<<<<<<<<<<<<<<<<<<<<---- end ---->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n";
}


//: get all the relation names
vcl_set<vcl_string> 
brdb_database::get_all_relation_names() const
{
  vcl_set<vcl_string> names;

  for(vcl_map<vcl_string, brdb_relation_sptr>::const_iterator itr = relations_.begin(); itr != relations_.end(); ++itr)
  {
    names.insert((*itr).first);
  }

  return names;
}


//: SQL join of two relations in the database, returning the result
brdb_relation_sptr
brdb_database::join(const vcl_string& r1, const vcl_string& r2) const
{
  vcl_map<vcl_string, brdb_relation_sptr>::const_iterator find_r1 = relations_.find(r1);
  vcl_map<vcl_string, brdb_relation_sptr>::const_iterator find_r2 = relations_.find(r2);

  if(find_r1 == relations_.end() || find_r2 == relations_.end())
  {
    vcl_cerr << "Database warning: trying to join relation that does not exist in database:" << r1 << " or " << r2 << vcl_endl;
    return NULL;
  }

  return brdb_join(find_r1->second, find_r2->second);
}


//: SQL join of two relations in the database, adding the result to the database
bool
brdb_database::join(const vcl_string& r1, const vcl_string& r2, const vcl_string& result )
{
  vcl_map<vcl_string, brdb_relation_sptr>::iterator itr = relations_.find(result);
  if(itr != relations_.end()){
    return false;
  }

  brdb_relation_sptr new_relation = this->join(r1,r2);
  if(!new_relation)
    return false;

  return this->add_relation(result, new_relation);
}


//: submit a query, get a selection which contains the result of query.
brdb_selection_sptr
brdb_database::select(const vcl_string& relation_name, brdb_query_aptr q) const
{
  if(!exists(relation_name)){
    vcl_cerr << "Database warning: trying to select in an unexisting "
             << "relation: " << relation_name << vcl_endl;
    return NULL;
  }

  brdb_relation_sptr relation = this->get_relation(relation_name);

  return new brdb_selection(relation, q);
}


//: merge another database into this one
bool
brdb_database::merge(const brdb_database_sptr& other)
{
  // disallow self merging
  if (this == other.ptr())
    return false;

  // simple copying case
  if(this->empty())
  {
    this->relations_ = other->relations_;
    return true;
  }

  typedef vcl_map<vcl_string, brdb_relation_sptr> r_map;

  // copy the current relations so that the original relations
  // are only modified if all merging is successfull.
  r_map new_relations(this->relations_);

  // for each relation in the other database
  for(r_map::iterator oi = other->relations_.begin();
      oi!=other->relations_.end(); ++oi)
  {
    // look for a relation with the same name in this database
    r_map::iterator ti = new_relations.find(oi->first);
    // if not found
    if(ti == this->relations_.end())
    {
      new_relations.insert(*oi);
      continue;
    }

    // test for compatiblity of relations
    if(!ti->second->is_compatible(oi->second))
      return false;

    // copy the old relation and merge tuples
    // if later merging fails, the original tuple is not modified
    brdb_relation_sptr new_relation = new brdb_relation(*ti->second);
    if(!new_relation->merge(oi->second))
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
  if(database_size != 0)
  {
    for(vcl_map<vcl_string, brdb_relation_sptr>::const_iterator itr = relations_.begin();
        itr != relations_.end(); ++itr)
    {
      // first write the relation name
      vcl_string relation_name((*itr).first);
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
  if(database_size != 0)
  {
    for(unsigned int i=0; i<database_size; i++)
    {
      // first read the relation name
      vcl_string relation_name;
      vsl_b_read(is, relation_name);

      // then read the relation
      brdb_relation_sptr current_relation = new brdb_relation();
      current_relation->b_read(is);

      // then add this relation into the database
      this->add_relation(relation_name, current_relation);
    }
  }
}

