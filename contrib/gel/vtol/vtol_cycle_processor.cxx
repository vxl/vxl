// This is gel/vtol/vtol_cycle_processor.cxx
#include "vtol_cycle_processor.h"
//:
// \file

#include <vcl_vector.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vnl/vnl_math.h>

#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_edgel_chain.h>

#include <vtol/vtol_vertex.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_one_chain.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_edge_2d_sptr.h>

//Constructors
vtol_cycle_processor::vtol_cycle_processor(vcl_vector<vtol_edge_2d_sptr>& edges,
                                           bool debug1, bool debug2)
{
  debug1_ = debug1;
  debug2_ = debug2;
  tolerance_ = 0;
  init(edges);
}

//: a more convenient interface
//
static void edge_2d_erase(vcl_vector<vtol_edge_2d_sptr>& edges,
                          vtol_edge_2d_sptr& e)
{
  vcl_vector<vtol_edge_2d_sptr>::iterator eit =
    vcl_find(edges.begin(), edges.end(), e);
  if (eit == edges.end())
    return;
  edges.erase(eit);
  return;
}

//: print a vector of vertices
//
static void print_vertices(vcl_vector<vtol_vertex_sptr>& verts)
{
  for (vcl_vector<vtol_vertex_sptr>::iterator vit = verts.begin();
       vit != verts.end(); vit++)
    vcl_cout << *vit << '('
             << (*vit)->cast_to_vertex_2d()->x()<< ' '
             << (*vit)->cast_to_vertex_2d()->y()<< ")\n\n";
}

//: print a vector of edges_2d
//
static void print_edges(vcl_vector<vtol_edge_2d_sptr>& edges)
{
  for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges.begin();
       eit != edges.end(); eit++)
    vcl_cout << *eit << '('
             << (*eit)->v1()->cast_to_vertex_2d()->x()<< ' '
             << (*eit)->v1()->cast_to_vertex_2d()->y()<< " |"
             << (*eit)->v2()->cast_to_vertex_2d()->x()<< ' '
             << (*eit)->v2()->cast_to_vertex_2d()->y()<< ")\n\n";
}

//: print a vector of edges
//
static void print_edges(vcl_vector<vtol_edge_sptr>& edges)
{
  for (vcl_vector<vtol_edge_sptr>::iterator eit = edges.begin();
       eit != edges.end(); eit++)
    vcl_cout << *eit << '('
             << (*eit)->v1()->cast_to_vertex_2d()->x()<< ' '
             << (*eit)->v1()->cast_to_vertex_2d()->y()<< " |"
             << (*eit)->v2()->cast_to_vertex_2d()->x()<< ' '
             << (*eit)->v2()->cast_to_vertex_2d()->y()<< ")\n\n";
}

//---------------------------------------------------------------
//
static void pop_stacks(vertex_list& verts,
                       vcl_vector<vtol_edge_2d_sptr>& edges,
                       vtol_vertex_sptr& v,
                       vcl_vector<vtol_edge_2d_sptr>& cycle_edges)
{
  bool found = false;
  while (verts.size()&&edges.size()&&!found)
    {
      found = verts.back()==v;
      cycle_edges.push_back(edges.back());
      verts.pop_back();
      edges.pop_back();
    }
  if (!edges.size()&&verts.size())
    verts.clear();
}

//: Access to flags
//the user flags on SpatialObject are used to define the orientation
//of vtol_edge(s) during the boundary tracing process.  In effect, FLAG1 and
//FLAG2 define half edges. vtol_edges are used up when both half edges are used.
static bool used(vtol_edge_2d_sptr& e)
{
  return e->get_user_flag(VSOL_FLAG1)&&e->get_user_flag(VSOL_FLAG2);
}

static bool unused(vtol_edge_2d_sptr& e)
{
  return !e->get_user_flag(VSOL_FLAG1)&&!e->get_user_flag(VSOL_FLAG2);
}

static bool plus_used(vtol_edge_2d_sptr& e)
{
  return e->get_user_flag(VSOL_FLAG1) != 0;
}

static bool minus_used(vtol_edge_2d_sptr& e)
{
  return e->get_user_flag(VSOL_FLAG2) != 0;
}

static bool half_used(vtol_edge_2d_sptr& e)
{
  return e->get_user_flag(VSOL_FLAG1) ^ e->get_user_flag(VSOL_FLAG2);
  // exclusive OR; was: return (dir1&&!dir2)||(!dir1&&dir2);
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
  return ch->get_user_flag(VSOL_FLAG1) != 0;
}

static bool cw(vtol_one_chain_sptr& ch)
{
  return ch->get_user_flag(VSOL_FLAG2) != 0;
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
  return e->get_user_flag(VSOL_FLAG3) != 0;
}

static bool self_loop(vtol_edge_2d_sptr& e)
{
  vtol_vertex_sptr v1 = e->v1();
  vtol_vertex_sptr v2 = e->v2();
  bool loop = v1 == v2;
  return loop;
}

