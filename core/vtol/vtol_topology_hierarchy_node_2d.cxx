#include <vtol/vtol_topology_hierarchy_node_2d.h>
#include <vtol/vtol_topology_object_2d.h>
#include <vcl/vcl_algorithm.h>

//:
// constructor
vtol_topology_hierarchy_node_2d::vtol_topology_hierarchy_node_2d()
  :  _inferiors(0), _superiors(0)
{
 
}

//:
// constructor
vtol_topology_hierarchy_node_2d::vtol_topology_hierarchy_node_2d
(int num_inferiors, int num_superiors):_inferiors(num_inferiors),_superiors(num_superiors)
{
}

//:
// destructor
vtol_topology_hierarchy_node_2d::~vtol_topology_hierarchy_node_2d()
{
  unlink_all_inferiors(0);
  unlink_all_superiors(0);
}


/*
******************************************************
*
*  Linking Functions
*/

//:
// -- Static variable.  When true, fast updates are done (superiors are not
// linked) under the understanding that they will be completed later.

bool
vtol_topology_hierarchy_node_2d::link_inferior (vtol_topology_object_2d *parent, vtol_topology_object_2d* child)
{
  if (child  == 0)
    {
      cerr << "Attempt to link null inferior element : " << endl;
      return false;
    }


  // Normal code
   

  // if (!_inferiors.find(child))
  if (vcl_find(_inferiors.begin(),_inferiors.end() , child) == _inferiors.end())
    {
      _inferiors.push_back(child);
      child->link_superior(parent);
      return true;
    }
  else
    return false;
}

