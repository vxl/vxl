#ifndef boxm2_ray_probe_functor_h
#define boxm2_ray_probe_functor_h
//:
// \file

#include <iostream>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/cpp/algo/boxm2_mog3_grey_processor.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class boxm2_ray_probe_functor
{
 public:
  //: "default" constructor
  boxm2_ray_probe_functor() = default;

  bool init_data(std::vector<boxm2_data_base*> & datas,
                 std::vector<float> & seg_len,
                 std::vector<float> & abs_depth,
                 std::vector<float> & alpha,
                 std::vector<float> & data_to_return,
                 std::string prefix,
                 int & nelems)
  {
    alpha_data_        = new boxm2_data<BOXM2_ALPHA>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
    data_ptr           = datas[1];
    seg_len_           = &seg_len;
    alpha_             = &alpha;
    abs_depth_         = &abs_depth;
    data_to_return_    = &data_to_return;
    prefix_            = prefix;
    switch (boxm2_data_info::data_type(prefix_))
    {
      case BOXM2_MOG3_GREY:
        nelems= 8;
        break;
      case BOXM2_LABEL_SHORT:
          nelems= 1;
          break;
      case BOXM2_FLOAT8:
        nelems = 8;
        break;
      case BOXM2_FLOAT:
        nelems = 1;
        break;
      case BOXM2_AUX0:
        nelems = 1;
        break;
      case BOXM2_AUX1:
        nelems = 1;
        break;
      case BOXM2_AUX2:
        nelems = 1;
        break;
      case BOXM2_AUX3:
        nelems = 1;
        break;

      case BOXM2_EXPECTATION:
        nelems = 1;
        break;
      case BOXM2_NUM_OBS:
        nelems = 4;
        break;
      case BOXM2_POINT:
        nelems = 4;
        break;
      case BOXM2_NORMAL:
        nelems = 4;
        break;
      case BOXM2_VIS_SCORE:
        nelems = 1;
        break;
      case BOXM2_COVARIANCE:
        nelems = 9;
        break;
      default:
        nelems = 0;
    }
    return true;
  }

  inline bool step_cell(float seg_len,int index,unsigned i, unsigned j, float abs_depth)
  {
    boxm2_data<BOXM2_ALPHA>::datatype alpha=alpha_data_->data()[index];

    switch (boxm2_data_info::data_type(prefix_))
    {
      case BOXM2_MOG3_GREY:
      {
        boxm2_data<BOXM2_MOG3_GREY>*  mog3_grey_app_data_= new boxm2_data<BOXM2_MOG3_GREY>(data_ptr->data_buffer(),data_ptr->buffer_length(),data_ptr->block_id());
        boxm2_data<BOXM2_MOG3_GREY>::datatype mog3_grey_app=mog3_grey_app_data_->data()[index];
        data_to_return_->push_back((float)mog3_grey_app[0] / 255.0f);
        data_to_return_->push_back((float)mog3_grey_app[1] / 255.0f);
        data_to_return_->push_back((float)mog3_grey_app[2] / 255.0f);
        data_to_return_->push_back((float)mog3_grey_app[3] / 255.0f);
        data_to_return_->push_back((float)mog3_grey_app[4] / 255.0f);
        data_to_return_->push_back((float)mog3_grey_app[5] / 255.0f);
        data_to_return_->push_back((float)mog3_grey_app[6] / 255.0f);
        data_to_return_->push_back((float)mog3_grey_app[7] / 255.0f);
        break;
      }
      case BOXM2_FLOAT8:
      {
        boxm2_data<BOXM2_FLOAT8>*  mog3_grey_app_data_= new boxm2_data<BOXM2_FLOAT8>(data_ptr->data_buffer(),data_ptr->buffer_length(),data_ptr->block_id());
        boxm2_data<BOXM2_FLOAT8>::datatype mog3_grey_app=mog3_grey_app_data_->data()[index];
        data_to_return_->push_back(mog3_grey_app[0]);
        data_to_return_->push_back(mog3_grey_app[1]);
        data_to_return_->push_back(mog3_grey_app[2]);
        data_to_return_->push_back(mog3_grey_app[3]);
        data_to_return_->push_back(mog3_grey_app[4]);
        data_to_return_->push_back(mog3_grey_app[5]);
        data_to_return_->push_back(mog3_grey_app[6]);
        data_to_return_->push_back(mog3_grey_app[7]);
        break;
      }
      case BOXM2_FLOAT:
      {
        boxm2_data<BOXM2_FLOAT>*  app_data= new boxm2_data<BOXM2_FLOAT>(data_ptr->data_buffer(),data_ptr->buffer_length(),data_ptr->block_id());
        boxm2_data<BOXM2_FLOAT>::datatype app=app_data->data()[index];
        data_to_return_->push_back(app);
        break;
      }
      case BOXM2_AUX0:
      {
        boxm2_data<BOXM2_AUX0>*  app_data  = new boxm2_data<BOXM2_AUX0>(data_ptr->data_buffer(),data_ptr->buffer_length(),data_ptr->block_id());
        boxm2_data<BOXM2_AUX0>::datatype app=app_data->data()[index];
        data_to_return_->push_back(app);
        break;
      }
      case BOXM2_AUX1:
      {
        boxm2_data<BOXM2_AUX1>*  app_data  = new boxm2_data<BOXM2_AUX1>(data_ptr->data_buffer(),data_ptr->buffer_length(),data_ptr->block_id());
        boxm2_data<BOXM2_AUX1>::datatype app=app_data->data()[index];
        data_to_return_->push_back(app);
        break;
      }
      case BOXM2_AUX2:
      {
        boxm2_data<BOXM2_AUX2>*  app_data  = new boxm2_data<BOXM2_AUX2>(data_ptr->data_buffer(),data_ptr->buffer_length(),data_ptr->block_id());
        boxm2_data<BOXM2_AUX2>::datatype app=app_data->data()[index];
        data_to_return_->push_back(app);
        break;
      }
      case BOXM2_AUX3:
      {
        boxm2_data<BOXM2_AUX3>*  app_data  = new boxm2_data<BOXM2_AUX3>(data_ptr->data_buffer(),data_ptr->buffer_length(),data_ptr->block_id());
        boxm2_data<BOXM2_AUX3>::datatype app=app_data->data()[index];
        data_to_return_->push_back(app);
        break;
      }

      case BOXM2_EXPECTATION:
      {
        boxm2_data<BOXM2_EXPECTATION>*  app_data  = new boxm2_data<BOXM2_EXPECTATION>(data_ptr->data_buffer(),data_ptr->buffer_length(),data_ptr->block_id());
        boxm2_data<BOXM2_EXPECTATION>::datatype app=app_data->data()[index];
        data_to_return_->push_back(app);
        break;
      }
      case BOXM2_NUM_OBS:
      {
        boxm2_data<BOXM2_NUM_OBS>* num_obs_data  = new boxm2_data<BOXM2_NUM_OBS>(data_ptr->data_buffer(),data_ptr->buffer_length(),data_ptr->block_id());
        boxm2_data<BOXM2_NUM_OBS>::datatype app=num_obs_data->data()[index];
        data_to_return_->push_back(app[0]);
        data_to_return_->push_back(app[1]);
        data_to_return_->push_back(app[2]);
        data_to_return_->push_back(app[3]);
        break;
      }
      case BOXM2_POINT:
      {
        boxm2_data<BOXM2_POINT>* point_data  = new boxm2_data<BOXM2_POINT>(data_ptr->data_buffer(),data_ptr->buffer_length(),data_ptr->block_id());
        boxm2_data<BOXM2_POINT>::datatype app=point_data->data()[index];
        data_to_return_->push_back(app[0]);
        data_to_return_->push_back(app[1]);
        data_to_return_->push_back(app[2]);
        data_to_return_->push_back(app[3]);
        break;
      }
      case BOXM2_NORMAL:
      {
        boxm2_data<BOXM2_NORMAL>* point_data  = new boxm2_data<BOXM2_NORMAL>(data_ptr->data_buffer(),data_ptr->buffer_length(),data_ptr->block_id());
        boxm2_data<BOXM2_NORMAL>::datatype app=point_data->data()[index];
        data_to_return_->push_back(app[0]);
        data_to_return_->push_back(app[1]);
        data_to_return_->push_back(app[2]);
        data_to_return_->push_back(app[3]);
        break;
      }
      case BOXM2_VIS_SCORE:
      {
        boxm2_data<BOXM2_VIS_SCORE>* point_data  = new boxm2_data<BOXM2_VIS_SCORE>(data_ptr->data_buffer(),data_ptr->buffer_length(),data_ptr->block_id());
        boxm2_data<BOXM2_VIS_SCORE>::datatype app=point_data->data()[index];
        data_to_return_->push_back(app);
        break;
      }
      case BOXM2_COVARIANCE:
      {
        boxm2_data<BOXM2_COVARIANCE>* point_data  = new boxm2_data<BOXM2_COVARIANCE>(data_ptr->data_buffer(),data_ptr->buffer_length(),data_ptr->block_id());
        boxm2_data<BOXM2_COVARIANCE>::datatype app=point_data->data()[index];
        for (unsigned k = 0 ; k < 9; k++)
          data_to_return_->push_back(app[k]);
        break;
      }
      case BOXM2_LABEL_SHORT:
      {
        boxm2_data<BOXM2_LABEL_SHORT>* point_data  = new boxm2_data<BOXM2_LABEL_SHORT>(data_ptr->data_buffer(),data_ptr->buffer_length(),data_ptr->block_id());

        boxm2_data<BOXM2_LABEL_SHORT>::datatype app=point_data->data()[index];

        data_to_return_->push_back((float)app);
        break;
      }
      default: // do nothing
          {
              std::cout<<"Unknown data "<<std::endl;

        break;
          }
    }
    seg_len_->push_back(seg_len);
    alpha_->push_back(alpha);
    abs_depth_->push_back(abs_depth);
    return true;
  }

 private:
  boxm2_data<BOXM2_ALPHA> * alpha_data_;
  boxm2_data_base * data_ptr;
  std::vector<float> * abs_depth_;
  std::vector<float> * seg_len_;
  std::vector<float> * alpha_;
  std::vector<float> * data_to_return_;
  std::string prefix_;
};

class boxm2_ray_app_density_functor
{
 public:
  //: "default" constructor
  boxm2_ray_app_density_functor() = default;

  bool init_data(std::vector<boxm2_data_base*> & datas,
                 std::vector<float> & app_density,
                 float intensity)
  {
    //alpha_data_=new boxm2_data<BOXM2_ALPHA>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
    mog3_data_ =new boxm2_data<BOXM2_MOG3_GREY>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
    app_density_=&app_density;
    intensity_  =intensity;
    return true;
  }

  inline bool step_cell(float seg_len,int index,unsigned i, unsigned j,float t_abs)
  {
    app_density_->push_back(boxm2_processor_type<BOXM2_MOG3_GREY>::type::prob_density(mog3_data_->data()[index], intensity_));
    return true;
  }
 private:
  boxm2_data<BOXM2_MOG3_GREY> * mog3_data_;
  std::vector<float> * app_density_;
  float intensity_;
};

#endif
