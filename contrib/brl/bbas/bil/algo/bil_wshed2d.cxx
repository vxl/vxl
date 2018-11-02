#include <cstdio>
#include <iostream>
#include <cmath>
#include "bil_wshed2d.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <climits>

#include <vil/vil_image_view.h>
#include <vil/vil_copy.h>
#include <vil/vil_math.h>
#include <vil/algo/vil_gauss_filter.h>
#include <vil/algo/vil_sobel_3x3.h>

#define WSHED_MASK -2
#define WSHED_INIT -1

//: Constructor
bil_wshed_2d::bil_wshed_2d()
= default;

//: Destructor
bil_wshed_2d::~bil_wshed_2d()
= default;

//public functions

std::vector< vil_image_view< unsigned char > >
bil_wshed_2d::bil_wshed_2d_main(vil_image_view< unsigned char > src_img, double gsigma1, double gsigma2,
                                int min_x, int min_y, int max_x, int max_y)
{
  //initialize
  width_ = max_x - min_x;
  height_ = max_y - min_y;
  input_img_.set_size(width_, height_);
  int i, j, ii, jj;
  for (j = min_y, jj = 0; j < max_y; j++, jj++)
    for (i = min_x, ii = 0; i < max_x; i++, ii++)
      input_img_(ii, jj) = src_img(i, j);

  image_size_ = width_ * height_;

  h_start_  = h_end_ = 0;
  current_label_ = 0;

  gradient_img_.set_size(width_, height_);
  distance_map_.set_size(width_, height_);
  output_img_.set_size(width_, height_);
  output_img_wout_wsheds_.set_size(width_, height_);

  for (int j = 0; j < height_; j++)
  {
    for (int i = 0; i < width_; i++)
    {
      distance_map_(i, j) = 0;
      output_img_(i, j) = WSHED_INIT;
    }
  }

  sorted_pixels_x_ = (int *) malloc(sizeof(int) * width_ * height_);
  sorted_pixels_y_ = (int *) malloc(sizeof(int) * width_ * height_);

  smooth_and_gradient_img(gsigma1, gsigma2);
  //sort the pixels w.r.t. intensity values of gradient_img_
  sort_pixels();
  compute_watershed_regions();
  remove_watershed_pixels();
  highlight_region_boundaries();
  calculate_region_properties();

  output_img_uchar_.set_size(src_img.ni(), src_img.nj());
  for (int j = 0; j < height_; j++)
  {
    for (int i = 0; i < width_; i++)
    {
      if (output_img_(i, j) != WSHED)
        output_img_uchar_(i + min_x, j + min_y) = (unsigned char)(output_img_(i, j) % 255 + 1);
      else
        output_img_uchar_(i + min_x, j + min_y) = WSHED;
    }
  }
  std::vector< vil_image_view< unsigned char > > output_vector;
  output_vector.push_back(output_img_uchar_);

  output_img_wout_wsheds_uchar_.set_size(src_img.ni(), src_img.nj());
  for (unsigned j = 0; j < src_img.nj(); j++)
    for (unsigned i = 0; i < src_img.ni(); i++)
      output_img_wout_wsheds_uchar_(i , j) = 0;

  for (int j = 0; j < height_; j++)
    for (int i = 0; i < width_; i++)
      output_img_wout_wsheds_uchar_(i + min_x, j + min_y) = (unsigned char)(output_img_wout_wsheds_(i, j) % 255 + 1);

  output_vector.push_back(output_img_wout_wsheds_uchar_);

  return output_vector;
}

//PROTECTED FUNCTIONS

//Put this pixel to the queue if it is neighbor of a previously flooded pixel
//It is necessary and enough to put a pixel to the queue in this condition,
//hence return after putting it to the queue
void
bil_wshed_2d::add_connected_pixels_to_queue(int pos_x, int pos_y)
{
  for (int j = -1; j < 2; j++)
  {
    int neigh_y = pos_y + j;
    if (neigh_y >=0 && neigh_y < height_)
    {
      for (int i = -1; i < 2; i++)
      {
        if (i || j)
        {
          int neigh_x = pos_x + i;
          if (neigh_x >= 0 && neigh_x < width_)
          {
            if (output_img_(neigh_x, neigh_y) > 0 || output_img_(neigh_x, neigh_y) == WSHED)
            {
              distance_map_(pos_x, pos_y) = 1;
              queue_x.push_back(pos_x);
              queue_y.push_back(pos_y);
              return;
            }
          }
        }
      }
    }
  }
}

