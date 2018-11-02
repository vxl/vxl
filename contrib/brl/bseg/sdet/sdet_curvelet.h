// This is sdet_curvelet.h
#ifndef sdet_curvelet_h
#define sdet_curvelet_h
//:
//\file
//\brief A class to represent an edgel grouping with an associated curve model (curvelet now called curve bundle)
//\author Amir Tamrakar
//\date 09/05/06
//
//\verbatim
//  Modifications
//  Amir Tamrakar 09/05/06          Moved it from sdet_se1.h to a new file
//
//  Ozge Can Ozcanli Jan 12, 2007   Added copy constructor
//
//\endverbatim

#include <vector>
#include <iostream>
#include <list>
#include <set>
#include <map>
#include <utility>
#include <vbl/vbl_ref_count.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vnl/vnl_math.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_clip.h>
#include <vgl/vgl_area.h>

#include <bvgl/algo/bvgl_eulerspiral.h>
#include <sdet/sdet_edgel.h>
#include <sdet/sdet_curve_model.h>

//forward declaration
class sdet_curvelet;

typedef std::list<sdet_curvelet*> sdet_curvelet_list;
typedef std::list<sdet_curvelet*>::iterator sdet_curvelet_list_iter;
typedef std::list<sdet_curvelet*>::const_iterator sdet_curvelet_list_const_iter;

//: The curvelet class: stores the ordered list of edgels defining the curvelet
//  and also the curve model defined by the grouping
//  It also stores a list of higher order curvelets that it forms.
class sdet_curvelet
{
public:
  sdet_edgel* ref_edgel;                        ///< ref edgel (the edgel to which it is anchored)

  std::vector<sdet_edgel*> edgel_chain;          ///< the ordered list of edgels
  sdet_curve_model* curve_model;                ///< associated curve model

  bool forward;                                  ///< is this a forward cvlet or a reverse cvlet
  double length;                                 ///< length of the curvelet
  double quality;                                ///< the quality of this grouping (determined by various means)

  bool used;                                     ///< to keep track of whether this curvelet was used in linking

  //: default constructor
  sdet_curvelet(): ref_edgel(nullptr), edgel_chain(0), curve_model(nullptr), forward(true), length(0.0), quality(0.0), used(false){}

  //: constructor 1
  sdet_curvelet(sdet_edgel* e) : ref_edgel(e), edgel_chain(0), curve_model(nullptr), forward(true), length(0.0), quality(0.0), used(false){}

  //: constructor 2
  sdet_curvelet(sdet_edgel* e, sdet_curve_model* cm, bool dir=true) :
    ref_edgel(e), edgel_chain(0), curve_model(cm), forward(dir), length(0.0), quality(0.0), used(false){}

  //: constructor 3
  sdet_curvelet(sdet_edgel* e, sdet_curve_model* cm, std::vector<sdet_edgel*> &echain, bool dir=true) :
    ref_edgel(e), edgel_chain(echain), curve_model(cm), forward(dir), length(0.0), quality(0.0), used(false){}

  //: constructor 4
  sdet_curvelet(sdet_edgel* e, sdet_curve_model* cm, std::deque<sdet_edgel*> &echain, bool dir=true) :
    ref_edgel(e), curve_model(cm), forward(dir), length(0.0), quality(0.0), used(false)
  {
    edgel_chain.insert(edgel_chain.end(), echain.begin(), echain.end());
  }

  //: copy constructor
  sdet_curvelet(const sdet_curvelet& other);

  //: copy constructor with provisions for copying a different curve bundle
  sdet_curvelet(const sdet_curvelet& other, sdet_curve_model* cm);

  //: destructor
  ~sdet_curvelet();

  //: return the order of this grouping
  unsigned order() const { return edgel_chain.size(); }

  //: add an edgel
  void push_back(sdet_edgel* e) { edgel_chain.push_back(e); }

  //: add the associated curve model for this grouping
  void set_curve_model(sdet_curve_model* cm) { curve_model = cm; }

  //: replace the curve model associated with this grouping with a new one
  void replace_curve_model(sdet_curve_model* cm)
  {
    if (!cm){
      std::cout << "attempting to replace CB with NULL!" << std::endl;
      return;
    }

    //assert(cm);
    if (curve_model)
      delete curve_model;

    curve_model = cm;
  }

  //: returns the forward(child) edgel_chain
  std::list<sdet_edgel*> child_chain();

  //: returns the backward(child) edgel_chain
  std::list<sdet_edgel*> parent_chain();

  //: compute properties of this curvelet once formed
  void compute_properties(double R, double token_len);

  //: print info to file
  void print(std::ostream&);
};

#endif // sdet_curvelet_h
