//:
// \file

#include <vcl_vector.h>
#include <vcl_algorithm.h>
#include <vnl/vnl_math.h>
#include <vcl_iostream.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>

#include <vtol/vtol_vertex.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_one_chain.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_face.h>
#include <vtol/vtol_cycle_processor.h>

//Constructors
vtol_cycle_processor::vtol_cycle_processor(vcl_vector<vtol_edge_2d_sptr>& edges)
{
  _tolerance = 0;
  init(edges);
}

//---------------------------------------------------------------
//
static void pop_stacks(vcl_vector<vtol_vertex_sptr>& verts, vcl_vector<vtol_edge_2d_sptr>& edges,
                       vtol_vertex_sptr& v, vcl_vector<vtol_edge_2d_sptr>& cycle_edges)
{
  bool found = false;
  while(verts.size()&&edges.size()&&!found)
    {
      found = (vtol_vertex_sptr)verts.back()==v.ptr();
      cycle_edges.push_back(edges.back());
      verts.pop_back();
      edges.pop_back();
    }
  if(!edges.size()&&verts.size())
    verts.clear();
}
//the user flags on SpatialObject are used to define the orientation
//of vtol_edge(s) during the boundary tracing process.  In effect, FLAG1 and
//FLAG2 define half edges. vtol_edges are used up when both half edges are used.
// Access to flags
static bool used(vtol_edge_2d_sptr& e)
{
  bool temp = e->get_user_flag(VSOL_FLAG1)&&e->get_user_flag(VSOL_FLAG2);
  return temp;
}

static bool unused(vtol_edge_2d_sptr& e)
{
  bool temp = !e->get_user_flag(VSOL_FLAG1)&&!e->get_user_flag(VSOL_FLAG2);
  return temp;
}
static bool plus_used(vtol_edge_2d_sptr& e)
{
  if (e->get_user_flag(VSOL_FLAG1)) return true; else return false;
}

static bool minus_used(vtol_edge_2d_sptr& e)
{
  if (e->get_user_flag(VSOL_FLAG2)) return true; else return false;
}

static bool half_used(vtol_edge_2d_sptr& e)
{
  unsigned dir1 = e->get_user_flag(VSOL_FLAG1);
  unsigned dir2 = e->get_user_flag(VSOL_FLAG2);
  if ((dir1&&!dir2)||(!dir1&&dir2)) return true; else return false;
}
// Assignment of flags
static void use_plus(vtol_edge_2d_sptr& e)
{
  e->set_user_flag(VSOL_FLAG1);
}

static void use_minus(vtol_edge_2d_sptr& e)
{
  e->set_user_flag(VSOL_FLAG2);
}
// One Chain flags
// predicates
static bool ccw(vtol_one_chain_sptr& ch)
{
  if (ch->get_user_flag(VSOL_FLAG1)) return true; else return false;
}

static bool cw(vtol_one_chain_sptr& ch)
{
  if (ch->get_user_flag(VSOL_FLAG2)) return true; else return false;
}

// assignment
static void set_ccw(vtol_one_chain_sptr& ch)
{
  ch->set_user_flag(VSOL_FLAG1);
}
static void set_cw(vtol_one_chain_sptr& ch)
{
  ch->set_user_flag(VSOL_FLAG2);
}
// other house keeping functions and predicates
// vtol_edge functions
static void clear(vtol_edge_2d_sptr& e)
{
  e->unset_user_flag(VSOL_FLAG1);
  e->unset_user_flag(VSOL_FLAG2);
  e->unset_user_flag(VSOL_FLAG3);
}
#if 0 // only untouch(vtol_one_chain_sptr&) is used
static void untouch(vtol_edge_2d_sptr& e)
{
  e->unset_user_flag(VSOL_FLAG3);
}
#endif

static void touch(vtol_edge_2d_sptr& e)
{
  e->set_user_flag(VSOL_FLAG3);
}

