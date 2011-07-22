// This is brad/brad_eigenspace.txx
#ifndef brad_eigenspace_txx_
#define brad_eigenspace_txx_
#include "brad_eigenspace.h"
#include <vcl_cstdlib.h> // for std::rand()
#include <vcl_cassert.h>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vnl/io/vnl_io_matrix.h>
#include <vnl/io/vnl_io_vector.h>
#include <vnl/vnl_numeric_traits.h>
#include <vil/vil_blocked_image_facade.h>
static void 
print_resource_stats(vcl_vector<vil_image_resource_sptr> const& rescs){
  vcl_cout << "processing " << rescs.size() << " resources\n";
  vcl_vector<vil_image_resource_sptr>::const_iterator rit = rescs.begin();
  for(unsigned i = 0; rit!= rescs.end(); ++rit, ++i){
    unsigned ni = (*rit)->ni(), nj = (*rit)->nj();
    vcl_cout << "[" << i << "]:(" << ni << ' ' << nj << ")\n" << vcl_flush;
  }
}
template <class T>
bool brad_eigenspace<T>::
compute_covariance_matrix(vcl_vector<vil_image_resource_sptr> const& rescs)
{
  unsigned n = funct_.size();
  if(!n) return false;
  vcl_cout << "computing covariance matrix\n" << vcl_flush;
  covar_valid_ = false;
  print_resource_stats(rescs);
  mean_.set_size(n);
  mean_.fill(0.0);
  vnl_matrix<double> var(n, n);
  var.fill(0.0);
  unsigned n_samples = 0;
  vcl_vector<vil_image_resource_sptr>::const_iterator rit = rescs.begin();
  for(; rit!= rescs.end(); ++rit){
    unsigned ni = (*rit)->ni(), nj = (*rit)->nj();
    if(ni==0||nj==0||ni<nib_||nj<njb_) return false;
    unsigned nbi = ni/nib_, nbj = nj/njb_;
    unsigned i0 = 0, j0 = 0;
    for(unsigned r = 0; r<nbj; ++r, j0+=njb_){
      i0 = 0;
      for(unsigned c = 0; c<nbi; ++c, i0+=nib_)
        {
          vil_image_view_base_sptr view_ptr = 
            (*rit)->get_view(i0, nib_, j0, njb_);
          vil_image_view<float> fview = vil_convert_cast(float(), view_ptr);
          vnl_vector<double> v = funct_(fview);
          mean_ += v;
          var += outer_product(v, v);
          n_samples++;
        }
      vcl_cout << '.'<< vcl_flush;
    }
    vcl_cout << '\n' << vcl_flush;
  }
  if(!n_samples) return false;
  double ninv = 1.0/static_cast<double>(n_samples);
  mean_ *= ninv;
  covar_ = ninv*var - outer_product(mean_, mean_);
  covar_valid_ = true;
  return true;
}
template <class T>
bool brad_eigenspace<T>::
compute_covariance_matrix_rand(vcl_vector<vil_image_resource_sptr> const& rescs, double frac, unsigned nit, unsigned njt){

  vcl_vector<vil_image_resource_sptr>::const_iterator rit = rescs.begin();
  double area = 0.0;
  for(unsigned i = 0; rit!= rescs.end(); ++rit, ++i){
    unsigned ni = (*rit)->ni(), nj = (*rit)->nj();
    if(ni<nib_||nj<njb_)
      return false;
    area += ni*nj;
  }
  //the total image area to be processed
  double proc_area = area*frac;
  // the number of random tiles to be processed
  double nitd = nit, njtd = njt;
  unsigned ntiles = static_cast<unsigned>(proc_area/(nitd*njtd));
  if(!ntiles) return false;

  //set up the covariance data
  unsigned n = funct_.size();
  if(!n) return false;
  vcl_cout << "computing covariance matrix - randomly selecting " 
           << ntiles << " (" << nit << 'x' << njt << ") tiles\n" << vcl_flush;
  covar_valid_ = false;
  print_resource_stats(rescs);
  mean_.set_size(n);
  mean_.fill(0.0);
  vnl_matrix<double> var(n, n);
  var.fill(0.0);
  unsigned n_samples = 0;

  // process the tiles
  double nd = rescs.size();
  for(unsigned t = 0; t<ntiles; ++t){
    //randomly select a resource
    unsigned ires = 
      static_cast<unsigned>((nd)*(vcl_rand()/(RAND_MAX+1.0)));  
    //compute random access to tile
    double nid = rescs[ires]->ni(), njd = rescs[ires]->nj();
    double rd = (njd-njtd-1.0)*(vcl_rand()/(RAND_MAX+1.0));
    double cd = (nid-nitd-1.0)*(vcl_rand()/(RAND_MAX+1.0));
    if(rd<0) rd = 0.0;
    if(cd<0) cd = 0.0;
    if(rd>(njd-njtd-1.0)) rd = (njd-njtd-1.0);
    if(cd>(nid-nitd-1.0)) cd = (nid-nitd-1.0);
    unsigned j0 = static_cast<unsigned>(rd), i0 = static_cast<unsigned>(cd);
    vil_image_view_base_sptr tile_ptr = 
      rescs[ires]->get_view(i0, nit, j0, njt);
    vil_image_view<float> ftile = vil_convert_cast(float(), tile_ptr);
    unsigned nbi = nit/nib_, nbj = njt/njb_;
    i0 = 0; j0=0;
    for(unsigned r = 0; r<nbj; ++r, j0+=njb_){
      i0 = 0;
      for(unsigned c = 0; c<nbi; ++c, i0+=nib_){
        vil_image_view<float> fview(nib_, njb_);
        for(unsigned j =0; j<njb_; ++j)
          for(unsigned i =0; i<nib_; ++i)
            fview(i, j) = ftile(i0+i, j0+j);
        vnl_vector<double> v = funct_(fview);
        mean_ += v;
        var += outer_product(v, v);
        n_samples++;
        if(n_samples%100==0)
          vcl_cout << ires << ' ' << vcl_flush;
      }
    }
  }
  if(!n_samples) return false;
  double ninv = 1.0/static_cast<double>(n_samples);
  mean_ *= ninv;
  covar_ = ninv*var - outer_product(mean_, mean_);
  covar_valid_ = true;
  vcl_cout << '\n' << vcl_flush;
  return true;
}

