// This is prip/vpyr/examples/simple_segmentation/simple_segmentation_builder.cxx
#include "simple_segmentation_builder.h"
#include <vmap/vmap_set_grid_structure.h>

simple_segmentation_builder::simple_segmentation_builder()
  : pyramid_(),top_map_(pyramid_)
{
}

void simple_segmentation_builder::initialise(const my_image_type & arg)
{
  vcl_cout<<"--------------------------------------------"<<vcl_endl
          <<"Begin initialization ..."<<vcl_endl ;
#ifdef DEBUG
  chrono_.start() ;
#endif
  vcl_cout<<"Initializing a grid structure."<<vcl_endl ;
#if 0
  grid_.initialize(arg.width(), arg.height()) ;

  vcl_cout<<"Computing its dual."<<vcl_endl ;
  grid_.set_dual() ;
  if (!grid_.valid_permutations())
  {
    vcl_cerr<<"Before : Invalid permutations."<<vcl_endl ;
  }
#endif // 0

  vcl_cout<<"Initialization of a 2X2 grid."<<vcl_endl ;

  grid_structure_.initialise(arg.ni(),arg.nj()) ;

  vmap_set_grid_structure(top_map_,grid_structure_) ;

#if 0
  top_map_.valid_permutations() ;
#endif // 0
  background_=&top_map_.face(grid_structure_.background_index()) ;
  vcl_cout<<"Set the base of the pyramid."<<vcl_endl ;
  top_map_.set_pyramid_base_structure() ;
#if 0
  pyramid_.base_map().valid_permutations() ;
  b.build(pyramid_.base_map()) ;
  pyramid_.base_map().valid_permutations() ;
  vcl_cout<<"Set the base of the pyramid."<<vcl_endl ;
  top_map_.synchronise() ;
  top_map_.valid_permutations() ;
  background_=&top_map_.face(top_map_.nb_faces()-1) ;
#endif // 0

  int i=0 ;
  for (my_top_map::vertex_iterator e=top_map_.begin_vertex(); e!=top_map_.end_vertex(); ++e, ++i)
  {
    e->set_value(i) ;
  }

  vcl_cout<<"Initialize values of edges."<<vcl_endl ;
  for (my_top_map::edge_iterator e=top_map_.begin_edge(); e!=top_map_.end_edge(); ++e)
  {
    if (!e->is_adjacent_to(*background_))
    {
      e->set_value(arg.top_left_ptr()[top_map_.position(e->left_face())]-arg.top_left_ptr()[top_map_.position(e->right_face())]);
    }
    else
    {
      e->set_value(2000) ;
    }
  }

#ifdef DEBUG
  chrono_.stop() ;
#endif
  vcl_cout<<"Initialization done" ;
#ifdef DEBUG
  vcl_cout<<" in "<<chrono_.read()<<" seconds"
#endif
  vcl_cout<<".\n--------------------------------------------"<<vcl_endl ;
}

void simple_segmentation_builder::draw_down_projection(my_image_type & res, const my_image_type & org) const
{
  res.set_size(grid_structure_.width(),grid_structure_.height()) ;
  vcl_cout<<"--------------------------------------------"<<vcl_endl
          <<"Drawing down projection of the top level ..."<<vcl_endl ;
  vcl_vector<vmap_face_index> corresp ;

  pyramid_.down_projection_faces(pyramid_.top_map_index(),corresp) ;

  for (int i=corresp.size()-1; i>=0; i--)
  {
    if (&top_map_.face(i)!=background_)
    {
      res.top_left_ptr()[pyramid_.base_map().face_position(i)]=org.top_left_ptr()[pyramid_.base_map().face_position(corresp[i])];
    }
  }

  vcl_cout<<"--------------------------------------------"<<vcl_endl ;
}

#if 0
  // build removal kernel
  vmap_non_oriented_kernel<removal_kernel_type> rk(top_level()) ;
  kernel.initialise() ;
  for (top_level_type::edge_iterator i=top_level().begin_edge(); i!=top_level().end_edge(); ++i)
  {
    // if can remove
    rk.add(*i) ;
  }
  rk.finalise() ;
  removal_top_level(rk) ;

  //build_contraction_kernel
  contraction_kernel_type ck ;
#endif // 0

