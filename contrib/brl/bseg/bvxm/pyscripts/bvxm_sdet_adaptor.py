from bvxm_register import bvxm_batch, dbvalue
import math


def create_classifier(lambda0, lambda1, n_scales, scale_interval, angle_interval, laplace_radius, gauss_radius, k, n_samples):
    bvxm_batch.init_process("sdetCreateTextureClassifierProcess")
    bvxm_batch.set_input_float(0, lambda0)      # lambda0
    bvxm_batch.set_input_float(1, lambda1)      # lambda1
    bvxm_batch.set_input_unsigned(2, n_scales)     # n_scales
    bvxm_batch.set_input_float(3, scale_interval)    # scale_interval
    bvxm_batch.set_input_float(4, angle_interval)    # angle_interval
    bvxm_batch.set_input_float(5, laplace_radius)     # laplace_radius
    bvxm_batch.set_input_float(6, gauss_radius)     # gauss_radius
    bvxm_batch.set_input_unsigned(7, k)  # k
    bvxm_batch.set_input_unsigned(8, n_samples)  # number of samples
    bvxm_batch.run_process()
    (tclsf_id, tclsf_type) = bvxm_batch.commit_output(0)
    tclsf = dbvalue(tclsf_id, tclsf_type)
    return tclsf


def save_classifier(tclsf, classifier_name):
    bvxm_batch.init_process("sdetSaveTextureClassifierProcess")
    bvxm_batch.set_input_from_db(0, tclsf)      # classifier instance
    bvxm_batch.set_input_string(1, classifier_name)
    bvxm_batch.run_process()


def load_classifier(classifier_name):
    bvxm_batch.init_process("sdetLoadTextureClassifierProcess")
    bvxm_batch.set_input_string(0, classifier_name)
    bvxm_batch.run_process()
    (tclsf_id, tclsf_type) = bvxm_batch.commit_output(0)
    tclsf = dbvalue(tclsf_id, tclsf_type)
    return tclsf


def load_dictionary(dictionary_name):
    bvxm_batch.init_process("sdetLoadTextureDictionaryProcess")
    bvxm_batch.set_input_string(0, dictionary_name)
    bvxm_batch.run_process()
    (tclsf_id, tclsf_type) = bvxm_batch.commit_output(0)
    tclsf = dbvalue(tclsf_id, tclsf_type)
    return tclsf


def extract_filter_bank(tclsf, img_name, filter_folder):
    bvxm_batch.init_process("sdetExtractFilterBankProcess")
    bvxm_batch.set_input_from_db(0, tclsf)      # classifier instance
    bvxm_batch.set_input_string(1, img_name)
    bvxm_batch.set_input_string(2, filter_folder)
    bvxm_batch.run_process()


def add_to_filter_bank(tclsf, img_name, plane, filter_folder, filter_name, is_gauss_smooth=True):
    bvxm_batch.init_process("sdetAddtoFilterBankProcess")
    bvxm_batch.set_input_from_db(0, tclsf)      # classifier instance
    bvxm_batch.set_input_string(1, img_name)
    # pass which plane to extract the gauss response from
    bvxm_batch.set_input_unsigned(2, plane)
    bvxm_batch.set_input_string(3, filter_folder)
    # pass a unique name to be used to write to filter_folder
    bvxm_batch.set_input_string(4, filter_name)
    bvxm_batch.set_input_bool(5, is_gauss_smooth)
    bvxm_batch.run_process()


def add_responses_to_filter_bank(tclsf, img_name, img, filter_folder, filter_name):
    bvxm_batch.init_process("sdetAddResponsestoFilterBankProcess")
    bvxm_batch.set_input_from_db(0, tclsf)      # classifier instance
    bvxm_batch.set_input_string(1, img_name)
    bvxm_batch.set_input_from_db(2, img)
    bvxm_batch.set_input_string(3, filter_folder)
    # pass a unique name to be used to write to filter_folder
    bvxm_batch.set_input_string(4, filter_name)
    bvxm_batch.run_process()


def add_to_filter_bank2(tclsf, img_name, filter_folder):
    bvxm_batch.init_process("sdetAddtoFilterBankProcess2")
    bvxm_batch.set_input_from_db(0, tclsf)      # classifier instance
    bvxm_batch.set_input_string(1, img_name)
    bvxm_batch.set_input_string(2, filter_folder)
    bvxm_batch.run_process()

# assumes that the training will be on the same image and the filter bank
# of it is already computed and saved at tclsf, e.g. using
# extract_filter_bank method