static bool bridge_traverse(double angle)
{
  double tol = 1e-3;
  double delta = vcl_fabs(vcl_fabs(angle)-180);
  return delta<tol;
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
  return ch->get_user_flag(VSOL_FLAG3) != 0;
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
  edge_list *edges = v->edges();
  for (edge_list::iterator eit = edges->begin(); eit != edges->end(); eit++)
    {
      vtol_edge_2d_sptr e = (*eit)->cast_to_edge_2d();
      if (vcl_find(b_edges.begin(), b_edges.end(),e) != b_edges.end())
        {
          if (used(e))
            continue;
          if (unused(e))
            edges_at_vertex.push_back(e);
          if (half_used(e)&&force)
            edges_at_vertex.push_back(e);
        }
    }
  delete edges;
}

static double flip_y(double ang)
{
  double rad_per_deg = vnl_math::pi/180.0;
  double rang = rad_per_deg*ang;
  double cs = vcl_cos(rang), si = -vcl_sin(rang);//here is where y is flipped
  ang = vcl_atan2(si, cs)/rad_per_deg;
  if (ang<0)
    ang+=360;
  return ang;
}

static double tangent_angle_at_vertex(vtol_vertex_sptr v, vtol_edge_2d_sptr e)
{
  double ang = 0;
  if (!e||!v||!(v==e->v1()||v==e->v2()))
    {
      vcl_cout << "vtol_vertex and vtol_edge not incident\n";
      return ang;
    }
  //Here we assume that the edgel_chain starts at v1 and ends at v2;
  if (v==e->v1())
    {
    ang = e->curve()->cast_to_digital_curve()->
                        get_interpolator()->get_tangent_angle(0);
    }
  else
    {
      int N = e->curve()->cast_to_digital_curve()->
                            get_interpolator()->get_edgel_chain()->size();

      ang = e->curve()->cast_to_digital_curve()->
                          get_interpolator()->get_tangent_angle(N-1);
      //reverse the angle since we are at the end rather than the start of the edge?
      ang += 180.0;
      if (ang>360)
        ang -= 360;
    }
  //If we want cw and ccw to be correct senses, we flip y because the input
  //edges are in image coordinates that has a left-handed coordinate system.
  ang = flip_y(ang);
  return ang;
}

//----------------------------------------------------------------
//:   Find the vtol_vertex, if any,  which is shared by two vtol_edge(s)
static vtol_vertex_sptr common_vertex(vtol_edge_2d_sptr& e0, vtol_edge_2d_sptr& e1)
{
  vtol_vertex_sptr v01 = e0->v1(), v02 = e0->v2();
  vtol_vertex_sptr v11 = e1->v1(), v12 = e1->v2();
  if ((vtol_vertex_sptr)v01==(vtol_vertex_sptr)v11)
    return v01;
  if ((vtol_vertex_sptr)v01==(vtol_vertex_sptr)v12)
    return v01;
  if ((vtol_vertex_sptr)v02==(vtol_vertex_sptr)v11)
    return v02;
  if ((vtol_vertex_sptr)v02==(vtol_vertex_sptr)v12)
    return v02;
  return NULL;
}

//----------------------------------------------------------------
//:   Compute the angle between two edges at the specified vtol_vertex, v
//    The angle is mapped to the interval [-180, 180].  The angle sense is
//    defined so that the e0 orientation is towards v and the e1
//    orientation is away from v.
double vtol_cycle_processor::angle_between_edges(vtol_edge_2d_sptr e0,
                                                 vtol_edge_2d_sptr e1,
                                                 vtol_vertex_sptr v)
{
  double theta0 = 180+tangent_angle_at_vertex(v, e0);
  if (theta0>360)
    theta0 -= 360;
  double theta1 = tangent_angle_at_vertex(v, e1);
  double angle = theta1-theta0;
  if (angle>180)
    angle-=360;
  if (angle<-180)
    angle+=360;
  return angle;
}

