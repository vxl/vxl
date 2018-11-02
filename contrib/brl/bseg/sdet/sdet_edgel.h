// This is sdet_edgel.h
#ifndef sdet_edgel_h
#define sdet_edgel_h
//:
//\file
//\brief Edgel class for the Symbolic edge linking algorithm
//\author Amir Tamrakar
//\date 09/05/06
//
//\verbatim
//  Modifications
//  Amir Tamrakar 09/05/06          Moved it from sdet_se1.h to a new file
//  Amir Tamrakar 11/15/06          Removed the link graph structures from this class to make it lighter
//
//  Ozge Can Ozcanli Jan 12, 2007   Added copy constructor
//  Ricardo Fabbri Aug 03, 2009     Fixed copy constructor to clone appearance members,
//                                  to avoid delete-ing unitialized memory.
//  Ricardo Fabbri Aug 12, 2009     Added copy assignment, avoiding segfaults.
//
//\endverbatim

#include <vector>
#include <iostream>
#include <deque>
#include <list>
#include <vbl/vbl_ref_count.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgl/vgl_point_2d.h>

#include <sdet/sdet_appearance.h>

//forward class definitions
class sdet_edgel;
class sdet_curvelet;

//useful type definitions
typedef std::deque<sdet_edgel* > sdet_edgel_list;
typedef std::deque<sdet_edgel* >::iterator sdet_edgel_list_iter;
typedef std::deque<sdet_edgel* >::const_iterator sdet_edgel_list_const_iter;

//: edgel class: contains pt, tangent and collection of all the groupings around it
class sdet_edgel
{
public:
  int id;                  ///< unique id

  vgl_point_2d<double> pt; ///< the location of the edgel
  double tangent;          ///< the orientation of the edgel in radians

  //: the strength of the edgel (typically gradient magnitude)
  double strength;

  //: second derivative of the image in the direction of the gradient
  double deriv;

  //: Harris Corner like measure to factor in the error in edge localization at
  // corners and junctions
  double uncertainty;

  vgl_point_2d<int> gpt;   ///< hack to store the grid coordinates

  //: appearance information stored with the edgels
  sdet_appearance* left_app;  ///< appearance on the left side of the edgel
  sdet_appearance* right_app; ///< appearance on the right side of the edgel

  //: default constructor
  sdet_edgel() :
    id((unsigned)-1),
    pt(vgl_point_2d<double>(0,0)),
    tangent(0.0),
    strength(0.0),
    deriv(0.0),
    uncertainty(0.0),
    left_app(new sdet_intensity(0.0)),
    right_app(new sdet_intensity(0.0)){}

  //: constructor
  //
  sdet_edgel(vgl_point_2d<double> new_pt, double tan,
      double conf=0.0, double der=0.0, double uncer=0.0,
      sdet_appearance* lapp=new sdet_intensity(0.0),
      sdet_appearance* rapp=new sdet_intensity(0.0));

  //: copy constructor
  sdet_edgel(const sdet_edgel& other);

  //: Assignment operator
  sdet_edgel & operator=(const sdet_edgel &rhs);

  //: destructor
  ~sdet_edgel();
};

//: A class to hold a chain of edgels that defines the image curve
class sdet_edgel_chain
{
public:
  sdet_edgel_list edgels;
  bool temp; //temp flag for the CFTG

  //: constructor
  sdet_edgel_chain(): edgels(0), temp(false){}
  ~sdet_edgel_chain()= default;

  //: copy constructor
  sdet_edgel_chain(const sdet_edgel_chain& chain):
    edgels(chain.edgels.size()), temp(false)
  {
    for (unsigned i=0; i<chain.edgels.size(); i++)
      edgels[i] = chain.edgels[i];
  }

  //merge two edge chains together (expect some overlap)
  void merge(sdet_edgel_chain* n_chain)
  {
    //there are going to be some duplications
    //need to remove those elements
    bool found_last=false;
    sdet_edgel_list_iter lit = n_chain->edgels.begin();
    for(;lit!=n_chain->edgels.end();lit++){
      if ((*lit)==edgels.back())
        found_last=true;

      if (found_last)
        edgels.push_back(*lit);
    }
  }

  void append(std::vector<sdet_edgel*>& n_chain)
  {
    for (auto i : n_chain)
      edgels.push_back(i);
  }

  void append(sdet_edgel_list& n_chain)
  {
    int sizeofchain=n_chain.size();
    for (int i=1; i<sizeofchain; i++)
      edgels.push_back(n_chain[i]);
  }

  void push_back(sdet_edgel* edgel){ edgels.push_back(edgel); }
  void push_front(sdet_edgel* edgel){ edgels.push_front(edgel); }
};

typedef std::list<sdet_edgel_chain*> sdet_edgel_chain_list;
typedef std::list<sdet_edgel_chain*>::iterator sdet_edgel_chain_list_iter;

#endif // sdet_edgel_h
