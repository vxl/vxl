// This is contrib/prip/vpyr/examples/simple_segmentation/simple_segmentation_builder.cxx

#include "simple_segmentation_builder.h"
#include "vcl_limits.h"
#include "vmap/vmap_set_grid_structure.h"

simple_segmentation_builder::simple_segmentation_builder()
   : _pyramid(),_top_map(_pyramid)
{
}

void simple_segmentation_builder::initialise(const my_image_type & arg)
{
   vcl_cout<<"--------------------------------------------"<<vcl_endl ;
   vcl_cout<<"Begin initialization ..."<<vcl_endl ;
   //_chrono.start() ;
   vcl_cout<<"Initializing a grid structure."<<vcl_endl ;
   //_grid.initialize(arg.width(), arg.height()) ;

   //cout<<"Computing its dual."<<endl ;
   //_grid.set_dual() ;
	/*if (!_grid.valid_permutations())
      cerr<<"Before : Invalid permutations."<<endl ;
   */
  vcl_cout<<"Initialization of a 2X2 grid."<<vcl_endl ;
  
	_grid_structure.initialise(arg.ni(),arg.nj()) ;
	
  vmap_set_grid_structure(_top_map,_grid_structure) ;
	
	//_top_map.valid_permutations() ;
	_background=&_top_map.face(_grid_structure.background_index()) ;
	vcl_cout<<"Set the base of the pyramid."<<vcl_endl ;
	_top_map.set_pyramid_base_structure() ;
	//_pyramid.base_map().valid_permutations() ;
	/*b.build(_pyramid.base_map()) ;
	_pyramid.base_map().valid_permutations() ;
	vcl_cout<<"Set the base of the pyramid."<<vcl_endl ;
  _top_map.synchronise() ;
	 _top_map.valid_permutations() ;
	_background=&_top_map.face(_top_map.nb_faces()-1) ;
	*/
	 int i=0 ;
   for (my_top_map::vertex_iterator e=_top_map.begin_vertex(); e!=_top_map.end_vertex(); ++e, ++i)
   {
      e->set_value(i) ;
   }

   vcl_cout<<"Initialize values of edges."<<vcl_endl ;
   for (my_top_map::edge_iterator e=_top_map.begin_edge(); e!=_top_map.end_edge(); ++e)
   {
      if (!e->is_adjacent_to(*_background))
      {
         e->set_value(arg.top_left_ptr()[_top_map.position(e->left_face())]-arg.top_left_ptr()[_top_map.position(e->right_face())]) ;
			}
			else
			{
         e->set_value(2000) ;
			}
   }

   //_chrono.stop() ;
   vcl_cout<<"Initialization done in "<<vcl_endl ;//_chrono.read()<<" seconds."<<endl ;
   vcl_cout<<"--------------------------------------------"<<vcl_endl ;

}

void simple_segmentation_builder::draw_down_projection(my_image_type & res, const my_image_type & org) const
{
   res.set_size(_grid_structure.width(),_grid_structure.height()) ;
   vcl_cout<<"--------------------------------------------"<<vcl_endl ;
   vcl_cout<<"Drawing down projection of the top level ..."<<vcl_endl ;
   vcl_vector<vmap_face_index> corresp ;
	
		_pyramid.down_projection_faces(_pyramid.top_map_index(),corresp) ;
		

		for (int i=corresp.size()-1; i>=0; i--)
		{
				if (&_top_map.face(i)!=_background)
					res.top_left_ptr()[_pyramid.base_map().face_position(i)]= org.top_left_ptr()[_pyramid.base_map().face_position(corresp[i])] ;
		}

   vcl_cout<<"--------------------------------------------"<<vcl_endl ;
}

	/*
		build removal kernel
		vmap_non_oriented_kernel<removal_kernel_type> rk(top_level()) ;
		kernel.initialise() ;
		for (top_level_type::edge_iterator i=top_level().begin_edge(); i!=top_level().end_edge(); ++i)
		{
			// if can remove
			rk.add(*i) ;
		}
		rk.finalise() ;
		removal_top_level(rk) ;
		
		build_contraction_kernel
		contraction_kernel_type ck
	
	*/
