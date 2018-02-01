# import the batch module and dbvalue from init
# set the global variable, batch, on init before importing this file
import brl_init
dbvalue = brl_init.DummyBatch()
batch = brl_init.DummyBatch()

#############################################################################
# PROVIDES higher level sdet python functions to make batch
# code more readable/refactored
#############################################################################

import math


def create_classifier(lambda0, lambda1, n_scales, scale_interval, angle_interval, laplace_radius, gauss_radius, k, n_samples):
    batch.init_process("sdetCreateTextureClassifierProcess")
    batch.set_input_float(0, lambda0)      # lambda0
    batch.set_input_float(1, lambda1)      # lambda1
    batch.set_input_unsigned(2, n_scales)     # n_scales
    batch.set_input_float(3, scale_interval)    # scale_interval
    batch.set_input_float(4, angle_interval)    # angle_interval
    batch.set_input_float(5, laplace_radius)     # laplace_radius
    batch.set_input_float(6, gauss_radius)     # gauss_radius
    batch.set_input_unsigned(7, k)  # k
    batch.set_input_unsigned(8, n_samples)  # number of samples
    batch.run_process()
    (tclsf_id, tclsf_type) = batch.commit_output(0)
    tclsf = dbvalue(tclsf_id, tclsf_type)
    return tclsf


def save_classifier(tclsf, classifier_name):
    batch.init_process("sdetSaveTextureClassifierProcess")
    batch.set_input_from_db(0, tclsf)      # classifier instance
    batch.set_input_string(1, classifier_name)
    batch.run_process()


def load_classifier(classifier_name):
    batch.init_process("sdetLoadTextureClassifierProcess")
    batch.set_input_string(0, classifier_name)
    batch.run_process()
    (tclsf_id, tclsf_type) = batch.commit_output(0)
    tclsf = dbvalue(tclsf_id, tclsf_type)
    return tclsf


def load_dictionary(dictionary_name):
    batch.init_process("sdetLoadTextureDictionaryProcess")
    batch.set_input_string(0, dictionary_name)
    batch.run_process()
    (tclsf_id, tclsf_type) = batch.commit_output(0)
    tclsf = dbvalue(tclsf_id, tclsf_type)
    return tclsf

def print_texton_dict(tclsf, dictionary_bin, print_mode = 'histograms'):
    batch.init_process("sdetPrintTextonDictProcess")
    batch.set_input_from_db(0, tclsf)          # classifier instance
    batch.set_input_string(1, dictionary_bin)  # trained texton dictionary
    batch.set_input_string(2, print_mode)      # can be 'histogram', 'textons', 'inter_probs' or 'inter_dist'
    batch.run_process()
    return

def extract_filter_bank(tclsf, img_name, filter_folder):
    batch.init_process("sdetExtractFilterBankProcess")
    batch.set_input_from_db(0, tclsf)      # classifier instance
    batch.set_input_string(1, img_name)
    batch.set_input_string(2, filter_folder)
    batch.run_process()

# extract the filter bank from float image view without saving it, assumes the image is scaled to [0,1] appropriately
def extract_filter_bank_img(tclsf, img):
    batch.init_process("sdetExtractFilterBankImgProcess")
    batch.set_input_from_db(0, tclsf)      # classifier instance
    batch.set_input_from_db(1, img)
    status = batch.run_process()
    return status

def add_to_filter_bank(tclsf, img_name, plane, filter_folder, filter_name, is_gauss_smooth=True):
    batch.init_process("sdetAddtoFilterBankProcess")
    batch.set_input_from_db(0, tclsf)      # classifier instance
    batch.set_input_string(1, img_name)
    # pass which plane to extract the gauss response from
    batch.set_input_unsigned(2, plane)
    batch.set_input_string(3, filter_folder)
    # pass a unique name to be used to write to filter_folder
    batch.set_input_string(4, filter_name)
    batch.set_input_bool(5, is_gauss_smooth)
    batch.run_process()


def add_responses_to_filter_bank(tclsf, img_name, img, filter_folder, filter_name):
    batch.init_process("sdetAddResponsestoFilterBankProcess")
    batch.set_input_from_db(0, tclsf)      # classifier instance
    batch.set_input_string(1, img_name)
    batch.set_input_from_db(2, img)
    batch.set_input_string(3, filter_folder)
    # pass a unique name to be used to write to filter_folder
    batch.set_input_string(4, filter_name)
    batch.run_process()


