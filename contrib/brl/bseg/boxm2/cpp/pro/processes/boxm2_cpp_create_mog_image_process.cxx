// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_create_mog_image_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process to compute the density for each pixel in an image given a scene model
//
// \author Ozge C. Ozcanli
// \date July 07, 2011

#include <vcl_fstream.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
//#include <vil/vil_image_view.h>
//#include <vil/vil_transform.h>
//brdb stuff
#include <brdb/brdb_value.h>
#include <boxm2/cpp/algo/boxm2_create_mog_image_functor.h>

//#include <vil/vil_math.h>
//#include <vil/vil_save.h>
//#include <vil/vil_convert.h>
#include <bsta/bsta_gauss_sf1.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_attributes.h>
#include <bbgm/bbgm_image_of.h>
#include <bbgm/bbgm_image_sptr.h>

//directory utility
#include <vcl_where_root_dir.h>


namespace boxm2_cpp_create_mog_image_process_globals
{
  const unsigned n_inputs_ = 6;
  const unsigned n_outputs_ = 1;
  vcl_size_t lthreads[2]={8,8};
}

bool boxm2_cpp_create_mog_image_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_create_mog_image_process_globals;

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "vpgl_camera_double_sptr";
  input_types_[3] = "unsigned"; // ni
  input_types_[4] = "unsigned"; // nj
  input_types_[5] = "vcl_string";// if identifier string is empty, then only one appearance model

  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "bbgm_image_sptr";
  
  bool good = pro.set_input_types(input_types_) &&
    pro.set_output_types(output_types_);
  // in case the 6th input is not set
  brdb_value_sptr idx = new brdb_value_t<vcl_string>("");
  pro.set_input(5, idx);
  return good;
}