void simple_segmentation_builder::filter_edge_below(int threshold)
{
  pyramid_.base_map().valid_permutations() ;
  my_top_removal_kernel rk(top_map_) ;

  vcl_cout<<"********************************************"<<vcl_endl
          <<"********************************************"<<vcl_endl
          <<" Constructing level "<<vcl_endl
          <<"--------------------------------------------"<<vcl_endl

          <<"removal kernel.."<<vcl_endl ;
  rk.initialise() ;
  for (my_top_map::edge_iterator e=top_map_.begin_edge(); e!=top_map_.end_edge(); ++e)
  {
    if (e->value()<=threshold && e->value()>=-threshold)
    {
      rk.add(*e) ;
#ifdef DEBUG
      vcl_cout<<e->value()<<' ' << vcl_flush;
#endif
    }
  }
  rk.finalise() ;
  if (!rk.valid())
  {
    vcl_cerr<<"KERNEL PROBLEM !"<<vcl_endl ;
  }
  //if (!rk.empty())
  {
    vcl_cout<<"removal.."<<rk.size()<<vcl_endl ;
    top_map_.removal(rk) ;
#ifdef DEBUG
    my_pyramid::level_type::dart_iterator dp=pyramid_.top_level().begin_dart() ;
    for (my_top_map::dart_iterator d=top_map_.begin_dart(); d!=top_map_.end_dart(); ++d, ++dp)
    {
      if (dp->vertex().sequence_index()!=d->vertex().sequence_index())
      {
        vcl_cout<<dp->vertex().sequence_index()<<"!="<<d->vertex().sequence_index()<<' ' << vcl_flush;
      }
    }
    my_level::vertex_iterator vp=pyramid_.top_level().begin_vertex() ;
    for (my_top_map::vertex_iterator v=top_map_.begin_vertex(); v!=top_map_.end_vertex(); ++v, ++vp)
    {
      if (pyramid_.base_map().vertex_position(vp->sequence_index())!=v->value())
      {
        vcl_cout<<pyramid_.base_map().vertex_position(vp->sequence_index())<<"ii"
                <<v->value()<<'/'<<vp->sequence_index()<<"pp"<<v->sequence_index()<<' ' << vcl_flush;
      }
    }
#endif // DEBUG
    vcl_cout<<"contract pendant darts.."<<vcl_endl ;
    my_top_map::contraction_kernel K2(top_map_) ;

    K2.initialise() ;
    K2.add_1_cycles() ;
    K2.finalise() ;
    if (!K2.valid()) vcl_cout<<"error"<<vcl_endl ;

    top_map_.contraction(K2) ;
#ifdef DEBUG
    vcl_cout<<top_map_.nb_vertices()<<'/'<<pyramid_.top_level().nb_vertices()<<' '
            <<top_map_.nb_darts()<<'/'<<pyramid_.top_level().nb_darts()<<vcl_endl ;
    dp=pyramid_.top_level().begin_dart() ;
    for (my_top_map::dart_iterator d=top_map_.begin_dart(); d!=top_map_.end_dart(); ++d, ++dp)
    {
      if (dp->vertex().sequence_index()!=d->vertex().sequence_index())
      {
        vcl_cout<<dp->vertex().sequence_index()<<"!="<<d->vertex().sequence_index()<<' ' << vcl_flush;
      }
    }
    vp=pyramid_.top_level().begin_vertex() ;
    for (my_top_map::vertex_iterator v=top_map_.begin_vertex(); v!=top_map_.end_vertex(); ++v, ++vp)
    {
      if (pyramid_.base_map().vertex_position(vp->sequence_index())!=v->value())
      {
        vcl_cout<<pyramid_.base_map().vertex_position(vp->sequence_index())<<"::"<<v->value()
                <<'/'<<vp->sequence_index()<<".."<<v->sequence_index()<<' ' << vcl_flush;
      }
    }
#endif // DEBUG

    vcl_cout<<"contract redundant darts.."<<vcl_endl ;
    K2.clear() ;
    K2.initialise() ;
    K2.add_2_cycles() ;
    K2.finalise() ;
    if (!K2.valid()) vcl_cout<<"error"<<vcl_endl ;
#ifdef DEBUG
    dp=pyramid_.top_level().begin_dart() ;
    for (my_top_map::dart_iterator d=top_map_.begin_dart(); d!=top_map_.end_dart(); ++d, ++dp)
    {
      if (dp->vertex().sequence_index()!=d->vertex().sequence_index())
      {
        vcl_cout<<dp->vertex().sequence_index()<<"!="<<d->vertex().sequence_index()<<' ' << vcl_flush;
      }
    }
#endif // DEBUG
    top_map_.contraction(K2) ;
  }
  vcl_cout<<"********************************************"<<vcl_endl
          <<"********************************************"<<vcl_endl ;
}
