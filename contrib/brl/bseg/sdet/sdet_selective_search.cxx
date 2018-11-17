#include <functional>
#include <algorithm>
#include "sdet_selective_search.h"
#include "sdet_graph_img_seg.h"
#include <vul/vul_timer.h>
static float log_map(float x){return 45.9859044f*std::log(x+1.0f);}
void sdet_selective_search::initialize_hists(){
  // initialize histograms
  if(!initial_regions_valid_)
    return;
  vul_timer t;
  std::map<unsigned, sdet_region_sptr>& regions= regions_;
  for(auto & region : regions_){
    unsigned lab = region.second->label();
    region.second->ComputeIntensityStdev();
    bsta_histogram<float> h(255.0f, nbins_);
    for(region.second->reset(); region.second->next();){
      auto v = static_cast<float>(region.second->I());
      v = log_map(v);
      h.upcount(v, 1.0f);
    }
    hists_[lab]=h;
  }
  std::cout << "Initialize hists " << t.real() << "msecs" << std::endl;
}

void sdet_selective_search::find_max_label(){
  //set max_label
  max_label_ = 0;
  for(auto & region : regions_)
    if(region.second->label() >max_label_)
      max_label_ = region.second->label();
}

void sdet_selective_search::extract_initial_regions(){
  if(byte_view_.ni()==0 || byte_view_.nj()==0){
    std::cout << "Null image view " << std::endl;
    return;
  }
  vul_timer t;
  sdet_segment_img_using_VD_edges<vxl_byte>(byte_view_, margin_, four_or_eight_conn_, weight_thresh_,
                                            sigma_, vd_noise_mul_, min_region_size_, color_region_view_, regions_,use_vd_edges_);
  std::cout << "extract initial regions " << t.real() << "msecs" << std::endl;
  initial_regions_valid_ = true;
}

