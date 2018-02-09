# import the batch module and dbvalue from init
# set the global variable, batch,  on init before importing this file
import brl_init
dbvalue = brl_init.DummyBatch()
batch = brl_init.DummyBatch()

###################################################
# Vil loading and saving
###################################################
# define the batch and dbvalue variables


def load_image(file_path):
    batch.init_process("vilLoadImageViewProcess")
    batch.set_input_string(0, file_path)
    status = batch.run_process()
    img = ni = nj = None
    if status:
        (id, type) = batch.commit_output(0)
        (ni_id, ni_type) = batch.commit_output(1)
        (nj_id, nj_type) = batch.commit_output(2)
        ni = batch.get_output_unsigned(ni_id)
        nj = batch.get_output_unsigned(nj_id)
        img = dbvalue(id, type)
        batch.remove_data(ni_id)
        batch.remove_data(nj_id)
    return img, ni, nj


def load_image_resource(file_path):
    batch.init_process("vilLoadImageResourceProcess")
    batch.set_input_string(0, file_path)
    status = batch.run_process()
    img_res = ni = nj = None
    if status:
        (id, type) = batch.commit_output(0)
        (ni_id, ni_type) = batch.commit_output(1)
        (nj_id, nj_type) = batch.commit_output(2)
        ni = batch.get_output_unsigned(ni_id)
        nj = batch.get_output_unsigned(nj_id)
        img_res = dbvalue(id, type)
        batch.remove_data(ni_id)
        batch.remove_data(nj_id)
    return img_res, ni, nj


def save_image(img, file_path):
    assert not isinstance(list, tuple)
    batch.init_process("vilSaveImageViewProcess")
    batch.set_input_from_db(0, img)
    batch.set_input_string(1, file_path)
    batch.run_process()


def save_image_resource(resc, file_path):
    batch.init_process("vilSaveImageResourceProcess")
    batch.set_input_from_db(0, resc)
    batch.set_input_string(1, file_path)
    status = batch.run_process()
    return status


def multi_plane_view_to_grey(resc, apply_mask=False):
    batch.init_process("vilMultiPlaneViewToGreyProcess")
    batch.set_input_from_db(0, resc)
    batch.set_input_bool(1, apply_mask)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        grey_resc = dbvalue(id, type)
        return grey_resc
    return None


def convert_image(img, type="byte"):
    batch.init_process("vilConvertPixelTypeProcess")
    batch.set_input_from_db(0, img)
    batch.set_input_string(1, type)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    cimg = dbvalue(id, type)
    return cimg

################################
# BAE raw file image stream
################################


def bae_raw_stream(file_path, ni=0, nj=0, pixelsize=0):
    batch.init_process("bilCreateRawImageIstreamProcess")
    batch.set_input_string(0, file_path)
    batch.set_input_int(1, ni)
    batch.set_input_int(2, nj)
    batch.set_input_int(3, pixelsize)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    stream = dbvalue(id, type)
    (id, type) = batch.commit_output(1)
    numImgs = batch.get_output_int(id)
    batch.remove_data(id)
    return stream, numImgs


def next_frame(rawStream):
    batch.init_process("bilReadFrameProcess")
    batch.set_input_from_db(0, rawStream)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    img = dbvalue(id, type)
    #(id, type) = batch.commit_output(1);
    #time = batch.get_output_unsigned(id);
    return img


def seek_frame(rawStream, frame):
    batch.init_process("bilSeekFrameProcess")
    batch.set_input_from_db(0, rawStream)
    batch.set_input_unsigned(1, frame)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    img = dbvalue(id, type)
    #(id, type) = batch.commit_output(1);
    #time = batch.get_output_unsigned(id);
    return img


def arf_stream(file_path):
    batch.init_process("bilCreateArfImageIstreamProcess")
    batch.set_input_string(0, file_path)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    stream = dbvalue(id, type)
    (id, type) = batch.commit_output(1)
    numImgs = batch.get_output_int(id)
    batch.remove_data(id)
    return stream, numImgs


