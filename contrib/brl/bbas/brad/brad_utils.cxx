#include <sstream>
#include <stdexcept>
#include <vector>

#include "brad_utils.h"

// validate band IDs against available bands
void
brad_utils::validate_band_ids(size_t n_bands,
                              std::vector<size_t> band_ids)
{
  for (auto band_id : band_ids) {
    if (band_id >= n_bands) {
      std::ostringstream oss;
      oss << "brad_utils::validate_band_ids: image with " << n_bands
          << " bands does not contain band " << band_id;
      throw std::invalid_argument(oss.str());
    }
  }
}

//: validate vector against available bands
void
brad_utils::validate_vector(size_t n_bands,
                            std::vector<double> vec,
                            std::string vec_name,
                            bool allow_empty)
{
  if (allow_empty && vec.empty()) {
    return;
  }

  if (vec.size() != n_bands) {
    std::ostringstream oss;
    oss << "brad_utils::validate_vector: "
        << vec_name << " vector (" << vec.size() << " elements) "
        << "is incompatible with image (" << n_bands << " bands)";
    throw std::invalid_argument(oss.str());
  }
}

