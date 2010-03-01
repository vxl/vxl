#ifndef boxm_render_expected_edge_tangent_image_functor_h
#define boxm_render_expected_edge_tangent_image_functor_h
//:
// \file
#include <boxm/boxm_apm_traits.h>
#include <boxm/opt/boxm_raytrace_function.h>
#include <boxm/opt/boxm_rt_sample.h>
#include <boxm/boxm_simple_grey_processor.h>
#include <boxm/boxm_mob_grey_processor.h>
#include <boxm/boxm_edge_tangent_sample.h>
#include <boxm/boxm_inf_line_sample.h>
#include <vnl/vnl_math.h>
#include <vil/vil_transform.h>
#include <sdet/sdet_img_edge.h>
#ifdef DEBUG
#include <vcl_iostream.h>
#endif

template <boxm_apm_type APM, class T_aux>
class boxm_render_expected_edge_tangent_image_functor
{
 public:
  //: "default" constructor
  boxm_render_expected_edge_tangent_image_functor(vil_image_view<typename boxm_apm_traits<APM>::obs_datatype> &expected,
                                                  vil_image_view<float> &mask_vis,
                                                  unsigned int /*ni*/=0, unsigned /*nj*/=0,
                                                  bool scene_read_only=false,
                                                  bool is_aux=true)
    : expected_(expected)
  {
    typename boxm_apm_traits<APM>::obs_datatype nil(0);
    expected_.fill(nil);
    scene_read_only_=scene_read_only;
    is_aux_=is_aux;
  }

  inline bool step_cell(unsigned int i, unsigned int j,
                        vgl_point_3d<double> s0,
                        vgl_point_3d<double> s1,
                        //boxm_inf_line_sample<typename boxm_apm_traits<APM>::obs_datatype> &cell_value,
                        float &cell_value,
                        T_aux aux_val)
  {
    typedef typename boxm_apm_traits<APM>::obs_datatype t;

    // compute segment length
    const float seg_len = (float)(s1-s0).length(); // FIXME: unused...
    if (expected_(i,j) < 0.01)
      expected_(i,j)=cell_value;
    return false;
  }

  bool scene_read_only_;
  bool is_aux_;

 private:
  vil_image_view<typename boxm_apm_traits<APM>::obs_datatype> &expected_;
};

template <class T_loc, class T_data>
void boxm_render_edge_tangent_image_rt(boxm_scene<boct_tree<T_loc, T_data > > &scene,
                                       vpgl_camera_double_sptr cam,
                                       vil_image_view<float> &expected,
                                       vil_image_view<float> & mask,
                                       float n_normal,
                                       unsigned int num_samples, //degree of freedom
                                       int bin = -1)
{
  
  //convert the tree, so that the cells that have and edge has a value, the rest are zero
  typedef boct_tree<short, boxm_inf_line_sample<float> > type;
  boxm_block_iterator<type> iter(&scene);
  boxm_scene<boct_tree<T_loc, float> > scene2(scene.lvcs(),scene.origin(),
    scene.block_dim(),scene.world_dim(),scene.max_level(),scene.init_level());

  iter.begin();
  while (!iter.end())
  {
    scene.load_block(iter.index());
    boxm_block<type>* block = *iter;
    type* tree = block->get_tree();

    // clone the tree to the new type
    scene2.load_block(iter.index());
    boct_tree<T_loc, float>* tree2=tree->clone_to_type<float>();

    vcl_vector<boct_tree_cell<short,boxm_inf_line_sample<float> >*> cells = tree->leaf_cells();
    vcl_vector<boct_tree_cell<short,float>*> cells2 = tree2->leaf_cells();

    // iterate over cells
    for (unsigned i=0; i<cells.size(); ++i)
    {
      boct_tree_cell<short,boxm_inf_line_sample<float> >* cell = cells[i];
      boxm_inf_line_sample<float> data = cell->data();
      vgl_infinite_line_3d<float> line = data.line_;
      vgl_vector_3d<double> dir(line.direction().x(), line.direction().y(), line.direction().z());
      cells2[i]->set_data(0.0);
      if (line.x0().x()!=0 && line.x0().y()!=0) {
        vgl_point_3d<double> p0,p1;
        vgl_box_3d<double> bb = tree->cell_bounding_box(cell);
        // convert to line type (from float to double)
        vgl_vector_2d<double> x0(line.x0().x(), line.x0().y());
        vgl_infinite_line_3d<double> dline(x0,dir);
        if (vgl_intersection<double>(bb, dline, p0, p1)) 
          cells2[i]->set_data(100.0);
      }
    }
    boxm_block<boct_tree<T_loc, float> >* block2=scene2.get_active_block();
    boct_tree<T_loc, float> *t = block2->get_tree();
    delete t;
    block2->set_tree(tree2);
    scene2.write_active_block();
    iter++;
  }

  typedef boxm_aux_traits<BOXM_AUX_NULL>::sample_datatype sample_datatype;
  boxm_aux_scene<T_loc, float, boxm_edge_tangent_sample<sample_datatype> > aux_scene
    (&scene2,boxm_aux_traits<BOXM_AUX_NULL>::storage_subdir(), boxm_aux_scene<T_loc, float,boxm_edge_tangent_sample<sample_datatype> >::LOAD);
  typedef boxm_render_expected_edge_tangent_image_functor<BOXM_EDGE_LINE,boxm_edge_tangent_sample<sample_datatype> > expfunctor;
  boxm_raytrace_function<expfunctor,T_loc, float, boxm_edge_tangent_sample<sample_datatype> > raytracer
    (scene2,aux_scene, cam.ptr(),expected.ni(),expected.nj());
  expfunctor exp_functor(expected,mask,expected.ni(),expected.nj(),true,false);
  raytracer.run(exp_functor);
#if 0
  normalize_expected_functor_edge<T_data> norm_fn(n_normal,num_samples-1);
  vil_transform2<float,float, normalize_expected_functor_edge<T_data> >(mask,expected,norm_fn);
#endif // 0
}

#endif
