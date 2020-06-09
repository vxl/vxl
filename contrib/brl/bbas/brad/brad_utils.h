#ifndef brad_utils_h_
#define brad_utils_h_

#include <vector>

namespace brad_utils {

//: validate band IDs against available bands
void validate_band_ids(size_t n_bands,
                       std::vector<size_t> band_ids);

//: validate vector against available bands
void validate_vector(size_t nbands,
                     std::vector<double> vec,
                     std::string vec_name = "",
                     bool allow_empty = false);

} // namespace brad_utils

#endif // brad_utils_h_