void
bil_wshed_2d::calculate_region_properties()
{
  double temp_mean;
  max_mean_intensity_ = 0;
  min_mean_intensity_ = 0xFFFFFFFF;
#if 0
  wshed_regions_ = (struct bil_wshed_2d_region *) malloc(sizeof(bil_wshed_2d_region) * (current_label_+1));
#endif
  wshed_regions_.resize(current_label_+1);
  for (int i = 1; i < current_label_+1; i++)
  {
    wshed_regions_[i].number_of_pixels = 0;
    wshed_regions_[i].total_sum = 0;
    wshed_regions_[i].mean_intensity = 0;
  }

  for (int j = 1; j < height_-1; j++)
  {
    for (int i = 1; i < width_-1; i++)
    {
      int region_id = output_img_wout_wsheds_(i, j);
      wshed_regions_[region_id].number_of_pixels++;
      wshed_regions_[region_id].total_sum += input_img_(i, j);
    }
  }
  for (int i = 1; i < current_label_+1; i++)
  {
    temp_mean = wshed_regions_[i].mean_intensity = (double)wshed_regions_[i].total_sum / wshed_regions_[i].number_of_pixels;
    if (temp_mean > max_mean_intensity_)
      max_mean_intensity_ = temp_mean;
    if (temp_mean < min_mean_intensity_)
      min_mean_intensity_ = temp_mean;
  }
}

void
bil_wshed_2d::collect_garbage()
{
  free(sorted_pixels_x_);
  free(sorted_pixels_y_);
  gradient_img_.clear();
}

void
bil_wshed_2d::compute_watershed_regions()
{
  int h;
  int control = 0;
  for (h = 0;h <= max_value_src_; h++)
  {
    //finding the start and end locations in sorted pixels lists for a specific height
    h_start_ = h_end_;
    while (true)
    {
      if (h_end_ < image_size_ && gradient_img_(sorted_pixels_x_[h_end_], sorted_pixels_y_[h_end_]) == h )
      {
        h_end_++;
        control = 1;
      }
      else
        break;
    }
    //if there are any pixels of the specific height -> call the flooding function
    if (control == 1)
    {
      std::printf("height %d start %d end %d\n", h, h_start_, h_end_);
      //h_start_ and h_end_ point to the start and end of the pixels in the sorted arrays to be processed
      flood_current_height();
      control = 0;
    }
  }
}

void
bil_wshed_2d::flood_current_height()
{
  int turn;
  for (turn = h_start_; turn < h_end_; turn++)
  {
    int pos_x = sorted_pixels_x_[turn];
    int pos_y = sorted_pixels_y_[turn];

    output_img_(pos_x, pos_y) = WSHED_MASK;
    add_connected_pixels_to_queue(pos_x, pos_y);
  }
  process_connected_pixels_in_queue();
  process_new_discovered_minima();
}

//There is a new basin (disconnected from other explored basins),
//and a point in this basin at the processed height is being filled
//together with checking its neighbors of the same height
void
bil_wshed_2d::flood_new_basin_from_given_point(int pos_x, int pos_y)
{
  for (int j = -1; j < 2; j++)
  {
    int neigh_y = pos_y + j;
    if (neigh_y >=0 && neigh_y < height_)
    {
      for (int i = -1; i < 2; i++)
      {
        if (i || j)
        {
          int neigh_x = pos_x + i;
          if (neigh_x >= 0 && neigh_x < width_)
          {
            //THIS IS THE MAIN PART
            if (output_img_(neigh_x, neigh_y) == WSHED_MASK)
            {
              queue_x.push_back(neigh_x);
              queue_y.push_back(neigh_y);
              output_img_(neigh_x, neigh_y) = current_label_;
            }
            //THIS IS THE MAIN PART
          }
        }
      }
    }
  }
}

