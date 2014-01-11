import bvxm_batch
bvxm_batch.not_verbose();
bvxm_batch.register_processes();
bvxm_batch.register_datatypes();
class dbvalue:
  def __init__(self, index, type):
    self.id = index    # unsigned integer
    self.type = type   # string


###################################################
# Vil loading and saving
###################################################
def bvxm_load_image(file_path) :
  bvxm_batch.init_process("vilLoadImageViewProcess");
  bvxm_batch.set_input_string(0, file_path);
  bvxm_batch.run_process();
  (id,type) = bvxm_batch.commit_output(0);
  (ni_id, ni_type) = bvxm_batch.commit_output(1);
  (nj_id, nj_type) = bvxm_batch.commit_output(2);
  ni = bvxm_batch.get_output_unsigned(ni_id);
  nj = bvxm_batch.get_output_unsigned(nj_id);
  img = dbvalue(id,type);
  bvxm_batch.remove_data(ni_id)
  bvxm_batch.remove_data(nj_id)
  return img, ni, nj;

def bvxm_save_image(img, file_path) :
  assert not isinstance(list, tuple)
  bvxm_batch.init_process("vilSaveImageViewProcess");
  bvxm_batch.set_input_from_db(0,img);
  bvxm_batch.set_input_string(1,file_path);
  bvxm_batch.run_process();

def convert_image(img, type="byte") :
  bvxm_batch.init_process("vilConvertPixelTypeProcess");
  bvxm_batch.set_input_from_db(0, img);
  bvxm_batch.set_input_string(1, type);
  bvxm_batch.run_process();
  (id,type) = bvxm_batch.commit_output(0);
  cimg = dbvalue(id,type);
  return cimg;

################################
# BAE raw file image stream
################################
def bae_raw_stream(file_path,ni=0,nj=0,pixelsize=0) :
  bvxm_batch.init_process("bilCreateRawImageIstreamProcess")
  bvxm_batch.set_input_string(0,file_path);
  bvxm_batch.set_input_int(1,ni);
  bvxm_batch.set_input_int(2,nj);
  bvxm_batch.set_input_int(3,pixelsize);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  stream = dbvalue(id, type);
  (id, type) = bvxm_batch.commit_output(1);
  numImgs = bvxm_batch.get_output_int(id);
  return stream, numImgs

def next_frame(rawStream) :
  bvxm_batch.init_process("bilReadFrameProcess")
  bvxm_batch.set_input_from_db(0,rawStream);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  img = dbvalue(id,type);
  #(id, type) = bvxm_batch.commit_output(1);
  #time = bvxm_batch.get_output_unsigned(id);
  return img

def seek_frame(rawStream, frame) :
  bvxm_batch.init_process("bilSeekFrameProcess")
  bvxm_batch.set_input_from_db(0,rawStream);
  bvxm_batch.set_input_unsigned(1,frame);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  img = dbvalue(id,type);
  #(id, type) = bvxm_batch.commit_output(1);
  #time = bvxm_batch.get_output_unsigned(id);
  return img
def arf_stream(file_path) :
  bvxm_batch.init_process("bilCreateArfImageIstreamProcess")
  bvxm_batch.set_input_string(0,file_path);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  stream = dbvalue(id, type);
  (id, type) = bvxm_batch.commit_output(1);
  numImgs = bvxm_batch.get_output_int(id);
  return stream, numImgs
def arf_next_frame(rawStream) :
  bvxm_batch.init_process("bilArfReadFrameProcess")
  bvxm_batch.set_input_from_db(0,rawStream);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  img = dbvalue(id,type);
  (id, type) = bvxm_batch.commit_output(1);
  time = bvxm_batch.get_output_unsigned(id);
  return img, time

def arf_seek_frame(rawStream, frame) :
  bvxm_batch.init_process("bilArfSeekFrameProcess")
  bvxm_batch.set_input_from_db(0,rawStream);
  bvxm_batch.set_input_unsigned(1,frame);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  img = dbvalue(id,type);
  (id, type) = bvxm_batch.commit_output(1);
  time = bvxm_batch.get_output_unsigned(id);
  return img, time

def read_CLIF07(indir,outdir,camnum,datatype="CLIF06") :
  bvxm_batch.init_process("bilReadCLIF07DataProcess")
  bvxm_batch.set_input_string(0,indir);
  bvxm_batch.set_input_string(1,outdir);
  bvxm_batch.set_input_int(2,camnum);
  bvxm_batch.set_input_string(3,datatype);
  bvxm_batch.run_process();

