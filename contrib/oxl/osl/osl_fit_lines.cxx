// This is oxl/osl/osl_fit_lines.cxx
#include "osl_fit_lines.h"
//:
//  \file

#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_list.h>

#include <vnl/vnl_math.h>

#include <osl/osl_canny_port.h>

#define warn_if_empty(e) \
if (true && e->size()==0) \
  vcl_cerr << __FILE__ ": " << __LINE__ << " edge \'" #e "' has zero length\n";

#define WARN if (false) { } else (vcl_cerr << __FILE__ ": " << __LINE__ << ' ')

//--------------------------------------------------------------------------------

osl_fit_lines::osl_fit_lines(osl_fit_lines_params const & params,
                             double scale, double x0, double y0)
  : osl_fit_lines_params(params)
{
  double temp_thresh = this->threshold_;
  if (this->use_square_fit_) temp_thresh *= this->threshold_;
  this->threshold_ = temp_thresh;
  data_ = new osl_OrthogRegress(scale,x0,y0);
}

osl_fit_lines::~osl_fit_lines()
{
  delete data_; data_ = 0;
}

//-----------------------------------------------------------------------------


// This is the simplest option. A non-incremental fitting
// algorithm that attempts to fits straight lines to the line
// segments. For reasonable behaviour the lines should be broken up into
// smaller segments of similar curvature before calling this function.
// When used in conjunction with BreakLines, it mimics Paul Beardsley's
// old straight line fitting code.
//
void osl_fit_lines::simple_fit_to_list(vcl_list<osl_edge *> *myedges,
                                       vcl_list<osl_edge *> *outedges)
{
  int edge_no = 0;
  vcl_list<osl_edgel_chain*> curves;
  for (vcl_list<osl_edge*>::iterator i=myedges->begin(); i!=myedges->end(); ++i)
  {
    osl_edge *edge = *i;

    bool angle_ok = true;
    {
      osl_edgel_chain *dc = edge;//->GetCurve()->CastToDigitalCurve();
      assert(dc!=0);
      bool success = false;

      // If the osl_edgel_chain is long enough fit
      if ( dc->size() >= min_fit_length_ )
      {
        double using_degrees = 1.0;
        // Since some code sets Theta in degrees, and some radians!
        // This is an attempt to guess whether the angles are degrees
        // or radians
        for (unsigned int ii=0; ii<dc->size(); ii++)
        {
          if (dc->GetTheta(ii) > 3.2 || dc->GetTheta(ii) < -3.2)
          {
            using_degrees = vnl_math::pi / 180;
            break;
          }
        }
        data_->Reset();
        double angle = 0.7853981;
        int  orient0 = int((dc->GetTheta(0) * using_degrees + angle/2.0) / angle);
        for (unsigned int ii=0;ii<dc->size();ii++)
        {
          data_->IncrByXY(dc->GetX(ii), dc->GetY(ii));
          int orient = int((dc->GetTheta(ii) * using_degrees + angle/2.0)/angle);
          int diff = vnl_math_abs(orient - orient0);
          if (diff > 1 && diff < 7)
            angle_ok = false;
        }
        data_->Fit();
        float mean_cost = MyGetCost(data_, 0, dc->size(), dc);
        float ls_cost = (float)data_->GetCost();
        if (use_square_fit_ && ls_cost < threshold_ ||
            !use_square_fit_ && mean_cost < threshold_ && angle_ok)
        {
          success = true;
          old_finish_ = 0;
          if (use_square_fit_)
            OutputLine(&curves, 0, dc->size(), dc, ls_cost);
          else
            OutputLine(&curves, 0, dc->size(), dc, mean_cost);
        }
      }

      if (success) { // ie. we have fitted a line and have no gunk
        osl_edgel_chain *mycurve = fsm_pop(&curves);
        // Edge will be an implicit digital line, with vertices on
        // the original curve. However, the start and end point of
        // the curve will not be the same as the curve data.
        //was:edge->SetCurve(mycurve);
        osl_edge *newedge = new osl_edge(*mycurve, edge->GetV1(), edge->GetV2());
        delete mycurve;
        edge->unref();
        edge = newedge;
      }
      edge->SetId(edge_no);
      //edge->GetCurve()->SetId(edge_no++);
      edge->ref/*Protect*/();
      outedges->push_front(edge);
      warn_if_empty(edge);
    }
  }
}

