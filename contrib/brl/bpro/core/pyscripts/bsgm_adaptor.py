import brl_init
batch = brl_init.DummyBatch()
dbvalue = brl_init.DummyBatch()

##############
# Take two rectified images, generate their disparity map calculated using
# semi-global matching stereo algorithm


def sgm_matching_stereo(rect_img_1, rect_img_2, min_disparity, num_disparity,
                        out_disp_txt, num_active_disparity=40,
                        error_check_mode=1, multi_scale_mode=1, shadow_intensity = 0):
    batch.init_process("bsgmMatchingStereoProcess")
    batch.set_input_from_db(0, rect_img_1)
    batch.set_input_from_db(1, rect_img_2)
    batch.set_input_int(2, min_disparity)
    batch.set_input_int(3, num_disparity)
    batch.set_input_int(4, num_active_disparity)
    batch.set_input_int(5, error_check_mode)
    batch.set_input_int(6, multi_scale_mode)
    batch.set_input_string(7, out_disp_txt)
    batch.set_input_unsigned(8, shadow_intensity)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        out_disp_img = dbvalue(id, type)
        (id, type) = batch.commit_output(1)
        out_disp_byte_img = dbvalue(id, type)
        return out_disp_img, out_disp_byte_img
    else:
        return None, None
