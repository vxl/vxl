// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>
#include <vtol/vtol_hierarchy_node_3d.h>
#include <vcl/vcl_algorithm.h>

//:
// constructor
vtol_hierarchy_node_3d::vtol_hierarchy_node_3d()
{

}

//:
// destructor

vtol_hierarchy_node_3d::~vtol_hierarchy_node_3d()
{

}


//****************************
//   Accessors
//***************************


//:
// set inferiors

void vtol_hierarchy_node_3d::set_hierarchy_inferiors(const hierarchy_node_list_3d& inferiors)
{
  _hierarchy_inferiors=inferiors;

}

//:
// set superiors

void vtol_hierarchy_node_3d::set_hierarchy_superiors(const hierarchy_node_list_3d& superiors)
{
  _hierarchy_superiors=superiors;
}

//:
// get inferiors

hierarchy_node_list_3d* vtol_hierarchy_node_3d::hierarchy_inferiors()
{
  hierarchy_node_list_3d *new_list = new hierarchy_node_list_3d();
  for(int i=0;i<_hierarchy_inferiors.size();i++)
    new_list->push_back(_hierarchy_inferiors[i]);
  return new_list;
}


//:
// get superiors

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

//:
// add inferior

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


//:
// remove inferior

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


//:
// add superior

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


//:
// remove superior

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


//:
// remover inferior one way

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

//:
// remove superior one way

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


