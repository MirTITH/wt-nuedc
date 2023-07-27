#include "just_float.hpp"

namespace vofa
{

JustFloatStream JFStream;

void EndJFStream(JustFloatStream &jfo)
{
    jfo.SendEndMarkAndUnlock();
}

} // namespace name
