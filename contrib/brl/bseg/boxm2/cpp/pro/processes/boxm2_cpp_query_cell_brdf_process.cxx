// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_query_cell_brdf_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for querying  cell data at a given 3-d location
//
// \author Vishal Jain
// \date Mar 10, 2011

#include <vcl_fstream.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/io/boxm2_stream_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/boxm2_util.h>
//brdb stuff
#include <brdb/brdb_value.h>
#include <bsta/bsta_joint_histogram.h>
//directory utility
#include <vcl_where_root_dir.h>
#include <boct/boct_bit_tree2.h>
#include <brad/brad_phongs_model_est.h>

//#define SEGLEN_FACTOR 93206.7555f   //Hack representation of int32.maxvalue/(ni*nj*block_length)

namespace boxm2_cpp_query_cell_brdf_process_globals
{
  const unsigned n_inputs_ =  9;
  const unsigned n_outputs_ = 0;

  typedef boxm2_data_traits<BOXM2_FLOAT8>::datatype float8_datatype;
  typedef boxm2_data_traits<BOXM2_AUX0>::datatype aux0_datatype;
  typedef boxm2_data_traits<BOXM2_AUX1>::datatype aux1_datatype;
  typedef boxm2_data_traits<BOXM2_AUX2>::datatype aux2_datatype;
  typedef boxm2_data_traits<BOXM2_AUX3>::datatype aux3_datatype;
}

