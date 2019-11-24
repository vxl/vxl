// This is gel/vtol/vtol_cycle_processor.cxx
#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>
#include "vtol_cycle_processor.h"
//:
// \file

#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif

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
vtol_cycle_processor::vtol_cycle_processor(std::vector<vtol_edge_2d_sptr>& edges,
                                           bool debug1, bool debug2)
{
  debug1_ = debug1;
  debug2_ = debug2;
  tolerance_ = 0;
  init(edges);
}

//: a more convenient interface
//
static void edge_2d_erase(std::vector<vtol_edge_2d_sptr>& edges,
                          vtol_edge_2d_sptr& e)
{
  auto eit =
    std::find(edges.begin(), edges.end(), e);
  if (eit == edges.end())
    return;
  edges.erase(eit);
  return;
}

//: print a vector of vertices
//
static void print_vertices(std::vector<vtol_vertex_sptr>& verts)
{
  for (auto & vert : verts)
    std::cout << vert << '('
             << vert->cast_to_vertex_2d()->x()<< ' '
             << vert->cast_to_vertex_2d()->y()<< ")\n\n";
}

//: print a vector of edges_2d
//
static void print_edges(std::vector<vtol_edge_2d_sptr>& edges)
{
  for (auto & edge : edges)
    std::cout << edge << '('
             << edge->v1()->cast_to_vertex_2d()->x()<< ' '
             << edge->v1()->cast_to_vertex_2d()->y()<< " |"
             << edge->v2()->cast_to_vertex_2d()->x()<< ' '
             << edge->v2()->cast_to_vertex_2d()->y()<< ")\n\n";
}

//: print a vector of edges
//
static void print_edges(std::vector<vtol_edge_sptr>& edges)
{
  for (auto & edge : edges)
    std::cout << edge << '('
             << edge->v1()->cast_to_vertex_2d()->x()<< ' '
             << edge->v1()->cast_to_vertex_2d()->y()<< " |"
             << edge->v2()->cast_to_vertex_2d()->x()<< ' '
             << edge->v2()->cast_to_vertex_2d()->y()<< ")\n\n";
}

//---------------------------------------------------------------
//
static void pop_stacks(vertex_list& verts,
                       std::vector<vtol_edge_2d_sptr>& edges,
                       vtol_vertex_sptr& v,
                       std::vector<vtol_edge_2d_sptr>& cycle_edges)
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
  return ch && ch->get_user_flag(VSOL_FLAG1) != 0;
}

