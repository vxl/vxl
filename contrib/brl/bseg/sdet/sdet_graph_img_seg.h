#ifndef sdet_graph_img_seg_h_
#define sdet_graph_img_seg_h_
//:
// \file
// \brief  A class to set up a graph using an image - this class aids segmentation algorithms which assigns various weights to the edges of the graph
// \author Ozge C. Ozcanli
// \date   July 9, 2013
//
// given a new image, prepare it to use Pedro Felzenszwalb's graph-based segmentation algorithm
// (International Journal of Computer Vision, Vol. 59, No. 2, September 2004) to compute segments in the image
// the nodes are the pixels in the image
// the edges are constructed by X-neighborhood of a pixel (.e.g for now only 8 or 4 neighborhood - in the future may have other types of neighborhoods)
// the edge weights will be computed later using the methods provided by this class - weight represents the "dissimilarity" of its nodes (so higher weight edges will be removed)
// the eventual segmentation algorithm finds a set of connected components in the graph by breaking up certain edges and keeping others
//  a static method that segments grey scale images using color is provided
//  each pixel/node is assigned to a connected component (segment) as a result
//

#include <vbl/vbl_ref_count.h>
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_iosfwd.h>
#include <vcl_string.h>
#include <vil/vil_image_view.h>
#include <vcl_utility.h>
#include <vbl/vbl_edge.h>
#include <vbl/vbl_graph_partition.h>

class sdet_graph_img_seg : public vbl_ref_count
{
 public:
   // constructor - provide a margin around the image to be left out from the graph. if neigh = 4 use 4-neighborhood of a pixel, otherwise 8
   sdet_graph_img_seg(unsigned ni, unsigned nj, unsigned margin, int neigh);

   int get_id(unsigned i, unsigned j) { return pixel_ids_(i,j); }
   vcl_pair<unsigned, unsigned> get_pixel(unsigned id) { return id_to_pix_[id]; }

   vcl_vector<vbl_edge>& get_edges() { return edges_; }
   unsigned node_cnt() { return node_cnt_; }
      
 protected:
   vil_image_view<int> pixel_ids_;
   vcl_map<int, vcl_pair<unsigned, unsigned> > id_to_pix_;
   vcl_vector<vbl_edge> edges_;
   unsigned node_cnt_;

};

#include <vil/vil_copy.h>
#include <vil/algo/vil_gauss_filter.h>
#include <vnl/vnl_random.h>

// segment a single plane image using value difference of neighbor pixels, neigh = 4 for 4-neighborhood, or 8
template <class T> 
void sdet_segment_img(vil_image_view<T> const& img, unsigned margin, int neigh, T weight_thres, float sigma, int min_size, vil_image_view<vil_rgb<vxl_byte> >& out_img)
{
  sdet_graph_img_seg* ss = new sdet_graph_img_seg(img.ni(), img.nj(), margin, neigh);

  // smooth the image
  vil_image_view<T> smoothed;
  
  if (sigma <= 0) 
    smoothed = vil_copy_deep(img);
  else {
    // smooth source image using gaussian filter
    vil_gauss_filter_5tap_params gauss_params(sigma);
    vil_gauss_filter_5tap<T, T>(img, smoothed, gauss_params);
  }

  // set up the edge costs as color differences
  vcl_vector<vbl_edge>& edges = ss->get_edges();
  for (unsigned i = 0; i < edges.size(); i++) {
    vcl_pair<unsigned, unsigned> pix0 = ss->get_pixel(edges[i].v0_);
    double c0 = (double)smoothed(pix0.first, pix0.second);
    vcl_pair<unsigned, unsigned> pix1 = ss->get_pixel(edges[i].v1_);
    double c1 = (double)smoothed(pix1.first, pix1.second);
    double dif = c1-c0;
    edges[i].w_ = (float)vcl_sqrt(dif*dif);
  }
    
  vbl_disjoint_sets ds; 
  ds.add_elements(ss->node_cnt());
  
  // segment graph
  vbl_graph_partition(ds, edges, weight_thres);
  
  // combine the segments with number of elements less than min_size
  // post process small components
  for (int i = 0; i < edges.size(); i++) {
      int v0 = ds.find_set(edges[i].v0_);
      int v1 = ds.find_set(edges[i].v1_);
      if ((v0 != v1) && ((ds.size(v0) < min_size) || (ds.size(v1) < min_size)))
          ds.set_union(v0, v1);
  }
  vcl_cout << " segmentation resulted in " << ds.num_sets() << " segments!n";

  out_img.set_size(img.ni(), img.nj());
  out_img.fill(vil_rgb<vxl_sbyte>(0,0,0));

  int n_segments = ds.num_elements();  // the number of colors need to be in the number of initial number of nodes, cause the final segment ids are the ids of the nodes
  vcl_vector<vil_rgb<vxl_byte> > colors;
  // create random colors for each set
  vnl_random rng;
  for (int i = 0; i < n_segments; i++) {
    vil_rgb<vxl_byte> c((vxl_byte)(rng.drand32()*255), (vxl_byte)(rng.drand32()*255), (vxl_byte)(rng.drand32()*255));
    colors.push_back(c);
  }

  for (unsigned i = 0; i<ss->node_cnt(); i++) {
    int comp = ds.find_set(i);
    vcl_pair<unsigned, unsigned> pix = ss->get_pixel(i);
    out_img(pix.first, pix.second) = colors[comp];
  }

  delete ss;
}