int
bil_wshed_2d::get_the_smallest_neighbor_region_label(int pos_x, int pos_y)
{
  int min_region_label = INT_MAX;
  for (int j = -1; j < 2; j++)
  {
    int neigh_y = pos_y + j;
    if (neigh_y >=0 && neigh_y < height_)
    {
      for (int i = -1; i < 2; i++)
      {
        if (i || j)
        {
          int neigh_x = pos_x + i;
          if (neigh_x >= 0 && neigh_x < width_)
          {
            //THIS IS THE MAIN PART
            if (output_img_(neigh_x, neigh_y) != WSHED && output_img_(neigh_x, neigh_y) < min_region_label)
              min_region_label = output_img_(neigh_x, neigh_y);
            //THIS IS THE MAIN PART
          }
        }
      }
    }
  }
  return min_region_label;
}

void
bil_wshed_2d::highlight_region_boundaries()
{
  for (int j = 0; j < height_; j++)
  {
    for (int i = 0; i < width_; i++)
    {
      if (output_img_(i, j) != WSHED && (is_there_a_neighbor_of_smaller_label(i,j)))
        output_img_(i, j) = WSHED;
    }
  }
}

int
bil_wshed_2d::is_there_a_neighbor_of_smaller_label(int pos_x, int pos_y)
{
  int region_id = output_img_(pos_x, pos_y);
  for (int j = -1; j < 2; j++)
  {
    int neigh_y = pos_y + j;
    if (neigh_y >=0 && neigh_y < height_)
    {
      for (int i = -1; i < 2; i++)
      {
        if (i || j)
        {
          int neigh_x = pos_x + i;
          if (neigh_x >= 0 && neigh_x < width_)
          {
            //THIS IS THE MAIN PART
            if (output_img_(neigh_x, neigh_y) != WSHED && output_img_(neigh_x, neigh_y) < region_id)
              return 1;
            //THIS IS THE MAIN PART
          }
        }
      }
    }
  }
  return 0;
}

//The pixels, which are connected to previously explored basins, of the processed height
//have been put in the queue. This is the function to process them.
void
bil_wshed_2d::process_connected_pixels_in_queue()
{
  current_distance_ = 1;
  //fictitious pixel to report the end of the current height or a layer of the current height
  queue_x.push_back(-1);
  queue_y.push_back(-1);

  while (true)
  {
    int pos_x = queue_x.front();
    int pos_y = queue_y.front();
    queue_x.pop_front();
    queue_y.pop_front();

    if (pos_x == -1)
    {
      if (queue_x.empty())
        break;
      else
      {
        queue_x.push_back(-1);
        queue_y.push_back(-1);
        current_distance_++;
        pos_x = queue_x.front();
        pos_y = queue_y.front();
        queue_x.pop_front();
        queue_y.pop_front();
      }
    }
    process_given_connected_pixel(pos_x, pos_y);
  }
}

//The processing function of a pixel taken from the queue
//This pixel was found to be connected to a previously explored basin
void
bil_wshed_2d::process_given_connected_pixel(int pos_x, int pos_y)
{
  for (int j = -1; j < 2; j++)
  {
    int neigh_y = pos_y + j;
    if (neigh_y >=0 && neigh_y < height_)
    {
      for (int i = -1; i < 2; i++)
      {
        if (i || j)
        {
          int neigh_x = pos_x + i;
          if (neigh_x >= 0 && neigh_x < width_)
          {
            //THIS IS THE MAIN PART
            if (distance_map_(neigh_x, neigh_y) < current_distance_ &&
                (output_img_(neigh_x, neigh_y) > 0 || output_img_(neigh_x, neigh_y) == WSHED))
            {
              if (output_img_(neigh_x, neigh_y) > 0)
              {
                if (output_img_(pos_x, pos_y) == WSHED_MASK || output_img_(pos_x, pos_y) == WSHED)
                    output_img_(pos_x, pos_y) = output_img_(neigh_x, neigh_y);
                else if (output_img_(neigh_x, neigh_y) != output_img_(pos_x, pos_y))
                  output_img_(pos_x, pos_y) = WSHED;
              }
              else if (output_img_(pos_x, pos_y) == WSHED_MASK)
                output_img_(pos_x, pos_y) = WSHED;
            }
            else if (output_img_(neigh_x, neigh_y) == WSHED_MASK && distance_map_(neigh_x, neigh_y) == 0)
            {
              distance_map_(neigh_x, neigh_y) = current_distance_ + 1;
              queue_x.push_back(neigh_x);
              queue_y.push_back(neigh_y);
            }
            //THIS IS THE MAIN PART
          }
        }
      }
    }
  }
}