bool boxm2_cpp_create_mog_image_process(bprb_func_process& pro)
{
  using namespace boxm2_cpp_create_mog_image_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(i++);
  vpgl_camera_double_sptr cam= pro.get_input<vpgl_camera_double_sptr>(i++);
  unsigned ni = pro.get_input<unsigned>(i++);
  unsigned nj = pro.get_input<unsigned>(i++);
  vcl_string identifier = pro.get_input<vcl_string>(i);

  bool foundDataType = false;
  bool foundNumObsType = false;

  vnl_vector_fixed<unsigned char, 8> mog3((unsigned char)0);
  vbl_array_2d<boxm2_data_traits<BOXM2_MOG3_GREY>::datatype>* mog3_img = new vbl_array_2d<boxm2_data_traits<BOXM2_MOG3_GREY>::datatype>(ni, nj, mog3);
  vnl_vector_fixed<unsigned char, 2> mog((unsigned char)0);
  vbl_array_2d<boxm2_data_traits<BOXM2_GAUSS_GREY>::datatype>* mog_img = new vbl_array_2d<boxm2_data_traits<BOXM2_GAUSS_GREY>::datatype>(ni, nj, mog);

  vcl_string data_type, num_obs_type;
  vcl_vector<vcl_string> apps = scene->appearances();
  for (unsigned int i=0; i<apps.size(); ++i) {
    if ( apps[i] == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
    {
      data_type = apps[i];
      foundDataType = true;
    }
    else if ( apps[i] == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() )
    {
      vcl_cout << "In boxm2_cpp_create_mog_image_process() - app type BOXM2_MOG3_GREY_16 is not supported!";
      return false;
    }
    else if ( apps[i] == boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix() )
    {
      data_type = apps[i];
      foundDataType = true;
    }
    else if ( apps[i] == boxm2_data_traits<BOXM2_NUM_OBS>::prefix() )
    {
      num_obs_type = apps[i];
      foundNumObsType = true;
    }
  }
  if (!foundDataType) {
    vcl_cout<<"BOXM2_CPP_RENDER_PROCESS ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<vcl_endl;
    return false;
  }


  if (identifier.size() > 0) {
    data_type += "_" + identifier;
    if (foundNumObsType)
      num_obs_type += "_" + identifier;
  }

  vcl_vector<boxm2_block_id> vis_order=scene->get_vis_blocks(reinterpret_cast<vpgl_generic_camera<double>*>(cam.ptr()));
  if (vis_order.empty())
  {
    vcl_cout<<" None of the blocks are visible from this viewpoint"<<vcl_endl;
    return true;
  }

  // function call
  vil_image_view<float> * vis_img=new vil_image_view<float>(ni,nj);
  vis_img->fill(1.0f);

  int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
  int nobsTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NUM_OBS>::prefix());

  vcl_vector<boxm2_block_id>::iterator id;
  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
    vcl_cout<<"Block Id "<<(*id)<<vcl_endl;
    boxm2_block *     blk  =  cache->get_block(*id);
    boxm2_data_base *  alph = cache->get_data_base(*id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
    boxm2_data_base *  nobs  = cache->get_data_base(*id,num_obs_type,alph->buffer_length()/alphaTypeSize*nobsTypeSize,false);
    boxm2_data_base *  mog  = cache->get_data_base(*id,data_type);
    vcl_vector<boxm2_data_base*> datas;
    datas.push_back(alph);
    datas.push_back(mog);
    datas.push_back(nobs);
    boxm2_scene_info_wrapper *scene_info_wrapper=new boxm2_scene_info_wrapper();
    scene_info_wrapper->info=scene->get_blk_metadata(*id);
    if ( data_type.find(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix()) != vcl_string::npos )
    {
      boxm2_create_mog_image_functor<BOXM2_MOG3_GREY> mog_functor;
      mog_functor.init_data(datas, mog3_img, vis_img);
      cast_ray_per_block<boxm2_create_mog_image_functor<BOXM2_MOG3_GREY> >
      (mog_functor,scene_info_wrapper->info,blk,cam,ni,nj);
    }
    else if (data_type.find(boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix()) != vcl_string::npos )
    {
      boxm2_create_mog_image_functor<BOXM2_GAUSS_GREY> mog_functor;
      mog_functor.init_data(datas, mog_img, vis_img);
      cast_ray_per_block<boxm2_create_mog_image_functor<BOXM2_GAUSS_GREY> >
        (mog_functor,scene_info_wrapper->info,blk,cam,ni,nj);
    }
  }
  
  typedef bsta_gauss_sf1 bsta_gauss1_t;
  typedef bsta_num_obs<bsta_gauss1_t> gauss_type1;
  typedef bsta_mixture<gauss_type1> mix_gauss_type1;
  typedef bsta_num_obs<mix_gauss_type1> obs_mix_gauss_type1;

  bbgm_image_sptr output_model;
  bbgm_image_of<obs_mix_gauss_type1>* mod = new bbgm_image_of<obs_mix_gauss_type1>(ni, nj, mix_gauss_type1());
  if ( boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() + "_" + identifier == data_type )
  {
    //bbgm_image_of<obs_mix_gauss_type1>* mod = new bbgm_image_of<obs_mix_gauss_type1>(ni, nj, mix_gauss_type1());
    for (unsigned i = 0; i < ni; i++)
      for (unsigned j = 0; j < nj; j++) {
        vnl_vector_fixed<unsigned char, 8>& mog3 = (*mog3_img)(i,j);
        float mu0=((float)mog3[0]/255.0f);float sigma0=((float)mog3[1]/255.0f);float w0=((float)mog3[2]/255.0f);
        float mu1=((float)mog3[3]/255.0f);float sigma1=((float)mog3[4]/255.0f);float w1=((float)mog3[5]/255.0f);
        float mu2=((float)mog3[6]/255.0f);float sigma2=((float)mog3[7]/255.0f);float w2=1-w0-w1;
        bsta_gauss1_t g0(mu0, sigma0);
        bsta_gauss1_t g1(mu1, sigma1);
        bsta_gauss1_t g2(mu2, sigma2);
        obs_mix_gauss_type1 mg;
        mg.insert(g0, w0);
        mg.insert(g1, w1);
        mg.insert(g2, w2);
        mod->set(i,j,mg);
      }
    output_model = mod;
  }
  else if ( boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix() + "_" + identifier == data_type )
  {
    //bbgm_image_of<gauss_type1>* mod = new bbgm_image_of<gauss_type1>(ni, nj, bsta_gauss1_t());
    for (unsigned i = 0; i < ni; i++)
      for (unsigned j = 0; j < nj; j++) {
        vnl_vector_fixed<unsigned char, 2>& mm = (*mog_img)(i,j);
        bsta_gauss1_t init_gauss(mm[0]/255.0f, mm[1]/255.0f);
        //mod->set(i,j,init_gauss);
        obs_mix_gauss_type1 mg;
        mg.insert(init_gauss, 1.0f);
        mod->set(i,j,mg);
      }
    output_model = mod;
  }

  brdb_value_sptr output = new brdb_value_t<bbgm_image_sptr>(output_model);
  pro.set_output(0, output);
  
  delete mog3_img;
  delete mog_img;
  return true;
}


