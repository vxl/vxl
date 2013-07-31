from bstm_register import bstm_batch, dbvalue;

###################################################
# Vil loading and saving
###################################################
def load_image(file_path) :
  bstm_batch.init_process("vilLoadImageViewProcess");
  bstm_batch.set_input_string(0, file_path);
  bstm_batch.run_process();
  (id,type) = bstm_batch.commit_output(0);
  (ni_id, ni_type) = bstm_batch.commit_output(1);
  (nj_id, nj_type) = bstm_batch.commit_output(2);
  ni = bstm_batch.get_output_unsigned(ni_id);
  nj = bstm_batch.get_output_unsigned(nj_id);
  img = dbvalue(id,type);
  bstm_batch.remove_data(ni_id)
  bstm_batch.remove_data(nj_id)
  return img, ni, nj;

def save_image(img, file_path) :
  assert not isinstance(list, tuple)
  bstm_batch.init_process("vilSaveImageViewProcess");
  bstm_batch.set_input_from_db(0,img);
  bstm_batch.set_input_string(1,file_path);
  bstm_batch.run_process();

def close_image(img, disk_radius) :
  bstm_batch.init_process("vilImageClosingDiskProcess");
  bstm_batch.set_input_from_db(0, img);
  bstm_batch.set_input_float(1, disk_radius);
  bstm_batch.run_process();
  (id,type) = bstm_batch.commit_output(0);
  cimg = dbvalue(id,type);
  return cimg;

def convert_image(img, type="byte") :
  bstm_batch.init_process("vilConvertPixelTypeProcess");
  bstm_batch.set_input_from_db(0, img);
  bstm_batch.set_input_string(1, type);
  bstm_batch.run_process();
  (id,type) = bstm_batch.commit_output(0);
  cimg = dbvalue(id,type);
  return cimg;


def rgb2gray(img):
  bstm_batch.init_process("vilRGBToGreyProcess")
  bstm_batch.set_input_from_db(0,img);
  bstm_batch.run_process();
  (id, type) = bstm_batch.commit_output(0);
  outimg = dbvalue(id,type);
  return outimg;


#pixel wise roc process for change detection images
def pixel_wise_roc(cd_img, gt_img, mask_img=None) :
  bstm_batch.init_process("vilPixelwiseRocProcess");
  bstm_batch.set_input_from_db(0,cd_img);
  bstm_batch.set_input_from_db(1,gt_img);
  if mask_img:
    bstm_batch.set_input_from_db(2,mask_img);
  bstm_batch.run_process();
  (id,type) = bstm_batch.commit_output(0);
  tp = bstm_batch.get_bbas_1d_array_float(id);
  (id,type) = bstm_batch.commit_output(1);
  tn = bstm_batch.get_bbas_1d_array_float(id);
  (id,type) = bstm_batch.commit_output(2);
  fp = bstm_batch.get_bbas_1d_array_float(id);
  (id,type) = bstm_batch.commit_output(3);
  fn = bstm_batch.get_bbas_1d_array_float(id);

  #return tuple of true positives, true negatives, false positives, etc..
  return (tp, tn, fp, fn);

#get image pixel value (always 0-1 float)
def pixel(img, point):
    bstm_batch.init_process("vilPixelValueProcess")
    bstm_batch.set_input_from_db(0,img)
    bstm_batch.set_input_int(1, int(point[0]))
    bstm_batch.set_input_int(2, int(point[1]))
    bstm_batch.run_process()
    (id,type) = bstm_batch.commit_output(0)
    val = bstm_batch.get_output_float(id)
    return val


#resize image (default returns float image
def resize(img, ni, nj, pixel="float"):
    bstm_batch.init_process("vilResampleProcess")
    bstm_batch.set_input_from_db(0,img)
    bstm_batch.set_input_int(1, ni)
    bstm_batch.set_input_int(2, nj)
    bstm_batch.set_input_string(3, pixel);
    bstm_batch.run_process()
    (id,type) = bstm_batch.commit_output(0)
    img = dbvalue(id,type)
    return img

