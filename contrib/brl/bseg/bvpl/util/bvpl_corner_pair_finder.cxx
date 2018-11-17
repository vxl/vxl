#include "bvpl_corner_pair_finder.h"
#include <bvpl/bvpl_subgrid_iterator.h>
#include <vnl/vnl_math.h>
#include <vil/algo/vil_colour_space.h>

void bvpl_convert_pair_grid_to_hsv_grid(bvxm_voxel_grid<bvpl_pair> *pair_grid,
                                        bvxm_voxel_grid<vnl_float_4> *out_grid,
                                        std::vector<float> colors)
{
  //reset iterator
  bvxm_voxel_grid<vnl_float_4>::iterator out_grid_it = out_grid->begin();
  bvxm_voxel_grid<bvpl_pair>::iterator pair_grid_it = pair_grid->begin();

  //convert to hsv grid
  float r,g,b;
  float col;
  for (; pair_grid_it != pair_grid->end(); ++pair_grid_it, ++out_grid_it)
  {
    bvxm_voxel_slab<bvpl_pair>::iterator pair_slab_it = (*pair_grid_it).begin();
    bvxm_voxel_slab<vnl_float_4>::iterator out_slab_it = (*out_grid_it).begin();
    for (; pair_slab_it!=(*pair_grid_it).end(); ++pair_slab_it , ++ out_slab_it)
    {
      if ((*pair_slab_it).id()<0){//negative ids are unitialized voxels
        (*out_slab_it)=vnl_float_4(0,0,0,0);
      }
      else {
        col=colors[(*pair_slab_it).id()]*360.0f;
        vil_colour_space_HSV_to_RGB<float>(col,1.0f,255.0f,&r,&g,&b);
        (*out_slab_it)=vnl_float_4(r,g,b,255.0f);
      }
    }
  }
}

void bvpl_convert_pair_grid_to_float_grid(bvxm_voxel_grid<bvpl_pair> *pair_grid,
                                          bvxm_voxel_grid<float> *out_grid)
{
  //reset iterator
  bvxm_voxel_grid<float>::iterator out_grid_it = out_grid->begin();
  bvxm_voxel_grid<bvpl_pair>::iterator pair_grid_it = pair_grid->begin();

  //convert to hsv grid
  for (; pair_grid_it != pair_grid->end(); ++pair_grid_it, ++out_grid_it)
  {
    bvxm_voxel_slab<bvpl_pair>::iterator pair_slab_it = (*pair_grid_it).begin();
    bvxm_voxel_slab<float>::iterator out_slab_it = (*out_grid_it).begin();
    for (; pair_slab_it!=(*pair_grid_it).end(); ++pair_slab_it , ++ out_slab_it)
    {
      if ((*pair_slab_it).id()<0){//negative ids are unitialized voxels
        (*out_slab_it)=0.0f;
      }
      else {
        (*out_slab_it)=(*pair_slab_it).response();
      }
    }
  }
}