//------------------------------------------------------------
//:   Find the most counter clockwise vtol_edge at the input vtol_vertex, from.
//
static vtol_edge_2d_sptr ccw_edge(vtol_edge_2d_sptr in_edg, vtol_vertex_sptr from,
                                  vcl_vector<vtol_edge_2d_sptr>& edges)
{
  double most_ccw = -360;
  vtol_edge_2d_sptr ccw = NULL;
  for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges.begin();
       eit != edges.end(); eit++)
    {
      if ((*eit)==in_edg)
        continue;
      double delta = vtol_cycle_processor::angle_between_edges(in_edg, *eit, from);
      if (delta>most_ccw)
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
//    (V1 to V2) has occurred then dir1 is true.  A second traversal
//    is not allowed and the edge is considered un-assignable.
//
bool vtol_cycle_processor::assignable(vtol_edge_2d_sptr edg, vtol_vertex_sptr last)
{
  if (debug2_)
    {
      vcl_cout << "In assignable(..)\n"
               << "edg " ; this->print_edge(edg);
      vcl_cout << "plus used(" << plus_used(edg) << ") minus used("
               << minus_used(edg) << ")\n"
               << "last " << *last << vcl_endl;
    }
  if (!(edg&&last))
    return false;
  if (unused(edg))
    return true;
  if (used(edg))
    return false;
  vtol_vertex_sptr s = edg->v1();
  vtol_vertex_sptr e = edg->v2();
  if (last==s&&!plus_used(edg))
    return true;
  if (last==e&&!minus_used(edg))
    return true;
  return false;
}

//----------------------------------------------------------------------
//:   Set up the first edge in a cycle (or bridge) traversal.
//    A positive
//    traversal (half edge) is in the direction from v1->v2.
//    Self loops are a special case and use both directions on one traversal.
//    There is no point in traversing the self loop twice.
void vtol_cycle_processor::assign_initial_edge(vtol_edge_2d_sptr& e,
                                               vtol_vertex_sptr& first,
                                               vtol_vertex_sptr& last)
{
  if (debug1_)
      vcl_cout << "==== entering assign_initial_edge =====\n"
               << "e " << *e << "plus used(" << plus_used(e) << ") minus used("
               << minus_used(e) << ")\n";

  if (used(e))
    {
      vcl_cout << "In vtol_cycle_processor::assign_intial_edge(..) "
               << "shouldn't happen - error\n";
      return;
    }
  vtol_vertex_sptr v1 = e->v1(), v2 = e->v2();
  if (v1==v2)
    {
      use_plus(e);
      use_minus(e);
      first = v1; last = v1;
    }
  if (plus_used(e))
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
  if (debug1_)
      vcl_cout << "==== leaving assign_initial_edge =====\n"
               << "plus used(" << plus_used(e) << ") minus used("
               << minus_used(e) << ")\n\n";
}

//------------------------------------------------------------
//:   Link the vtol_edge, "edg" to the vtol_vertex, "last".
//    Set the appropriate direction flag

void vtol_cycle_processor::assign_ends(vtol_edge_2d_sptr edg, vtol_vertex_sptr& last)
{
  if (debug1_)
      vcl_cout << "==== entering assign_ends =====\n"
               << "edg " << *edg << "plus used(" << plus_used(edg) << ") minus used("
               << minus_used(edg) << ")\n";
  vtol_vertex_sptr s = edg->v1();
  vtol_vertex_sptr e = edg->v2();
  // compare to last point added
  // Here we need to be able to use the previous
  // edge if there is no other choice
  if (last == s)
    {
      last = e;
      use_plus(edg);//Forward direction
      if (s==e)
        use_minus(edg);//For a self-loop, any traversal uses it up
      return;
    }
  if (last == e)
    {
      last = s;
      use_minus(edg);//Reverse direction
      if (s==e)
        use_plus(edg);//For a self-loop, any traversal uses it up
      return;
    }
  vcl_cout << "In vtol_cycle_processor::assign ends(..) - shouldn't happen\n";
}

//------------------------------------------------------------
//:
//    Assign the next edge to the top of the edge stack when
//    a cycle is popped off the stack. That is, start the new path
//    with the edge at the top of the stack.  If the stack is
//    empty, then the last assignment to l_ is used.
static void assign_stack_edge(vcl_vector<vtol_edge_2d_sptr>& e_stack, vtol_edge_2d_sptr& next_edge)
{
  if (!e_stack.size())
    return;
  next_edge = e_stack.back();
}

//------------------------------------------------------------------
//:   The initial setup of the cycle process.
//    Used by the vtol_cycle_processor
//    constructors to establish the start conditions
void vtol_cycle_processor::init(vcl_vector<vtol_edge_2d_sptr>& in_edges)
{
  edges_.clear();
  chains_.clear();
  nested_one_cycles_.clear();
  for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = in_edges.begin();
       eit != in_edges.end(); eit++)
    {
      clear(*eit);
      edges_.push_back(*eit);
    }
  this->set_bridge_vars();
  valid_ = false;
}

//---------------------------------------------------------------
//:  Initializes the search for cycles starting with an unexplored vtol_edge.
//   This initialization is called after a connected set of vtol_edge(s) is
//   completely explored and removed from edges_.
void vtol_cycle_processor::set_bridge_vars()
{
  if (!edges_.size())
    return;
  v_stack_.clear();
  e_stack_.clear();
  l_ = edges_[0];
  e_stack_.push_back(l_);
  assign_initial_edge(l_, first_, last_);
  cycle_ = first_==last_;
  found_next_edge_ = true;
  v_stack_.push_back(last_);
  if (!cycle_)
    v_stack_.push_back(first_);//why do we put both first and last on the stack?
  else
    touch(l_);
  if (debug1_)
    {
      vcl_cout << "------init bridge vars-------\n"
               << "oooooooo Vertex Stack ooooooooo\n";
      print_vertices(v_stack_);
      vcl_cout << "oooooooo Edge Stack ooooooooo\n";
      print_edges(e_stack_);
    }
}

