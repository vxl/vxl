// This is contrib/prip/vmap/vmap_grid_structure.h
#ifndef vmap_grid_structure_h_
#define vmap_grid_structure_h_
//:
// \file
// \brief provides a class which initialises the structure of a 2-map or a 2-tmap to a grid.
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  06 May 2004 Jocelyn Marchadier
// \endverbatim


#include "vmap_2_map.h"
#include "vmap_2_tmap.h"

//: A vmap_grid_structure is a map with a vmap_grid_structure structure.
class vmap_grid_structure
{
public :
  vmap_grid_structure(){}
	vmap_grid_structure(int arg_width, int arg_height) ;

  //: returns the index of the vertex located at i,j
  vmap_vertex_index vertex(int i,int j) const
  {
    return j*(_width+1)+i ;
  }

  //: returns the index of the face located at i,j
  vmap_face_index face(int i,int j) const
  {
    return j*_width+i ;
  }

  //: Returns alpha("arg"), i.e. the opposite dart of the same edge.
  vmap_dart_index alpha (vmap_dart_index arg) const
  {
    return (arg%2==0? arg+1:arg-1) ;//_nb_darts-1-arg ;//
  }

  //: index of the dart in the "up" direction associated to the vertex located at i,j
  vmap_dart_index up_dart(int i,int j) const
  {
    return alpha(2*(_horizontalEdges+i+(j-1)*(_width+1))) ;
  }
  
  //: index of the dart in the "right" direction associated to the vertex located at i,j
  vmap_dart_index right_dart(int i,int j) const
  {
    return 2*(i+j*_width) ;
  }
  
  //: index of the dart in the "down" direction associated to the vertex located at i,j
  vmap_dart_index down_dart(int i,int j) const
  {
    return 2*(_horizontalEdges+j*(_width+1)+i) ;
  }
  
  //: index of the dart in the "left" direction associated to the vertex located at i,j
  vmap_dart_index left_dart(int i,int j) const
  {
    return alpha(2*(i-1+j*_width)) ;
  }
  
	//: Returns the location of the face "arg".
  void face_location(vmap_face_index arg,int &i, int &j) const
  {
    i=arg%_width ;
    j=arg/_width ;
  }

  //: Returns the location of the vertex "arg".
  void vertex_location(vmap_face_index arg,int &i, int &j) const
  {
    i=arg%(_width+1) ;
    j=arg/(_width+1) ;
  }

  //: Returns the index of the background face.
  vmap_face_index background_index() const
  {
    return _width*_height ;
  }

  //: Initializes the vmap_grid_structure with "arg_width"*"arg_height" faces
  void initialise(int arg_width, int arg_height) ;

  //: returns the width of the grid to be built
  int width() const
  {
    return _width ;
  }

  //: returns the height of the grid to be built
  int height() const
  {
    return _height ;
  }
	
	int nb_darts() const
	{
		return _nb_darts ;
	}
	
	int nb_vertices() const
	{
		return (_width+1)*(_height+1) ;
	}
	
	int nb_edges() const
	{
		return _horizontalEdges+_verticalEdges ;
	}
	
	int nb_faces() const
	{
		return _width*_height+1 ;
	}
protected:
	
	//: The attributes of the grid
  int _width, _height, _horizontalEdges,_verticalEdges,_nb_darts;

};

#endif