static bool touched(vtol_edge_2d_sptr& e)
{
  if (e->get_user_flag(VSOL_FLAG3)) return true; else return false;
}
static bool self_loop(vtol_edge_2d_sptr& e)
{
  vtol_vertex_sptr v1 = e->v1();
  vtol_vertex_sptr v2 = e->v2();
  bool loop = v1 == v2;
  return loop;
}

static bool bridge_traverse(float angle)
{
  double tol = 1e-3;
  float delta = vcl_fabs(vcl_fabs(angle)-180);
  if (delta<tol) return true; else return false;;
}

//vtol_one_chain functions
static void untouch(vtol_one_chain_sptr& ch)
{
  ch->unset_user_flag(VSOL_FLAG3);
}
#if 0 // only clear(vtol_edge_2d_sptr& ) is used
static void clear(vtol_one_chain_sptr& ch)
{
  ch->unset_user_flag(VSOL_FLAG1);
  ch->unset_user_flag(VSOL_FLAG2);
  ch->unset_user_flag(VSOL_FLAG3);
}
#endif

static void touch(vtol_one_chain_sptr& ch)
{
  ch->set_user_flag(VSOL_FLAG3);
}

static bool touched(vtol_one_chain_sptr& ch)
{
  if (ch->get_user_flag(VSOL_FLAG3)) return true; else return false;
}

//----------------------------------------------------------
//:   Get an array of edges attached to a vertex.
//    Only those edges
//    present in the given edge array are considered attached. Previously
//    un-traversed edges are returned unless force == true. Then edges
//    which are half-used are allowed in the returned set.
static void v_edges(vtol_vertex_sptr v, vcl_vector<vtol_edge_2d_sptr>& b_edges,
                    bool force, vcl_vector<vtol_edge_2d_sptr>& edges_at_vertex)
{
  edges_at_vertex.clear();
  vcl_vector<vtol_edge_sptr> *edges = v->edges();
  vcl_vector<vtol_edge_sptr>::iterator eit;
  for( eit = edges->begin(); eit != edges->end(); eit++)
    {
      vtol_edge_2d_sptr e = (*eit)->cast_to_edge_2d();
      if(vcl_find(b_edges.begin(), b_edges.end(),e) != b_edges.end()) 
        {
	  if(used(e))
	    continue;
	  if(unused(e))
	    edges_at_vertex.push_back(e);
	  if(half_used(e)&&force)
	    edges_at_vertex.push_back(e);
        }
    }
  delete edges;
}

