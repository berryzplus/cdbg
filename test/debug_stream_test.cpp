// -*- mode: c++ -*-

#include "dbgstream.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <CrtAllocHook.hpp>

using ::testing::_;
using ::testing::AnyNumber;
using ::testing::Invoke;
using ::testing::Return;
using ::testing::StrEq;

template<class Ch, class Tr = std::char_traits<Ch>>
class test_debug_streambuf : public basic_debug_streambuf<Ch, Tr>
{
public:
  using basic_debug_streambuf<Ch, Tr>::overflow;
  using basic_debug_streambuf<Ch, Tr>::sputn;
};

template<class Ch>
class mock_debug_streambuf : public basic_debug_streambuf<Ch>
{
public:
  using int_type = typename std::char_traits<Ch>::int_type;
  using basic_debug_streambuf<Ch>::sync;
  using basic_debug_streambuf<Ch>::pptr;
  using basic_debug_streambuf<Ch>::setp;
  using basic_debug_streambuf<Ch>::pbump;

  MOCK_CONST_METHOD1_T(output_debug_string, void(std::basic_string_view<Ch> output_string));
  MOCK_METHOD1_T(overflow, int_type(int_type ch));

  int_type original_overflow(int_type ch)
  {
    return basic_debug_streambuf<Ch>::overflow(ch);
  }
};

TEST(cdbg, test_without_anycheck)
{
  debug_stream dbg;
  dbg << "1+1=" << 1 + 1 << std::endl;
}

TEST(debug_streambuf, fail_on_initial_alloc)
{
  auto buf = test_debug_streambuf<char>();

  const CrtAllocHook allocHook(HookFunc<char, _HOOK_ALLOC, 0x100>);
  EXPECT_THROW({ buf.overflow('\0'); }, std::bad_alloc);
}

TEST(debug_streambuf, fail_on_overflow)
{
  auto buf  = test_debug_streambuf<char>();
  auto text = std::string(0x100, 'x');
  EXPECT_NO_THROW({ buf.sputn(text.data(), text.length()); });

  const CrtAllocHook allocHook(HookFunc<char, _HOOK_ALLOC, 0x200>);
  EXPECT_THROW({ buf.overflow('x'); }, std::bad_alloc);
}

TEST(debug_streambuf, output_debug_string)
{
  auto mock = mock_debug_streambuf<char>();
  EXPECT_CALL(mock, output_debug_string(StrEq("test\n")))
    .Times(1);
  EXPECT_CALL(mock, overflow('\0'))
    .Times(1);

  const auto buf_size = 10;
  auto       buffer   = std::string(buf_size, '\0');
  strcpy_s(&buffer[0], buf_size, "test\nrest");
  mock.setp(&buffer[0], &buffer[buf_size]);
  mock.pbump(5);

  EXPECT_EQ(0, mock.sync());
}

TEST(debug_streambuf, fail_on_sync)
{
  auto mock = mock_debug_streambuf<char>();
  EXPECT_CALL(mock, output_debug_string(_))
    .Times(AnyNumber());
  EXPECT_CALL(mock, overflow('\0'))
    .WillOnce(Return(std::char_traits<char>::eof()));

  const auto buf_size = 10;
  auto       buffer   = std::string(buf_size, '\0');
  strcpy_s(&buffer[0], buf_size, "test100");
  mock.setp(&buffer[0], &buffer[buf_size]);
  mock.pbump(4);

  EXPECT_NE(0, mock.sync());
}

TEST(wcdbg, test_without_anycheck)
{
  wdebug_stream dbg;
  dbg << L"1+1=" << 1 + 1 << std::endl;
}

TEST(wdebug_streambuf, fail_on_initial_alloc)
{
  auto buf = test_debug_streambuf<wchar_t>();

  const CrtAllocHook allocHook(HookFunc<wchar_t, _HOOK_ALLOC, 0x100>);
  EXPECT_THROW({ buf.overflow('\0'); }, std::bad_alloc);
}

TEST(wdebug_streambuf, fail_on_overflow)
{
  auto buf  = test_debug_streambuf<wchar_t>();
  auto text = std::wstring(0x100, 'x');
  EXPECT_NO_THROW({ buf.sputn(text.data(), text.length()); });

  const CrtAllocHook allocHook(HookFunc<wchar_t, _HOOK_ALLOC, 0x200>);
  EXPECT_THROW({ buf.overflow('x'); }, std::bad_alloc);
}

TEST(wdebug_streambuf, output_debug_string)
{
  auto mock = mock_debug_streambuf<wchar_t>();
  EXPECT_CALL(mock, output_debug_string(StrEq(L"test\n")))
    .Times(1);
  EXPECT_CALL(mock, overflow('\0'))
    .Times(1);

  const auto buf_size = 10;
  auto       buffer   = std::wstring(buf_size, '\0');
  wcscpy_s(&buffer[0], buf_size, L"test\nrest");
  mock.setp(&buffer[0], &buffer[buf_size]);
  mock.pbump(5);

  EXPECT_EQ(0, mock.sync());
}

TEST(wdebug_streambuf, fail_on_sync)
{
  auto mock = mock_debug_streambuf<wchar_t>();
  EXPECT_CALL(mock, output_debug_string(_))
    .Times(AnyNumber());
  EXPECT_CALL(mock, overflow('\0'))
    .WillOnce(Return(std::char_traits<wchar_t>::eof()));

  const auto buf_size = 10;
  auto       buffer   = std::wstring(buf_size, '\0');
  wcscpy_s(&buffer[0], buf_size, L"test100");
  mock.setp(&buffer[0], &buffer[buf_size]);
  mock.pbump(4);

  EXPECT_NE(0, mock.sync());
}
