#pragma once
#include <string>

namespace meta { namespace debug {

template<typename... Args>
std::string to_string()
{
  return __PRETTY_FUNCTION__;
}

}}
