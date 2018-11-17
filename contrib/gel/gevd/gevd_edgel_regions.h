// This is gel/gevd/gevd_edgel_regions.h
#ifndef gevd_edgel_regions_h_
#define gevd_edgel_regions_h_
//:
// \file
// \brief generation of regions bounded by gevd_edgel chains
// \verbatim
//     Inputs:  Image, EdgelGroup
//     Output: CoolListP<IntensityFace*>
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
//  are used to construct a multiply-connected sub-class of Face, the
//  IntensityFace.  The IntensityFace maintains a scatter matrix of
//  a planar fit to the intensity distribution over the corresponding
//  region.
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
//                  by GetLabel, in accordance with gevd_region_edge change
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vector>
#include <iostream>
#include <map>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vtol/vtol_vertex_sptr.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <gevd/gevd_region_edge.h>
#include <gevd/gevd_bufferxy.h>
#include <vtol/vtol_intensity_face_sptr.h>
#include <vil1/vil1_image.h>


class gevd_edgel_regions
{
 public:
  enum RegionLabel {UNLABELED=0, EDGE, LABEL};
  //Constructors/Destructors
  gevd_edgel_regions(bool debug = false);
  ~gevd_edgel_regions();
  //Main process method
  bool compute_edgel_regions(vil1_image* image,
                             std::vector<vtol_edge_2d_sptr>& sgrp,
                             std::vector<vtol_intensity_face_sptr>& faces);

  bool compute_edgel_regions(gevd_bufferxy* buf,
                             std::vector<vtol_edge_2d_sptr>& sgrp,
                             std::vector<vtol_intensity_face_sptr>& faces);
  //Acessors
  void SetVerbose() {verbose_ = true;}
  void ClearVerbose() {verbose_ = false;}
  void SetDebug() {debug_ = true;}
  void ClearDebug() {debug_ = false;}
  void set_magnification(float magnification){magnification_=magnification;}
  unsigned int BaseLabel(unsigned int label);
  unsigned int GetMaxRegionLabel() const {return max_region_label_;}
  void SetMaxRegionLabel(unsigned int label){max_region_label_ = label;}
  unsigned int** GetRegionArray(){return region_label_array_;}
  int GetXSize() const {return xend_ - xo_ + 1;}
  int GetYSize() const {return yend_ - yo_ + 1;}
  vil1_image* GetEdgeImage(std::vector<vtol_edge_2d_sptr>& edgels);
#if 0
  topo_debug_data_ref get_topo_debug_data() {return debug_data_;}
#endif
  // Utilities (especially for testing)
  bool InsertRegionEquivalence(unsigned int label_b, unsigned int label_a);
  void GrowEquivalenceClasses();
  void PropagateEquivalence();
  unsigned int GetLabel(const vtol_edge_2d_sptr& e, unsigned int nr);
  // Debug print methods
  void print_region_array();
  void print_region_equivalence();
  void print_reverse_region_equivalence();
  void print_base_equivalence();
  void print_intensity_data();
 protected:
  // Utilities
  bool GroupContainsEdges(std::vector<vtol_edge_2d_sptr>& sg);
  bool InitRegionArray(std::vector<vtol_edge_2d_sptr>& sg);
  unsigned char label_code(unsigned int label);
  bool add_to_forward(unsigned int key, unsigned int value);
  bool add_to_reverse(unsigned int key, unsigned int value);
  unsigned char EncodeNeighborhood(unsigned int ul, unsigned int ur,
                                   unsigned int ll, unsigned int lr);
  void UpdateConnectedNeighborhood(unsigned int x, unsigned int y);
  void AssignEdgeLabels(unsigned int x, unsigned int y);
  void ApplyRegionEquivalence();
  bool out_of_bounds(unsigned int x, unsigned int y);
  void insert_adjacency(unsigned int region, const vtol_edge_2d_sptr& e);
  void CollectEdges();
  void CollectFaceEdges();
  void ConstructFaces();
  void AccumulateMeans();
  void AccumulateRegionData();
  void InsertFaceData();
  unsigned int X(unsigned int x);
  unsigned int Y(unsigned int y);
  unsigned int Xf(float x);
  unsigned int Yf(float y);
  bool insert_edgel(float pre_x, float pre_y, float x, float y,
                    gevd_region_edge* e);
  void insert_equivalence(unsigned int ll, unsigned int ur, unsigned int& lr);
  bool merge_equivalence(std::map<unsigned int, std::vector<unsigned int>* >& tab,
                         unsigned int cur_label,
                         unsigned int label);
  bool get_next_label(std::vector<unsigned int>* labels,
                      unsigned int& label);
  void print_edge_colis(unsigned int x, unsigned int y,
                        gevd_region_edge* r1, gevd_region_edge* r2);
  bool corrupt_boundary(std::vector<vtol_edge_2d_sptr>& edges,
                        std::vector<vtol_vertex_sptr>& bad_verts);
  bool remove_hairs(std::vector<vtol_edge_2d_sptr>& edges);
  bool connect_ends(std::vector<vtol_edge_2d_sptr>& edges,
                    std::vector<vtol_vertex_sptr>& bad_verts);
  void repair_failed_insertions(std::vector<vtol_edge_2d_sptr>& edges,
                                std::vector<vtol_vertex_sptr>& bad_verts);
  void get_buffer_row(unsigned int row);
  void get_image_row(unsigned int row);
  unsigned short get_intensity(unsigned int x);
  int bytes_per_pix();

  //to be used after image or buf are set
  bool compute_edgel_regions(std::vector<vtol_edge_2d_sptr>& sgrp,
                             std::vector<vtol_intensity_face_sptr>& faces);
  //members
  bool verbose_;
  bool debug_;
  bool image_source_;
  bool buf_source_;
  float magnification_;
  vil1_image* image_;
  gevd_bufferxy* buf_;
  gevd_region_edge*** edge_boundary_array_;
  unsigned int** region_label_array_;
  unsigned int min_region_label_;
  unsigned int max_region_label_;
  float Xob_;//buffer X origin in original image
  float Yob_;//buffer Y origin in original image
  unsigned int xo_;                    //X coor of starting x pixel
  unsigned int yo_;                    //Y coor of starting y pixel
  unsigned int xend_;                  //X coor of ending x pixel
  unsigned int yend_;                  //Y coor of ending y pixel
  //Region label equivalency hash tables
  std::map<unsigned int, std::vector<unsigned int>* > region_pairs_forward_;
  std::map<unsigned int, std::vector<unsigned int>* > region_pairs_reverse_;
  std::map<unsigned int, std::vector<unsigned int>* > equivalence_set_;
  std::map<unsigned int, unsigned int > label_map_;
  //hash table for Edge<->gevd_region_edge relationship
  std::map<int, gevd_region_edge*> region_edges_;
  std::map<unsigned int, std::vector<vtol_edge_2d_sptr>* > region_edge_adjacency_;
  //Final output vtol_intensity_face(s) and relation to corresponding region label
  std::vector<vtol_intensity_face_sptr>* faces_;
  vtol_intensity_face_sptr* intensity_face_index_;
  std::vector<vtol_edge_2d_sptr>** face_edge_index_;
  std::vector<vtol_edge_2d_sptr>* failed_insertions_; //Short edges that fail
#if 0
  topo_debug_data_ref debug_data_;
#endif
  unsigned char* ubuf_;
  unsigned short* sbuf_;
};

#endif // gevd_edgel_regions_h_
