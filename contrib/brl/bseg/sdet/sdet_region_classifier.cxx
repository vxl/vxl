#include <functional>
#include <algorithm>
#include <limits>
#include <cmath>
#include "sdet_region_classifier.h"
#include "sdet_graph_img_seg.h"
#include <vul/vul_timer.h>
#if 0
void sdet_region_classifier::find_iou_clusters(const std::map<unsigned, sdet_region_sptr >& regions){
  vul_timer t;
  vgl_rtree<V_, B_, C_> tr; // the rtree
  // insert regions in the rtree
  std::map<unsigned, sdet_region_sptr>::const_iterator rit = regions.begin();
  for(;rit != regions.end(); ++rit){
    vgl_box_2d<float> bb =(*rit).second->bbox();
    if(!bb.is_empty())
      tr.add(rit->second);
  }
  std::cout << "insert in rtree in " << t.real() << " msec" << std::endl;
  t.mark();
  // find sets of high int over union regions
  float iou_thresh = 0.5f;
  rit = regions.begin();
  for(;rit != regions.end(); ++rit){
    vgl_box_2d<float> bb =(*rit).second->bbox();
    unsigned lab = (*rit).second->label();
    std::vector<sdet_region_sptr> intersecting_regions;
    tr.get(bb, intersecting_regions);
    std::map<unsigned, float> iou_index;
    for(std::vector<sdet_region_sptr>::iterator iit = intersecting_regions.begin();
        iit != intersecting_regions.end(); ++iit){
      if((*iit)->label() == lab){
        iou_index[lab]=1.0f;
        continue;
      }
      float iou = (*iit)->int_over_min_area(bb); //JLM
      if(iou>iou_thresh)
        iou_index[(*iit)->label()]=iou;
    }
    if(iou_index.size())
      iou_clusters_[lab]=iou_index;
  }
  std::cout << "find IOU clusters in " << t.real() << " msec" << std::endl;
}
#endif
void sdet_region_classifier::find_iou_clusters(const std::map<unsigned, sdet_region_sptr >& regions){
  vul_timer t;
  vgl_rtree<V_, B_, C_> tr; // the rtree
  // insert regions in the rtree
  auto rit = regions.begin();
  for(;rit != regions.end(); ++rit){
    vgl_box_2d<float> bb =(*rit).second->bbox();
    if(!bb.is_empty())
      tr.add(rit->second);
  }
  std::cout << "insert in rtree in " << t.real() << " msec" << std::endl;
  t.mark();
  // find sets of high int over union regions
  std::set<unsigned> already_clustered;
  float iou_thresh = 0.25f;
  rit = regions.begin();
  for(;rit != regions.end(); ++rit){
    vgl_box_2d<float> bb =(*rit).second->bbox();
    unsigned lab = (*rit).second->label();
    auto ait = already_clustered.find(lab);
    if(ait != already_clustered.end())
      continue;
    std::vector<sdet_region_sptr> intersecting_regions;
    tr.get(bb, intersecting_regions);
    std::map<unsigned, float> iou_index;
    for(auto & intersecting_region : intersecting_regions){
      if(intersecting_region->label() == lab){
        iou_index[lab]=1.0f;
        already_clustered.insert(lab);
        continue;
      }
      unsigned labi = intersecting_region->label();
      ait = already_clustered.find(labi);
      if(ait != already_clustered.end())
        continue;
      float iou = intersecting_region->int_over_min_area(bb); //JLM
      if(iou>iou_thresh){
        iou_index[labi]=iou;
        already_clustered.insert(labi);
      }
    }
    if(iou_index.size())
      iou_clusters_[lab]=iou_index;
  }
  std::cout << "find IOU clusters in " << t.real() << " msec" << std::endl;
}
void sdet_region_classifier::compute_iou_cluster_similarity(){
  for(auto cit0 =  iou_clusters_.begin();
      cit0 != iou_clusters_.end(); ++cit0){
    unsigned lab0 = cit0->first;
    const std::map<unsigned, float>& iou_index0 = cit0->second;
    std::map<unsigned, region_sim> sim_map;
    for(auto cit1 =  cit0;
        cit1 != iou_clusters_.end(); ++cit1){
      unsigned lab1 = cit1->first;
      if(lab0 == lab1)
        continue;//skip equal labels
      const std::map<unsigned, float>& iou_index1 = cit1->second;
      //if iou_index1 contains lab0 then skip the entire cluster (for now)
      auto iit = iou_index1.find(lab0);
      if(iit != iou_index1.end())
        continue;
      // see if clusters intersect
      bool intersect = false;
      for(auto iit0 = iou_index0.begin();
          iit0 != iou_index0.end()&&!intersect; ++iit0){
        unsigned labi0 = iit0->first;
        for(auto iit1 = iou_index1.begin();
            iit1 != iou_index1.end()&&!intersect; ++iit1){
          unsigned labi1 = iit1->first;
          if(labi1 == labi0)
            intersect = true;
        }
      }
      if(intersect)
        continue;
      // iterate through the clusters and find the most similar region pair
      float max_s = 0.0f;
      region_sim max_rsim;
      for(auto iit0 : iou_index0){
        unsigned labi0 = iit0.first;
        const sdet_region_sptr& r0 = diverse_regions_[labi0];
        const bsta_histogram<float> h0 = diverse_hists_[labi0];
        for(auto iit1 : iou_index1){
          unsigned labi1 = iit1.first;
          const sdet_region_sptr& r1 = diverse_regions_[labi1];
          const bsta_histogram<float>& h1 = diverse_hists_[labi1];
          float s = similarity(r0, h0, r1, h1);
          if(s>max_s){
            max_s = s;
            max_rsim.ri_= labi0;
            max_rsim.rj_= labi1;
            max_rsim.s_ = max_s;
          }
        }
      }
      sim_map[lab1]=max_rsim;
    }
         region_sim self_rsim;
         self_rsim.ri_ = lab0;
         self_rsim.rj_ = lab0;
         self_rsim.s_ = 2.0f;
    sim_map[lab0]=self_rsim;
    cluster_similarity_[lab0]=sim_map;
  }
}
void sdet_region_classifier::remove_diverse_region(unsigned label){
  auto dit =  diverse_regions_.find(label);
  if(dit != diverse_regions_.end())
    diverse_regions_.erase(dit);
  auto hit = diverse_hists_.find(label);
  if(hit != diverse_hists_.end())
    diverse_hists_.erase(hit);
}

