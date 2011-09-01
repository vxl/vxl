from boxm2_adaptor import *; 
import random, os, sys;  
from numpy import *; 
import pylab  
  
#calculates pixelwise ROC curve, and estimated area under ROC curve
def calc_roc(cd_img, gt_img, plot = False, mask_img = None) : 

  boxm2_batch.init_process("vilPixelwiseRocProcess");
  boxm2_batch.set_input_from_db(0,cd_img);
  boxm2_batch.set_input_from_db(1,gt_img);
  if mask_img : boxm2_batch.set_input_from_db(2,mask_img); 
  boxm2_batch.run_process();
  (id,type) = boxm2_batch.commit_output(0);
  tp_temp = boxm2_batch.get_bbas_1d_array_float(id);
  (id,type) = boxm2_batch.commit_output(1);
  tn_temp = boxm2_batch.get_bbas_1d_array_float(id);
  (id,type) = boxm2_batch.commit_output(2);
  fp_temp = boxm2_batch.get_bbas_1d_array_float(id);
  (id,type) = boxm2_batch.commit_output(3);
  fn_temp = boxm2_batch.get_bbas_1d_array_float(id);	
  
  #convert to numpy arrays
  tps = array(tp_temp, float); 
  tns = array(tn_temp, float); 
  fps = array(fp_temp, float); 
  fns = array(fn_temp, float); 
  
  tp_rate = tps / (tps + fns); 
  fp_rate = fps / (fps + tns); 
  
  #append points 0,0 and 1,1 (if they are not there already)    
  if tp_rate[0] != 1 and fp_rate[0] != 1 : 
    tp_rate = concatenate( (array([1.0]), tp_rate) ) ; 
    fp_rate = concatenate( (array([1.0]), fp_rate) ) ; 
  
  ilast = len(tp_rate)-1; 
  if tp_rate[ilast] != 0 and fp_rate[ilast] != 0 : 
    tp_rate = concatenate( (tp_rate, array([0.0])) ) ; 
    fp_rate = concatenate( (fp_rate, array([0.0])) ) ; 

  if(plot) : 
    print "tp_rates: ", tp_rate; 
    print "fb_rates: ", fp_rate; 
    pylab.plot(fp_rate, tp_rate); 
    pylab.xlabel('false positive rate'); 
    pylab.ylabel('true positive rate'); 
    pylab.show(); 

  #these are pylab.array types
  return tp_rate, fp_rate;  

def area_under_curve(tp_rate, fp_rate) : 
    
  if len(tp_rate) != len(fp_rate) :
    print "ERROR tp_rate and fp_rate are not one to one!"
    return -1; 
    
  if tp_rate[0] != 1 and fp_rate[0] != 1 : 
    tp_rate = concatenate( (array([1.0]), tp_rate) ) ; 
    fp_rate = concatenate( (array([1.0]), fp_rate) ) ; 

  #sum rectangles
  sum = 0.0; 
  for i in range(0, len(tp_rate) - 1) : 
    tp     = tp_rate[i];
    tpn    = tp_rate[i+1];  
    fp     = fp_rate[i]; 
    fpn    = fp_rate[i+1]; 
    height = (tp+tpn)/2; 
    width  = fp-fpn; 
    sum += height*width; 
  
  return sum; 

def array_to_string(arr) : 
  aStr = "[" 
  for a in arr : 
    aStr += str(a)+", "
  aStr += "]"
  return aStr
