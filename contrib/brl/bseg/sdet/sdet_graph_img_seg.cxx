#include "sdet_graph_img_seg.h"
#include "sdet_graph_img_seg_sptr.h"
//
#include <vcl_cstdlib.h> // for std::rand()
#include <vcl_iostream.h>
#include <vcl_algorithm.h>
#include <vcl_cassert.h>
#include <vil/vil_load.h>


// constructor - provide a margin around the image to be left out from the graph
sdet_graph_img_seg::sdet_graph_img_seg(unsigned ni, unsigned nj, unsigned margin, int neigh)
{
  pixel_ids_.set_size(ni, nj);
  pixel_ids_.fill(-1);
  int inner_i = ni-margin;
  int inner_j = nj-margin;
  node_cnt_ = 0;
  for (int i = margin; i < inner_i; i++) 
    for (int j = margin; j < inner_j; j++) {
      pixel_ids_(i,j) = node_cnt_;
      id_to_pix_[node_cnt_] = vcl_pair<unsigned, unsigned>(i,j);
      node_cnt_++;
    } 

  // create edges "to the right"
  for (int i = margin; i < inner_i-1; i++) 
    for (int j = margin; j < inner_j; j++) {
      vbl_edge e(pixel_ids_(i, j), pixel_ids_(i+1,j), -1.0f);
      edges_.push_back(e);
    }
  
  // create edges "to below"
  for (int i = margin; i < inner_i; i++) 
    for (int j = margin; j < inner_j-1; j++) {
      vbl_edge e(pixel_ids_(i, j), pixel_ids_(i,j+1), -1.0f);
      edges_.push_back(e);
    }

  if (neigh == 8) { // for now only two options (4 or 8 neighborhood) - if 8-neighborhood add more edges
    // create edges "to down-right"
    for (int i = margin; i < inner_i-1; i++) 
      for (int j = margin; j < inner_j-1; j++) {
        vbl_edge e(pixel_ids_(i, j), pixel_ids_(i+1,j+1), -1.0f);
        edges_.push_back(e);
      }
  
    // create edges "to down-left"
    for (int i = margin+1; i < inner_i; i++) 
      for (int j = margin; j < inner_j-1; j++) {
        vbl_edge e(pixel_ids_(i, j), pixel_ids_(i-1,j+1), -1.0f);
        edges_.push_back(e);
      }

  } 

}