def add_to_filter_bank2(tclsf, img_name, filter_folder):
    batch.init_process("sdetAddtoFilterBankProcess2")
    batch.set_input_from_db(0, tclsf)      # classifier instance
    batch.set_input_string(1, img_name)
    batch.set_input_string(2, filter_folder)
    batch.run_process()

# assumes that the training will be on the same image and the filter bank
# of it is already computed and saved at tclsf, e.g. using
# extract_filter_bank method


def train_classifier(tclsf, poly_file, category_name, dictionary_name, compute_category_textons, finish_and_write_dictionary):
    batch.init_process("sdetTextureTrainingProcess2")
    batch.set_input_from_db(0, tclsf)
    if compute_category_textons:
        # compute the textons if this is the last polygon file for the category
        batch.set_input_bool(1, 1)
    else:
        batch.set_input_bool(1, 0)
    batch.set_input_string(2, poly_file)
    batch.set_input_string(3, category_name)
    batch.set_input_string(4, dictionary_name)
    batch.run_process()
    (tclsf_id, tclsf_type) = batch.commit_output(0)
    tclsf = dbvalue(tclsf_id, tclsf_type)
    if finish_and_write_dictionary:
        batch.finish_process()
    return tclsf


def dump_binary_data_as_txt(poly_file, output_file):
    batch.init_process("sdetDumpVsolBinaryDataProcess")
    batch.set_input_string(0, poly_file)
    batch.set_input_string(1, output_file)
    batch.run_process()


def test_classifier(tclsf, block_size, category_id_file="", category_name=""):
    batch.init_process("sdetTextureClassifierProcess2")
    batch.set_input_from_db(0, tclsf)
    batch.set_input_unsigned(1, block_size)
    batch.set_input_string(2, category_id_file)
    batch.set_input_string(3, category_name)
    status = batch.run_process()
    if status:
        (out_id, out_type) = batch.commit_output(0)
        out_max_prob_img = dbvalue(out_id, out_type)
        (out_id, out_type) = batch.commit_output(1)
        out_color_img = dbvalue(out_id, out_type)
        (out_id, out_type) = batch.commit_output(2)
        out_id_img = dbvalue(out_id, out_type)
        (out_id, out_type) = batch.commit_output(3)
        out_cat_prob_img = dbvalue(out_id, out_type)
        return out_max_prob_img, out_color_img, out_id_img, out_cat_prob_img
    else:
        return None, None, None, None

def test_classifier_unsupervised(tclsf):
    batch.init_process("sdetTextureUnsupervisedClassifierProcess")
    batch.set_input_from_db(0, tclsf)
    status = batch.run_process()
    if status:
        (out_id, out_type) = batch.commit_output(0)
        out_color_img = dbvalue(out_id, out_type)
        (out_id, out_type) = batch.commit_output(1)
        out_id_img = dbvalue(out_id, out_type)
        return out_color_img, out_id_img
    else:
        return None

def test_classifier_clouds(tclsf, dictionary_name, image_resource, i, j, ni, nj, block_size, percent_cat_name, category_id_file="", scale_factor=1.0 / 2048.0):
    batch.init_process("sdetTextureClassifySatelliteCloudsProcess")
    batch.set_input_from_db(0, tclsf)
    batch.set_input_string(1, dictionary_name)
    batch.set_input_from_db(2, image_resource)
    batch.set_input_unsigned(3, i)
    batch.set_input_unsigned(4, j)
    batch.set_input_unsigned(5, ni)
    batch.set_input_unsigned(6, nj)
    batch.set_input_unsigned(7, block_size)
    batch.set_input_string(8, category_id_file)
    batch.set_input_string(9, percent_cat_name)
    batch.set_input_float(10, scale_factor)
    status = batch.run_process()
    if status:
        (out_id, out_type) = batch.commit_output(0)
        out_crop = dbvalue(out_id, out_type)
        (out_id, out_type) = batch.commit_output(1)
        out_id_map = dbvalue(out_id, out_type)
        (out_id, out_type) = batch.commit_output(2)
        out_rgb_map = dbvalue(out_id, out_type)
        (percent_id, percent_type) = batch.commit_output(3)
        percent = batch.get_output_float(percent_id)
        batch.remove_data(percent_id)
        return out_crop, out_id_map, out_rgb_map, percent
    else:
        out_crop = 0
        out_id_map = 0
        out_rgb_map = 0
        percent = 100
        return out_crop, out_id_map, out_rgb_map, percent