//------------------------------------------------------------------------
//:   check for bridges and compute winding angle.
//    (just convenient code packaging for use in classify_path)
//
static void classify_adjacent_edges(vtol_edge_2d_sptr& e0, vtol_edge_2d_sptr& e1,
                                    bool& all_bridge, double& angle)
{
  vtol_vertex_sptr cv = common_vertex(e0, e1);
  if (cv)
    {
      angle = vtol_cycle_processor::angle_between_edges(e0, e1, cv);
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
  double angle1 = vtol_cycle_processor::angle_between_edges(e0, e1, v1);
  double angle2 = vtol_cycle_processor::angle_between_edges(e0, e1, v2);
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
  while (edges_at_last.size())
    {
      vtol_edge_2d_sptr temp = ccw_edge(l_, last_, edges_at_last);
      if (debug2_)
        {
          vcl_cout << "next ccw_edge\n";
          this->print_edge(temp);
        }
      if (!temp || assignable(temp, last_))
        return temp;
      edge_2d_erase(edges_at_last, temp);
    }
  return NULL;
}

//---------------------------------------------------------------------
//:   A suitable vtol_edge is added to the evolving path
//
void vtol_cycle_processor::add_edge_to_path()
{
  vtol_vertex_sptr temp = last_;
  assign_ends(next_edge_, temp);
  if (debug2_)
      vcl_cout << "==== after assign_ends =====\n"
               << "next_edge_ "  << *next_edge_ << "plus used("
               << plus_used(next_edge_) << ") minus used("
               << minus_used(next_edge_) << ")\n\n";
  v_stack_.push_back(last_);
  last_ = temp;
  l_ = next_edge_;
  e_stack_.push_back(l_);
  touch(l_);
  if (debug1_)
    {
      vcl_cout << "------assign_edge_to_path-------\n"
               << "oooooooo Vertex Stack ooooooooo\n";
      print_vertices(v_stack_);
      vcl_cout << "oooooooo Edge Stack ooooooooo\n";
      print_edges(e_stack_);
    }
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
  if (debug1_)
        vcl_cout << "======= In classify_path ========\n";
  if (!path_edges.size())
    return false;
  edge_list c_edges;
  vtol_edge_2d_sptr e0 = *path_edges.begin();
  //If the path is a self_loop then the treatment is special
  //A self loop is classified as both a cw and ccw cycle
  if (self_loop(e0))
    {
      c_edges.push_back(e0->cast_to_edge());
      chain = new vtol_one_chain(c_edges, true);
      set_ccw(chain); set_cw(chain);
      return true;
    }
  //scan the path and determine if it is a bridge.  Also compute
  //the cumulative angle between vtol_edge(s) along the path
  double winding_angle = 0, angle = 0;
  bool all_bridge = used(e0);
  //If the path has two edges it is simpler to deal with it as follows
  //JLM why don't we mark the edges as all used?
  if (path_edges.size()==2)
    if (classify_two_edge_path(e0, *(path_edges.begin()+1)))
      {
        c_edges.push_back(e0->cast_to_edge());
        chain = new vtol_one_chain(c_edges, true);
        set_ccw(chain); set_cw(chain);
        return true;
      }
  //the typical case, three or more edges
  for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = path_edges.begin()+1;
       eit != path_edges.end(); eit++)
    {
      classify_adjacent_edges(e0, *eit, all_bridge, angle);

      if (debug1_)
        vcl_cout << "wind_ang " << winding_angle << " - " << angle << " = "
                 << winding_angle - angle << vcl_endl;

      winding_angle -= angle;//because pop_stacks reverses the traverse order

      e0 = *eit;
    }
  vtol_edge_2d_sptr eN = *path_edges.begin();//The closure of the loop
  classify_adjacent_edges(e0, eN, all_bridge, angle);
  winding_angle -= angle;
  //If the path is completely a bridge then nothing more is done
  if (all_bridge)
    return false;
  //Form a cycle from the path
  for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = path_edges.begin();
       eit != path_edges.end(); eit++)
    c_edges.push_back((*eit)->cast_to_edge());

  chain = new vtol_one_chain(c_edges, true);
  //classify the cycle
  if (winding_angle>0)
    set_ccw(chain); //ccw chain (outer boundary)
  else
    set_cw(chain);//cw chain (hole boundary)
  return true;
}

void vtol_cycle_processor::print_edge(vtol_edge_2d_sptr& e)
{
  if (!e)
    return;
  vcl_cout << e << " :[(" << e->v1()->cast_to_vertex_2d()->x()
           << ' ' << e->v1()->cast_to_vertex_2d()->y()<< "), ("
           << e->v2()->cast_to_vertex_2d()->x() << ' '
           << e->v2()->cast_to_vertex_2d()->y() << ")]\n";
}