void simple_segmentation_builder::filter_edge_below(int threshold) 
{
_pyramid.base_map().valid_permutations() ;
   my_top_removal_kernel rk(_top_map) ;
		
		vcl_cout<<"********************************************"<<vcl_endl ;
   vcl_cout<<"********************************************"<<vcl_endl ;
   vcl_cout<<" Constructing level "<<vcl_endl ;
   vcl_cout<<"--------------------------------------------"<<vcl_endl ;

   vcl_cout<<"removal kernel.."<<vcl_endl ;
		rk.initialise() ;
		for (my_top_map::edge_iterator e=_top_map.begin_edge(); e!=_top_map.end_edge(); ++e)
		{
			if (e->value()<=threshold && e->value()>=-threshold)
			{
				rk.add(*e) ; //vcl_cout<<e->value()<<" " ; vcl_cout.flush() ;
			}
		}
		rk.finalise() ;
		if (!rk.valid()) 
			vcl_cerr<<"KERNEL PROBLEM !"<<vcl_endl ;
		//if (!rk.empty())
		{
			vcl_cout<<"removal.."<<rk.size()<<vcl_endl ;
			_top_map.removal(rk) ;
			/*my_pyramid::level_type::dart_iterator dp=_pyramid.top_level().begin_dart() ;
						for (my_top_map::dart_iterator d=_top_map.begin_dart(); d!=_top_map.end_dart(); ++d, ++dp)
  		{
				if (dp->vertex().sequence_index()!=d->vertex().sequence_index())
					vcl_cout<<dp->vertex().sequence_index()<<"!="<<d->vertex().sequence_index()<<" " ;vcl_cout.flush() ;
  	}
			my_level::vertex_iterator vp=_pyramid.top_level().begin_vertex() ;
			for (my_top_map::vertex_iterator v=_top_map.begin_vertex(); v!=_top_map.end_vertex(); ++v, ++vp)
  		{
				if (_pyramid.base_map().vertex_position(vp->sequence_index())!=v->value())
				{
					vcl_cout<<_pyramid.base_map().vertex_position(vp->sequence_index())<<"ii"<<v->value()<<'/'<<vp->sequence_index()<<"pp"<<v->sequence_index()<<" " ;vcl_cout.flush() ;
				}
  	 }*/
		 vcl_cout<<"contract pendant darts.."<<vcl_endl ;
			my_top_map::contraction_kernel K2(_top_map) ;
			
  		K2.initialise() ;
  		K2.add_1_cycles() ;
  		K2.finalise() ;
			if (!K2.valid()) vcl_cout<<"error"<<vcl_endl ;
			
		 _top_map.contraction(K2) ;
			//vcl_cout<<_top_map.nb_vertices()<<'/'<<_pyramid.top_level().nb_vertices()<<" "<<_top_map.nb_darts()<<'/'<<_pyramid.top_level().nb_darts()<<vcl_endl ;
			/*dp=_pyramid.top_level().begin_dart() ;
			for (my_top_map::dart_iterator d=_top_map.begin_dart(); d!=_top_map.end_dart(); ++d, ++dp)
  		{
				if (dp->vertex().sequence_index()!=d->vertex().sequence_index())
				{
					vcl_cout<<dp->vertex().sequence_index()<<"!="<<d->vertex().sequence_index()<<" " ;vcl_cout.flush() ;
				}
  	}
			vp=_pyramid.top_level().begin_vertex() ;
			for (my_top_map::vertex_iterator v=_top_map.begin_vertex(); v!=_top_map.end_vertex(); ++v, ++vp)
  		{
				if (_pyramid.base_map().vertex_position(vp->sequence_index())!=v->value())
				{
					vcl_cout<<_pyramid.base_map().vertex_position(vp->sequence_index())<<"::"<<v->value()<<'/'<<vp->sequence_index()<<".."<<v->sequence_index()<<" " ;vcl_cout.flush() ;
				}
  	 }*/

  		vcl_cout<<"contract redundant darts.."<<vcl_endl ;
			K2.clear() ;
  		K2.initialise() ;
  		K2.add_2_cycles() ;
  		K2.finalise() ;
			if (!K2.valid()) vcl_cout<<"error"<<vcl_endl ;
			/*dp=_pyramid.top_level().begin_dart() ;
			for (my_top_map::dart_iterator d=_top_map.begin_dart(); d!=_top_map.end_dart(); ++d, ++dp)
  		{
				if (dp->vertex().sequence_index()!=d->vertex().sequence_index())
					vcl_cout<<dp->vertex().sequence_index()<<"!="<<d->vertex().sequence_index()<<" " ;vcl_cout.flush() ;
  	}*/
  		_top_map.contraction(K2) ;
		}
   vcl_cout<<"********************************************"<<vcl_endl ;
   vcl_cout<<"********************************************"<<vcl_endl ;

   //return true ;
}