def train_classifier(tclsf, poly_file, category_name, dictionary_name, compute_category_textons, finish_and_write_dictionary):
    bvxm_batch.init_process("sdetTextureTrainingProcess2")
    bvxm_batch.set_input_from_db(0, tclsf)
    if compute_category_textons:
        # compute the textons if this is the last polygon file for the category
        bvxm_batch.set_input_bool(1, 1)
    else:
        bvxm_batch.set_input_bool(1, 0)
    bvxm_batch.set_input_string(2, poly_file)
    bvxm_batch.set_input_string(3, category_name)
    bvxm_batch.set_input_string(4, dictionary_name)
    bvxm_batch.run_process()
    (tclsf_id, tclsf_type) = bvxm_batch.commit_output(0)
    tclsf = dbvalue(tclsf_id, tclsf_type)
    if finish_and_write_dictionary:
        bvxm_batch.finish_process()
    return tclsf


def dump_binary_data_as_txt(poly_file, output_file):
    bvxm_batch.init_process("sdetDumpVsolBinaryDataProcess")
    bvxm_batch.set_input_string(0, poly_file)
    bvxm_batch.set_input_string(1, output_file)
    bvxm_batch.run_process()

# def test_classifier(tclsf, img, block_size):


def test_classifier(tclsf, block_size, category_id_file="", category_name=""):
    bvxm_batch.init_process("sdetTextureClassifierProcess2")
    bvxm_batch.set_input_from_db(0, tclsf)
    bvxm_batch.set_input_unsigned(1, block_size)
    bvxm_batch.set_input_string(2, category_id_file)
    bvxm_batch.set_input_string(3, category_name)
    status = bvxm_batch.run_process()
    if status:
      (out_id, out_type) = bvxm_batch.commit_output(0)
      out_max_prob = dbvalue(out_id, out_type)
      (out_id, out_type) = bvxm_batch.commit_output(1)
      out_color_img = dbvalue(out_id, out_type)
      (out_id, out_type) = bvxm_batch.commit_output(2)
      out_id_img = dbvalue(out_id, out_type)
      (out_id, out_type) = bvxm_batch.commit_output(3)
      out_cat_prob = dbvalue(out_id, out_type)
      return out_max_prob, out_color_img, out_id_img, out_cat_prob
    else:
      return None, None, None, None


def test_classifier_clouds(tclsf, dictionary_name, image_resource, i, j, width, height, block_size, percent_cat_name, category_id_file=""):
    bvxm_batch.init_process("sdetTextureClassifySatelliteCloudsProcess")
    bvxm_batch.set_input_from_db(0, tclsf)
    bvxm_batch.set_input_string(1, dictionary_name)
    bvxm_batch.set_input_from_db(2, image_resource)
    bvxm_batch.set_input_unsigned(3, i)
    bvxm_batch.set_input_unsigned(4, j)
    bvxm_batch.set_input_unsigned(5, width)
    bvxm_batch.set_input_unsigned(6, height)
    bvxm_batch.set_input_unsigned(7, block_size)
    bvxm_batch.set_input_string(8, category_id_file)
    bvxm_batch.set_input_string(9, percent_cat_name)
    status = bvxm_batch.run_process()
    if status:
        (out_id, out_type) = bvxm_batch.commit_output(0)
        out_crop = dbvalue(out_id, out_type)
        (out_id, out_type) = bvxm_batch.commit_output(1)
        out_id_map = dbvalue(out_id, out_type)
        (out_id, out_type) = bvxm_batch.commit_output(2)
        out_rgb_map = dbvalue(out_id, out_type)
        (percent_id, percent_type) = bvxm_batch.commit_output(3)
        percent = bvxm_batch.get_output_float(percent_id)
        bvxm_batch.remove_data(percent_id)
        return out_crop, out_id_map, out_rgb_map, percent
    else:
        out_crop = 0
        out_id_map = 0
        out_rgb_map = 0
        percent = 100
        return out_crop, out_id_map, out_rgb_map, percent


def test_classifier_clouds2(tclsf, dictionary_name, image_resource, i, j, width, height, block_size, percent_cat_name, category_id_file=""):
    bvxm_batch.init_process("sdetTextureClassifySatelliteCloudsProcess2")
    bvxm_batch.set_input_from_db(0, tclsf)
    bvxm_batch.set_input_string(1, dictionary_name)
    bvxm_batch.set_input_from_db(2, image_resource)
    bvxm_batch.set_input_unsigned(3, i)
    bvxm_batch.set_input_unsigned(4, j)
    bvxm_batch.set_input_unsigned(5, width)
    bvxm_batch.set_input_unsigned(6, height)
    bvxm_batch.set_input_unsigned(7, block_size)
    bvxm_batch.set_input_string(8, category_id_file)
    bvxm_batch.set_input_string(9, percent_cat_name)
    status = bvxm_batch.run_process()
    if status:
        (out_id, out_type) = bvxm_batch.commit_output(0)
        out_crop = dbvalue(out_id, out_type)
        (out_id, out_type) = bvxm_batch.commit_output(1)
        out_id_map = dbvalue(out_id, out_type)
        (out_id, out_type) = bvxm_batch.commit_output(2)
        out_rgb_map = dbvalue(out_id, out_type)
        (out_id, out_type) = bvxm_batch.commit_output(3)
        percent = bvxm_batch.get_output_float(out_id)
        bvxm_batch.remove_data(out_id)
        return out_crop, out_id_map, out_rgb_map, percent
    else:
        return 0, 0, 0, 100