//------------------------------------------------------------------------
//: The main cycle tracing algorithm.
//  The input is edges_ and the output is chains_, a set of 1-cycles.
//  The approach is to select an vtol_edge from edges_ and explore all the
//  vtol_edge(s) connected to it.  The search proceeds by traversing connected
//  vtol_edge(s), turning in a cw or ccw direction depending on the initial vtol_edge
//  orientation.  If the search returns to a vertex already on the path,
//  then a cycle is output.  The traversed vtol_edge(s) and vertices are pushed
//  onto a stack so that cycles can be "popped" off an the search continued
//  from a proper state.  Each vtol_edge can be traversed in a plus or minus
//  direction (2 half_edges). Thus boundaries might be traced twice producing
//  identical cycles but traversed in opposite senses.
//
//  Bridges are detected by the fact that all vtol_edge(s) in the bridge are
//  used(plus and minus) and the traversal winding angle is 180 deg, i.e.,
//  the path folds exactly back on itself.
//
//  Cycles are labeled according to the accumulated winding angle in
//  traversing the cycle.  If the accumulated angle is + then the
//  cycle is ccw, otherwise cw.  Here, the winding angle is defined as the
//  exterior angle between two sequential vtol_edge(s) in the traversed path.
//
//  In the traversal, completely unused vtol_edge(s) are favored to continue in
//  an unexplored path.  If none are available then the bool, force,
//  is set and the remaining half_edge is used, retracing a previous path
//  in the opposite direction.
void vtol_cycle_processor::compute_cycles()
{
  int limit = 10*edges_.size();//just to be guard against any infinite loop
  while (edges_.size()&&limit--)//should be removed when sure none can happen
    if (found_next_edge_&&!cycle_)
      {
        bool force = false;

        if (debug1_&&last_){
          vcl_cout << "last_ ="; last_->print(); vcl_cout <<vcl_endl;}

        vcl_vector<vtol_edge_2d_sptr> edges_at_last;
        v_edges(last_, edges_, force, edges_at_last);
        next_edge_ = search_for_next_edge(edges_at_last);

        if (!next_edge_&&!force)
          {
            force = true;
            v_edges(last_, edges_, force, edges_at_last);
            next_edge_ = search_for_next_edge(edges_at_last);
          }
        if (debug1_&&next_edge_){
          vcl_cout << "next_edge_("<< force <<") = "; this->print_edge(next_edge_);}

        if (!next_edge_)
          found_next_edge_ = false;
        else
          add_edge_to_path();
        if (debug1_)
            vcl_cout << "========checking for cycle ===========\n"
                     << " last_ " << last_ << '('
                     << last_->cast_to_vertex_2d()->x()<< ' '
                     << last_->cast_to_vertex_2d()->y()<< ")\n";
        cycle_ = vcl_find(v_stack_.begin(), v_stack_.end(), last_) != v_stack_.end();
        if (debug1_&&cycle_)
          vcl_cout << "...Found Cycle....\n\n";
      }
    else
      {
        if (cycle_)
          {
            cycle_ = false;
            vcl_vector<vtol_edge_2d_sptr> cycle_edges;
            pop_stacks(v_stack_, e_stack_, last_, cycle_edges);
            if (debug1_)
              {
                vcl_cout << "======== In Cycle Classifer =======\n"
                         << "cycle_edges\n";
                print_edges(cycle_edges);
              }
            assign_stack_edge(e_stack_, l_);
            vtol_one_chain_sptr cycle;
            bool is_cycle = classify_path(cycle_edges, cycle);
            if (debug1_)
              {
                vcl_cout << "is_cycle(" << is_cycle << ")\n";
                if (cycle)
                  {
                    vcl_cout << "cycle " << cycle << "[cw(" << cw(cycle)
                             << "), ccw(" << ccw(cycle) << ")]\n";
                    vcl_vector<vtol_edge_sptr>* c_edges = cycle->edges();
                    vcl_cout << "cycle edges\n";
                    print_edges(*c_edges);
                    delete c_edges;
                  }
              }
            if (is_cycle)
              chains_.push_back(cycle);
            else//path was all bridge edges, so remove them from consideration
              for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = cycle_edges.begin();
                   eit != cycle_edges.end(); eit++)
                edge_2d_erase(edges_,*eit);
          }
        if (!found_next_edge_)
          {
            //Get rid of edges touched in the search
            vcl_vector<vtol_edge_2d_sptr> removed_edges;
            for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges_.begin();
                 eit != edges_.end(); eit++)
              if (touched(*eit)&&used(*eit))
                removed_edges.push_back(*eit);
            for (vcl_vector<vtol_edge_2d_sptr>::iterator
                 eit = removed_edges.begin(); eit != removed_edges.end();
                 ++eit)
              edge_2d_erase(edges_,*eit);

            this->set_bridge_vars();
          }
      }
  if (!limit)
    vcl_cout << "Limit Exhaused in vtol_cycle_processor::compute_cycles(..)\n";
}

