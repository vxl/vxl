// This is brl/bseg/bmrf/bmrf_curve_3d.cxx
//:
// \file

#include "bmrf_curve_3d.h"
#include "bmrf_curvel_3d.h"
#include "bmrf_node.h"
#include "bmrf_epi_seg.h"
#include <vsl/vsl_list_io.h>
#include <vnl/algo/vnl_qr.h>


//: Constructor
bmrf_curve_3d::bmrf_curve_3d()
{
}


//: Trim the ends of the curve with few correspondences
void
bmrf_curve_3d::trim(int min_prj)
{
  for (iterator itr = this->begin(); itr != this->end(); )
  {
    iterator next_itr = itr;
    ++next_itr;
    if ((*itr)->num_projections(true) < min_prj)
      this->erase(itr);
    itr = next_itr;
  }
}


//: Trim curvels with large deviation in gamma
void
bmrf_curve_3d::stat_trim(double max_std)
{
  for (iterator itr = this->begin(); itr != this->end(); )
  {
    iterator next_itr = itr;
    ++next_itr;
    if ((*itr)->gamma_std() > max_std)
      this->erase(itr);
    itr = next_itr;
  }
}


//: Attempt to fill in missing correspondences
void
bmrf_curve_3d::fill_gaps(const vcl_set<int>& frames, double da)
{
  for ( vcl_set<int>::const_iterator fitr = frames.begin();
        fitr != frames.end();  ++fitr )
  {
    bool filled = true;
    while ( filled )
    {
      filled = false;

      for ( iterator itr = this->begin(); itr != this->end(); )
      {
        bmrf_curvel_3d_sptr last_curvel = NULL;
        // move to the next valid node
        while (itr != this->end() && !(*itr)->node_at_frame(*fitr))
          ++itr;
        // move to the next NULL node
        while (itr != this->end() && (*itr)->node_at_frame(*fitr))
          last_curvel = *(itr++);

        if ( itr == this->end() )
          break;

        bmrf_node_sptr node = last_curvel->node_at_frame(*fitr);
        double alpha = last_curvel->alpha_at_frame(*fitr) + da;
        if ( node->epi_seg()->max_alpha() > alpha ){
          (*itr)->set_proj_in_frame(*fitr, alpha, node);
          filled = true;
        }
      }

      for (reverse_iterator itr = this->rbegin(); itr != this->rend(); )
      {
        bmrf_curvel_3d_sptr last_curvel = NULL;
        // move to the next valid node
        while (itr != this->rend() && !(*itr)->node_at_frame(*fitr))
          ++itr;
        // move to the next NULL node
        while (itr != this->rend() && (*itr)->node_at_frame(*fitr))
          last_curvel = *(itr++);

        if ( itr == this->rend() )
          break;

        bmrf_node_sptr node = last_curvel->node_at_frame(*fitr);
        double alpha = last_curvel->alpha_at_frame(*fitr) - da;
        if ( node->epi_seg()->min_alpha() < alpha ){
          (*itr)->set_proj_in_frame(*fitr, alpha, node);
          filled = true;
        }
      }
    }
  }
}


//: Attempt to interpolate artificial values for missing correspondences
void
bmrf_curve_3d::interp_gaps(const vcl_set<int>& frames)
{
  for ( vcl_set<int>::const_iterator fitr = frames.begin();
        fitr != frames.end();  ++fitr )
  {
    vnl_double_2 last_point;
    iterator last_itr = this->end();
    int gap_size = 0;
    for (iterator itr = this->begin(); itr != this->end(); ++itr )
    {
      vnl_double_2 temp_pt;
      if ( (*itr)->pos_in_frame(*fitr, temp_pt) )
      {
        if ( gap_size > 0 && gap_size < 4)
        {
          int gap = 1;
          vnl_double_2 step = temp_pt - last_point;
          for (iterator fill_itr = ++last_itr;
               fill_itr != itr;  ++fill_itr, ++gap)
          {
            double ratio = double(gap)/double(gap_size+1);
            vnl_double_2 new_pt = last_point + step*ratio;
            (*fill_itr)->set_psuedo_point(*fitr, new_pt );
          }
        }
        last_point = temp_pt;
        last_itr = itr;
        gap_size = 0;
      }
      else if (last_itr != this->end()) {
        ++gap_size;
      }
    }
  }
}