//-----------------------------------------------------------------------------
//:
// Method that takes a vcl_list<Edge*>* that represents the
// current segmentation and breaks off each Edge for further
// investigation. An incremental fitting process is applied to
// the osl_edgel_chain in each osl_edge. The resultant segmentation
// is then written to output (with edges destroyed).
void osl_fit_lines::incremental_fit_to_list(vcl_list<osl_edge *> *myedges,
                                            vcl_list<osl_edge *> *outedges)
{
#ifdef DEBUG
  vcl_cout << "Fitting lines to Edge(s), threshold = " << threshold_ << vcl_endl;
#endif
  int edge_no = 0;
  int vertex_no = 1000;
  // Take each Edge in turn and attempt to fit
  // a number of straight lines to it

  outedges->clear();
  for (vcl_list<osl_edge*>::iterator iter=myedges->begin(); iter!=myedges->end(); ++iter)
  {
    osl_edge *edge = *iter;
    ++edge_no;

    vcl_list<osl_edgel_chain*> curves;
    if (use_square_fit_)
      SquareIncrementalFit(&curves, edge);
    else
      MeanIncrementalFit(&curves, edge);

    // Within 'curves' we now have a list of osl_edgel_chains for which the
    // fitting succeeded and some for which line fitting failed.
    // We want to develop a topology structure from the list that
    // will replace the current edge (so that it has consistent endpoints)
    // with a number of other edges.

    // If we have failed to find a fit.
    if ( curves.empty() )
    {
      edge->SetId(edge_no);
      outedges->push_front(edge);
      warn_if_empty(edge);
    }

    // If we have fitted a line and have no gunk.
    else if ( curves.size() == 1 )
    {
      osl_edgel_chain *mycurve = fsm_pop(&curves);
      outedges->push_front(new osl_edge(*mycurve, edge->GetV1(), edge->GetV2()));
      outedges->front()->SetId(edge_no);
      warn_if_empty(edge);
      edge->unref(); // delete the old one. Can't we reuse it?
    }

    // We have multiple lines derived from a single input edge.
    else
    {
      curves.reverse();
      osl_vertex *v2 = edge->GetV1();
      while (!curves.empty())
      {
        osl_edgel_chain *next = fsm_pop(&curves);
        osl_vertex* v1 = v2;
        if (curves.empty()) // last one
          v2 = edge->GetV2();
        else // somewhere in the middle
          v2 = new osl_vertex(next->GetX(next->size()-1), next->GetY(next->size()-1), vertex_no++);
        outedges->push_front(new osl_edge(*next, v1, v2));
        warn_if_empty(outedges->front());
      }
    }
  }
}