bool boxm2_cpp_query_cell_brdf_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_query_cell_brdf_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "boxm2_stream_cache_sptr";
  input_types_[3] = "float"; //x
  input_types_[4] = "float"; //y
  input_types_[5] = "float"; //z
  input_types_[6] = "float"; //y
  input_types_[7] = "float"; //z
  input_types_[8] = "vil_image_view_base_sptr"; //z

  vcl_vector<vcl_string>  output_types_(n_outputs_);
  //for (unsigned i=0;i<2;i++)       output_types_[i] = "float";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool  boxm2_cpp_query_cell_brdf_process(bprb_func_process& pro)
{
  using namespace boxm2_cpp_query_cell_brdf_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache= pro.get_input<boxm2_cache_sptr>(i++);
  boxm2_stream_cache_sptr str_cache= pro.get_input<boxm2_stream_cache_sptr>(i++);
  float x =pro.get_input<float>(i++);
  float y =pro.get_input<float>(i++);
  float z =pro.get_input<float>(i++);
  float sun_elev =pro.get_input<float>(i++);
  float sun_azim =pro.get_input<float>(i++);
  vil_image_view_base_sptr img  = pro.get_input<vil_image_view_base_sptr>(i++);

  vil_image_view_base_sptr float_img =boxm2_util::prepare_input_image(img);
  bsta_histogram_sptr hist_base =  boxm2_util::generate_image_histogram(float_img, 8);

  bsta_histogram<float> * hist =static_cast<bsta_histogram<float> *> (hist_base.ptr());
  // set arguments
  vgl_point_3d<double> local;
  boxm2_block_id id;
  if (!scene->contains(vgl_point_3d<double>(x, y, z), id, local))
    return false;

  int index_x=(int)vcl_floor(local.x());
  int index_y=(int)vcl_floor(local.y());
  int index_z=(int)vcl_floor(local.z());
  boxm2_block * blk=cache->get_block(id);
  boxm2_block_metadata mdata = scene->get_block_metadata_const(id);
  vnl_vector_fixed<unsigned char,16> treebits=blk->trees()(index_x,index_y,index_z);
  boct_bit_tree2 tree(treebits.data_block(),mdata.max_level_);
  int bit_index=tree.traverse(local);
  int index=tree.get_data_index(bit_index,false);
  boxm2_data_base          *  phongs_base  = cache->get_data_base(id,boxm2_data_traits<BOXM2_FLOAT8>::prefix("phongs_model"));
  boxm2_data<BOXM2_FLOAT8> *  phongs_data  = new boxm2_data<BOXM2_FLOAT8>(phongs_base->data_buffer(),
                                                                          phongs_base->buffer_length(),
                                                                          phongs_base->block_id());

  vcl_cout<<"id = "<<id;
  vcl_vector<aux0_datatype>  aux0_raw   = str_cache->get_random_i<BOXM2_AUX0>(id, index);
  vcl_vector<aux1_datatype>  aux1_raw   = str_cache->get_random_i<BOXM2_AUX1>(id, index);
  vcl_vector<aux2_datatype>  aux2_raw   = str_cache->get_random_i<BOXM2_AUX2>(id, index);
  vcl_vector<aux3_datatype>  aux3_raw   = str_cache->get_random_i<BOXM2_AUX3>(id, index);

  for (unsigned m = 0; m < aux0_raw.size(); m++) {
    if (aux0_raw[m]>1e-10f)
    {
      aux1_raw[m] /=aux0_raw[m];
      aux2_raw[m] /=aux0_raw[m];
      aux3_raw[m] /=aux0_raw[m];
    }
    else
    {
      aux1_raw[m] = 0.0;
      aux2_raw[m] = 0.0;
    }
  }

  int half_m = aux0_raw.size()/2;

  vcl_vector<aux1_datatype> Iobs;
  vcl_vector<aux2_datatype> vis;
  vcl_vector<aux1_datatype> xdir;
  vcl_vector<aux2_datatype> ydir;
  vcl_vector<aux3_datatype> zdir;

  Iobs.insert(Iobs.begin(), aux1_raw.begin(), aux1_raw.begin()+half_m);
  vis.insert(vis.begin(), aux2_raw.begin(), aux2_raw.begin()+half_m);

  xdir.insert(xdir.begin(), aux1_raw.begin()+half_m, aux1_raw.end());
  ydir.insert(ydir.begin(), aux2_raw.begin()+half_m, aux2_raw.end());
  zdir.insert(zdir.begin(), aux3_raw.begin()+half_m, aux3_raw.end());

  typedef boxm2_data_traits<BOXM2_FLOAT8>::datatype boxm2_float8_datatype;
  boxm2_array_1d<boxm2_float8_datatype> phongs_data_array=phongs_data->data();
  boxm2_float8_datatype model=phongs_data_array[index];

  brad_phongs_model pmodel(model[0],
                           model[1],
                           model[2],
                           model[3],
                           model[4]);

  vcl_cout<<"Phongs Model ["<<model[0]<<','
                            <<model[1]<<','
                            <<model[2]<<','
                            <<model[3]<<','
                            <<model[4]<<','
                            <<model[5]<<']'<<vcl_endl;

  vcl_cout<<"No of Obs  "<<Iobs.size()<<vcl_endl;

  bsta_histogram<float> data_histogram(0.0f,1.0f,8);

  for (unsigned i = 0; i< Iobs.size(); i++)
  {
    float pmdensity = 0.0f;
    float phist = 0.0f;
    if (vis[i] > 0.0)
    {
      vnl_double_3 view_dir(xdir[i],ydir[i],zdir[i]);
      float Imodel = pmodel.val(view_dir,sun_elev,sun_azim);

      pmdensity = 1/vcl_sqrt(2*vnl_math::pi*model[5])*vcl_exp(-(Iobs[i]-Imodel)*(Iobs[i]-Imodel)/(2*model[5]));
      unsigned int bin_index = (int) vcl_floor(Iobs[i]* 8);
      bin_index = bin_index > 7 ? 7:bin_index;
      phist =hist->p(bin_index) * hist->nbins(); // this factor of 8.0 represents density
      data_histogram.upcount(Iobs[i],vis[i]);
      vcl_cout.precision(4);
      vcl_cout<<Iobs[i]<<'\t'<<vis[i]<<'\t'<<pmdensity<<'\t'<<phist<<vcl_endl;
    }
  }

  vcl_cout<<"Histograms"<<vcl_endl;

  for (unsigned i = 0 ; i < 8; i++)
    vcl_cout<<hist->p(i)<<'\t'<<data_histogram.p(i)<<vcl_endl;
  bsta_joint_histogram<float> joint_hist(-1.0f,1.0f,8,0.0f,1.0f,8);
  bsta_histogram<float> gradI_histogram(0.0f,1.0f,32);
  for (unsigned i =0 ; i < Iobs.size()-1; i++)
  {
    //for (unsigned j =i+1 ; j < Iobs.size(); j++)
    unsigned int j = i + 1;
    {
      if (vis[i] > 0.0f && vis[j] > 0.0f)
      {
        float costheta = xdir[i]*xdir[j]+ydir[i]*ydir[j]+zdir[i]*zdir[j];
        float gradI  = vcl_fabs(Iobs[i]-Iobs[j]);
        joint_hist.upcount(costheta,vis[i],gradI,vis[j]);
        gradI_histogram.upcount(gradI, (vis[i]+vis[j])/2);
      }
    }
  }
  for (unsigned i =0 ; i < joint_hist.nbins_a(); i++)
  {
    for (unsigned j =0 ; j < joint_hist.nbins_b(); j++)
    {
      vcl_cout<<joint_hist.counts()(i,j)/joint_hist.volume()<<' ';
    }
    vcl_cout<<vcl_endl;
  }

  vcl_cout<<"Entropy is  "<<joint_hist.entropy()<<'\n'
          <<"Entropy is  "<<gradI_histogram.entropy()/gradI_histogram.nbins()<<vcl_endl;
  return true;
}
