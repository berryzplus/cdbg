// -*- mode: c++ -*-

#include "dbgstream.h"

#include <Windows.h>

template<>
void basic_debug_streambuf<char, std::char_traits<char>>::output_debug_string(std::string_view output_string) const
{
  OutputDebugStringA(output_string.data());
}

template<>
void basic_debug_streambuf<wchar_t, std::char_traits<wchar_t>>::output_debug_string(std::wstring_view output_string) const
{
  OutputDebugStringW(output_string.data());
}

debug_stream  cdbg;
wdebug_stream wcdbg;
