// This is sdet_edgel_link_graph.h
#ifndef sdet_edgel_link_graph_h
#define sdet_edgel_link_graph_h
//:
//\file
//\brief Link graph structure
//\author Amir Tamrakar
//\date 11/15/06
//
//\verbatim
//  Modifications
//\endverbatim

#include <vector>
#include <iostream>
#include <list>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <sdet/sdet_edgel.h>
#include <sdet/sdet_curvelet.h>

//: structure to describe an edgel link in the link graph
struct sdet_link {
  sdet_edgel* pe;                  ///< parent edgel
  sdet_edgel* ce;                  ///< child edgel

  double vote;                      ///< record votes for each link

  int deg_overlap;                  ///< to keep track of the degree of overlap
  bool flag;                        ///< a falg for miscellaneous purposes

  std::list<sdet_curvelet*> curvelets; ///< list of curvelets containing this link

  //: constructor
  sdet_link(sdet_edgel* e1, sdet_edgel* e2): pe(e1), ce(e2), vote(0.0), deg_overlap(0), flag(false), curvelets(0){}

  //: prune redundant curvelets
  void prune_redundant_curvelets()
  {
    //go over all the curvelets
    sdet_curvelet_list_iter cv_it = curvelets.begin();
    for (; cv_it != curvelets.end(); cv_it++)
    {
      sdet_curvelet* cvlet = (*cv_it);

      sdet_curvelet_list_iter cv_it2 = cv_it;
      cv_it2++;
      for (; cv_it2 != curvelets.end(); cv_it2++)
      {
        sdet_curvelet* dup_cvlet = (*cv_it2);

        //determine if this is duplicate
        if (cvlet->edgel_chain.size() != dup_cvlet->edgel_chain.size())
          continue;

        bool duplicate = true; //reset flag
        for (unsigned k=0; k<cvlet->edgel_chain.size(); k++)
          duplicate = duplicate && (cvlet->edgel_chain[k]==dup_cvlet->edgel_chain[k]);

        if (duplicate){
          cv_it2--;
          curvelets.remove(dup_cvlet);
        }
      }
    }
  }

};

typedef std::list<sdet_link*> sdet_link_list;
typedef std::list<sdet_link*>::iterator sdet_link_list_iter;

//: This class represents the link graph formed from the edgels
//  The links are described by an adjacency list organized by the id of the
//  edgel
class sdet_edgel_link_graph
{
public:
  std::vector<sdet_link_list> cLinks; ///< child links
  std::vector<sdet_link_list> pLinks; ///< parent links
  std::vector<bool> linked;            ///< flag to signal whether this edgel has been linked already

  //temporary link graph created for finding selected edgel chains
  std::vector<sdet_link_list> cLinks2; ///< child links
  std::vector<sdet_link_list> pLinks2; ///< parent links

  //: constructor
  sdet_edgel_link_graph(int size=0): cLinks(size), pLinks(size), linked(size), cLinks2(size), pLinks2(size){}

  //: destructor
  ~sdet_edgel_link_graph()
  {
    clear();//delete everything upon exit
  }

  //: resize the graph
  void resize(unsigned size)
  {
    if (size!=cLinks.size())
      clear();

    cLinks.resize(size);
    pLinks.resize(size);
    linked.resize(size);

    cLinks2.resize(size);
    pLinks2.resize(size);
  }

  //: clear the graph
  void clear()
  {
    //delete all the links
    for (auto & cLink : cLinks){
      sdet_link_list_iter l_it = cLink.begin();
      for (;l_it!=cLink.end(); l_it++)
        delete (*l_it);
    }

    cLinks.clear();
    pLinks.clear();
    linked.clear();

    cLinks2.clear();
    pLinks2.clear();
  }

  void clear_linked_flag()
  {
    linked.clear();
    linked.resize(cLinks.size());
  }

  //utility functions
  bool is_an_end(int id)  {return cLinks[id].size()==0 || pLinks[id].size()==0; }
  bool is_a_bifurc(int id){return cLinks[id].size()==2;}
  bool is_a_split(int id) {return cLinks[id].size()>1;}
  bool is_a_merge(int id) {return pLinks[id].size()>1;}
  bool is_a_junct(int id) {return cLinks[id].size()>1 || pLinks[id].size()>1;} //X-junction

  //: is this edgel legal to start a one-chain
  bool edgel_is_legal_first_edgel(sdet_edgel* eA)
  {
    return (!linked[eA->id] &&            //not yet part of any edgel chain
            (pLinks[eA->id].size()==1 &&   //one link before it and
             cLinks[eA->id].size()==1) &&  //one link after it
             pLinks[eA->id].front()->pe != cLinks[eA->id].front()->ce //not a loop
           );
  }

  //: is this edgel legal to be on an edgel chain
  bool edgel_is_legal(sdet_edgel* eA)
  {
    return (!linked[eA->id] &&            //not yet part of any edgel chain
             pLinks[eA->id].size()<=1 &&   //one link before it (or endpoint) and
             cLinks[eA->id].size()<=1      //one link after it (or endpoint)
           );
  }

  //: check if edgels are linked,
  //  if they are linked, return the link
  sdet_link* are_linked(sdet_edgel* e1, sdet_edgel* e2)
  {
    sdet_link_list_iter cit = cLinks[e1->id].begin();
    for(; cit!=cLinks[e1->id].end(); cit++){
      if ((*cit)->ce == e2)
        return (*cit); //return link
    }
    sdet_link_list_iter cit1 = cLinks[e2->id].begin();
    for(; cit1!=cLinks[e2->id].end(); cit1++){
      if ((*cit1)->ce == e1)
        return (*cit1); //return link
    }
    return nullptr; //not linked
  }

  //: link edgels if not already linked
  void link(sdet_edgel* e1, sdet_edgel* e2, sdet_curvelet* cvlet)
  {
    sdet_link* link = are_linked(e1, e2);
    if (!link){
      //create a link
      sdet_link* new_link = new sdet_link(e1, e2);

      cLinks[e1->id].push_back(new_link);
      pLinks[e2->id].push_back(new_link);

      //also add a vote
      new_link->vote += 1.0;
      new_link->curvelets.push_back(cvlet);
    }
    else {
      //link already exists so simply add to the vote count
      link->vote += 1.0;

      //and add the cvlet that gave rise to this link
      link->curvelets.push_back(cvlet);
    }
  }

  //: remove the link between these edgels
  void remove_link(sdet_link* link)
  {
    pLinks[link->ce->id].remove(link);
    cLinks[link->pe->id].remove(link);

    delete link;
  }

  //: move a link from the first link graph to the second
  void move_link(sdet_link* link)
  {
    pLinks[link->ce->id].remove(link);
    cLinks[link->pe->id].remove(link);

    pLinks2[link->ce->id].push_back(link);
    cLinks2[link->pe->id].push_back(link);
  }
};

#endif // sdet_sel_base_h
