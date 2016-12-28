#include "sdet_region_classifier.h"
#include "sdet_graph_img_seg.h"
#include <functional>
#include <algorithm>
#include <vul/vul_timer.h>
void sdet_region_classifier::find_iou_clusters(){
  vul_timer t;
  // insert regions in the rtree
  std::map<unsigned, sdet_region_sptr>::const_iterator rit = diverse_regions_.begin();
  for(;rit != diverse_regions_.end(); ++rit){
    vgl_box_2d<float> bb =(*rit).second->bbox();
    if(!bb.is_empty())
      tr_.add(rit->second);
  }    
  std::cout << "insert in rtree in " << t.real() << " msec" << std::endl;
  t.mark();
  // find sets of high int over union regions
  float iou_thresh = 0.9f;
  rit = diverse_regions_.begin();
  for(;rit != diverse_regions_.end(); ++rit){
    vgl_box_2d<float> bb =(*rit).second->bbox();
    unsigned lab = (*rit).second->label();
    std::vector<sdet_region_sptr> intersecting_regions;
    tr_.get(bb, intersecting_regions);
    std::map<unsigned, float> iou_index;
    for(std::vector<sdet_region_sptr>::iterator iit = intersecting_regions.begin();
        iit != intersecting_regions.end(); ++iit){
      if((*iit)->label() == lab)
        continue;
      float iou = (*iit)->int_over_union(bb);
      if(iou>iou_thresh)
        iou_index[(*iit)->label()]=iou;
    }
    if(iou_index.size())
      iou_clusters_[lab]=iou_index;
  }
  std::cout << "find IOU clusters in " << t.real() << " msec" << std::endl;
}
void sdet_region_classifier::remove_diverse_region(unsigned label){
  std::map<unsigned, sdet_region_sptr>::iterator dit =  diverse_regions_.find(label);
  if(dit != diverse_regions_.end())
    diverse_regions_.erase(dit);
  std::map<unsigned, bsta_histogram<float> >::iterator hit = diverse_hists_.find(label);
  if(hit != diverse_hists_.end())
    diverse_hists_.erase(hit);
}
void sdet_region_classifier::remove_identical_regions(){
  vul_timer t;
  std::set<unsigned> to_remove;
  for(std::map< unsigned, std::map<unsigned, float> >::iterator iit = iou_clusters_.begin();
      iit != iou_clusters_.end(); ++iit){
    std::set<unsigned>::iterator rit =  to_remove.find(iit->first);
    if(rit != to_remove.end())
        continue;
    std::map<unsigned, float>& ious = iit->second;
    for(std::map<unsigned, float>::iterator sit = ious.begin(); sit != ious.end(); ++sit)
      to_remove.insert(sit->first);
  }
  for(std::set<unsigned>::iterator rmit =  to_remove.begin();
      rmit != to_remove.end(); ++rmit){
    this->remove_diverse_region(*rmit);
  }
 std::cout << "remove identical regions in " << t.real() << " msec" << std::endl;
}

void sdet_region_classifier::compute_hist_of_nbrs(){
  for(std::map<unsigned, sdet_region_sptr>::iterator rit = diverse_regions_.begin();
      rit != diverse_regions_.end(); ++rit){
    unsigned lab = rit->first;
    const std::set<unsigned>& nbrs = (*rit).second->nbrs();
    std::set<unsigned>::const_iterator nit = nbrs.begin();
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

  for(std::map<unsigned, bsta_histogram<float> >::iterator hit = diverse_hists_.begin();
      hit != diverse_hists_.end(); ++hit){
    unsigned lab = hit->first;
    bsta_histogram<float>& hr = hit->second;
    bsta_histogram<float>& hn = neighbors_hists_[lab];
    float hr_median = hr.value_with_area_above(0.5f);//median
    float hn_median = hn.value_with_area_above(0.5f);
    float delta = 0.025*(hr_median + hn_median);
    if(hr_median>(hn_median+delta))
      bright_regions_.insert(lab);
  }
}