//-----------------------------------------------------------------
//:
//    The input is a set of 1-cycles in chains_.  These cycles are
//    sorted so that they form a proper containment relation.  That
//    is, there is one outer cycle, with traversal in the ccw direction
//    and zero or more interior hole boundaries with traversal in
//    the cw direction. All other cycles are removed.  The output is
//    nested_one_cycles_.
void vtol_cycle_processor::sort_one_cycles()
{
  if (!chains_.size())
    {
      vcl_cout << "In vtol_cycle_processor:: sort_one_cycles(..) no cycles\n";
      return;
    }
  nested_one_cycles_.clear();
  //First, find the outer bounding vtol_one_chain. This outer boundary is
  //defined as a ccw cycle with the largest bounding box.
  //search for the largest ccw bounding box
  double area = 0;
  vtol_one_chain_sptr outer_chain = 0;
  for (one_chain_list::iterator cit=chains_.begin(); cit!=chains_.end(); cit++)
    {
      untouch(*cit);
      if (!ccw(*cit))
        continue;
      vsol_box_2d_sptr box = (*cit)->get_bounding_box();
      double WxH = box->width()*box->height();
      if (WxH>area)
        {
          area = WxH;
          outer_chain = *cit;
        }
    }

  if (!outer_chain||!ccw(outer_chain))
    {
      vcl_cout << " In vtol_cycle_processor::sort_one_cycles(..)"
               << " Shouldn't happen that there is no outer chain\n"
               << "N cycles = " << chains_.size() << vcl_endl;
      for (one_chain_list::iterator cit = chains_.begin();
           cit != chains_.end(); cit++)
        {
          vcl_cout << " is chain ccw?  " << ccw(*cit) << vcl_endl
                   << " is chain cw?  " << cw(*cit) << vcl_endl;
        }
      vcl_cout << "Outer Chain " << outer_chain << vcl_endl;
      return;
    }
  nested_one_cycles_.push_back(outer_chain);
  touch(outer_chain);
  if (debug1_)
    vcl_cout << "Outer Cycle area = " << area << vcl_endl;
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
  vsol_box_2d_sptr b = outer_chain->get_bounding_box();
  for (one_chain_list::iterator cit=chains_.begin(); cit!=chains_.end(); cit++)
    if (cw(*cit)&&!touched(*cit))
    {
      if ((*cit)==outer_chain)
        continue;
      vsol_box_2d_sptr bc = (*cit)->get_bounding_box();
      if ((*bc<*b)&&!bc->near_equal(*b, tolerance_))
      {
        vsol_box_2d_sptr bc = (*cit)->get_bounding_box();
        if ((*bc<*b)&&!bc->near_equal(*b, tolerance_))
          {
            if (debug1_)
              vcl_cout << "Adding inner cycle with area = "
                       << bc->width()*bc->height() << vcl_endl;

            nested_one_cycles_.push_back(*cit);
            touch(*cit);
          }
      }
    }
}

void vtol_cycle_processor::process()
{
  this->compute_cycles();
  this->sort_one_cycles();
  valid_ = true;
}

bool vtol_cycle_processor::nested_one_cycles(one_chain_list& one_chains,
                                             const float& tolerance)
{
  if (!valid_||tolerance!=tolerance_)
    {
      tolerance_ = tolerance;
      process();
    }
  one_chains = nested_one_cycles_;
  return true; //later return error state
}

//: flag mutation functions
static void clear_flags(vcl_vector<vtol_edge_sptr>& s, unsigned int flag)
{
  for (vcl_vector<vtol_edge_sptr>::iterator eit = s.begin();
       eit != s.end(); eit++)
    (*eit)->unset_user_flag(flag);
}

static void set_flags(vcl_vector<vtol_edge_sptr>& s, unsigned int flag)
{
  for (vcl_vector<vtol_edge_sptr>::iterator eit = s.begin();
       eit != s.end(); eit++)
    (*eit)->set_user_flag(flag);
}

//---------------------------------------------------------------------------
//: This method scans the edge sets s1, s2 and computes their set intersection.
// If the intersection is empty then the method returns false.
// The method uses flags to mark edges appearing in both lists. Thus the
// computation is O(n1+n2).
//
// This method might not be needed if stl algorithms worked (small_g++ probs)
// however with flags this might be faster than stl
//
bool vtol_cycle_processor::intersect_edges(vcl_vector<vtol_edge_sptr>& s1,
                                           vcl_vector<vtol_edge_sptr>& s2,
                                           vcl_vector<vtol_edge_sptr>& s1_and_s2)
{
  s1_and_s2.clear();
  //If either set is empty then the result is empty
  if (!(s1.size()&&s2.size()))
    return false;
  //Get Flags
  unsigned int flag1 = VSOL_FLAG5, flag2 = VSOL_FLAG6;
  //Scan through s2 and clear the flags
  clear_flags(s2, flag1);
  clear_flags(s2, flag2);

  //Scan through s1 and set flag 1 which is used to indicate
  //that an edge is in s1.
  set_flags(s1, flag1);

  //Scan s2 again and push edges also in s1  onto the set intersection
  //mark the edge as in the output list with flag2.
  for (vcl_vector<vtol_edge_sptr>::iterator eit = s2.begin();
       eit != s2.end(); eit++)
    {
      vtol_edge_sptr e = *eit;
      if (e->get_user_flag(flag1)&&!e->get_user_flag(flag2))
        {
          e->set_user_flag(flag2);//mark the edge as in the output
          s1_and_s2.push_back(e);
        }
    }
  //clean up the flags
  clear_flags(s1, flag1);
  clear_flags(s2, flag1);
  clear_flags(s1, flag2);
  clear_flags(s2, flag2);
  return s1_and_s2.size()>0;
}

