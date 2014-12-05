#include <boxm2/boxm2_blocks_vis_graph.h>
#include <vcl_queue.h>

boxm2_block_vis_graph::boxm2_block_vis_graph(vcl_map<boxm2_block_id,boxm2_block_metadata> & blkmetadata, vpgl_generic_camera<double> & cam ): nodes_()
{

    vcl_map<boxm2_block_id,boxm2_block_metadata>::iterator iter = blkmetadata.begin();
    vcl_list<boxm2_block_vis_graph_node_sptr> temp_nodes;
    for(;iter!=blkmetadata.end();iter++)
    {
        boxm2_block_vis_graph_node * node = new boxm2_block_vis_graph_node();
        node->id_ = iter->first;
        temp_nodes.push_back(node);
    }
    vcl_list<boxm2_block_vis_graph_node_sptr>::iterator set_iter = temp_nodes.begin();
    for(;set_iter != temp_nodes.end();set_iter++)
    {
        boxm2_block_metadata md = blkmetadata[(*set_iter)->id_];
        vgl_box_3d<double> bbox = md.bbox();
        vcl_vector<vgl_point_3d<double> > vertices =  bbox.vertices() ;
        double u,v ;
        for( int x = -1 ; x <=1; x+=2)
            for( int y = -1 ; y <=1; y+=2)
                for( int z = -1 ; z <=1; z+=2)
                {
                    cam.project(bbox.centroid_x()+ x * bbox.width()/2,
                        bbox.centroid_y()+ y * bbox.height()/2,
                        bbox.centroid_z()+ z * bbox.depth()/2,
                        u,v);
                    if(u <0.0 || v< 0.0 || u >= cam.cols()-1 || v >= cam.rows()-1 )
                        continue;
                    vgl_ray_3d<double> ray = cam.ray(u,v);
                    //cam.project(
                    if(ray.direction().x()*x > 0)
                    {
                        vcl_list<boxm2_block_vis_graph_node_sptr>::iterator find_iter
                            = this->find(boxm2_block_id((*set_iter)->id_.i()+x,(*set_iter)->id_.j(),(*set_iter)->id_.k()), temp_nodes) ;
                        if ( find_iter != temp_nodes.end() )
                        {
                            (*set_iter)->out_edges.insert((*find_iter)->id_);
                            (*find_iter)->in_edges.insert((*set_iter)->id_);
                        }
                    }
                    if(ray.direction().y()*y > 0)
                    {
                        vcl_list<boxm2_block_vis_graph_node_sptr>::iterator find_iter
                            = this->find(boxm2_block_id((*set_iter)->id_.i(),(*set_iter)->id_.j()+y,(*set_iter)->id_.k()), temp_nodes) ;
                        if ( find_iter != temp_nodes.end() )
                        {
                            (*set_iter)->out_edges.insert((*find_iter)->id_);
                            (*find_iter)->in_edges.insert((*set_iter)->id_);
                        }
                    }
                    if(ray.direction().z()*z > 0)
                    {
                        vcl_list<boxm2_block_vis_graph_node_sptr>::iterator find_iter
                            = this->find(boxm2_block_id((*set_iter)->id_.i(),(*set_iter)->id_.j(),(*set_iter)->id_.k()+z), temp_nodes) ;
                        if ( find_iter != temp_nodes.end() )
                        {
                            (*set_iter)->out_edges.insert((*find_iter)->id_);
                            (*find_iter)->in_edges.insert((*set_iter)->id_);
                        }
                    }
                }
    }
    //: find the block with minmum or 0 in edges
    vcl_queue<boxm2_block_vis_graph_node_sptr> visitednodes ;
    for(set_iter = temp_nodes.begin();set_iter != temp_nodes.end();set_iter++)
    {
        if((*set_iter)->in_edges.size() == 0)
        {
            (*set_iter)->visited = true;
            visitednodes.push(*set_iter);
            break;
        }
    }
    while(!visitednodes.empty())
    {
         boxm2_block_vis_graph_node_sptr node = visitednodes.front();

         ordered_nodes_.push_back(node->id_);
         vcl_cout<<node->id_<<": ";
         for(vcl_set<boxm2_block_id>::iterator outiter = node->out_edges.begin(); outiter !=node->out_edges.end();outiter++)
         {
 
             vcl_list<boxm2_block_vis_graph_node_sptr>::iterator find_iter = this->find( boxm2_block_id(*outiter), temp_nodes );
             if(find_iter !=  temp_nodes.end()   )
                 if(! (*find_iter)->visited)
                 {
                    visitednodes.push(*find_iter);
                    (*find_iter)->visited = true;
                    vcl_cout<<*outiter<<"  ";
                 }
         }
         vcl_cout<<vcl_endl;
         visitednodes.pop();
    }
}
vcl_list<boxm2_block_vis_graph_node_sptr>::iterator 
    boxm2_block_vis_graph::find(const boxm2_block_id & id, vcl_list<boxm2_block_vis_graph_node_sptr> & list_nodes )
{
    vcl_list<boxm2_block_vis_graph_node_sptr>::iterator iter = list_nodes.begin();
    for (; iter!= list_nodes.end(); iter++)
    {
        if(id == (*iter)->id_ )
            return iter;
    }
    return list_nodes.end();
}