static float tangent_angle_at_vertex(vtol_vertex_sptr v, vtol_edge_2d_sptr e)
{
  float ang = 0;
  if(!e||!v||!(v==e->v1()||v==e->v2()))
    {
      vcl_cout << "vtol_vertex and vtol_edge not incident" << vcl_endl;
      return ang;
    }
#if 0
  vsol_point_2d* p = v->get_point();
  vsol_point_2d st = e->GetCurve()->GetStartPoint();
  vsol_point_2d nd = e->GetCurve()->GetEndPoint();
#endif
  vtol_vertex_2d_sptr v2d = v->cast_to_vertex_2d();
  vtol_edge_2d_sptr e2d = e->cast_to_edge_2d();
  if(v2d->euclidean_distance(*e->v1()->cast_to_vertex_2d()) <= v2d->euclidean_distance(*e->v2()->cast_to_vertex_2d()))
    ang = e2d->curve()->cast_to_digital_curve()->get_interpolator()->get_theta(0);
  else
    {
    ang = e2d->curve()->cast_to_digital_curve()->get_interpolator()->get_theta(1);
    ang += 180.0;
    if(ang>360)
      ang -= 360;
    }
#if 0
  //-tpk- don't know if this is still needed
  //now if we are dealing with a 2-d digital curve then
  //flip y. All image-derived geometry has a left-handed Euclidan frame.
  if(e->GetCurve()->CastToDigitalCurve())
    {
      double rad_per_deg = IUE_math::pi/180.0;
      double rang = rad_per_deg*ang;
      double cs = vcl_cos(rang), si = -vcl_sin(rang);
      ang = vcl_atan2(si, cs)/rad_per_deg;
      if(ang<0)
        ang+=360;
    }
#endif
  return ang;
}
//----------------------------------------------------------------
//:   Find the vtol_vertex, if any,  which is shared by two vtol_edge(s)
static vtol_vertex_sptr common_vertex(vtol_edge_2d_sptr& e0, vtol_edge_2d_sptr& e1)
{
  vtol_vertex_sptr v01 = e0->v1(), v02 = e0->v2();
  vtol_vertex_sptr v11 = e1->v1(), v12 = e1->v2();
  if((vtol_vertex_sptr)v01==(vtol_vertex_sptr)v11)
    return v01;
  if((vtol_vertex_sptr)v01==(vtol_vertex_sptr)v12)
    return v01;
  if((vtol_vertex_sptr)v02==(vtol_vertex_sptr)v11)
    return v02;
  if((vtol_vertex_sptr)v02==(vtol_vertex_sptr)v12)
    return v02;
  return NULL;
}
//----------------------------------------------------------------
//:   Compute the angle between two edges at the specified vtol_vertex, v
//    The angle is mapped to the interval [-180, 180].  The angle sense is
//    defined so that the e0 orientation is towards v and the e1
//    orientation is away from v.
static float angle_between_edges(vtol_edge_2d_sptr e0, vtol_edge_2d_sptr e1, vtol_vertex_sptr v)
{
  float theta0 = 180+tangent_angle_at_vertex(v, e0);
  if(theta0>360)
    theta0 -= 360;
  float theta1 = tangent_angle_at_vertex(v, e1);
  float angle = theta1-theta0;
  if(angle>180)
    angle-=360;
  if(angle<-180)
    angle+=360;
  return angle;
}
//------------------------------------------------------------
//:   Find the most counter clockwise vtol_edge at the input vtol_vertex, from.
//
static vtol_edge_2d_sptr ccw_edge(vtol_edge_2d_sptr in_edg, vtol_vertex_sptr from,
                      vcl_vector<vtol_edge_2d_sptr>& edges)
{
  float most_ccw = -360;
  vtol_edge_2d_sptr ccw = NULL;
  for(vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges.begin();
      eit != edges.end(); eit++)
    {
      if((*eit)==in_edg)
        continue;
      float delta = angle_between_edges(in_edg, *eit, from);
      if(delta>most_ccw)
        {
          most_ccw = delta;
          ccw = *eit;
        }
    }
  //There were no edges found except the incoming edge, so return it.
  if (!ccw &&  vcl_find(edges.begin(), edges.end(),in_edg) != edges.end())
    ccw = in_edg;
  return ccw;
}
//-----------------------------------------------------
//:   Is an edge assignable to a path?
//    "Assignable" is defined by
//    the condition that an edge has not already been traversed in
//    the required direction.  That is, if a traversal from s to e,
//    (V1 to V2) has occured then dir1 is true.  A second traversal
//    is not allowed and the edge is considered un-assignable.
//
static bool assignable(vtol_edge_2d_sptr edg, vtol_vertex_sptr last)
{
  if(!(edg&&last))
    return false;
  if(unused(edg))
    return true;
  if(used(edg))
    return false;
  vtol_vertex_sptr s = edg->v1();
  vtol_vertex_sptr e = edg->v2();
  if(last==s&&!plus_used(edg))
    return true;
  if(last==e&&!minus_used(edg))
    return true;
  return false;
}
//----------------------------------------------------------------------
//:   Set up the first edge in a cycle (or bridge) traversal.
//    A positive
//    traversal (half edge) is in the direction from v1->v2.
//    Self loops are a special case and use both directions on one traversal.
//    There is no point in traversing the self loop twice.
static void assign_initial_edge(vtol_edge_2d_sptr& e,
                                vtol_vertex_sptr& first, vtol_vertex_sptr& last)
{
  if(used(e))
    {
      vcl_cout << "In vtol_cycle_processor::assign_intial_edge(..) "
           << "shouldn't happen - error" << vcl_endl;
      return;
    }
  vtol_vertex_sptr v1 = e->v1(), v2 = e->v2();
  if(v1==v2)
    {
      use_plus(e);
      use_minus(e);
      first = v1; last = v1;
    }
  if(plus_used(e))
    {
      use_minus(e);
      first = v2;
      last  = v1;
    }
  else
    {
      use_plus(e);
      first = v1;
      last  = v2;
    }
}

