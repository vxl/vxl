#include <vtol/vtol_hierarchy_node_3d.h>
#include <vcl/vcl_algorithm.h>
#include <vcl/vcl_iostream.h>

// constructor
vtol_hierarchy_node_3d::vtol_hierarchy_node_3d()
{

}

// destructor

vtol_hierarchy_node_3d::~vtol_hierarchy_node_3d()
{

}


//****************************
//   Accessors
//***************************


// -- set inferiors

void vtol_hierarchy_node_3d::set_hierarchy_inferiors(const hierarchy_node_list_3d& inferiors)
{
  _hierarchy_inferiors=inferiors;

}

// -- set superiors

void vtol_hierarchy_node_3d::set_hierarchy_superiors(const hierarchy_node_list_3d& superiors)
{
  _hierarchy_superiors=superiors;
}

// -- get inferiors

hierarchy_node_list_3d* vtol_hierarchy_node_3d::hierarchy_inferiors()
{
  hierarchy_node_list_3d *new_list = new hierarchy_node_list_3d();
  for(int i=0;i<_hierarchy_inferiors.size();i++)
    new_list->push_back(_hierarchy_inferiors[i]);
  return new_list;
}


// -- get superiors

hierarchy_node_list_3d* vtol_hierarchy_node_3d::hierarchy_superiors()
{
  hierarchy_node_list_3d *new_list = new hierarchy_node_list_3d();
  for(int i=0;i<_hierarchy_superiors.size();i++)
    {
      new_list->push_back(_hierarchy_superiors[i]);
    }
  return new_list;
}


// *****************************
//   Utility Functions
// *****************************

// -- add inferior

bool vtol_hierarchy_node_3d::add_hierarchy_inferior(vtol_hierarchy_node_3d* link)
{
  if (link  == NULL)
    {
      cerr << "Attempt to link null inferior element\n";
      return false;
    }
  if (vcl_find(_hierarchy_inferiors.begin(),_hierarchy_inferiors.end(),link) != _hierarchy_inferiors.end())
    {
      _hierarchy_inferiors.push_back(link);
      link->add_hierarchy_superior(this);
      return true;
    }
  else return false;
}


// -- remove inferior

bool vtol_hierarchy_node_3d::remove_hierarchy_inferior(vtol_hierarchy_node_3d*  link)
{
  if (link  == NULL) return false;

  hierarchy_node_list_3d::iterator i =
    vcl_find(_hierarchy_inferiors.begin(),_hierarchy_inferiors.end(),link);
  if (i != _hierarchy_inferiors.end()) 
    {

      _hierarchy_inferiors.erase(i);

      link->remove_hierarchy_superior(this);
      return true;
    }
  else return false;
}


// -- add superior

bool vtol_hierarchy_node_3d::add_hierarchy_superior(vtol_hierarchy_node_3d* link)
{
  if (link  == NULL)
    {
      cerr << "Attempt to link null superior element\n";
      return false;
    }


  
  hierarchy_node_list_3d::iterator i =
    vcl_find(_hierarchy_superiors.begin(),_hierarchy_superiors.end(),link);
  if (i != _hierarchy_superiors.end())
    {
      _hierarchy_superiors.push_back(link);
      link->add_hierarchy_inferior(this);
      return true;
    }
  else return false;
}


// -- remove superior

bool vtol_hierarchy_node_3d::remove_hierarchy_superior(vtol_hierarchy_node_3d* link)
{
  if (link  == NULL) return false;


  
  hierarchy_node_list_3d::iterator i =
    vcl_find(_hierarchy_superiors.begin(),_hierarchy_superiors.end(),link);
  if (i != _hierarchy_superiors.end())
    {
      _hierarchy_superiors.erase(i);
      link->remove_hierarchy_inferior(this);
      return true;
    }
  else return false;
}


// -- remove inferior one way

bool vtol_hierarchy_node_3d::add_hierarchy_inferior_oneway(vtol_hierarchy_node_3d* link)
{
  if (link  == NULL)
    {
      cerr << "Attempt to link null inferior element\n";
      return false;
    }
  

  hierarchy_node_list_3d::iterator i =
  vcl_find(_hierarchy_inferiors.begin(),_hierarchy_inferiors.end(),link);
   if (i == _hierarchy_inferiors.end()) 
     {
      _hierarchy_inferiors.push_back(link);
      return true;
    }
  else return false;
}

// -- remove superior one way

bool vtol_hierarchy_node_3d::add_hierarchy_superior_oneway(vtol_hierarchy_node_3d* link)
{
  if (link  == NULL)
    {
      cerr << "Attempt to link null superior element\n";
      return false;
    }


  hierarchy_node_list_3d::iterator i =
  vcl_find(_hierarchy_superiors.begin(),_hierarchy_superiors.end(),link);
   if (i == _hierarchy_superiors.end())
     {
      _hierarchy_superiors.push_back(link);
      return true;
    }
    else return false;
}


