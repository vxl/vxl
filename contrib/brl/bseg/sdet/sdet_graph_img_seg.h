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
// the edge weights will be computed later using the methods provided by this class - weight represents the "dissimilarity" of its nodes (so lower weight edges will be removed)
// the eventual segmentation algorithm finds a set of connected components in the graph by breaking up certain edges and keeping others
//  a static method that segments grey scale images using color is provided
//  each pixel/node is assigned to a connected component (segment) as a result
//

#include <vector>
#include <map>
#include <iostream>
#include <iosfwd>
#include <string>
#include <utility>
#include <limits>
#include <vbl/vbl_ref_count.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#include <vbl/vbl_edge.h>
#include <vbl/vbl_graph_partition.h>
#include "sdet_detector.h"
#include "sdet_detector_params.h"
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_edgel.h>
#include <vsol/vsol_box_2d.h>
#include <vul/vul_timer.h>
class sdet_graph_img_seg : public vbl_ref_count
{
 public:
   // constructor - provide a margin around the image to be left out from the graph. if neigh = 4 use 4-neighborhood of a pixel, otherwise 8
   sdet_graph_img_seg(unsigned ni, unsigned nj, unsigned margin, int neigh);

   int get_id(unsigned i, unsigned j) { return pixel_ids_(i,j); }
   std::pair<unsigned, unsigned> get_pixel(unsigned id) { return id_to_pix_[id]; }

   std::vector<vbl_edge>& get_edges() { return edges_; }
   unsigned node_cnt() { return node_cnt_; }

   static void create_colors(std::vector<vil_rgb<vxl_byte> >& colors, int n_segments);
 protected:
   vil_image_view<int> pixel_ids_;
   //std::map<int, std::pair<unsigned, unsigned> > id_to_pix_;
   std::vector<std::pair<unsigned, unsigned> > id_to_pix_;//see if this container is faster.
   std::vector<vbl_edge> edges_;
   unsigned node_cnt_;
};

#include <vil/vil_copy.h>
#include <vil/algo/vil_gauss_filter.h>
#include <vnl/vnl_random.h>
#include "sdet_region.h"
#include "sdet_region_sptr.h"
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
  std::vector<vbl_edge>& edges = ss->get_edges();
  for (auto & edge : edges)
    {
    std::pair<unsigned, unsigned> pix0 = ss->get_pixel(edge.v0_);
    double c0 = (double)smoothed(pix0.first, pix0.second);
    std::pair<unsigned, unsigned> pix1 = ss->get_pixel(edge.v1_);
    double c1 = (double)smoothed(pix1.first, pix1.second);
    double dif = c1-c0;
    edge.w_ = (float)std::sqrt(dif*dif);
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

  out_img.set_size(img.ni(), img.nj());
  out_img.fill(vil_rgb<vxl_sbyte>(0,0,0));

  int n_segments = ds.num_elements();  // the number of colors need to be in the number of initial number of nodes, cause the final segment ids are the ids of the nodes
  std::vector<vil_rgb<vxl_byte> > colors;
  // create unique colors for each set
  sdet_graph_img_seg::create_colors(colors, n_segments);

  for (unsigned int i = 0; i<ss->node_cnt(); ++i)
    {
    int comp = ds.find_set(i);
    std::pair<unsigned, unsigned> pix = ss->get_pixel(i);
    out_img(pix.first, pix.second) = colors[comp];
    }

  delete ss;
}


