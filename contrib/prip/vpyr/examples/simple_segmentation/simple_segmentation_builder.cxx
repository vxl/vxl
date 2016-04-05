// This is prip/vpyr/examples/simple_segmentation/simple_segmentation_builder.cxx
#include "simple_segmentation_builder.h"
#include <vmap/vmap_set_grid_structure.h>

simple_segmentation_builder::simple_segmentation_builder()
  : pyramid_(),top_map_(pyramid_)
{
}

void simple_segmentation_builder::initialise(const my_image_type & arg)
{
  std::cout<<"--------------------------------------------"<<std::endl
          <<"Begin initialization ..."<<std::endl ;
#ifdef DEBUG
  chrono_.start() ;
#endif
  std::cout<<"Initializing a grid structure."<<std::endl ;
#if 0
  grid_.initialize(arg.width(), arg.height()) ;

  std::cout<<"Computing its dual."<<std::endl ;
  grid_.set_dual() ;
  if (!grid_.valid_permutations())
  {
    std::cerr<<"Before : Invalid permutations."<<std::endl ;
  }
#endif // 0

  std::cout<<"Initialization of a 2X2 grid."<<std::endl ;

  grid_structure_.initialise(arg.ni(),arg.nj()) ;

  vmap_set_grid_structure(top_map_,grid_structure_) ;

#if 0
  top_map_.valid_permutations() ;
#endif // 0
  background_=&top_map_.face(grid_structure_.background_index()) ;
  std::cout<<"Set the base of the pyramid."<<std::endl ;
  top_map_.set_pyramid_base_structure() ;
#if 0
  pyramid_.base_map().valid_permutations() ;
  b.build(pyramid_.base_map()) ;
  pyramid_.base_map().valid_permutations() ;
  std::cout<<"Set the base of the pyramid."<<std::endl ;
  top_map_.synchronise() ;
  top_map_.valid_permutations() ;
  background_=&top_map_.face(top_map_.nb_faces()-1) ;
#endif // 0

  int i=0 ;
  for (my_top_map::vertex_iterator e=top_map_.begin_vertex(); e!=top_map_.end_vertex(); ++e, ++i)
  {
    e->set_value(i) ;
  }

  std::cout<<"Initialize values of edges."<<std::endl ;
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
  std::cout<<"Initialization done" ;
#ifdef DEBUG
  std::cout<<" in "<<chrono_.read()<<" seconds"
#endif
  std::cout<<".\n--------------------------------------------"<<std::endl ;
}