bvpl_corner_pairs_sptr bvpl_corner_pair_finder::find_pairs(bvxm_voxel_grid<int>* id_grid,
                                                           const bvpl_kernel_vector_sptr& search_kernels,
                                                           const bvpl_kernel_vector_sptr& corner_kernels)
{
  //sanity check
  if (search_kernels->size() != corner_kernels->size()){
    std::cerr << "Error: kernels have different size()\n";
    return nullptr;
  }

  std::vector<std::vector<vgl_line_segment_3d<int> > > all_lines;

  //1. run the same direction(id) kernel as the one we are interested in
  for (int target_id=0; target_id<search_kernels->size(); ++target_id)
  {
    bvpl_kernel_sptr this_search_kernel = search_kernels->kernels_[target_id];

    bvpl_kernel_sptr target_corner_kernel = corner_kernels->kernels_[target_id];

    std::cout << "Search kernel " << target_id << " with size" << this_search_kernel->max_point() << ',' << this_search_kernel->min_point()<<std::endl;

    /*******************************************************************/
    bvpl_subgrid_iterator<int> id_subgrid_iter(id_grid, this_search_kernel->min_point(), this_search_kernel->max_point());
    std::vector<vgl_line_segment_3d<int> > lines;

    while (!id_subgrid_iter.isDone())
    {
      bvpl_kernel_iterator kernel_iter = this_search_kernel->iterator();
      bvpl_voxel_subgrid<int> id_subgrid = *id_subgrid_iter;
      vgl_point_3d<int> p1 = id_subgrid_iter.global_cur_voxel();
      int id_at_center = id_subgrid.get_voxel();
      if (id_at_center == target_id) {
#ifdef DEBUG
        std::cout << p1 << " id " << id_at_center << std::endl;
#endif // DEBUG
        //reset the iterator
        kernel_iter.begin();
        while (!kernel_iter.isDone()) {
          vgl_point_3d<int> idx = kernel_iter.index();
          int this_id=-1;
          if (id_subgrid.voxel(idx, this_id)) {
            if (this_id >=0) {
              bvpl_kernel_sptr this_corner_kernel = corner_kernels->kernels_[this_id];
              if (this_corner_kernel->axis()== target_corner_kernel->axis()){//same plane now look for opossite diagonal corners
                if (std::abs(std::abs(this_corner_kernel->angle() - target_corner_kernel->angle()) - vnl_math::pi_over_2) < 1e-7) {
                  vgl_point_3d<int> p2(p1.x()+idx.x(), p1.y()+idx.y(), p1.z()-idx.z());
                  lines.emplace_back(p1, p2);
                }
              }
            }
          }
          ++kernel_iter;
        }
      }
      ++id_subgrid_iter;
    }
    all_lines.push_back(lines);
  }

  return new bvpl_corner_pairs(all_lines);
}

bvpl_corner_pairs_sptr bvpl_corner_pair_finder::find_pairs(bvxm_voxel_grid<int>* id_grid,
                                                           bvxm_voxel_grid<float>* response_grid,
                                                           const bvpl_kernel_vector_sptr& search_kernels,
                                                           const bvpl_kernel_vector_sptr& corner_kernels)
{
  std::vector<std::vector<vgl_line_segment_3d<int> > > all_lines;
  std::vector<std::vector<vgl_box_3d<int> > > all_boxes;

  //1. run the same direction(id) kernel as the one we are interested in
  for (int target_id=0; target_id<search_kernels->size(); ++target_id)
  {
    bvpl_kernel_sptr this_search_kernel = search_kernels->kernels_[target_id];

    bvpl_kernel_sptr target_corner_kernel = corner_kernels->kernels_[target_id];

    std::cout << "Search kernel " << target_id << " with size" << this_search_kernel->max_point() << ',' << this_search_kernel->min_point()<<std::endl;

    /*******************************************************************/
    bvpl_subgrid_iterator<int> id_subgrid_iter(id_grid, this_search_kernel->min_point(), this_search_kernel->max_point());
    bvpl_subgrid_iterator<float> response_subgrid_iter(response_grid, this_search_kernel->min_point(), this_search_kernel->max_point());
    std::vector<vgl_line_segment_3d<int> > lines;
    std::vector<vgl_box_3d<int> > boxes;

    while (!id_subgrid_iter.isDone() && !response_subgrid_iter.isDone())
    {
      bvpl_voxel_subgrid<float> response_subgrid = *response_subgrid_iter;
      if (response_subgrid.get_voxel() > 1e-15)
      {
        bvpl_kernel_iterator kernel_iter = this_search_kernel->iterator();
        bvpl_voxel_subgrid<int> id_subgrid = *id_subgrid_iter;
        vgl_point_3d<int> p1 = id_subgrid_iter.global_cur_voxel();
        int id_at_center = id_subgrid.get_voxel();
        if (id_at_center == target_id)
        {
          //get the box of kernel at this point for visualization purposes
          vgl_point_3d<int> local_min=this_search_kernel->min_point();
          vgl_point_3d<int> local_max=this_search_kernel->max_point();
          vgl_point_3d<int> min_pt(p1.x() + local_min.x(), p1.y()+local_min.y(), p1.z()-local_max.z());
          vgl_point_3d<int> max_pt(p1.x() + local_max.x(), p1.y()+local_max.y(), p1.z()-local_min.z());
          boxes.emplace_back(min_pt, max_pt);

          kernel_iter.begin();
          while (!kernel_iter.isDone())
          {
            vgl_point_3d<int> idx = kernel_iter.index();
            float this_response;// = 0.0f;
            response_subgrid.voxel(idx, this_response);
            if (this_response > 1e-15)
            {
              int this_id=-1;
              if (id_subgrid.voxel(idx, this_id)) {
                if (this_id >=0) {
                  bvpl_kernel_sptr this_corner_kernel = corner_kernels->kernels_[this_id];
#ifdef DEBUG
                  std::cout << idx << std::endl;
#endif // DEBUG
                  if (this_corner_kernel->axis()== target_corner_kernel->axis()) {
                    //same plane now look for opposite diagonal corners
                    if (std::abs(std::sin(this_corner_kernel->angle() - target_corner_kernel->angle()) - 1.0) < 1e-4) {
                      std::cout << "this angle " <<this_corner_kernel->angle() << " target angle " << target_corner_kernel->angle()
                               << " condition " <<std::abs((this_corner_kernel->angle() - target_corner_kernel->angle()) - vnl_math::pi_over_2)
                               << " response " << this_response << " this_id " << this_id << '\n';
                      vgl_point_3d<int> p2(p1.x()+idx.x(), p1.y()+idx.y(), p1.z()-idx.z());
                      lines.emplace_back(p1, p2);
                    }
                  }
                }
              }
              else std::cout << "**********************************\n";
            }
            ++kernel_iter;
          }
        }
       }
       ++id_subgrid_iter;
      ++response_subgrid_iter;
    }
    std::cout << "Target id " << target_id << " has " << lines.size() << " lines\n";
    all_lines.push_back(lines);
    all_boxes.push_back(boxes);
  }

  return new bvpl_corner_pairs(all_lines, all_boxes);
}


