import boxm2_batch
boxm2_batch.not_verbose();
boxm2_batch.register_processes();
boxm2_batch.register_datatypes();
class dbvalue:
  def __init__(self, index, type):
    self.id = index    # unsigned integer
    self.type = type   # string


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
def bae_raw_stream(file_path,ni=0,nj=0,pixelsize=0) :
  boxm2_batch.init_process("bilCreateRawImageIstreamProcess")
  boxm2_batch.set_input_string(0,file_path);
  boxm2_batch.set_input_int(1,ni);
  boxm2_batch.set_input_int(2,nj);
  boxm2_batch.set_input_int(3,pixelsize);
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
  #(id, type) = boxm2_batch.commit_output(1);
  #time = boxm2_batch.get_output_unsigned(id);
  return img

def seek_frame(rawStream, frame) :
  boxm2_batch.init_process("bilSeekFrameProcess")
  boxm2_batch.set_input_from_db(0,rawStream);
  boxm2_batch.set_input_unsigned(1,frame);
  boxm2_batch.run_process();
  (id, type) = boxm2_batch.commit_output(0);
  img = dbvalue(id,type);
  #(id, type) = boxm2_batch.commit_output(1);
  #time = boxm2_batch.get_output_unsigned(id);
  return img
def arf_stream(file_path) :
  boxm2_batch.init_process("bilCreateArfImageIstreamProcess")
  boxm2_batch.set_input_string(0,file_path);
  boxm2_batch.run_process();
  (id, type) = boxm2_batch.commit_output(0);
  stream = dbvalue(id, type);
  (id, type) = boxm2_batch.commit_output(1);
  numImgs = boxm2_batch.get_output_int(id);
  return stream, numImgs
def arf_next_frame(rawStream) :
  boxm2_batch.init_process("bilArfReadFrameProcess")
  boxm2_batch.set_input_from_db(0,rawStream);
  boxm2_batch.run_process();
  (id, type) = boxm2_batch.commit_output(0);
  img = dbvalue(id,type);
  (id, type) = boxm2_batch.commit_output(1);
  time = boxm2_batch.get_output_unsigned(id);
  return img, time

def arf_seek_frame(rawStream, frame) :
  boxm2_batch.init_process("bilArfSeekFrameProcess")
  boxm2_batch.set_input_from_db(0,rawStream);
  boxm2_batch.set_input_unsigned(1,frame);
  boxm2_batch.run_process();
  (id, type) = boxm2_batch.commit_output(0);
  img = dbvalue(id,type);
  (id, type) = boxm2_batch.commit_output(1);
  time = boxm2_batch.get_output_unsigned(id);
  return img, time

def read_CLIF07(indir,outdir,camnum,datatype="CLIF06") :
  boxm2_batch.init_process("bilReadCLIF07DataProcess")
  boxm2_batch.set_input_string(0,indir);
  boxm2_batch.set_input_string(1,outdir);
  boxm2_batch.set_input_int(2,camnum);
  boxm2_batch.set_input_string(3,datatype);
  boxm2_batch.run_process();

def debayer(img):
  boxm2_batch.init_process("vilDebayerBGGRToRGBProcess")
  boxm2_batch.set_input_from_db(0,img);
  boxm2_batch.run_process();
  (id, type) = boxm2_batch.commit_output(0);
  outimg = dbvalue(id,type);
  return outimg;
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
  (id,type) = boxm2_batch.commit_output(6);
  outimg = dbvalue(id,type);
  #return tuple of true positives, true negatives, false positives, etc..
  return (tp, tn, fp, fn,outimg);

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

def threshold_image(img, value, threshold_above=True):
    boxm2_batch.init_process("vilThresholdImageProcess")
    boxm2_batch.set_input_from_db(0,img)
    boxm2_batch.set_input_float(1,value)
    boxm2_batch.set_input_bool(2,threshold_above)
    boxm2_batch.run_process()
    (id,type) = boxm2_batch.commit_output(0)
    mask = dbvalue(id,type)
    return mask