void sdet_region_classifier::compute_hist_of_nbrs(){
  for(auto & diverse_region : diverse_regions_){
    unsigned lab = diverse_region.first;
    const std::set<unsigned>& nbrs = diverse_region.second->nbrs();
    auto nit = nbrs.begin();
    bsta_histogram<float> hn = diverse_hists_[*nit]; ++nit;
    while(nit != nbrs.end()){
      if(*nit == lab)
        continue;
      bsta_histogram<float>& hi = diverse_hists_[*nit];
      if(!merge_hists(hi, hn, hn)){
        std::cout << "compute hist of neighbors -- failed! " << *nit << std::endl;
        return;
      }
      ++nit;
    }
    neighbors_hists_[lab] = hn;
  }
}
void sdet_region_classifier::compute_bright_regions(){

  for(auto & diverse_hist : diverse_hists_){
    unsigned lab = diverse_hist.first;
    bsta_histogram<float>& hr = diverse_hist.second;
    bsta_histogram<float>& hn = neighbors_hists_[lab];
    float hr_median = hr.value_with_area_above(0.5f);//median
    float hn_median = hn.value_with_area_above(0.5f);
    float delta = 0.025*(hr_median + hn_median);
    if(hr_median>(hn_median+delta))
      bright_regions_.insert(lab);
  }
}
float sdet_region_classifier::
compute_partition_quality(std::map< unsigned, std::map<unsigned, region_sim> > const& cluster_sim){
  auto nf = static_cast<float>(cluster_sim.size());
  float neu_sum = 0.0f, den_sum = 0.0f;
  for(const auto & sit : cluster_sim){
    unsigned lab = sit.first;
    const std::map<unsigned, region_sim>& rsim = sit.second;
    for(auto rit : rsim)
      if(rit.first == lab)
        neu_sum += rit.second.s_;
      else
        den_sum += rit.second.s_;
  }
  float q = (nf-1.0f)*neu_sum/den_sum;
  return q;
}
bool sdet_region_classifier::merge_similarity_map(std::map< unsigned, std::map<unsigned, region_sim> > const& sim_before,
                       std::map< unsigned, std::map<unsigned, region_sim> >& sim_after,
                       unsigned labi, unsigned labj, unsigned new_label){
  auto sit = sim_before.find(labi);
  if(sit == sim_before.end()){
    std::cout << labi << " not in similarity matrix" << std::endl;
    return false;
  }
  sit = sim_before.find(labj);
  if(sit == sim_before.end()){
    std::cout << labj << " not in similarity matrix" << std::endl;
    return false;
  }
  sit = sim_before.find(new_label);
  if(sit != sim_before.end()){
    std::cout << new_label << " not unique" << std::endl;
    return false;
  }
  // find similarities to both labi and labj
  std::map<unsigned, region_sim> simi, simj;
  for(const auto & sit : sim_before){
        unsigned lab = sit.first;
        const std::map<unsigned, region_sim>& rsim = sit.second;
    if(lab == labi){
      for(auto itr : rsim){
        unsigned labr = itr.first;
        simi[labr] = itr.second;
        if(labr == labj)
          simj[labr] = itr.second;
      }
    }else if(lab == labj){
      for(auto itr : rsim){
        unsigned labr = itr.first;
        simj[labr] = itr.second;
        if(labr == labi)
          simi[labr] = itr.second;
      }
    }else{

        for(auto itr : rsim){
          unsigned labr = itr.first;
          if(labr == labi)
            simi[lab] = itr.second;
          if(labr == labj)
            simj[lab] = itr.second;
        }
      }
    }
  //fill out the similarity table except for the merged rows and cols
  //assume that the merged label will be at the last row and col
  for(const auto & sit : sim_before){
    unsigned lab = sit.first;
    std::map<unsigned, region_sim> temp;
    const std::map<unsigned, region_sim>& rsim = sit.second;
    // skip the merged rows
    if(lab == labi || lab == labj)
      continue;
    for(auto rit : rsim){
      unsigned labr = rit.first;
      // skip the merged cols
      if(labr == labi||labr == labj)
        continue;
      temp[labr] = rit.second;
    }
    sim_after[lab]=temp;
  }

  // fill in the  last row (new_label)
  std::map<unsigned, region_sim> temp;
  region_sim& rsimi = simj[labi];
  region_sim& rsimj = simi[labj];
  if(rsimi.s_>rsimj.s_)
    temp[new_label]=rsimi;
  else
    temp[new_label]=rsimj;
  sim_after[new_label] = temp;
  // fill in the last column (new_label)
  for(auto riti = simi.begin();
      riti != simi.end(); ++riti){
    unsigned labri = riti->first;
    if(labri == labi)
      continue;
    for(auto ritj = simj.begin();
        ritj != simj.end(); ++ritj){
          unsigned labrj = ritj->first;
      if(labrj == labj)
        continue;
      if(labri == labrj){
        std::map<unsigned, region_sim>& rsim = sim_after[labri];
        region_sim& rsimi = simj[labri];
        region_sim& rsimj = simi[labrj];
        if(rsimi.s_>rsimj.s_)
          rsim[new_label]=rsimi;
        else
          rsim[new_label]=rsimj;
      }
    }
  }
 return true;
}