def debayer(img):
  bvxm_batch.init_process("vilDebayerBGGRToRGBProcess")
  bvxm_batch.set_input_from_db(0,img);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  outimg = dbvalue(id,type);
  return outimg;
#pixel wise roc process for change detection images
def pixel_wise_roc(cd_img, gt_img, mask_img=None) :
  bvxm_batch.init_process("vilPixelwiseRocProcess");
  bvxm_batch.set_input_from_db(0,cd_img);
  bvxm_batch.set_input_from_db(1,gt_img);
  if mask_img:
    bvxm_batch.set_input_from_db(2,mask_img);
  bvxm_batch.run_process();
  (id,type) = bvxm_batch.commit_output(0);
  tp = bvxm_batch.get_bbas_1d_array_float(id);
  (id,type) = bvxm_batch.commit_output(1);
  tn = bvxm_batch.get_bbas_1d_array_float(id);
  (id,type) = bvxm_batch.commit_output(2);
  fp = bvxm_batch.get_bbas_1d_array_float(id);
  (id,type) = bvxm_batch.commit_output(3);
  fn = bvxm_batch.get_bbas_1d_array_float(id);
  (id,type) = bvxm_batch.commit_output(6);
  outimg = dbvalue(id,type);
  #return tuple of true positives, true negatives, false positives, etc..
  return (tp, tn, fp, fn,outimg);

#get image pixel value (always 0-1 float)
def pixel(img, point):
    bvxm_batch.init_process("vilPixelValueProcess")
    bvxm_batch.set_input_from_db(0,img)
    bvxm_batch.set_input_int(1, int(point[0]))
    bvxm_batch.set_input_int(2, int(point[1]))
    bvxm_batch.run_process()
    (id,type) = bvxm_batch.commit_output(0)
    val = bvxm_batch.get_output_float(id)
    return val


#resize image (default returns float image
def bvxm_resize(img, ni, nj, pixel="float"):
    bvxm_batch.init_process("vilResampleProcess")
    bvxm_batch.set_input_from_db(0,img)
    bvxm_batch.set_input_int(1, ni)
    bvxm_batch.set_input_int(2, nj)
    bvxm_batch.set_input_string(3, pixel);
    bvxm_batch.run_process()
    (id,type) = bvxm_batch.commit_output(0)
    img = dbvalue(id,type)
    return img

# get image dimensions
def image_size(img):
    bvxm_batch.init_process('vilImageSizeProcess')
    bvxm_batch.set_input_from_db(0,img)
    bvxm_batch.run_process()
    (id,type) = bvxm_batch.commit_output(0)
    ni = bvxm_batch.get_output_unsigned(id)
    (id,type) = bvxm_batch.commit_output(1)
    nj = bvxm_batch.get_output_unsigned(id)
    return ni,nj

def image_range(img):
    bvxm_batch.init_process('vilImageRangeProcess')
    bvxm_batch.set_input_from_db(0,img)
    bvxm_batch.run_process()
    (id,type) = bvxm_batch.commit_output(0)
    minVal = bvxm_batch.get_output_float(id)
    (id,type) = bvxm_batch.commit_output(1)
    maxVal = bvxm_batch.get_output_float(id)
    return minVal, maxVal

def gradient(img) :
    bvxm_batch.init_process('vilGradientProcess')
    bvxm_batch.set_input_from_db(0,img)
    bvxm_batch.run_process()
    #x image
    (id,type) = bvxm_batch.commit_output(0)
    dIdx = dbvalue(id,type)
    #y image
    (id,type) = bvxm_batch.commit_output(1)
    dIdy = dbvalue(id,type)
    #mag image
    (id,type) = bvxm_batch.commit_output(2)
    magImg = dbvalue(id,type)
    return dIdx, dIdy, magImg

def gradient_angle(Ix, Iy) :
    bvxm_batch.init_process('vilGradientAngleProcess')
    bvxm_batch.set_input_from_db(0,Ix)
    bvxm_batch.set_input_from_db(1,Iy)
    bvxm_batch.run_process()
    #x image
    (id,type) = bvxm_batch.commit_output(0)
    angleImg = dbvalue(id,type)
    return angleImg

def threshold_image(img, value, threshold_above=True):
    bvxm_batch.init_process("vilThresholdImageProcess")
    bvxm_batch.set_input_from_db(0,img)
    bvxm_batch.set_input_float(1,value)
    bvxm_batch.set_input_bool(2,threshold_above)
    bvxm_batch.run_process()
    (id,type) = bvxm_batch.commit_output(0)
    mask = dbvalue(id,type)
    return mask