def max_threshold_image(img, threshold):
    boxm2_batch.init_process("vilThresholdMaxImageProcess")
    boxm2_batch.set_input_from_db(0,img)
    boxm2_batch.set_input_float(1,threshold)
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

def truncate_image(img,min_value,max_value):
    boxm2_batch.init_process("vilTruncateImageProcess")
    boxm2_batch.set_input_from_db(0,img)
    boxm2_batch.set_input_float(1,min_value)
    boxm2_batch.set_input_float(2,max_value)
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

def crop_image(img,i0,j0,ni,nj):
  boxm2_batch.init_process("vilCropImageProcess")
  boxm2_batch.set_input_from_db(0,img)
  boxm2_batch.set_input_unsigned(1,i0)
  boxm2_batch.set_input_unsigned(2,j0)
  boxm2_batch.set_input_unsigned(3,ni)
  boxm2_batch.set_input_unsigned(4,nj)
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  img_out = dbvalue(id,type)
  return img_out

def scale_and_offset_values(img,scale,offset):
  boxm2_batch.init_process("vilScaleAndOffsetValuesProcess")
  boxm2_batch.set_input_from_db(0,img)
  boxm2_batch.set_input_float(1,scale)
  boxm2_batch.set_input_float(2,offset)
  boxm2_batch.run_process()
  return

def init_float_img(ni,nj,np,val):
  boxm2_batch.init_process("vilInitFloatImageProcess")
  boxm2_batch.set_input_unsigned(0,ni)
  boxm2_batch.set_input_unsigned(1,nj)
  boxm2_batch.set_input_unsigned(2,np)
  boxm2_batch.set_input_float(3,val)
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  img_out = dbvalue(id,type)
  return img_out

def nitf_date_time(image_filename):
  boxm2_batch.init_process("vilNITFDateTimeProcess");
  boxm2_batch.set_input_string(0,image_filename);
  boxm2_batch.run_process();
  (id,type)=boxm2_batch.commit_output(0);
  year =  boxm2_batch.get_output_int(id);
  (id,type)=boxm2_batch.commit_output(1);
  month =  boxm2_batch.get_output_int(id);
  (id,type)=boxm2_batch.commit_output(2);
  day =  boxm2_batch.get_output_int(id);
  (id,type)=boxm2_batch.commit_output(3);
  hour = boxm2_batch.get_output_int(id);
  (id,type)=boxm2_batch.commit_output(4);
  minute = boxm2_batch.get_output_int(id);
  return year, month, day, hour, minute

def undistort_image(img, param_file, iters) :
  boxm2_batch.init_process("vilUndistortImageProcess");
  boxm2_batch.set_input_from_db(0,img)
  boxm2_batch.set_input_string(1, param_file);
  boxm2_batch.set_input_int(2, iters);
  boxm2_batch.run_process();
  (o_id,o_type) = boxm2_batch.commit_output(0);
  out_img = dbvalue(o_id,o_type);
  return out_img;

def combine_eo_ir(eo_img,ir_img):
  boxm2_batch.init_process("vilEOIRCombineProcess")
  boxm2_batch.set_input_from_db(0,eo_img)
  boxm2_batch.set_input_from_db(1,ir_img)
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  img_out = dbvalue(id,type)
  return img_out

def detect_shadow_rgb(img,threshold) :
  boxm2_batch.init_process("vilShadowDetectionProcess");
  boxm2_batch.set_input_from_db(0,img)
  boxm2_batch.set_input_float(1, threshold);
  boxm2_batch.run_process();
  (o_id,o_type) = boxm2_batch.commit_output(0);
  region_img = dbvalue(o_id,o_type);
  return region_img;

def detect_shadow_ridge(region_img,blob_size_t, sun_angle) :
  boxm2_batch.init_process("vilShadowRidgeDetectionProcess");
  boxm2_batch.set_input_from_db(0,region_img)
  boxm2_batch.set_input_int(1, blob_size_t);
  boxm2_batch.set_input_float(2, sun_angle);
  boxm2_batch.run_process();
  (o_id,o_type) = boxm2_batch.commit_output(0);
  region_img = dbvalue(o_id,o_type);
  (o_id,o_type) = boxm2_batch.commit_output(1);
  out_img = dbvalue(o_id,o_type);
  (o_id,o_type) = boxm2_batch.commit_output(2);
  dist_img = dbvalue(o_id,o_type);
  return region_img, out_img, dist_img;

