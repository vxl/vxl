#ifndef bvpl_direction_to_color_map_h_
#define bvpl_direction_to_color_map_h_
//:
// \file
#include <bvxm/grid/bvxm_voxel_grid.h>
#include <bvpl/bvpl_kernel_factory.h>
#include <bdgl/bdgl_peano_curve.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_closest_point.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_line_3d_2_points.h>
#include <vil/algo/vil_colour_space.h>

#include <bxml/bsvg/bsvg_document.h>
#include <bxml/bsvg/bsvg_element.h>
#include <bxml/bsvg/bsvg_plot.h>
#include <bxml/bxml_find.h>
#include <bxml/bxml_write.h>
#include <vcl_iomanip.h>
#include <vcl_limits.h>

struct point_3d_cmp
{
  bool operator()(vgl_point_3d<double> p1, vgl_point_3d<double> p2) const
  {
    if (p2.x()- p1.x()> vcl_numeric_limits<double>::epsilon() )
      return true;
    else if (p2.x()- p1.x() < vcl_numeric_limits<double>::epsilon() )
    {
      if (p2.y()- p1.y()> vcl_numeric_limits<double>::epsilon() )
        return true;
      else if (p2.y()-p1.y() <vcl_numeric_limits<double>::epsilon())
        return (p2.z()- p1.z()>vcl_numeric_limits<double>::epsilon());
      else
        return false;
    }
    else
      return false;
  }
};

//: project a unit radius sphere onto the cube circumscribing it using gnomonic projection
void project_sphereical_samples_to_cubes(vcl_vector<vgl_point_3d<double> > samples, vcl_vector<vgl_point_3d<double> > & proj_on_cube)
{
  vcl_vector<vgl_vector_3d<double> > normals;
  // X high
  normals.push_back(vgl_vector_3d<double>(1,0,0));
  // Y high
  normals.push_back(vgl_vector_3d<double>(0,1,0));
  // Z high
  normals.push_back(vgl_vector_3d<double>(0,0,1));
  // X low
  normals.push_back(vgl_vector_3d<double>(-1,0,0));
  // Y low
  normals.push_back(vgl_vector_3d<double>(0,-1,0));
  // Z low
  normals.push_back(vgl_vector_3d<double>(0,0,-1));
  //: iterate over all the spherical samples
  for (unsigned i=0;i<samples.size();++i)
  {
    double tmax=-10.0;
    int jmax=-1;
    for (unsigned j=0;j<normals.size();++j)
    {
      double t=samples[i].x()*normals[j].x()+
               samples[i].y()*normals[j].y()+
               samples[i].z()*normals[j].z();
      if (t>tmax || j==0)
      {
        tmax=t;
        jmax=j;
      }
    }
    // find intersection with appropriate plane
    switch (jmax) {
      case 0:
        proj_on_cube.push_back(vgl_point_3d<double>
                               (1.0,samples[i].y()/samples[i].x(),samples[i].z()/samples[i].x()));
        break;
      case 1:
        proj_on_cube.push_back(vgl_point_3d<double>
                               (samples[i].x()/samples[i].y(),1.0,samples[i].z()/samples[i].y()));
        break;
      case 2:
        proj_on_cube.push_back(vgl_point_3d<double>
                               (samples[i].x()/samples[i].z(),samples[i].y()/samples[i].z(),1.0));
        break;
      case 3:
        proj_on_cube.push_back(vgl_point_3d<double>
                               (-1.0,-samples[i].y()/samples[i].x(),-samples[i].z()/samples[i].x()));
        break;
      case 4:
        proj_on_cube.push_back(vgl_point_3d<double>
                               (-samples[i].x()/samples[i].y(),-1.0,-samples[i].z()/samples[i].y()));
        break;
      case 5:
        proj_on_cube.push_back(vgl_point_3d<double>
                               (-samples[i].x()/samples[i].z(),-samples[i].y()/samples[i].z(),-1.0));
        break;
    }
  }
}

