// This is gel/gevd/gevd_clean_edgels.cxx
#include "gevd_clean_edgels.h"
//:
// \file

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_algorithm.h> // for vcl_find()
#include <vul/vul_timer.h>
#include <vsol/vsol_point_2d.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator_sptr.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_interpolator_linear.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_edge_2d.h>

static bool verbose = false;

static bool near_equal(vdgl_digital_curve_sptr /*dc1*/, vdgl_digital_curve_sptr /*dc2*/, float /*tolerance*/)
{
  vcl_cerr << __FILE__ << ": near_equal(dc1,dc2) not yet implemented\n";
  return false; // TODO
#if 0
  if (!(dc1&&dc2))
    return false;
  bool similar=true;
  double n1 = dc1->length(), n2 = dc2->length(), ns;
  //Curves should be similar in length
  if (vcl_fabs(n1-n2)>tolerance)
    return false;

  //Get the shortest curve to probe with
  vdgl_digital_curve_sptr dcs = NULL, dcl = NULL; //s for short, l for long
  if (n1>n2) { dcs = dc2; dcl = dc1; ns = n2; }
  else       { dcs = dc1; dcl = dc2; ns = n1; }

  //Scan the sort curve and get the distance from each edgel
  //to the longer curve.
  for (int i = 0; i<ns; i++)
  {
    vnl_vector<float> ps(X[i], Y[i], 0.0);
    float d = dcl->DistanceFrom(ps);
    if (d>tolerance) // A single distance violation means they aren't similar
    { similar = false; break; }
  }
  return similar;
#endif
}


void gevd_clean_edgels::print_protection()
{
#ifdef DEBUG
  vcl_cout << "Protection Values: ";
  for (EdgelGroup::iterator egit = out_edgels_->begin();
       egit != out_edgels_->end(); ++egit)
    vcl_cout << (*egit)->GetProtection() << ' ';
  vcl_cout << vcl_endl << vcl_endl;
#endif
}


//:Default Constructor
gevd_clean_edgels::gevd_clean_edgels()
{
  out_edgels_ = NULL;
}


//:Default Destructor
gevd_clean_edgels::~gevd_clean_edgels()
{
}


//: The main process method.  The input edgel group is filtered to remove bridges and short edges.
void gevd_clean_edgels::DoCleanEdgelChains(vcl_vector<vtol_edge_2d_sptr>& in_edgels,
                                           vcl_vector<vtol_edge_2d_sptr>& out_edgels, int steps)
{
  vul_timer t;
  out_edgels_= &out_edgels;
  out_edgels_->clear();
  //Copy the input edges to the output
  for (vcl_vector<vtol_edge_2d_sptr>::iterator egit = in_edgels.begin();
       egit != in_edgels.end(); ++egit)
    out_edgels_->push_back(*egit);
  if (steps > 0)
    this->JumpGaps();
  if (steps > 1)
    this->DeleteShortEdges();
  if (steps > 2)
    this->FixDefficientEdgels();
  if (steps > 3)
    this->RemoveBridges();
  if (steps > 4)
    this->RemoveJaggies();
  if (steps > 5)
    this->RemoveLoops();
  vcl_cout << "Total Clean Time(" << out_edgels_->size() << ") in " << t.real() << " msecs.\n";
}