//---------------------------------------------------------------------------
//: This method scans the edge sets s1, s2 and computes their set difference.
// i.e, s1 with any edges also in s2 removed. If the difference
// is empty then the method returns false. The method uses flags to mark
// edges appearing in both lists. Thus the computation is O(n1+n2).
//
// This method might not be needed if stl algorithms worked (small_g++ probs)
// however with flags this might be faster than stl
//
bool vtol_cycle_processor::difference_edges(vcl_vector<vtol_edge_sptr>& s1,
                                            vcl_vector<vtol_edge_sptr>& s2,
                                            vcl_vector<vtol_edge_sptr>& s1_minus_s2)
{
  s1_minus_s2.clear();
  //If either set is empty then the result is empty
  if (!(s1.size()&&s2.size()))
    return false;
  //Get Flags
  unsigned int flag1 = VSOL_FLAG5, flag2 = VSOL_FLAG6;
  //Scan through s1 and clear the flags
  clear_flags(s1, flag1);
  clear_flags(s1, flag2);

  //Scan through s2 and set flag1 which marks that it is in s2.
  set_flags(s2, flag1);

  //Scan s1 again and push edges exclusively in s1 onto the output
  //mark the edge as in the output list with flag2.
  for (vcl_vector<vtol_edge_sptr>::iterator eit = s1.begin();
       eit != s1.end(); eit++)
    {
      vtol_edge_sptr e = *eit;
      if (!e->get_user_flag(flag1)&&!e->get_user_flag(flag2))
        {
          e->set_user_flag(flag2);//mark the edge as in the output
          s1_minus_s2.push_back(e);
        }
    }
  //Clean up the flags
  clear_flags(s1, flag1);
  clear_flags(s2, flag1);
  clear_flags(s1, flag2);
  clear_flags(s2, flag2);
  return s1_minus_s2.size()>0;
}

//--------------------------------------------------------------------
//: mark all vertices as used if they are incident on exactly two edges.
// Vertices that are not incident two edges are output in the vector, bad_verts.
bool vtol_cycle_processor::corrupt_boundary(vcl_vector<vtol_edge_2d_sptr>& edges,
                                            vcl_vector<vtol_vertex_sptr>& bad_verts)
{
  bool bad = false;
  //Initialize Markers
  if (! edges.front())
    {
      vcl_cout << "In cycle_processor::corrupt_boundary - null edge\n";
      return false;
    }
  //setup vertex flags
  unsigned int f1=VSOL_FLAG4, f2=VSOL_FLAG5, f3=VSOL_FLAG6;
  //Initialize Flags
  for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges.begin();
       eit != edges.end(); eit++)
  {
    vtol_vertex_sptr v1 = (*eit)->v1();
    vtol_vertex_sptr v2 = (*eit)->v2();
    v1->unset_user_flag(f1);
    v1->unset_user_flag(f2);
    v1->unset_user_flag(f3);
    v2->unset_user_flag(f1);
    v2->unset_user_flag(f2);
    v2->unset_user_flag(f3);
  }
  //Mark using flags that a vertex is incident on two edges
  //Flags f1 and f2 act as a counter
  for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges.begin();
       eit != edges.end(); eit++)
  {
    vtol_vertex_sptr v1 = (*eit)->v1();
    vtol_vertex_sptr v2 = (*eit)->v2();
    if (!v1->get_user_flag(f1))
      v1->set_user_flag(f1);
    else
      v1->set_user_flag(f2);
    if (!v2->get_user_flag(f1))
      v2->set_user_flag(f1);
    else
      v2->set_user_flag(f2);
  }
  for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges.begin();
       eit != edges.end(); eit++)
  {
    vtol_vertex_sptr v1 = (*eit)->v1();
    vtol_vertex_sptr v2 = (*eit)->v2();
    if ((v1!=v2)&&*v1==*v2)
      vcl_cout << "Improper Loop(\n" << *v1 << *v2 << ")\n\n";
    bool bad1 = !v1->get_user_flag(f2);
    bool bad2 = !v2->get_user_flag(f2);
    // flag f3 keeps track that we have already put the vertex onto the bad list
    if (bad1)
      {
        if (!v1->get_user_flag(f3))
          {
            bad_verts.push_back(v1);
            v1->set_user_flag(f3);
          }
        bad = true;
      }
    if (bad2)
      {
        if (!v2->get_user_flag(f3))
          {
            bad_verts.push_back(v2);
            v2->set_user_flag(f3);
          }
        bad = true;
      }
  }
  //release the flags
  for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges.begin();
       eit != edges.end(); eit++)
  {
    vtol_vertex_sptr v1 = (*eit)->v1();
    vtol_vertex_sptr v2 = (*eit)->v2();
    v1->unset_user_flag(f1);
    v1->unset_user_flag(f2);
    v1->unset_user_flag(f3);
    v2->unset_user_flag(f1);
    v2->unset_user_flag(f2);
    v2->unset_user_flag(f3);
  }
  return bad;
}

