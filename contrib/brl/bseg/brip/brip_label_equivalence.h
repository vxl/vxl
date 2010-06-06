// This is brl/bseg/brip/brip_label_equivalence.h
#ifndef brip_label_equivalence_h_
#define brip_label_equivalence_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief Maintain label equivalences and determine transitive closure of equivalence classes.
//
// \verbatim
//  Modifications
//   Initial version May 5, 2008
// \endverbatim
//
#include <vcl_map.h>
#include <vcl_set.h>

class brip_label_equivalence
{
 public:
  brip_label_equivalence():max_label_(0){}
  ~brip_label_equivalence(){}

  //:add a label pair equivalence
  void add_label_pair(unsigned la, unsigned lb);

  //:carry out full propagation of label equivalence
  void transitive_closure();

  //:the full set of labels
  vcl_set<unsigned> labels() const;

  //:the set of labels equivalent to a given label
  vcl_map<unsigned, vcl_set<unsigned> >& equivalence_sets()
    { return equivalence_sets_;}

 protected:
  unsigned max_label_;
  bool get_next_label(vcl_set<unsigned> const& labels, unsigned int& label);

  bool merge_equivalence(vcl_map<unsigned int, vcl_set<unsigned int> >& tab,
                         unsigned int cur_label,
                         unsigned int label);
  vcl_map<unsigned, vcl_set<unsigned> > forward_pairs_;
  vcl_map<unsigned, vcl_set<unsigned> > reverse_pairs_;
  vcl_map<unsigned, vcl_set<unsigned> > equivalence_sets_;
};

#endif // brip_label_equivalence_h_