//------------------------------------------------------------
//:   Link the vtol_edge, "edg" to the vtol_vertex, "last".
//    Set the appropriate direction flag

static void assign_ends(vtol_edge_2d_sptr edg, vtol_vertex_sptr& last)
{
  vtol_vertex_sptr s = edg->v1();
  vtol_vertex_sptr e = edg->v2();
  // compare to last point added
  // Here we need to be able to use the previous
  // edge if there is no other choice
  if (last == s)
    {
      last = e;
      use_plus(edg);//Forward direction
      if(s==e)
        use_minus(edg);//For a self-loop, any traversal uses it up
      return;
    }
  if (last == e)
    {
      last = s;
      use_minus(edg);//Reverse direction
      if(s==e)
        use_plus(edg);//For a self-loop, any traversal uses it up
      return;
    }
  vcl_cout << "In vtol_cycle_processor::assign ends(..) - shouldn't happen" << vcl_endl;
}

//------------------------------------------------------------
//:
//    Assign the next edge to the top of the edge stack when
//    a cycle is popped off the stack. That is, start the new path
//    with the edge at the top of the stack.  If the stack is
//    empty, then the last assignment to _l is used.
static void assign_stack_edge(vcl_vector<vtol_edge_2d_sptr>& e_stack, vtol_edge_2d_sptr& next_edge)
{
  if(!e_stack.size())
    return;
  next_edge = e_stack.back();
}


//------------------------------------------------------------------
//:   The initial setup of the cycle process.
//    Used by the vtol_cycle_processor
//    constructors to establish the start conditions
void vtol_cycle_processor::init(vcl_vector<vtol_edge_2d_sptr>& in_edges)
{
  _edges.clear();
  _chains.clear();
  _nested_one_cycles.clear();
  for(vcl_vector<vtol_edge_2d_sptr>::iterator eit = in_edges.begin();
      eit != in_edges.end(); eit++)
    {
      clear(*eit);
      _edges.push_back(*eit);
    }
  this->set_bridge_vars();
  _valid = false;
}


