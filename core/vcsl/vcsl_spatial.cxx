#include <vcsl/vcsl_spatial.h>

#include <vcl_cassert.h>

#include <vcsl/vcsl_spatial_transformation.h>
#include <vcsl/vcsl_graph.h>

//***************************************************************************
// Constructors/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
// Default constructor
//---------------------------------------------------------------------------
vcsl_spatial::vcsl_spatial(void)
{
  parent_=0;
  _potential_children=new vcl_vector<vcsl_spatial_ref>;
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vcsl_spatial::~vcsl_spatial()
{
  if(graph_.ptr()!=0)
    graph_->remove(this);
  delete _potential_children;
}

//***************************************************************************
// Because VXL does not use dynamic_cast<> :-(
//***************************************************************************

const vcsl_spatial *vcsl_spatial::cast_to_spatial(void) const
{
  return this;
}

const vcsl_cartesian_2d *vcsl_spatial::cast_to_cartesian_2d(void) const
{
  return 0;
}

const vcsl_polar *vcsl_spatial::cast_to_polar(void) const
{
  return 0;
}

const vcsl_cartesian_3d *vcsl_spatial::cast_to_cartesian_3d(void) const
{
  return 0;
}

const vcsl_cylindrical *vcsl_spatial::cast_to_cylindrical(void) const
{
  return 0;
}

const vcsl_spherical *vcsl_spatial::cast_to_spherical(void) const
{
  return 0;
}

//***************************************************************************
// Status report
//***************************************************************************
//---------------------------------------------------------------------------
// Return the list of time clocks
//---------------------------------------------------------------------------
vcl_vector<double> *vcsl_spatial::beat(void) const
{
  return beat_;
}

//---------------------------------------------------------------------------
// Return the list of parent coordinate system along the time
//---------------------------------------------------------------------------
vcl_vector<vcsl_spatial_ref> *vcsl_spatial::parent(void) const
{
  return parent_;
}

//---------------------------------------------------------------------------
// Return the list of transformations along the time
//---------------------------------------------------------------------------
vcl_vector<vcsl_spatial_transformation_ref> *vcsl_spatial::motion(void) const
{
  return motion_;
}

//---------------------------------------------------------------------------
// Is `time' between the two time bounds ?
//---------------------------------------------------------------------------
bool vcsl_spatial::valid_time(const double time) const
{
  bool result;

  result=beat_==0;
  if(!result)
    result=((*beat_)[0]<=time)&&(time<=(*beat_)[beat_->size()-1]);

  return result; 
}

//***************************************************************************
// Status setting
//***************************************************************************

//---------------------------------------------------------------------------
// Return the list of time clocks
//---------------------------------------------------------------------------
void vcsl_spatial::set_beat(vcl_vector<double> &new_beat)
{
  beat_=&new_beat;
}

//---------------------------------------------------------------------------
// Set the list of parent coordinate system along the time
//---------------------------------------------------------------------------
void vcsl_spatial::set_parent(vcl_vector<vcsl_spatial_ref> &new_parent)
{
  vcl_vector<vcsl_spatial_ref>::iterator i;
  vcl_vector<vcsl_spatial_ref> *children;
  vcl_vector<vcsl_spatial_ref>::iterator j;

  if(parent_!=&new_parent)
    {
      if(parent_!=0)
        {
          for(i=parent_->begin();i!=parent_->end();++i)
            {
              children=(*i)->_potential_children;
              for(j=children->begin();j!=children->end()&&(*j).ptr()!=this;++j)
                ;
              children->erase(j);
            }
        }
      parent_=&new_parent;
      for(i=parent_->begin();i!=parent_->end();++i)
        {
          if((*i).ptr()!=0)
            {
              children=(*i)->_potential_children;
              children->push_back(this);
            }
        }
    }
}

//---------------------------------------------------------------------------
// Set the list of transformations along the time
//---------------------------------------------------------------------------
void vcsl_spatial::set_motion(vcl_vector<vcsl_spatial_transformation_ref> &new_motion)
{
  motion_=&new_motion;
}

//---------------------------------------------------------------------------
// Set the unique parent and the unique motion
//---------------------------------------------------------------------------
void
vcsl_spatial::set_unique(const vcsl_spatial_ref &new_parent,
                         const vcsl_spatial_transformation_ref &new_motion)
{
  motion_=new vcl_vector<vcsl_spatial_transformation_ref>(1);
  (*motion_)[0]=new_motion;
  parent_=new vcl_vector<vcsl_spatial_ref>(1);
  (*parent_)[0]=new_parent;
  beat_=0;
}

//***************************************************************************
// Basic operations
//***************************************************************************

//---------------------------------------------------------------------------
// Return the index of the beat inferior or equal to `time'
// REQUIRE: parent()!=0
// REQUIRE: valid_time(time)
//---------------------------------------------------------------------------
int vcsl_spatial::matching_interval(const double time) const
{
  // require
  assert(parent()!=0);
  assert(valid_time(time));

  int result;
  int inf;
  int sup;
  int mid;

  // Dichotomic research of the index

  if(beat_==0) // unique
    result=0;
  else
    {
      inf=0;
      sup=beat_->size()-1;
      while((sup-inf)!=1)
        {
          mid=(inf+sup)/2;
          if((*beat_)[mid]>time)
            sup=mid;
          else
            inf=mid;
        }
      result=inf;
    }
  return result;
}

//---------------------------------------------------------------------------
// Does a path from `this' to `other' exist ?
//---------------------------------------------------------------------------
bool vcsl_spatial::path_from_local_to_cs_exists(const vcsl_spatial_ref &other,
                                                const double time)
{
  bool result;

  graph_->init_vertices();

  result=recursive_path_from_local_to_cs_exists(other,time);

  return result;
}

//---------------------------------------------------------------------------
// Does a path from `this' to `other' exist ? Called only by
// path_to_cs_exists()
//---------------------------------------------------------------------------
bool vcsl_spatial::recursive_path_from_local_to_cs_exists(const vcsl_spatial_ref &other,
                                                          const double time)
{
  bool result;
  int i;
  int j;
  vcl_vector<vcsl_spatial_ref>::const_iterator child;
  if(parent_!=0)
    i=matching_interval(time);
  set_reached(true);

  result=!is_absolute(time);
  if(result)
    result=(*parent_)[i]==other;
  if(!result)
    {
      if(!is_absolute(time))
        if(!(*parent_)[i]->reached())
          result=(*parent_)[i]->recursive_path_from_local_to_cs_exists(other,
                                                                       time);
      if(!result)
        {
          if(_potential_children!=0)
            {
              for(child=_potential_children->begin();
                  child!=_potential_children->end()&&!result;
                  ++child)
                {
                  result=!(*child)->reached();
                  if(result)
                    {
                      j=(*child)->matching_interval(time);
                      result=(*(*child)->parent_)[j].ptr()==this;
                    }
                  if(result)
                    result=(*(*child)->motion_)[j]->is_invertible(time);
                  if(result)
                    {
                      result=(*child)==other;
                      if(!result)
                        result=(*child)->recursive_path_from_local_to_cs_exists(other,
                                                                                time);
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
void vcsl_spatial::path_from_local_to_cs(const vcsl_spatial_ref &other,
                                         const double time,
                                         vcl_vector<vcsl_spatial_transformation_ref> &path,
                                         vcl_vector<bool> &sens)
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
vcsl_spatial::recursive_path_from_local_to_cs(const vcsl_spatial_ref &other,
                                              const double time,
                                              vcl_vector<vcsl_spatial_transformation_ref> &path,
                                              vcl_vector<bool> &sens)
{
  bool result;
  int i;
  int j;
  vcl_vector<vcsl_spatial_ref>::const_iterator child;

  if(parent_!=0)
    i=matching_interval(time);

  set_reached(true);

  result=!is_absolute(time);
  if(result)
    result=(*parent_)[i]==other;

  if(result)
    {
      path.push_back((*motion_)[i]);
      sens.push_back(false);
    }

  if(!result)
    {
      if(!is_absolute(time))
        if(!(*parent_)[i]->reached())
          {
            path.push_back((*motion_)[i]);
            sens.push_back(false);
            result=(*parent_)[i]->recursive_path_from_local_to_cs(other,time,
                                                                  path,sens);
            if(!result)
              {
                path.pop_back();
                sens.pop_back();
              }
          }
      if(!result)
        {
          if(_potential_children!=0)
            {
              for(child=_potential_children->begin();
                  child!=_potential_children->end()&&!result;
                  ++child)
                {
                  result=!(*child)->reached();
                  if(result)
                    {
                      j=(*child)->matching_interval(time);
                      result=(*(*child)->parent_)[j].ptr()==this;
                    }
                  if(result)
                    result=(*(*child)->motion_)[j]->is_invertible(time);
                  if(result)
                    {
                      result=(*child)==other;
                      path.push_back((*(*child)->motion_)[j]);
                      sens.push_back(true);
                      if(!result)
                        {
                          result=(*child)->recursive_path_from_local_to_cs(other,time,
                                                                           path,sens);
                        
                          if(!result)
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

  return result;
}

//---------------------------------------------------------------------------
// Is `this' an absolute spatial coordinate system at time `time'?
// REQUIRE: valid_time(time)
//---------------------------------------------------------------------------
bool vcsl_spatial::is_absolute(const double time) const
{
  // require
  assert(valid_time(time));

  bool result;
  int i;

  result=parent_==0;
  if(!result)
    {
      i=matching_interval(time);
      result=(*parent_)[i].ptr()==0;
    }

  return result;
}

//---------------------------------------------------------------------------
// From a vector `v' exprimed in `this',
// return a vector exprimed in the spatial coordinate system `other'
// REQUIRE: path_from_local_to_cs_exists(other,time)
//---------------------------------------------------------------------------
vnl_vector<double> *
vcsl_spatial::from_local_to_cs(const vnl_vector<double> &v,
                               const vcsl_spatial_ref &other,
                               const double time)
{
  // require
  assert(path_from_local_to_cs_exists(other,time));

  vnl_vector<double> *result;
  vnl_vector<double> *tmp;
  const vnl_vector<double> *tmp2;

  vcl_vector<vcsl_spatial_transformation_ref> path;
  vcl_vector<bool> sens;

  vcl_vector<vcsl_spatial_transformation_ref>::const_iterator i;
  vcl_vector<bool>::const_iterator j;

  path_from_local_to_cs(other,time,path,sens);

  tmp2=from_cs_to_standard_units(v);

  j=sens.begin();

  for(i=path.begin();i!=path.end();++i)
    {
      if(*j)
        tmp=(*i)->inverse(*tmp2,time);
      else
        tmp=(*i)->execute(*tmp2,time);
      delete tmp2;
      tmp2=tmp;
      ++j;
    }
  tmp=other->from_standard_units_to_cs(*tmp2);
  delete tmp2;
  result=tmp;

  return result;
}

void vcsl_spatial::set_graph(const vcsl_graph_ref &new_graph)
{
  if(graph_.ptr()!=0)
    graph_->remove(this);
  graph_=new_graph;
  graph_->put(this);
}

void vcsl_spatial::set_reached(const bool &new_reached)
{
  reached_=new_reached;
}

bool vcsl_spatial::reached(void) const
{
  return reached_;
}
