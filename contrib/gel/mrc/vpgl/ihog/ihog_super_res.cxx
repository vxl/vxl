// This is vpgl/ihog/ihog_super_res.cxx
//:
// \file

#include <ihog/ihog_super_res.h>
#include <ihog/ihog_region.h>

#include <vgl/vgl_polygon.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_area.h>
#include <vgl/vgl_box_2d.h>

#include <vcl_list.h>

#include <vul/vul_timer.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <vil/vil_copy.h>
#include <vil/vil_fill.h>
#include <vil/vil_math.h>
#include <vcl_sstream.h>
#include <vcl_iomanip.h>
#include <vimt/vimt_sample_grid_bilin.h>
#include <vnl/algo/vnl_qr.h>


//: Constructor
ihog_super_res::ihog_super_res(vcl_vector<vimt_image_2d_of<vxl_byte> >& images,
                               int block_size)
 : images_(images), bsi_(block_size), bsj_(block_size), blur_(0.0)
{
}


//: Compute the higher resolution image
vil_image_view<double> 
ihog_super_res::compute_high_res(ihog_region_sptr region, vcl_string path, double blur)
{
  region_ = region;
  blur_ = blur;
  vcl_cout << "Region: " << region_->ni()<<' '<<region_->nj()<<vcl_endl;
  vil_image_view<double> super_image(region_->ni(),region_->nj(),images_[0].image().nplanes());
  vil_fill(super_image,0.0);

  for (int h=0; h<region_->ni(); h+=bsi_){
    for (int v=0; v<region_->nj(); v+=bsj_){
      vcl_cout << "location: "<<h <<","<< v << vcl_endl;
      vgl_box_2d<int> bounds(h,h+bsi_,v,v+bsj_);
      bounds.set_max_x(vcl_min(bounds.max_x(), region_->ni()));
      bounds.set_max_y(vcl_min(bounds.max_y(), region_->nj()));
      vnl_matrix<double> data;
      vnl_matrix<double> weights;
      vul_timer time;
      this->compute_weights(data, weights, bounds);
      vcl_cout << "Weight time: " << time.real() << vcl_endl;
      time.mark();
      vnl_matrix<double> super_data = vnl_qr<double>(weights).solve(data);
      vcl_cout << "QR time: " << time.real() <<'\n'<< vcl_endl;

      //vcl_cout << "----Data----\n"<<data << vcl_endl;
      //vcl_cout << "----Weights----\n"<<weights << vcl_endl;
      //vcl_cout << "----Inverse----\n"<<inv_weights << vcl_endl;
      //vcl_cout << "----Super Data----\n"<< super_data << '\n' << vcl_endl;

      vil_image_view<double> super_block( super_data.data_block(), 
                                          bounds.width(), bounds.height(), data.cols(), 
                                          bounds.height()*data.cols(), data.cols(), 1);
      vil_copy_to_window(super_block, super_image, h, v);

      vil_image_view<vxl_byte> output;
      vil_math_truncate_range(super_image, 0.0, 1.0);
      vil_convert_stretch_range(super_image, output);
      vil_save(output, path.c_str());  
    }
  }

  return super_image;
}


//: Compute the largest region to estimate
ihog_region_sptr 
ihog_super_res::compute_region(double area_mag)
{
  double total_area = 0.0;
  double aspect_ratio = 0.0;
  typedef vcl_vector<vimt_image_2d_of<vxl_byte> >::const_iterator image_itr;
  for(image_itr itr = images_.begin(); itr!=images_.end(); ++itr){
    vimt_transform_2d xform = itr->world2im();

    vgl_point_2d<double> points[4];
    points[0] = xform(vgl_point_2d<double>(0,0));
    points[1] = xform(vgl_point_2d<double>(1,0));
    points[2] = xform(vgl_point_2d<double>(1,1));
    points[3] = xform(vgl_point_2d<double>(0,1));
    vgl_polygon<double> quad(points, 4);
    total_area += vgl_area(quad);
    double width = ( vgl_distance(points[0],points[1])
                   + vgl_distance(points[2],points[3]) )/2.0;
    double height =( vgl_distance(points[1],points[2]) 
                   + vgl_distance(points[0],points[3]) )/2.0;
    aspect_ratio += width/height;
  }
  double avg_asp = aspect_ratio/images_.size();
  vcl_cout << "Total Area = " << total_area << vcl_endl;
  vcl_cout << "Average Aspect = " << avg_asp << vcl_endl;

  double avg_area = total_area/images_.size();
  double width = vcl_sqrt(area_mag * avg_area * avg_asp);
  double height = vcl_sqrt(area_mag * avg_area / avg_asp);
  return new ihog_region(int(width), int(height));
}