//: Merge edges which have all edgels within a given tolerance
//
void gevd_clean_edgels::detect_similar_edges(vcl_vector<vtol_edge_2d_sptr >& common_edges,
                                             float tolerance,
                                             vcl_vector<vtol_edge_2d_sptr >& deleted_edges)
{
  vcl_vector<vtol_edge_2d_sptr > temp;
  for (vcl_vector<vtol_edge_2d_sptr >::iterator e1it = common_edges.begin();
       e1it != common_edges.end(); ++e1it)
  {
    vtol_edge_2d_sptr e1 = (*e1it);
    vdgl_digital_curve_sptr dc1 = e1->curve()->cast_to_digital_curve();
    if (!dc1) continue;
    vcl_vector<vtol_edge_2d_sptr >::iterator e2it = e1it;
    for (e2it++; e2it != common_edges.end(); ++e2it)
    {
      vtol_edge_2d_sptr e2 = (*e2it);
      vdgl_digital_curve_sptr dc2 = e2->curve()->cast_to_digital_curve();
      if (near_equal(dc1, dc2, tolerance))
        temp.push_back(e2);
    }
  }
  for (vcl_vector<vtol_edge_2d_sptr >::iterator eit = temp.begin();
       eit != temp.end(); ++eit)
  {
    vtol_edge_2d_sptr e = (*eit);
    // e->unlink_all_inferiors(); // -tpk-
    deleted_edges.push_back(e);
  }
}


//: Find similar edges between v1 and some other vertex v and remove them.
//  In this case, similar means all edgels of the similar edges lie within a given pixel tolerance
//  from each other.
void gevd_clean_edgels::remove_similar_edges(vtol_vertex_2d*& v1, vcl_vector<vtol_edge_2d_sptr >& deleted_edges)
{
  float tol = 3.0f;
  vcl_vector<vtol_edge_sptr>* v1_edges = v1->edges();
  vcl_vector<vtol_vertex_2d_sptr> opposite_v1_verts;
  //Find all the vertics wes opposite from v1
  for (vcl_vector<vtol_edge_sptr>::iterator eit = v1_edges->begin();
       eit != v1_edges->end(); ++eit)
  {
    vtol_vertex_2d_sptr v11 = (*eit)->v1()->cast_to_vertex_2d(),
                        v12 = (*eit)->v2()->cast_to_vertex_2d();
    if (v11==v1)
      {opposite_v1_verts.push_back(v12); continue;}
    if (v12==v1)
      {opposite_v1_verts.push_back(v11); continue;}
    vcl_cout << "In gevd_clean_edgels::remove_similar_edges(..) shouldn't happen\n";
  }
  //Then get the opposite vertices, v, with more than one edge between v1 and v
  //For these edges merge them into a common edge if they are too close
  for (vcl_vector<vtol_vertex_2d_sptr>::iterator vit= opposite_v1_verts.begin();
       vit != opposite_v1_verts.end(); ++vit)
  {
    vcl_vector<vtol_edge_2d_sptr > intersection;
    this->edge_exists(v1, (*vit), intersection);
    if (intersection.size()>1)
      this->detect_similar_edges(intersection, tol, deleted_edges);
  }
}


//: Find if an edge already exists between the given vertices
bool gevd_clean_edgels::edge_exists(vtol_vertex_2d_sptr v1, vtol_vertex_2d_sptr v2, vcl_vector<vtol_edge_2d_sptr >& intersection)
{
  bool found = false;
  intersection.clear();

  vcl_vector<vtol_edge_sptr>* edges = v1->edges();

  for (vcl_vector<vtol_edge_sptr>::iterator eit = edges->begin();
       eit != edges->end(); ++eit)
  {
    vtol_edge_2d* e = (*eit)->cast_to_edge_2d();
    if (!e) continue;
    if ( (e->v1() == v1 && e->v2() == v2) || (e->v1() == v2 && e->v2() == v1) )
    {
      intersection.push_back(e);
      found = true;
    }
  }
  delete edges;
  return found;
}


//: Remove edges which are already connected to the given vertex.
void gevd_clean_edgels::remove_connected_edges(vtol_vertex_2d* v, vcl_vector<vtol_edge_2d_sptr >& edges)
{
  vcl_vector<vtol_edge_2d_sptr > tmp;
  for (vcl_vector<vtol_edge_2d_sptr >::iterator eit = edges.begin();
       eit != edges.end(); ++eit)
  {
    vtol_edge_2d_sptr e = (*eit);
    if (e->v1() != v && e->v2() != v)
      tmp.push_back(e);
  }
  edges = tmp;
}