bvpl_corner_pairs_sptr bvpl_corner_pair_finder::find_pairs(bvxm_voxel_grid<int>* id_grid,
                                                           bvxm_voxel_grid<float>* response_grid,
                                                           const bvpl_kernel_vector_sptr& search_kernels,
                                                           const bvpl_kernel_vector_sptr& corner_kernels,
                                                           bvxm_voxel_grid<bvpl_pair> *pair_grid)
{
  std::vector<std::vector<vgl_line_segment_3d<int> > > all_lines;
  std::vector<std::vector<vgl_box_3d<int> > > all_boxes;

  //1. run the same direction(id) kernel as the one we are interested in
  for (int target_id=0; target_id<search_kernels->size(); ++target_id)
  {
    bvpl_kernel_sptr this_search_kernel = search_kernels->kernels_[target_id];

    bvpl_kernel_sptr target_corner_kernel = corner_kernels->kernels_[target_id];

    std::cout << "Search kernel " << target_id << " with size" << this_search_kernel->max_point() << ',' << this_search_kernel->min_point()<<std::endl;

    /*******************************************************************/
    bvpl_subgrid_iterator<int> id_subgrid_iter(id_grid, this_search_kernel->min_point(), this_search_kernel->max_point());
    bvpl_subgrid_iterator<float> response_subgrid_iter(response_grid, this_search_kernel->min_point(), this_search_kernel->max_point());
    bvpl_subgrid_iterator<bvpl_pair> pair_subgrid_iter(pair_grid, this_search_kernel->min_point(), this_search_kernel->max_point());
    std::vector<vgl_line_segment_3d<int> > lines;
    std::vector<vgl_box_3d<int> > boxes;

    while (!id_subgrid_iter.isDone() && !response_subgrid_iter.isDone() && !pair_subgrid_iter.isDone())
    {
      bvpl_voxel_subgrid<float> response_subgrid = *response_subgrid_iter;

      if (response_subgrid.get_voxel() > 1e-15)
      {
        bvpl_kernel_iterator kernel_iter = this_search_kernel->iterator();
        bvpl_voxel_subgrid<int> id_subgrid = *id_subgrid_iter;
        vgl_point_3d<int> p1 = id_subgrid_iter.global_cur_voxel();
        int id_at_center = id_subgrid.get_voxel();
        if (id_at_center == target_id)
        {
          //get the box of kernel at this point for visualization purposes
          vgl_point_3d<int> local_min=this_search_kernel->min_point();
          vgl_point_3d<int> local_max=this_search_kernel->max_point();
          vgl_point_3d<int> min_pt(p1.x() + local_min.x(), p1.y()+local_min.y(), p1.z()-local_max.z());
          vgl_point_3d<int> max_pt(p1.x() + local_max.x(), p1.y()+local_max.y(), p1.z()-local_min.z());
          boxes.emplace_back(min_pt, max_pt);

          kernel_iter.begin();
          while (!kernel_iter.isDone())
          {
            vgl_point_3d<int> idx = kernel_iter.index();
            float this_response;// = 0.0f;
            response_subgrid.voxel(idx, this_response);
            if (this_response > 1e-15)
            {
              int this_id=-1;
              if (id_subgrid.voxel(idx, this_id)) {
                if (this_id >=0) {
                  bvpl_kernel_sptr this_corner_kernel = corner_kernels->kernels_[this_id];
                  // std::cout << idx << std::endl;
                  if (this_corner_kernel->axis()== target_corner_kernel->axis()){
                    //same plane now look for opossite diagonal corners
                    if (std::abs(std::sin(this_corner_kernel->angle() - target_corner_kernel->angle()) - 1.0) < 1e-4) {
                      bvpl_feature f1(p1, target_id, target_corner_kernel->axis(), target_corner_kernel->angle(),response_subgrid.get_voxel());
                      vgl_point_3d<int> p2(p1.x()+idx.x(), p1.y()+idx.y(), p1.z()-idx.z());
                      vgl_point_3d<int> p2_2(p1.x()+idx.x()/2, p1.y()+idx.y()/2, p1.z()-idx.z()/2);
                      lines.emplace_back(p1, p2);
                      bvpl_feature f2(p2, this_id, this_corner_kernel->axis(), this_corner_kernel->angle(), this_response);
                      bvpl_pair this_pair(p2_2, f1, f2, target_id, this_search_kernel->axis(), this_search_kernel->angle(), 1.0f);
                      (*pair_subgrid_iter).set_voxel_at(idx.x()/2, idx.y()/2, idx.z()/2, this_pair);
                    }
                  }
                }
              }
              else std::cout << "**********************************\n";
            }
            ++kernel_iter;
          }
        }
      }
      ++id_subgrid_iter;
      ++response_subgrid_iter;
      ++pair_subgrid_iter;
    }
    std::cout << "Target id " << target_id << " has " << lines.size() << " lines\n";
    all_lines.push_back(lines);
    all_boxes.push_back(boxes);
  }

  return new bvpl_corner_pairs(all_lines, all_boxes);
}


