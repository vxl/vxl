// This is core/vcsl/vcsl_spatial.cxx
#include "vcsl_spatial.h"
#include <vcl_cassert.h>
#include <vcsl/vcsl_spatial_transformation.h>
#include <vcsl/vcsl_graph.h>

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vcsl_spatial::~vcsl_spatial()
{
  if (graph_)
    graph_->remove(this);
}

//---------------------------------------------------------------------------
// Is `time' between the two time bounds ?
//---------------------------------------------------------------------------
bool vcsl_spatial::valid_time(double time) const
{
  if (beat_.size()==0)
    return true;
  else
    return (beat_[0]<=time)&&(time<=beat_[beat_.size()-1]);
}

//---------------------------------------------------------------------------
// Set the list of parent coordinate system along the time
//---------------------------------------------------------------------------
void vcsl_spatial::set_parent(vcl_vector<vcsl_spatial_sptr> const& new_parent)
{
  vcl_vector<vcsl_spatial_sptr>::iterator i, j;

  if (parent_!=new_parent)
  {
    // Erase 'this' from the list of the old parents' potential children
    for (i=parent_.begin();i!=parent_.end();++i)
    {
      vcl_vector<vcsl_spatial_sptr> children=(*i)->potential_children_;
      for (j=children.begin(); j!=children.end()&&(*j).ptr()!=this; ++j)
        ;
      if ((*j).ptr()==this) children.erase(j);
    }
    parent_=new_parent;

    // Add 'this' to the list of the new parents' potential children
    for (i=parent_.begin();i!=parent_.end();++i)
      if (*i)
        (*i)->potential_children_.push_back(this);
  }
}

//---------------------------------------------------------------------------
// Set the unique parent and the unique motion
//---------------------------------------------------------------------------
void
vcsl_spatial::set_unique(const vcsl_spatial_sptr &new_parent,
                         const vcsl_spatial_transformation_sptr &new_motion)
{
  motion_.clear(); motion_.push_back(new_motion);
  vcl_vector<vcsl_spatial_sptr> temp_parent; temp_parent.push_back(new_parent);
  set_parent(temp_parent);
  beat_.clear();
}

//---------------------------------------------------------------------------
// Return the index of the beat inferior or equal to `time'
// REQUIRE: parent().size()!=0
// REQUIRE: valid_time(time)
//---------------------------------------------------------------------------
int vcsl_spatial::matching_interval(double time) const
{
  // require
  assert(parent_.size()!=0);
  assert(valid_time(time));

  // Dichotomic research of the index
  int inf=0;
  int sup=beat_.size()-1;
  while (sup-inf>1)
  {
    int mid=(inf+sup)/2;
    if (beat_[mid]>time)
      sup=mid;
    else
      inf=mid;
  }
  return inf;
}

//---------------------------------------------------------------------------
// Does a path from `this' to `other' exist ?
//---------------------------------------------------------------------------
bool vcsl_spatial::path_from_local_to_cs_exists(const vcsl_spatial_sptr &other,
                                                double time)
{
  graph_->init_vertices();

  return recursive_path_from_local_to_cs_exists(other,time);
}

//---------------------------------------------------------------------------
// Does a path from `this' to `other' exist ? Called only by
// path_to_cs_exists()
//---------------------------------------------------------------------------
bool vcsl_spatial::recursive_path_from_local_to_cs_exists(const vcsl_spatial_sptr &other,
                                                          double time)
{
  bool result;
  int i = -1; // dummy initialisation to avoid compiler warning
  vcl_vector<vcsl_spatial_sptr>::const_iterator child;
  if (parent_.size()!=0) // If 'this' is not absolute
    i=matching_interval(time);
  set_reached(true);

  // Check if parent of 'this' (if it exists) is 'other'
  result=!is_absolute(time); // true if 'this' has a parent
  if (result)
    result=parent_[i]==other; // true if parent is 'other' (the cs sought)

  // If 'this' has no parent or its parent is not 'other':
  if (!result)
  {
    // Check if 'other' can be reached through parent
    if (!is_absolute(time)) // if 'this' has a parent
      // If parent has not already been checked
      if (!parent_[i]->reached())
        // Check if 'other' can be reached from it
        result=parent_[i]->recursive_path_from_local_to_cs_exists(other, time);
    // If 'other' not found, check if 'other' can be reached through children of 'this'
    if (!result)
    {
      if (potential_children_.size()!=0)
      {
        for (child=potential_children_.begin();
             !result && child!=potential_children_.end();
             ++child)
        {
          result=!(*child)->reached();
          if (result)
          {
            int j=(*child)->matching_interval(time);
            result=(*child)->parent_[j].ptr()==this;
            if (result)
              result=(*child)->motion_[j]->is_invertible(time);
          }
          if (result)
          {
            result=(*child)==other;
            if (!result)
              result=(*child)->recursive_path_from_local_to_cs_exists(other, time);
          }
        }
      }
    }
  }
  return result;
}