# get image dimensions
def image_size(img):
    bstm_batch.init_process('vilImageSizeProcess')
    bstm_batch.set_input_from_db(0,img)
    bstm_batch.run_process()
    (id,type) = bstm_batch.commit_output(0)
    ni = bstm_batch.get_output_unsigned(id)
    (id,type) = bstm_batch.commit_output(1)
    nj = bstm_batch.get_output_unsigned(id)
    return ni,nj

def image_range(img):
    bstm_batch.init_process('vilImageRangeProcess')
    bstm_batch.set_input_from_db(0,img)
    bstm_batch.run_process()
    (id,type) = bstm_batch.commit_output(0)
    minVal = bstm_batch.get_output_float(id)
    (id,type) = bstm_batch.commit_output(1)
    maxVal = bstm_batch.get_output_float(id)
    return minVal, maxVal

def gradient(img) :
    bstm_batch.init_process('vilGradientProcess')
    bstm_batch.set_input_from_db(0,img)
    bstm_batch.run_process()
    #x image
    (id,type) = bstm_batch.commit_output(0)
    dIdx = dbvalue(id,type)
    #y image
    (id,type) = bstm_batch.commit_output(1)
    dIdy = dbvalue(id,type)
    #mag image
    (id,type) = bstm_batch.commit_output(2)
    magImg = dbvalue(id,type)
    return dIdx, dIdy, magImg

def gradient_angle(Ix, Iy) :
    bstm_batch.init_process('vilGradientAngleProcess')
    bstm_batch.set_input_from_db(0,Ix)
    bstm_batch.set_input_from_db(1,Iy)
    bstm_batch.run_process()
    #x image
    (id,type) = bstm_batch.commit_output(0)
    angleImg = dbvalue(id,type)
    return angleImg

def threshold_image(img, value, threshold_above=True):
    bstm_batch.init_process("vilThresholdImageProcess")
    bstm_batch.set_input_from_db(0,img)
    bstm_batch.set_input_float(1,value)
    bstm_batch.set_input_bool(2,threshold_above)
    bstm_batch.run_process()
    (id,type) = bstm_batch.commit_output(0)
    mask = dbvalue(id,type)
    return mask

def image_mean(img):
  bstm_batch.init_process("vilImageMeanProcess")
  bstm_batch.set_input_from_db(0,img)
  bstm_batch.run_process()
  (id,type) = bstm_batch.commit_output(0)
  mean_val = bstm_batch.get_output_float(id)
  bstm_batch.remove_data(id)
  return mean_val

def crop_image(img,i0,j0,ni,nj):
  bstm_batch.init_process("vilCropImageProcess")
  bstm_batch.set_input_from_db(0,img)
  bstm_batch.set_input_unsigned(1,i0)
  bstm_batch.set_input_unsigned(2,j0)
  bstm_batch.set_input_unsigned(3,ni)
  bstm_batch.set_input_unsigned(4,nj)
  bstm_batch.run_process()
  (id,type) = bstm_batch.commit_output(0)
  img_out = dbvalue(id,type)
  return img_out

def scale_and_offset_values(img,scale,offset):
  bstm_batch.init_process("vilScaleAndOffsetValuesProcess")
  bstm_batch.set_input_from_db(0,img)
  bstm_batch.set_input_float(1,scale)
  bstm_batch.set_input_float(2,offset)
  bstm_batch.run_process()
  return



def binary_img_op(img1, img2, operation="sum"):
  bstm_batch.init_process("vilBinaryImageOpProcess")
  bstm_batch.set_input_from_db(0,img1)
  bstm_batch.set_input_from_db(1,img2)
  bstm_batch.set_input_string(2,operation)
  bstm_batch.run_process()
  (id,type) = bstm_batch.commit_output(0)
  out = dbvalue(id, type);
  return out

def img_sum(img, plane_index=0):
  bstm_batch.init_process("vilImageSumProcess")
  bstm_batch.set_input_from_db(0,img)
  bstm_batch.set_input_unsigned(1,plane_index)
  bstm_batch.run_process()
  (id,type) = bstm_batch.commit_output(0)
  value = bstm_batch.get_output_double(id)
  return value