void bvpl_corner_pair_finder::find_pairs_no_lines(bvxm_voxel_grid<int>* id_grid,
                                                  bvxm_voxel_grid<float>* response_grid,
                                                  const bvpl_kernel_vector_sptr& search_kernels,
                                                  const bvpl_kernel_vector_sptr& corner_kernels,
                                                  bvxm_voxel_grid<bvpl_pair> *pair_grid)
{
  //1. run the same direction(id) kernel as the one we are interested in
  for (int target_id=0; target_id<search_kernels->size(); ++target_id)
  {
    bvpl_kernel_sptr this_search_kernel = search_kernels->kernels_[target_id];

    bvpl_kernel_sptr target_corner_kernel = corner_kernels->kernels_[target_id];

    std::cout << "Search kernel " << target_id << " with size" << this_search_kernel->max_point() << ',' << this_search_kernel->min_point()<<std::endl;

    /*******************************************************************/
    bvpl_subgrid_iterator<int> id_subgrid_iter(id_grid, this_search_kernel->min_point(), this_search_kernel->max_point());
    bvpl_subgrid_iterator<float> response_subgrid_iter(response_grid, this_search_kernel->min_point(), this_search_kernel->max_point());
    bvpl_subgrid_iterator<bvpl_pair> pair_subgrid_iter(pair_grid, this_search_kernel->min_point(), this_search_kernel->max_point());

    while (!id_subgrid_iter.isDone() && !response_subgrid_iter.isDone() && !pair_subgrid_iter.isDone())
    {
      bvpl_voxel_subgrid<float> response_subgrid = *response_subgrid_iter;

      if (response_subgrid.get_voxel() > 1e-15)
      {
        bvpl_kernel_iterator kernel_iter = this_search_kernel->iterator();
        bvpl_voxel_subgrid<int> id_subgrid = *id_subgrid_iter;
        vgl_point_3d<int> p1 = id_subgrid_iter.global_cur_voxel();
        int id_at_center = id_subgrid.get_voxel();
        if (id_at_center == target_id)
        {
          kernel_iter.begin();
          while (!kernel_iter.isDone())
          {
            vgl_point_3d<int> idx = kernel_iter.index();
            float this_response;// = 0.0f;
            response_subgrid.voxel(idx, this_response);
            if (this_response > 1e-15)
            {
              int this_id=-1;
              if (id_subgrid.voxel(idx, this_id)) {
                if (this_id >=0) {
                  bvpl_kernel_sptr this_corner_kernel = corner_kernels->kernels_[this_id];
                  // std::cout << idx << std::endl;
                  if (this_corner_kernel->axis()== target_corner_kernel->axis()){
                    //same plane now look for opossite diagonal corners
                    if (std::abs(std::sin(this_corner_kernel->angle() - target_corner_kernel->angle()) - 1.0) < 1e-4) {
                      bvpl_feature f1(p1, target_id, target_corner_kernel->axis(), target_corner_kernel->angle(),response_subgrid.get_voxel());
                      vgl_point_3d<int> p2(p1.x()+idx.x(), p1.y()+idx.y(), p1.z()-idx.z());
                      vgl_point_3d<int> p2_2(p1.x()+idx.x()/2, p1.y()+idx.y()/2, p1.z()-idx.z()/2);
                      bvpl_feature f2(p2, this_id, this_corner_kernel->axis(), this_corner_kernel->angle(), this_response);
                      bvpl_pair this_pair(p2_2, f1, f2, target_id, this_search_kernel->axis(), this_search_kernel->angle(), 1.0f);
                      (*pair_subgrid_iter).set_voxel_at(idx.x()/2, idx.y()/2, idx.z()/2, this_pair);
                    }
                  }
                }
              }
              else std::cout << "**********************************\n";
            }
            ++kernel_iter;
          }
        }
      }
      ++id_subgrid_iter;
      ++response_subgrid_iter;
      ++pair_subgrid_iter;
    }
  }

  return;
}