//---------------------------------------------------------------------------
// Find the sequence of transformations from `this' to `other'
// REQUIRE: path.size()==0 and sens.size()==0
// REQUIRE: path_from_local_to_cs_exists(other,time)
//---------------------------------------------------------------------------
void vcsl_spatial::path_from_local_to_cs(const vcsl_spatial_sptr &other,
                                         double time,
                                         vcl_vector<vcsl_spatial_transformation_sptr> &path,
                                         VCSL_SPATIAL_VECTOR_BOOL &sens)
{
  // require
  assert(path.size()==0);
  assert(sens.size()==0);
  assert(path_from_local_to_cs_exists(other,time));

  //unused bool path_exists;

  graph_->init_vertices();
  /*path_exists=*/ recursive_path_from_local_to_cs(other,time,path,sens);
}

//---------------------------------------------------------------------------
// Find the sequence of transformations from `this' to `other'
// Called only by path_from_local_to_cs()
//---------------------------------------------------------------------------
bool
vcsl_spatial::recursive_path_from_local_to_cs(const vcsl_spatial_sptr &other,
                                              double time,
                                              vcl_vector<vcsl_spatial_transformation_sptr> &path,
                                              VCSL_SPATIAL_VECTOR_BOOL &sens)
{
  bool result;
  int i = -1; // dummy initialisation to avoid compiler warning
  vcl_vector<vcsl_spatial_sptr>::const_iterator child;

  if (parent_.size()!=0)
    i=matching_interval(time);

  set_reached(true);

  result=!is_absolute(time);
  if (result)
    result=parent_[i]==other;

  if (result)
  {
    path.push_back(motion_[i]);
    sens.push_back(false);
  }

  if (!result)
  {
    if (!is_absolute(time))
      if (!parent_[i]->reached())
      {
        path.push_back(motion_[i]);
        sens.push_back(false);
        result=parent_[i]->recursive_path_from_local_to_cs(other,time,path,sens);
        if (!result)
        {
          path.pop_back();
          sens.pop_back();
        }
      }
    if (!result)
    {
      if (potential_children_.size()!=0)
      {
        for (child=potential_children_.begin();
             !result && child!=potential_children_.end();
             ++child)
        {
          result=!(*child)->reached();
          if (result)
          {
            int j=(*child)->matching_interval(time);
            result=(*child)->parent_[j].ptr()==this;
            if (result)
              result=(*child)->motion_[j]->is_invertible(time);
            if (result)
            {
              result=(*child)==other;
              path.push_back((*child)->motion_[j]);
              sens.push_back(true);
              if (!result)
              {
                result=(*child)->recursive_path_from_local_to_cs(other,time,
                                                                 path,sens);
                if (!result)
                {
                  path.pop_back();
                  sens.pop_back();
                }
              }
            }
          }
        }
      }
    }
  }

  return result;
}

//---------------------------------------------------------------------------
// Is `this' an absolute spatial coordinate system at time `time'?
// REQUIRE: valid_time(time)
//---------------------------------------------------------------------------
bool vcsl_spatial::is_absolute(double time) const
{
  // require
  assert(valid_time(time));

  // If list of parents is NULL, 'this' must be absolute
  if (parent_.size()==0)
    return true;
  else
  {
    // If parent at given interval is NULL, 'this' must be absolute
    int i=matching_interval(time);
    return !parent_[i];
  }
}

//---------------------------------------------------------------------------
// From a vector `v' expressed in `this',
// return a vector expressed in the spatial coordinate system `other'
// REQUIRE: path_from_local_to_cs_exists(other,time)
//---------------------------------------------------------------------------
vnl_vector<double>
vcsl_spatial::from_local_to_cs(const vnl_vector<double> &v,
                               const vcsl_spatial_sptr &other,
                               double time)
{
  // require
  assert(path_from_local_to_cs_exists(other,time));

  vcl_vector<vcsl_spatial_transformation_sptr> path;
  VCSL_SPATIAL_VECTOR_BOOL sens;

  vcl_vector<vcsl_spatial_transformation_sptr>::const_iterator i;
  VCSL_SPATIAL_VECTOR_BOOL::const_iterator j;

  path_from_local_to_cs(other,time,path,sens);

  vnl_vector<double> tmp=from_cs_to_standard_units(v);

  j=sens.begin();

  for (i=path.begin();i!=path.end();++i,++j)
  {
    if (*j)
      tmp=(*i)->inverse(tmp,time);
    else
      tmp=(*i)->execute(tmp,time);
  }
  return other->from_standard_units_to_cs(tmp);
}

void vcsl_spatial::set_graph(const vcsl_graph_sptr &new_graph)
{
  if (graph_)
    graph_->remove(this);
  graph_=new_graph;
  graph_->put(this);
}