void sdet_selective_search::compute_initial_similarity(){
  vul_timer t;
  std::map<unsigned, bsta_histogram<float> >& hists= hists_;
  std::set< std::pair<unsigned, unsigned> > sim_pair_computed;
  for(auto & region : regions_){
    sdet_region_sptr& ri = region.second;
    unsigned labi = ri->label();
    bsta_histogram<float>& histi = hists[labi];
    //float npix_i = static_cast<float>(ri->Npix());
    const std::set<unsigned>& nbrs = ri->nbrs();
    for(const auto & nbr : nbrs){
      std::pair<unsigned, unsigned> p(labi,nbr);
      auto pit = sim_pair_computed.find(p);
      if(pit != sim_pair_computed.end())
        continue; // pair already computed
      sdet_region_sptr& rnbr = regions_[nbr];
      if(!rnbr) continue;//null region - shouldn't happen
      std::pair<unsigned, unsigned> pi(labi,nbr);
      std::pair<unsigned, unsigned> pn(nbr,labi);
      sim_pair_computed.insert(pi);//record pair in either order
      sim_pair_computed.insert(pn);
      //float npix_j = static_cast<float>(rnbr->Npix());
      bsta_histogram<float>& histj = hists[nbr];
      // This computation of similarity is based on Uijlings et al
      // and merges small, similarly sized regions before large regions
      // that have similar intensity distributions
      float st = similarity(ri, histi, rnbr, histj, image_area_);
      sim_.push(region_sim(labi, nbr, st));//max similarity always at top of queue
    }
  }
  std::cout << "compute initial similarity " << t.real() << "msecs" << std::endl;
}
void sdet_selective_search::insert_similarities(const sdet_region_sptr& newr, std::set<unsigned>& removed_labels){
  unsigned lab = newr->label();
  const std::set<unsigned>& nbrs = newr->nbrs();
  //float npix_n = static_cast<float>(newr->Npix());
  bsta_histogram<float>& hr = hists_[lab];
  for(const auto & nbr : nbrs){
    //check if neighbor is already removed
    auto rit = removed_labels.find(nbr);
    if(rit != removed_labels.end())
      continue;
    sdet_region_sptr& rnbr = regions_[nbr];
    if(!rnbr) continue;//null region - shouldn't happen
    //float npix_nb = static_cast<float>(rnbr->Npix());
    auto hitn = hists_.find(nbr);
    if(hitn == hists_.end())
      continue;//hist not found
    bsta_histogram<float>& hn = hitn->second;
    // This computation of similarity is based on Uijlings et al
    // and merges small, similarly sized regions before large regions
    // that have similar intensity distributions
    //float sim_hist = hist_intersect(hr, hn);//min probability
    //float sim_size = 1.0f - (npix_n + npix_nb)/image_area_;//small regions first
    //float st = sim_hist + sim_size;
    float st = similarity(newr, hr, rnbr, hn, image_area_);
    sim_.push(region_sim(lab, nbr, st));//max similarity always at top of queue
  }
}
void sdet_selective_search::merge_regions(){
  vul_timer t;
  unsigned new_label = max_label_+1;
  std::set<unsigned> removed_labels;
  std::map<unsigned, unsigned> equivalent_labels;
  while(!sim_.empty()){
    region_sim sim = sim_.top();
    //check if similarity is irrelevant since is with respect to already removed labels
    auto rlit = removed_labels.find(sim.ri_);
    if(rlit != removed_labels.end()){//found the label ri
      sim_.pop();
      continue;
    }else{
      rlit = removed_labels.find(sim.rj_);
      if(rlit != removed_labels.end()){//found the label rj
        sim_.pop();
        continue;
      }
    }
    //a new pair to merge
    const sdet_region_sptr& ri = regions_[sim.ri_];
    const sdet_region_sptr& rj = regions_[sim.rj_];
    sdet_region_sptr rij = merge(ri, rj, new_label++);
    if(!rij){
      std::cout << "Warning null region in merge " << std::endl;
      continue;
    }
    bsta_histogram<float> hm;
    merge_hists(hists_[sim.ri_], hists_[sim.rj_], hm);
    unsigned lij = rij->label();
    regions_[lij] = rij;
    hists_[lij] = hm;
    removed_labels.insert(sim.ri_);
    removed_labels.insert(sim.rj_);
    equivalent_labels[sim.ri_]=lij;
    equivalent_labels[sim.rj_]=lij;
    // remove dead labels from new region's neighbors
    // temp storage of dead labels is needed since set iterator is
    // undefined when an element is erased.
    std::vector<unsigned> dead_nbrs;
    const std::set<unsigned>& nbrs = rij->nbrs();
    for(const auto &nbr : nbrs){
      auto rit = removed_labels.find(nbr);
      if(rit != removed_labels.end())
        dead_nbrs.push_back(nbr);
    }
    for(unsigned int & dead_nbr : dead_nbrs){
      rij->remove_neighbor(dead_nbr);
      auto eit = equivalent_labels.find(dead_nbr);
      if(eit !=equivalent_labels.end())
        rij->add_neighbor(eit->second);
    }
    sim_.pop();
    this->insert_similarities(rij, removed_labels);
  }
  diverse_max_label_ += new_label;
  max_label_ = new_label;
  std::cout << "merge regions " << t.real() << "msecs" << std::endl;
}
void sdet_selective_search::update_region_labels(){
  std::map<unsigned, sdet_region_sptr> regions;
  unsigned delta = diverse_max_label_ - max_label_;
  if(delta == 0){//just transfer to diverse no change in labels
    for(auto & region : regions_){
      sdet_region_sptr& r = region.second;
      unsigned lab = r->label();
      diverse_regions_[lab]=r;
      auto hit = hists_.find(lab);
      if(hit != hists_.end()){
        bsta_histogram<float>& h = hit->second;
        diverse_hists_[lab] = h;
      }
    }
  }
  for(auto & region : regions_){
    sdet_region_sptr& r = region.second;
    unsigned lab = r->label();
    unsigned new_lab = lab + delta;
    r->increment_neighbors(delta);
    r->set_label(new_lab);
    diverse_regions_[new_lab]=r;
    auto hit = hists_.find(lab);
    if(hit != hists_.end()){
      bsta_histogram<float>& h = hit->second;
      diverse_hists_[new_lab] = h;
    }
  }
}
void sdet_selective_search::create_color_region_view(unsigned min_region_area,
                                                     unsigned max_region_area){
  unsigned n = regions_.size();
  std::vector<vil_rgb<vxl_byte> > colors;
  sdet_graph_img_seg::create_colors(colors, n);
  vil_image_view<vil_rgb<vxl_byte> > color_view(byte_view_.ni(), byte_view_.nj());
  vil_rgb<vxl_byte> black((vxl_byte)0, (vxl_byte)0, (vxl_byte)0);
  color_view.fill(black);
  unsigned k = 0;
  for(auto rit = regions_.begin();
      rit != regions_.end(); ++rit, ++k){
    sdet_region_sptr& ri = (*rit).second;
    if(ri->Npix()<min_region_area || ri->Npix()>max_region_area)
      continue;
    vil_rgb<vxl_byte> c = colors[k];
        std::cout << "label " << ri->label() << " " << c << std::endl;
    for(ri->reset(); ri->next();){
      auto u = static_cast<unsigned>(ri->X()), v = static_cast<unsigned>(ri->Y());
      color_view(u, v) = c;
    }
  }
  color_region_view_=color_view;
}
void sdet_selective_search::save_bboxes(std::ostream& ostr){
  for(auto & region : regions_){
    const sdet_region_sptr& r = region.second;
    vgl_box_2d<float> bbox = r->bbox();
    bbox.write(ostr);
  }
}