//:
// link in a superior
bool
vtol_topology_hierarchy_node_2d::link_superior (vtol_topology_object_2d *parent, vtol_topology_object_2d* child)
{
  if (parent  == 0)
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

//: 
// Creates a link from the calling object to superior by adding
// superior to the calling object's _superiors list.  The link
// is one way in that the calling object is not added to 
// superior's _inferiors list.  If superior is sucessfully
// added to the calling object's _superiors list, true is
// returned.  False is returned otherwise.
//
bool
vtol_topology_hierarchy_node_2d::link_superior_oneway( vtol_topology_object_2d * superior )
{
  if (superior == 0)
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

//:
// Creates a link from the calling object to inferior by adding
// inferior to the calling object's _inferiors list.  The link
// is one way in that the calling object is not added to 
// inferiors's _superiors list.  If inferior is successfully
// added to the calling object's _inferiors list, true is
// returned.  False is returned otherwise.
//
bool
vtol_topology_hierarchy_node_2d::link_inferior_oneway( vtol_topology_object_2d * inferior )
{
  if (inferior == 0)
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

//:
// unlink an inferior

bool
vtol_topology_hierarchy_node_2d::unlink_inferior (vtol_topology_object_2d* parent, vtol_topology_object_2d* child)
{
  if (child  == 0) return false;

  if (find(_inferiors,child))
    {
      remove(_inferiors,child);
      child->unlink_superior(parent);
      return true;
    }
  else return false;
}


//:
// unlink a superior

bool
vtol_topology_hierarchy_node_2d::unlink_superior (vtol_topology_object_2d* parent, vtol_topology_object_2d* child)
{
  if (parent  == 0) return false;

  if (find(_superiors,parent))
    {
      remove(_superiors,parent);
      parent->unlink_inferior(child);
      return true;
    }
  else
    return false;
}


//:
// unlink a superior

bool
vtol_topology_hierarchy_node_2d::unlink_superior_simple (vtol_topology_object_2d* link)
{
  if (link  == 0) return false;

  if (find(_superiors,link))
    {
      remove(_superiors,link);
      return true;
    }
  else return false;
}


//:
// unlink ab inferior

bool
vtol_topology_hierarchy_node_2d::unlink_inferior_simple (vtol_topology_object_2d* link)
{
  if (link  == 0) return false;
  if (find(_inferiors,link))
    {
      remove(_inferiors,link);
      return true;
    }
  else  return false;
}

//:
// unlink all superiors

void
vtol_topology_hierarchy_node_2d::unlink_all_superiors (vtol_topology_object_2d * child)
{
  // Child is the object calling this method.

  if (child != 0)
    {
//    vtol_topology_object_2d* link;
//    int size = _superiors.length();
//    for(register int i=0;i<size;i++)
//	{
//	  link = _superiors.get(i);
          unlink_inferior_simple(child);
//	}
    }
  _superiors.clear();
}


//:
// unlink all inferiors

void
vtol_topology_hierarchy_node_2d::unlink_all_inferiors (vtol_topology_object_2d *parent)
{
  // Parent is the object calling this method.

  if (parent != 0)
    {
//    vtol_topology_object_2d* link;
//    int size = _inferiors.length();
//    for(register int i=0;i<size;i++)
//	{
//	  link = _inferiors.get(i);
          unlink_superior_simple(parent);
//	}
    }

  _inferiors.clear();
}

//:
// Removes the two way links between the calling object
// and all its inferiors.  In other words, for each 
// inferior of thisobj (the invoking object), the pointer 
// from thisobj to the inferior is removed and the pointer
// from the inferior to thisobj is removed.  RYF
//
void 
vtol_topology_hierarchy_node_2d::unlink_all_inferiors_twoway( vtol_topology_object_2d * thisobj )
{
    // thisobj should be a pointer to the invoking object
  //  for ( _inferiors.reset(); _inferiors.next(); )
  for(topology_list_2d::iterator i = _inferiors.begin();
      i != _inferiors.end(); ++i)
  {
        vtol_topology_object_2d* link;
        link = *i;
        if ( link )
	  link->unlink_superior_simple( thisobj ); // remove up link
        else
  	    cout << "Null value in inferior's list\n";
    }
    _inferiors.clear();  // remove all down links
}

//: 
// Removes the two way links between the calling object
// and all its superiors.  In other words, for each 
// superior of thisobj (the invoking object), the pointer 
// from thisobj to the superior is removed and the pointer
// from the superior to thisobj is removed.  RYF
//
void 
vtol_topology_hierarchy_node_2d::unlink_all_superiors_twoway( vtol_topology_object_2d * thisobj )
{
    // thisobj should be a pointer to the invoking object
  // for ( _superiors.reset(); _superiors.next(); )
    
    for(topology_list_2d::iterator i = _superiors.begin();
      i != _superiors.end(); ++i)
      {
        vtol_topology_object_2d* link;
        link = *i;
        if ( link )
	  link->unlink_inferior_simple( thisobj ); // remove down link
        else
  	    cout << "Null value in inferior's list\n";
    }
    _superiors.clear();  // remove all up links
}

//:
// find function defined 
bool vtol_topology_hierarchy_node_2d::find(topology_list_2d &list, 
					vtol_topology_object_2d *object)
{
  // returns true if the element is in the list
  
  topology_list_2d::iterator i =
    vcl_find(list.begin(), list.end(),object);
  
  if(i!= list.end()){
    return true;
  }
  
  return false;
}

//:
// remove an element from the list

void vtol_topology_hierarchy_node_2d::remove(topology_list_2d &list, 
					vtol_topology_object_2d *object)
{
 
    topology_list_2d::iterator i =
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


// void vtol_topology_hierarchy_node_2d::Describeinferiors(ostream& strm,int blanking)
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
//      _inferiors.value()->print();
//    }
//}



//void vtol_topology_hierarchy_node_2d::Describesuperiors(ostream& strm,int blanking)
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
//      _superiors.value()->print();
//    }
//}

//:
// print the node
void vtol_topology_hierarchy_node_2d::print(ostream& strm)
{
  strm << "<vtol_topology_hierarchy_node_2d " << (void *)this << ">" << endl;
  strm << "number of inferiors " << numinf() << endl;
  strm << "number of superiors " << numsup() << endl;
    

}

void vtol_topology_hierarchy_node_2d::describe_inferiors(ostream& strm,int blanking)
{

  if (_inferiors.size()==0)
    strm << "**INFERIORS:  Empty" << endl;
  else  strm << "**INFERIORS:" << endl;
  
  
  for (topology_list_2d::iterator ii=_inferiors.begin();ii!= _inferiors.end();ii++)
    {
      (*ii)->print();
    }
}



void vtol_topology_hierarchy_node_2d::describe_superiors(ostream& strm,int blanking)
{

  if (_superiors.size()==0)
    strm << "**SUPERIORS:  Empty" << endl;
  else  strm << "**SUPERIORS:" << endl;

  for (topology_list_2d::iterator ii=_superiors.begin();ii!= _superiors.end();ii++)
    {
      (*ii)->print();
    }
}


void vtol_topology_hierarchy_node_2d::describe(ostream& strm,int blanking)
{
  describe_inferiors(strm, blanking);
  describe_superiors(strm, blanking);
}