//: Find the closest vertex within a given radius on a given edge
//  If one of the edge vertices is within the radius, then choose it.
//  Otherwise return a vertex which lies on, and interior to, the edge.
//  Original compared end vertex distances to radius, now with actual
//  jump span - JLM Sept. 99
bool gevd_clean_edgels::closest_vertex(vtol_edge_2d_sptr e, vsol_point_2d_sptr p, float radius, vtol_vertex_2d_sptr& v)
{
  vdgl_digital_curve_sptr dc = e->curve()->cast_to_digital_curve();
  if (!dc){ v = NULL; return false;}
  vsol_point_2d_sptr sp = new vsol_point_2d(*p);
  vsol_point_2d_sptr pc = dc->get_interpolator()->closest_point_on_curve( sp );
  double span_sq = p->distance(pc);
  if (radius < span_sq)
    vcl_cerr << __FILE__ << ": closest_vertex(): Warning: ignoring radius="
             << radius << " since span=" << span_sq << " is larger\n";

  vtol_vertex_2d_sptr v1 = e->v1()->cast_to_vertex_2d(), v2 = e->v2()->cast_to_vertex_2d();
  double d1 = v1->point()->distance(p);
  double d2 = v2->point()->distance(p);
  if (d1<d2)
    {
      if (d1<=span_sq)
        {
          v = v1;
          return true;
        }
    }
  else
    {
    if (d2<=span_sq)
      {
        v = v2;
        return true;
      }
    }
  v = new vtol_vertex_2d(*pc);
  return false;
}


//: Split an edge at a vertex which is assumed geometrically to lie on the edge.
bool gevd_clean_edgels::split_edge(vtol_edge_2d_sptr e, vtol_vertex_2d_sptr new_v,
                                   vtol_edge_2d_sptr & e1, vtol_edge_2d_sptr & e2)
{
  if (!e||!new_v)
  {
    vcl_cout << "In gevd_clean_edgels::split_edge(..) null edge or vertex\n";
    return false;
  }
  vdgl_digital_curve_sptr dc = e->curve()->cast_to_digital_curve();
  if (!dc)
  {
    vcl_cout << "In gevd_clean_edgels::split_edge(..) no digital curve\n";
    return false;
  }

  // Find the proper index
  int index = -1;
  double min_distance = 10e5;
  for (unsigned int i=0; i< dc->get_interpolator()->get_edgel_chain()->size(); ++i)
  {
    vgl_point_2d<double> curve_point = dc->get_interpolator()->get_edgel_chain()->edgel(i).get_pt();
    double d = new_v->point()->distance(vsol_point_2d(curve_point));
    if (d < min_distance)
    {
      index = i;
      min_distance = d;
    }
  }

  vdgl_edgel_chain_sptr cxy= dc->get_interpolator()->get_edgel_chain();

  // 2. Create first subchain up to and including junction pixel.
  vtol_edge_2d_sptr edge1 = new vtol_edge_2d();    // create subchains, broken at junction.
  vdgl_edgel_chain_sptr ec= new vdgl_edgel_chain;
  vdgl_interpolator_sptr it= new vdgl_interpolator_linear( ec);
  vdgl_digital_curve_sptr dc1 = new vdgl_digital_curve( it);
  edge1->set_curve(*dc1);
  vdgl_edgel_chain_sptr cxy1= ec;

  for (int k = 0; k < index; k++)
    cxy1->add_edgel( (*cxy)[k] );

  vtol_vertex_2d * v1 = e->v1()->cast_to_vertex_2d();

  edge1->set_v1(v1);            // link both directions v-e
  edge1->set_v2(new_v->cast_to_vertex());     // unlink when stronger.UnProtect()

  // Create second subchain from and including junction pixel.
  vtol_edge_2d_sptr edge2 = new vtol_edge_2d();    // create second subchain
  vdgl_edgel_chain *ec2= new vdgl_edgel_chain;
  vdgl_interpolator *it2= new vdgl_interpolator_linear( ec2);
  vdgl_digital_curve_sptr dc2= new vdgl_digital_curve( it2);
  edge2->set_curve(*dc2);
  vdgl_edgel_chain *cxy2= ec2;

  for (unsigned int k = index; k < dc->get_interpolator()->get_edgel_chain()->size(); ++k)
    cxy2->add_edgel( cxy->edgel( k ));

  vtol_vertex_sptr v2 = e->v2()->cast_to_vertex();

  edge2->set_v1(new_v->cast_to_vertex());     // link both directions v-e
  edge2->set_v2(v2.ptr());            // unlink when stronger.UnProtect()


#if 0
  if (!dc->Split(*(new_v->GetLocation()), dc1, dc2))
    {
      e1 = NULL; e2 = NULL;
      return false;
    }
#endif
  // vtol_vertex_2d *v1 = (vtol_vertex_2d*)e->v1().ptr(), *v2 = (vtol_vertex_2d*)e->v2().ptr();
  // e1 = new vtol_edge_2d(v1, new_v); e2 = new vtol_edge_2d(new_v, v2);
  // e1->SetCurve(dc1) ; e2->SetCurve(dc2);
  e1 = edge1;
  e2 = edge2;
  //e->unlink_all_inferiors(); // -tpk-
  return true;
}