def create_texture_classifier(lambda0, lambda1, n_scales, scale_interval, angle_interval, laplace_radius, gauss_radius, k, n_samples):
    bvxm_batch.init_process("sdetCreateTextureClassifierProcess")
    bvxm_batch.set_input_float(0, lambda0)
    bvxm_batch.set_input_float(1, lambda1)
    bvxm_batch.set_input_unsigned(2, n_scales)
    bvxm_batch.set_input_float(3, scale_interval)
    bvxm_batch.set_input_float(4, angle_interval)
    bvxm_batch.set_input_float(5, laplace_radius)
    bvxm_batch.set_input_float(6, gauss_radius)
    bvxm_batch.set_input_unsigned(7, k)
    bvxm_batch.set_input_unsigned(8, n_samples)
    bvxm_batch.run_process()
    (tclsf_id, tclsf_type) = bvxm_batch.commit_output(0)
    tclsf = dbvalue(tclsf_id, tclsf_type)
    return tclsf

# pass the color output image of the classifier and the bin file name
# prefix for vsol_spatial_object_2d polygon files for ground-truth


def generate_roc(tclsf, class_out_prob_img, class_out_color_img, orig_img, prefix_for_bin_files, positive_category_name, category_id_file):
    bvxm_batch.init_process("sdetTextureClassifierROCProcess")
    bvxm_batch.set_input_from_db(0, tclsf)
    bvxm_batch.set_input_from_db(1, class_out_prob_img)
    bvxm_batch.set_input_from_db(2, class_out_color_img)
    bvxm_batch.set_input_from_db(3, orig_img)
    bvxm_batch.set_input_string(4, prefix_for_bin_files)
    bvxm_batch.set_input_string(5, positive_category_name)
    bvxm_batch.set_input_string(6, category_id_file)
    bvxm_batch.run_process()
    (id, type) = bvxm_batch.commit_output(0)
    thres = bvxm_batch.get_bbas_1d_array_float(id)
    (id, type) = bvxm_batch.commit_output(1)
    tp = bvxm_batch.get_bbas_1d_array_float(id)
    (id, type) = bvxm_batch.commit_output(2)
    tn = bvxm_batch.get_bbas_1d_array_float(id)
    (id, type) = bvxm_batch.commit_output(3)
    fp = bvxm_batch.get_bbas_1d_array_float(id)
    (id, type) = bvxm_batch.commit_output(4)
    fn = bvxm_batch.get_bbas_1d_array_float(id)
    (id, type) = bvxm_batch.commit_output(5)
    tpr = bvxm_batch.get_bbas_1d_array_float(id)
    (id, type) = bvxm_batch.commit_output(6)
    fpr = bvxm_batch.get_bbas_1d_array_float(id)
    (id, type) = bvxm_batch.commit_output(7)
    outimg = dbvalue(id, type)
    return thres, tp, tn, fp, fn, tpr, fpr, outimg

# ROC analysis for a binary classifier


def generate_roc2(image, gt_pos_file, pos_sign="low"):
    bvxm_batch.init_process("sdetTextureClassifierROCProcess2")
    bvxm_batch.set_input_from_db(0, image)
    bvxm_batch.set_input_string(1, gt_pos_file)
    bvxm_batch.set_input_string(2, pos_sign)
    status = bvxm_batch.run_process()
    if status:
        (id, type) = bvxm_batch.commit_output(0)
        threshold = bvxm_batch.get_bbas_1d_array_float(id)
        (id, type) = bvxm_batch.commit_output(1)
        tp = bvxm_batch.get_bbas_1d_array_float(id)
        (id, type) = bvxm_batch.commit_output(2)
        tn = bvxm_batch.get_bbas_1d_array_float(id)
        (id, type) = bvxm_batch.commit_output(3)
        fp = bvxm_batch.get_bbas_1d_array_float(id)
        (id, type) = bvxm_batch.commit_output(4)
        fn = bvxm_batch.get_bbas_1d_array_float(id)
        (id, type) = bvxm_batch.commit_output(5)
        tpr = bvxm_batch.get_bbas_1d_array_float(id)
        (id, type) = bvxm_batch.commit_output(6)
        fpr = bvxm_batch.get_bbas_1d_array_float(id)
        return threshold, tp, tn, fp, fn, tpr, fpr
    else:
        return 0, 0, 0, 0, 0, 0, 0

