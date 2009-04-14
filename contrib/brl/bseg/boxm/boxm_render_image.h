#ifndef boxm_render_image_h_
#define boxm_render_image_h_

#include <vcl_vector.h>

#include <boct/boct_tree.h>
#include <boxm/boxm_scene.h>
#include <boxm/boxm_sample.h>

#include <vpgl/vpgl_camera.h>

#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_transform.h>
#include <vil/vil_save.h>
#include <vil/vil_view_as.h>



template <class T_loc, boxm_apm_type APM>
void boxm_render_image_splatting(boxm_scene<T_loc, boxm_sample<APM> > &scene, vpgl_camera_double_sptr cam, 
					   vil_image_view<typename boxm_apm_traits<APM>::obs_datatype> &expected, 
					   vil_image_view<float> &mask, 
					   bool use_black_background = false)
{
  //: code to iterate over the blocks in order of visibility 
  boxm_block_vis_graph_iterator<tree_type> block_vis_iter(cam, &scene, expected.ni(), expected.nj());

  vcl_vector<boxm_block<tree_type>*> blocks;
  while (block_vis_iter.next()) {
    vcl_cout << "Frontier\n";
    blocks = block_vis_iter.frontier();
    for(unsigned i=0; i<blocks.size(); i++) {
      vcl_cout << i << "- " << blocks[i]->bounding_box() << vcl_endl; 
    }
  }
  return;
}




#endif

