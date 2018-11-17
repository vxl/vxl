#include "sdet_curvelet_map.h"

#include "sdet_curvelet.h"
#include "sdet_edgemap.h"

//: constructor
sdet_curvelet_map
::sdet_curvelet_map(const sdet_edgemap_sptr& EM,
                    sdet_curvelet_params params):
  EM_(EM),
  params_(params),
  map_(0)
{}

//: destructor
sdet_curvelet_map
::~sdet_curvelet_map()
{
  clear(); //delete everything upon exit
  EM_=nullptr; //delete the reference to the edgemap
}

//: resize the graph
void
sdet_curvelet_map
::resize(unsigned size)
{
  if (size!=map_.size())
    clear();

  map_.resize(size);
  map2_.resize(size);
}

//: clear the graph
void
sdet_curvelet_map
::clear()
{
  clear_all_curvelets();
  map_.clear();
  map2_.clear();
}

//: clear all the curvelets in the graph
void
sdet_curvelet_map
::clear_all_curvelets()
{
  std::set<sdet_curvelet*> deleted;
  //delete all the curvelets in the map
  for (unsigned i=0; i<map_.size(); i++)
  {
    //delete all the curvelets formed by this edgel
    auto p_it = map_[i].begin();
    for (; p_it != map_[i].end(); p_it++) {
      if (deleted.find(*p_it) == deleted.end()) {
        delete (*p_it);
        deleted.insert(*p_it);
      }
    }

    //delete all the curvelets formed by this edgel
    auto p_it2 = map2_[i].begin();
    for (; p_it2 != map2_[i].end(); p_it2++) {
      if (deleted.find(*p_it2) == deleted.end()) {
        delete (*p_it2);
        deleted.insert(*p_it2);
      }
    }

    map_[i].clear();
    map2_[i].clear();
  }
}

//: add a curvelet to this edgel
void
sdet_curvelet_map
::add_curvelet(sdet_curvelet* curvelet, bool dir)
{
  if (dir)
    map_[curvelet->ref_edgel->id].push_back(curvelet);
  else
    map2_[curvelet->ref_edgel->id].push_back(curvelet);
}

//: remove a curvelet from this edgel
void
sdet_curvelet_map
::remove_curvelet(sdet_curvelet* curvelet)
{
  //go over the list of curvelets of the ref edgel
  auto c_it = map_[curvelet->ref_edgel->id].begin();
  for (; c_it != map_[curvelet->ref_edgel->id].end(); c_it++){
    if (curvelet == (*c_it)){
      map_[curvelet->ref_edgel->id].erase(c_it);
      return;
    }
  }

  //also search the other map if the curvelet has not been found
  auto c_it2 = map2_[curvelet->ref_edgel->id].begin();
  for (; c_it2 != map2_[curvelet->ref_edgel->id].end(); c_it2++){
    if (curvelet == (*c_it2)){
      map2_[curvelet->ref_edgel->id].erase(c_it2);
      return;
    }
  }
}

//: delete all the curvelets formed by this edgel
void
sdet_curvelet_map
::delete_all_curvelets(sdet_edgel* e)
{
  auto p_it = map_[e->id].begin();
  for (; p_it != map_[e->id].end(); p_it++)
    delete (*p_it);

  //delete all the curvelets formed by this edgel
  auto p_it2 = map2_[e->id].begin();
  for (; p_it2 != map2_[e->id].end(); p_it2++)
    delete (*p_it2);

  map_[e->id].clear();
  map2_[e->id].clear();
}

//: does the current curvelet exist?
sdet_curvelet*
sdet_curvelet_map
::does_curvelet_exist(sdet_edgel* e, std::deque<sdet_edgel*> & chain)
{
  //go over all the curvelets of the current size formed by the current edgel
  auto cv_it = map_[e->id].begin();
  for (; cv_it != map_[e->id].end(); cv_it++){
    sdet_curvelet* cvlet = (*cv_it);

    if (cvlet->edgel_chain.size() != chain.size())
      continue;

    bool cvlet_exists = true; //reset flag
    for (unsigned k=0; k<chain.size(); k++)
      cvlet_exists = cvlet_exists && (cvlet->edgel_chain[k]==chain[k]);

    //the flag will remain true only if all the edgels match
    if (cvlet_exists)
      return cvlet; //return matching curvelet
  }

  return nullptr; //curvelet does not exist
}

//: does the given edgel pair exist on the ref edgel?
sdet_curvelet*
sdet_curvelet_map
::find_pair(sdet_edgel* ref, sdet_edgel* eA, sdet_edgel* eB)
{
  //go over all the pairs formed by this edgel
  auto p_it = map_[ref->id].begin();
  for (; p_it != map_[ref->id].end(); p_it++){
    sdet_curvelet* p1 = (*p_it);
    if (p1->edgel_chain.size()==2 &&
        p1->edgel_chain[0]==eA && p1->edgel_chain[1]==eB)
      return p1; //pair found
  }
  return nullptr; //not found
}

//: does the given triplet exist on the ref edgel?
sdet_curvelet*
sdet_curvelet_map
::find_triplet(sdet_edgel* ref, sdet_edgel* eA, sdet_edgel* eB, sdet_edgel* eC)
{
  //go over all the triplets formed by this edgel
  auto t_it = map_[ref->id].begin();
  for (; t_it != map_[ref->id].end(); t_it++){
    sdet_curvelet* t1 = (*t_it);
    if (t1->edgel_chain.size()==3 &&
        t1->edgel_chain[0]==eA && t1->edgel_chain[1]==eB && t1->edgel_chain[2]==eC)
      return t1; //triplet found
  }
  return nullptr; //not found
}

//: return largest curvelet formed at the given edgel
sdet_curvelet*
sdet_curvelet_map
::largest_curvelet(sdet_edgel* e)
{
  unsigned largest_size = 0;
  sdet_curvelet* largest_cvlet = nullptr;

  auto cv_it = map_[e->id].begin();
  for ( ; cv_it!=map_[e->id].end(); cv_it++)
  {
    sdet_curvelet* cvlet = (*cv_it);
    if (cvlet->edgel_chain.size()>largest_size){
      largest_size = cvlet->edgel_chain.size();
      largest_cvlet = cvlet;
    }
  }
  return largest_cvlet;
}