def arf_next_frame(rawStream):
    batch.init_process("bilArfReadFrameProcess")
    batch.set_input_from_db(0, rawStream)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    img = dbvalue(id, type)
    (id, type) = batch.commit_output(1)
    time = batch.get_output_unsigned(id)
    batch.remove_data(id)
    return img, time


def arf_seek_frame(rawStream, frame):
    batch.init_process("bilArfSeekFrameProcess")
    batch.set_input_from_db(0, rawStream)
    batch.set_input_unsigned(1, frame)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    img = dbvalue(id, type)
    (id, type) = batch.commit_output(1)
    time = batch.get_output_unsigned(id)
    batch.remove_data(id)
    return img, time


def read_CLIF07(indir, outdir, camnum, datatype="CLIF06"):
    batch.init_process("bilReadCLIF07DataProcess")
    batch.set_input_string(0, indir)
    batch.set_input_string(1, outdir)
    batch.set_input_int(2, camnum)
    batch.set_input_string(3, datatype)
    batch.run_process()


def debayer(img):
    batch.init_process("vilDebayerBGGRToRGBProcess")
    batch.set_input_from_db(0, img)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    outimg = dbvalue(id, type)
    return outimg


# pixel wise roc process for change detection images
def pixel_wise_roc(cd_img, gt_img, mask_img=None):
    batch.init_process("vilPixelwiseRocProcess")
    batch.set_input_from_db(0, cd_img)
    batch.set_input_from_db(1, gt_img)
    if mask_img:
        batch.set_input_from_db(2, mask_img)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    tp = batch.get_bbas_1d_array_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(1)
    tn = batch.get_bbas_1d_array_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(2)
    fp = batch.get_bbas_1d_array_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(3)
    fn = batch.get_bbas_1d_array_float(id)
    (id, type) = batch.commit_output(4)
    tpr = batch.get_bbas_1d_array_float(id)
    (id, type) = batch.commit_output(5)
    fpr = batch.get_bbas_1d_array_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(6)
    outimg = dbvalue(id, type)
    # return tuple of true positives, true negatives, false positives, etc..
    return (tp, tn, fp, fn, outimg)


# get image pixel value (always 0-1 float)
def pixel(img, point):
    batch.init_process("vilPixelValueProcess")
    batch.set_input_from_db(0, img)
    batch.set_input_int(1, int(point[0]))
    batch.set_input_int(2, int(point[1]))
    batch.run_process()
    (id, type) = batch.commit_output(0)
    val = batch.get_output_float(id)
    batch.remove_data(id)
    return val


# resize image (default returns float image
def resize(img, ni, nj, pixel="float"):
    batch.init_process("vilResampleProcess")
    batch.set_input_from_db(0, img)
    batch.set_input_int(1, ni)
    batch.set_input_int(2, nj)
    batch.set_input_string(3, pixel)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    img = dbvalue(id, type)
    return img

# get image dimensions


