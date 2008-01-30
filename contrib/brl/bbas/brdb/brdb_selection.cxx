// This is brl/bbas/brdb/brdb_selection.cxx
//:
// \file
//
// Auther by Yong Zhao
// Apr 4th, 2007

#include <brdb/brdb_selection.h>

#include <vcl_set.h>
#include <vcl_cassert.h>
#include <vcl_algorithm.h>
#include <vcl_iterator.h>
#include <brdb/brdb_query.h>
#include <brdb/brdb_relation.h>
#include <brdb/brdb_tuple.h>



//: Constructor
brdb_selection::brdb_selection(const brdb_relation_sptr& relation, brdb_query_aptr query)
  : relation_(relation), query_(query)
{
  this->time_stamp_ = relation->get_timestamp();
  produce(query_, selected_set_);
}

//: Constructor
brdb_selection::brdb_selection(const brdb_selection_sptr& selection, brdb_query_aptr query)
{
  if(selection && selection->relation_ && selection->query_.get()){
    this->relation_ = selection->relation_;
    this->time_stamp_ = relation_->get_timestamp();
    selection->check_and_update();
    this->selected_set_ = selection->selected_set_;
    refine(query, selected_set_);
    this->query_ = brdb_query_aptr(new brdb_query_and(selection->query_->clone(), query));
  }
}


//: Constructor with no query
// only for private use 
brdb_selection::brdb_selection(const brdb_relation_sptr& relation)
  : relation_(relation)
{
  assert(relation);
  this->time_stamp_ = relation_->get_timestamp();
}


//: destructor
brdb_selection::~brdb_selection() 
{
}


//: get the iterator pointing to the beginning of the selecton set
selection_t::const_iterator 
brdb_selection::begin()
{
  // check and make sure that the selection is updated.
  this->check_and_update();
  return selected_set_.begin();
}

//: get the iterator pointing to the ending fo the selection set;
selection_t::const_iterator
brdb_selection::end()
{
  // check and make sure that the selection is updated.
  this->check_and_update();
  return selected_set_.end();
}

//: return true if selection is empty;
bool 
brdb_selection::empty() 
{
  // check and make sure that the selection is updated.
  this->check_and_update();
  return selected_set_.empty();
}

//: update the selected tuple (suppose there is only one tuple been selected)
bool
brdb_selection::update_selected_tuple(const brdb_tuple_sptr& new_tuple)
{
  // check and make sure that the selection is updated.
  this->check_and_update();

  // make sure that the size of this selection is 1;
  if(size()!=1)
  {
    vcl_cout << "DB Selection error: trying to update tuples with zero or more than one new tuples. " << vcl_endl;   
    return false;
  }

  selection_t::iterator itr = selected_set_.begin();

  (*(*(itr))) = new_tuple;
  return true;
}

//: update the selected tuple with one new attribute value (suppose there is only one tuple been selected)
bool
brdb_selection::update_selected_tuple(const vcl_string& attribute_name, const brdb_value& value)
{
  // check and make sure that the selection is updated.
  this->check_and_update();

  // make sure that the size of this selection is 1;
  if(size()!=1)
  {
    vcl_cout << "DB Selection error: trying to update tuples with zero or more than one new tuple. " << vcl_endl;   
    return false;
  }

  selection_t::iterator itr = selected_set_.begin();

  unsigned int index = this->relation_->index(attribute_name);
  return (*(*(itr)))->set_value(index, value);
}


//: get value from selected tupe (suppose there is only one tuple been selected)
bool 
brdb_selection::get_value(const vcl_string& attribute_name, brdb_value& value)
{
  // check and make sure that the selection is updated.
  this->check_and_update();

  // make sure that the size of this selection is 1;
  if(size()!=1)
  {
    vcl_cout << "DB Selection error: trying to update tuples with zero or more than one new tuple. " << vcl_endl;   
    return false;
  }

  selection_t::const_iterator itr = selected_set_.begin();
  
  unsigned int index = this->relation_->index(attribute_name);
  return (*(*(itr)))->get_value(index ,value);
}

//: convenient function to get value from selected tuples with an index (say, the value of ith selected tuples)
bool 
brdb_selection::get_value(const vcl_string& attribute_name, unsigned int index, brdb_value& value)
{
  // check and make sure that the selection is updated.
  this->check_and_update();

  if(index < 0 || index >= size())
  {
    vcl_cout << "DB warning: trying to get value from an invalid index!" << vcl_endl;
    return false;
  }

  selection_t::const_iterator itr = selected_set_.begin();
  while(index > 0){
    --index;
    ++itr;
  }

  unsigned int attribute_index = this->relation_->index(attribute_name);
  return (*(*(itr)))->get_value(attribute_index, value);

  return true;
}


