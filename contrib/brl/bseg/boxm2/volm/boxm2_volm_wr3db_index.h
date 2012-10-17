//This is brl/bseg/boxm2/volm/boxm2_volm_wr3db_index.h
#ifndef boxm2_volm_wr3db_index_h_
#define boxm2_volm_wr3db_index_h_
//:
// \file
// \brief  A class to index a given voxel world for fast matching to volumetric queries
//
// \author Ozge C. Ozcanli 
// \date October 07, 2012
// \verbatim
//   Modifications
//
// \endverbatim
// 

#include <vbl/vbl_ref_count.h>
#include <bbas/volm/volm_spherical_container.h>
#include <bbas/volm/volm_spherical_container_sptr.h>

class boxm2_volm_wr3db_index : public vbl_ref_count
{
  public:
    boxm2_volm_wr3db_index(volm_spherical_container_sptr cont, bool only_vis) : 
      cont_(cont), size_((unsigned int)cont->get_voxels().size()), only_vis_(only_vis), vis_values_(0), prob_values_(0) {}
    ~boxm2_volm_wr3db_index();
    
    void allocate_mem();
   
  protected:
    volm_spherical_container_sptr cont_;
    unsigned int size_;
    bool only_vis_;
    char* vis_values_;
    char* prob_values_;
};

#endif  // boxm2_volm_wr3db_index_h_