//--------------------------------------------------------------------
//:
//    Input is a set of edges that do not form cycles. There is a
//    set of vertices that represent the unconnected endpoints of a
//    set of paths.  Two endpoints can be connected if there exists
//    an edge between them that is not included in the input set of
//    edges.
bool vtol_cycle_processor::connect_paths(vcl_vector<vtol_edge_2d_sptr>& edges,
                                         vcl_vector<vtol_vertex_sptr>& bad_verts)
{
  bool paths_connected = true;
  if (!bad_verts.size())
    return paths_connected;

//   if (edges.size()==1)
//     vcl_cout << "One Edge\n";

  //Clear the bad vertex flags
  vcl_vector<vtol_vertex_sptr> temp;//temporary bad_verts array

  //
  //Establish flags
  //flag1 defines the state of a vertex in the search for a connecting edge
  //flag2 defines the state of a vertex in forming the set edge_verts,
  //that is there should be no duplicate vertices
  unsigned int flag1 = VSOL_FLAG5, flag2=VSOL_FLAG6;
  //here we assume that all vertices are uniform in flag use.
  if (! bad_verts.front())
    return false;
  //make a copy of bad_verts
  for (vcl_vector<vtol_vertex_sptr>::iterator vit = bad_verts.begin();
       vit != bad_verts.end(); vit++)
  {
    (*vit)->unset_user_flag(flag1);
    temp.push_back(*vit);
  }
  //Collect the vertices from edges
  //Initialize the flags.
  for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges.begin();
      eit != edges.end(); eit++)
    {
      vtol_vertex_sptr v1 = (*eit)->v1(), v2 = (*eit)->v2();
      v1->unset_user_flag(flag1); v2->unset_user_flag(flag1);
      v1->unset_user_flag(flag2); v2->unset_user_flag(flag2);
    }
  //cache the verts for the edges already in the paths
  //flag1 keeps track of vertices added to edge_verts
  vcl_vector<vtol_vertex_sptr> edge_verts;
  for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges.begin();
       eit != edges.end(); eit++)
  {
    vtol_vertex_sptr v1 = (*eit)->v1(), v2 = (*eit)->v2();
    if (!v1->get_user_flag(flag1))
      {edge_verts.push_back(v1); v1->set_user_flag(flag1);}
    if (!v2->get_user_flag(flag1))
      {edge_verts.push_back(v2); v2->set_user_flag(flag1);}
  }

  //search through the list of bad verts and attempt to connect them
  //repaired_verts allows the successfully connected vertices to be
  //removed from the bad_verts set.  flag2 marks vertices as used.
  vcl_vector<vtol_vertex_sptr> repaired_verts;
  for (vcl_vector<vtol_vertex_sptr>::iterator vit=bad_verts.begin();
       vit != bad_verts.end(); vit++)
  {
    if ((*vit)->get_user_flag(flag2))//skip used vertices
      continue;
    bool found_edge = false;
    //find edges attached to each bad vert
    vcl_vector<vtol_edge_sptr>* vedges = (*vit)->edges();
    //scan through vedges to find a connecting edge
    for (vcl_vector<vtol_edge_sptr>::iterator eit = vedges->begin();
         eit != vedges->end()&&!found_edge; eit++)
      {
        vtol_edge_sptr e = *eit;
        vtol_vertex_sptr v = (*eit)->other_endpoint(*(*vit));
        //Continue if:
        //  1)the vertex v has been used;
        //  2)v can't be found in bad_verts;
        //  3)v can't be found in edge_verts;
        //  4)e is already in the input edge set.
        if (v->get_user_flag(flag2))
          continue; //condition 1)
        bool found_in_bad_verts =
          vcl_find(temp.begin(), temp.end(), v) != temp.end();
        bool found_in_edge_verts = false;
        if (!found_in_bad_verts) //condition 2)
          found_in_edge_verts=
            vcl_find(edge_verts.begin(), edge_verts.end(), v) != edge_verts.end();
        if (!(found_in_bad_verts||found_in_edge_verts)) // condition 3)
          continue;
        if ( vcl_find(edges.begin(), edges.end(), e) != edges.end())
          continue; //condition 4)

        //Found a connecting edge, so add it to the input edges
        edges.push_back(e->cast_to_edge_2d());
        found_edge = true;
        v->set_user_flag(flag2);
        (*vit)->set_user_flag(flag2);
        //record the successes
        repaired_verts.push_back(*vit);
        repaired_verts.push_back(v);//should also be in bad_verts
      }
    delete vedges;
    paths_connected =
      paths_connected&&(*vit)->get_user_flag(flag2);
  }
  //Clear the flags
  for (vcl_vector<vtol_vertex_sptr>::iterator vit = bad_verts.begin();
       vit!=bad_verts.end(); vit++)
  {
    (*vit)->unset_user_flag(flag1);
    (*vit)->unset_user_flag(flag2);
  }
  for (vcl_vector<vtol_vertex_sptr>::iterator vit = edge_verts.begin();
       vit!=edge_verts.end(); vit++)
  {
    (*vit)->unset_user_flag(flag1);
    (*vit)->unset_user_flag(flag2);
  }
  //Remove the successful vertex connections
  for (vcl_vector<vtol_vertex_sptr>::iterator vit = repaired_verts.begin();
       vit != repaired_verts.end(); vit++)
  {
    vcl_vector<vtol_vertex_sptr>::iterator erit;
    erit = vcl_find(bad_verts.begin(), bad_verts.end(), *vit);
    if (erit != bad_verts.end())
      bad_verts.erase(erit);
  }
  return paths_connected;
}