//: get sql view (independent relation) from selection
brdb_relation_sptr 
brdb_selection::get_sqlview()
{
  // check and make sure that the selection is updated.
  this->check_and_update();

  // get the names and types information from selection;
  unsigned int arity = relation_->arity();
  vcl_vector<vcl_string> names;
  vcl_vector<vcl_string> types;
  for(unsigned int i = 0; i<arity; i++)
  {
    names.push_back(relation_->name(i));
    types.push_back(relation_->type(i));
  }

  brdb_relation_sptr sql_view = new brdb_relation(names, types);

  if(size() == 0)
    return sql_view;

  for(selection_t::const_iterator itr = selected_set_.begin();
      itr != selected_set_.end(); ++itr)
  {
    sql_view->add_tuple((*(*itr)));
  }
  return sql_view;

}

//: apply AND operation on two selections, return the resulting selection
brdb_selection_sptr 
brdb_selection::selection_and(const brdb_selection_sptr& s)
{
  if(!s)
    return NULL;

  if(s->relation_ && s->relation_ != this->relation_){
    vcl_cerr << "warning: attempting to combine selections on different relations" << vcl_endl;
    return NULL;
  }

  brdb_selection_sptr result = new brdb_selection(this->relation_);
  result->query_ = brdb_query_aptr(new brdb_query_and(*this->query_, *s->query_));

  // check and make sure that each selection is updated.
  this->check_and_update();
  s->check_and_update();

  vcl_set_intersection(this->selected_set_.begin(), this->selected_set_.end(),
                       s->selected_set_.begin(),    s->selected_set_.end(),
                       vcl_insert_iterator<selection_t>(result->selected_set_,
                                                        result->selected_set_.end()));

  return result;
}

//: apply OR operation on two selections, return the resulting selection
brdb_selection_sptr 
brdb_selection::selection_or(const brdb_selection_sptr& s)
{
  if(!s)
    return NULL;

  if(s->relation_ && s->relation_ != this->relation_){
    vcl_cerr << "warning: attempting to combine selections on different relations" << vcl_endl;
    return NULL;
  }

  brdb_selection_sptr result = new brdb_selection(this->relation_);
  result->query_ = brdb_query_aptr(new brdb_query_or(*this->query_, *s->query_));

  // check and make sure that each selection is updated.
  this->check_and_update();
  s->check_and_update();

  vcl_set_union(this->selected_set_.begin(), this->selected_set_.end(),
                s->selected_set_.begin(),    s->selected_set_.end(),
                vcl_insert_iterator<selection_t>(result->selected_set_,
                                                 result->selected_set_.end()));

  return result;
}

//: apply XOR operation on two selections, return the resulting selection
brdb_selection_sptr 
brdb_selection::selection_xor(const brdb_selection_sptr& s)
{
  if(!s)
    return NULL;

  if(s->relation_ && s->relation_ != this->relation_){
    vcl_cerr << "warning: attempting to combine selections on different relations" << vcl_endl;
    return NULL;
  }

  brdb_selection_sptr result = new brdb_selection(this->relation_);
  result->query_ = brdb_query_aptr(new brdb_query_or(*this->query_, *s->query_));

  // check and make sure that each selection is updated.
  this->check_and_update();
  s->check_and_update();

  vcl_set_symmetric_difference(
                this->selected_set_.begin(), this->selected_set_.end(),
                s->selected_set_.begin(),    s->selected_set_.end(),
                vcl_insert_iterator<selection_t>(result->selected_set_,
                                                 result->selected_set_.end()));

  return result;
}

//: apply NOT operation on selection, return the resulting selection
brdb_selection_sptr 
brdb_selection::selection_not()
{
  brdb_selection_sptr result = new brdb_selection(this->relation_);
  result->query_ = this->query_->complement();

  // check and make sure that each selection is updated.
  this->check_and_update();


  for(vcl_vector<brdb_tuple_sptr>::iterator itr = relation_->begin();
      itr != relation_->end(); ++itr)
  {
    // check the attribute against the query
    if(!this->contains(itr))
    {
      //add the iterator to this tuple to selection
      result->selected_set_.insert(itr);
    }
  }

  return result;
}


//: print selection
void 
brdb_selection::print()
{
    // check and make sure that the selection is updated.
    this->check_and_update();

    vcl_cout << "print selection: " << vcl_endl;

    for(unsigned int i=0; i<this->relation_->arity(); i++)
    {
      vcl_cout << this->relation_->name(i) << "(" << this->relation_->type(i) << ")   ";
    }
    vcl_cout << vcl_endl;

    if(!selected_set_.size())
    {
      vcl_cout << "This selection is empty." << vcl_endl;
    }
    for(selection_t::const_iterator itr = selected_set_.begin();
        itr != selected_set_.end(); ++itr)
    {
      brdb_tuple_sptr tuple = (*(*itr));
      tuple->print();
    }
}