//-----------------------------------------------------------------------------
//:
// This method is used to fit lines incrementally using the mean
// absolute error instead of the mean square error. The difference
// between this and SquareIncrementalFit is very small.
void osl_fit_lines::MeanIncrementalFit(vcl_list<osl_edgel_chain*> *curves_, osl_edge *edge)
{
  //vcl_cerr << "MeanIncrementalFit()\n";
  float new_cost, new_est_cost;
  // Get the digital curve
  osl_edgel_chain *dc = edge;//->GetCurve()->CastToDigitalCurve();
  assert(dc!=0);

  // If the EdgelChain is long enough fit
  if ( dc->size() < min_fit_length_ )
    return;

  bool added = false;
  unsigned int i,j,segment_length;

  // Set up the data class for the first set of Edgels in the DigitalCurve
  unsigned int start = ignore_end_edgels_;
  old_finish_ = 1;  // set old_finish_ so we record from the start
  unsigned int finish = start + min_fit_length_;

  data_->Reset();
  for (i=start;i<finish;i++)
    data_->IncrByXY(dc->GetX(i),dc->GetY(i));

  // Now, until the end of the chain, test whether each
  // Edgel belongs to a straight line
  while ( finish <= dc->size() )
  {
    // Define the start and end points of the DigitalCurve segment
    segment_length = finish - start;

    // Fit by orthogonal regression
    data_->Fit();
    new_cost = MyGetCost(data_, start, finish, dc);

    // Now evaluate the fit, and if both good and there are more points, grow
    if ( new_cost < threshold_)
    {
      // If there are no more points, store the current fit,
      // and mark finish to ensure the next DigitalCurve is used.
      if ( finish == dc->size() )
      {
        OutputLine(curves_, start,finish-ignore_end_edgels_,dc,new_cost);
        finish++;
      }
      else
      {
        // Else, if possible, add more points to the current
        // interpretation by adapting the data class and moving the
        // finish variable. First declare that no points have been
        // added, and that the estimated
        // fitting cost is equal to the actual cost
        added = false;
        new_est_cost = new_cost;
        while ( (finish<dc->size()) && (new_est_cost<threshold_ ) )
        {
          // Compute an upper bound estimate on the fitting cost
          // by adding the distance of the next point to the
          // currently fitted line
          double distance =
            data_->GetA()*dc->GetX(finish) + data_->GetB()*dc->GetY(finish) + data_->GetC();
          new_est_cost = float(segment_length*new_est_cost + vcl_abs(distance)) / (segment_length+1);

          // If this residual is low enough, include the point within
          // the orthogonal regression data class
          if ( new_est_cost < threshold_ )
          {
            data_->IncrByXY(dc->GetX(finish),dc->GetY(finish));
            finish++;
            segment_length++;
            added = true;
          }
        }

        // If no points can be added, output the line, store the fit,
        // and reset the data class
        if ( !added )
        {
          // capes Aug 1999 - now backtrack by ignore_end_edgels_ and
          // refit to avoid fitting to the garbage end edgels which are curving away
          // from the line
          for (unsigned int ii=0; ii<ignore_end_edgels_; ++ii)
            data_->DecrByXY(dc->GetX(finish-1-ii), dc->GetY(finish-1-ii));
          data_->Fit();
          new_cost = MyGetCost(data_, start, finish-ignore_end_edgels_, dc);
          OutputLine(curves_, start,finish-ignore_end_edgels_,dc,new_cost);
          start = finish+ignore_end_edgels_;  finish = start + min_fit_length_;
          if ( finish<=dc->size() )
          {
            data_->Reset();
            for (i=start;i<finish;i++)
              data_->IncrByXY(dc->GetX(i),dc->GetY(i));
          }
        }
      }
    }

    // Else the fit is not good enough. We therefore remove the first
    // point and add or delete points from the end of the current line
    // segment until the resulting segment length is min_fit_length_
    else
    {
      // Unfortunately, using the mean absolute error sometimes
      // means that we have to rewind a bit from the end before
      // drawing the next straight line.
      if ( added )
      {
        added = false;
        i = finish;
        while (new_cost > threshold_ && segment_length > min_fit_length_)
        {
          data_->DecrByXY(dc->GetX(i), dc->GetY(i));
          i --;
          segment_length --;
          data_->Fit();
          new_cost = MyGetCost(data_, start, i, dc);
        }
        finish = i;
        OutputLine(curves_, start,finish,dc,new_cost);
        start = finish;  finish = start + min_fit_length_;
        if ( finish<=dc->size() )
        {
          data_->Reset();
          for (i=start;i<finish;i++)
            data_->IncrByXY(dc->GetX(i),dc->GetY(i));
        }
      }
      else
      {
        data_->DecrByXY(dc->GetX(start),dc->GetY(start));
        start++;

        if ( segment_length > min_fit_length_ )
          while ( segment_length > min_fit_length_+1 )
          {
            data_->DecrByXY(dc->GetX(finish),dc->GetY(finish));
            finish--;
            segment_length--;
          }
        else if ( finish<dc->size() )
        {
          data_->IncrByXY(dc->GetX(finish),dc->GetY(finish));
          finish++;
        }

        else
          finish++;
      }
    }
  }

  // Finally, record any junk edgels at the end of the edgelchain that
  // have not been described by a straight line, that is so long as
  // a line has been fitted
  unsigned int length = dc->size() - old_finish_;
  if ( curves_->size() && length )
  {
    osl_edgel_chain *ndc = new osl_edgel_chain(length);
    for (i=0,j=old_finish_;j<dc->size();i++,j++)
    {
      ndc->SetX(dc->GetX(j),i);
      ndc->SetY(dc->GetY(j),i);
      ndc->SetGrad(dc->GetGrad(j),i);
      ndc->SetTheta(dc->GetTheta(j),i);
    }
    curves_->push_front(ndc);
  }
}

