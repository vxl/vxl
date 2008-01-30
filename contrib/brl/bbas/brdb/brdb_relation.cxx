// This is brl/bbas/brdb/brdb_relation.cxx
//:
// \file
//
// updated by Yong Zhao
// Apr 4th, 2007
// Make it work with the whole database initially based on Matt's sketch.

#include <brdb/brdb_relation.h>

#include <vcl_set.h>
#include <vcl_cassert.h>
#include <vcl_algorithm.h>
#include <vsl/vsl_vector_io.h>
#include <brdb/brdb_value.h>
#include <brdb/brdb_tuple.h>


//======================= Constructors / Destructors ========================


//: Default Constructor (0-tuple)
brdb_relation::brdb_relation()
{
  names_.clear();
  types_.clear();
  tuples_.clear();
  // init the time stamp;
  this->time_stamp_ = 0;
}

//: Constructor - create an empty relation but define the columns
brdb_relation::brdb_relation( const vcl_vector<vcl_string>& names,
                              const vcl_vector<vcl_string>& types )
 : names_(names), types_(types)
{
  assert(this->is_valid());
  // init the time stamp;
  this->time_stamp_ = 0;
}

//: Constructor - create a relation populated with tuples
brdb_relation::brdb_relation( const vcl_vector<vcl_string>& names,
                              const vcl_vector<brdb_tuple_sptr>& tuples,
                              const vcl_vector<vcl_string>& types )
 : names_(names), types_(types), tuples_(tuples)
{
  // if no types are specified infer them from the data
  if(types_.empty())
  {
    types_.resize(names.size(),"");
    for(unsigned int i=0; i<types_.size(); ++i)
    {
      for(unsigned int j=0; j<tuples_.size(); ++j)
      {
        if(!tuples_[j]->is_null(i))
        {
          types_[i] = (*tuples_[j])[i].is_a();
          break;
        }
      }
    }
  }
  assert(this->is_valid());
  // init the time stamp;
  this->time_stamp_ = 0;
}

//: Destructor
brdb_relation::~brdb_relation()
{
}


//: Verify that the data stored in this class make a valid relation
bool
brdb_relation::is_valid() const
{
  if(names_.size() != types_.size())
    return false;
  // check that all names are unique
  if(vcl_set<vcl_string>(names_.begin(), names_.end()).size() != names_.size())
    return false;

  // check for valid type names
  for(unsigned int i=0; i<types_.size(); ++i)
    if(brdb_value::registry().count(types_[i]) <= 0)
      return false;

  // check that each tuple matches the arity and types of the relation
  for(unsigned int i=0; i<tuples_.size(); ++i){
    if(!is_valid(tuples_[i]))
      return false;
  }

  // All is good!
  return true;
}


//: Verify that a tuple is valid relative to this relation
bool
brdb_relation::is_valid(const brdb_tuple_sptr& tuple) const
{
  // tuple arity must match that of this relation
  if(tuple->arity() != types_.size())
    return false;
  // the tuple's types must match those of this relation
  for(unsigned int i=0; i<types_.size(); ++i)
    if(types_[i] != (*tuple)[i].is_a())
      return false;
  return true;
}


//: Set a value by name
bool
brdb_relation::set_value(vcl_vector<brdb_tuple_sptr>::iterator pos, const vcl_string& name, const brdb_value& value)
{
  update_timestamp();

  return (*pos)->set_value(index(name), value);
}

//: Convenience function for setting a value by name
template<class T>
bool 
brdb_relation::set( vcl_vector<brdb_tuple_sptr>::iterator pos, const vcl_string& name , const T& value )
{
  return set_value(pos, name, brdb_value_t<T>(value) );
}


//: Get a value by name
bool
brdb_relation::get_value(vcl_vector<brdb_tuple_sptr>::iterator pos, const vcl_string& name, brdb_value& value) const
{
  return (*pos)->get_value(index(name), value);
}