//: delete the selected tuples
void
brdb_selection::delete_tuples()
{
    // check and make sure that the selection is updated.
    this->check_and_update();


    if(empty())
    {
      return;
    }

    // deletion must be done from back to front, because each time an element is deleted, the iterators after it will be updated.
    selection_t::iterator itr = selected_set_.end();
    // notice, everytime anything happens to the original relation, the size() will reflect the new information.
    unsigned int original_selection_size = this->size();  
    for(unsigned int i=0; i<original_selection_size; i++)
    {
      --itr;
      relation_->remove_tuple((*itr));
    }

    // clear the selection;
    selected_set_.clear();
}

//: return the size of selection;
unsigned int 
brdb_selection::size()
{
  // check and make sure that the selection is updated.
  this->check_and_update();
  return selected_set_.size();
}



//: see whether a tuple exists in this selection;
bool 
brdb_selection::tuple_exist(const brdb_tuple_sptr& tuple)
{
  // check and make sure that the selection is updated.
  this->check_and_update();

  if(this->empty())
    return false;

  for(selection_t::const_iterator itr = this->begin(); itr != this->end(); ++ itr)
  {
    if((*(*itr)) == tuple)
      return true;
  }

  return false;
}


//: see whether a tuple exists in this selection;
bool
brdb_selection::contains(const vcl_vector<brdb_tuple_sptr>::iterator& relation_itr) const
{
  // assume already updated
  selection_t::const_iterator sitr = this->selected_set_.find(relation_itr);
  return sitr == this->selected_set_.end();
}


//: check timestamp of selection and update selection if needed;
void
brdb_selection::check_and_update()
{
  if(this->relation_ == NULL)
    return;

  if(this->time_stamp_ == this->relation_->get_timestamp())
    return;

  this->selected_set_.clear();

  produce(query_, selected_set_);
  this->time_stamp_ = this->relation_->get_timestamp();
}


//: apply a query to produce a selected set
bool
brdb_selection::produce(const brdb_query_aptr& q, selection_t& s)
{
  if(const brdb_query_and* qa = dynamic_cast<const brdb_query_and*>(q.get()))
  {
    produce(qa->first(), s);
    refine(qa->second(), s);
  }
  else if(const brdb_query_or* qo = dynamic_cast<const brdb_query_or*>(q.get()))
  {
    selection_t s1,s2;
    produce(qo->first(), s1);
    produce(qo->second(), s2);
    vcl_set_union(s1.begin(), s1.end(), s2.begin(), s2.end(),
                  vcl_insert_iterator<selection_t>(s, s.end()));
  }
  else if(const brdb_query_comp* qc = dynamic_cast<const brdb_query_comp*>(q.get()))
  {
    unsigned int attr_index = relation_->index(qc->attribute_name());
    // go through all the tuples
    for(vcl_vector<brdb_tuple_sptr>::iterator itr = relation_->begin();
        itr != relation_->end(); ++itr)
    {
      // check the attribute against the query
      if(qc->pass((*(*itr))[attr_index]))
      {
        //add the iterator to this tuple to selection
        s.insert(itr);
      }
    }
  }
  else
    return false;

  return true;
}


//: apply a refine to produce a selected set
bool
brdb_selection::refine(const brdb_query_aptr& q, selection_t& s)
{
  if(const brdb_query_and* qa = dynamic_cast<const brdb_query_and*>(q.get()))
  {
    refine(qa->first(), s);
    refine(qa->second(), s);
  }
  else if(const brdb_query_or* qo = dynamic_cast<const brdb_query_or*>(q.get()))
  {
    selection_t s1(s), s2(s);
    refine(qo->first(), s1);
    refine(qo->second(), s2);
    s.clear();
    vcl_set_union(s1.begin(), s1.end(), s2.begin(), s2.end(),
                  vcl_insert_iterator<selection_t>(s, s.end()));
  }
  else if(const brdb_query_comp* qc = dynamic_cast<const brdb_query_comp*>(q.get()))
  {
    unsigned int attr_index = relation_->index(qc->attribute_name());
    selection_t s_new;
    // go through all the tuples pointed by the selection
    for(selection_t::const_iterator itr = s.begin(); itr != s.end(); ++itr)
    {
      // get the attribute value
      if(qc->pass((*(*(*itr)))[attr_index]))
      {
        //add the iterator to this tuple to selection;
        s_new.insert(*itr);
      }
    }
    s.swap(s_new);
  }
  else
    return false;

  return true;
}