//--------------------------------------------------------------------------------
//:
// Method that takes the canny edge data stored in an edge
// and its associated DigitalCurve, and fits lines using
// orthogonal regression with mean square error residual and incremental
// fitting.
void osl_fit_lines::SquareIncrementalFit(vcl_list<osl_edgel_chain*> *curves_, osl_edge *edge)
{
#ifdef DEBUG
  vcl_cerr << "SquareIncrementalFit()\n";
#endif
  // Get the digital curve
  osl_edgel_chain *dc = edge;//->GetCurve()->CastToDigitalCurve();
  assert(dc!=0);

  // If the EdgelChain is long enough fit
  if ( dc->size() < min_fit_length_ )
    return;

  bool added = false;
  unsigned int i,j,segment_length;

  // Set up the data class for the first set of Edgels in the DigitalCurve
  unsigned int start = ignore_end_edgels_;
  old_finish_ = 1;  // set old_finish_ so we record from the start
  unsigned int finish = start + min_fit_length_;

  data_->Reset();
  for (i=start;i<finish;i++)
    data_->IncrByXY(dc->GetX(i),dc->GetY(i));

  // Now, until the end of the chain, test whether each
  // Edgel belongs to a straight line
  while ( finish <= dc->size() )
  {
    // Define the start and end points of the DigitalCurve segment
    segment_length = finish - start;

    // Fit by orthogonal regression
    data_->Fit();


    // Now evaluate the fit, and if both good and there are more points, grow
    if ( data_->GetCost() < threshold_ )
    {
      // If there are no more points, store the current fit,
      // and mark finish to ensure the next DigitalCurve is used.
      if ( finish == dc->size() )
      {
        OutputLine(curves_, start,finish-ignore_end_edgels_,dc,float(data_->GetCost()));
        finish++;
      }

      else
      {
        // Else, if possible, add more points to the current
        // interpretation by adapting the data class and moving the
        // finish variable. First declare that no points have been
        // added, and that the estimated
        // fitting cost is equal to the actual cost
        added = false;
        data_->SetEstCost(data_->GetCost());
        while ( (finish<dc->size()) && (data_->GetEstCost()<threshold_) )
        {
          // Compute an upper bound estimate on the fitting cost
          // by adding the distance of the next point to the
          // currently fitted line
          double distance =
            data_->GetA()*dc->GetX(finish) + data_->GetB()*dc->GetY(finish) + data_->GetC();
          data_->SetEstCost( (segment_length*data_->GetEstCost() + distance*distance) / (segment_length+1) );

          // If this residual is low enough, include the point within
          // the orthogonal regression data class
          if ( data_->GetEstCost()<threshold_ )
          {
            data_->IncrByXY(dc->GetX(finish),dc->GetY(finish));
            finish++;
            segment_length++;
            added = true;
          }
        }

        // If no points can be added, output the line, store the fit,
        // and reset the data class
        if ( !added )
        {
          // capes Aug 1999 - now backtrack by ignore_end_edgels_ and
          // refit to avoid fitting to the garbage end edgels which are curving away
          // from the line
          for (unsigned int ii=0; ii<ignore_end_edgels_; ++ii)
            data_->DecrByXY(dc->GetX(finish-1-ii), dc->GetY(finish-1-ii));
          data_->Fit();

          OutputLine(curves_, start,finish-ignore_end_edgels_,dc,float(data_->GetCost()));
          start = finish+ignore_end_edgels_;  finish = start + min_fit_length_;
          if ( finish<=dc->size() )
          {
            data_->Reset();
            for (i=start;i<finish;i++)
              data_->IncrByXY(dc->GetX(i),dc->GetY(i));
          }
        }
      }
    }

    // Else the fit is not good enough. We therefore remove the first
    // point and add or delete points from the end of the current line
    // segment until the resulting segment length is min_fit_length_
    else
    {
      data_->DecrByXY(dc->GetX(start),dc->GetY(start));
      start++;

      if ( segment_length > min_fit_length_ )
        while ( (segment_length-1) > min_fit_length_ )
        {
          data_->DecrByXY(dc->GetX(finish),dc->GetY(finish));
          finish--;
          segment_length--;
        }
      else if ( finish<dc->size() )
      {
        data_->IncrByXY(dc->GetX(finish),dc->GetY(finish));
        finish++;
      }

      else
        finish++;
    }
  }

  // Finally, record any junk edgels at the end of the edgelchain that
  // have not been described by a straight line, that is so long as
  // a line has been fitted.
  unsigned int length = dc->size() - old_finish_;
  if ( curves_->size() && length )
  {
    osl_edgel_chain *ndc = new osl_edgel_chain(length);
    for (i=0,j=old_finish_;j<dc->size();i++,j++)
    {
      ndc->SetX(dc->GetX(j),i);
      ndc->SetY(dc->GetY(j),i);
      ndc->SetGrad(dc->GetGrad(j),i);
      ndc->SetTheta(dc->GetTheta(j),i);
    }
    curves_->push_front(ndc);
  }
}