def test_classifier_clouds2(tclsf, dictionary_name, image_resource, i, j, width, height, block_size, percent_cat_name, category_id_file=""):
    batch.init_process("sdetTextureClassifySatelliteCloudsProcess2")
    batch.set_input_from_db(0, tclsf)
    batch.set_input_string(1, dictionary_name)
    batch.set_input_from_db(2, image_resource)
    batch.set_input_unsigned(3, i)
    batch.set_input_unsigned(4, j)
    batch.set_input_unsigned(5, width)
    batch.set_input_unsigned(6, height)
    batch.set_input_unsigned(7, block_size)
    batch.set_input_string(8, category_id_file)
    batch.set_input_string(9, percent_cat_name)
    status = batch.run_process()
    if status:
        (out_id, out_type) = batch.commit_output(0)
        out_crop = dbvalue(out_id, out_type)
        (out_id, out_type) = batch.commit_output(1)
        out_id_map = dbvalue(out_id, out_type)
        (out_id, out_type) = batch.commit_output(2)
        out_rgb_map = dbvalue(out_id, out_type)
        (out_id, out_type) = batch.commit_output(3)
        percent = batch.get_output_float(out_id)
        batch.remove_data(out_id)
        return out_crop, out_id_map, out_rgb_map, percent
    else:
        return 0, 0, 0, 100


def create_texture_classifier(lambda0, lambda1, n_scales, scale_interval, angle_interval, laplace_radius, gauss_radius, k, n_samples):
    batch.init_process("sdetCreateTextureClassifierProcess")
    batch.set_input_float(0, lambda0)
    batch.set_input_float(1, lambda1)
    batch.set_input_unsigned(2, n_scales)
    batch.set_input_float(3, scale_interval)
    batch.set_input_float(4, angle_interval)
    batch.set_input_float(5, laplace_radius)
    batch.set_input_float(6, gauss_radius)
    batch.set_input_unsigned(7, k)
    batch.set_input_unsigned(8, n_samples)
    batch.run_process()
    (tclsf_id, tclsf_type) = batch.commit_output(0)
    tclsf = dbvalue(tclsf_id, tclsf_type)
    return tclsf

# pass the color output image of the classifier and the bin file name
# prefix for vsol_spatial_object_2d polygon files for ground-truth


def generate_roc(tclsf, class_out_prob_img, class_out_color_img, orig_img, prefix_for_bin_files, positive_category_name, category_id_file):
    batch.init_process("sdetTextureClassifierROCProcess")
    batch.set_input_from_db(0, tclsf)
    batch.set_input_from_db(1, class_out_prob_img)
    batch.set_input_from_db(2, class_out_color_img)
    batch.set_input_from_db(3, orig_img)
    batch.set_input_string(4, prefix_for_bin_files)
    batch.set_input_string(5, positive_category_name)
    batch.set_input_string(6, category_id_file)
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
    batch.remove_data(id)
    (id, type) = batch.commit_output(4)
    tpr = batch.get_bbas_1d_array_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(5)
    fpr = batch.get_bbas_1d_array_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(6)
    outimg = dbvalue(id, type)
    return tp, tn, fp, fn, tpr, fpr, outimg


def generate_roc2(image, gt_pos_file, pos_sign="low"):
    batch.init_process("sdetTextureClassifierROCProcess2")
    batch.set_input_from_db(0, image)
    batch.set_input_string(1, gt_pos_file)
    batch.set_input_string(2, pos_sign)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        threshold = batch.get_bbas_1d_array_float(id)
        (id, type) = batch.commit_output(1)
        tp = batch.get_bbas_1d_array_float(id)
        (id, type) = batch.commit_output(2)
        tn = batch.get_bbas_1d_array_float(id)
        (id, type) = batch.commit_output(3)
        fp = batch.get_bbas_1d_array_float(id)
        (id, type) = batch.commit_output(4)
        fn = batch.get_bbas_1d_array_float(id)
        (id, type) = batch.commit_output(5)
        tpr = batch.get_bbas_1d_array_float(id)
        (id, type) = batch.commit_output(6)
        fpr = batch.get_bbas_1d_array_float(id)
        return threshold, tp, tn, fp, fn, tpr, fpr
    else:
        return 0, 0, 0, 0, 0, 0, 0


