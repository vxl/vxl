from bvxm_register import bvxm_batch, dbvalue

# truncate 16 bits NITF image by ignoring certain bits
# note the original NITF image will be truncated to a byte image by default


def truncate_nitf_image(img, is_byte=True, is_scale=False):
    bvxm_batch.init_process("bripTruncateNITFBitProcess")
    bvxm_batch.set_input_from_db(0, img)
    bvxm_batch.set_input_bool(1, is_byte)
    bvxm_batch.set_input_bool(2, is_scale)
    bvxm_batch.run_process()
    (id, type) = bvxm_batch.commit_output(0)
    out_img = dbvalue(id, type)
    return out_img

# compute the mutual information between two images


def image_mutual_info(image1, image2, min_val, max_val, n_bins):
    bvxm_batch.init_process("bripImageMutualInfoProcess")
    bvxm_batch.set_input_from_db(0, image1)
    bvxm_batch.set_input_from_db(1, image2)
    bvxm_batch.set_input_double(2, min_val)
    bvxm_batch.set_input_double(3, max_val)
    bvxm_batch.set_input_unsigned(4, n_bins)
    status = bvxm_batch.run_process()
    if status:
        (id, type) = bvxm_batch.commit_output(0)
        mutual_info = bvxm_batch.get_output_double(id)
        return mutual_info
    else:
        return -1.0

def phase_correlation(image1, image2, gauss_sigma = 1.0, peak_radius = 2.0, alpha = 0.5):
    bvxm_batch.init_process("bripPhaseCorrelationProcess")
    bvxm_batch.set_input_from_db(0, image1)
    bvxm_batch.set_input_from_db(1, image2)
    bvxm_batch.set_input_float(2, gauss_sigma)
    bvxm_batch.set_input_float(3, peak_radius)
    bvxm_batch.set_input_float(4, alpha)
    status = bvxm_batch.run_process()
    if status:
        (id, type) = bvxm_batch.commit_output(0)
        tu = bvxm_batch.get_output_float(id)
        (id, type) = bvxm_batch.commit_output(1)
        tv = bvxm_batch.get_output_float(id)
        (id, type) = bvxm_batch.commit_output(2)
        conf = bvxm_batch.get_output_float(id)
        (id, type) = bvxm_batch.commit_output(3)
        out_img = dbvalue(id, type)
        return status, tu, tv, conf, out_img
    else:
        return status, 0.0, 0.0, 0.0, 0