//---------------------------------------------------------------
//:  Initializes the search for cycles starting with an unexplored vtol_edge.
//   This initialization is called after a connected set of vtol_edge(s) is
//   completely explored and removed from _edges.
void vtol_cycle_processor::set_bridge_vars()
{
  if(!_edges.size())
    return;
  _v_stack.clear();
  _e_stack.clear();
  _l = _edges[0];
  _e_stack.push_back(_l);
  assign_initial_edge(_l, _first, _last);
  _cycle = _first==_last;
  _found_next_edge = true;
  _v_stack.push_back(_last);
  if(!_cycle)
    _v_stack.push_back(_first);
  else
    touch(_l);
}
//------------------------------------------------------------------------
//:   check for bridges and compute winding angle.
//    (just convenient code packaging for use in classify_path)
//
static void classify_adjacent_edges(vtol_edge_2d_sptr& e0, vtol_edge_2d_sptr& e1,
                                    bool& all_bridge, float& angle)
{
  vtol_vertex_sptr cv = common_vertex(e0, e1);
  if(cv)
    {
      angle = angle_between_edges(e0, e1, cv);
      all_bridge =
        all_bridge&&used(e1)&&bridge_traverse(angle);
    }
}
//------------------------------------------------------------------------
//:  Classify two edges, it is simpler to deal with this case exhaustively.
//   Returns true if the path is cycle (not a bridge)
//
static bool classify_two_edge_path(vtol_edge_2d_sptr& e0, vtol_edge_2d_sptr& e1)
{
  vtol_vertex_sptr v1 = e0->v1(), v2 = e0->v1();
  float angle1 = angle_between_edges(e0, e1, v1);
  float angle2 = angle_between_edges(e0, e1, v2);
  bool bridge = bridge_traverse(angle1)&&bridge_traverse(angle2);
  return !bridge;
}
//---------------------------------------------------------------------
//: 
//   Search the set of vtol_edge(s) connected to the last path vertex for
//   a suitable addition to the path
//
vtol_edge_2d_sptr vtol_cycle_processor::search_for_next_edge(vcl_vector<vtol_edge_2d_sptr>& edges_at_last)
{
  while(edges_at_last.size())
    {
      vtol_edge_2d_sptr temp = ccw_edge(_l, _last, edges_at_last);
      if (!temp || assignable(temp, _last))
        return temp;
      edges_at_last.erase(vcl_find(edges_at_last.begin(), edges_at_last.end(),temp));
    }
  return NULL;
}
//---------------------------------------------------------------------
//:   A suitable vtol_edge is added to the evolving path
//
void vtol_cycle_processor::add_edge_to_path()
{
  vtol_vertex_sptr temp = _last;
  assign_ends(_next_edge, temp);
  _v_stack.push_back(_last);
  _last = temp;
  _l = _next_edge;
  _e_stack.push_back(_l);
  touch(_l);
}
//------------------------------------------------------------------------
//:   Classify a closed path as a cycle or bridge.
//    The path traverse is reversed since the vtol_edge sequence was
//    popped from the path stack.
//    Thus, the winding angle is opposite in sign, which is
//    accounted for in code.
bool vtol_cycle_processor::classify_path(vcl_vector<vtol_edge_2d_sptr>& path_edges,
                                    vtol_one_chain_sptr& chain)
{
  if(!path_edges.size())
    return false;
  vcl_vector<vtol_edge_sptr> c_edges;
  vtol_edge_2d_sptr e0 = *path_edges.begin();
  //If the path is a self_loop then the treatment is special
  //A self loop is classified as both a cw and ccw cycle
  if(self_loop(e0))
    {
      c_edges.push_back(e0->cast_to_edge());
      chain = new vtol_one_chain(c_edges, true);
      set_ccw(chain); set_cw(chain);
      return true;
    }
  //scan the path and determine if it is a bridge.  Also compute
  //the cumulative angle between vtol_edge(s) along the path
  float winding_angle = 0, angle = 0;
  bool all_bridge = used(e0);
  //If the path has two edges it is simpler to deal with it as follows
  if(path_edges.size()==2)
    if(classify_two_edge_path(e0, *(path_edges.begin()+1)))
      {
        c_edges.push_back(e0->cast_to_edge());
        chain = new vtol_one_chain(c_edges, true);
        set_ccw(chain); set_cw(chain);
        return true;
      }
  //the typical case, three or more edges
  for(vcl_vector<vtol_edge_2d_sptr>::iterator eit = path_edges.begin()+1;
      eit != path_edges.end(); eit++)
    {
      classify_adjacent_edges(e0, *eit, all_bridge, angle);
      winding_angle -= angle;//because pop_stacks reverses the traverse order
      e0 = *eit;
    }
  vtol_edge_2d_sptr eN = *path_edges.begin();//The closure of the loop
  classify_adjacent_edges(e0, eN, all_bridge, angle);
  winding_angle -= angle;
  //If the path is completely a bridge then nothing more is done
  if(all_bridge)
    return false;
  //Form a cycle from the path
  for(vcl_vector<vtol_edge_2d_sptr>::iterator eit = path_edges.begin();
      eit != path_edges.end(); eit++)
    c_edges.push_back((*eit)->cast_to_edge());

  chain = new vtol_one_chain(c_edges, true);
  //classify the cycle
  if(winding_angle>0)
    set_ccw(chain); //ccw chain (outer boundary)
  else
    set_cw(chain);//cw chain (hole boundary)
  return true;
}
//------------------------------------------------------------------------
//:   The main cycle tracing algorithm.
//    The input is _edges and the output is _chains, a set of 1-cycles.
//    The approach is to select an vtol_edge from _edges and explore all the
//    vtol_edge(s) connected to it.  The search proceeds by traversing connected
//    vtol_edge(s), turning in a cw or ccw direction depending on the initial vtol_edge
//    orientation.  If the search returns to a vertex already on the path,
//    then a cycle is output.  The traversed vtol_edge(s) and vertices are pushed
//    onto a stack so that cycles can be "popped" off an the search continued
//    from a proper state.  Each vtol_edge can be traversed in a plus or minus
//    direction (2 half_edges). Thus boundaries might be traced twice producing
//    identical cycles but traversed in opposite senses.
//
//    Bridges are detected by the fact that all vtol_edge(s) in the bridge are
//    used(plus and minus) and the traveral winding angle is 180 deg, i.e.,
//    the path folds exactly back on itself.
//
//    Cycles are labled according to the accumulated winding angle in
//    traversing the cycle.  If the accumulated angle is + then the
//    cycle is ccw, otherwise cw.  Here, the winding angle is defined as the
//    exterior angle between two sequential vtol_edge(s) in the traversed path.
//
//    In the traversal, completely unused vtol_edge(s) are favored to continue in
//    an unexplored path.  If none are available then the bool, force,
//    is set and the remaining half_edge is used, retracing a previous path
//    in the opposite direction.
void vtol_cycle_processor::compute_cycles()
{
  int limit = 10*_edges.size();//just to be guard against any infinite loop
  while(_edges.size()&&limit--)//should be removed when sure none can happen
    if(_found_next_edge&&!_cycle)
      {
        bool force = false;
        vcl_vector<vtol_edge_2d_sptr> edges_at_last;
        v_edges(_last, _edges, force, edges_at_last);
        _next_edge = search_for_next_edge(edges_at_last);
        if(!_next_edge&&!force)
          {
            force = true;
            v_edges(_last, _edges, force, edges_at_last);
            _next_edge = search_for_next_edge(edges_at_last);
          }
        if(!_next_edge)
          _found_next_edge = false;
        else
          add_edge_to_path();
        _cycle = *vcl_find(_v_stack.begin(),_v_stack.end(),_last);
        //        _cycle = stl_cool_topology::vcl_vector_vertex_find(_v_stack, _last);
      }
    else
      {
        if(_cycle)
          {
            _cycle = false;
            vcl_vector<vtol_edge_2d_sptr> cycle_edges;
            pop_stacks(_v_stack, _e_stack, _last, cycle_edges);
            assign_stack_edge(_e_stack, _l);
            vtol_one_chain_sptr cycle;
            bool is_cycle = classify_path(cycle_edges, cycle);
            if(is_cycle)
              _chains.push_back(cycle);
            else//path was all bridge edges
              for(vcl_vector<vtol_edge_2d_sptr>::iterator eit = cycle_edges.begin();
                  eit != cycle_edges.end(); eit++)
                cycle_edges.erase(eit);
          }
        if(!_found_next_edge)
          {
            //Get rid of edges touched in the search
            vcl_vector<vtol_edge_2d_sptr> removed_edges;
            for(vcl_vector<vtol_edge_2d_sptr>::iterator eit = _edges.begin();
                eit != _edges.end(); eit++)
              if(touched(*eit)&&used(*eit))
                removed_edges.push_back(*eit);

            for(vcl_vector<vtol_edge_2d_sptr>::iterator eit = removed_edges.begin();
                eit != removed_edges.end(); eit++)
              removed_edges.erase(eit);

            this->set_bridge_vars();
          }
      }
  if(!limit)
    vcl_cout << "Limit Exhaused in vtol_cycle_processor::compute_cycles(..) " << vcl_endl;
}