def bvxm_stretch_image(img, min_value, max_value, output_type_str='float'):
    bvxm_batch.init_process("vilStretchImageProcess")
    bvxm_batch.set_input_from_db(0,img)
    bvxm_batch.set_input_float(1,min_value)
    bvxm_batch.set_input_float(2,max_value)
    bvxm_batch.set_input_string(3,output_type_str)
    bvxm_batch.run_process()
    (id,type) = bvxm_batch.commit_output(0)
    img_out = dbvalue(id,type)
    return img_out

def bvxm_truncate_image(img,min_value,max_value):
    bvxm_batch.init_process("vilTruncateImageProcess")
    bvxm_batch.set_input_from_db(0,img)
    bvxm_batch.set_input_float(1,min_value)
    bvxm_batch.set_input_float(2,max_value)
    bvxm_batch.run_process()
    (id,type) = bvxm_batch.commit_output(0)
    img_out = dbvalue(id,type)
    return img_out

def normalize_image(img):
    bvxm_batch.init_process("vilImageNormaliseProcess")
    bvxm_batch.set_input_from_db(0,img)
    bvxm_batch.run_process()

def median_filter(img, operator_half_size):
    bvxm_batch.init_process("vilMedianFilterProcess")
    bvxm_batch.set_input_from_db(0,img)
    bvxm_batch.set_input_int(1, operator_half_size)
    bvxm_batch.run_process()
    (id,type) = bvxm_batch.commit_output(0)
    img_out = dbvalue(id,type)
    return img_out

def image_mean(img):
  bvxm_batch.init_process("vilImageMeanProcess")
  bvxm_batch.set_input_from_db(0,img)
  bvxm_batch.run_process()
  (id,type) = bvxm_batch.commit_output(0)
  mean_val = bvxm_batch.get_output_float(id)
  bvxm_batch.remove_data(id)
  return mean_val

def crop_image(img,i0,j0,ni,nj):
  bvxm_batch.init_process("vilCropImageProcess")
  bvxm_batch.set_input_from_db(0,img)
  bvxm_batch.set_input_unsigned(1,i0)
  bvxm_batch.set_input_unsigned(2,j0)
  bvxm_batch.set_input_unsigned(3,ni)
  bvxm_batch.set_input_unsigned(4,nj)
  bvxm_batch.run_process()
  (id,type) = bvxm_batch.commit_output(0)
  img_out = dbvalue(id,type)
  return img_out

def scale_and_offset_values(img,scale,offset):
  bvxm_batch.init_process("vilScaleAndOffsetValuesProcess")
  bvxm_batch.set_input_from_db(0,img)
  bvxm_batch.set_input_float(1,scale)
  bvxm_batch.set_input_float(2,offset)
  bvxm_batch.run_process()
  return

def init_float_img(ni,nj,np,val):
  bvxm_batch.init_process("vilInitFloatImageProcess")
  bvxm_batch.set_input_unsigned(0,ni)
  bvxm_batch.set_input_unsigned(1,nj)
  bvxm_batch.set_input_unsigned(2,np)
  bvxm_batch.set_input_float(3,val)
  bvxm_batch.run_process()
  (id,type) = bvxm_batch.commit_output(0)
  img_out = dbvalue(id,type)
  return img_out

def init_byte_img(ni,nj,np = 1,val = 0):
  bvxm_batch.init_process("vilInitByteImageProcess")
  bvxm_batch.set_input_unsigned(0,ni);
  bvxm_batch.set_input_unsigned(1,nj);
  bvxm_batch.set_input_unsigned(2,np);
  bvxm_batch.set_input_unsigned(3,val);
  bvxm_batch.run_process()
  (id, type) = bvxm_batch.commit_output(0)
  img_out = dbvalue(id,type)
  return img_out

def nitf_date_time(image_filename):
  bvxm_batch.init_process("vilNITFDateTimeProcess");
  bvxm_batch.set_input_string(0,image_filename);
  bvxm_batch.run_process();
  (id,type)=bvxm_batch.commit_output(0);
  year =  bvxm_batch.get_output_int(id);
  (id,type)=bvxm_batch.commit_output(1);
  month =  bvxm_batch.get_output_int(id);
  (id,type)=bvxm_batch.commit_output(2);
  day =  bvxm_batch.get_output_int(id);
  (id,type)=bvxm_batch.commit_output(3);
  hour = bvxm_batch.get_output_int(id);
  (id,type)=bvxm_batch.commit_output(4);
  minute = bvxm_batch.get_output_int(id);
  return year, month, day, hour, minute

