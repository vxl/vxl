from boxm2_adaptor import boxm2_batch, dbvalue

##############
## Take two rectified images, generate their disparity map calculated using semi-global matching stereo algorithm
def sgm_matching_stereo(rect_img_1, rect_img_2, min_disparity, num_disparity, out_disp_txt, num_active_disparity = 40, error_check_mode = 1):
  boxm2_batch.init_process("bsgmMatchingStereoProcess")
  boxm2_batch.set_input_from_db(0, rect_img_1)
  boxm2_batch.set_input_from_db(1, rect_img_2)
  boxm2_batch.set_input_int(2, min_disparity)
  boxm2_batch.set_input_int(3, num_disparity)
  boxm2_batch.set_input_int(4, num_active_disparity)
  boxm2_batch.set_input_int(5, error_check_mode)
  boxm2_batch.set_input_string(6, out_disp_txt)
  status = boxm2_batch.run_process()
  if status:
    (id, type) = boxm2_batch.commit_output(0)
    out_disp_img = dbvalue(id, type)
    (id, type) = boxm2_batch.commit_output(1)
    out_disp_byte_img = dbvalue(id, type)
    return out_disp_img, out_disp_byte_img
  else:
    return None, None