//: Compute the weights and data vector for all images
void
ihog_super_res::compute_weights( vnl_matrix<double>& data, 
                                 vnl_matrix<double>& weights,
                                 const vgl_box_2d<int>& bounds) const
{
  vcl_list<vnl_vector<double> > weight_list;
  vcl_list<vnl_vector<double> > data_list;
  for(unsigned int i=0; i<images_.size(); ++i){
    vcl_list<vnl_vector<double> > curr_data;
    vcl_list<vnl_vector<double> > curr_weights;
    this->compute_weights(i, curr_data, curr_weights, bounds);
    weight_list.splice(weight_list.end(), curr_weights);
    data_list.splice(data_list.end(), curr_data);
  }
  
  data.set_size(data_list.size(), data_list.front().size());
  weights.set_size(weight_list.size(), bounds.area());
  int r=0;
  typedef vcl_list<vnl_vector<double> >::iterator vec_itr;
  for( vec_itr d_itr = data_list.begin(), w_itr = weight_list.begin();
       d_itr != data_list.end(); ++d_itr, ++w_itr, ++r )
  {
    weights.set_row(r, *w_itr);
    data.set_row(r, *d_itr);
  }
  
}

//: Compute the weights and data vector for the given index
void
ihog_super_res::compute_weights( int index, 
                                 vcl_list<vnl_vector<double> >& data, 
                                 vcl_list<vnl_vector<double> >& weights,
                                 const vgl_box_2d<int>& bounds) const
{
  vimt_transform_2d xform = images_[index].world2im() * region_->xform();
  vil_image_view<vxl_byte> image(images_[index].image());

  vgl_point_2d<double> points[4];
  points[0] = xform(vgl_point_2d<double>(bounds.min_x(),  bounds.min_y()  ));
  points[1] = xform(vgl_point_2d<double>(bounds.max_x()-1,bounds.min_y()  ));
  points[2] = xform(vgl_point_2d<double>(bounds.max_x()-1,bounds.max_y()-1));
  points[3] = xform(vgl_point_2d<double>(bounds.min_x(),  bounds.max_y()-1));

  vgl_box_2d<double> xform_box;
  xform_box.add(points[0]);
  xform_box.add(points[1]);
  xform_box.add(points[2]);
  xform_box.add(points[3]);

  int i_min = vcl_max(int(xform_box.min_x()+0.5) - 1, 0);
  int i_max = vcl_min(int(xform_box.max_x()+0.5) + 1, int(image.ni())-1);
  int j_min = vcl_max(int(xform_box.min_y()+0.5) - 1, 0);
  int j_max = vcl_min(int(xform_box.max_y()+0.5) + 1, int(image.nj())-1);

  //data.set_size((i_max-i_min+1)*(j_max-j_min+1), image.nplanes());
  //weights.set_size(data.rows(), bounds.area());

  int n_planes = image.nplanes();

  for (int i=i_min; i<=i_max; ++i){
    for (int j=j_min; j<=j_max; ++j){
      vnl_vector<double> pixel(n_planes);
      for (int p=0; p<n_planes; ++p){
        pixel[p] = double(image(i,j,p))/255.0;
      }
      data.push_back(pixel);

      vgl_point_2d<double> center(i,j);
      vnl_vector<double> weight(bounds.area(), 0.0);
      int p = 0;
      for (int x=bounds.min_x(); x<bounds.max_x(); ++x){
        for (int y=bounds.min_y(); y<bounds.max_y(); ++y, ++p){
          vgl_point_2d<double> pt = xform(vgl_point_2d<double>(x,y));
          weight[p] = point_spread(center, pt);
        }
      }
      weights.push_back(weight);
    }
  }

  typedef vcl_list<vnl_vector<double> >::iterator vec_itr;
  for( vec_itr d_itr = data.begin(), w_itr = weights.begin();
       d_itr != data.end(); ++d_itr, ++w_itr )
  {
    double sum = w_itr->sum();
    if( sum > 0.0 )
      *w_itr /= sum;
    else{
      data.erase(d_itr--);
      weights.erase(w_itr--);
    }
  }

  //vcl_cout << "Frame "<<index<<'\n'
  //         << " number of pixels = " << data.size() << vcl_endl;
  //vcl_cout << "data size = "<< data.size() << vcl_endl;
  //vcl_cout << "weight size = " << weights.rows()<<','<<weights.cols()<<vcl_endl;
  
}


//; Sample from the estimated point spread function 
double 
ihog_super_res::point_spread( const vgl_point_2d<double>& m, 
                              const vgl_point_2d<double>& p ) const
{
  
  double b_max = 0.5 + blur_;
  double b_min = 0.5 - blur_;

  double dx = vcl_abs(m.x() - p.x());
  double dy = vcl_abs(m.y() - p.y());

  if(dx > b_max || dy > b_max)
    return 0.0;
  
  double C = 1.0/(2.0* blur_*blur_);

  double x_val, y_val;
  if(dx > 0.5)
    x_val = C*(dx - b_max)*(dx - b_max);
  else if (dx > b_min)
    x_val = 1.0 - C*(dx - b_min)*(dx - b_min);
  else
    x_val = 1.0;

  if(dy > 0.5)
    y_val = C*(dy - b_max)*(dy - b_max);
  else if (dy > b_min)
    y_val = 1.0 - C*(dy - b_min)*(dy - b_min);
  else
    y_val = 1.0;

  return x_val*y_val;
}