def undistort_image(img, param_file, iters) :
  bvxm_batch.init_process("vilUndistortImageProcess");
  bvxm_batch.set_input_from_db(0,img)
  bvxm_batch.set_input_string(1, param_file);
  bvxm_batch.set_input_int(2, iters);
  bvxm_batch.run_process();
  (o_id,o_type) = bvxm_batch.commit_output(0);
  out_img = dbvalue(o_id,o_type);
  return out_img;

def combine_eo_ir(eo_img,ir_img):
  bvxm_batch.init_process("vilEOIRCombineProcess")
  bvxm_batch.set_input_from_db(0,eo_img)
  bvxm_batch.set_input_from_db(1,ir_img)
  bvxm_batch.run_process()
  (id,type) = bvxm_batch.commit_output(0)
  img_out = dbvalue(id,type)
  return img_out

def detect_shadow_rgb(img,threshold) :
  bvxm_batch.init_process("vilShadowDetectionProcess");
  bvxm_batch.set_input_from_db(0,img)
  bvxm_batch.set_input_float(1, threshold);
  bvxm_batch.run_process();
  (o_id,o_type) = bvxm_batch.commit_output(0);
  region_img = dbvalue(o_id,o_type);
  return region_img;

def detect_shadow_ridge(region_img,blob_size_t, sun_angle) :
  bvxm_batch.init_process("vilShadowRidgeDetectionProcess");
  bvxm_batch.set_input_from_db(0,region_img)
  bvxm_batch.set_input_int(1, blob_size_t);
  bvxm_batch.set_input_float(2, sun_angle);
  bvxm_batch.run_process();
  (o_id,o_type) = bvxm_batch.commit_output(0);
  region_img = dbvalue(o_id,o_type);
  (o_id,o_type) = bvxm_batch.commit_output(1);
  out_img = dbvalue(o_id,o_type);
  (o_id,o_type) = bvxm_batch.commit_output(2);
  dist_img = dbvalue(o_id,o_type);
  return region_img, out_img, dist_img;

def binary_img_op(img1, img2, operation="sum"):
  bvxm_batch.init_process("vilBinaryImageOpProcess")
  bvxm_batch.set_input_from_db(0,img1)
  bvxm_batch.set_input_from_db(1,img2)
  bvxm_batch.set_input_string(2,operation)
  bvxm_batch.run_process()
  (id,type) = bvxm_batch.commit_output(0)
  out = dbvalue(id, type);
  return out

def img_sum(img, plane_index=0):
  bvxm_batch.init_process("vilImageSumProcess")
  bvxm_batch.set_input_from_db(0,img)
  bvxm_batch.set_input_unsigned(1,plane_index)
  bvxm_batch.run_process()
  (id,type) = bvxm_batch.commit_output(0)
  value = bvxm_batch.get_output_double(id)
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
  bvxm_batch.init_process("vilFillHolesInRegionsProcess")
  bvxm_batch.set_input_from_db(0,img)
  bvxm_batch.run_process()
  (id,type) = bvxm_batch.commit_output(0)
  outimg = dbvalue(id, type);
  return outimg

def grey_to_rgb(img, color_txt):
  bvxm_batch.init_process("vilGreyToRGBProcess")
  bvxm_batch.set_input_from_db(0,img)
  bvxm_batch.set_input_string(1,color_txt)
  result = bvxm_batch.run_process()
  if result:
    (id, type) = bvxm_batch.commit_output(0)
    outimg = dbvalue(id, type);
  else:
    outimg = 0
  return outimg

def mask_image_using_id(img, id_img, input_id):
  bvxm_batch.init_process("vilMaskImageUsingIDsProcess");
  bvxm_batch.set_input_from_db(0, img);
  bvxm_batch.set_input_from_db(1, id_img);
  bvxm_batch.set_input_unsigned(2, input_id);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  masked_img = dbvalue(id, type);
  return masked_img;

def get_plane(img, plane_id):
  bvxm_batch.init_process("vilGetPlaneProcess");
  bvxm_batch.set_input_from_db(0, img);
  bvxm_batch.set_input_unsigned(1, plane_id);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  img_plane = dbvalue(id, type);
  return img_plane;

def combine_planes(img_red, img_green, img_blue):
  bvxm_batch.init_process("vilCombinePlanesProcess");
  bvxm_batch.set_input_from_db(0, img_red);
  bvxm_batch.set_input_from_db(1, img_green);
  bvxm_batch.set_input_from_db(2, img_blue);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  img_out = dbvalue(id, type);
  return img_out;