//: Convenience function for getting a value by name
template<class T>
bool 
brdb_relation::get(vcl_vector<brdb_tuple_sptr>::iterator pos, const vcl_string& name, const T& value) 
{
  return get_value(pos, name, brdb_value_t<T>(value) );
}


//========================= Accessors / Modifiers ===========================


//: Return the name for \p index
// \note returns the empty string if the index is out of range
vcl_string
brdb_relation::name(unsigned int index) const
{
  if(index < names_.size())
    return names_[index];
  return "";
}


//: Return the index for the attribute with \p name
// \note returns the arity (max index + 1) if name is not found
unsigned int
brdb_relation::index(const vcl_string& name) const
{
  vcl_vector<vcl_string>::const_iterator itr = vcl_find(names_.begin(), names_.end(), name);
  return itr - names_.begin();
}

//: Return the type by attribute name
vcl_string 
brdb_relation::type(const vcl_string& name) const
{
  return types_[index(name)];
}

//: Return the type by index
vcl_string 
brdb_relation::type(unsigned int index) const
{
  return types_[index];
}

//: Return true if there is an attribute in the relation with such a name
bool 
brdb_relation::exists(const vcl_string& name) const
{
  if(vcl_find(names_.begin(), names_.end(), name) == names_.end())
    return false;
  else
    return true;
}



//: Sort the tuples by a certain attribute name
bool
brdb_relation::order_by(const vcl_string& name, bool ascending)
{
   update_timestamp();
   return this->order_by(this->index(name), ascending);
}

//: Sort the tuples by a certain attribute index
bool
brdb_relation::order_by(unsigned int index, bool ascending)
{
  update_timestamp();

  if(index < names_.size()){
    if(ascending)
      vcl_sort(tuples_.begin(), tuples_.end(), brdb_tuple_less(index));
    else
      vcl_sort(tuples_.begin(), tuples_.end(), brdb_tuple_greater(index));
    return true;
  }
  return false;
}


//: Add one tuple to relation
bool 
brdb_relation::add_tuple(const brdb_tuple_sptr& new_tuple)
{
  update_timestamp();

  if(is_valid(new_tuple))
  {
    tuples_.push_back(new brdb_tuple(*new_tuple));

    return true;
  }
  else
    return false;
}


//: Add one tuple to relation
bool 
brdb_relation::insert_tuple(const brdb_tuple_sptr& new_tuple, const vcl_vector<brdb_tuple_sptr>::iterator& pos)
{
  update_timestamp();

  if(is_valid(new_tuple))
  {
    brdb_tuple_sptr ins_tuple = new brdb_tuple(*new_tuple);
    tuples_.insert(pos, ins_tuple);

    return true;
  }
  else
    return false;
}


//: remove a tuple at certain position from the relation
bool
brdb_relation::remove_tuple(const vcl_vector<brdb_tuple_sptr>::iterator& pos)
{
  update_timestamp();

  // erase a tuple
  tuples_.erase(pos);

  return true;
}

//: print out the relation
void 
brdb_relation::print() const
{
  // print the attributes name and type
  for(unsigned int i=0; i<arity(); i++)
  {
    vcl_cout << name(i) << "(" << type(i) << ")   "; 
  }
  vcl_cout << vcl_endl;

  for(unsigned int i=0; i<size(); i++)
  {
    tuples_[i]->print();
  }
}


//: binary io read
void
brdb_relation::b_read(vsl_b_istream &is)
{
  update_timestamp();

  // clear the relation including tuples, names and types.
  this->clear();
  this->names_.clear();
  this->types_.clear();

  // first read the version
  unsigned int ver;
  vsl_b_read(is, ver);

  switch(ver)
  {
  case 1:
    {

      // then write the names
      vsl_b_read(is, this->names_);

      // then write the types
      vsl_b_read(is, this->types_);

      // then write the size of the relation
      unsigned int relation_size;
      vsl_b_read(is, relation_size);

      brdb_tuple_sptr proto_tuple = brdb_tuple::make_prototype(this->types_);

      // then write all the tuples
      for(unsigned int i=0; i<relation_size; i++)
      {
        brdb_tuple_sptr new_tuple = new brdb_tuple(*proto_tuple);
        new_tuple->b_read_values(is);
        this->add_tuple(new_tuple);
      }

    }
    break;

  default:
    break;
  }
}


