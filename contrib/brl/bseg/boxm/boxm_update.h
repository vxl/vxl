#ifndef boxm_update_h_
#define boxm_update_h_

#include <boct/boct_tree.h>
#include <boct/boct_tree_cell.h>
#include <boxm/boxm_utils.h>
#include <boxm/boxm_block.h>
#include <boxm/boxm_cell_vis_graph_iterator.h>
#include <boxm/boxm_block_vis_graph_iterator.h>
#include <vil/vil_math.h>
#include <vul/vul_timer.h>
#include <boxm/boxm_render_image.h>

//: functor used for normalizing cell_expected image
class safe_inverse_functor
{
public:
  safe_inverse_functor(float tol = 0.0f) : tol_(tol) {}

  float operator()(float x)      const { return (x > tol_)? 1.0f/x : 0.0f; }

  float tol_;
};

template <class T_loc, boxm_apm_type APM>
void boxm_update_pass1(boxm_scene<boct_tree<T_loc, boxm_sample<APM> > > &scene, 
					   vpgl_camera_double_sptr cam, 
					   vil_image_view<typename boxm_apm_traits<APM>::obs_mathtype> &img,
					   vil_image_view<float> &norm_img,
					   typename boxm_apm_traits<APM>::apm_datatype background_model)
{

  typedef boct_tree<T_loc, boxm_sample<APM> > tree_type;
  unsigned ni=img.ni();
  unsigned nj=img.nj();
  vil_image_view<float> pre(ni,nj,1); pre.fill(0.0f);
  vil_image_view<float> vis(ni,nj,1); vis.fill(1.0f);
  vil_image_view<float> alpha_integral(ni,nj,1); alpha_integral.fill(0.0f);
  vil_image_view<float> PI_img(ni,nj,1); PI_img.fill(0.0f);

  // code to iterate over the blocks in order of visibility
  boxm_block_vis_graph_iterator<boct_tree<T_loc, boxm_sample<APM> > > block_vis_iter(cam, &scene, ni,nj);
  int cnt=0;
  while (block_vis_iter.next()) {
    vcl_vector<vgl_point_3d<int> > block_indices = block_vis_iter.frontier_indices();
    for (unsigned i=0; i<block_indices.size(); i++) { // code for each block
      scene.load_block(block_indices[i].x(),block_indices[i].y(),block_indices[i].z());
      boxm_block<tree_type> * curr_block=scene.get_active_block();
      // project vertices to the image determine which faces of the cell are visible
      boxm_cell_vis_graph_iterator<T_loc, boxm_sample<APM> > frontier_it(cam,curr_block->get_tree(),ni,nj);

      // for each frontier layer of each block
      boct_tree<T_loc,boxm_sample<APM> > * tree=curr_block->get_tree();
	  vil_image_view<float> front_xyz(ni,nj,3);
	  vil_image_view<float> back_xyz(ni,nj,3); 
	  vil_image_view<float> alphas(ni,nj,1);
	  vil_image_view<float> vis_end(ni,nj,1);
	  vil_image_view<float> temp_expected(ni,nj,1);

      while (frontier_it.next())
	  {
        vcl_vector<boct_tree_cell<T_loc, boxm_sample<APM> > *> vis_cells=frontier_it.frontier();
        typename vcl_vector<boct_tree_cell<T_loc, boxm_sample<APM> > *>::iterator cell_it=vis_cells.begin();
        front_xyz.fill(0.0f);
        back_xyz.fill(0.0f);
        alphas.fill(0.0f);
        vis_end.fill(0.0f);
        temp_expected.fill(0.0f);
		vcl_cout<<".";
        for (;cell_it!=vis_cells.end();cell_it++)
        {
			// for each cell
			boxm_sample<APM> sample=(*cell_it)->data();
			// get vertices of cell in the form of a bounding box (cells are always axis-aligned))
			vgl_box_3d<double> cell_bb = tree->cell_bounding_box(*cell_it);
			vcl_vector<vgl_point_3d<double> > corners=boxm_utils::corners_of_box_3d(cell_bb);
			double * xverts=new double[8]; double *yverts=new double[8];
			boxm_utils::project_corners(corners,cam,xverts,yverts);
			boct_face_idx  vis_face_ids=boxm_utils::visible_faces(cell_bb,cam,xverts,yverts);
			boxm_utils::project_cube_xyz(corners,vis_face_ids,front_xyz,back_xyz,xverts,yverts);
			// get  alpha
			boxm_utils::project_cube_fill_val( vis_face_ids,alphas,sample.alpha, xverts,yverts);
			typename boxm_apm_traits<APM>::obs_datatype cell_mean_obs;
			if (boxm_utils::cube_uniform_mean(vis_face_ids, img, cell_mean_obs,xverts,yverts)) {
				// get probability density of mean observation
				float cell_PI = boxm_apm_traits<APM>::apm_processor::prob_density(sample.appearance, cell_mean_obs);
				if (!((cell_PI >= 0) && (cell_PI < 1e8)) ) {
					vcl_cout << vcl_endl << "cell_PI = " << cell_PI << vcl_endl;
					vcl_cout << "  cell_obs = " << cell_mean_obs << vcl_endl;
					vcl_cout << "  cell id = " << *cell_it << vcl_endl; 
				}
				// fill obs probability density image
				boxm_utils::project_cube_fill_val(vis_face_ids,PI_img,cell_PI, xverts,yverts);
			}
			delete [] xverts;
			delete [] yverts;
        }
        // compute the length of ray segment at each pixel
        vil_image_view<float> len_seg(ni,nj,1);
        len_seg.fill(0.0f);
        vil_math_image_difference<float,float>(back_xyz,front_xyz,back_xyz);
        vil_math_sum_sqr<float,float>(back_xyz,len_seg);
        vil_math_sqrt<float>(len_seg);

		vil_math_image_product(len_seg,alphas, alphas);
        // compute visibility
        vil_math_image_difference(alpha_integral, alphas, alpha_integral);
        // compute new vis image
        image_exp_functor exp_fun;
        vil_transform(alpha_integral,vis_end,exp_fun);
        // compute weights for each pixel
        vil_math_image_difference(vis,vis_end,vis);
        // scale cell expected image by weighting function..
        vil_math_image_product( PI_img,vis, PI_img);
        // ..and use result to update final expected image
        vil_math_image_sum(PI_img,pre,pre);

        vis.deep_copy(vis_end);


	  }
	}
  }
  vil_image_view<float> PI_background(norm_img.ni(),norm_img.nj(),1);
  for (unsigned int j=0; j<img.nj(); ++j) {
	  for (unsigned int i=0; i<img.ni(); ++i) {
		  // this will have to be modified slightly when we deal with multi-plane images -dec
		  PI_background(i,j) = boxm_apm_traits<APM>::apm_processor::prob_density(background_model, img(i,j));
	  }
  }
  vil_save(pre,"./pre.tiff");
  vil_math_image_product(PI_background, vis, norm_img);
  vil_math_image_sum(pre,norm_img,norm_img);
  safe_inverse_functor inv_func(1e-8f);
  vil_transform(norm_img, inv_func);

}




