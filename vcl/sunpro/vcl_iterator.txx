#undef VCL_COPY_INSTANTIATE
#define VCL_COPY_INSTANTIATE(Inp, Out) \
template Out std::copy(Inp, Inp, Out)

