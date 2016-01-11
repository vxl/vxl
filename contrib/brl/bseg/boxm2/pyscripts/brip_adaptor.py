from boxm2_register import boxm2_batch, dbvalue;

###########################################################
# Calculate mutual information per blob between two images
###########################################################
def blobwise_mutual_info(in_img, exp_img, gradImg, blob_img) :
  boxm2_batch.init_process("bripBlobwiseMutualInfoProcess")
  boxm2_batch.set_input_from_db(0, in_img);
  boxm2_batch.set_input_from_db(1, exp_img);
  boxm2_batch.set_input_from_db(2, gradImg);
  boxm2_batch.set_input_from_db(3, blob_img);
  boxm2_batch.run_process();
  (id,type) = boxm2_batch.commit_output(0);
  vis_img = dbvalue(id,type);
  return vis_img;

###########################################################
# Calculate KL Divergence per blob between two images
###########################################################
def blobwise_kl_div(p_img, q_img, blob_img, thresh=.1) :
  boxm2_batch.init_process("bripBlobwiseKLDivProcess")
  boxm2_batch.set_input_from_db(0, p_img);
  boxm2_batch.set_input_from_db(1, q_img);
  boxm2_batch.set_input_from_db(2, blob_img);
  boxm2_batch.set_input_float(3, thresh);
  boxm2_batch.run_process();
  (id,type) = boxm2_batch.commit_output(0);
  kl_img = dbvalue(id,type);
  (id,type) = boxm2_batch.commit_output(1);
  thresh_img = dbvalue(id,type);
  return kl_img, thresh_img;

###########################################################
# Calculate number of true blob intersections, false blobs and total true blobs
###########################################################
def blob_intersection(mp_img, gt_img):
  boxm2_batch.init_process("bripBlobIntersectionProcess")
  boxm2_batch.set_input_from_db(0, mp_img);
  boxm2_batch.set_input_from_db(1, gt_img);
  boxm2_batch.run_process();

  (id,type) = boxm2_batch.commit_output(0);
  tp = boxm2_batch.get_output_int(id);
  boxm2_batch.remove_data(id);
  (id,type) = boxm2_batch.commit_output(1);
  fp = boxm2_batch.get_output_int(id);
  boxm2_batch.remove_data(id);
  (id,type) = boxm2_batch.commit_output(2);
  numBlobs = boxm2_batch.get_output_int(id);
  boxm2_batch.remove_data(id);
  return (tp, fp, numBlobs)

###########################################################
# Truncate 16 bits NITF image by ignoring certain bits
###########################################################
def truncate_nitf_image(img, is_short = True, is_scale = True):
  boxm2_batch.init_process("bripTruncateNITFBitProcess");
  boxm2_batch.set_input_from_db(0, img);
  boxm2_batch.set_input_bool(1, is_short);
  boxm2_batch.set_input_bool(2, is_scale);
  boxm2_batch.run_process();
  (id, type) = boxm2_batch.commit_output(0);
  out_img = dbvalue(id, type);
  return out_img;

###########################################################
# Truncate 16 bits NITF image by ignoring certain bits
###########################################################
def solve_apply_gain_offset(mimg,timg,mimg_mask= None):
  boxm2_batch.init_process("bripSolveGainOffsetProcess")
  boxm2_batch.set_input_from_db(0, mimg);
  boxm2_batch.set_input_from_db(1, timg);
  if mimg_mask is not None:
    boxm2_batch.set_input_from_db(2, mimg_mask);
  out_img = None
  if boxm2_batch.run_process() is not False:
    (id, type) = boxm2_batch.commit_output(0);
    out_img = dbvalue(id, type);
  return out_img;

def solve_apply_gain_offset_constraints(mimg,timg,weight = 1.0, mimg_mask= None):
  boxm2_batch.init_process("bripSolveGainOffsetConstraintsProcess")
  boxm2_batch.set_input_from_db(0, mimg);
  boxm2_batch.set_input_from_db(1, timg);
  boxm2_batch.set_input_from_db(2, weight);
  if mimg_mask is not None:
    boxm2_batch.set_input_from_db(3, mimg_mask);
  out_img = None
  if boxm2_batch.run_process() is not False:
    (id, type) = boxm2_batch.commit_output(0);
    out_img = dbvalue(id, type);
  return out_img;

###########################################################
# Compute mutual information between two images
###########################################################
def image_mutual_info(image1, image2, min_val, max_val, n_bins):
  boxm2_batch.init_process("bripImageMutualInfoProcess")
  boxm2_batch.set_input_from_db(0, image1)
  boxm2_batch.set_input_from_db(1, image2)
  boxm2_batch.set_input_double(2, min_val)
  boxm2_batch.set_input_double(3, max_val)
  boxm2_batch.set_input_unsigned(4, n_bins)
  status = boxm2_batch.run_process()
  if status:
    (id, type) = boxm2_batch.commit_output(0)
    mutual_info = boxm2_batch.get_output_double(id)
    return mutual_info
  else:
    return -1.0