//: Simultaneously reconstruct all points in a 3d curve
void
bmrf_curve_3d::reconstruct(const vcl_map<int,vnl_double_3x4>& cameras, float sigma)
{
  unsigned int num_pts = this->size();

  float kernel[2];
  kernel[0] = 0.0f;
  kernel[1] = 1.0f;
  if (sigma > 0.0f) {
    kernel[0] = float(vcl_exp(-1.0/(2*sigma*sigma)));
    float kernel_sum = 2.0f*kernel[0] + kernel[1];
    kernel[0] /= kernel_sum;
    kernel[1] /= kernel_sum;
  }

  vnl_matrix<double> A(3*num_pts, 3*num_pts, 0.0);
  vnl_vector<double> b(3*num_pts, 0.0);

  bmrf_curve_3d::iterator itr = this->begin();
  for (unsigned int cnt=0; itr != this->end(); ++itr, ++cnt)
  {
    unsigned int num_views = (*itr)->num_projections(true);
    vnl_matrix<double> C(2*num_views, 3, 0.0);
    vnl_vector<double> d(2*num_views, 0.0);
    unsigned int v=0;
    for ( vcl_map<int,vnl_double_3x4>::const_iterator C_itr = cameras.begin();
          C_itr != cameras.end();  ++C_itr ) {
      const int f = C_itr->first;
      const vnl_double_3x4 cam = C_itr->second;
      vnl_double_2 pos;
      if ( (*itr)->pos_in_frame(f,pos) ) {
        for (unsigned int i=0; i<3; i++) {
          C[2*v  ][i] = (pos[0]*cam[2][i] - cam[0][i])/2000;
          C[2*v+1][i] = (pos[1]*cam[2][i] - cam[1][i])/2000;
        }
        d[2*v  ] = -(pos[0]*cam[2][3] - cam[0][3])/2000;
        d[2*v+1] = -(pos[1]*cam[2][3] - cam[1][3])/2000;
        ++v;
      }
    }
    vnl_matrix<double> Ct = C.transpose();
    C = Ct * C;
    d = Ct * d;
    for (int i=0; i<3; ++i)
    {
      for (int j=0; j<3; ++j) {
        A[3*cnt+i][3*cnt+j] = C[i][j];
      }
      b[3*cnt+i] = d[i];
      if (cnt > 0 && cnt < num_pts-1) {
        A[3*cnt+i][3*(cnt-1)+i] -= double(kernel[0]);
        A[3*cnt+i][3* cnt   +i] += 1.0-double(kernel[1]);
        A[3*cnt+i][3*(cnt+1)+i] -= double(kernel[0]);
      }
    }
  }

  vnl_qr<double> qr_solver(A);
  vnl_vector<double> p = qr_solver.solve(b);

  vnl_vector<double> error = A*p - b;
  error.normalize();

  vnl_vector<double> error2(error.size()/3,0.0);
  for (unsigned int cnt=0; cnt < error2.size(); ++cnt) {
    double e1 = error[3*cnt];
    double e2 = error[3*cnt+1];
    double e3 = error[3*cnt+2];
    error2[cnt] = vcl_sqrt(e1*e1+e2*e2+e3*e3);
  }

  double max = error2.max_value();
  double min = error2.min_value();
  error2 -= min;
  error2 *= 1.0/(max-min);

  itr = this->begin();
  for (unsigned int cnt=0; itr != this->end(); ++itr, ++cnt)
  {
    (*itr)->set(p[cnt*3], p[cnt*3+1], p[cnt*3+2]);
    (*itr)->set_proj_error(error2[cnt]);
  }
}


//: Binary save self to stream.
void
bmrf_curve_3d::b_write( vsl_b_ostream& os ) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, *((vcl_list<bmrf_curvel_3d_sptr> const*)this));
}


//: Binary load self from stream.
void
bmrf_curve_3d::b_read( vsl_b_istream& is )
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1:
   {
    vsl_b_read(is, *((vcl_list<bmrf_curvel_3d_sptr>*)this));

    break;
   }

   default:
    vcl_cerr << "I/O ERROR: bmrf_curve_3d::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//: Return IO version number;
short
bmrf_curve_3d::version() const
{
  return 1;
}


//: Print an ascii summary to the stream
void
bmrf_curve_3d::print_summary( vcl_ostream& os ) const
{
  os << "num_pts=" << this->size();
}

//-----------------------------------------------------------------------------------------
// External functions
//-----------------------------------------------------------------------------------------

//: Binary save bmrf_curve_3d to stream.
void
vsl_b_write(vsl_b_ostream &os, const bmrf_curve_3d* c)
{
  if (c==0) {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else{
    vsl_b_write(os,true); // Indicate non-null pointer stored
    c->b_write(os);
  }
}


//: Binary load bmrf_curve_3d from stream.
void
vsl_b_read(vsl_b_istream &is, bmrf_curve_3d* &c)
{
  delete c;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
    c = new bmrf_curve_3d();
    c->b_read(is);
  }
  else
    c = 0;
}


//: Print an ASCII summary to the stream
void
vsl_print_summary(vcl_ostream &os, const bmrf_curve_3d* c)
{
  os << "bmrf_curve_3d{ ";
  c->print_summary(os);
  os << " }";
}