//: Jump gaps by finding the nearest edge to a vertex which is not incident on the vertex.
//    If some point on the digital curve of
//    edge is within a radius of the vertex, then jump across.
//
void gevd_clean_edgels::JumpGaps()
{
  vul_timer t;
  float radius = 5.0f;
  //  float radius = 5.0; Feb 08, 2001 - used in recent DDB exp
  //  float radius = 6.0;
  //All the vertices of the initial segmentation
  vcl_vector<vtol_vertex_2d*> verts;
  vcl_vector<vtol_edge_2d_sptr >::iterator eit;
  for (eit = out_edgels_->begin(); eit != out_edgels_->end(); ++eit)
  {
    verts.push_back( (*eit)->v1()->cast_to_vertex_2d() );
    verts.push_back( (*eit)->v2()->cast_to_vertex_2d() );
  }
  //Iterate over the vertices and find nearby edgel chains
  for (vcl_vector<vtol_vertex_2d*>::iterator vit = verts.begin();
       vit != verts.end(); ++vit)
  {
    vtol_vertex_2d* v = (*vit);
    vsol_point_2d_sptr p = v->point();
    //Get the edges within the radius of the vertex
    vcl_vector<vtol_edge_2d_sptr > near_edges;

    // out_edgels_->EdgesWithinRadius(*p, radius, near_edges);
    // Find edges within the given radius
    for (eit = out_edgels_->begin(); eit != out_edgels_->end(); ++eit)
    {
      vdgl_digital_curve_sptr dc = (*eit)->curve()->cast_to_digital_curve();
      if (dc && radius > dc->get_interpolator()->distance_curve_to_point(p))
        near_edges.push_back( *eit );
    }

    if (verbose) vcl_cout << "Found: " << near_edges.size() << " near edges, ";
    //Get rid of edges already connected to the vertex

    this->remove_connected_edges(v, near_edges);
    if (verbose) vcl_cout << "There were " << near_edges.size() << " after connected removal\n";
    //Now iterate over the nearby edges and try to connect
    //We assume that the edges all have digital curves
          int nnn = 1;
    for (eit = near_edges.begin(); eit != near_edges.end() && nnn == 1; ++eit)
    {
      ///      nnn=0;
      vtol_edge_2d_sptr  e = (*eit);
      vtol_vertex_2d_sptr new_v = NULL;
      //If end_vertex is true, then one of the vertices of e is
      //within the given radius
      bool end_vertex = this->closest_vertex(e, p, radius, new_v);
      if (!new_v) continue; // should always have a new_v
      if (!end_vertex)
      {
        //The new vertex is interior to e,
        //so we have to split the edge
        vtol_edge_2d_sptr e1=NULL, e2=NULL;
        if (verbose) vcl_cout << "Splitting " << e->v1()->cast_to_vertex() << e->v2()->cast_to_vertex() << vcl_endl;
        if (!this->split_edge(e, new_v, e1, e2))
          continue;
        if (verbose) vcl_cout << "It Split, new is: " << *new_v << vcl_endl;
        out_edgels_->push_back(e1);
        out_edgels_->push_back(e2);
        vcl_vector<vtol_edge_2d_sptr >::iterator f;
        //e->unlink(); -tpk-
        f = vcl_find(out_edgels_->begin(), out_edgels_->end(), e);
        if (f != out_edgels_->end())
        {
          if (verbose) vcl_cout <<"getting rid of old edge\n";
          out_edgels_->erase(f);
        }
      }
      //Check if an edge already exists
      vcl_vector<vtol_edge_2d_sptr > intersection;//Contains the duplicate edge(s)
      if (this->edge_exists(v, new_v, intersection))
        {
        continue;
        }
      //Now add the new edge which fills the gap
      if (verbose) vcl_cout << "Adding a gap jumping edgel from " << *v << " to " << *new_v << vcl_endl;
      vtol_edge_2d_sptr new_edge = new vtol_edge_2d(v, new_v);
      // vdgl_digital_curve_sptr dc = new vdgl_digital_curve(v->point(),new_v->point());
      vdgl_edgel_chain* new_chain = new vdgl_edgel_chain();
      new_chain->add_edgel(vdgl_edgel(v->point()->x(), v->point()->y()));
      new_chain->add_edgel(vdgl_edgel(new_v->point()->x(), new_v->point()->y()));
      vdgl_digital_curve_sptr dc = new vdgl_digital_curve(new vdgl_interpolator_linear(new_chain));
      // dc->set_p0(vsol_point_2d_sptr(new vsol_point_2d(v)));
      new_edge->set_curve(*dc);
      out_edgels_->push_back(new_edge);
    }
  }
  // delete verts;
  vcl_cout << "JumpGaps(" << out_edgels_->size() << ") in " << t.real() << " msecs.\n";
}


