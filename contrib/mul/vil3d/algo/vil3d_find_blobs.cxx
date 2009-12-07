// This is mul/vil3d/algo/vil3d_find_blobs.cxx
#include "vil3d_find_blobs.h"
//:
// \file
// \brief Identify and enumerate all disjoint blobs in a binary image.
// \author Ian Scott, Kevin de Souza

#include <vcl_vector.h>
#include <vcl_cassert.h>
#include <vcl_algorithm.h>

#include <vil3d/vil3d_image_view.h>


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

  vcl_vector<unsigned> renumbering(1u, 0u); // renumber 0->0
  vcl_vector<unsigned> neighbouring_labels;

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

  typedef vcl_vector<unsigned>::iterator ITER;

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
          if (jj >= ni) continue;
          unsigned kk = k + neighbourhood_kk[l];
          if (kk >= nk) continue;
          unsigned d = dst(ii,jj,kk);
          if (d==0) continue;
          neighbouring_labels.push_back(d);
        }
        if (neighbouring_labels.empty())
        {
          unsigned new_label = renumbering.size();
          dst(i,j,k) = new_label;
          renumbering.push_back(new_label);
        }
        else
        {
          // See how many unique labels neighbouring labels we have
          vcl_sort(neighbouring_labels.begin(), neighbouring_labels.end());
          ITER end = vcl_unique(neighbouring_labels.begin(), neighbouring_labels.end());
          // don't bother erasing unique suffix, just keeping the end iterator
          // will be enough.
          ITER it=neighbouring_labels.begin();
          unsigned label = *it++;
          dst(i,j,k) = label;

          // If we have neighbours with multiple labels.
          //   then record merges of the previously disjointly labelled blocks.
          // If there was only a single unique label, the following for loop
          //   will not execute.
          for (; it!=end; ++it)
            renumbering[*it] = vcl_min(renumbering[*it], label);
        }
      }

  // Propagate the renumbering so that when 3 points to 2 and 2 points to 1,
  //   3 should then point to 1.
  for (ITER it=renumbering.begin(), end=renumbering.end(); it!=end; ++it)
    *it = renumbering[*it];

  // Now due to the renumbering, the set of labels may not compactly occupy
  // the number line. So renumber the renumbering array.
  vcl_vector<unsigned> labels(renumbering.begin(), renumbering.end());
  vcl_sort(labels.begin(), labels.end());
  labels.erase(vcl_unique(labels.begin(), labels.end()), labels.end());
  const unsigned dodgy = static_cast<unsigned>(-1);
  vcl_vector<unsigned> renum_renum(renumbering.size(), dodgy);
  renum_renum[0]=0;
  for (unsigned l=0, n=labels.size(); l<n; ++l)
    renum_renum[labels[l]] = l;

  for (ITER it=renumbering.begin(), end=renumbering.end(); it!=end; ++it)
    *it=renum_renum[*it];

  // Check than no DODGY values got into the renumbering.
  assert(vcl_find(renumbering.begin(), renumbering.end(), dodgy)
    == renumbering.end() );

  // Renumber the labels, to merge connected blobs, with a compact set of labels.

  for (unsigned k=0; k<nk; ++k)
    for (unsigned j=0; j<nj; ++j)
      for (unsigned i=0; i<ni; ++i)
        dst(i,j,k) = renumbering[dst(i,j,k)];
}