# generate ROC for the input probability image of ground truth category
def generate_roc3(tclsf, in_prob_img, prefix_for_bin_files, positive_category_name):
  bvxm_batch.init_process("sdetTextureClassifierROCProcess3")
  bvxm_batch.set_input_from_db(0, tclsf)
  bvxm_batch.set_input_from_db(1, in_prob_img)
  bvxm_batch.set_input_string(2, prefix_for_bin_files)
  bvxm_batch.set_input_string(3, positive_category_name)
  status = bvxm_batch.run_process()
  if status:
    (id, type) = bvxm_batch.commit_output(0)
    thres = bvxm_batch.get_bbas_1d_array_float(id)
    (id, type) = bvxm_batch.commit_output(1)
    tp = bvxm_batch.get_bbas_1d_array_float(id)
    (id, type) = bvxm_batch.commit_output(2)
    tn = bvxm_batch.get_bbas_1d_array_float(id)
    (id, type) = bvxm_batch.commit_output(3)
    fp = bvxm_batch.get_bbas_1d_array_float(id)
    (id, type) = bvxm_batch.commit_output(4)
    fn = bvxm_batch.get_bbas_1d_array_float(id)
    (id, type) = bvxm_batch.commit_output(5)
    tpr = bvxm_batch.get_bbas_1d_array_float(id)
    (id, type) = bvxm_batch.commit_output(6)
    fpr = bvxm_batch.get_bbas_1d_array_float(id)
    return thres, tp, tn, fp, fn, tpr, fpr
  else:
    return None, None, None, None, None, None, None

def segment_image(img, weight_thres, margin=0, min_size=50, sigma=1, neigh=8):
    bvxm_batch.init_process("sdetSegmentImageProcess")
    bvxm_batch.set_input_from_db(0, img)
    bvxm_batch.set_input_int(1, margin)
    bvxm_batch.set_input_int(2, neigh)
    bvxm_batch.set_input_float(3, weight_thres)
    bvxm_batch.set_input_float(4, sigma)
    bvxm_batch.set_input_int(5, min_size)
    bvxm_batch.run_process()
    (id, type) = bvxm_batch.commit_output(0)
    seg_img = dbvalue(id, type)
    return seg_img


def segment_image_using_edges(img, edge_img, weight_thres, margin=0, min_size=50, sigma=1, neigh=8):
    bvxm_batch.init_process("sdetSegmentUsingEdgesProcess")
    bvxm_batch.set_input_from_db(0, img)
    bvxm_batch.set_input_from_db(1, edge_img)
    bvxm_batch.set_input_int(2, margin)
    bvxm_batch.set_input_int(3, neigh)
    bvxm_batch.set_input_float(4, weight_thres)
    bvxm_batch.set_input_float(5, sigma)
    bvxm_batch.set_input_int(6, min_size)
    bvxm_batch.run_process()
    (id, type) = bvxm_batch.commit_output(0)
    seg_img = dbvalue(id, type)
    return seg_img


def segment_image_using_height(img, height_img, weight_thres, margin=0, min_size=50, sigma=1, neigh=8):
    bvxm_batch.init_process("sdetSegmentUsingHeightMapProcess")
    bvxm_batch.set_input_from_db(0, img)
    bvxm_batch.set_input_from_db(1, height_img)
    bvxm_batch.set_input_int(2, margin)
    bvxm_batch.set_input_int(3, neigh)
    bvxm_batch.set_input_float(4, weight_thres)
    bvxm_batch.set_input_float(5, sigma)
    bvxm_batch.set_input_int(6, min_size)
    bvxm_batch.run_process()
    (id, type) = bvxm_batch.commit_output(0)
    seg_img = dbvalue(id, type)
    return seg_img


def segment_image_using_height2(img, height_img, edge_img, weight_thres, margin=0, min_size=50, sigma=1, neigh=8):
    bvxm_batch.init_process("sdetSegmentUsingHeightMapProcess2")
    bvxm_batch.set_input_from_db(0, img)
    bvxm_batch.set_input_from_db(1, height_img)
    bvxm_batch.set_input_from_db(2, edge_img)
    bvxm_batch.set_input_int(3, margin)
    bvxm_batch.set_input_int(4, neigh)
    bvxm_batch.set_input_float(5, weight_thres)
    bvxm_batch.set_input_float(6, sigma)
    bvxm_batch.set_input_int(7, min_size)
    bvxm_batch.run_process()
    (id, type) = bvxm_batch.commit_output(0)
    seg_img = dbvalue(id, type)
    return seg_img
