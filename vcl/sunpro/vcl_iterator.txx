#undef VCL_COPY_INSTANTIATE
#define VCL_COPY_INSTANTIATE(Inp, Out) \
template Out std::copy(Inp, Inp, Out)

#undef VCL_FIND_INSTANTIATE
#define VCL_FIND_INSTANTIATE(Inp,Out) \
template Inp std::find(Inp, Inp, Out const&)