//: Remove all edges which are shorter than two pixels.
//     That is, e.V1() and e.v2().ptr() are closer than three pixels along both image
//     axes, and no edgel in the vtol_edge_2d is farther than two pixels from vertices.
//     The edge is removed and replaced by a single vertex at the
//     average location.
void gevd_clean_edgels::DeleteShortEdges()
{
  int d_remove=2;
  int d_close = 1;
  vul_timer t;
  int N_total=0, N_close=0;
  vcl_vector<vtol_edge_2d_sptr > deleted_edges;
  int edgelcount = 0;
  for (vcl_vector<vtol_edge_2d_sptr >::iterator egit = out_edgels_->begin();
       egit != out_edgels_->end(); egit++, ++N_total)
  {
    if ( (edgelcount % 100) == 0 )
      vcl_cout << "Edgels: " << edgelcount << '/' << out_edgels_->size() << vcl_endl;
    edgelcount++;
    vtol_edge_2d_sptr e = (vtol_edge_2d_sptr )(*egit);
    vtol_vertex_2d* v1 = (vtol_vertex_2d*)e->v1().ptr();
    vtol_vertex_2d* v2 = (vtol_vertex_2d*)e->v2().ptr();
    double fx1 = v1->x(), fy1 = v1->y();
    double fx2 = v2->x(), fy2 = v2->y();
    int x1 = int(fx1), y1 = int(fy1);
    int x2 = int(fx2), y2 = int(fy2);
    int dx = x2-x1; if (dx < 0) dx = -dx; // dx = vcl_abs(x2-x1);
    int dy = y2-y1; if (dy < 0) dy = -dy; // dy = vcl_abs(y2-y1);
    //First, are the vertices too close?
    if (dx<d_remove && dy<d_remove)
    {
      N_close++;
      //If so, then check if all edgels in the EdgelChain are
      // too close, i.e, within 2 pixels of both vertices
      vdgl_digital_curve_sptr dc = e->curve()->cast_to_digital_curve();
      vdgl_edgel_chain_sptr chain = dc->get_interpolator()->get_edgel_chain();
      int n_edgels = chain->size();
      bool all_close = true;
      for (int t = 0; (t<n_edgels)&&all_close; t++)
      {
        int xe = int(chain->edgel(t).x());
        int ye = int(chain->edgel(t).y());
        dx = xe-x1; if (dx < 0) dx = -dx; // dx = vcl_abs(xe-x1);
        dy = ye-y1; if (dy < 0) dy = -dy; // dy = vcl_abs(ye-y1);
        bool far_from_v1 = (dx>d_close||dy>d_close);
        dx = xe-x2; if (dx < 0) dx = -dx; // dx = vcl_abs(xe-x2);
        dy = ye-y2; if (dy < 0) dy = -dy; // dy = vcl_abs(ye-y2);
        bool far_from_v2 = (dx>d_close||dy>d_close);
        if (far_from_v1&&far_from_v2)
          all_close = false;
      }

      //They are all too close so, get rid of the edge
      if (all_close)
      {
        // e->unlink_all_inferiors_twoway(e);
        // e->unlink_all_inferiors();
        // v1->merge_references(v2);
        v1->set_x((fx1+fx2)/2);//This could cause missing edgels
        v1->set_y((fy1+fy2)/2);
        this->remove_similar_edges(v1, deleted_edges);
        //We remove e last since it may already
        //have been removed by remove_similar_edges
        deleted_edges.push_back(e);
      }
    }
  }
  for (vcl_vector<vtol_edge_2d_sptr >::iterator eit = deleted_edges.begin();
       eit != deleted_edges.end(); ++eit)
  {
    vcl_vector<vtol_edge_2d_sptr >::iterator f;
    // (*eit)->unlink();
    f = vcl_find(out_edgels_->begin(), out_edgels_->end(), *eit);
    if (f != out_edgels_->end())
      out_edgels_->erase( f );
  }

  vcl_cout << "Delete Short Edges(" << out_edgels_->size() << ") in " << t.real() << " msecs.\n"
           << "Ntotal = " << N_total << "  Nclose =  " << N_close << vcl_endl;
}


