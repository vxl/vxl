// This is mul/vil3d/algo/vil3d_find_blobs.cxx
#include <vector>
#include <iostream>
#include <algorithm>
#include "vil3d_find_blobs.h"
//:
// \file
// \brief Identify and enumerate all disjoint blobs in a binary image.
// \author Ian Scott, Kevin de Souza

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_math.h>

namespace
{
  //: Manages the relabelling structure.
  // Got the idea from http://en.wikipedia.org/wiki/Disjoint-set_data_structure Mar 2011.
  // although no code was copied.
  class disjoint_sets
  {
    typedef unsigned LABEL;
    typedef unsigned LEN;
    struct node
    {
      LABEL parent;
      LEN rank;
    };
    std::vector<node> store_;
  public:
    disjoint_sets(): store_(1u)
    { // renumber 0->0
      store_.front().parent=0;
      store_.front().rank=0;
    }

    //: Get the root label for label v;
    LABEL root(LABEL v)
    {
      node & n=store_[v];
      if (n.parent == v)
        return v;
      else
      {
        n.parent=root(n.parent); // relabel to speed up later searches
        return n.parent;
      }
    }

    //: Merge two sets containing labels left and right.
    void merge_labels(LABEL left_label, LABEL right_label)
    {
      LABEL left_root = root(left_label);
      LABEL right_root = root(right_label);
      if (left_root == right_root) return; // already merged.
      node& left_root_node = store_[left_root];
      node& right_root_node = store_[right_root];
      if (left_root_node.rank > right_root_node.rank) // Find the larger tree.
      { // add the right tree to the left
        right_root_node.parent = left_root_node.parent;
      }
      else
      { // add the left tree to the right
        left_root_node.parent = right_root_node.parent;
        if (left_root_node.rank == right_root_node.rank)
          right_root_node.rank++;
      }
    }

    //: Create a new label;
    LABEL new_label()
    {
      node n = {(LABEL)store_.size(), 0};
      store_.push_back(n);
      return n.parent;
    }

    LEN size()
    {
      return store_.size();
    }
  };
}