// segment an image using an edge image as a helper, try to respect edges in forming the regions
template <class T>
void sdet_segment_img_using_edges(vil_image_view<T> const& img, vil_image_view<float> const& edge_img, unsigned margin, int neigh, T weight_thres, float sigma, int min_size, vil_image_view<vil_rgb<vxl_byte> >& out_img)
{
  // check if the input images are of the same size
  if (edge_img.ni() != img.ni() || edge_img.nj() != img.nj()) {
    std::cerr << "Input edge image does not have the same size as the input image to be segmented!\n";
    return;
  }

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

  // set up the edge costs as color differences and with high weights across the edges
  std::vector<vbl_edge>& edges = ss->get_edges();
  for (auto & edge : edges) {
    std::pair<unsigned, unsigned> pix0 = ss->get_pixel(edge.v0_);
    double c0 = (double)smoothed(pix0.first, pix0.second);
    std::pair<unsigned, unsigned> pix1 = ss->get_pixel(edge.v1_);
    double c1 = (double)smoothed(pix1.first, pix1.second);
    double dif = c1-c0;
    edge.w_ = (float)std::sqrt(dif*dif);

    double e0 = (double)edge_img(pix0.first, pix0.second);
    double e1 = (double)edge_img(pix1.first, pix1.second);
    //dif = e0-e1;
    //edges[i].w_ += (float)std::sqrt(dif*dif);
    edge.w_ += ( e0 > e1 ? e0 : e1);
  }

  vbl_disjoint_sets ds;
  ds.add_elements(ss->node_cnt());

  // segment graph
  vbl_graph_partition(ds, edges, weight_thres);

  // combine the segments with number of elements less than min_size
  // post process small components
  for (auto & edge : edges) {
      int v0 = ds.find_set(edge.v0_);
      int v1 = ds.find_set(edge.v1_);
      if ((v0 != v1) && ((ds.size(v0) < min_size) || (ds.size(v1) < min_size)))
          ds.set_union(v0, v1);
  }
  std::cout << " segmentation resulted in " << ds.num_sets() << " segments!\n";

  out_img.set_size(img.ni(), img.nj());
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
// segment an image using VanDuc edge chains as high graph edge cost
template <class T>
void sdet_segment_img_using_VD_edges(vil_image_view<T> const& img, unsigned margin, int neigh, double weight_thres, float sigma, float vd_noise_mul, int min_size, vil_image_view<vil_rgb<vxl_byte> >& out_img, std::map<unsigned, sdet_region_sptr>& regions, bool use_edges = true)
{
  vul_timer t;
  // form edge image
  vil_image_view<bool> edge_map(img.ni(), img.nj());
  edge_map.fill(false);
  if(use_edges){
    sdet_detector_params det_params;
    det_params.aggressive_junction_closure=1;
    det_params.filterFactor = 0.0;
    det_params.borderp = false;
    det_params.smooth = sigma;
    det_params.noise_multiplier = vd_noise_mul;
    sdet_detector det(det_params);
    vil_image_view_base const& vbase = dynamic_cast<vil_image_view_base const&>(img);
    vil_image_resource_sptr resc = vil_new_image_resource_of_view(vbase);
    det.SetImage(resc);
    if(!det.DoContour()){
      std::cout << "Edgel detection failed\n";
      return;
    }
    std::vector<vdgl_digital_curve_sptr> vd_edges;
    if (!det.get_vdgl_edges(vd_edges)){
      std::cout << "Detection worked but returned no edgels\n";
      return;
    }
    for (auto & vd_edge : vd_edges)
      {
        //get the edgel chain
        vdgl_interpolator_sptr itrp = vd_edge->get_interpolator();
        vdgl_edgel_chain_sptr ech = itrp->get_edgel_chain();
        unsigned int n = ech->size();
        for (unsigned int i=0; i<n;i++)
          {
            vdgl_edgel ed = (*ech)[i];
            unsigned u = static_cast<unsigned>(ed.x() +0.5);
            unsigned v = static_cast<unsigned>(ed.y() +0.5);
            edge_map(u,v) = true;
          }
      }
    std::cout << " VD edges in " << t.real() << "msecs" << std::endl;
  }
  t.mark();
  sdet_graph_img_seg ss(img.ni(), img.nj(), margin, neigh);

  // smooth the image
  vil_image_view<T> smoothed;

  if (sigma <= 0)
    smoothed = vil_copy_deep(img);
  else {
    // smooth source image using gaussian filter
    vil_gauss_filter_5tap_params gauss_params(sigma);
    vil_gauss_filter_5tap<T, T>(img, smoothed, gauss_params);
  }

  // set up the edge costs as color differences and with high weights across the edges
  std::vector<vbl_edge>& edges = ss.get_edges();
  for (auto & edge : edges) {
    std::pair<unsigned, unsigned> pix0 = ss.get_pixel(edge.v0_);
    double c0 = (double)smoothed(pix0.first, pix0.second);
    std::pair<unsigned, unsigned> pix1 = ss.get_pixel(edge.v1_);
    double c1 = (double)smoothed(pix1.first, pix1.second);
    double dif = c1-c0;
    edge.w_ = (float)std::sqrt(dif*dif);

    bool e0 = edge_map(pix0.first, pix0.second);
    bool e1 = edge_map(pix1.first, pix1.second);
    if(e0 || e1)
      edge.w_ = std::numeric_limits<float>::max();
  }

  vbl_disjoint_sets* ds = new vbl_disjoint_sets();
  ds->add_elements(ss.node_cnt());

  // segment graph
  vbl_graph_partition(*ds, edges, weight_thres);

  // combine the segments with number of elements less than min_size
  // post process small components
  for (auto & edge : edges) {
      int v0 = ds->find_set(edge.v0_);
      int v1 = ds->find_set(edge.v1_);
      if ((v0 != v1) && ((ds->size(v0) < min_size) || (ds->size(v1) < min_size)))
          ds->set_union(v0, v1);
  }
  std::cout << "create disjoint set regions " << t.real() << "msecs" << std::endl;
  std::cout << " segmentation resulted in " << ds->num_sets() << " segments!\n";
  t.mark();
  out_img.set_size(img.ni(), img.nj());
  out_img.fill(vil_rgb<vxl_sbyte>(0,0,0));

  int n_segments = ds->num_elements();  // the number of colors need to be in the number of initial number of nodes, cause the final segment ids are the ids of the nodes
  std::vector<vil_rgb<vxl_byte> > colors;
  // create unique colors for each set
  sdet_graph_img_seg::create_colors(colors, n_segments);

  for (unsigned i = 0; i<ss.node_cnt(); i++) {
    int comp = ds->find_set(i);
    std::pair<unsigned, unsigned> pix = ss.get_pixel(i);
    vil_rgb<vxl_byte>& c = colors[comp];
    out_img(pix.first, pix.second) = c;
  }
  vil_rgb<vxl_byte> black((vxl_byte)0, (vxl_byte)0, (vxl_byte)0);
  for(unsigned j = 0; j<img.nj(); ++j)
    for(unsigned i = 0; i<img.ni(); ++i)
      if(edge_map(i,j))
        out_img(i,j) = black;
  std::cout << "create color image in " << t.real() << "msecs" << std::endl;
  t.mark();
  // output regions
  for (unsigned i = 0; i<ss.node_cnt(); i++) {
    int comp = ds->find_set(i);
    int n = ds->size(comp);
    std::map<unsigned, sdet_region_sptr>::iterator rit;
    rit = regions.find(comp);
    if(rit == regions.end()){
      regions[comp] = new sdet_region();
      regions[comp]->set_label(comp);
      regions[comp]->SetNpts(n);
      regions[comp]->InitPixelArrays();
    }
    sdet_region_sptr& reg = regions[comp];
    std::pair<unsigned, unsigned> pix = ss.get_pixel(i);
    unsigned short intensity = static_cast<unsigned short>(img(pix.first, pix.second));
    float u = static_cast<float>(pix.first), v = static_cast<float>(pix.second);
    reg->IncrementMeans(u, v, intensity);
    reg->InsertInPixelArrays(u,v,intensity);
  }
  edges = ss.get_edges();
  for (auto & edge : edges) {
    int v0 = ds->find_set(edge.v0_);
    int v1 = ds->find_set(edge.v1_);
    if(v0 != v1){
      int comp0 = ds->find_set(v0);
      int comp1 = ds->find_set(v1);
      sdet_region_sptr& reg0 = regions[comp0];
      sdet_region_sptr& reg1 = regions[comp1];
      reg0->add_neighbor(comp1);
      reg1->add_neighbor(comp0);
    }
  }
  std::cout <<  "fill sdet regions " << t.real() << "msecs" << std::endl;
  delete ds; //takes a long time
}

// segment an image using two features, takes two normalized feature images as dimension 1 and dimension 2, calculates Euclidean distance
// only works for float images in [0,1]
// use sigma1 to smooth the first image and sigma2 to smooth the second image if needed, pass 0 if smoothing is not necessary
void sdet_segment_img2(vil_image_view<float> const& img1, vil_image_view<float> const& img2, unsigned margin, int neigh, float weight_thres, float sigma1, float sigma2, int min_size, vil_image_view<vil_rgb<vxl_byte> >& out_img);

//: segment using two features and also respect the edges in the edge image
void sdet_segment_img2_using_edges(vil_image_view<float> const& img1, vil_image_view<float> const& img2, vil_image_view<float> const& edge_img, unsigned margin, int neigh, float weight_thres, float sigma1, float sigma2, int min_size, vil_image_view<vil_rgb<vxl_byte> >& out_img);

#endif // sdet_graph_img_seg_h_