static bool cw(vtol_one_chain_sptr& ch)
{
  return ch && ch->get_user_flag(VSOL_FLAG2) != 0;
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
  double delta = std::fabs(std::fabs(angle)-180);
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
static void v_edges(const vtol_vertex_sptr& v, std::vector<vtol_edge_2d_sptr>& b_edges,
                    bool force, std::vector<vtol_edge_2d_sptr>& edges_at_vertex)
{
  edges_at_vertex.clear();
  edge_list edges; v->edges(edges);
  for (auto & edge : edges)
  {
    vtol_edge_2d_sptr e = edge->cast_to_edge_2d();
    if (std::find(b_edges.begin(), b_edges.end(),e) != b_edges.end())
    {
      if (used(e))
        continue;
      if (unused(e))
        edges_at_vertex.push_back(e);
      if (half_used(e)&&force)
        edges_at_vertex.push_back(e);
    }
  }
}

inline static double flip_y(double ang)
{
  // no need to use sin(), cos() and atan2() for this job! - PVr
  ang = std::fmod(ang,360.0);
  if (ang <= 0) ang += 360.0;
  return 360.0-ang;
}

static double tangent_angle_at_vertex(const vtol_vertex_sptr& v, const vtol_edge_2d_sptr& e)
{
  double ang = 0;
  if (!e||!v||!(v==e->v1()||v==e->v2()))
  {
    std::cout << "vtol_vertex and vtol_edge not incident\n";
    return ang;
  }
  //Here we assume that the edgel_chain starts at v1 and ends at v2;
  if (v==e->v1())
  {
    ang = e->curve()->cast_to_vdgl_digital_curve()->
                        get_interpolator()->get_tangent_angle(0);
  }
  else
  {
    int N = e->curve()->cast_to_vdgl_digital_curve()->
                          get_interpolator()->get_edgel_chain()->size();

    ang = e->curve()->cast_to_vdgl_digital_curve()->
                        get_interpolator()->get_tangent_angle(N-1);
    //reverse the angle since we are at the end rather than the start of the edge?
    ang += 180.0;
  }
  //If we want cw and ccw to be correct senses, we flip y because the input
  //edges are in image coordinates that has a left-handed coordinate system.
  return flip_y(ang);
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
  return nullptr;
}

//----------------------------------------------------------------
//:   Compute the angle between two edges at the specified vtol_vertex, v
//    The angle is mapped to the interval [-180, 180].  The angle sense is
//    defined so that the e0 orientation is towards v and the e1
//    orientation is away from v.
double vtol_cycle_processor::angle_between_edges(const vtol_edge_2d_sptr& e0,
                                                 const vtol_edge_2d_sptr& e1,
                                                 const vtol_vertex_sptr& v)
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
static vtol_edge_2d_sptr ccw_edge(const vtol_edge_2d_sptr& in_edg, const vtol_vertex_sptr& from,
                                  std::vector<vtol_edge_2d_sptr>& edges)
{
  double most_ccw = -360;
  vtol_edge_2d_sptr ccw = nullptr;
  for (auto & edge : edges)
  {
    if (edge==in_edg)
      continue;
    double delta = vtol_cycle_processor::angle_between_edges(in_edg, edge, from);
    if (delta>most_ccw)
    {
      most_ccw = delta;
      ccw = edge;
    }
  }
  //There were no edges found except the incoming edge, so return it.
  if (!ccw && std::find(edges.begin(), edges.end(),in_edg) != edges.end())
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
bool vtol_cycle_processor::assignable(vtol_edge_2d_sptr edg, const vtol_vertex_sptr& last)
{
  if (debug2_)
  {
    std::cout << "In assignable(..)\n"
             << "edg " ; this->print_edge(edg);
    std::cout << "plus used(" << plus_used(edg) << ") minus used("
             << minus_used(edg) << ")\n"
             << "last " << *last << std::endl;
  }
  if (!edg||!last)
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
      std::cout << "==== entering assign_initial_edge =====\n"
               << "e " << *e << "plus used(" << plus_used(e) << ") minus used("
               << minus_used(e) << ")\n";

  if (used(e))
  {
    std::cout << "In vtol_cycle_processor::assign_intial_edge(..) "
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
    std::cout << "==== leaving assign_initial_edge =====\n"
             << "plus used(" << plus_used(e) << ") minus used("
             << minus_used(e) << ")\n\n";
}

//------------------------------------------------------------
//:   Link the vtol_edge, "edg" to the vtol_vertex, "last".
//    Set the appropriate direction flag

void vtol_cycle_processor::assign_ends(vtol_edge_2d_sptr edg, vtol_vertex_sptr& last)
{
  if (debug1_)
      std::cout << "==== entering assign_ends =====\n"
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
  std::cout << "In vtol_cycle_processor::assign ends(..) - shouldn't happen\n";
}

//------------------------------------------------------------
//:
//    Assign the next edge to the top of the edge stack when
//    a cycle is popped off the stack. That is, start the new path
//    with the edge at the top of the stack.  If the stack is
//    empty, then the last assignment to l_ is used.
static void assign_stack_edge(std::vector<vtol_edge_2d_sptr>& e_stack, vtol_edge_2d_sptr& next_edge)
{
  if (!e_stack.size())
    return;
  next_edge = e_stack.back();
}

//------------------------------------------------------------------
//:   The initial setup of the cycle process.
//    Used by the vtol_cycle_processor
//    constructors to establish the start conditions
void vtol_cycle_processor::init(std::vector<vtol_edge_2d_sptr>& in_edges)
{
  edges_.clear();
  chains_.clear();
  nested_one_cycles_.clear();
  for (auto & in_edge : in_edges)
  {
    clear(in_edge);
    edges_.push_back(in_edge);
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
    std::cout << "------init bridge vars-------\n"
             << "oooooooo Vertex Stack ooooooooo\n";
    print_vertices(v_stack_);
    std::cout << "oooooooo Edge Stack ooooooooo\n";
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
    all_bridge = all_bridge && used(e1) && bridge_traverse(angle);
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
vtol_edge_2d_sptr vtol_cycle_processor::search_for_next_edge(std::vector<vtol_edge_2d_sptr>& edges_at_last)
{
  while (edges_at_last.size())
  {
    vtol_edge_2d_sptr temp = ccw_edge(l_, last_, edges_at_last);
    if (debug2_)
    {
      std::cout << "next ccw_edge\n";
      this->print_edge(temp);
    }
    if (!temp || assignable(temp, last_))
      return temp;
    edge_2d_erase(edges_at_last, temp);
  }
  return nullptr;
}

//---------------------------------------------------------------------
//:   A suitable vtol_edge is added to the evolving path
//
void vtol_cycle_processor::add_edge_to_path()
{
  vtol_vertex_sptr temp = last_;
  assign_ends(next_edge_, temp);
  if (debug2_)
    std::cout << "==== after assign_ends =====\n"
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
    std::cout << "------assign_edge_to_path-------\n"
             << "oooooooo Vertex Stack ooooooooo\n";
    print_vertices(v_stack_);
    std::cout << "oooooooo Edge Stack ooooooooo\n";
    print_edges(e_stack_);
  }
}

//------------------------------------------------------------------------
//:   Classify a closed path as a cycle or bridge.
//    The path traverse is reversed since the vtol_edge sequence was
//    popped from the path stack.
//    Thus, the winding angle is opposite in sign, which is
//    accounted for in code.
bool vtol_cycle_processor::classify_path(std::vector<vtol_edge_2d_sptr>& path_edges,
                                         vtol_one_chain_sptr& chain)
{
  if (debug1_)
        std::cout << "======= In classify_path ========\n";
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
  for (auto eit = path_edges.begin()+1;
       eit != path_edges.end(); ++eit)
  {
    classify_adjacent_edges(e0, *eit, all_bridge, angle);

    if (debug1_)
      std::cout << "wind_ang " << winding_angle << " - " << angle << " = "
               << winding_angle - angle << std::endl;

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
  for (auto & path_edge : path_edges)
    c_edges.push_back(path_edge->cast_to_edge());

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
  std::cout << e << " :[(" << e->v1()->cast_to_vertex_2d()->x()
           << ' ' << e->v1()->cast_to_vertex_2d()->y()<< "), ("
           << e->v2()->cast_to_vertex_2d()->x() << ' '
           << e->v2()->cast_to_vertex_2d()->y() << ")]\n";
}

//------------------------------------------------------------------------
//: The main cycle tracing algorithm.
//  The input is edges_ and the output is chains_, a set of 1-cycles.
//  The approach is to select a vtol_edge from edges_ and explore all the
//  vtol_edge(s) connected to it.  The search proceeds by traversing connected
//  vtol_edge(s), turning in a cw or ccw direction depending on the initial vtol_edge
//  orientation.  If the search returns to a vertex already on the path,
//  then a cycle is output.  The traversed vtol_edge(s) and vertices are pushed
//  onto a stack so that cycles can be "popped" off and the search continued
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

      if (debug1_&&last_) {
        std::cout << "last_ ="; last_->print(); std::cout <<std::endl;
      }

      std::vector<vtol_edge_2d_sptr> edges_at_last;
      v_edges(last_, edges_, force, edges_at_last);
      next_edge_ = search_for_next_edge(edges_at_last);

      if (!next_edge_&&!force)
      {
        force = true;
        v_edges(last_, edges_, force, edges_at_last);
        next_edge_ = search_for_next_edge(edges_at_last);
      }
      if (debug1_&&next_edge_) {
        std::cout << "next_edge_("<< force <<") = "; this->print_edge(next_edge_);
      }

      if (!next_edge_)
        found_next_edge_ = false;
      else
        add_edge_to_path();
      if (debug1_)
          std::cout << "========checking for cycle ===========\n"
                   << " last_ " << last_ << '('
                   << last_->cast_to_vertex_2d()->x()<< ' '
                   << last_->cast_to_vertex_2d()->y()<< ")\n";
      cycle_ = std::find(v_stack_.begin(), v_stack_.end(), last_) != v_stack_.end();
      if (debug1_&&cycle_)
        std::cout << "...Found Cycle....\n\n";
    }
    else
    {
      if (cycle_)
      {
        cycle_ = false;
        std::vector<vtol_edge_2d_sptr> cycle_edges;
        pop_stacks(v_stack_, e_stack_, last_, cycle_edges);
        if (debug1_)
        {
          std::cout << "======== In Cycle Classifier =======\n"
                   << "cycle_edges\n";
          print_edges(cycle_edges);
        }
        assign_stack_edge(e_stack_, l_);
        vtol_one_chain_sptr cycle;
        bool is_cycle = classify_path(cycle_edges, cycle);
        if (debug1_)
        {
          std::cout << "is_cycle(" << is_cycle << ")\n";
          if (cycle)
          {
            std::cout << "cycle " << cycle << "[cw(" << cw(cycle)
                     << "), ccw(" << ccw(cycle) << ")]\n";
            std::vector<vtol_edge_sptr> c_edges; cycle->edges(c_edges);
            std::cout << "cycle edges\n";
            print_edges(c_edges);
          }
        }
        if (is_cycle)
          chains_.push_back(cycle);
        else//path was all bridge edges, so remove them from consideration
          for (auto & cycle_edge : cycle_edges)
            edge_2d_erase(edges_,cycle_edge);
      }
      if (!found_next_edge_)
      {
        //Get rid of edges touched in the search
        std::vector<vtol_edge_2d_sptr> removed_edges;
        for (auto & edge : edges_)
          if (touched(edge)&&used(edge))
            removed_edges.push_back(edge);
        for (auto & removed_edge : removed_edges)
          edge_2d_erase(edges_,removed_edge);

        this->set_bridge_vars();
      }
    }
  if (!limit)
    std::cout << "Limit Exhaused in vtol_cycle_processor::compute_cycles(..)\n";
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
    std::cout << "In vtol_cycle_processor::sort_one_cycles(..) no cycles\n";
    return;
  }
  nested_one_cycles_.clear();
  //First, find the outer bounding vtol_one_chain. This outer boundary is
  //defined as a ccw cycle with the largest bounding box.
  //search for the largest ccw bounding box
  double area = 0;
  vtol_one_chain_sptr outer_chain = nullptr;
  for (auto & chain : chains_)
  {
    untouch(chain);
    if (!ccw(chain))
      continue;
    vsol_box_2d_sptr box = chain->get_bounding_box();
    double WxH = box->width()*box->height();
    if (WxH>area)
    {
      area = WxH;
      outer_chain = chain;
    }
  }

  if (!outer_chain||!ccw(outer_chain))
  {
    std::cout << "In vtol_cycle_processor::sort_one_cycles(..)"
             << " Shouldn't happen that there is no outer chain\n"
             << "N cycles = " << chains_.size() << std::endl;
    for (auto & chain : chains_)
    {
      std::cout << " chain is " << (ccw(chain) ? "" : "not ") << "ccw, "
               << "chain is " << (cw(chain) ? "" : "not ") << "cw\n";
    }
    std::cout << "Outer Chain " << outer_chain << std::endl;
    return;
  }
  nested_one_cycles_.push_back(outer_chain);
  touch(outer_chain);
  if (debug1_)
    std::cout << "Outer Cycle area = " << area << std::endl;
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
  for (auto & chain : chains_)
    if (cw(chain)&&!touched(chain))
    {
      if (chain==outer_chain)
        continue;
      vsol_box_2d_sptr bc = chain->get_bounding_box();
      if ((*bc<*b)&&!bc->near_equal(*b, tolerance_))
      {
        vsol_box_2d_sptr bc = chain->get_bounding_box();
        if ((*bc<*b)&&!bc->near_equal(*b, tolerance_))
        {
          if (debug1_)
            std::cout << "Adding inner cycle with area = "
                     << bc->width()*bc->height() << std::endl;

          nested_one_cycles_.push_back(chain);
          touch(chain);
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
static void clear_flags(std::vector<vtol_edge_sptr>& s, unsigned int flag)
{
  for (auto & eit : s)
    eit->unset_user_flag(flag);
}

static void set_flags(std::vector<vtol_edge_sptr>& s, unsigned int flag)
{
  for (auto & eit : s)
    eit->set_user_flag(flag);
}

//---------------------------------------------------------------------------
//: This method scans the edge sets s1, s2 and computes their set intersection.
// If the intersection is empty then the method returns false.
// The method uses flags to mark edges appearing in both lists. Thus the
// computation is O(n1+n2).
//
// This method might not be needed if stl algorithms worked ("old" vcl probs)
// however with flags this might be faster than stl
//
bool vtol_cycle_processor::intersect_edges(std::vector<vtol_edge_sptr>& s1,
                                           std::vector<vtol_edge_sptr>& s2,
                                           std::vector<vtol_edge_sptr>& s1_and_s2)
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
  for (const auto& e : s2)
  {
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
// This method might not be needed if stl algorithms worked ("old" vcl probs)
// however with flags this might be faster than stl
//
bool vtol_cycle_processor::difference_edges(std::vector<vtol_edge_sptr>& s1,
                                            std::vector<vtol_edge_sptr>& s2,
                                            std::vector<vtol_edge_sptr>& s1_minus_s2)
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
  for (const auto& e : s1)
  {
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
bool vtol_cycle_processor::corrupt_boundary(std::vector<vtol_edge_2d_sptr>& edges,
                                            std::vector<vtol_vertex_sptr>& bad_verts)
{
  bool bad = false;
  //Initialize Markers
  if (!edges.front())
  {
    std::cout << "In cycle_processor::corrupt_boundary - null edge\n";
    return false;
  }
  //setup vertex flags
  unsigned int f1=VSOL_FLAG4, f2=VSOL_FLAG5, f3=VSOL_FLAG6;
  //Initialize Flags
  for (auto & edge : edges)
  {
    vtol_vertex_sptr v1 = edge->v1();
    vtol_vertex_sptr v2 = edge->v2();
    v1->unset_user_flag(f1);
    v1->unset_user_flag(f2);
    v1->unset_user_flag(f3);
    v2->unset_user_flag(f1);
    v2->unset_user_flag(f2);
    v2->unset_user_flag(f3);
  }
  //Mark using flags that a vertex is incident on two edges
  //Flags f1 and f2 act as a counter
  for (auto & edge : edges)
  {
    vtol_vertex_sptr v1 = edge->v1();
    vtol_vertex_sptr v2 = edge->v2();
    if (!v1->get_user_flag(f1))
      v1->set_user_flag(f1);
    else
      v1->set_user_flag(f2);
    if (!v2->get_user_flag(f1))
      v2->set_user_flag(f1);
    else
      v2->set_user_flag(f2);
  }
  for (auto & edge : edges)
  {
    vtol_vertex_sptr v1 = edge->v1();
    vtol_vertex_sptr v2 = edge->v2();
    if ((v1!=v2)&&*v1==*v2)
      std::cout << "Improper Loop(\n" << *v1 << *v2 << ")\n\n";
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
  for (auto & edge : edges)
  {
    vtol_vertex_sptr v1 = edge->v1();
    vtol_vertex_sptr v2 = edge->v2();
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
bool vtol_cycle_processor::connect_paths(std::vector<vtol_edge_2d_sptr>& edges,
                                         std::vector<vtol_vertex_sptr>& bad_verts)
{
  bool paths_connected = true;
  if (!bad_verts.size())
    return paths_connected;

//   if (edges.size()==1)
//     std::cout << "One Edge\n";

  //Clear the bad vertex flags
  std::vector<vtol_vertex_sptr> temp;//temporary bad_verts array

  //
  //Establish flags
  //flag1 defines the state of a vertex in the search for a connecting edge
  //flag2 defines the state of a vertex in forming the set edge_verts,
  //that is there should be no duplicate vertices
  unsigned int flag1 = VSOL_FLAG5, flag2=VSOL_FLAG6;
  //here we assume that all vertices are uniform in flag use.
  if (!bad_verts.front())
    return false;
  //make a copy of bad_verts
  for (auto & bad_vert : bad_verts)
  {
    bad_vert->unset_user_flag(flag1);
    temp.push_back(bad_vert);
  }
  //Collect the vertices from edges
  //Initialize the flags.
  for (auto & edge : edges)
  {
    vtol_vertex_sptr v1 = edge->v1(), v2 = edge->v2();
    v1->unset_user_flag(flag1); v2->unset_user_flag(flag1);
    v1->unset_user_flag(flag2); v2->unset_user_flag(flag2);
  }
  //cache the verts for the edges already in the paths
  //flag1 keeps track of vertices added to edge_verts
  std::vector<vtol_vertex_sptr> edge_verts;
  for (auto & edge : edges)
  {
    vtol_vertex_sptr v1 = edge->v1(), v2 = edge->v2();
    if (!v1->get_user_flag(flag1))
      {edge_verts.push_back(v1); v1->set_user_flag(flag1);}
    if (!v2->get_user_flag(flag1))
      {edge_verts.push_back(v2); v2->set_user_flag(flag1);}
  }

  //search through the list of bad verts and attempt to connect them
  //repaired_verts allows the successfully connected vertices to be
  //removed from the bad_verts set.  flag2 marks vertices as used.
  std::vector<vtol_vertex_sptr> repaired_verts;
  for (auto & bad_vert : bad_verts)
  {
    if (bad_vert->get_user_flag(flag2))//skip used vertices
      continue;
    bool found_edge = false;
    //find edges attached to each bad vert
    std::vector<vtol_edge_sptr> vedges; bad_vert->edges(vedges);
    //scan through vedges to find a connecting edge
    for (auto eit = vedges.begin();
         eit != vedges.end()&&!found_edge; ++eit)
    {
      vtol_edge_2d_sptr e = (*eit)->cast_to_edge_2d();
      vtol_vertex_sptr v = (*eit)->other_endpoint(*bad_vert);
      //Continue if:
      //  0)the edge e is not a 2d edge
      //  1)the vertex v has been used;
      //  2)v can't be found in bad_verts;
      //  3)v can't be found in edge_verts;
      //  4)e is already in the input edge set.
      if (!e)
        continue; //condition 0)
      if (v->get_user_flag(flag2))
        continue; //condition 1)
      bool found_in_bad_verts = std::find(temp.begin(),temp.end(),v) != temp.end();
      bool found_in_edge_verts = false;
      if (!found_in_bad_verts) //condition 2)
        found_in_edge_verts = std::find(edge_verts.begin(),edge_verts.end(),v) != edge_verts.end();
      if (!found_in_bad_verts && !found_in_edge_verts) // condition 3)
        continue;
      if ( std::find(edges.begin(),edges.end(),e) != edges.end())
        continue; //condition 4)

      //Found a connecting edge, so add it to the input edges
      edges.push_back(e);
      found_edge = true;
      v->set_user_flag(flag2);
      bad_vert->set_user_flag(flag2);
      //record the successes
      repaired_verts.push_back(bad_vert);
      repaired_verts.push_back(v);//should also be in bad_verts
    }
    paths_connected =
      paths_connected&&bad_vert->get_user_flag(flag2);
  }
  //Clear the flags
  for (auto & bad_vert : bad_verts)
  {
    bad_vert->unset_user_flag(flag1);
    bad_vert->unset_user_flag(flag2);
  }
  for (auto & edge_vert : edge_verts)
  {
    edge_vert->unset_user_flag(flag1);
    edge_vert->unset_user_flag(flag2);
  }
  //Remove the successful vertex connections
  for (auto & repaired_vert : repaired_verts)
  {
    std::vector<vtol_vertex_sptr>::iterator erit;
    erit = std::find(bad_verts.begin(), bad_verts.end(), repaired_vert);
    if (erit != bad_verts.end())
      bad_verts.erase(erit);
  }
  return paths_connected;
}