bvpl_corner_pairs_sptr bvpl_corner_pair_finder::find_pairs(bvxm_voxel_grid<bvpl_pair> *pair_grid_in,
                                                           const bvpl_kernel_vector_sptr& search_kernels,
                                                           bvxm_voxel_grid<bvpl_pair> *pair_grid_out,
                                                           int opposite_angle)
{
  std::vector<std::vector<vgl_line_segment_3d<int> > > all_lines;
  std::vector<std::vector<vgl_box_3d<int> > > all_boxes;

  //1. run the same direction(id) kernel as the one we are interested in
  for (int target_id=0; target_id<search_kernels->size(); ++target_id)
  {
    bvpl_kernel_sptr this_search_kernel = search_kernels->kernels_[target_id];

    std::cout << "\n Search kernel " << target_id << " with size" << this_search_kernel->max_point() << ',' << this_search_kernel->min_point()
             << "Axis " << this_search_kernel->axis() << " Angle " << this_search_kernel->angle() <<std::endl;

    /*******************************************************************/
    bvpl_subgrid_iterator<bvpl_pair> in_pair_subgrid_iter(pair_grid_in, this_search_kernel->min_point(), this_search_kernel->max_point());
    bvpl_subgrid_iterator<bvpl_pair> out_pair_subgrid_iter(pair_grid_out, this_search_kernel->min_point(), this_search_kernel->max_point());
    std::vector<vgl_line_segment_3d<int> > lines;
    std::vector<vgl_box_3d<int> > boxes;

    while (!in_pair_subgrid_iter.isDone() && !out_pair_subgrid_iter.isDone())
    {
      float response_at_center = (*in_pair_subgrid_iter).get_voxel().response();

      if (response_at_center > 1e-15)
      {
        bvpl_kernel_iterator kernel_iter = this_search_kernel->iterator();
        vgl_point_3d<int> p1 = in_pair_subgrid_iter.global_cur_voxel();
        bvpl_pair pair_at_center =(*in_pair_subgrid_iter).get_voxel();
        int id_at_center = pair_at_center.id();

        if (id_at_center == target_id)
        {
          //get the box of kernel at this point for visualization purposes
          vgl_point_3d<int> local_min=this_search_kernel->min_point();
          vgl_point_3d<int> local_max=this_search_kernel->max_point();
          vgl_point_3d<int> min_pt(p1.x() + local_min.x(), p1.y()+local_min.y(), p1.z()-local_max.z());
          vgl_point_3d<int> max_pt(p1.x() + local_max.x(), p1.y()+local_max.y(), p1.z()-local_min.z());
          boxes.emplace_back(min_pt, max_pt);


          kernel_iter.begin();
          while (!kernel_iter.isDone())
          {
            vgl_point_3d<int> idx = kernel_iter.index();
            bvpl_pair this_pair;
            (*in_pair_subgrid_iter).voxel(idx, this_pair);
            float this_response = this_pair.response();
            if (this_response > 1e-15)
            {
              int this_id=this_pair.id();
              if (this_id >=0)
              {
                //check that current visiting feature has the same axis as feature at center
                if (this_pair.axis()== pair_at_center.axis())
                {
                  //same plane now look for opossite diagonal corners
                  bool condition = false;

                  if (opposite_angle == 90)
                    condition=(std::abs(std::sin(this_pair.angle() - pair_at_center.angle()) - 1.0) < 1e-4);
                  if (opposite_angle == 180)
                    condition=(std::abs(std::sin((this_pair.angle() - pair_at_center.angle())/2.0f) - 1.0) < 1e-4);

                  if (condition)
                  {
                    bvpl_feature f1(p1, target_id, pair_at_center.axis(), pair_at_center.angle(),response_at_center);
                    vgl_point_3d<int> p2(p1.x()+idx.x(), p1.y()+idx.y(), p1.z()-idx.z());
                    vgl_point_3d<int> p2_2(p1.x()+idx.x()/2, p1.y()+idx.y()/2, p1.z()-idx.z()/2);
                    lines.emplace_back(p1, p2);
                    bvpl_feature f2(p2, this_id, this_pair.axis(), this_pair.angle(), this_response);
                    bvpl_pair out_pair(p2_2, f1, f2, target_id, this_search_kernel->axis(), this_search_kernel->angle(), 1.0f);
                    (*out_pair_subgrid_iter).set_voxel_at(idx.x()/2, idx.y()/2, idx.z()/2, out_pair);
#if 0
                    std::cout << "**********************************\n"
                             << "this angle " << this_pair.angle() << " axis " << this_pair.axis() << '\n'
                             << "center angle " << pair_at_center.angle() << " center axis " << pair_at_center.axis()
                             << " condition " << opposite_angles << '\n';
#endif // 0
                  }
                }
              }
              else std::cout << "**********************************\n";
            }
            ++kernel_iter;
          }
        }
      }
      ++in_pair_subgrid_iter;
      ++out_pair_subgrid_iter;
    }
    if (lines.size() > 0)
      std::cout << "\n Target id " << target_id << " has " << lines.size() << " lines\n";
    all_lines.push_back(lines);
    all_boxes.push_back(boxes);
  }

  return new bvpl_corner_pairs(all_lines, all_boxes);
}

