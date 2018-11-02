// This is sdet_CFTG.h
#ifndef sdet_CFTG_h
#define sdet_CFTG_h
//:
//\file
//\brief Curve Fragment Topology Graph (CFTG) structure
//\author Amir Tamrakar
//\date 08/08/08
//
//\verbatim
//  Modifications
//
//    Amir Tamrakar
//\endverbatim

#include <vector>
#include <iostream>
#include <list>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <sdet/sdet_edgel.h>
#include <sdet/sdet_edgemap.h>

class sdet_CFTG_link
{
public:
  sdet_edgel* eS;
  sdet_edgel* eE;
  sdet_edgel_chain_list cCFs; ///< candidate curve fragments
  double cost;

  sdet_CFTG_link(sdet_edgel* e1, sdet_edgel* e2): eS(e1), eE(e2), cCFs(0), cost(0.0){}
  ~sdet_CFTG_link(){ eS = nullptr; eE = nullptr; cCFs.clear(); cost = 0;}

  void add_fragment(sdet_edgel_chain* chain)
  {
    cCFs.push_back(chain);
  }

};

typedef std::list<sdet_CFTG_link*> sdet_CFTG_link_list;
typedef std::list<sdet_CFTG_link*>::iterator sdet_CFTG_link_list_iter;

//: This class represents the curve fragment topology graph formed from the contour fragments
//  The links are equivalence classes of curve fragments represented by sets of edgel chains
class sdet_CFTG
{
public:
  std::vector<sdet_CFTG_link_list> cLinks; ///< child links
  std::vector<sdet_CFTG_link_list> pLinks; ///< parent links

  sdet_CFTG_link_list Links; ///< redundant single list of all links

  //: constructor
  sdet_CFTG(int size=0): cLinks(size), pLinks(size){}

  //: destructor
  ~sdet_CFTG()
  {
    clear(); //delete everything upon exit
  }

  //Access functions
  unsigned size() { return cLinks.size(); }//should be the same as edgels.size()

  //: resize the graph
  void resize(unsigned size)
  {
    if (size!=cLinks.size())
      clear();

    cLinks.resize(size);
    pLinks.resize(size);
  }

  //: clear the graph
  void clear()
  {
    //delete all the curve fragments
    sdet_CFTG_link_list_iter l_it = Links.begin();
    for (; l_it != Links.end(); l_it++)
      delete (*l_it);

    Links.clear();
    cLinks.clear();
    pLinks.clear();
  }

  //: add a curve fragment to the graph
  void insert_fragment(sdet_edgel_chain* chain)
  {
    sdet_edgel* e1 = chain->edgels.front();
    sdet_edgel* e2 = chain->edgels.back();

    //if there is a link already, add it to the existing link
    sdet_CFTG_link* cur_Link = nullptr;
    if (cLinks[e1->id].size()>0){
      //find the link in here
      sdet_CFTG_link_list_iter l_it = cLinks[e1->id].begin();
      for (; l_it != cLinks[e1->id].end(); l_it++){
        if ((*l_it)->eE==e2){
          cur_Link = (*l_it);
          break;
        }
      }
    }

    if (cur_Link){
      cur_Link->add_fragment(chain);
    }
    else //otherwise create a new link and add it to it
    {
      cur_Link = new sdet_CFTG_link(e1, e2);
      cur_Link->add_fragment(chain);

      //add the link to the graph
      cLinks[e1->id].push_back(cur_Link);
      pLinks[e2->id].push_back(cur_Link);

      Links.push_back(cur_Link);
    }
  }


  //: remove a Link
  void remove_link(sdet_CFTG_link* link)
  {
    cLinks[link->eS->id].remove(link);
    pLinks[link->eE->id].remove(link);

    Links.remove(link);

    delete link;
  }

};

#endif // sdet_CFTG_h
