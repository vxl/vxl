//:
// \file
// \brief Finds connected regions in a boolean image.
// \author Ian Scott

#include <algorithm>
#include "vil_blob.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

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

// Produce a label image that enumerates all disjoint blobs in a binary image
void vil_blob_labels(const vil_image_view<bool>& src_binary,
                     vil_blob_connectivity conn,
                     vil_image_view<unsigned>& dest_label)
{
  unsigned ni=src_binary.ni();
  unsigned nj=src_binary.nj();
  dest_label.set_size(ni, nj);
  dest_label.fill(0);

  disjoint_sets merge_list;
  std::vector<unsigned> neighbouring_labels;

  unsigned n_prev_neighbours;
  switch (conn)
  {
   case vil_blob_4_conn:
    n_prev_neighbours=2;
    break;
   case vil_blob_8_conn:
    n_prev_neighbours=4;
    break;
   default:
    n_prev_neighbours=0; assert(!"unknown connectivity");
  }

  // The 2-prev-(of 6)-neighbourhood are the first two entries.
  // The 4-prev-(of 8)-neighbourhood are those two plus the rest.
  int neighbourhood_ii[] = { -1,  0, -1, +1};
  int neighbourhood_jj[] = {  0, -1, -1, -1};


  for (unsigned j=0; j<nj; ++j)
    for (unsigned i=0; i<ni; ++i)
    {
      if (!src_binary(i,j)) continue;
      neighbouring_labels.clear();
      for (unsigned l=0; l<n_prev_neighbours; ++l)
      {
        unsigned ii = i + neighbourhood_ii[l];
        if (ii >= ni) continue; // rely on wraparound to find -ne overruns.
        unsigned jj = j + neighbourhood_jj[l];
        if (jj >= nj) continue;
        unsigned d = dest_label(ii,jj);
        if (d!=0) neighbouring_labels.push_back(d);
      }
      if (neighbouring_labels.empty())
      {
        unsigned new_label = merge_list.new_label();
        dest_label(i,j) = new_label;
      }
      else
      {
        // See how many unique labels neighbouring labels we have
        std::sort(neighbouring_labels.begin(), neighbouring_labels.end());
        auto end = std::unique(neighbouring_labels.begin(), neighbouring_labels.end());
        // don't bother erasing unique's suffix, just keeping the end iterator
        // will be enough.
        auto it=neighbouring_labels.begin();
        unsigned label = *it++;
        dest_label(i,j) = label;

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

  for (unsigned j=0; j<nj; ++j)
    for (unsigned i=0; i<ni; ++i)
      dest_label(i,j) = renumbering[dest_label(i,j)];
}


//: Set all non-blob-edge pixels in a blob label image to zero.
void vil_blob_labels_to_edge_labels(const vil_image_view<unsigned>& src_label,
                                    vil_blob_connectivity conn,
                                    vil_image_view<unsigned>& dest_label)
{
  unsigned ni=src_label.ni();
  unsigned nj=src_label.nj();
  dest_label.set_size(ni, nj);
  dest_label.fill(0);

  unsigned n_edge_neighbours;
  switch (conn)
  {
   case vil_blob_4_conn:
    n_edge_neighbours=8;
    break;
   case vil_blob_8_conn:
    n_edge_neighbours=4;
    break;
   default:
    n_edge_neighbours=0; assert(!"unknown connectivity");
  }

  // A  4-conn blob pixel is on the edge if any of its 8-conn neighbours has different value.
  // An 8-conn blob pixel is on the edge if any of its 4-conn neighbours has different value.
  int neighbourhood_ii[] = {  0, -1,  1,  0,  -1,  1, -1,  1};
  int neighbourhood_jj[] = { -1,  0,  0,  1,  -1, -1,  1,  1};

  for (unsigned j=0; j<nj; ++j)
    for (unsigned i=0; i<ni; ++i)
    {
      unsigned v = src_label(i,j);
      if (!v) continue;
      for (unsigned l=0; l<n_edge_neighbours; ++l)
      {
        unsigned ii = i + neighbourhood_ii[l];
        if (ii >= ni) continue; // rely on wraparound to find -ne overruns.
        unsigned jj = j + neighbourhood_jj[l];
        if (jj >= nj) continue;
        if (v!=src_label(ii,jj)) // Only pixels that have neighbours with different values are edge pixels.
        {
          dest_label(i,j) = v;
          break;
        }
      }
    }
}


//: Convert a label image into a list of chorded regions.
// A blob label value of n will be returned in dest_regions[n-1].
void vil_blob_labels_to_regions(const vil_image_view<unsigned>& src_label,
                                std::vector<vil_blob_region>& dest_regions)
{
  dest_regions.clear();
  unsigned ni=src_label.ni();
  unsigned nj=src_label.nj();

  for (unsigned j=0; j<nj; ++j)
    for (unsigned i=0; i<ni;) // don't auto increment i, since the loop body does it.
    {
      unsigned label = src_label(i,j);
      if (!label)
      { // if not a label - go to next pixel.
        ++i;
        continue;
      }
      // Make sure there is a region for this label.
      if (label > dest_regions.size()) dest_regions.resize(label);
      unsigned i_start=i;
      // Find end of chord.
      while (++i < ni && src_label(i,j)==label);
      dest_regions[label-1].push_back(vil_chord(i_start, i-1, j));
    }
}

//: Convert a label image (or an edge label image) into a set of pixel lists.
// A blob label value of n will be returned in dest_pixels_lists[n-1].
// Note that pixel lists are not ordered.
void vil_blob_labels_to_pixel_lists(const vil_image_view<unsigned>& src_label,
                                    std::vector<vil_blob_pixel_list>& dest_pixel_lists)
{
  dest_pixel_lists.clear();
  unsigned ni=src_label.ni();
  unsigned nj=src_label.nj();

  for (unsigned j=0; j<nj; ++j)
    for (unsigned i=0; i<ni; ++i)
    {
      unsigned label = src_label(i,j);
      if (!label) continue;
      // Make sure there is a pixel list for this label.
      if (label > dest_pixel_lists.size()) dest_pixel_lists.resize(label);
      dest_pixel_lists[label-1].push_back(std::make_pair(i,j));
    }
}