void bvpl_corner_pair_finder::find_pairs_no_lines(bvxm_voxel_grid<bvpl_pair> *pair_grid_in,
                                                  const bvpl_kernel_vector_sptr& search_kernels,
                                                  bvxm_voxel_grid<bvpl_pair> *pair_grid_out,
                                                  int opposite_angle)
{
  //1. run the same direction(id) kernel as the one we are interested in
  for (int target_id=0; target_id<search_kernels->size(); ++target_id)
  {
    bvpl_kernel_sptr this_search_kernel = search_kernels->kernels_[target_id];

    std::cout << "\n Search kernel " << target_id << " with size" << this_search_kernel->max_point() << ',' << this_search_kernel->min_point()
             << "Axis " << this_search_kernel->axis() << " Angle " << this_search_kernel->angle() <<std::endl;

    /*******************************************************************/
    bvpl_subgrid_iterator<bvpl_pair> in_pair_subgrid_iter(pair_grid_in, this_search_kernel->min_point(), this_search_kernel->max_point());
    bvpl_subgrid_iterator<bvpl_pair> out_pair_subgrid_iter(pair_grid_out, this_search_kernel->min_point(), this_search_kernel->max_point());
    std::vector<vgl_box_3d<int> > boxes;

    while (!in_pair_subgrid_iter.isDone() && !out_pair_subgrid_iter.isDone())
    {
      float response_at_center = (*in_pair_subgrid_iter).get_voxel().response();

      if (response_at_center > 1e-15)
      {
        bvpl_kernel_iterator kernel_iter = this_search_kernel->iterator();
        vgl_point_3d<int> p1 = in_pair_subgrid_iter.global_cur_voxel();
        bvpl_pair pair_at_center =(*in_pair_subgrid_iter).get_voxel();
        int id_at_center = pair_at_center.id();

        if (id_at_center == target_id)
        {
          kernel_iter.begin();
          while (!kernel_iter.isDone())
          {
            vgl_point_3d<int> idx = kernel_iter.index();
            bvpl_pair this_pair;
            (*in_pair_subgrid_iter).voxel(idx, this_pair);
            float this_response = this_pair.response();
            if (this_response > 1e-15)
            {
              int this_id=this_pair.id();
              if (this_id >=0) {
                //check that current visiting feature has the same axis as feature at center
                if (this_pair.axis()== pair_at_center.axis())
                {
                  //same plane now look for opossite diagonal corners
                  bool condition = false;

                  if (opposite_angle == 90)
                    condition=(std::abs(std::sin(this_pair.angle() - pair_at_center.angle()) - 1.0) < 1e-4);
                  if (opposite_angle == 180)
                    condition=(std::abs(std::sin((this_pair.angle() - pair_at_center.angle())/2.0f) - 1.0) < 1e-4);

                  if (condition)
                  {
                    bvpl_feature f1(p1, target_id, pair_at_center.axis(), pair_at_center.angle(),response_at_center);
                    vgl_point_3d<int> p2(p1.x()+idx.x(), p1.y()+idx.y(), p1.z()-idx.z());
                    vgl_point_3d<int> p2_2(p1.x()+idx.x()/2, p1.y()+idx.y()/2, p1.z()-idx.z()/2);
                    bvpl_feature f2(p2, this_id, this_pair.axis(), this_pair.angle(), this_response);
                    bvpl_pair out_pair(p2_2, f1, f2, target_id, this_search_kernel->axis(), this_search_kernel->angle(), 1.0f);
                    (*out_pair_subgrid_iter).set_voxel_at(idx.x()/2, idx.y()/2, idx.z()/2, out_pair);
#if 0
                    std::cout << "**********************************\n"
                             << "this angle " <<this_pair.angle() << " axis " << this_pair.axis() << '\n'
                             << "center angle " << pair_at_center.angle() << " center axis " << pair_at_center.axis()
                             << " condition " << opposite_angles << '\n';
#endif // 0
                  }
                }
              }
              else std::cout << "**********************************\n";
            }
            ++kernel_iter;
          }
        }
      }
      ++in_pair_subgrid_iter;
      ++out_pair_subgrid_iter;
    }
  }

  return;
}