void simple_segmentation_builder::draw_down_projection(my_image_type & res, const my_image_type & org) const
{
  res.set_size(grid_structure_.width(),grid_structure_.height()) ;
  std::cout<<"--------------------------------------------"<<std::endl
          <<"Drawing down projection of the top level ..."<<std::endl ;
  std::vector<vmap_face_index> corresp ;

  pyramid_.down_projection_faces(pyramid_.top_map_index(),corresp) ;

  for (int i=corresp.size()-1; i>=0; i--)
  {
    if (&top_map_.face(i)!=background_)
    {
      res.top_left_ptr()[pyramid_.base_map().face_position(i)]=org.top_left_ptr()[pyramid_.base_map().face_position(corresp[i])];
    }
  }

  std::cout<<"--------------------------------------------"<<std::endl ;
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

  std::cout<<"********************************************"<<std::endl
          <<"********************************************"<<std::endl
          <<" Constructing level "<<std::endl
          <<"--------------------------------------------"<<std::endl

          <<"removal kernel.."<<std::endl ;
  rk.initialise() ;
  for (my_top_map::edge_iterator e=top_map_.begin_edge(); e!=top_map_.end_edge(); ++e)
  {
    if (e->value()<=threshold && e->value()>=-threshold)
    {
      rk.add(*e) ;
#ifdef DEBUG
      std::cout<<e->value()<<' ' << std::flush;
#endif
    }
  }
  rk.finalise() ;
  if (!rk.valid())
  {
    std::cerr<<"KERNEL PROBLEM !"<<std::endl ;
  }
  //if (!rk.empty())
  {
    std::cout<<"removal.."<<rk.size()<<std::endl ;
    top_map_.removal(rk) ;
#ifdef DEBUG
    my_pyramid::level_type::dart_iterator dp=pyramid_.top_level().begin_dart() ;
    for (my_top_map::dart_iterator d=top_map_.begin_dart(); d!=top_map_.end_dart(); ++d, ++dp)
    {
      if (dp->vertex().sequence_index()!=d->vertex().sequence_index())
      {
        std::cout<<dp->vertex().sequence_index()<<"!="<<d->vertex().sequence_index()<<' ' << std::flush;
      }
    }
    my_level::vertex_iterator vp=pyramid_.top_level().begin_vertex() ;
    for (my_top_map::vertex_iterator v=top_map_.begin_vertex(); v!=top_map_.end_vertex(); ++v, ++vp)
    {
      if (pyramid_.base_map().vertex_position(vp->sequence_index())!=v->value())
      {
        std::cout<<pyramid_.base_map().vertex_position(vp->sequence_index())<<"ii"
                <<v->value()<<'/'<<vp->sequence_index()<<"pp"<<v->sequence_index()<<' ' << std::flush;
      }
    }
#endif // DEBUG
    std::cout<<"contract pendant darts.."<<std::endl ;
    my_top_map::contraction_kernel K2(top_map_) ;

    K2.initialise() ;
    K2.add_1_cycles() ;
    K2.finalise() ;
    if (!K2.valid()) std::cout<<"error"<<std::endl ;

    top_map_.contraction(K2) ;
#ifdef DEBUG
    std::cout<<top_map_.nb_vertices()<<'/'<<pyramid_.top_level().nb_vertices()<<' '
            <<top_map_.nb_darts()<<'/'<<pyramid_.top_level().nb_darts()<<std::endl ;
    dp=pyramid_.top_level().begin_dart() ;
    for (my_top_map::dart_iterator d=top_map_.begin_dart(); d!=top_map_.end_dart(); ++d, ++dp)
    {
      if (dp->vertex().sequence_index()!=d->vertex().sequence_index())
      {
        std::cout<<dp->vertex().sequence_index()<<"!="<<d->vertex().sequence_index()<<' ' << std::flush;
      }
    }
    vp=pyramid_.top_level().begin_vertex() ;
    for (my_top_map::vertex_iterator v=top_map_.begin_vertex(); v!=top_map_.end_vertex(); ++v, ++vp)
    {
      if (pyramid_.base_map().vertex_position(vp->sequence_index())!=v->value())
      {
        std::cout<<pyramid_.base_map().vertex_position(vp->sequence_index())<<"::"<<v->value()
                <<'/'<<vp->sequence_index()<<".."<<v->sequence_index()<<' ' << std::flush;
      }
    }
#endif // DEBUG

    std::cout<<"contract redundant darts.."<<std::endl ;
    K2.clear() ;
    K2.initialise() ;
    K2.add_2_cycles() ;
    K2.finalise() ;
    if (!K2.valid()) std::cout<<"error"<<std::endl ;
#ifdef DEBUG
    dp=pyramid_.top_level().begin_dart() ;
    for (my_top_map::dart_iterator d=top_map_.begin_dart(); d!=top_map_.end_dart(); ++d, ++dp)
    {
      if (dp->vertex().sequence_index()!=d->vertex().sequence_index())
      {
        std::cout<<dp->vertex().sequence_index()<<"!="<<d->vertex().sequence_index()<<' ' << std::flush;
      }
    }
#endif // DEBUG
    top_map_.contraction(K2) ;
  }
  std::cout<<"********************************************"<<std::endl
          <<"********************************************"<<std::endl ;
}
