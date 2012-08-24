// This is brl/bseg/boxm2/pro/processes/boxm2_ingest_convex_mesh_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for exporting a texture mapped mesh of a scene
//
// \author Vishal Jain
// \date Aug 22, 2012

#include <vcl_fstream.h>
#include <vul/vul_file.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>
#include <boct/boct_bit_tree.h>

//vil includes
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#include <vil/vil_math.h>

//vgl
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <bvgl/bvgl_triangle_interpolation_iterator.h>

//vpgl camera stuff
#include <vpgl/vpgl_perspective_camera.h>
#include <imesh/imesh_fileio.h>
#include <imesh/imesh_operations.h>
#include <imesh/algo/imesh_intersect.h>
#include <imesh/imesh_face.h>

#include <bmsh3d/bmsh3d_mesh.h>
#include <bmsh3d/algo/bmsh3d_fileio.h>
#include <bmsh3d/bmsh3d_mesh_mc.h>
#include <vgl/vgl_intersection.h>
namespace boxm2_ingest_convex_mesh_process_globals
{
  const unsigned n_inputs_ = 5;
  const unsigned n_outputs_ = 0;

  typedef vnl_vector_fixed<unsigned char, 16> uchar16;
}

bool boxm2_ingest_convex_mesh_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ingest_convex_mesh_process_globals;

  //process takes 2 inputs
  int i=0;
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[i++] = "boxm2_scene_sptr";  // scene
  input_types_[i++] = "boxm2_cache_sptr";  // scene
  input_types_[i++] = "vcl_string";        // input ply file 
  input_types_[i++] = "int";			   // label id 
  input_types_[i++] = "vcl_string";        // Name of the category 

  // process has 1 output
  vcl_vector<vcl_string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_ingest_convex_mesh_process(bprb_func_process& pro)
{
  using namespace boxm2_ingest_convex_mesh_process_globals;
  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get the inputs
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(0);
  boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(1);
  vcl_string ply_file = pro.get_input<vcl_string>(2);
  int label_id  = pro.get_input<int>(3);
  vcl_string identifier = pro.get_input<vcl_string>(4);

  //: IO for mesh
  vgl_box_3d<double> bbox;
  bmsh3d_mesh_mc *  bmesh = new bmsh3d_mesh_mc();
  bmsh3d_load_ply(bmesh,ply_file.c_str());
  bmesh->IFS_to_MHE();
  bmesh->orient_face_normals();
  vcl_map<int, bmsh3d_face* > fmap = bmesh->facemap();
  vcl_map<int, bmsh3d_face* >::iterator face_it ;
  for ( unsigned k = 0 ; k < bmesh->num_vertices() ; k++)
  {
	  bmsh3d_vertex * v  = bmesh->vertexmap(k);
	  bbox.add(v->get_pt());
  }
 
  vcl_map<boxm2_block_id, boxm2_block_metadata> blocks=scene->blocks(); //->get_block_ids();
  vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator iter;
  for(iter = blocks.begin(); iter!= blocks.end(); iter ++)
  {
	  boxm2_block_metadata mdata = iter->second;
	  vgl_box_3d<double> rbox = vgl_intersection<double>(mdata.bbox(),bbox);

	  if(!rbox.is_empty())
	  {
		
		  vcl_cout<<" Paint Cells in block "<<iter->first<<vcl_endl;
		  vgl_vector_3d<double> min_dir = rbox.min_point()-mdata.bbox().min_point();
		  vgl_vector_3d<double> max_dir = rbox.max_point()-mdata.bbox().min_point();
		
		  unsigned int min_i = (unsigned int ) vcl_floor(min_dir.x()/mdata.sub_block_dim_.x());
		  unsigned int min_j = (unsigned int ) vcl_floor(min_dir.y()/mdata.sub_block_dim_.y());
		  unsigned int min_k = (unsigned int ) vcl_floor(min_dir.z()/mdata.sub_block_dim_.z());

		  unsigned int max_i = (unsigned int ) vcl_floor(max_dir.x()/mdata.sub_block_dim_.x());
		  unsigned int max_j = (unsigned int ) vcl_floor(max_dir.y()/mdata.sub_block_dim_.y());
		  unsigned int max_k = (unsigned int ) vcl_floor(max_dir.z()/mdata.sub_block_dim_.z());

		  vcl_cout<<"("<<min_i<<","<<min_j<<","<<min_k<<")"<<vcl_endl;
		  vcl_cout<<"("<<max_i<<","<<max_j<<","<<max_k<<")"<<vcl_endl;
		  // read the trees info 
		  boxm2_block *     blk  = cache->get_block(iter->first);
		  boxm2_data_base *  alpha  = cache->get_data_base(iter->first,boxm2_data_traits<BOXM2_ALPHA>::prefix(),0,true);
		  int len_buffer  = alpha->buffer_length()/4*boxm2_data_info::datasize("boxm2_label_short") ;
		  boxm2_data_base *  label_data_base  = new boxm2_data_base(new char[len_buffer],len_buffer,iter->first, false) ;
		  boxm2_data<BOXM2_LABEL_SHORT> label_data(label_data_base->data_buffer(),label_data_base->buffer_length(),iter->first);
		  boxm2_array_3d<uchar16>  trees = blk->trees()   ;
		  for(unsigned int i = min_i ; i <= max_i; i++)
			  for(unsigned int j = min_j ; j <= max_j; j++)
				  for(unsigned int k = min_k ; k <= max_k; k++)
				  {
					  boct_bit_tree tree( trees(i,j,k).data_block() );
					  vcl_vector<int> leaf_bits = tree.get_leaf_bits();
					  for(unsigned t = 0 ; t < leaf_bits.size(); t++)
					  {
						  vgl_point_3d<double> cc = tree.cell_center(leaf_bits[t]);
						  vgl_point_3d<double> global_cc(((double)i + cc.x())*mdata.sub_block_dim_.x() + mdata.bbox().min_x(),
														 ((double)j + cc.y())*mdata.sub_block_dim_.y() + mdata.bbox().min_y(),
														 ((double)k + cc.z())*mdata.sub_block_dim_.z() + mdata.bbox().min_z());
						  bool inside = true ;
						  face_it = fmap.begin();
						  while (face_it != fmap.end()) {
							  bmsh3d_face_mc* face = (bmsh3d_face_mc*) face_it->second;
							  vgl_point_3d<double> pt =  face->compute_center_pt();
							  vgl_vector_3d<double> normal = face->compute_normal();
							  double dotprod=dot_product<double>(pt-global_cc,normal);
							  if (dotprod<0)
								 inside = false ;
							  face_it++;
						  }
						  if(inside)
						  {
								int dataindex = tree.get_data_index(t);
								label_data.data()[dataindex] = label_id;
						  }


					  }

					  
				  }
				  boxm2_sio_mgr::save_block_data_base(scene->data_path(), iter->first, label_data_base,boxm2_data_traits<BOXM2_LABEL_SHORT>::prefix(identifier));
	  }
  }
  return true;
}