//-----------------------------------------------------------------------------
//
//: Output the fitted line.
void osl_fit_lines::OutputLine(vcl_list<osl_edgel_chain*> *curves_,
                               int start, int finish,
                               osl_edgel_chain *dc,
                               float /*cost*/)
{
  // First of all, store the edgel data between the current fit
  // and the previous fit to a DigitalCurve
  int length = start - old_finish_;
  if ( length > 0 )
  {
    osl_edgel_chain *ndc = new osl_edgel_chain(length);
    for (int i=0,j=old_finish_;j<start;i++,j++)
    {
      ndc->SetX(dc->GetX(j),i);
      ndc->SetY(dc->GetY(j),i);
      ndc->SetGrad(dc->GetGrad(j),i);
      ndc->SetTheta(dc->GetTheta(j),i);
    }
    curves_->push_front(ndc);
  }
  old_finish_ = finish;

  // Create a osl_edgel_chain from the fit
  osl_edgel_chain *line = new osl_edgel_chain(finish-start);
  for (int i=0,j=start;j<finish;i++,j++) {  // Copy the edgels into the new chain
    line->SetX(dc->GetX(j),i);
    line->SetY(dc->GetY(j),i);
    line->SetGrad(dc->GetGrad(j),i);
    line->SetTheta(dc->GetTheta(j),i);
  }
  curves_->push_front(line);

  // and check to see if we can merge two lines together
  MergeLines(curves_);
}