//There is/are new minimum/minima (meaning new basin(s)), which is/are processed by this function
void
bil_wshed_2d::process_new_discovered_minima()
{
  int turn;
  for (turn = h_start_; turn < h_end_;turn++)
  {
    int pos_x = sorted_pixels_x_[turn];
    int pos_y = sorted_pixels_y_[turn];
    distance_map_(pos_x, pos_y) = 0;
    if (output_img_(pos_x, pos_y) == WSHED_MASK)
    {
      current_label_++;
      queue_x.push_back(pos_x);
      queue_y.push_back(pos_y);
      output_img_(pos_x, pos_y) = current_label_;
      while (! queue_x.empty())
      {
        int pos_x_2 = queue_x.front();
        int pos_y_2 = queue_y.front();
        queue_x.pop_front();
        queue_y.pop_front();
        flood_new_basin_from_given_point(pos_x_2, pos_y_2);
      }
    }
  }
}

void
bil_wshed_2d::remove_watershed_pixels()
{
  for (int j = 0; j < height_; j++)
  {
    for (int i = 0; i < width_; i++)
    {
      if (output_img_(i, j) == WSHED)
        output_img_wout_wsheds_(i, j) = get_the_smallest_neighbor_region_label(i,j);
      else
        output_img_wout_wsheds_(i, j) = output_img_(i,j);
    }
  }
}

void
bil_wshed_2d::smooth_and_gradient_img(double gsigma1, double gsigma2)
{
  vil_math_value_range(input_img_, min_value_src_, max_value_src_);

  vil_image_view< unsigned char > smoothed_img(width_, height_);
  vil_image_view< float > gradient_img_float(width_, height_);

  //smoothing on original image
  if (gsigma1 > 0.0)
  {
    vil_gauss_filter_5tap_params params(gsigma1);
    vil_gauss_filter_5tap(input_img_, smoothed_img, params);
  }
  else
  {
    std::cout << std::endl << "No Gaussian Smoothing Applied..." << std::endl;
    vil_copy_deep(input_img_, smoothed_img);
  }
  //gradient
  vil_sobel_3x3(smoothed_img, gradient_img_float);
  if (gsigma2 > 0.0)
  {
    //smoothing on gradient. input and output images are same since vil_gauss function
    //makes an internal copy of the input, and assigns the result on the output at the
    //end of the algorithm
    vil_gauss_filter_5tap_params params(gsigma2);
    vil_gauss_filter_5tap(gradient_img_float, gradient_img_float, params);
  }

  for (int j = 0; j < height_; j++)
    for (int i = 0; i < width_; i++)
      gradient_img_float(i, j) = std::fabs(gradient_img_float(i, j));

  float min_value_grad, max_value_grad;
  vil_math_value_range(gradient_img_float, min_value_grad, max_value_grad);

  for (int j = 0; j < height_; j++)
    for (int i = 0; i < width_; i++)
      gradient_img_(i, j) = (unsigned char)(gradient_img_float(i, j) * max_value_src_ / max_value_grad);

  for (int j = 0; j < height_; j++)
  {
    gradient_img_(0, j) = max_value_src_;
    gradient_img_(width_-1, j) = max_value_src_;
  }
  for (int i = 0; i < width_; i++)
  {
    gradient_img_(i, 0) = max_value_src_;
    gradient_img_(i, height_-1) = max_value_src_;
  }

  smoothed_img.clear();
  gradient_img_float.clear();
}

//bucket sorting used
void
bil_wshed_2d::sort_pixels()
{
  int i,j;
  int *intensity_frequencies = (int *) calloc(max_value_src_ + 1, sizeof(int));

  //counting pixel value frequencies
  for (j = 0; j < height_; j++)
    for (i = 0; i < width_; i++)
      intensity_frequencies[ gradient_img_(i, j) ]++;

  //calculating cumulative frequencies
  for (i = 1; i < max_value_src_ + 1; i++)
    intensity_frequencies[i] += intensity_frequencies[i-1];

  //setting location pointers
  for (j = height_-1; j >= 0; j--)
  {
    for (i = width_-1; i >= 0; i--)
    {
      unsigned char pixel_value = gradient_img_(i, j);
      sorted_pixels_x_[ intensity_frequencies[pixel_value]-1 ] = i;
      sorted_pixels_y_[ intensity_frequencies[pixel_value]-1 ] = j;
      intensity_frequencies[ pixel_value ]--;
    }
  }

  free(intensity_frequencies);
}