def binary_img_op(img1, img2, operation="sum"):
  boxm2_batch.init_process("vilBinaryImageOpProcess")
  boxm2_batch.set_input_from_db(0,img1)
  boxm2_batch.set_input_from_db(1,img2)
  boxm2_batch.set_input_string(2,operation)
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  out = dbvalue(id, type);
  return out

def img_sum(img, plane_index=0):
  boxm2_batch.init_process("vilImageSumProcess")
  boxm2_batch.set_input_from_db(0,img)
  boxm2_batch.set_input_unsigned(1,plane_index)
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  value = boxm2_batch.get_output_double(id)
  return value

## input a visibility image with float values in 0,1 range, negate this image and threshold to generate a byte image as a mask
def prepare_mask_image_from_vis_image(vis_image, ni2, nj2, threshold):
  img_1 = init_float_img(ni2,nj2,1,-1.0);
  vis_image_neg = binary_img_op(img_1, vis_image, "sum");
  scale_and_offset_values(vis_image_neg,-1.0,0.0);
  exp_img_mask_f = threshold_image(vis_image_neg, threshold);
  sum = img_sum(exp_img_mask_f);
  #print "mask sum: " + str(sum) + " ratio of true: " + str(sum/(ni2*nj2));
  ratio = sum/(ni2*nj2)*100;
  exp_img_mask = stretch_image(exp_img_mask_f, 0, 1, 'byte');
  return exp_img_mask, img_1, vis_image_neg, ratio


def fill_holes(img):
  boxm2_batch.init_process("vilFillHolesInRegionsProcess")
  boxm2_batch.set_input_from_db(0,img)
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  outimg = dbvalue(id, type);
  return outimg

def grey_to_rgb(img, color_txt):
  boxm2_batch.init_process("vilGreyToRGBProcess")
  boxm2_batch.set_input_from_db(0,img)
  boxm2_batch.set_input_string(1,color_txt)
  result = boxm2_batch.run_process()
  if result:
    (id, type) = boxm2_batch.commit_output(0)
    outimg = dbvalue(id, type);
  else:
    outimg = 0
  return outimg

def mask_image_using_id(img, id_img, input_id):
  boxm2_batch.init_process("vilMaskImageUsingIDsProcess");
  boxm2_batch.set_input_from_db(0, img);
  boxm2_batch.set_input_from_db(1, id_img);
  boxm2_batch.set_input_unsigned(2, input_id);
  boxm2_batch.run_process();
  (id, type) = boxm2_batch.commit_output(0);
  masked_img = dbvalue(id, type);
  return masked_img;

def get_plane(img, plane_id):
  boxm2_batch.init_process("vilGetPlaneProcess");
  boxm2_batch.set_input_from_db(0, img);
  boxm2_batch.set_input_unsigned(1, plane_id);
  boxm2_batch.run_process();
  (id, type) = boxm2_batch.commit_output(0);
  img_plane = dbvalue(id, type);
  return img_plane;

def combine_planes(img_red, img_green, img_blue):
  boxm2_batch.init_process("vilCombinePlanesProcess");
  boxm2_batch.set_input_from_db(0, img_red);
  boxm2_batch.set_input_from_db(1, img_green);
  boxm2_batch.set_input_from_db(2, img_blue);
  boxm2_batch.run_process();
  (id, type) = boxm2_batch.commit_output(0);
  img_out = dbvalue(id, type);
  return img_out;

def median_filter_image(img, neighborhood_radius):
    boxm2_batch.init_process("vilMedianFilterProcess")
    boxm2_batch.set_input_from_db(0,img)
    boxm2_batch.set_input_int(1,neighborhood_radius)
    boxm2_batch.run_process()
    (id,type) = boxm2_batch.commit_output(0)
    filt_img = dbvalue(id,type)
    return filt_img
