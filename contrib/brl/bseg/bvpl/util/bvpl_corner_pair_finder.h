// This is//projects/vxl/src/contrib/brl/bseg/bvpl/util/bvpl_pair_finder.h
#ifndef bvpl_pair_finder_h
#define bvpl_pair_finder_h

//:
// \file
// \brief 
// \author Isabel Restrepo mir@lems.brown.edu
// \date  9/23/09
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <bvpl/bvpl_kernel.h>
#include <bvxm/grid/bvxm_voxel_grid.h>
#include <vgl/vgl_line_segment_3d.h>
#include <vgl/vgl_box_3d.h>


class bvpl_corner_pairs: public vbl_ref_count
{
public:
  
  //: Default constructor
  bvpl_corner_pairs() {}
  
  //: Constructor from a vector
  bvpl_corner_pairs(vcl_vector<vcl_vector<vgl_line_segment_3d<int> > >& all_lines) { pairs_ = all_lines;}
  
  //: Constructor from  2 vectors
  bvpl_corner_pairs(vcl_vector<vcl_vector<vgl_line_segment_3d<int> > >& all_lines,
                    vcl_vector<vcl_vector<vgl_box_3d<int> > > all_boxes)
  { 
    pairs_ = all_lines;
    boxes_ =all_boxes;
  }
  
  int size() {return pairs_.size(); }
  
  //: vector of pairs
  vcl_vector<vcl_vector<vgl_line_segment_3d<int> > > pairs_;
  
  // a vector of kernel coverage at certain points
  vcl_vector<vcl_vector<vgl_box_3d<int> > > boxes_;
  
};

typedef vbl_smart_ptr<bvpl_corner_pairs> bvpl_corner_pairs_sptr;

class bvpl_corner_pair_finder
{
public:
	//:Construnctor 
	bvpl_corner_pair_finder(){}
	
	//:Find pairs that are 90 degrees appart e.g |_ with _|
	static bvpl_corner_pairs_sptr find_pairs(bvxm_voxel_grid<int>* id_grid, 
                                           bvpl_kernel_vector_sptr search_kernels,
											                     bvpl_kernel_vector_sptr corner_kernels);
	
	//:Find pairs that are 90 degrees appart e.g |_ with _|
	static bvpl_corner_pairs_sptr find_pairs(bvxm_voxel_grid<int>* id_grid, 
											                     bvxm_voxel_grid<float>* response_grid,
											                     bvpl_kernel_vector_sptr search_kernels,
											                     bvpl_kernel_vector_sptr corner_kernels);
	
};


#endif

