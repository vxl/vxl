#ifndef _gevd_edgel_regions_h_
#define _gevd_edgel_regions_h_
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
//  25 April 2000 - collinsr@cs.rpi.edu - switched _region_edges
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
#include <vcl_list.h>
#include <vcl_map.h>

#include <vtol/vtol_vertex.h>
#include <vtol/vtol_edge_sptr.h>
#include <vsol/vsol_spatial_object_2d.h>
#include <gevd/gevd_region_edge.h>
#include <gevd/gevd_bufferxy.h>
#include <gevd/gevd_detector.h>
#include <gevd/gevd_intensity_face.h>
#include <vil/vil_image.h>


class gevd_edgel_regions
{
public:
  enum RegionLabel {UNLABELED=0, EDGE, LABEL};
  //Constructors/Destructors
  gevd_edgel_regions(bool debug = false);
  ~gevd_edgel_regions();
  //Main process method
  bool compute_edgel_regions(vil_image* image,
                             vcl_vector<vsol_spatial_object_2d*>& sgrp,
                             vcl_vector<gevd_intensity_face*>& faces);

  bool compute_edgel_regions(gevd_bufferxy* buf,
                             vcl_vector<vsol_spatial_object_2d*>& sgrp,
                             vcl_vector<gevd_intensity_face*>& faces);
  //Acessors
  void SetVerbose() {_verbose = true;}
  void ClearVerbose() {_verbose = false;}
  void SetDebug() {_debug = true;}
  void ClearDebug() {_debug = false;}
  unsigned int BaseLabel(unsigned int label);
  unsigned int GetMaxRegionLabel(){return _max_region_label;}
  void SetMaxRegionLabel(unsigned int label){_max_region_label = label;}
  unsigned int** GetRegionArray(){return _region_label_array;}
  int GetXSize(){return (_xend - _xo + 1);}
  int GetYSize(){return (_yend - _yo + 1);}
  vil_image* GetEdgeImage(vcl_vector<vsol_spatial_object_2d *>& edgels);
#if 0
  topo_debug_data_ref get_topo_debug_data(){return _debug_data;};
#endif
  //Utitities (especially for testing)
  bool InsertRegionEquivalence(unsigned int label_b, unsigned int label_a);
  void GrowEquivalenceClasses();
  void PropagateEquivalence();
#if 0 // now obsolete: replaced by GetLabel
  unsigned int GetLeftLabel(vtol_edge_sptr e);
  unsigned int GetRightLabel(vtol_edge_sptr e);
#endif
  unsigned int GetLabel(vtol_edge_sptr e, unsigned int nr);
  //Debug print methods
  void print_region_array();
  void print_region_equivalence();
  void print_reverse_region_equivalence();
  void print_base_equivalence();
  void print_intensity_data();
protected:
  //Utilities
  bool GroupContainsEdges(vcl_vector<vsol_spatial_object_2d *>& sg);
  bool InitRegionArray(vcl_vector<vsol_spatial_object_2d *>& sg);
  unsigned char label_code(unsigned int label);
  bool add_to_forward(unsigned int key, unsigned int value);
  bool add_to_reverse(unsigned int key, unsigned int value);
  unsigned char EncodeNeighborhood(unsigned int ul, unsigned int ur,
                                   unsigned int ll, unsigned int lr);
  void UpdateConnectedNeighborhood(unsigned int x, unsigned int y);
  void AssignEdgeLabels(unsigned int x, unsigned int y);
  void ApplyRegionEquivalence();
  bool out_of_bounds(unsigned int x, unsigned int y);
  void insert_adjacency(unsigned int region, vtol_edge_sptr e);
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
  bool merge_equivalence(vcl_map<unsigned int, vcl_vector<unsigned int>* >& tab,
                         unsigned int cur_label,
                         unsigned int label);
  bool get_next_label(vcl_vector<unsigned int>* labels,
                      unsigned int& label);
  void print_edge_colis(unsigned int x, unsigned int y,
                        gevd_region_edge* r1, gevd_region_edge* r2);
  bool corrupt_boundary(vcl_vector<vtol_edge_sptr>& edges,
                        vcl_vector<vtol_vertex_sptr>& bad_verts);
  bool remove_hairs(vcl_vector<vtol_edge_sptr>& edges);
  bool connect_ends(vcl_vector<vtol_edge_sptr>& edges,
                    vcl_vector<vtol_vertex_sptr>& bad_verts);
  void repair_failed_insertions(vcl_vector<vtol_edge_sptr>& edges,
                                vcl_vector<vtol_vertex_sptr>& bad_verts);
  void get_buffer_row(unsigned int row);
  void get_image_row(unsigned int row);
  unsigned short get_intensity(unsigned int x);
  int bytes_per_pix();

  //to be used after image or buf are set
  bool compute_edgel_regions(vcl_vector<vsol_spatial_object_2d *>& sgrp,
                             vcl_vector<gevd_intensity_face*>& faces);
  //members
  bool _verbose;
  bool _debug;
  bool _image_source;
  bool _buf_source;
  int _magnification;
  vil_image* _image;
  gevd_bufferxy* _buf;
  gevd_region_edge*** _edge_boundary_array;
  unsigned int** _region_label_array;
  unsigned int _min_region_label;
  unsigned int _max_region_label;
  float _Xob;//buffer X origin in original image
  float _Yob;//buffer Y origin in original image
  unsigned int _xo;                    //X coor of starting x pixel
  unsigned int _yo;                    //Y coor of starting y pixel
  unsigned int _xend;                  //X coor of ending x pixel
  unsigned int _yend;                  //Y coor of ending y pixel
  //Region label equivalency hash tables
  vcl_map<unsigned int, vcl_vector<unsigned int>* > _region_pairs_forward;
  vcl_map<unsigned int, vcl_vector<unsigned int>* > _region_pairs_reverse;
  vcl_map<unsigned int, vcl_vector<unsigned int>* > _equivalence_set;
  vcl_map<unsigned int, unsigned int > _label_map;
  //hash table for Edge<->gevd_region_edge relationship
  vcl_map<int, gevd_region_edge*> _region_edges;
  vcl_map<unsigned int, vcl_vector<vtol_edge_sptr>* > _region_edge_adjacency;
  //Final output gevd_intensity_face(s) and relation to corresponding region label
  vcl_vector<gevd_intensity_face*>* _faces;
  gevd_intensity_face** _intensity_face_index;
  vcl_vector<vtol_edge_sptr>** _face_edge_index;
  vcl_vector<vtol_edge_sptr>* _failed_insertions; //Short edges that fail
#if 0
  topo_debug_data_ref _debug_data;
#endif
  unsigned char* _ubuf;
  unsigned short* _sbuf;
};

#endif