// Identify and enumerate all disjoint blobs in a binary image.
void vil3d_find_blobs(const vil3d_image_view<bool>& src,
                      vil3d_find_blob_connectivity conn,
                      vil3d_image_view<unsigned>& dst)
{
  unsigned ni=src.ni();
  unsigned nj=src.nj();
  unsigned nk=src.nk();
  dst.set_size(ni, nj, nk);
  dst.fill(0);

  disjoint_sets merge_list;
  std::vector<unsigned> neighbouring_labels;

  unsigned n_neighbours;
  switch (conn)
  {
   case vil3d_find_blob_connectivity_6_conn:
    n_neighbours=3;
    break;
   case vil3d_find_blob_connectivity_26_conn:
    n_neighbours=13;
    break;
   default:
    n_neighbours=0; assert(!"unknown connectivity");
  }

  // The 3-prev-(6)-neighbourhood are the first three entries.
  // The 13-prev-(26)-neighbourhood are those three plus the rest.
  int neighbourhood_ii[] = { -1, 0, 0, -1,  0, +1, -1, +1, -1,  0, +1, -1, +1};
  int neighbourhood_jj[] = { 0, -1, 0, -1, -1, -1,  0,  0, +1, +1, +1, -1, -1};
  int neighbourhood_kk[] = { 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0,  0};


  for (unsigned k=0; k<nk; ++k)
    for (unsigned j=0; j<nj; ++j)
      for (unsigned i=0; i<ni; ++i)
      {
        if (!src(i,j,k)) continue;
        neighbouring_labels.clear();
        for (unsigned l=0; l<n_neighbours; ++l)
        {
          unsigned ii = i + neighbourhood_ii[l];
          if (ii >= ni) continue; // rely on wraparound to find -ne overruns.
          unsigned jj = j + neighbourhood_jj[l];
          if (jj >= nj) continue;
          unsigned kk = k + neighbourhood_kk[l];
          if (kk >= nk) continue;
          unsigned d = dst(ii,jj,kk);
          if (d==0) continue;
          neighbouring_labels.push_back(d);
        }
        if (neighbouring_labels.empty())
        {
          unsigned new_label = merge_list.new_label();
          dst(i,j,k) = new_label;
        }
        else
        {
          // See how many unique labels neighbouring labels we have
          std::sort(neighbouring_labels.begin(), neighbouring_labels.end());
          auto end = std::unique(neighbouring_labels.begin(), neighbouring_labels.end());
          // don't bother erasing unique suffix, just keeping the end iterator
          // will be enough.
          auto it=neighbouring_labels.begin();
          unsigned label = *it++;
          dst(i,j,k) = label;

          // If we have neighbours with multiple labels.
          //   then record merges of the previously disjointly labelled blocks.
          // If there was only a single unique label, the following for loop
          //   will not execute.
          for (; it!=end; ++it)
            merge_list.merge_labels(*it, label);
        }
      }

  unsigned n_merge=merge_list.size();
  std::vector<unsigned> renumbering(n_merge, 0u);
  // Convert the merge lsit into a simple renumbering array,
  // and change to root of each disjoint set to its lowest member.
  // The reinstates label order to the original raster order.
  for (unsigned l=1; l<n_merge; ++l)
  {
    if (renumbering[l]!=0) continue;
    unsigned root_label = merge_list.root(l);
    unsigned root_label_renumber = renumbering[root_label];
    if (root_label_renumber==0)
    {
      renumbering[root_label]=l;
      renumbering[l]=l;
    }
    else
      renumbering[l]=renumbering[root_label];
  }

  // Now due to the renumbering, the set of labels may not compactly occupy
  // the number line. So renumber the renumbering array.
  std::vector<unsigned> labels(renumbering.begin(), renumbering.end());
  std::sort(labels.begin(), labels.end());
  labels.erase(std::unique(labels.begin(), labels.end()), labels.end());
  const auto dodgy = static_cast<unsigned>(-1);
  std::vector<unsigned> renum_renum(renumbering.size(), dodgy);
  renum_renum[0]=0;
  for (unsigned l=0, n=labels.size(); l<n; ++l)
    renum_renum[labels[l]] = l;

  for (unsigned int & it : renumbering)
    it=renum_renum[it];

  // Check than no DODGY values got into the renumbering.
  assert(std::find(renumbering.begin(), renumbering.end(), dodgy)
    == renumbering.end() );

  // Renumber the labels, to merge connected blobs, with a compact set of labels.

  for (unsigned k=0; k<nk; ++k)
    for (unsigned j=0; j<nj; ++j)
      for (unsigned i=0; i<ni; ++i)
        dst(i,j,k) = renumbering[dst(i,j,k)];
}



//: Convert a label image into a list of chorded regions.
// A blob label value of n will be returned in dest_regions[n-1].
void vil3d_blob_labels_to_regions(const vil3d_image_view<unsigned>& src_label,
                                std::vector<vil3d_region>& regions)
{
  regions.clear();
  unsigned ni=src_label.ni();
  unsigned nj=src_label.nj();
  unsigned nk=src_label.nk();

  unsigned min_v,max_v;
  vil3d_math_value_range(src_label,min_v,max_v);
  if (max_v==0) return;  // No blobs.

  regions.resize(max_v);

  for (unsigned k=0; k<nk; ++k)
   for (unsigned j=0; j<nj; ++j)
    for (unsigned i=0; i<ni;) // don't auto increment i, since the loop body does it.
    {
      unsigned label = src_label(i,j,k);
      if (!label)
      { // if not a label - go to next pixel.
        ++i;
        continue;
      }
      unsigned i_start=i;
      // Find end of chord.
      while (++i < ni && src_label(i,j,k)==label);
      regions[label-1].push_back(vil3d_chord(i_start, i-1, j,k));
    }
}