// segment a multi-plane image. smooth and segment each plane separately, combine the segments to generate a single output image
template <class T> 
void sdet_segment_img_multi(vil_image_view<T> const& img, unsigned margin, int neigh, T weight_thres, float sigma, int min_size, vil_image_view<vil_rgb<vxl_byte> >& out_img)
{
  sdet_graph_img_seg* ss = new sdet_graph_img_seg(img.ni(), img.nj(), margin, neigh);

  // smooth the image
  vil_image_view<T> smoothed;
  
  if (sigma <= 0) 
    smoothed = vil_copy_deep(img);
  else {
    // smooth source image using gaussian filter
    vil_gauss_filter_5tap_params gauss_params(sigma);
    vil_gauss_filter_5tap<T, T>(img, smoothed, gauss_params);
  }

  // set up the edge costs as color differences
  vcl_vector<vbl_edge>& edges = ss->get_edges();
  for (unsigned i = 0; i < edges.size(); i++) {
    vcl_pair<unsigned, unsigned> pix0 = ss->get_pixel(edges[i].v0_);
    double c0 = (double)smoothed(pix0.first, pix0.second);
    vcl_pair<unsigned, unsigned> pix1 = ss->get_pixel(edges[i].v1_);
    double c1 = (double)smoothed(pix1.first, pix1.second);
    double dif = c1-c0;
    edges[i].w_ = (float)vcl_sqrt(dif*dif);
  }
    
  vbl_disjoint_sets ds; 
  ds.add_elements(ss->node_cnt());
  
  // segment graph
  vbl_graph_partition(ds, edges, weight_thres);
  
  // combine the segments with number of elements less than min_size
  // post process small components
  for (int i = 0; i < edges.size(); i++) {
      int v0 = ds.find_set(edges[i].v0_);
      int v1 = ds.find_set(edges[i].v1_);
      if ((v0 != v1) && ((ds.size(v0) < min_size) || (ds.size(v1) < min_size)))
          ds.set_union(v0, v1);
  }
  vcl_cout << " segmentation resulted in " << ds.num_sets() << " segments!n";

  out_img.set_size(img.ni(), img.nj());
  out_img.fill(vil_rgb<vxl_sbyte>(0,0,0));

  int n_segments = ds.num_elements();  // the number of colors need to be in the number of initial number of nodes, cause the final segment ids are the ids of the nodes
  vcl_vector<vil_rgb<vxl_byte> > colors;
  // create random colors for each set
  vnl_random rng;
  for (int i = 0; i < n_segments; i++) {
    vil_rgb<vxl_byte> c((vxl_byte)(rng.drand32()*255), (vxl_byte)(rng.drand32()*255), (vxl_byte)(rng.drand32()*255));
    colors.push_back(c);
  }

  for (unsigned i = 0; i<ss->node_cnt(); i++) {
    int comp = ds.find_set(i);
    vcl_pair<unsigned, unsigned> pix = ss->get_pixel(i);
    out_img(pix.first, pix.second) = colors[comp];
  }

  delete ss;
}



#endif // sdet_graph_img_seg_h_
