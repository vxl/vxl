#ifndef boxm2_blocks_vis_graph_h_
#define boxm2_blocks_vis_graph_h_


#include <vcl_set.h>
#include <vbl/vbl_smart_ptr.h>
#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/boxm2_scene.h>
#include <vpgl/vpgl_generic_camera.h>
#include <vbl/vbl_ref_count.h>

class boxm2_block_vis_graph_node;
typedef vbl_smart_ptr<boxm2_block_vis_graph_node> boxm2_block_vis_graph_node_sptr;

class boxm2_block_vis_graph_node : public vbl_ref_count
{
 public:
    boxm2_block_vis_graph_node(boxm2_block_id id): id_(id), num_incoming_edges_(0){}
    boxm2_block_id id_;
    vcl_set<boxm2_block_vis_graph_node_sptr> out_edges;
    int num_incoming_edges_;
};

class boxm2_block_vis_graph
{
 public:
  boxm2_block_vis_graph(){}
  boxm2_block_vis_graph(vcl_map<boxm2_block_id,boxm2_block_metadata> & blkmetadata,  vpgl_generic_camera<double> const& cam );
  vcl_list<boxm2_block_vis_graph_node_sptr>::iterator find(const boxm2_block_id & id, vcl_list<boxm2_block_vis_graph_node_sptr> & list_nodes );
  vcl_vector<boxm2_block_id>  get_ordered_ids(){return ordered_nodes_;}

private:
  bool construct_graph();
  vcl_list<boxm2_block_vis_graph_node_sptr> nodes_;
  vcl_vector<boxm2_block_id> ordered_nodes_;
};
#endif
