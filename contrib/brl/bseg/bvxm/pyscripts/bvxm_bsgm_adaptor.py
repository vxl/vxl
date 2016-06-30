from bvxm_register import bvxm_batch, dbvalue

##############
## Take two rectified images, generate their disparity map calculated using semi-global matching stereo algorithm
def sgm_matching_stereo(rect_img_1, rect_img_2, min_disparity, num_disparity, out_disp_txt, num_active_disparity = 40, error_check_mode = 1,
                        multi_scale_mode = 1):
  bvxm_batch.init_process("bsgmMatchingStereoProcess")
  bvxm_batch.set_input_from_db(0, rect_img_1)
  bvxm_batch.set_input_from_db(1, rect_img_2)
  bvxm_batch.set_input_int(2, min_disparity)
  bvxm_batch.set_input_int(3, num_disparity)
  bvxm_batch.set_input_int(4, num_active_disparity)
  bvxm_batch.set_input_int(5, error_check_mode)
  bvxm_batch.set_input_int(6, multi_scale_mode)
  bvxm_batch.set_input_string(7, out_disp_txt)
  status = bvxm_batch.run_process()
  if status:
    (id, type) = bvxm_batch.commit_output(0)
    out_disp_img = dbvalue(id, type)
    (id, type) = bvxm_batch.commit_output(1)
    out_disp_byte_img = dbvalue(id, type)
    return out_disp_img, out_disp_byte_img
  else:
    return None, None