//-----------------------------------------------------------------------------
//:
// Takes the top two lines from the vcl_list<ImplicitDigitalLine*>
// and tests whether a single line fit would satisfy them both. The
// process involves fitting a single line to the other two lines by
// minimising the integral of the Euclidean distance between the
// lines. If the lines appear to come from the same line, a refit
// should be done using the underlying edgel data, *** BUT THIS IS
// NOT YET IMPLEMENTED ***
//
void osl_fit_lines::MergeLines(vcl_list<osl_edgel_chain*> *curves_)
{
  if ( curves_->size() < 2 )
    return;

  // Take the first two lines off the list (must be careful about the
  // ordering if we are to produce a correctly formed EdgelChain).
  osl_edgel_chain *dc2 = fsm_pop(curves_);
  osl_edgel_chain *dc1 = fsm_pop(curves_);

#ifndef fsm_is_cute
  osl_ortho_regress fitter;
  fitter.reset();
  for (unsigned int i=0; i<dc1->size(); ++i)
    fitter.add_point(dc1->GetX(i), dc1->GetY(i));
  for (unsigned int i=0; i<dc2->size(); ++i)
    fitter.add_point(dc2->GetX(i), dc2->GetY(i));
  double a, b, c;
  fitter.fit(&a, &b, &c);
  if (fitter.cost(a, b, c) >= threshold_)
  {
    curves_->push_front(dc1);
    curves_->push_front(dc2);
  }
  else
  {
    // FIXME: what if endpoints are equal?
    osl_edgel_chain *dc = new osl_edgel_chain(dc1->size() + dc2->size());
    for (unsigned int i=0; i<dc1->size(); ++i)
    {
      dc->SetX(dc1->GetX(i), i);
      dc->SetY(dc1->GetY(i), i);
      dc->SetGrad(dc1->GetGrad(i), i);
      dc->SetTheta(dc1->GetTheta(i), i);
    }
    for (unsigned int i=0; i<dc2->size(); ++i)
    {
      dc->SetX(dc2->GetX(i), i + dc1->size());
      dc->SetY(dc2->GetY(i), i + dc1->size());
      dc->SetGrad(dc2->GetGrad(i), i + dc1->size());
      dc->SetTheta(dc2->GetTheta(i), i + dc1->size());
    }
    delete dc1;
    delete dc2;
    curves_->push_front(dc);
  }

#else
  ImplicitDigitalLine *l1 = (ImplicitDigitalLine*) dc1;
  ImplicitDigitalLine *l2 = (ImplicitDigitalLine*) dc2;

  // Compute the line parameters
  double x1[2],y1[2],x2[2],y2[2];
  ImplicitDigitalLine *line = l1;
  for (int i=0; i<2; i++,line=l2)
  {
    x1[i] = line->GetStartX();  y1[i] = line->GetStartY();
    x2[i] = line->GetEndX();    y2[i] = line->GetEndY();
  }

  // We have problems later if either of the lines are vertical. Therefore
  // rotate all of the points by phi which is defined in the following way
  // (we have to undo this rotation later):
  float xstart = vcl_min( vcl_min(x1[0],x1[1]), vcl_min(x2[0],x2[1]) );
  float xfinish= vcl_max( vcl_max(x1[0],x1[1]), vcl_max(x2[0],x2[1]) );
  float ystart = vcl_min( vcl_min(y1[0],y1[1]), vcl_min(y2[0],y2[1]) );
  float yfinish= vcl_max( vcl_max(y1[0],y1[1]), vcl_max(y2[0],y2[1]) );
  float phi = vcl_atan2(yfinish-ystart,xfinish-xstart);
  double cp = vcl_cos(phi), sp = vcl_sin(phi);

  for (int i=0;i<2;i++)
  {
    float x = x1[i]*cp + y1[i]*sp;
    y1[i]   = y1[i]*cp - x1[i]*sp;
    x1[i] = x;

    x     = x2[i]*cp + y2[i]*sp;
    y2[i] = y2[i]*cp - x2[i]*sp;
    x2[i] = x;
  }

  // Determine the rotated line parameters
  double a[2],b[2],c[2];
  for (int i=0;i<2;i++)
  {
    // This next bit for the Bhattacharyya comparison
    a[i] = y1[i] - y2[i];
    b[i] = x2[i] - x1[i];
    c[i] = x1[i]*y2[i] - x2[i]*y1[i];

    double m = vcl_sqrt(a[i]*a[i] + b[i]*b[i]);
    if ( b[i] > 0 ) m = -m;

    a[i] /= m;  b[i] /= m;  c[i] /= m;
  }

  // Check the angle between the lines, if it is too great return
  double theta = 180.0*vcl_acos(a[0]*a[1]+b[0]*b[1])/M_PI;
  // Best we can do is eliminate cases that don't double
  // back on themselves
  if ( theta > 90.0 )
    theta = 180.0 - theta;
  if ( theta > theta_ )
  {
    curves_->push_front(dc1);  curves_->push_front(dc2);
    return;
  }

  // Parameters and scatter matrices
  double x1_2,x1_3,x2_2,x2_3;
  vnl_matrix<double> S(3,3,0.0);
  vnl_matrix<double> Si(3,3),S1i(3,3),S2i(3,3),S3i(3,3);
  vnl_matrix<double> S11(2,2),S12(2,1),S_lambda,vec3(1,1);
  double S22;
  double normaliser=0.0; // Effecively the total number of points in the lines

  // Form a scatter matrix for the two lines
  for (int i=0;i<2;i++)
  {
    // Form the scatter matrices for each line
    S1i.put(0,0,1.0);  S1i.put(0,1,-a[i]/b[i]);  S1i.put(0,2,0.0);
    S1i.put(1,0,-a[i]/b[i]);  S1i.put(1,1,(a[i]*a[i])/(b[i]*b[i]));
    S1i.put(1,2,0.0);
    S1i.put(2,0,0.0);  S1i.put(2,1,0.0);         S1i.put(2,2,0.0);

    S2i.put(0,0,0.0);  S2i.put(0,1,-c[i]/(2.0*b[i]));
    S2i.put(0,2,0.5);
    S2i.put(1,0,-c[i]/(2.0*b[i]));  S2i.put(1,1,(a[i]*c[i])/(b[i]*b[i]));
    S2i.put(1,2,-a[i]/(2.0*b[i]));
    S2i.put(2,0,0.5);  S2i.put(2,1,-a[i]/(2.0*b[i]));
    S2i.put(2,2,0.0);

    S3i.put(0,0,0.0);  S3i.put(0,1,0.0);      S3i.put(0,2,0.0);
    S3i.put(1,0,0.0);  S3i.put(1,1,(c[i]*c[i])/(b[i]*b[i]));
    S3i.put(1,2,-c[i]/b[i]);
    S3i.put(2,0,0.0);  S3i.put(2,1,-c[i]/b[i]);
    S3i.put(2,2,1.0);

    x1_2 = x1[i]*x1[i];  x1_3 = x1_2*x1[i];
    x2_2 = x2[i]*x2[i];  x2_3 = x2_2*x2[i];

    // Compute the total scatter matrix for each line
    if ( x2[i] > x1[i] )
    {
      Si = (x2_3-x1_3)/3.0*S1i;
      Si += (x2_2-x1_2)*S2i;
      Si += (x2[i]-x1[i])*S3i;
      double s = vcl_fabs(b[i]);
      Si *= (1.0/s);
    }
    else
    {
      Si = ((x1_3-x2_3)*S1i/3.0+(x1_2-x2_2)*S2i+
            (x1[i]-x2[i])*S3i);
      Si *= 1/vcl_fabs(b[i]);
    }

    // and add this to the total scatter matrix
    S += Si;

    // Adjust the normaliser (denonimator of the algebraic distance)
    normaliser += vcl_fabs((x2[i]-x1[i])/b[i]);
  }

  // Form the sub-partitioned scatter matrices
  S11.put(0,0,S.get(0,0));  S11.put(0,1,S.get(0,1));
  S11.put(1,0,S.get(1,0));  S11.put(1,1,S.get(1,1));
  S12.put(0,0,S.get(0,2));  S12.put(1,0,S.get(1,2));
  S22 = S.get(2,2);

  // Now compute the matrix whose eigenvalues are required
  S_lambda = S11 - S12 * (1.0/S22) * S12.transpose();

  // Assign data for the Eigenvalue/vector computation
  vnl_matrix<double> U(2,2),V(2,2),W(2,2);
  vnl_matrix<double> eigenvector(2,1);

  // Compute the Eigenvalues and the fitted line parameters
  S_lambda.singular_value_decomposition(U,W,V);

  // Check that the Eigenvalues are both semi-positive
  if ((W.get(0,0)<0.0) || (W.get(1,1)<0.0))
  {
    WARN << "numerical ill-conditioning in MergeLines";
    curves_->push_front(l1);  curves_->push_front(l2);
    return;
  }

  double cost;  // Again this is the mean square distance
  if ( W.get(0,0) < W.get(1,1) )
  {
    cost = W.get(0,0)/normaliser;
    eigenvector.put(0,0,U.get(0,0));  eigenvector.put(1,0,U.get(1,0));
  }
  else
  {
    cost = W.get(1,1)/normaliser;
    eigenvector.put(0,0,U.get(0,1));  eigenvector.put(1,0,U.get(1,1));
  }

  // If the cost of the fitted line is too high restore the EdgeList and return
  if ( cost >= threshold_ )
  {
    curves_->push_front(l1);  curves_->push_front(l2);
    return;
  }
  else
    WARN << "Lines merged with a cost " << vcl_sqrt(cost) << vcl_endl;

  // We now have the parameters for the re-fitted line
  vec3 = - (1.0/S22) * S12.transpose() * eigenvector;
  double la,lb,lc;
  la = eigenvector.get(0,0);  lb = eigenvector.get(1,0);  lc = vec3(0,0);
  double m = vcl_sqrt(la*la+lb*lb);
  // Make sure that b >= 0 to be consistent with osl_OrthogRegress - though
  // we don't actually store {a,b,c}
  if ( lb < 0.0 )
    m *= -1.0;
  la /= m;  lb /= m;  lc /= m;

  // All that is left to do is to project the endpoints of the
  // original lines onto the new line segment and to take the
  // extremal ones. Thus we can form a new ImplicitDigitalLine
  double xa,ya,xb,yb;
  xa = lb*lb*x1[0] - la*lb*y1[0] - la*lc;
  ya = la*la*y1[0] - la*lb*x1[0] - lb*lc;
  xb = lb*lb*x2[1] - la*lb*y2[1] - la*lc;
  yb = la*la*y2[1] - la*lb*x2[1] - lb*lc;

  // Set the new edge-line to have these endpoints after putting back the
  // rotation
  float XA,YA,XB,YB;
  XA = xa*cp - ya*sp;
  YA = ya*cp + xa*sp;
  XB = xb*cp - yb*sp;
  YB = yb*cp + xb*sp;

  // Create a new ImplicitLine and EdgelChain with these parameters
  line = new ImplicitDigitalLine(l1->size()+l2->size(),
                                 new IUPoint(XA,YA,0.0),
                                 new IUPoint(XB,YB,0.0));

  int i = 0;
  for (int j=0;j<l1->size();++i,++j) {  // Copy the edgels from l1 to new line
    line->SetX(l1->GetX(j),i);
    line->SetY(l1->GetY(j),i);
    line->SetGrad(l1->GetGrad(j),i);
    line->SetTheta(l1->GetTheta(j),i);
  }
  for (int j=0;j<l2->size();++i,++j) {  // Copy the edgels from l2 to new line
    line->SetX(l2->GetX(j),i);
    line->SetY(l2->GetY(j),i);
    line->SetGrad(l2->GetGrad(j),i);
    line->SetTheta(l2->GetTheta(j),i);
  }

  // Store the new ImplicitDigitalLine
  curves_->push_front(line);

  // and remove the old ones

  osl_IUDelete(l1);  osl_IUDelete(l2);
#endif
}

//-----------------------------------------------------------------------------
//
//: This method calculates the current mean absolute cost of fitting a line.
float osl_fit_lines::MyGetCost(osl_OrthogRegress const *fitter,
                               int start, int finish,
                               osl_edgel_chain *dc)
{
  double A = fitter->GetA();
  double B = fitter->GetB();
  double C = fitter->GetC();

  float distance = 0;

  for (int i = start; i < finish; i ++)
    distance += (float)vcl_fabs(A*dc->GetX(i) + B*dc->GetY(i) + C);

  return distance / (finish - start);
}