//: binary io write
void
brdb_relation::b_write(vsl_b_ostream &os) const
{
  // first write the version
  unsigned int ver = 1;
  vsl_b_write(os, ver);

  // then write the names
  vsl_b_write(os, this->names_);

  // then write the types
  vsl_b_write(os, this->types_);

  // then write the size of the relation
  unsigned int relation_size = this->size();
  vsl_b_write(os, relation_size);

  // then write all the tuples
  for(unsigned int i=0; i<relation_size; i++)
  {
    tuples_[i]->b_write_values(os);
  }

}


//: update the timestamp of this relation
void 
brdb_relation::update_timestamp()
{
  this->time_stamp_++;
}

//: check whether the time stamp is the most updated, if not, return false, otherwise, return true;
bool
brdb_relation::check_timestamp(const unsigned& time_stamp_check) const
{
  return (time_stamp_check == this->time_stamp_);
}

//: clear the relation
// \keep the names and types, but remove all the tuples
void
brdb_relation::clear()
{ 
  this->update_timestamp(); 
  return tuples_.clear(); 
}

//: check whether another relation is compatible with this relation;
bool 
brdb_relation::is_compatible(const brdb_relation_sptr& other) const
{
  if(!other)
    return false;

  // check whether they have same number of attributes
  if(this->arity() != other->arity())
  {
#ifndef NDEBUG
    vcl_cerr << "Relations are not compatible because they have different "
             << "arity." << vcl_endl;
#endif
    return false;
  }

  // in the order, check the name of the attributes and types are compatible
  for(unsigned int attribute_itr = 0; attribute_itr < this->arity(); attribute_itr++)
  {
    if(this->name(attribute_itr) != other->name(attribute_itr))
    {
#ifndef NDEBUG
      vcl_cerr << "Relations are not compatible because they have different "
               << "attribute names: " << this->name(attribute_itr) << " and "
               << other->name(attribute_itr) << vcl_endl;
#endif
      return false;
    }

    if(this->type(attribute_itr) != other->type(attribute_itr))
    {
#ifndef NDEBUG
      vcl_cerr << "Relations are not compatible because they have different "
               << "types: "<< this->type(attribute_itr) << " and "
               << other->type(attribute_itr) << vcl_endl;
#endif
      return false;
    }
  }
  return true;
}


//: if compatible, add tuples from the other relation into this one
bool
brdb_relation::merge(const brdb_relation_sptr& other)
{
  if(!other || !this->is_compatible(other))
    return false;

  for(vcl_vector<brdb_tuple_sptr>::const_iterator itr = other->tuples_.begin();
      itr != other->tuples_.end(); ++itr)
  {
    tuples_.push_back(new brdb_tuple(**itr));
  }
  return true;
}


//========================= External Functions ===========================