template <class T_loc, boxm_apm_type APM>
void boxm_update_pass2(boxm_scene<boct_tree<T_loc, boxm_sample<APM> > > &scene, 
					   vpgl_camera_double_sptr cam, 
					   vil_image_view<typename boxm_apm_traits<APM>::obs_mathtype> &img,
					   vil_image_view<float> &norm_img)
{
  unsigned ni=img.ni();
  unsigned nj=img.nj();
	typedef boct_tree<T_loc, boxm_sample<APM> > tree_type;
	vil_image_view<float> pre_img(ni,nj,1); pre_img.fill(0.0f);
	vil_image_view<float> vis(ni,nj,1); vis.fill(1.0f);
	vil_image_view<float> alpha_integral(ni,nj,1); alpha_integral.fill(0.0f);
	vil_image_view<float> PI_img(ni,nj,1); PI_img.fill(0.0f);
	vil_image_view<float> pix_weights(ni,nj,1); 

	vul_timer t;
	t.mark();
	// code to iterate over the blocks in order of visibility
	boxm_block_vis_graph_iterator<boct_tree<T_loc, boxm_sample<APM> > > block_vis_iter(cam, &scene, ni,nj);
	int cnt=0;
	while (block_vis_iter.next()) {
		vcl_vector<vgl_point_3d<int> > block_indices = block_vis_iter.frontier_indices();
		for (unsigned i=0; i<block_indices.size(); i++) { // code for each block
			scene.load_block(block_indices[i].x(),block_indices[i].y(),block_indices[i].z());
			boxm_block<tree_type> * curr_block=scene.get_active_block();
			// project vertices to the image determine which faces of the cell are visible
			boxm_cell_vis_graph_iterator<T_loc, boxm_sample<APM> > frontier_it(cam,curr_block->get_tree(),ni,nj);

			// for each frontier layer of each block
			boct_tree<T_loc,boxm_sample<APM> > * tree=curr_block->get_tree();
			vil_image_view<float> front_xyz(ni,nj,3);
			vil_image_view<float> back_xyz(ni,nj,3); 
			vil_image_view<float> alphas(ni,nj,1);
			vil_image_view<float> vis_end(ni,nj,1);
			vil_image_view<float> temp_expected(ni,nj,1);
			vil_image_view<float> update_factor(ni,nj,1);

			while (frontier_it.next())
			{
				vcl_vector<boct_tree_cell<T_loc, boxm_sample<APM> > *> vis_cells=frontier_it.frontier();
				typename vcl_vector<boct_tree_cell<T_loc, boxm_sample<APM> > *>::iterator cell_it=vis_cells.begin();
				front_xyz.fill(0.0f);
				back_xyz.fill(0.0f);
				alphas.fill(0.0f);
				vis_end.fill(0.0f);
				temp_expected.fill(0.0f);
				vcl_cout<<".";
				for (;cell_it!=vis_cells.end();cell_it++)
				{
					// for each cell
					boxm_sample<APM> sample=(*cell_it)->data();

					// get vertices of cell in the form of a bounding box (cells are always axis-aligned))
					vgl_box_3d<double> cell_bb = tree->cell_bounding_box(*cell_it);
					vcl_vector<vgl_point_3d<double> > corners=boxm_utils::corners_of_box_3d(cell_bb);
					double * xverts=new double[8]; double *yverts=new double[8];
					boxm_utils::project_corners(corners,cam,xverts,yverts);
					boct_face_idx  vis_face_ids=boxm_utils::visible_faces(cell_bb,cam,xverts,yverts);
					boxm_utils::project_cube_xyz(corners,vis_face_ids,front_xyz,back_xyz,xverts,yverts);
					// get  alpha
					boxm_utils::project_cube_fill_val( vis_face_ids,alphas,sample.alpha, xverts,yverts);
					typename boxm_apm_traits<APM>::obs_datatype cell_mean_obs;
					if (boxm_utils::cube_uniform_mean(vis_face_ids, img, cell_mean_obs,xverts,yverts)) {
						// get probability density of mean observation
						float cell_PI = boxm_apm_traits<APM>::apm_processor::prob_density(sample.appearance, cell_mean_obs);
						//if (!((cell_PI >= 0) && (cell_PI < 1e8)) ) {
						//	vcl_cout << vcl_endl << "cell_PI = " << cell_PI << vcl_endl;
						//	vcl_cout << "  cell_obs = " << cell_mean_obs << vcl_endl;
						//	vcl_cout << "  cell id = " << *cell_it << vcl_endl; 
						//}
						// fill obs probability density image
						boxm_utils::project_cube_fill_val(vis_face_ids,PI_img,(float)cell_PI, xverts,yverts);
					}
					float cell_mean_vis = 0.0f;
					if (boxm_utils::cube_uniform_mean(vis_face_ids, vis, cell_mean_vis,xverts,yverts)) {
						// update appearance model
						if (cell_mean_vis > 1e-6) {
							boxm_apm_traits<APM>::apm_processor::update(sample.appearance, cell_mean_obs, cell_mean_vis);
						}
					}
					(*cell_it)->set_data(sample);
					delete [] xverts;
					delete [] yverts;

				}
				// compute the length of ray segment at each pixel
				vil_image_view<float> len_seg(ni,nj,1);
				len_seg.fill(0.0f);
				vil_math_image_difference<float,float>(back_xyz,front_xyz,back_xyz);
				vil_math_sum_sqr<float,float>(back_xyz,len_seg);
				vil_math_sqrt<float>(len_seg);

				vil_math_image_product(len_seg,alphas, alphas);
				// compute visibility
				vil_math_image_difference(alpha_integral, alphas, alpha_integral);
				// compute new vis image
				image_exp_functor exp_fun;
				vil_transform(alpha_integral,vis_end,exp_fun);
				// compute weights for each pixel
				vil_math_image_difference(vis,vis_end,pix_weights);
				// multiply PI by vis..
				vil_math_image_product(PI_img, vis, update_factor);
				// ..add preX to get numerator term for update..
				vil_math_image_sum(pre_img, update_factor, update_factor);
				//.. and normalize
				vil_math_image_product(norm_img, update_factor, update_factor);

				float max_cell_P=0.99f;
				float min_cell_P=0.001f;
				for (cell_it=vis_cells.begin();cell_it!=vis_cells.end();cell_it++)
				{
					// for each cell
					boxm_sample<APM> sample=(*cell_it)->data();
					// get vertices of cell in the form of a bounding box (cells are always axis-aligned))
					vgl_box_3d<double> cell_bb = tree->cell_bounding_box(*cell_it);
					vcl_vector<vgl_point_3d<double> > corners=boxm_utils::corners_of_box_3d(cell_bb);
					double * xverts=new double[8]; double *yverts=new double[8];
					boxm_utils::project_corners(corners,cam,xverts,yverts);
					boct_face_idx  vis_face_ids=boxm_utils::visible_faces(cell_bb,cam,xverts,yverts);
					boxm_utils::project_cube_xyz(corners,vis_face_ids,front_xyz,back_xyz,xverts,yverts);
					float mean_update_factor = 0.0f;
					if (boxm_utils::cube_uniform_mean(vis_face_ids, update_factor, mean_update_factor,xverts,yverts))
					{	// update alpha value
						sample.alpha *= mean_update_factor;
						// do bounds check on new alpha value
						float cell_len = float(cell_bb.max_x() - cell_bb.min_x());
						float max_alpha = -vcl_log(1.0f - max_cell_P)/cell_len;
						float min_alpha = -vcl_log(1.0f - min_cell_P)/cell_len;
						if (sample.alpha > max_alpha) 
							sample.alpha = max_alpha;
						
						if (sample.alpha < min_alpha) 
							sample.alpha = min_alpha;
						
						if (!((sample.alpha >= min_alpha) && (sample.alpha <= max_alpha)) ){
							vcl_cerr << vcl_endl << "error: cell.alpha = " << sample.alpha << vcl_endl;
							vcl_cerr << "mean_update_factor = " << mean_update_factor << vcl_endl;
						}
											(*cell_it)->set_data(sample);

					}

					delete [] xverts;
					delete [] yverts;
				}
				// multiply cell_PI by cell weights
				vil_math_image_product(pix_weights, PI_img, PI_img);
				// update pre_img 
				vil_math_image_sum(PI_img, pre_img, pre_img);

				vis.deep_copy(vis_end);
			}
		}
	}

}

template <class T_loc, boxm_apm_type APM>
void boxm_update(boxm_scene<boct_tree<T_loc, boxm_sample<APM> > > &scene, 
				 vil_image_view<typename boxm_apm_traits<APM>::obs_mathtype> &img,
				 vpgl_camera_double_sptr cam,
				 bool black_background = false)
{
  typename boxm_apm_traits<APM>::apm_datatype background_apm;
  
  if (black_background) {
    vcl_cout << "using black background model" << vcl_endl;
    for (unsigned int i=0; i<4; ++i) {
      boxm_apm_traits<APM>::apm_processor::update(background_apm, 0.0f, 1.0f);
      float peak = boxm_apm_traits<APM>::apm_processor::prob_density(background_apm,0.0f);
    }
  }
    vil_image_view<float> norm_img(img.ni(), img.nj(), 1);

  boxm_update_pass1<T_loc,APM>(scene, cam,img,norm_img,background_apm);

  vcl_cout << "update: pass1 completed" << vcl_endl;
  
  boxm_update_pass2<T_loc,APM>(scene, cam,img,norm_img);

  vcl_cout << "update: pass2 completed" << vcl_endl;

  return;
}

#endif