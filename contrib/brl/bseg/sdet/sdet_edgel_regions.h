// This is brl/bseg/sdet/sdet_edgel_regions.h
#ifndef sdet_edgel_regions_h_
#define sdet_edgel_regions_h_
//:
// \file
// \brief generation of regions bounded by sdet_edgel chains
// \verbatim
//     Inputs:  vil1_image, vector<vtol_edge_2d_sptr>
//     Output: vector<gevd_intensity_face_sptr>
// \endverbatim
//  The idea is to generate regions by inserting boundaries into an
//  array.  The boundaries are defined by a set of edgel chains and
//  a boundary location is inserted at each edgel.  The array is
//  assumed to have a boundary at the perimeter of the ROI.
//
//  The array is scanned with a 2x2 neighborhood to form connected
//  components in the usual way. Each of the connected component labels are
//  reduced to the lowest equivalent label id by a transitive closure
//  on the equivalence table.
//
// A second scan of the label array is made to determine adjacency to
//  a given edge.  This process is enabled by a companion class,
//  the gevd_region_edge which has storage for two labels, corresponding to
//  each side of the edge.
//
//  The region edges are then used to collect the input edges which are
//  adjacent to a given region.  The set of edges adjacent to a region
//  are used to construct a multiply-connected sub-class of vtol_face_2d, the
//  vtol_intensity_face.  The vtol_intensity_face contains a
//  vdgl_digtial_region which holds arrays of pixel coordinates and values
//  within the face boundaries.
//
// \author Author J. L. Mundy - January 14, 1999
// \verbatim
// Modifications
//  25 April 2000 - collinsr@cs.rpi.edu - switched region_edges_
//                  to key on the Id() rather than the pointer value
//                  to avoid different hash tables (and different
//                  segmentations) for identical inputs (leaving the
//                  question of why the hash table order matters for
//                  another day)
// 24 April 2002 - Peter Vanroose - replaced GetLeftLabel and GetRightLabel
//                  by GetLabel, in accordance with sdet_region_edge change
//
// 19 November 2002 - J.L. Mundy moved to brl for further evolution
// 09 January 2003  - J.L. Mundy implemented expanded array resolution
//                    for more accurate connection neigborhoods
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vbl/vbl_array_2d.h>
#include <vil1/vil1_image.h>
#include <vil/vil_image_resource.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <gevd/gevd_bufferxy.h>
#include <vtol/vtol_intensity_face_sptr.h>
#include <sdet/sdet_region_edge_sptr.h>

class sdet_edgel_regions
{
 public:
  enum RegionLabel {UNLABELED=0, EDGE, LABEL};
  //Constructors/Destructors
  sdet_edgel_regions(int array_scale=2, bool verbose = false,
                     bool debug = false);
  ~sdet_edgel_regions();
  //Main process method
  bool compute_edgel_regions(vil1_image const& image,
                             vcl_vector<vtol_edge_2d_sptr>& sgrp,
                             vcl_vector<vtol_intensity_face_sptr>& faces);

  bool compute_edgel_regions(vil_image_resource_sptr const& image,
                             vcl_vector<vtol_edge_2d_sptr>& sgrp,
                             vcl_vector<vtol_intensity_face_sptr>& faces);


  bool compute_edgel_regions(gevd_bufferxy* buf,
                             vcl_vector<vtol_edge_2d_sptr>& sgrp,
                             vcl_vector<vtol_intensity_face_sptr>& faces);
  //Acessors
  void SetVerbose() { verbose_ = true; }
  void ClearVerbose() { verbose_ = false; }
  void SetDebug() { debug_ = true; }
  void ClearDebug() { debug_ = false; }
#if 0
  void set_magnification(float magnification) { magnification_=magnification; }
#endif // 0
  unsigned int BaseLabel(unsigned int label);
  unsigned int GetMaxRegionLabel() const { return max_region_label_; }
  void SetMaxRegionLabel(unsigned int label) { max_region_label_ = label; }
  vbl_array_2d<unsigned int> const& GetRegionArray() const { return region_label_array_; }
  int GetXSize() const { return xend_ - xo_ + 1; }
  int GetYSize() const { return yend_ - yo_ + 1; }

