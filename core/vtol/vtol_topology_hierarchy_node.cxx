#include <vtol/vtol_topology_hierarchy_node.h>
#include <vtol/top_hold.h>
#include <vcl/vcl_algorithm.h>


vtol_topology_hierarchy_node::vtol_topology_hierarchy_node()
  :  _inferiors(0), _superiors(0)
{
 
}

vtol_topology_hierarchy_node::~vtol_topology_hierarchy_node()
{
  unlink_all_inferiors(NULL);
  unlink_all_superiors(NULL);
}


/*
******************************************************
*
*  Linking Functions
*/

// -- Static variable.  When true, fast updates are done (superiors are not
// linked) under the understanding that they will be completed later.
// bool vtol_topology_hierarchy_node::DEFER_SUPERIORS = false;

bool
vtol_topology_hierarchy_node::link_inferior (top_hold *parent, top_hold* child)
{
  if (child  == NULL)
    {
      cerr << "Attempt to link null inferior element : " << endl;
      return false;
    }


  // Normal code
   

  // if (!_inferiors.find(child))
  if (!find(_inferiors , child))
    {
      _inferiors.push_back(child);
      child->link_superior(parent);
      return true;
    }
  else
    return false;
}

bool
vtol_topology_hierarchy_node::link_superior (top_hold *parent, top_hold* child)
{
  if (parent  == NULL)
    {
      cerr << "Attempt to link null superior element : " << endl;
      return false;
    }

  // if (!_superiors.find(parent))
  if (!find(_superiors,parent))
    {
      _superiors.push_back(parent);
      parent->link_inferior(child);
      return true;
    }
  else
    return false;
}

// 
// Creates a link from the calling object to superior by adding
// superior to the calling object's _superiors list.  The link
// is one way in that the calling object is not added to 
// superior's _inferiors list.  If superior is sucessfully
// added to the calling object's _superiors list, true is
// returned.  False is returned otherwise.
//
bool
vtol_topology_hierarchy_node::link_superior_oneway( top_hold * superior )
{
  if (superior == NULL)
  {
      cerr << "Attempt to link null superior element : " << endl;
      return false;
  }

  if ( !find(_superiors, superior ) )
  {
      _superiors.push_back( superior );
      return true;
  }
  else
    return false;
}

// 
// Creates a link from the calling object to inferior by adding
// inferior to the calling object's _inferiors list.  The link
// is one way in that the calling object is not added to 
// inferiors's _superiors list.  If inferior is successfully
// added to the calling object's _inferiors list, true is
// returned.  False is returned otherwise.
//
bool
vtol_topology_hierarchy_node::link_inferior_oneway( top_hold * inferior )
{
  if (inferior == NULL)
  {
      cerr << "Attempt to link null inferior element : " << endl;
      return false;
  }

  // if ( !_inferiors.find( inferior ) )
  if ( !find(_inferiors, inferior ) )
  {
      _inferiors.push_back( inferior );
      return true;
  }
  else
    return false;
}

 /*
 ******************************************************
 *
 *   Unlinking functions
 */

bool
vtol_topology_hierarchy_node::unlink_inferior (top_hold* parent, top_hold* child)
{
  if (child  == NULL) return false;

  if (find(_inferiors,child))
    {
      remove(_inferiors,child);
      child->unlink_superior(parent);
      return true;
    }
  else return false;
}

bool
vtol_topology_hierarchy_node::unlink_superior (top_hold* parent, top_hold* child)
{
  if (parent  == NULL) return false;

  if (find(_superiors,parent))
    {
      remove(_superiors,parent);
      parent->unlink_inferior(child);
      return true;
    }
  else
    return false;
}


bool
vtol_topology_hierarchy_node::unlink_superior_simple (top_hold* link)
{
  if (link  == NULL) return false;

  if (find(_superiors,link))
    {
      remove(_superiors,link);
      return true;
    }
  else return false;
}


bool
vtol_topology_hierarchy_node::unlink_inferior_simple (top_hold* link)
{
  if (link  == NULL) return false;
  if (find(_inferiors,link))
    {
      remove(_inferiors,link);
      return true;
    }
  else  return false;
}


void
vtol_topology_hierarchy_node::unlink_all_superiors (top_hold * child)
{
  // Child is the object calling this method.

  if (child != NULL)
    {
//    top_hold* link;
//    int size = _superiors.length();
//    for(register int i=0;i<size;i++)
//	{
//	  link = _superiors.get(i);
          unlink_inferior_simple(child);
//	}
    }
  _superiors.clear();
}