//: A bridge is an edge or a sequence of edges which is not closed.
//    In this approach, a set of vertices with order one (one incident edge) is
//    found and the associated edges are deleted.  The process is repeated
//    until no more vertices of order one are found.
void gevd_clean_edgels::RemoveBridges()
{
  vul_timer t;
  bool order_one = true;
  vcl_vector<vtol_vertex_2d*> v_one;
  while (order_one)
  {
    //Get the current set of vertices
    // vcl_vector<vtol_vertex_2d*>* verts = Vertices(out_edgels_);
    vcl_vector<vtol_vertex_2d*> verts;
    vcl_vector<vtol_edge_2d_sptr >::iterator eit;
    for (eit = out_edgels_->begin(); eit != out_edgels_->end(); ++eit)
    {
      verts.push_back( (*eit)->v1()->cast_to_vertex_2d() );
      verts.push_back( (*eit)->v2()->cast_to_vertex_2d() );
    }
    v_one.clear();
    //Collect all the vertices of order one which are not self-loops.
    for (vcl_vector<vtol_vertex_2d*>::iterator vit = verts.begin();
         vit != verts.end(); ++vit)
    {
      vtol_vertex_2d* v = *vit;
      vcl_vector<vtol_edge_sptr>* edges = v->edges();
      if (edges->size()==1)
      {
        vtol_edge_sptr e = (*edges)[0];
        if (e->v1()!=e->v2())
          v_one.push_back(v);
      }
    }
    //The main termination condition, i.e., no order one vertices
    if (v_one.size()==0)
    {
      order_one=false;
      continue;
    }
    //Remove the Edge(s) attached to order zero vertices
    for (vcl_vector<vtol_vertex_2d*>::iterator v1 = v_one.begin();
         v1 != v_one.end(); ++v1)
    {
      vcl_vector<vtol_edge_sptr>* v_edges = (*v1)->edges();
      int order = v_edges->size();
      if (order<1 )
      {
        //We can leave isolated vertices in v_one
        //but they will go away on the next sweep
        delete v_edges;
        continue;
      }
      vtol_edge_sptr ep = (*v_edges)[0];
      delete v_edges;
      if (!ep)
      {
        vcl_cout << "In gevd_clean_edgels::RemoveBridges() - null edge\n";
        order_one = false;
        continue;
      }
      vtol_edge_2d_sptr e = ep->cast_to_edge_2d();
      if (!e)
      {
        vcl_cout << "In gevd_clean_edgels::RemoveBridges() - edge is not an edge_2d\n";
        order_one = false;
        continue;
      }
      // e->unlink_all_inferiors_twoway(e);
      // e->unlink_all_inferiors();
      vcl_vector<vtol_edge_2d_sptr >::iterator f;
      vcl_cout << "Removing from output edgels: " << e << vcl_endl;
      // e->unlink();
      f = vcl_find(out_edgels_->begin(), out_edgels_->end(), e);
      if (f != out_edgels_->end())
      {
        out_edgels_->erase(f);
      }
    }
  }
  vcl_cout << "Remove Bridges(" << out_edgels_->size() << ") in " << t.real() << " msecs.\n";
}


