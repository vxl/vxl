// This is prip/vpyr/examples/simple_segmentation/simple_segmentation_builder.h
#ifndef simple_segmentation_builder_h_
#define simple_segmentation_builder_h_
//:
// \file
// \brief a builder which builds a simple segmentation.
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  06 May 2004 Jocelyn Marchadier
// \endverbatim

#include <vmap/vmap_non_oriented_kernel.h>
#include <vmap/vmap_grid_structure.h>
#include <vpyr/vpyr_top_tmap.h>
#include <vpyr/vpyr_2_tpyramid.h>
#include <vil/vil_image_view.h>

//: a builder which builds a simple segmentation.
class simple_segmentation_builder
{
 public:
  class my_top_edge : public vmap_2_tmap_edge
  {
   public :

    void set_value(int arg)
    {
      value_=arg ;
    }

    int value() const
    {
      return value_;
    }

   private:

    int value_ ;
  };
  class my_top_vertex : public vmap_2_tmap_vertex
  {
   public :

    void set_value(int arg)
    {
      value_=arg ;
    }

    int value() const
    {
      return value_;
    }

   private:

    int value_ ;
  };
  typedef vpyr_2_tpyramid<> my_pyramid ;
  typedef my_pyramid::level_type my_level ;
  typedef vpyr_top_tmap<my_pyramid, my_top_vertex, my_top_edge> my_top_map ;
  typedef vmap_non_oriented_kernel<my_top_map::removal_kernel> my_top_removal_kernel ;

  typedef vil_image_view<vxl_byte> my_image_type ;

  simple_segmentation_builder();

  void initialise(const my_image_type & arg) ;

  void draw_down_projection(my_image_type & res, const my_image_type & org) const ;

  void filter_edge_below(int threshold) ;

  // Additional Public Declarations

 protected:
  vmap_grid_structure grid_structure_;

  my_pyramid pyramid_ ;
  my_top_map top_map_ ;

  my_top_map::face_type * background_ ;
};

#endif