void
vtol_topology_hierarchy_node::unlink_all_inferiors (top_hold *parent)
{
  // Parent is the object calling this method.

  if (parent != NULL)
    {
//    top_hold* link;
//    int size = _inferiors.length();
//    for(register int i=0;i<size;i++)
//	{
//	  link = _inferiors.get(i);
          unlink_superior_simple(parent);
//	}
    }

  _inferiors.clear();
}

// 
// Removes the two way links between the calling object
// and all its inferiors.  In other words, for each 
// inferior of thisobj (the invoking object), the pointer 
// from thisobj to the inferior is removed and the pointer
// from the inferior to thisobj is removed.  RYF
//
void 
vtol_topology_hierarchy_node::unlink_all_inferiors_twoway( top_hold * thisobj )
{
    // thisobj should be a pointer to the invoking object
  //  for ( _inferiors.reset(); _inferiors.next(); )
  for(topology_list::iterator i = _inferiors.begin();
      i != _inferiors.end(); ++i)
  {
        top_hold* link;
        link = *i;
        if ( link )
	  link->unlink_superior_simple( thisobj ); // remove up link
        else
  	    cout << "Null value in inferior's list\n";
    }
    _inferiors.clear();  // remove all down links
}

// 
// Removes the two way links between the calling object
// and all its superiors.  In other words, for each 
// superior of thisobj (the invoking object), the pointer 
// from thisobj to the superior is removed and the pointer
// from the superior to thisobj is removed.  RYF
//
void 
vtol_topology_hierarchy_node::unlink_all_superiors_twoway( top_hold * thisobj )
{
    // thisobj should be a pointer to the invoking object
  // for ( _superiors.reset(); _superiors.next(); )
    
    for(topology_list::iterator i = _superiors.begin();
      i != _superiors.end(); ++i)
      {
        top_hold* link;
        link = *i;
        if ( link )
	  link->unlink_inferior_simple( thisobj ); // remove down link
        else
  	    cout << "Null value in inferior's list\n";
    }
    _superiors.clear();  // remove all up links
}

bool vtol_topology_hierarchy_node::find(topology_list &list, 
					top_hold *object)
{
  // returns true if the element is in the list
  
  topology_list::iterator i =
    vcl_find(list.begin(), list.end(),object);
  
  if(i!= list.end()){
    return true;
  }
  
  return false;
}

void vtol_topology_hierarchy_node::remove(topology_list &list, 
					top_hold *object)
{
 
    topology_list::iterator i =
    vcl_find(list.begin(), list.end(),object);
  
    if(i!= list.end()){
      
      list.erase(i);
    }
}




/*
******************************************************
*
*   Accessing functions
*/



// New methods for transformation and
// list traversal.


// void vtol_topology_hierarchy_node::Describeinferiors(ostream& strm,int blanking)
//{
//  BLANK_DESCRIBE;
//
//  if (_inferiors.is_empty())
//    strm << "**INFERIORS:  Empty" << endl;
//  else  strm << "**INFERIORS:" << endl;
//
//  for (_inferiors.reset(); _inferiors.next();)
//    {
//    strm << TopoNames[_inferiors.value()->GetType()] << _inferiors.value() << "   ";
//      _inferiors.value()->Print();
//    }
//}



//void vtol_topology_hierarchy_node::Describesuperiors(ostream& strm,int blanking)
//{
//
//  BLANK_DESCRIBE;
//  if (_superiors.is_empty())
//    strm << "**SUPERIORS:  Empty" << endl;
//  else  strm << "**SUPERIORS:" << endl;
//
//  for (_superiors.reset(); _superiors.next();)
//    {
//    strm << TopoNames[_superiors.value()->GetType()] << _superiors.value() << "  ";
//      _superiors.value()->Print();
//    }
//}


void vtol_topology_hierarchy_node::Print(ostream& strm)
{
  strm << "<vtol_topology_hierarchy_node " << (void *)this << ">" << endl;
  strm << "number of inferiors " << numinf() << endl;
  strm << "number of superiors " << numsup() << endl;
    

}


//void vtol_topology_hierarchy_node::Describe(ostream& strm,int blanking)
//{
//  Describeinferiors(strm, blanking);
//  Describesuperiors(strm, blanking);
//}