//-----------------------------------------------------------------
//: 
//    The input is a set of 1-cycles in _chains.  These cycles are
//    sorted so that they form a proper containment relation.  That
//    is, there is one outer cycle, with traversal in the ccw direction
//    and zero or more interior hole boundaries with traversal in
//    the cw direction. All other cycles are removed.  The ouput is
//    _nested_one_cycles.
void vtol_cycle_processor::sort_one_cycles()
{
  if(!_chains.size())
    {
      vcl_cout << "In vtol_cycle_processor:: sort_one_cycles(..) no cycles "
           << vcl_endl;
      return;
    }
  _nested_one_cycles.clear();
  //First, find the outer bounding vtol_one_chain. This outer boundary is
  //defined as a ccw cycle with the largest bounding box.
  //search for the largest ccw bounding box
  vcl_vector<vtol_one_chain_sptr>::iterator cit = _chains.begin();
  float area = 0;
  vtol_one_chain_sptr outer_chain;
  for(cit=_chains.begin(); cit != _chains.end(); cit++)
    {
      untouch(*cit);
      if(!ccw(*cit))
        continue;
      vsol_box_3d* box = (*cit)->get_bounding_box();
      float WxH = box->width()*box->height();
      if(WxH>area)
        {
          area = WxH;
          outer_chain = *cit;
        }
    }

  if(!outer_chain||!ccw(outer_chain))
    {
      vcl_cout << " In vtol_cycle_processor::sort_one_cycles(..) "
           << " Shouldn't happen that there is no outer chain"
           << vcl_endl
           << "N cycles = " << _chains.size() << vcl_endl;
      for(vcl_vector<vtol_one_chain_sptr>::iterator cit = _chains.begin();
          cit != _chains.end(); cit++)
        {
          vcl_cout << " is chain ccw?  " << ccw(*cit) << vcl_endl;
          vcl_cout << " is chain cw?  " << cw(*cit) << vcl_endl;
        }
      vcl_cout << "Outer Chain " << outer_chain << vcl_endl;
      return;
    }
  _nested_one_cycles.push_back(outer_chain);
  touch(outer_chain);
  //At this point, we have the outer bounding chain.
  //next we will include any cw cycles that lie inside the
  //outer_chain.  We exclude any cycle with the same bounding
  //box as the outer cycle.  This condition can occur since the outer
  //boundary is mostly traced twice, once ccw and once cw when there is
  //an included loop in the outer boundary.  The boundary vertices will
  //be identical and thus the bounding box will be the same.
  //
  // - one caveat is that the equality test below is exact.
  //   some situations may require a tolerance
  vsol_box_3d* b = outer_chain->get_bounding_box();
  for(cit = _chains.begin(); cit != _chains.end(); cit++)
      if(cw(*cit)&&!touched(*cit))
        {
          if((vtol_one_chain_sptr)(*cit)==(vtol_one_chain_sptr)outer_chain)
            continue;
          vsol_box_3d* bc = (*cit)->get_bounding_box();
          if((*bc<*b)&&!bc->near_equal(*b, _tolerance))
            {
              _nested_one_cycles.push_back(*cit);
              touch(*cit);
            }
        }
}


void vtol_cycle_processor::process()
{
  this->compute_cycles();
  this->sort_one_cycles();
  _valid = true;
}

bool vtol_cycle_processor::nested_one_cycles(vcl_vector<vtol_one_chain_sptr>& one_chains,
                                        const float& tolerance)
{
  if(!_valid||tolerance!=_tolerance)
    {
      _tolerance = tolerance;
      process();
    }
  one_chains = _nested_one_cycles;
  return true; //later return error state
}
