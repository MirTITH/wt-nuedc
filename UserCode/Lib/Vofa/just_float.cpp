#include "just_float.hpp"

namespace vofa
{

JustFloatStream JFStream;

void EndJFStream(JustFloatStream &jfo)
{
    jfo.uart_.Write(vofa_internal::kTail, sizeof(vofa_internal::kTail));
}

} // namespace name