def image_size(img):
    batch.init_process('vilImageSizeProcess')
    batch.set_input_from_db(0, img)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    ni = batch.get_output_unsigned(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(1)
    nj = batch.get_output_unsigned(id)
    batch.remove_data(id)
    return ni, nj


def image_range(img):
    batch.init_process('vilImageRangeProcess')
    batch.set_input_from_db(0, img)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    minVal = batch.get_output_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(1)
    maxVal = batch.get_output_float(id)
    batch.remove_data(id)
    return minVal, maxVal


def gradient(img):
    batch.init_process('vilGradientProcess')
    batch.set_input_from_db(0, img)
    batch.run_process()
    # x image
    (id, type) = batch.commit_output(0)
    dIdx = dbvalue(id, type)
    # y image
    (id, type) = batch.commit_output(1)
    dIdy = dbvalue(id, type)
    # mag image
    (id, type) = batch.commit_output(2)
    magImg = dbvalue(id, type)
    return dIdx, dIdy, magImg


def gradient_angle(Ix, Iy):
    batch.init_process('vilGradientAngleProcess')
    batch.set_input_from_db(0, Ix)
    batch.set_input_from_db(1, Iy)
    batch.run_process()
    # x image
    (id, type) = batch.commit_output(0)
    angleImg = dbvalue(id, type)
    return angleImg


def threshold_image(img, value, threshold_above=True, id=255):
    batch.init_process("vilThresholdImageProcess")
    batch.set_input_from_db(0, img)
    batch.set_input_float(1, value)
    batch.set_input_bool(2, threshold_above)
    batch.set_input_unsigned(3, id)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    mask = dbvalue(id, type)
    return mask


def max_threshold_image(img, threshold):
    batch.init_process("vilThresholdMaxImageProcess")
    batch.set_input_from_db(0, img)
    batch.set_input_float(1, threshold)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    mask = dbvalue(id, type)
    return mask


def threshold_image_inside(img, min_thres, max_thres, threshold_inside=True):
    batch.init_process("vilThresholdImageInsideProcess")
    batch.set_input_from_db(0, img)
    batch.set_input_float(1, min_thres)
    batch.set_input_float(2, max_thres)
    batch.set_input_bool(3, threshold_inside)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    mask = dbvalue(id, type)
    return mask


def stretch_image(img, min_value, max_value, output_type_str='float'):
    batch.init_process("vilStretchImageProcess")
    batch.set_input_from_db(0, img)
    batch.set_input_float(1, min_value)
    batch.set_input_float(2, max_value)
    batch.set_input_string(3, output_type_str)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    img_out = dbvalue(id, type)
    return img_out


def truncate_image(img, min_value, max_value):
    batch.init_process("vilTruncateImageProcess")
    batch.set_input_from_db(0, img)
    batch.set_input_float(1, min_value)
    batch.set_input_float(2, max_value)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    img_out = dbvalue(id, type)
    return img_out


def normalize_image(img):
    batch.init_process("vilImageNormaliseProcess")
    batch.set_input_from_db(0, img)
    batch.run_process()


def image_mean(img):
    batch.init_process("vilImageMeanProcess")
    batch.set_input_from_db(0, img)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    mean_val = batch.get_output_float(id)
    batch.remove_data(id)
    return mean_val


def compute_image_mean_and_variance(img, n):
    batch.init_process("vilMeanAndVarianceImageProcess")
    batch.set_input_from_db(0, img)
    batch.set_input_unsigned(1, n)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        img_mean = dbvalue(id, type)
        (id, type) = batch.commit_output(1)
        img_var = dbvalue(id, type)
        return img_mean, img_var
    else:
        return None, None


def crop_image(img, i0, j0, ni, nj):
    batch.init_process("vilCropImageProcess")
    batch.set_input_from_db(0, img)
    batch.set_input_unsigned(1, i0)
    batch.set_input_unsigned(2, j0)
    batch.set_input_unsigned(3, ni)
    batch.set_input_unsigned(4, nj)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    img_out = dbvalue(id, type)
    return img_out


def crop_image_res(img_res, i0, j0, ni, nj):
    batch.init_process("vilCropImageResProcess")
    batch.set_input_from_db(0, img_res)
    batch.set_input_unsigned(1, i0)
    batch.set_input_unsigned(2, j0)
    batch.set_input_unsigned(3, ni)
    batch.set_input_unsigned(4, nj)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    img_out = dbvalue(id, type)
    return img_out


def scale_and_offset_values(img, scale, offset):
    batch.init_process("vilScaleAndOffsetValuesProcess")
    batch.set_input_from_db(0, img)
    batch.set_input_float(1, scale)
    batch.set_input_float(2, offset)
    batch.run_process()
    return


def init_byte_img(ni, nj, np, val):
    batch.init_process("vilInitByteImageProcess")
    batch.set_input_unsigned(0, ni)
    batch.set_input_unsigned(1, nj)
    batch.set_input_unsigned(2, np)
    batch.set_input_unsigned(3, val)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    img_out = dbvalue(id, type)
    return img_out


def init_float_img(ni, nj, np, val):
    batch.init_process("vilInitFloatImageProcess")
    batch.set_input_unsigned(0, ni)
    batch.set_input_unsigned(1, nj)
    batch.set_input_unsigned(2, np)
    batch.set_input_float(3, val)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    img_out = dbvalue(id, type)
    return img_out


def nitf_date_time(image_filename):
    batch.init_process("vilNITFDateTimeProcess")
    batch.set_input_string(0, image_filename)
    status = batch.run_process()
    year = month = day = hour = minute = None
    if status:
        (id, type) = batch.commit_output(0)
        year = batch.get_output_int(id)
        batch.remove_data(id)
        (id, type) = batch.commit_output(1)
        month = batch.get_output_int(id)
        batch.remove_data(id)
        (id, type) = batch.commit_output(2)
        day = batch.get_output_int(id)
        batch.remove_data(id)
        (id, type) = batch.commit_output(3)
        hour = batch.get_output_int(id)
        batch.remove_data(id)
        (id, type) = batch.commit_output(4)
        minute = batch.get_output_int(id)
        batch.remove_data(id)
    return year, month, day, hour, minute


def undistort_image(img, param_file, iters):
    batch.init_process("vilUndistortImageProcess")
    batch.set_input_from_db(0, img)
    batch.set_input_string(1, param_file)
    batch.set_input_int(2, iters)
    batch.run_process()
    (o_id, o_type) = batch.commit_output(0)
    out_img = dbvalue(o_id, o_type)
    return out_img


def combine_eo_ir(eo_img, ir_img):
    batch.init_process("vilEOIRCombineProcess")
    batch.set_input_from_db(0, eo_img)
    batch.set_input_from_db(1, ir_img)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    img_out = dbvalue(id, type)
    return img_out


def detect_shadow_rgb(img, threshold):
    batch.init_process("vilShadowDetectionProcess")
    batch.set_input_from_db(0, img)
    batch.set_input_float(1, threshold)
    batch.run_process()
    (o_id, o_type) = batch.commit_output(0)
    region_img = dbvalue(o_id, o_type)
    return region_img


def detect_shadow_ridge(region_img, blob_size_t, sun_angle):
    batch.init_process("vilShadowRidgeDetectionProcess")
    batch.set_input_from_db(0, region_img)
    batch.set_input_int(1, blob_size_t)
    batch.set_input_float(2, sun_angle)
    batch.run_process()
    (o_id, o_type) = batch.commit_output(0)
    region_img = dbvalue(o_id, o_type)
    (o_id, o_type) = batch.commit_output(1)
    out_img = dbvalue(o_id, o_type)
    (o_id, o_type) = batch.commit_output(2)
    dist_img = dbvalue(o_id, o_type)
    return region_img, out_img, dist_img


def binary_img_op(img1, img2, operation="sum"):
    batch.init_process("vilBinaryImageOpProcess")
    batch.set_input_from_db(0, img1)
    batch.set_input_from_db(1, img2)
    batch.set_input_string(2, operation)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    out = dbvalue(id, type)
    return out


def img_sum(img, plane_index=0):
    batch.init_process("vilImageSumProcess")
    batch.set_input_from_db(0, img)
    batch.set_input_unsigned(1, plane_index)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    value = batch.get_output_double(id)
    batch.remove_data(id)
    return value

# input a visibility image with float values in 0,1 range, negate this
# image and threshold to generate a byte image as a mask


def prepare_mask_image_from_vis_image(vis_image, ni2, nj2, threshold):
    img_1 = init_float_img(ni2, nj2, 1, -1.0)
    vis_image_neg = binary_img_op(img_1, vis_image, "sum")
    scale_and_offset_values(vis_image_neg, -1.0, 0.0)
    exp_img_mask_f = threshold_image(vis_image_neg, threshold)
    sum = img_sum(exp_img_mask_f)
    # print "mask sum: " + str(sum) + " ratio of true: " + str(sum/(ni2*nj2));
    ratio = sum / (ni2 * nj2) * 100
    exp_img_mask = stretch_image(exp_img_mask_f, 0, 1, 'byte')
    batch.remove_data(exp_img_mask_f.id)
    return exp_img_mask, img_1, vis_image_neg, ratio


def fill_holes(img):
    batch.init_process("vilFillHolesInRegionsProcess")
    batch.set_input_from_db(0, img)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    outimg = dbvalue(id, type)
    return outimg


def grey_to_rgb(img, color_txt):
    batch.init_process("vilGreyToRGBProcess")
    batch.set_input_from_db(0, img)
    batch.set_input_string(1, color_txt)
    result = batch.run_process()
    if result:
        (id, type) = batch.commit_output(0)
        outimg = dbvalue(id, type)
    else:
        outimg = 0
    return outimg


def rgb_to_grey(img):
    batch.init_process("vilRGBToGreyProcess")
    batch.set_input_from_db(0, img)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        outimg = dbvalue(id, type)
        return outimg
    else:
        return None


def mask_image_using_id(img, id_img, input_id):
    batch.init_process("vilMaskImageUsingIDsProcess")
    batch.set_input_from_db(0, img)
    batch.set_input_from_db(1, id_img)
    batch.set_input_unsigned(2, input_id)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    masked_img = dbvalue(id, type)
    return masked_img


def get_plane(img, plane_id):
    batch.init_process("vilGetPlaneProcess")
    batch.set_input_from_db(0, img)
    batch.set_input_unsigned(1, plane_id)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    img_plane = dbvalue(id, type)
    return img_plane


def get_number_of_planes(img):
    batch.init_process("vilGetNumberOfPlanesProcess")
    batch.set_input_from_db(0, img)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    n_planes = batch.get_output_unsigned(id)
    batch.remove_data(id)
    return n_planes


def combine_planes(img_red, img_green, img_blue):
    batch.init_process("vilCombinePlanesProcess")
    batch.set_input_from_db(0, img_red)
    batch.set_input_from_db(1, img_green)
    batch.set_input_from_db(2, img_blue)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    img_out = dbvalue(id, type)
    return img_out

# combine them in the given order in the output image


def combine_planes2(img_blue, img_green, img_red, img_nir):
    batch.init_process("vilCombinePlanesProcess2")
    batch.set_input_from_db(0, img_blue)
    batch.set_input_from_db(1, img_green)
    batch.set_input_from_db(2, img_red)
    batch.set_input_from_db(3, img_nir)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    img_out = dbvalue(id, type)
    return img_out


# combine 8 bands into one output image
# note that the band order in output image is same as input and user is
# responsible for the passing sequence of image bands
def combine_planes_8_bands(img_coastal, img_blue, img_green, img_yellow,
                           img_red, img_red_edge, img_nir1, img_nir2):
    batch.init_process("vilCombinePlanes8BandsProcess")
    batch.set_input_from_db(0, img_coastal)
    batch.set_input_from_db(1, img_blue)
    batch.set_input_from_db(2, img_green)
    batch.set_input_from_db(3, img_yellow)
    batch.set_input_from_db(4, img_red)
    batch.set_input_from_db(5, img_red_edge)
    batch.set_input_from_db(6, img_nir1)
    batch.set_input_from_db(7, img_nir2)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        out_img = dbvalue(id, type)
        return out_img
    else:
        return 0


def image_entropy(img, block_size=5, bins=16):
    batch.init_process("vilBlockEntropyProcess")
    batch.set_input_from_db(0, img)
    batch.set_input_unsigned(1, block_size)
    batch.set_input_unsigned(2, bins)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        entropy_img = dbvalue(id, type)
        return entropy_img
    else:
        return None


def edge_detection(img, noise_multiplier=1.5, smooth=1.5, auto_threshold=False,
                   junctionp=False, aggressive_junc_closure=False):
    batch.init_process("vilEdgeDetectionProcess")
    batch.set_input_from_db(0, img)
    batch.set_input_float(1, noise_multiplier)
    batch.set_input_float(2, smooth)
    batch.set_input_bool(3, auto_threshold)
    batch.set_input_bool(4, junctionp)
    batch.set_input_bool(5, aggressive_junc_closure)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        edge_img = dbvalue(id, type)
        return edge_img
    else:
        return None


def median_filter_image(img, neighborhood_radius):
    batch.init_process("vilMedianFilterProcess")
    batch.set_input_from_db(0, img)
    batch.set_input_int(1, neighborhood_radius)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    filt_img = dbvalue(id, type)
    return filt_img


def binary_edge_detection(img, max_size, min_size, threshold_id=255):
    batch.init_process("vilBinaryEdgeDetectionProcess")
    batch.set_input_from_db(0, img)
    batch.set_input_unsigned(1, max_size)
    batch.set_input_unsigned(2, min_size)
    batch.set_input_unsigned(3, threshold_id)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    edge_img = dbvalue(id, type)
    return edge_img


def BGR_to_RGB(inimg):
    batch.init_process("vilBGRToRGBProcess")
    batch.set_input_from_db(0, inimg)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    out_img = dbvalue(id, type)
    return out_img


def histogram_equalize(img):
    batch.init_process("vilHistogramEqualizeProcess")
    batch.set_input_from_db(0, img)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    img_equalized = dbvalue(id, type)
    return img_equalized


def invert_float_image(img):
    batch.init_process("vilInvertFloatImageProcess")
    batch.set_input_from_db(0, img)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    inverted_img = dbvalue(id, type)
    return inverted_img

# remote the invalid pixel region existed in satellite imagery


def remove_nitf_margin():
    batch.init_process("vilNITFRemoveMarginProcess")
    batch.set_input_from_db(0, img_res)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        vi = batch.get_output_unsigned(id)
        (id, type) = batch.commit_output(1)
        vj = batch.get_output_unsigned(id)
        (id, type) = batch.commit_output(2)
        vni = batch.get_output_unsigned(id)
        (id, type) = batch.commit_output(3)
        vnj = batch.get_output_unsigned(id)
        return vi, vj, vni, vnj
    else:
        return 0, 0, 0, 0


# register a source image to a target image by explicitly minimizing the
# root-mean-square-error (RMSE) of pixel value difference
def img_registration_by_rmse(src_img, tgr_img, sx, sy, sz=0.0, pixel_res=1.0,
                             invalid_pixel=-9999.0, mask_img=None):
    batch.init_process("vilImageRegistrationProcess")
    batch.set_input_from_db(0, src_img)
    batch.set_input_from_db(1, tgr_img)
    batch.set_input_unsigned(2, sx)
    batch.set_input_unsigned(3, sy)
    batch.set_input_double(4, sz)
    batch.set_input_double(5, pixel_res)
    batch.set_input_float(6, invalid_pixel)
    if mask_img:
        batch.set_input_from_db(7, mask_img)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        trans_x = batch.get_output_double(id)
        (id, type) = batch.commit_output(1)
        trans_y = batch.get_output_double(id)
        (id, type) = batch.commit_output(2)
        trans_z = batch.get_output_double(id)
        (id, type) = batch.commit_output(3)
        rmse_z = batch.get_output_double(id)
        (id, type) = batch.commit_output(4)
        var_z = batch.get_output_double(id)
        return trans_x, trans_y, trans_z, rmse_z, var_z
    else:
        return -1.0, -1.0, -1.0, -1.0, -1.0

# Dilate a binary image using a disk structural element


def dilate_image_disk(in_img, disk_size):
    batch.init_process("vilImageDilateDiskProcess")
    batch.set_input_from_db(0, in_img)
    batch.set_input_float(1, disk_size)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        out_img = dbvalue(id, type)
        return out_img
    else:
        return None

def binarize_using_otsu_threshold(in_img, orange, obins, margin=0, invalid_pix = 0.0):
    batch.init_process("vilBinarizeOtsuProcess")
    batch.set_input_from_db(0, in_img)
    batch.set_input_double(1, orange)
    batch.set_input_int(2, obins)
    batch.set_input_int(3, margin)
    batch.set_input_double(4, invalid_pix)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        out_img = dbvalue(id, type)
        (id, type) = batch.commit_output(1)
        dt = batch.get_output_double(id)
        return out_img, dt
    else:
        return None, 0

def blob_detection(img, min_size, max_size):
    batch.init_process("vilBlobDetectionProcess")
    batch.set_input_from_db(0, img)
    batch.set_input_unsigned(1, min_size)
    batch.set_input_unsigned(2, max_size)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    out = dbvalue(id, type)
    (id, type) = batch.commit_output(1)
    out_color = dbvalue(id, type)
    (id, type) = batch.commit_output(2)
    cnt = batch.get_output_unsigned(id)
    return out, out_color, cnt
