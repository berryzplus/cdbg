// -*- mode: c++ -*-

#pragma once

#include <iostream>

template <class Ch,class Tr=std::char_traits<Ch> >
class basic_debug_streambuf : public std::basic_streambuf<Ch,Tr> {
private:
  static constexpr int  init_size = 0x100;
  int                   _Size = 0;
  std::unique_ptr<Ch[]> _Buffer;

public:
  using int_type = std::basic_streambuf<Ch, Tr>::int_type;

  basic_debug_streambuf() = default;

  ~basic_debug_streambuf()
  {
    sync();
  }

protected:
  int_type overflow(int_type ch = Tr::eof()) override {
    if (!this->pptr())
    {
      _Buffer = std::make_unique<Ch[]>(init_size);
      this->setp(&_Buffer[0], &_Buffer[init_size]);
      _Size = init_size;
    }
    else if (this->pptr() >= this->epptr())
    {
      const auto old_size = _Size;
      const auto new_size = old_size * 2;

      auto new_buffer = std::make_unique<Ch[]>(new_size);
      std::copy(this->pbase(), this->epptr(), new_buffer.get());
      _Buffer.swap(std::move(new_buffer));

      this->setp(&_Buffer[0], &_Buffer[new_size]);
      this->pbump(old_size);
      _Size = new_size;
    }

    return this->sputc(Tr::to_char_type(ch));
  }

  int sync(void) override {
    if (const auto count = this->pptr() - this->pbase())
    {
      if (*this->pptr() && Tr::eq_int_type(Tr::eof(), overflow('\0')))
      {
        return 1; // fail
      }

      auto output_string = std::basic_string_view<Ch, Tr>(this->pbase(), count);
      output_debug_string(output_string);
      this->setp(this->pbase(), this->epptr());
    }

    return 0;
  }

  virtual void output_debug_string(std::basic_string_view<Ch, Tr> output_string) const;
};

template <class Ch,class Tr=std::char_traits<Ch> >
class basic_debug_stream : public std::basic_ostream<Ch,Tr> {
private:
  std::unique_ptr<basic_debug_streambuf<Ch, Tr>> _StreamBuf;

public:
  basic_debug_stream(void)
    : basic_debug_stream<Ch, Tr>(std::make_unique<basic_debug_streambuf<Ch, Tr>>()) {
  }

  explicit basic_debug_stream(std::unique_ptr<basic_debug_streambuf<Ch, Tr>>&& stream_buf)
    : std::basic_ostream<Ch, Tr>(std::_Noinit)
    , _StreamBuf(std::move(stream_buf)) {
    std::basic_ios<Ch, Tr>::init(_StreamBuf.get(), false);
  }

  ~basic_debug_stream(void) {
    this->flush();
  }
};

using debug_streambuf  = basic_debug_streambuf<char>;
using wdebug_streambuf = basic_debug_streambuf<wchar_t>;
using debug_stream     = basic_debug_stream<char>;
using wdebug_stream    = basic_debug_stream<wchar_t>;

extern debug_stream  cdbg;
extern wdebug_stream wcdbg;