template <class T>
bool brad_eigenspace<T>::
compute_eigensystem(){
  if(!covar_valid_)
    return false;
  eigensystem_valid_ = false;
  vcl_cout << "computing eigensystem\n" << vcl_flush;
  vnl_symmetric_eigensystem<double> sym_eig(covar_);
  unsigned n = covar_.rows();
  eigenvectors_ = sym_eig.V;
  eigenvalues_.set_size(n);
  for(unsigned i = 0; i<n; ++i)
    eigenvalues_[i] = sym_eig.get_eigenvalue(i);
  eigensystem_valid_ = true;
  return true;
}
template <class T>
bool brad_eigenspace<T>::
compute_eigenimage(vil_image_resource_sptr const& resc,
                   vcl_string const& output_path)
{
  if(!eigensystem_valid_)
    return false;
  vcl_cout << "computing eigenvalue color image\n";
  unsigned n = funct_.size();
  vnl_vector<double> v(n);
  vnl_vector<double> v0 = eigenvectors_.get_column(n-1);
  vnl_vector<double> v1 = eigenvectors_.get_column(n-2);
  vnl_vector<double> v2 = eigenvectors_.get_column(n-3);
  unsigned ni = resc->ni(), nj = resc->nj();
  if(ni==0||nj==0||ni<nib_||nj<njb_) return false;
  unsigned nbi = ni/nib_, nbj = nj/njb_;
  vil_image_resource_sptr out_resc =
    vil_new_image_resource(output_path.c_str(), nbi, nbj, 3,
                           VIL_PIXEL_FORMAT_FLOAT,
                           "tiff");
  unsigned i0 = 0, j0 = 0;
  for(unsigned r = 0; r<nbj; ++r, j0+=njb_){
    i0 = 0;
    vil_image_view<float> row(nbi, 1, 3);
    for(unsigned c = 0; c<nbi; ++c, i0+=nib_)
      {
        vil_image_view_base_sptr view_ptr = 
          resc->get_view(i0, nib_, j0, njb_);
        vil_image_view<float> fview = vil_convert_cast(float(), view_ptr);
        vnl_vector<double> v = funct_(fview); 
        float eig0 = static_cast<float>(dot_product(v, v0));
        float eig1 = static_cast<float>(dot_product(v, v1));
        float eig2 = static_cast<float>(dot_product(v, v2));
        row(c, 0, 0) = eig0; row(c, 0, 1) = eig1; row(c, 0, 2) = eig2;
      }
    out_resc->put_view(row, 0, r);
    vcl_cout << '.'<< vcl_flush;
  }
  vcl_cout << '\n' << vcl_flush;
  return true;
}
template <class T>
bool brad_eigenspace<T>::
init_histogram(vil_image_resource_sptr const& resc, unsigned nbins,
               bsta_joint_histogram_3d<float>& hist){
  if(!eigensystem_valid_)
    return false;
  vcl_cout << "intializing eigenvalue histogram\n";
  unsigned n = funct_.size();
  vnl_vector<double> v(n), minv(n), maxv(n);
  minv.fill(vnl_numeric_traits<float>::maxval);
  maxv.fill(0.0f);
  vnl_vector<double> v0 = eigenvectors_.get_column(n-1);
  vnl_vector<double> v1 = eigenvectors_.get_column(n-2);
  vnl_vector<double> v2 = eigenvectors_.get_column(n-3);
  unsigned ni = resc->ni(), nj = resc->nj();
  if(ni==0||nj==0||ni<nib_||nj<njb_) return false;
  unsigned nbi = ni/nib_, nbj = nj/njb_;
  unsigned i0 = 0, j0 = 0;
  for(unsigned r = 0; r<nbj; ++r, j0+=njb_){
    i0 = 0;
    for(unsigned c = 0; c<nbi; ++c, i0+=nib_)
      {
        vil_image_view_base_sptr view_ptr = 
          resc->get_view(i0, nib_, j0, njb_);
        vil_image_view<float> fview = vil_convert_cast(float(), view_ptr);
        vnl_vector<double> v = funct_(fview); 
        float eig0 = static_cast<float>(dot_product(v, v0));
        float eig1 = static_cast<float>(dot_product(v, v1));
        float eig2 = static_cast<float>(dot_product(v, v2));
        if(eig0<minv[0]) minv[0]=eig0; if(eig1<minv[1]) minv[1]=eig1;
        if(eig2<minv[2]) minv[2]=eig2;
        if(eig0>maxv[0]) maxv[0]=eig0; if(eig1>maxv[1]) maxv[1]=eig1;
        if(eig2>maxv[2]) maxv[2]=eig2;
      }
    vcl_cout << '.'<< vcl_flush;
  }
  vcl_cout << '\n' << vcl_flush;
  vnl_vector<double> delta = (maxv-minv)/static_cast<float>(nbins);
  float min0 = static_cast<float>(minv[0]-delta[0]);
  float max0 = static_cast<float>(maxv[0]+delta[0]);
  float min1 = static_cast<float>(minv[1]-delta[1]);
  float max1 = static_cast<float>(maxv[1]+delta[1]);
  float min2 = static_cast<float>(minv[2]-delta[2]);
  float max2 = static_cast<float>(maxv[2]+delta[2]);
  hist = bsta_joint_histogram_3d<float>(min0, max0, nbins,
                                        min1, max1, nbins,
                                        min2, max2, nbins);
  return true;
}

