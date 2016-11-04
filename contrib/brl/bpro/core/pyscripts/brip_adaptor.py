# import the batch module and dbvalue from init
# set the global variable, batch, on init before importing this file
import brl_init
dbvalue = brl_init.DummyBatch()
batch = brl_init.DummyBatch()

###########################################################
# Calculate mutual information per blob between two images
###########################################################


def blobwise_mutual_info(in_img, exp_img, gradImg, blob_img):
  batch.init_process("bripBlobwiseMutualInfoProcess")
  batch.set_input_from_db(0, in_img)
  batch.set_input_from_db(1, exp_img)
  batch.set_input_from_db(2, gradImg)
  batch.set_input_from_db(3, blob_img)
  batch.run_process()
  (id, type) = batch.commit_output(0)
  vis_img = dbvalue(id, type)
  return vis_img

###########################################################
# Calculate KL Divergence per blob between two images
###########################################################


def blobwise_kl_div(p_img, q_img, blob_img, thresh=.1):
  batch.init_process("bripBlobwiseKLDivProcess")
  batch.set_input_from_db(0, p_img)
  batch.set_input_from_db(1, q_img)
  batch.set_input_from_db(2, blob_img)
  batch.set_input_float(3, thresh)
  batch.run_process()
  (id, type) = batch.commit_output(0)
  kl_img = dbvalue(id, type)
  (id, type) = batch.commit_output(1)
  thresh_img = dbvalue(id, type)
  return kl_img, thresh_img

###########################################################
# Calculate number of true blob intersections, false blobs and total true blobs
###########################################################


def blob_intersection(mp_img, gt_img):
  batch.init_process("bripBlobIntersectionProcess")
  batch.set_input_from_db(0, mp_img)
  batch.set_input_from_db(1, gt_img)
  batch.run_process()

  (id, type) = batch.commit_output(0)
  tp = batch.get_output_int(id)
  batch.remove_data(id)
  (id, type) = batch.commit_output(1)
  fp = batch.get_output_int(id)
  batch.remove_data(id)
  (id, type) = batch.commit_output(2)
  numBlobs = batch.get_output_int(id)
  batch.remove_data(id)
  return (tp, fp, numBlobs)

###########################################################
# Truncate 16 bits NITF image by ignoring certain bits
###########################################################


def truncate_nitf_image(img, is_short=True, is_scale=True):
  batch.init_process("bripTruncateNITFBitProcess")
  batch.set_input_from_db(0, img)
  batch.set_input_bool(1, is_short)
  batch.set_input_bool(2, is_scale)
  batch.run_process()
  (id, type) = batch.commit_output(0)
  out_img = dbvalue(id, type)
  return out_img

###########################################################
# Truncate 16 bits NITF image by ignoring certain bits
###########################################################


def solve_apply_gain_offset(mimg, timg, mimg_mask=None):
  batch.init_process("bripSolveGainOffsetProcess")
  batch.set_input_from_db(0, mimg)
  batch.set_input_from_db(1, timg)
  if mimg_mask is not None:
    batch.set_input_from_db(2, mimg_mask)
  out_img = None
  if batch.run_process() is not False:
    (id, type) = batch.commit_output(0)
    out_img = dbvalue(id, type)
  return out_img


def solve_apply_gain_offset_constraints(mimg, timg, weight=1.0, mimg_mask=None):
  batch.init_process("bripSolveGainOffsetConstraintsProcess")
  batch.set_input_from_db(0, mimg)
  batch.set_input_from_db(1, timg)
  batch.set_input_from_db(2, weight)
  if mimg_mask is not None:
    batch.set_input_from_db(3, mimg_mask)
  out_img = None
  if batch.run_process() is not False:
    (id, type) = batch.commit_output(0)
    out_img = dbvalue(id, type)
  return out_img

###########################################################
# Compute mutual information between two images
###########################################################


def image_mutual_info(image1, image2, min_val, max_val, n_bins):
  batch.init_process("bripImageMutualInfoProcess")
  batch.set_input_from_db(0, image1)
  batch.set_input_from_db(1, image2)
  batch.set_input_double(2, min_val)
  batch.set_input_double(3, max_val)
  batch.set_input_unsigned(4, n_bins)
  status = batch.run_process()
  if status:
    (id, type) = batch.commit_output(0)
    mutual_info = batch.get_output_double(id)
    return mutual_info
  else:
    return -1.0
