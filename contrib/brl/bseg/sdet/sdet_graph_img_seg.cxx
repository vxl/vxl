#include <cstdlib>
#include <iostream>
#include <algorithm>
#include "sdet_graph_img_seg.h"
#include "sdet_graph_img_seg_sptr.h"
//
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vil/vil_load.h>
#include <vil/vil_math.h>


// constructor - provide a margin around the image to be left out from the graph
sdet_graph_img_seg::sdet_graph_img_seg(unsigned ni, unsigned nj, unsigned margin, int neigh)
{
  pixel_ids_.set_size(ni, nj);
  pixel_ids_.fill(-1);
  int inner_i = ni-margin;
  int inner_j = nj-margin;
  id_to_pix_.resize((ni-2*margin)*(nj-2*margin));
  node_cnt_ = 0;
  for (int i = margin; i < inner_i; i++)
    for (int j = margin; j < inner_j; j++) {
      pixel_ids_(i,j) = node_cnt_;
      id_to_pix_[node_cnt_] = std::pair<unsigned, unsigned>(i,j);
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

void sdet_graph_img_seg::create_colors(std::vector<vil_rgb<vxl_byte> >& colors, int n_segments)
{
  /*// create random colors for each set
  vnl_random rng;
  for (int i = 0; i < n_segments; i++) {
    vil_rgb<vxl_byte> c((vxl_byte)(rng.drand32()*255), (vxl_byte)(rng.drand32()*255), (vxl_byte)(rng.drand32()*255));
    colors.push_back(c);
  }*/
  // create a unique color for each segment
  int r_init = 0, b_init = 255, g_init = 128;
  int r_cnt = r_init, b_cnt = b_init, g_cnt = g_init, increment = 20;
  for (int i = 0; i < n_segments; i++) {
    vil_rgb<vxl_byte> c((vxl_byte)r_cnt, (vxl_byte)g_cnt, (vxl_byte)b_cnt);
    colors.push_back(c);
    if (i%3 == 0) {
      r_cnt += increment;
      if (r_cnt > 255)
        r_cnt = ++r_init;
    } else if (i%3 == 1) {
      b_cnt -= increment;
      if (b_cnt < 0)
        b_cnt = --b_init;
    } else if (i%3 == 2) {
      g_cnt += increment;
      if (g_cnt > 255)
        g_cnt = ++g_init;
      if (g_init > 255)
        g_init = 0;
    }
  }

}

// segment an image using two features, takes two normalized feature images as dimension 1 and dimension 2, calculates Euclidean distance
// only works for float images in [0,1]
// use sigma1 to smooth the first image and sigma2 to smooth the second image if needed, pass 0 if smoothing is not necessary
void sdet_segment_img2(vil_image_view<float> const& img1, vil_image_view<float> const& img2, unsigned margin, int neigh, float weight_thres, float sigma1, float sigma2, int min_size, vil_image_view<vil_rgb<vxl_byte> >& out_img)
{
  // check if the input images are of the same size
  if (img2.ni() != img1.ni() || img2.nj() != img1.nj()) {
    std::cerr << "Input height image does not have the same size as the input image to be segmented!\n";
    return;
  }

  auto* ss = new sdet_graph_img_seg(img1.ni(), img1.nj(), margin, neigh);

  // smooth the image
  vil_image_view<float> smoothed1, smoothed2;

  if (sigma1 <= 0)
    smoothed1 = vil_copy_deep(img1);
  else {
    // smooth source image using gaussian filter
    vil_gauss_filter_5tap_params gauss_params(sigma1);
    vil_gauss_filter_5tap<float, float>(img1, smoothed1, gauss_params);
  }

  if (sigma2 <= 0)
    smoothed2 = vil_copy_deep(img2);
  else {
    // smooth source image using gaussian filter
    vil_gauss_filter_5tap_params gauss_params(sigma2);
    vil_gauss_filter_5tap<float, float>(img2, smoothed2, gauss_params);
  }


  // set up the edge costs as difference in color and height space
  std::vector<vbl_edge>& edges = ss->get_edges();
  for (auto & edge : edges) {
    std::pair<unsigned, unsigned> pix0 = ss->get_pixel(edge.v0_);
    auto c0 = (double)smoothed1(pix0.first, pix0.second);
    std::pair<unsigned, unsigned> pix1 = ss->get_pixel(edge.v1_);
    auto c1 = (double)smoothed1(pix1.first, pix1.second);
    double dif = std::abs(c1-c0);

    auto h0 = (double)smoothed2(pix0.first, pix0.second);
    auto h1 = (double)smoothed2(pix1.first, pix1.second);
    double dif2 = std::abs(h0-h1);

    edge.w_ = (float)std::sqrt(dif*dif + dif2*dif2);
    /*if (dif < dif2)
      edges[i].w_ = (float)std::sqrt(dif*dif);
    else
      edges[i].w_ = (float)std::sqrt(dif2*dif2);*/
  }

  vbl_disjoint_sets ds;
  ds.add_elements(ss->node_cnt());

  // segment graph
  vbl_graph_partition(ds, edges, weight_thres);

  // combine the segments with number of elements less than min_size
  // post process small components
  for (auto & edge : edges)
    {
    int v0 = ds.find_set(edge.v0_);
    int v1 = ds.find_set(edge.v1_);
    if ((v0 != v1) && ((ds.size(v0) < min_size) || (ds.size(v1) < min_size)))
      ds.set_union(v0, v1);
    }
  std::cout << " segmentation resulted in " << ds.num_sets() << " segments!\n";

  out_img.set_size(img1.ni(), img1.nj());
  out_img.fill(vil_rgb<vxl_sbyte>(0,0,0));

  int n_segments = ds.num_elements();  // the number of colors need to be in the number of initial number of nodes, cause the final segment ids are the ids of the nodes
  std::vector<vil_rgb<vxl_byte> > colors;
  // create unique colors for each set
  sdet_graph_img_seg::create_colors(colors, n_segments);

  for (unsigned i = 0; i<ss->node_cnt(); i++) {
    int comp = ds.find_set(i);
    std::pair<unsigned, unsigned> pix = ss->get_pixel(i);
    out_img(pix.first, pix.second) = colors[comp];
  }

  delete ss;
}

//: segment using two features and also respect the edges in the edge image
void sdet_segment_img2_using_edges(vil_image_view<float> const& img1, vil_image_view<float> const& img2, vil_image_view<float> const& edge_img, unsigned margin, int neigh, float weight_thres, float sigma1, float sigma2, int min_size, vil_image_view<vil_rgb<vxl_byte> >& out_img)
{
    // check if the input images are of the same size
  if (img2.ni() != img1.ni() || img2.nj() != img1.nj()) {
    std::cerr << "Input height image does not have the same size as the input image to be segmented!\n";
    return;
  }

  auto* ss = new sdet_graph_img_seg(img1.ni(), img1.nj(), margin, neigh);

  // smooth the image
  vil_image_view<float> smoothed1, smoothed2;

  if (sigma1 <= 0)
    smoothed1 = vil_copy_deep(img1);
  else {
    // smooth source image using gaussian filter
    vil_gauss_filter_5tap_params gauss_params(sigma1);
    vil_gauss_filter_5tap<float, float>(img1, smoothed1, gauss_params);
  }

  if (sigma2 <= 0)
    smoothed2 = vil_copy_deep(img2);
  else {
    // smooth source image using gaussian filter
    vil_gauss_filter_5tap_params gauss_params(sigma2);
    vil_gauss_filter_5tap<float, float>(img2, smoothed2, gauss_params);
  }

  // set up the edge costs as difference in color and height space
  std::vector<vbl_edge>& edges = ss->get_edges();
  for (auto & edge : edges) {
    std::pair<unsigned, unsigned> pix0 = ss->get_pixel(edge.v0_);
    auto c0 = (double)smoothed1(pix0.first, pix0.second);
    std::pair<unsigned, unsigned> pix1 = ss->get_pixel(edge.v1_);
    auto c1 = (double)smoothed1(pix1.first, pix1.second);
    double dif = c1-c0;

    auto h0 = (double)smoothed2(pix0.first, pix0.second);
    auto h1 = (double)smoothed2(pix1.first, pix1.second);
    double dif2 = h0-h1;

    edge.w_ = (float)std::sqrt(dif*dif + dif2*dif2);

    auto e0 = (double)edge_img(pix0.first, pix0.second);
    auto e1 = (double)edge_img(pix1.first, pix1.second);
    edge.w_ += ( e0 > e1 ? e0 : e1);
    //edges[i].w_ = (float)std::sqrt(dif*dif + dif2*dif2 + 100*(e0-e1)*(e0-e1));
  }

  vbl_disjoint_sets ds;
  ds.add_elements(ss->node_cnt());

  // segment graph
  vbl_graph_partition(ds, edges, weight_thres);

  // combine the segments with number of elements less than min_size
  // post process small components
  for (auto & edge : edges)
    {
    int v0 = ds.find_set(edge.v0_);
    int v1 = ds.find_set(edge.v1_);
    if ((v0 != v1) && ((ds.size(v0) < min_size) || (ds.size(v1) < min_size)))
      ds.set_union(v0, v1);
    }
  std::cout << " segmentation resulted in " << ds.num_sets() << " segments!\n";

  out_img.set_size(img1.ni(), img1.nj());
  out_img.fill(vil_rgb<vxl_sbyte>(0,0,0));

  int n_segments = ds.num_elements();  // the number of colors need to be in the number of initial number of nodes, cause the final segment ids are the ids of the nodes
  std::vector<vil_rgb<vxl_byte> > colors;
  // create unique colors for each set
  sdet_graph_img_seg::create_colors(colors, n_segments);

  for (unsigned i = 0; i<ss->node_cnt(); i++) {
    int comp = ds.find_set(i);
    std::pair<unsigned, unsigned> pix = ss->get_pixel(i);
    out_img(pix.first, pix.second) = colors[comp];
  }

  delete ss;
}