static 
bool get_view(vil_image_resource_sptr const& ir,
              unsigned ni, unsigned nj,
              vil_image_view<float>& view, 
              bool& done){
  static bool first = true;
  done = false;
  static unsigned cbi = 0, cbj = 0;
  static unsigned i0 = 0, j0 = 0;
  static unsigned bi = 0, bj = 0;
  static vil_image_view<float> cblock;
  static vil_blocked_image_resource_sptr bif = 0;
  if(first){
    unsigned si = ni*16, sj = nj*16;
    bif = vil_new_blocked_image_facade(ir, si, sj);
  }
  unsigned rni = ir->ni(), rnj = ir->nj();
  unsigned sbi = bif->size_block_i();
  unsigned sbj = bif->size_block_j();
  unsigned nbi = bif->n_block_i();
  if(rni%sbi!=0) nbi--;//discard partial blocks
  unsigned nbj = bif->n_block_j();
  if(rnj%sbj!=0) nbj--;//discard partial blocks
  view.set_size(ni, nj);  
  if(first){
    vil_image_view_base_sptr cblock_ptr = bif->get_block(bi, bj);
    cblock = vil_convert_cast(float(), cblock_ptr);
    cbi = bi; cbj =bj;
    first = false;
  }else if(bi!=cbi||bj!=cbj){
    vil_image_view_base_sptr cblock_ptr = bif->get_block(bi, bj);
    cblock = vil_convert_cast(float(), cblock_ptr);
    cbi = bi; cbj =bj;
  }
  for(unsigned j = 0; j<nj; ++j)
    for(unsigned i = 0; i<ni; ++i)
      view(i,j) = cblock(i0+i, j0+j);

  if(i0+ni>=sbi){
    i0 = 0;
    if(j0+nj>=sbj){
      if((bi+1)>=nbi){
        bi = 0;
        if((bj+1)>=nbj){
          done = true;
          vcl_cout << '\n' << vcl_flush;
          return true;
        }
        bj++; j0 = 0;
        vcl_cout << bj << ' '<< vcl_flush;
      }else{bi++; j0 = 0;}
    }else j0+=nj;
  }else i0+=ni;
  return true;
}
template <class T>
bool brad_eigenspace<T>::
update_histogram(vil_image_resource_sptr const& resc,
                 bsta_joint_histogram_3d<float>& hist)
{
  if(!eigensystem_valid_)
    return false;
  vcl_cout << "updating eigenvalue histogram\n";

  unsigned n = funct_.size();
  vnl_vector<double> v(n);
  vnl_vector<double> v0 = eigenvectors_.get_column(n-1);
  vnl_vector<double> v1 = eigenvectors_.get_column(n-2);
  vnl_vector<double> v2 = eigenvectors_.get_column(n-3);
  bool done = false;
  vil_image_view<float> fview;  
  while(!done)
    {
      if(!get_view(resc, nib_, njb_, fview, done))
        return false;
      vnl_vector<double> v = funct_(fview); 
      float eig0 = static_cast<float>(dot_product(v, v0));
      float eig1 = static_cast<float>(dot_product(v, v1));
      float eig2 = static_cast<float>(dot_product(v, v2));
      hist.upcount(eig0, 0.333f, eig1, 0.333f, eig2, 0.333f);
    }
  vcl_cout << '\n' << vcl_flush;
  return true;
}
#if 0//old implementation
template <class T>
bool brad_eigenspace<T>::
update_histogram(vil_image_resource_sptr const& resc,
                 bsta_joint_histogram_3d<float>& hist)
{
  if(!eigensystem_valid_)
    return false;
  vcl_cout << "updating eigenvalue histogram\n";

  unsigned n = funct_.size();
  vnl_vector<double> v(n);
  vnl_vector<double> v0 = eigenvectors_.get_column(n-1);
  vnl_vector<double> v1 = eigenvectors_.get_column(n-2);
  vnl_vector<double> v2 = eigenvectors_.get_column(n-3);
  unsigned ni = resc->ni(), nj = resc->nj();
  if(ni==0||nj==0||ni<nib_||nj<njb_) return false;
  unsigned nbi = ni/nib_, nbj = nj/njb_;
  unsigned i0 = 0, j0 = 0;
  for(unsigned r = 0; r<nbj; ++r, j0+=njb_){
    i0 = 0;
    for(unsigned c = 0; c<nbi; ++c, i0+=nib_)
      {
        vil_image_view_base_sptr view_ptr = 
          bif->get_view(i0, nib_, j0, njb_);
        vil_image_view<float> fview = vil_convert_cast(float(), view_ptr);
        vnl_vector<double> v = funct_(fview); 
        float eig0 = static_cast<float>(dot_product(v, v0));
        float eig1 = static_cast<float>(dot_product(v, v1));
        float eig2 = static_cast<float>(dot_product(v, v2));
        hist.upcount(eig0, 0.333f, eig1, 0.333f, eig2, 0.333f);
      }
    vcl_cout << '.'<< vcl_flush;
  }
  vcl_cout << '\n' << vcl_flush;
  return true;
}
#endif
template <class T>
bool brad_eigenspace<T>::
init_histogram(vcl_vector<vil_image_resource_sptr> const& rescs,
               unsigned nbins, bsta_joint_histogram_3d<float>& hist){
  if(!eigensystem_valid_)
    return false;
  vcl_cout << "intializing eigenvalue histogram\n";
  unsigned n = funct_.size();
  vnl_vector<double> v(n), minv(n), maxv(n);
  minv.fill(vnl_numeric_traits<float>::maxval);
  maxv.fill(0.0f);
  vnl_vector<double> v0 = eigenvectors_.get_column(n-1);
  vnl_vector<double> v1 = eigenvectors_.get_column(n-2);
  vnl_vector<double> v2 = eigenvectors_.get_column(n-3);
  for(vcl_vector<vil_image_resource_sptr>::const_iterator rit = rescs.begin();
      rit != rescs.end(); ++rit){
    unsigned ni = (*rit)->ni(), nj = (*rit)->nj();
    if(ni==0||nj==0||ni<nib_||nj<njb_) return false;
    unsigned nbi = ni/nib_, nbj = nj/njb_;
    unsigned i0 = 0, j0 = 0;
    for(unsigned r = 0; r<nbj; ++r, j0+=njb_){
      i0 = 0;
      for(unsigned c = 0; c<nbi; ++c, i0+=nib_)
        {
          vil_image_view_base_sptr view_ptr = 
            (*rit)->get_view(i0, nib_, j0, njb_);
          vil_image_view<float> fview = vil_convert_cast(float(), view_ptr);
          vnl_vector<double> v = funct_(fview); 
          float eig0 = static_cast<float>(dot_product(v, v0));
          float eig1 = static_cast<float>(dot_product(v, v1));
          float eig2 = static_cast<float>(dot_product(v, v2));
          if(eig0<minv[0]) minv[0]=eig0; if(eig1<minv[1]) minv[1]=eig1;
          if(eig2<minv[2]) minv[2]=eig2;
          if(eig0>maxv[0]) maxv[0]=eig0; if(eig1>maxv[1]) maxv[1]=eig1;
          if(eig2>maxv[2]) maxv[2]=eig2;
        }
      vcl_cout << '.'<< vcl_flush;
    }
    vcl_cout << '\n' << vcl_flush;
  }
  vnl_vector<double> delta = (maxv-minv)/static_cast<float>(nbins);
  float min0 = static_cast<float>(minv[0]-delta[0]);
  float max0 = static_cast<float>(maxv[0]+delta[0]);
  float min1 = static_cast<float>(minv[1]-delta[1]);
  float max1 = static_cast<float>(maxv[1]+delta[1]);
  float min2 = static_cast<float>(minv[2]-delta[2]);
  float max2 = static_cast<float>(maxv[2]+delta[2]);
  hist = bsta_joint_histogram_3d<float>(min0, max0, nbins,
                                        min1, max1, nbins,
                                        min2, max2, nbins);
  return true;
}