//:
//  Check if the number of edgels is <=2.  If so, replace the
//  vdgl_digital_curve_sptr with one formed from a straight line between the endpoints.
void gevd_clean_edgels::FixDefficientEdgels()
{
  vul_timer t;
  for (vcl_vector<vtol_edge_2d_sptr >::iterator egit = out_edgels_->begin();
       egit!=out_edgels_->end(); ++egit)
  {
    bool fix_it = false;
    vtol_edge_2d_sptr e = (vtol_edge_2d_sptr )(*egit);
    vdgl_digital_curve_sptr dc = e->curve()->cast_to_digital_curve();
    fix_it = fix_it || !dc;
    int n_edgels = dc->get_interpolator()->get_edgel_chain()->size();
    fix_it = fix_it || n_edgels<=2;
    if (fix_it)
    {
      vtol_vertex_2d* v1 = (vtol_vertex_2d*)e->v1().ptr();
      vtol_vertex_2d* v2 = (vtol_vertex_2d*)e->v2().ptr();
      vsol_point_2d_sptr p1 = v1->point();
      vsol_point_2d_sptr p2 = v2->point();
      // vdgl_digital_curve_sptr dc = new vdgl_digital_curve(p1, p2);
      vdgl_edgel_chain_sptr chain = new vdgl_edgel_chain();
      // chain->set_p0(p1);
      // chain->set_p1(p2);
      chain->add_edgel(vdgl_edgel(p1->x(), p1->y()));
      chain->add_edgel(vdgl_edgel(p2->x(), p2->y()));
      vdgl_digital_curve_sptr dc = new vdgl_digital_curve(new vdgl_interpolator_linear(chain));
      e->set_curve(*dc);
    }
  }
  vcl_cout << "Fix Defficient Edgels(" << out_edgels_->size() << ") in " << t.real() << " msecs.\n";
}