vcl_map<vgl_point_3d<double>, float,point_3d_cmp>
find_closest_points_from_cube_to_peano_curve(vcl_vector<vgl_point_3d<double> > samples,
                                             vcl_vector<vgl_point_3d<double> > peano_curve,
                                             vcl_vector<vgl_point_3d<double> > proj_on_cube)
{
  vcl_map<vgl_point_3d<double>, float,point_3d_cmp> indices_of_cube_projs;
  for (unsigned i=0;i<proj_on_cube.size();++i)
  {
    double dmin=1e5;
    int indexj=-1;
    //: find the closest index point
    for (unsigned j=0;j<peano_curve.size();++j)
    {
      double d=vgl_distance<double>(proj_on_cube[i],peano_curve[j]);
      if (d<dmin || j==0)
      {
        dmin=d;
        indexj=j;
      }
    }
    //: find the closest point on the two line segments emanating from the index point
    if (indexj==0)
    {
      vgl_line_3d_2_points<double> l1(peano_curve[indexj],peano_curve[indexj+1]);
      double t1=vgl_closest_point_t(l1,proj_on_cube[i]);
      if (t1<0)
        t1=0;

      double length=(peano_curve[indexj+1]-peano_curve[indexj]).length();
      indices_of_cube_projs[samples[i]]=float(indexj+t1/length);
    }
    else if (unsigned(indexj+1)==proj_on_cube.size())
    {
      vgl_line_3d_2_points<double> l2(peano_curve[indexj-1],peano_curve[indexj]);
      double t2=vgl_closest_point_t(l2,proj_on_cube[i]);
      if (t2<0)
        t2=0;
      double length=(peano_curve[indexj-1]-peano_curve[indexj]).length();
      indices_of_cube_projs[samples[i]]=float(indexj-1+t2/length);
    }
    else
    {
      vgl_line_3d_2_points<double> l1(peano_curve[indexj],peano_curve[indexj+1]);
      vgl_line_3d_2_points<double> l2(peano_curve[indexj-1],peano_curve[indexj]);

      double t1=vgl_closest_point_t(l1,proj_on_cube[i]);
      double t2=vgl_closest_point_t(l2,proj_on_cube[i]);

      if (vcl_fabs(t1)<vcl_fabs(t2))
      {
        if (t1<0)
          t1=0;
        double length=(peano_curve[indexj+1]-peano_curve[indexj]).length();
        indices_of_cube_projs[samples[i]]=float(indexj+t1/length);
      }
      else
      {
        if (t2<0)
          t2=0;

        double length=(peano_curve[indexj-1]-peano_curve[indexj]).length();
        indices_of_cube_projs[samples[i]]=float(indexj-1+t2/length);
      }
    }
    //indices_of_cube_projs.push_back(indexj);
  }
  return indices_of_cube_projs;
}

//: function to map direction vectors to a color map.
bool bvpl_direction_to_color_map(vcl_vector<vgl_point_3d<double> > samples, vcl_map<vgl_point_3d<double>, float,point_3d_cmp> & color)
{
  vcl_vector<vgl_point_3d<double> >  proj_on_cube;
  project_sphereical_samples_to_cubes(samples,proj_on_cube);
  //: level 2 peano curve
  vcl_vector<vgl_point_3d<double> > peano_curve=peano_curve_on_cube(2);
  color=find_closest_points_from_cube_to_peano_curve(samples,peano_curve,proj_on_cube);
  //: linear mapping of peano curve to color values (0,1);
  vcl_map<float,vgl_point_3d<double> > color_samples;
  for (unsigned i=0;i<samples.size();++i)
    color_samples[color[samples[i]]]=samples[i];

  vcl_map<float, vgl_point_3d<double> >::iterator iter=color_samples.begin();
  for (float j=0; iter!=color_samples.end(); ++iter)
    color[iter->second]=++j;

  for (unsigned i=0;i<samples.size();++i)
    color[samples[i]]/=samples.size();

  return true;
}

