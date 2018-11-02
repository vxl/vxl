#include <utility>
#include <iostream>
#include "brip_label_equivalence.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: add a label pair equivalence
void brip_label_equivalence::add_label_pair(unsigned la, unsigned lb)
{
  std::pair<std::set<unsigned>::iterator, bool> result;
  result = forward_pairs_[la].insert(lb);
  if (result.second)
    if (la>max_label_)
      max_label_ = la;
  result =  reverse_pairs_[lb].insert(la);
  if (result.second)
    if (la>max_label_)
      max_label_ = lb;
}
//: find all the individual labels and determine the largest label
std::set<unsigned> brip_label_equivalence::labels() const
{
  std::set<unsigned> labs;
  auto mit =
    forward_pairs_.begin();
  for (; mit != forward_pairs_.end(); ++mit) {
    labs.insert((*mit).first);
  }
  mit = reverse_pairs_.begin();
  for (; mit != reverse_pairs_.end(); ++mit) {
    labs.insert((*mit).first);
  }
  return labs;
}

bool brip_label_equivalence::
merge_equivalence(std::map<unsigned int, std::set<unsigned int> >& tab,
                  unsigned int cur_label,
                  unsigned int label)
{
  std::map<unsigned , std::set<unsigned> >::iterator hashi;
  //If we can't find the label in tab then there are no equivalences to be merged
  hashi = tab.find(label);
  if (hashi == tab.end()) {
    return false;
  }
  //We did find label, and labels is a set of equivalent labels
  std::set<unsigned>& labels = hashi->second;

  //If the set is empty then nothing to do
  unsigned len = labels.size();
  if (!len)
    return false;

  hashi = equivalence_sets_.find(cur_label);
  if ( hashi == equivalence_sets_.end())
  {
    //We didn't find any equivalent labels for cur_label so we initialize a new one
    //and insert it into equivalence_sets
    equivalence_sets_[cur_label] = std::set<unsigned>();
  }

  for (const auto & label : labels)
    equivalence_sets_[cur_label].insert(label);
  return true;
}

//----------------------------------------------------------------
//: Find the next label not accounted for in the current equivalence set.
//  The set of labels is searched to find a label larger than label, but
//  not in the set, labels.
bool brip_label_equivalence::get_next_label(std::set<unsigned> const& labels,
                                            unsigned int& label)
{
  //If the set labels is null then
  //just return the next larger label (if less than max_region_label_)
  unsigned int tmp = label+1;
  if (!labels.size())
    if (tmp<max_label_)
    {
      label = tmp;
      return true;
    }

  //The set is not empty, so search for a label not found in the
  //set.
  for (unsigned int i = tmp; i<=max_label_; i++)
  {
    auto sit=labels.find(i);
    if (sit==labels.end()){
      label = i;
      return true;
    }
  }
  return false;
}


void brip_label_equivalence::transitive_closure()
{
  std::set<unsigned> labs = this->labels();
  if (labs.size()<=1)
    return;
  unsigned cur_label = *(labs.begin());

  //the iterator for equivalence_sets_
  std::map<unsigned , std::set<unsigned > >::iterator mei;
  while (true)
  {
    bool merging = true;
    unsigned i = cur_label;
    std::set<unsigned> cur_set;
    int len = 0;
    int old_len;
    while (merging)
    {
      old_len = len;
      merging = false;
      //find label equivalence in the forward map
      bool find_forward =
        this->merge_equivalence(forward_pairs_, cur_label, i);
      std::map<unsigned, std::set<unsigned> >::iterator sit;
      if (find_forward)
      {
        sit = forward_pairs_.find(i);
        if (sit!=forward_pairs_.end())
          forward_pairs_.erase(sit);
      }
#if 0
      if (find_forward)
        std::cout << "merged forward pairs on label " << i << '\n' << std::flush;
#endif
      //find label equivalence in the reverse map
      bool find_reverse =
        this->merge_equivalence(reverse_pairs_, cur_label, i);
      if (find_reverse)
      {
        sit = reverse_pairs_.find(i);
        if (sit!=reverse_pairs_.end())
          reverse_pairs_.erase(sit);
      }
#if 0
      if (find_reverse)
        std::cout << "merged reverse pairs on label " << i << '\n' << std::flush;
#endif
      //At this point we may have established or added to the equivalence set
      //for cur_label stored in equivalence_sets[cur_label]
      //we check if we have
      mei = equivalence_sets_.find(cur_label);
      if (mei == equivalence_sets_.end()) continue;
      //If we don't find cur_label, it means that we couldn't find label i
      //in either forward_pairs or reverse_pairs, so we need
      //to get a new i.

      //We did find a growing equivalence set, cur_set, but it might be null
      //or empty
      cur_set =equivalence_sets_[cur_label];
      len = cur_set.size();
      if (!len) continue;

      //Now we check cur_set has actually been extended.

      if (len > old_len)  i = cur_label;
      // Limit the size of an equivalence class
      if (len > 200)
      {
        merging = false;
        continue;
      }
      //Get the next larger label from cur_set
      //so that we can insert its equivalent labels
      for (auto cit = cur_set.begin();
           cit != cur_set.end() && !merging; ++cit)
        if (*cit>i)
        {
          i = *cit;
          merging = true;
        }
      //If we reach here with merging = false then we have finished the
      //current equivalence class
    }
    //Find the next largest label that isn't in cur_set to seed the
    //next equivalence class
    if (!get_next_label(cur_set, cur_label)) return;
#if 0
    std::cout << "Getting next label to seed equivalence "
             << cur_label << '\n' << std::flush;
#endif
  }
}
