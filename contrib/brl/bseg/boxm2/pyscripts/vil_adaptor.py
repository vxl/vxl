from boxm2_register import boxm2_batch, dbvalue;

###################################################
# Vil loading and saving
###################################################
def load_image(file_path) :
  boxm2_batch.init_process("vilLoadImageViewProcess");
  boxm2_batch.set_input_string(0, file_path);
  boxm2_batch.run_process();
  (id,type) = boxm2_batch.commit_output(0);
  (ni_id, ni_type) = boxm2_batch.commit_output(1);
  (nj_id, nj_type) = boxm2_batch.commit_output(2);
  ni = boxm2_batch.get_output_unsigned(ni_id);
  nj = boxm2_batch.get_output_unsigned(nj_id);
  img = dbvalue(id,type);
  boxm2_batch.remove_data(ni_id)
  boxm2_batch.remove_data(nj_id)
  return img, ni, nj;

def save_image(img, file_path) :
  assert not isinstance(list, tuple) 
  boxm2_batch.init_process("vilSaveImageViewProcess");
  boxm2_batch.set_input_from_db(0,img);
  boxm2_batch.set_input_string(1,file_path);
  boxm2_batch.run_process();

def convert_image(img, type="byte") :
  boxm2_batch.init_process("vilConvertPixelTypeProcess");
  boxm2_batch.set_input_from_db(0, img);
  boxm2_batch.set_input_string(1, type);
  boxm2_batch.run_process();
  (id,type) = boxm2_batch.commit_output(0);
  cimg = dbvalue(id,type);
  return cimg;
  
################################
# BAE raw file image stream
################################
def bae_raw_stream(file_path) : 
  boxm2_batch.init_process("bilCreateRawImageIstreamProcess")
  boxm2_batch.set_input_string(0,file_path);
  boxm2_batch.run_process();
  (id, type) = boxm2_batch.commit_output(0);
  stream = dbvalue(id, type);
  (id, type) = boxm2_batch.commit_output(1); 
  numImgs = boxm2_batch.get_output_int(id);
  return stream, numImgs 

def next_frame(rawStream) :
  boxm2_batch.init_process("bilReadFrameProcess")
  boxm2_batch.set_input_from_db(0,rawStream);
  boxm2_batch.run_process();
  (id, type) = boxm2_batch.commit_output(0);
  img = dbvalue(id,type);
  (id, type) = boxm2_batch.commit_output(1);
  time = boxm2_batch.get_output_unsigned(id);
  return img, time
  
def seek_frame(rawStream, frame) :
  boxm2_batch.init_process("bilSeekFrameProcess")
  boxm2_batch.set_input_from_db(0,rawStream);
  boxm2_batch.set_input_unsigned(1,frame);
  boxm2_batch.run_process();
  (id, type) = boxm2_batch.commit_output(0);
  img = dbvalue(id,type);
  (id, type) = boxm2_batch.commit_output(1);
  time = boxm2_batch.get_output_unsigned(id);
  return img, time
  
#pixel wise roc process for change detection images
def pixel_wise_roc(cd_img, gt_img, mask_img=None) :
  boxm2_batch.init_process("vilPixelwiseRocProcess");
  boxm2_batch.set_input_from_db(0,cd_img);
  boxm2_batch.set_input_from_db(1,gt_img);
  if mask_img:
    boxm2_batch.set_input_from_db(2,mask_img);
  boxm2_batch.run_process();
  (id,type) = boxm2_batch.commit_output(0);
  tp = boxm2_batch.get_bbas_1d_array_float(id);
  (id,type) = boxm2_batch.commit_output(1);
  tn = boxm2_batch.get_bbas_1d_array_float(id);
  (id,type) = boxm2_batch.commit_output(2);
  fp = boxm2_batch.get_bbas_1d_array_float(id);
  (id,type) = boxm2_batch.commit_output(3);
  fn = boxm2_batch.get_bbas_1d_array_float(id);

  #return tuple of true positives, true negatives, false positives, etc..
  return (tp, tn, fp, fn);

#get image pixel value (always 0-1 float)
def pixel(img, point):
    boxm2_batch.init_process("vilPixelValueProcess") 
    boxm2_batch.set_input_from_db(0,img)
    boxm2_batch.set_input_int(1, int(point[0]))
    boxm2_batch.set_input_int(2, int(point[1]))
    boxm2_batch.run_process()
    (id,type) = boxm2_batch.commit_output(0)
    val = boxm2_batch.get_output_float(id)
    return val
    