  vil1_image GetEdgeImage(vcl_vector<vtol_edge_2d_sptr>& edgels);
#ifdef DEBUG
  topo_debug_data_ref get_topo_debug_data() { return debug_data_; }
#endif
  //Utitities (especially for testing)
  bool InsertRegionEquivalence(unsigned int label_b, unsigned int label_a);
  void GrowEquivalenceClasses();
  void PropagateEquivalence();
  unsigned int GetLabel(vtol_edge_2d_sptr e, unsigned int nr) const;
  //Debug print methods
  void print_region_array();
  void print_region_equivalence();
  void print_reverse_region_equivalence();
  void print_base_equivalence();
  void print_intensity_data();
 protected:
  //Utilities
  bool GroupContainsEdges(vcl_vector<vtol_edge_2d_sptr>& sg);
  bool InitRegionArray(vcl_vector<vtol_edge_2d_sptr>& sg);
  unsigned char label_code(unsigned int label);
  bool add_to_forward(unsigned int key, unsigned int value);
  bool add_to_reverse(unsigned int key, unsigned int value);
  unsigned char EncodeNeighborhood(unsigned int ul, unsigned int ur,
                                   unsigned int ll, unsigned int lr);
  void UpdateConnectedNeighborhood(unsigned int x, unsigned int y);
  void AssignEdgeLabels(unsigned int x, unsigned int y);
  void ApplyRegionEquivalence();
  bool out_of_bounds(unsigned int x, unsigned int y);
  void insert_adjacency(unsigned int region, vtol_edge_2d_sptr e);
  void CollectEdges();
  void CollectFaceEdges();
  void ConstructFaces();
  void AccumulateMeans();
  void AccumulateRegionData();
  void InsertFaceData();
  unsigned int X(unsigned int x);
  unsigned int Y(unsigned int y);
  float Xf(float x);
  float Yf(float y);
  bool insert_edgel(float pre_x, float pre_y, float x, float y,
                    sdet_region_edge_sptr const& e);
  void insert_equivalence(unsigned int ll, unsigned int ur, unsigned int& lr);
  bool merge_equivalence(vcl_map<unsigned int, vcl_vector<unsigned int>* >& tab,
                         unsigned int cur_label,
                         unsigned int label);
  bool get_next_label(vcl_vector<unsigned int>* labels,
                      unsigned int& label);
  void print_edge_colis(unsigned int x, unsigned int y,
                        sdet_region_edge_sptr const & r1,
                        sdet_region_edge_sptr const & r2);

  bool remove_hairs(vcl_vector<vtol_edge_2d_sptr>& edges);

  void get_buffer_row(unsigned int row);
  void get_image_row(unsigned int row);
  unsigned short get_intensity(unsigned int x);
  int bytes_per_pix();

  //to be used after image or buf are set
  bool compute_edgel_regions(vcl_vector<vtol_edge_2d_sptr>& sgrp,
                             vcl_vector<vtol_intensity_face_sptr>& faces);

  //members
  bool verbose_;
  bool debug_;
  bool image_source_;
  bool buf_source_;
  int s_;//resolution factor for label, edge arrays
  vil1_image image_;
  gevd_bufferxy* buf_;
  vbl_array_2d<sdet_region_edge_sptr> edge_boundary_array_;
  vbl_array_2d<unsigned int> region_label_array_;
  unsigned int min_region_label_;
  unsigned int max_region_label_;
  float Xob_;//buffer X origin in original image
  float Yob_;//buffer Y origin in original image
  unsigned int xo_;                    //X index of starting array element
  unsigned int yo_;                    //Y index of starting array element
  unsigned int xend_;                  //X index of ending array element
  unsigned int yend_;                  //Y index of ending array element
  unsigned int xs_;
  unsigned int ys_;
  //Region label equivalency hash tables
  vcl_map<unsigned int, vcl_vector<unsigned int>* > region_pairs_forward_;
  vcl_map<unsigned int, vcl_vector<unsigned int>* > region_pairs_reverse_;
  vcl_map<unsigned int, vcl_vector<unsigned int>* > equivalence_set_;
  vcl_map<unsigned int, unsigned int > label_map_;
  //hash table for Edge<->sdet_region_edge relationship
  vcl_map<int, sdet_region_edge_sptr> region_edges_;
  vcl_map<unsigned int, vcl_vector<vtol_edge_2d_sptr>* > region_edge_adjacency_;
  //Final output vtol_intensity_face(s) and relation to corresponding region label
  vcl_vector<vtol_intensity_face_sptr>* faces_;
  vtol_intensity_face_sptr* intensity_face_index_;
  vcl_vector<vtol_edge_2d_sptr>** face_edge_index_;
  vcl_vector<vtol_edge_2d_sptr>* failed_insertions_; //Short edges that fail
#ifdef DEBUG
  topo_debug_data_ref debug_data_;
#endif
  unsigned char* ubuf_;
  unsigned short* sbuf_;
};

#endif // sdet_edgel_regions_h_