# generate ROC for the input probability image of ground truth category
def generate_roc3(tclsf, in_prob_img, prefix_for_bin_files, positive_category_name):
    batch.init_process("sdetTextureClassifierROCProcess3")
    batch.set_input_from_db(0, tclsf)
    batch.set_input_from_db(1, in_prob_img)
    batch.set_input_string(2, prefix_for_bin_files)
    batch.set_input_string(3, positive_category_name)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        thres = batch.get_bbas_1d_array_float(id)
        (id, type) = batch.commit_output(1)
        tp = batch.get_bbas_1d_array_float(id)
        (id, type) = batch.commit_output(2)
        tn = batch.get_bbas_1d_array_float(id)
        (id, type) = batch.commit_output(3)
        fp = batch.get_bbas_1d_array_float(id)
        (id, type) = batch.commit_output(4)
        fn = batch.get_bbas_1d_array_float(id)
        (id, type) = batch.commit_output(5)
        tpr = batch.get_bbas_1d_array_float(id)
        (id, type) = batch.commit_output(6)
        fpr = batch.get_bbas_1d_array_float(id)
        return thres, tp, tn, fp, fn, tpr, fpr
    else:
        return None, None, None, None, None, None, None


def segment_image(img, weight_thres, margin=0, min_size=50, sigma=1, neigh=8):
    batch.init_process("sdetSegmentImageProcess")
    batch.set_input_from_db(0, img)
    batch.set_input_int(1, margin)
    batch.set_input_int(2, neigh)
    batch.set_input_float(3, weight_thres)
    batch.set_input_float(4, sigma)
    batch.set_input_int(5, min_size)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    seg_img = dbvalue(id, type)
    return seg_img


def segment_image_using_edges(img, edge_img, weight_thres, margin=0, min_size=50, sigma=1, neigh=8):
    batch.init_process("sdetSegmentUsingEdgesProcess")
    batch.set_input_from_db(0, img)
    batch.set_input_from_db(1, edge_img)
    batch.set_input_int(2, margin)
    batch.set_input_int(3, neigh)
    batch.set_input_float(4, weight_thres)
    batch.set_input_float(5, sigma)
    batch.set_input_int(6, min_size)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    seg_img = dbvalue(id, type)
    return seg_img


def segment_image_using_height(img, height_img, weight_thres, margin=0, min_size=50, sigma=1, neigh=8):
    batch.init_process("sdetSegmentUsingHeightMapProcess")
    batch.set_input_from_db(0, img)
    batch.set_input_from_db(1, height_img)
    batch.set_input_int(2, margin)
    batch.set_input_int(3, neigh)
    batch.set_input_float(4, weight_thres)
    batch.set_input_float(5, sigma)
    batch.set_input_int(6, min_size)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    seg_img = dbvalue(id, type)
    return seg_img


def segment_image_using_height2(img, height_img, edge_img, weight_thres, margin=0, min_size=50, sigma=1, neigh=8):
    batch.init_process("sdetSegmentUsingHeightMapProcess2")
    batch.set_input_from_db(0, img)
    batch.set_input_from_db(1, height_img)
    batch.set_input_from_db(2, edge_img)
    batch.set_input_int(3, margin)
    batch.set_input_int(4, neigh)
    batch.set_input_float(5, weight_thres)
    batch.set_input_float(6, sigma)
    batch.set_input_int(7, min_size)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    seg_img = dbvalue(id, type)
    return seg_img

def ss_segment_image(img, weight_thres):
    batch.init_process("sdetSelectiveSearchProcess")
    batch.set_input_from_db(0, img)
    batch.set_input_float(1, weight_thres)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    seg_img = dbvalue(id, type)
    (id, type) = batch.commit_output(1)
    bb_img = dbvalue(id, type)
    return seg_img, bb_img

def fit_oriented_box(color_blob_image):
    batch.init_process("sdetFitOrientedBoxesProcess")
    batch.set_input_from_db(0, color_blob_image)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        cnt = batch.get_output_unsigned(id)
        (id, type) = batch.commit_output(1)
        corners = batch.get_bbas_1d_array_float(id)
        (id, type) = batch.commit_output(2)
        box_dimension = batch.get_bbas_1d_array_float(id)
        return cnt, corners, box_dimension
    else:
        return 0, 0, 0

def fit_oriented_box_from_geotiff(byte_img, in_cam, out_kml, pixel_thres = 30):
    batch.init_process("sdetFitOrientedBoxesFromGeotiffProcess")
    batch.set_input_from_db(0, byte_img)
    batch.set_input_from_db(1, in_cam)
    batch.set_input_unsigned(2, pixel_thres)
    batch.set_input_string(3, out_kml)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        cnt = batch.get_output_unsigned(id)
        (id, type) = batch.commit_output(1)
        box_img = dbvalue(id, type)
        return cnt, box_img
    else:
        return None, None