//:
//    The VD edgedetector produces wild jaggies in the contour
//    from time to time.  This function "smooths' the digital
//    chains by removing sharp adjacent excursions.
void gevd_clean_edgels::RemoveJaggies()
{
  vul_timer t;
  for (vcl_vector<vtol_edge_2d_sptr >::iterator egit = out_edgels_->begin();
       egit!=out_edgels_->end(); ++egit)
  {
    vtol_edge_2d_sptr e = (vtol_edge_2d_sptr )(*egit);
    vtol_vertex_2d* v1 = (vtol_vertex_2d*)e->v1().ptr();
    vtol_vertex_2d* v2 = (vtol_vertex_2d*)e->v2().ptr();
    double x1 = v1->x(), y1 = v1->y();
    double x2 = v2->x(), y2 = v2->y();
    vdgl_digital_curve_sptr dc = e->curve()->cast_to_digital_curve();
    vdgl_edgel_chain_sptr chain = dc->get_interpolator()->get_edgel_chain();
    int n_edgels = chain->size();
    int n1 = n_edgels-1;
    double xo = chain->edgel(0).x(), yo = chain->edgel(0).y();
    // dc->GetX(0), yo = dc->GetY(0);
    // float xn = dc->GetX(n1), yn = dc->GetY(n1);
    double xn = chain->edgel(n1).x(), yn = chain->edgel(n1).y();
    double d1o = (x1-xo)*(x1-xo) + (y1-yo)*(y1-yo);
    double d1n = (x1-xn)*(x1-xn) + (y1-yn)*(y1-yn);
    if (d1o<d1n)//The expected case
    {
      // dc->SetX(x1, 0); dc->SetY(y1, 0);
      chain->edgel(0).set_x(x1);
      chain->edgel(0).set_y(y1);
      // dc->SetX(x2, n1); dc->SetY(y2, n1);
      chain->edgel(n1).set_x(x2);
      chain->edgel(n1).set_y(y2);
    }
    else
    {
      // dc->SetX(x2, 0); dc->SetY(y2, 0);
      chain->edgel(0).set_x(x2);
      chain->edgel(0).set_y(y2);
      // dc->SetX(x1, n1); dc->SetY(y1, n1);
      chain->edgel(n1).set_x(x1);
      chain->edgel(n1).set_y(y1);
    }
  }
  vcl_cout << "Remove Jaggies(" << out_edgels_->size() << ") in " << t.real() << " msecs.\n";
}


//: Removal of edges can also produce loops.  Short loops should be removed.
void gevd_clean_edgels::RemoveLoops()
{
  vul_timer t;
  float min_loop_length = 4.0f;  //6 is normally used in Clean
  vcl_vector<vtol_edge_2d_sptr > removed_edges;
  for (vcl_vector<vtol_edge_2d_sptr >::iterator egit = out_edgels_->begin();
       egit!=out_edgels_->end(); ++egit)
  {
    vtol_edge_2d_sptr e = (vtol_edge_2d_sptr )(*egit);
    vtol_vertex_2d* v1 = (vtol_vertex_2d*)e->v1().ptr();
    vtol_vertex_2d* v2 = (vtol_vertex_2d*)e->v2().ptr();
    if (*v1==*v2)//We have a loop
    {
      vdgl_digital_curve_sptr c = e->curve()->cast_to_digital_curve();
      double len = c->length();
      if (verbose) vcl_cout << "In Remove Loops: "<< v1 << " L = " << len << vcl_endl;
      if (len<min_loop_length)
      {
        // e->unlink_all_inferiors_twoway(e);
        // e->unlink_all_inferiors();
        removed_edges.push_back(e);
      }
    }
  }

  for (vcl_vector<vtol_edge_2d_sptr >::iterator eit = removed_edges.begin();
       eit != removed_edges.end(); ++eit)
  {
    vcl_vector<vtol_edge_2d_sptr>::iterator f = vcl_find(out_edgels_->begin(), out_edgels_->end(), *eit);
    if (f != out_edgels_->end())
      out_edgels_->erase(f);
  }

  vcl_cout << "Remove Loops(" << out_edgels_->size() << ") in " << t.real() << " msecs.\n";
}