void bvpl_generate_direction_samples_from_kernels(bvpl_kernel_vector_sptr kernel_vector,
                                                  vcl_vector<vgl_point_3d<double> > & samples)
{
  vcl_vector< vcl_pair<vnl_vector_fixed<float,3>, bvpl_kernel_sptr > >::iterator iter;
  for (iter=kernel_vector->begin();iter!=kernel_vector->end();++iter)
    samples.push_back(vgl_point_3d<double>(iter->first[0],iter->first[1],iter->first[2]));
}


void bvpl_convert_grid_to_hsv_grid(bvxm_voxel_grid<vnl_vector_fixed<float,4> > *grid,
                                   bvxm_voxel_grid<vnl_vector_fixed<float,4> > *out_grid,
                                   vcl_map<vgl_point_3d<double>,float,point_3d_cmp>  colors)
{
  bvxm_voxel_grid<vnl_vector_fixed<float,4> >::iterator grid1_it = grid->begin();
  bvxm_voxel_grid<vnl_vector_fixed<float,4> >::iterator grid2_it = out_grid->begin();

  float r,g,b;
  float col;
  for (; grid1_it != grid->end(); ++grid1_it, ++grid2_it)
  {
    bvxm_voxel_slab<vnl_vector_fixed<float,4> >::iterator slab1_it = (*grid1_it).begin();
    bvxm_voxel_slab<vnl_vector_fixed<float,4> >::iterator slab2_it = (*grid2_it).begin();
    for (; slab1_it!=(*grid1_it).end(); ++slab1_it ,++slab2_it)
    {
      vgl_point_3d<double> v((*slab1_it)[0],(*slab1_it)[1],(*slab1_it)[2]);
      col=colors[v]*360;
      vil_colour_space_HSV_to_RGB<float>(col,1.0f,255.0f,&r,&g,&b);
      vnl_vector_fixed<float,4> this_feature(r,g,b,(*slab1_it)[3]*255.0f);
      (*slab2_it)=this_feature;
    }
  }
}

void bvpl_make_svg_color_map(vcl_map<vgl_point_3d<double>,float,point_3d_cmp>  colors,
                             vcl_string outfile)
{
  bsvg_document doc(400, 400);

  vcl_map<vgl_point_3d<double>,float,point_3d_cmp>::iterator iter=colors.begin();
  vcl_map<float,vgl_point_3d<double> > colors_ordered_by_index;
  for (;iter!=colors.end();iter++)
  {
    colors_ordered_by_index[iter->second]=iter->first;
  }
  int i=0;  float r,g,b;
  vcl_map<float,vgl_point_3d<double> >::iterator iter1=colors_ordered_by_index.begin();
  for (;iter1!=colors_ordered_by_index.end();iter1++,i++)
  {
    float col=iter1->first*360;
    vil_colour_space_HSV_to_RGB<float>(col,1.0f,255.0f,&r,&g,&b);

    vcl_ostringstream os;
    os<<'#'<<vcl_setw(2)<<vcl_setfill('0')<<vcl_hex<<(int)r
      <<vcl_setw(2)<<vcl_setfill('0')<<vcl_hex<<(int)g
      <<vcl_setw(2)<<vcl_setfill('0')<<vcl_hex<<(int)b;

    vcl_ostringstream os_dir;
    os_dir.precision(2);
    os_dir<<'['<<vcl_setw(5)<<iter1->second.x()<<','<<vcl_setw(5)<<iter1->second.y()<<','<<vcl_setw(5)<<iter1->second.z()<<']';
    bsvg_text* t = new bsvg_text(os_dir.str());
    t->set_font_size(15);
    t->set_location(10.0f, 15.0f*(i+1));

    bsvg_ellipse* e1 = new bsvg_ellipse(25, 7);
    e1->set_location(250.0f, 15.0f*(i+1));
    e1->set_fill_color(os.str());
    doc.add_element(e1);
    doc.add_element(t);
  }

  bxml_write(outfile, doc);
}

#endif // bvpl_direction_to_color_map_h_
