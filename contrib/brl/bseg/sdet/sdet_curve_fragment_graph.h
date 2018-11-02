// This is sdet_curve_fragment_graph.h
#ifndef sdet_curve_fragment_graph_h
#define sdet_curve_fragment_graph_h
//:
//\file
//\brief Curve Fragment graph structure
//\author Amir Tamrakar
//\date 04/20/07
//
//\verbatim
//  Modifications
//
//    Amir Tamrakar  The graph structure used to be represented as a node adjacency list
//                   where the nodes are stored in a vector indexed by its id. This was simply
//                   mimicking the edge link graph setup. However, this is a waste for representing
//                   a curve fragment graph where the nodes are only the endpoint and junction edgels.
//                   So I've switched to maps instead.
//\endverbatim

#include <vector>
#include <iostream>
#include <list>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <sdet/sdet_edgel.h>
#include <sdet/sdet_edgemap.h>
#include <sdet/sdet_CFTG.h>

//: This class represents the curve fragment graph formed from the edgels
//  The links are curve fragments represented by edgel chains.
class sdet_curve_fragment_graph
{
public:
  std::vector<sdet_edgel_chain_list> cFrags; ///< child curve fragments
  std::vector<sdet_edgel_chain_list> pFrags; ///< parent curve fragments

  sdet_edgel_chain_list frags; ///< redundant single list of all fragments

  //this is a hack (need to move this out of here into the storage class)
  sdet_CFTG CFTG; ///< The Curve Fragment Topology Graph (CFTG)

  //: constructor
  sdet_curve_fragment_graph(int size=0): cFrags(size), pFrags(size){}

  //: destructor
  ~sdet_curve_fragment_graph()
  {
    clear(); //delete everything upon exit
    CFTG.clear();
  }

  //Access functions
  unsigned size() { return cFrags.size(); }//should be the same as edgels.size()

  //: resize the graph
  void resize(unsigned size)
  {
    if (size!=cFrags.size()){
      clear();
      CFTG.clear();
    }

    cFrags.resize(size);
    pFrags.resize(size);

    CFTG.resize(size);
  }

  //: clear the graph
  void clear()
  {
    //delete all the curve fragments
    sdet_edgel_chain_list_iter f_it = frags.begin();
    for (; f_it != frags.end(); f_it++)
      delete (*f_it);

    frags.clear();
    cFrags.clear();
    pFrags.clear();

    CFTG.clear();
  }

  //: add a curve fragment to the graph
  void insert_fragment(sdet_edgel_chain* chain)
  {
    sdet_edgel* e1 = chain->edgels.front();
    sdet_edgel* e2 = chain->edgels.back();

    cFrags[e1->id].push_back(chain);
    pFrags[e2->id].push_back(chain);

    frags.push_back(chain);
  }

  //: remove a curve fragment
  void remove_fragment(sdet_edgel_chain* chain)
  {
    sdet_edgel* e1 = chain->edgels.front();
    sdet_edgel* e2 = chain->edgels.back();

    pFrags[e2->id].remove(chain);
    cFrags[e1->id].remove(chain);

    frags.remove(chain);

    delete chain;
  }

  //: just extract a curve fragment from the graph do not delete
  void extract_fragment(sdet_edgel_chain* chain)
  {
    sdet_edgel* e1 = chain->edgels.front();
    sdet_edgel* e2 = chain->edgels.back();

    pFrags[e2->id].remove(chain);
    cFrags[e1->id].remove(chain);

    frags.remove(chain);
  }

  friend class sdet_edge_map;
};

#endif // sdet_curve_fragment_graph_h