template <class T>
bool brad_eigenspace<T>::
update_histogram(vcl_vector<vil_image_resource_sptr> const& rescs,
                 bsta_joint_histogram_3d<float>& hist){
  for(vcl_vector<vil_image_resource_sptr>::const_iterator rit = rescs.begin();
      rit != rescs.end(); ++rit)
    if(!this->update_histogram((*rit), hist))
      return false;
  return true;
}
template <class T>
bool brad_eigenspace<T>::
init_histogram_rand(vcl_vector<vil_image_resource_sptr> const& rescs,
                    unsigned nbins,
                    bsta_joint_histogram_3d<float>& hist,
                    double frac, unsigned nit, unsigned njt){
  if(!eigensystem_valid_)
    return false;
  vcl_vector<vil_image_resource_sptr>::const_iterator rit = rescs.begin();
  double area = 0.0;
  for(unsigned i = 0; rit!= rescs.end(); ++rit, ++i){
    unsigned ni = (*rit)->ni(), nj = (*rit)->nj();
    if(ni<nib_||nj<njb_)
      return false;
    area += ni*nj;
  }
  //the total image area to be processed
  double proc_area = area*frac;
  // the number of random tiles to be processed
  double nitd = nit, njtd = njt;
  unsigned ntiles = static_cast<unsigned>(proc_area/(nitd*njtd));
  if(!ntiles) return false;
  unsigned n = funct_.size();
  if(!n) return false;
  vcl_cout << "initializing histogram - randomly selecting " 
           << ntiles << " (" << nit << 'x' << njt << ") tiles\n" << vcl_flush;
  print_resource_stats(rescs);

  vnl_vector<double> minv(n), maxv(n);
  minv.fill(vnl_numeric_traits<float>::maxval);
  maxv.fill(0.0f);
  vnl_vector<double> v0 = eigenvectors_.get_column(n-1);
  vnl_vector<double> v1 = eigenvectors_.get_column(n-2);
  vnl_vector<double> v2 = eigenvectors_.get_column(n-3);
  // process the tiles
  double nd = rescs.size();
  for(unsigned t = 0; t<ntiles; ++t){
    //randomly select a resource
    unsigned ires = 
      static_cast<unsigned>((nd)*(vcl_rand()/(RAND_MAX+1.0)));  
    //compute random access to tile
    double nid = rescs[ires]->ni(), njd = rescs[ires]->nj();
    double rd = (njd-njtd-1.0)*(vcl_rand()/(RAND_MAX+1.0));
    double cd = (nid-nitd-1.0)*(vcl_rand()/(RAND_MAX+1.0));
    if(rd<0) rd = 0.0;
    if(cd<0) cd = 0.0;
    if(rd>(njd-njtd-1.0)) rd = (njd-njtd-1.0);
    if(cd>(nid-nitd-1.0)) cd = (nid-nitd-1.0);
    unsigned j0 = static_cast<unsigned>(rd), i0 = static_cast<unsigned>(cd);
    vil_image_view_base_sptr tile_ptr = 
      rescs[ires]->get_view(i0, nit, j0, njt);
    vil_image_view<float> ftile = vil_convert_cast(float(), tile_ptr);
    unsigned nbi = nit/nib_, nbj = njt/njb_;
    i0 = 0; j0=0;
    for(unsigned r = 0; r<nbj; ++r, j0+=njb_){
      i0 = 0;
      for(unsigned c = 0; c<nbi; ++c, i0+=nib_){
        vil_image_view<float> fview(nib_, njb_);
        for(unsigned j =0; j<njb_; ++j)
          for(unsigned i =0; i<nib_; ++i)
            fview(i, j) = ftile(i0+i, j0+j);
        vnl_vector<double> v = funct_(fview);
        float eig0 = static_cast<float>(dot_product(v, v0));
        float eig1 = static_cast<float>(dot_product(v, v1));
        float eig2 = static_cast<float>(dot_product(v, v2));
        if(eig0<minv[0]) minv[0]=eig0; if(eig1<minv[1]) minv[1]=eig1;
        if(eig2<minv[2]) minv[2]=eig2;
        if(eig0>maxv[0]) maxv[0]=eig0; if(eig1>maxv[1]) maxv[1]=eig1;
        if(eig2>maxv[2]) maxv[2]=eig2;
      }
    }
    vcl_cout << '.' << vcl_flush;
  }
  vnl_vector<double> delta = (maxv-minv)/static_cast<float>(nbins);
  float min0 = static_cast<float>(minv[0]-delta[0]);
  float max0 = static_cast<float>(maxv[0]+delta[0]);
  float min1 = static_cast<float>(minv[1]-delta[1]);
  float max1 = static_cast<float>(maxv[1]+delta[1]);
  float min2 = static_cast<float>(minv[2]-delta[2]);
  float max2 = static_cast<float>(maxv[2]+delta[2]);
  hist = bsta_joint_histogram_3d<float>(min0, max0, nbins,
                                        min1, max1, nbins,
                                        min2, max2, nbins);
  vcl_cout << '\n' << vcl_flush;
  return true;
}
template <class T>
bool brad_eigenspace<T>::
update_histogram_rand(vcl_vector<vil_image_resource_sptr> const& rescs,
                      bsta_joint_histogram_3d<float>& hist,
                      double frac, unsigned nit, unsigned njt){
  if(!eigensystem_valid_)
    return false;
  vcl_vector<vil_image_resource_sptr>::const_iterator rit = rescs.begin();
  double area = 0.0;
  for(unsigned i = 0; rit!= rescs.end(); ++rit, ++i){
    unsigned ni = (*rit)->ni(), nj = (*rit)->nj();
    if(ni<nib_||nj<njb_)
      return false;
    area += ni*nj;
  }
  //the total image area to be processed
  double proc_area = area*frac;
  // the number of random tiles to be processed
  double nitd = nit, njtd = njt;
  unsigned ntiles = static_cast<unsigned>(proc_area/(nitd*njtd));
  if(!ntiles) return false;

  unsigned n = funct_.size();
  if(!n) return false;
  vcl_cout << "updating histogram - randomly selecting " 
           << ntiles << " (" << nit << 'x' << njt << ") tiles\n" << vcl_flush;
  print_resource_stats(rescs);

  vnl_vector<double> v0 = eigenvectors_.get_column(n-1);
  vnl_vector<double> v1 = eigenvectors_.get_column(n-2);
  vnl_vector<double> v2 = eigenvectors_.get_column(n-3);
  // process the tiles
  double nd = rescs.size();
  for(unsigned t = 0; t<ntiles; ++t){
    //randomly select a resource
    unsigned ires = 
      static_cast<unsigned>((nd)*(vcl_rand()/(RAND_MAX+1.0)));  
    //compute random access to tile
    double nid = rescs[ires]->ni(), njd = rescs[ires]->nj();
    double rd = (njd-njtd-1.0)*(vcl_rand()/(RAND_MAX+1.0));
    double cd = (nid-nitd-1.0)*(vcl_rand()/(RAND_MAX+1.0));
    if(rd<0) rd = 0.0;
    if(cd<0) cd = 0.0;
    if(rd>(njd-njtd-1.0)) rd = (njd-njtd-1.0);
    if(cd>(nid-nitd-1.0)) cd = (nid-nitd-1.0);
    unsigned j0 = static_cast<unsigned>(rd), i0 = static_cast<unsigned>(cd);
    vil_image_view_base_sptr tile_ptr = 
      rescs[ires]->get_view(i0, nit, j0, njt);
    vil_image_view<float> ftile = vil_convert_cast(float(), tile_ptr);
    unsigned nbi = nit/nib_, nbj = njt/njb_;
    i0 = 0; j0=0;
    for(unsigned r = 0; r<nbj; ++r, j0+=njb_){
      i0 = 0;
      for(unsigned c = 0; c<nbi; ++c, i0+=nib_){
        vil_image_view<float> fview(nib_, njb_);
        for(unsigned j =0; j<njb_; ++j)
          for(unsigned i =0; i<nib_; ++i)
            fview(i, j) = ftile(i0+i, j0+j);
        vnl_vector<double> v = funct_(fview);
        float eig0 = static_cast<float>(dot_product(v, v0));
        float eig1 = static_cast<float>(dot_product(v, v1));
        float eig2 = static_cast<float>(dot_product(v, v2));
        hist.upcount(eig0, 0.333f, eig1, 0.333f, eig2, 0.333f);
      }
    }
    vcl_cout << '.' << vcl_flush;
  }
  vcl_cout << '\n' << vcl_flush;
  return true;
}