//: SQL join of two generic relations
brdb_relation_sptr 
brdb_join(const brdb_relation_sptr& r1, const brdb_relation_sptr& r2)
{
    // compose name and type list;
    vcl_vector<vcl_string> names;
    vcl_vector<vcl_string> types;

    // first add all r1 attributes
    for(unsigned int i=0; i<r1->arity(); i++)
    {
      names.push_back(r1->name(i));
      types.push_back(r1->type(i));
    }

    unsigned int common_attribute_count = 0;
    vcl_vector<vcl_string> common_attribute;
    vcl_vector<unsigned int> r1_common_attribute_index;
    vcl_vector<unsigned int> r2_common_attribute_index;
    vcl_vector<unsigned int> r1_non_common_attribute_index;
    vcl_vector<unsigned int> r2_non_common_attribute_index;

    // add the non-common attributes from r2;
    for(unsigned int i=0; i<r2->arity(); i++)
    {
      vcl_string name = r2->name(i);
      vcl_string type = r2->type(i);

      if(r1->exists(name))
      {
        // check whether the type matches
        vcl_string type1 = r1->type(name);
        vcl_string type2 = r2->type(name);
        if(type1 != type2)
        {
          vcl_cerr << "join: trying to join relations which having same name "
                   << "attributes with different types!" << vcl_endl;
          return NULL;
        }

        common_attribute_count++;
        common_attribute.push_back(name);
        r1_common_attribute_index.push_back(r1->index(name));
        r2_common_attribute_index.push_back(i);
      }
      else
      {
        names.push_back(name);
        types.push_back(type);

        r2_non_common_attribute_index.push_back(i);
      }
    }

    // get r1_non_common_attribute_index
    for(unsigned int i=0; i<r1->arity(); i++)
    {
      bool isCommon = false;
      for(unsigned int j=0; j<r1_common_attribute_index.size(); j++)
      {
        if(i == r1_common_attribute_index[j])
          isCommon = true;
      }

      if(!isCommon)
       r1_non_common_attribute_index.push_back(i);
    }

    // check if there is any common attributes
    if(common_attribute_count == 0 || 
      (r1_common_attribute_index.size() != r2_common_attribute_index.size()) || 
      ((r1_non_common_attribute_index.size() + r1_common_attribute_index.size()) != r1->arity()) ||
      ((r2_non_common_attribute_index.size() + r2_common_attribute_index.size()) != r2->arity()))
    {
      vcl_cerr << "join: trying to join relations which don't have any "
               << "common attributes." << vcl_endl;
      return NULL;
    }

    // compose a new relation
    brdb_relation_sptr new_relation = new brdb_relation(names, types);

    // iteratively find the matching tuples from r1 and r2
    unsigned int arity1 = r1->arity();
    unsigned int arity2 = r2->arity();

    // go through all tuples in r1
    for(vcl_vector<brdb_tuple_sptr>::iterator itr_1 = r1->begin(); itr_1<r1->end(); ++itr_1)
    {
      // go through all touples in r2
      for(vcl_vector<brdb_tuple_sptr>::iterator itr_2 = r2->begin(); itr_2<r2->end(); ++itr_2)
      {
        bool isMatched = true;
        for(unsigned int k=0; k<common_attribute_count; k++)
        {
          if( (*(*itr_1))[r1_common_attribute_index[k]] != (*(*itr_2))[r2_common_attribute_index[k]])
          {
            isMatched = false;
            break;
          }
        }

        // find a matched tuple
        if(isMatched == true)
        {
          // create a new empty tuple and add into the resulting relation.
          brdb_tuple_sptr new_tup = new brdb_tuple();

          // add all values of tuple in r1 first
          for(unsigned int m=0; m<arity1; m++)
          {
            if(!new_tup->add_value((*(*itr_1))[m]))
              return NULL;
          }

          // add all non-comming values of tuple in r2;
          for(unsigned int m=0; m<r2_non_common_attribute_index.size(); m++)
          {
            if(!new_tup->add_value((*(*itr_2))[r2_non_common_attribute_index[m]]))
              return NULL;
          }

          // check the size of the new tuple
          if(new_tup->arity() != (arity1+arity2-common_attribute_count))
            return NULL;

          // add the new tuple into the resulting relation
          if(!new_relation->add_tuple(new_tup))
          {
            vcl_cerr << "join: failed to add tuple. " << vcl_endl;
            return NULL;
          }
        }
      }
    }

    // assign the resulting relation to output.
    return new_relation;
}