#resize image (default returns float image
def resize(img, ni, nj, pixel="float"):
    boxm2_batch.init_process("vilResampleProcess") 
    boxm2_batch.set_input_from_db(0,img)
    boxm2_batch.set_input_int(1, ni)
    boxm2_batch.set_input_int(2, nj)
    boxm2_batch.set_input_string(3, pixel);
    boxm2_batch.run_process()
    (id,type) = boxm2_batch.commit_output(0)
    img = dbvalue(id,type)
    return img
    
# get image dimensions
def image_size(img):
    boxm2_batch.init_process('vilImageSizeProcess')
    boxm2_batch.set_input_from_db(0,img)
    boxm2_batch.run_process()
    (id,type) = boxm2_batch.commit_output(0)
    ni = boxm2_batch.get_output_unsigned(id)
    (id,type) = boxm2_batch.commit_output(1)
    nj = boxm2_batch.get_output_unsigned(id)
    return ni,nj

def image_range(img):
    boxm2_batch.init_process('vilImageRangeProcess')
    boxm2_batch.set_input_from_db(0,img)
    boxm2_batch.run_process()
    (id,type) = boxm2_batch.commit_output(0)
    minVal = boxm2_batch.get_output_float(id)
    (id,type) = boxm2_batch.commit_output(1)
    maxVal = boxm2_batch.get_output_float(id)
    return minVal, maxVal
    
def gradient(img) :
    boxm2_batch.init_process('vilGradientProcess')
    boxm2_batch.set_input_from_db(0,img)
    boxm2_batch.run_process()
    #x image
    (id,type) = boxm2_batch.commit_output(0)
    dIdx = dbvalue(id,type)
    #y image
    (id,type) = boxm2_batch.commit_output(1)
    dIdy = dbvalue(id,type)
    #mag image
    (id,type) = boxm2_batch.commit_output(2)
    magImg = dbvalue(id,type)
    return dIdx, dIdy, magImg
    
def gradient_angle(Ix, Iy) :
    boxm2_batch.init_process('vilGradientAngleProcess')
    boxm2_batch.set_input_from_db(0,Ix)
    boxm2_batch.set_input_from_db(1,Iy)
    boxm2_batch.run_process()
    #x image
    (id,type) = boxm2_batch.commit_output(0)
    angleImg = dbvalue(id,type)
    return angleImg

def init_float_image(ni,nj,init_val):
    boxm2_batch.init_process("vilInitFloatImageProcess")
    boxm2_batch.set_input_unsigned(0,ni)
    boxm2_batch.set_input_unsigned(1,nj)
    boxm2_batch.set_input_float(2, init_val)
    boxm2_batch.run_process()
    (id,type) = boxm2_batch.commit_output(0)
    img = dbvalue(id,type)
    return img

def threshold_image(img, value, threshold_above=True):
    boxm2_batch.init_process("vilThresholdImageProcess")
    boxm2_batch.set_input_from_db(0,img)
    boxm2_batch.set_input_float(1,value)
    boxm2_batch.set_input_bool(2,threshold_above)
    boxm2_batch.run_process()
    (id,type) = boxm2_batch.commit_output(0)
    mask = dbvalue(id,type)
    return mask

def stretch_image(img, min_value, max_value, output_type_str='float'):
    boxm2_batch.init_process("vilStretchImageProcess")
    boxm2_batch.set_input_from_db(0,img)
    boxm2_batch.set_input_float(1,min_value)
    boxm2_batch.set_input_float(2,max_value)
    boxm2_batch.set_input_string(3,output_type_str)
    boxm2_batch.run_process()
    (id,type) = boxm2_batch.commit_output(0)
    img_out = dbvalue(id,type)
    return img_out

def image_mean(img):
  boxm2_batch.init_process("vilImageMeanProcess")
  boxm2_batch.set_input_from_db(0,img)
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  mean_val = boxm2_batch.get_output_float(id)
  boxm2_batch.remove_data(id)
  return mean_val

def scale_and_offset_values(img,scale,offset):
  boxm2_batch.init_process("vilScaleAndOffsetValuesProcess")
  boxm2_batch.set_input_from_db(0,img)
  boxm2_batch.set_input_float(1,scale)
  boxm2_batch.set_input_float(2,offset)
  boxm2_batch.run_process()
  return