template <class T>
void vsl_b_write(vsl_b_ostream &os, const brad_eigenspace<T>& ep)
{
  vsl_b_write(os, ep.nib());
  vsl_b_write(os, ep.njb());
  vsl_b_write(os, ep.mean());
  vsl_b_write(os, ep.covariance());
  vsl_b_write(os, ep.eigenvalues());
  vsl_b_write(os, ep.eigenvectors());
  vsl_b_write(os, ep.functor());
}

//: Binary load brad_eigenspace from stream.
template <class T>
void vsl_b_read(vsl_b_istream &is, brad_eigenspace<T>& ep)
{
  unsigned nib, njb;
  vnl_vector<double> mean, eigenvals;
  vnl_matrix<double> covar, eigenvecs;
  T funct;
  vsl_b_read(is, nib);
  vsl_b_read(is, njb);
  vsl_b_read(is, mean);
  vsl_b_read(is, covar);
  vsl_b_read(is, eigenvals);
  vsl_b_read(is, eigenvecs);
  vsl_b_read(is, funct);
  ep.set_nib(nib); ep.set_njb(njb);
  ep.set_mean_covar(mean, covar);
  ep.set_eigensystem(eigenvals, eigenvecs);
  ep.set_functor(funct);
}
template <class T>
void brad_eigenspace<T>::print(vcl_ostream& os) const{
  os << "image block size(" << nib_ << ' ' << njb_ << ")\n";
  os << "feature vector size " << funct_.size() << '\n';
  os << "feature vector type [" << funct_.type() << "]\n";
}
//: Print summary
template <class T>
void 
vsl_print_summary(vcl_ostream &os, const brad_eigenspace<T>& ep)
{
  ep.print(os);
}

#undef BRAD_EIGENSPACE_INSTANTIATE
#define BRAD_EIGENSPACE_INSTANTIATE(T) \
template class brad_eigenspace<T > ; \
template void vsl_print_summary(vcl_ostream&, const brad_eigenspace<T>& ); \
template void vsl_b_read(vsl_b_istream&, brad_eigenspace<T>&); \
template void vsl_b_write(vsl_b_ostream &os, const brad_eigenspace<T>&)
#endif // brad_eigenspace_txx_
