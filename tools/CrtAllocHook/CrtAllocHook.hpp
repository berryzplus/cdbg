// -*- mode: c++ -*-

#pragma once

#include <Windows.h>

#include <new>
#include <crtdbg.h>

template<class Ch, int _AllocType, size_t _Count>
int HookFunc(int nAllocType, void *pvData, size_t nSize, int nBlockUse, long lRequest, const unsigned char *szFileName, int nLine)
{
  UNREFERENCED_PARAMETER(pvData);
  UNREFERENCED_PARAMETER(lRequest);
  UNREFERENCED_PARAMETER(szFileName);
  UNREFERENCED_PARAMETER(nLine);

  if (nBlockUse == _CRT_BLOCK)
  {
    return TRUE;
  }

  if (nAllocType != _AllocType)
  {
    return TRUE;
  }

  if (nSize != sizeof(Ch) * _Count)
  {
    return TRUE;
  }

  return FALSE;
}

class CrtAllocHook
{
private:
  struct alloc_hook_reseter
  {
    void operator()(_CRT_ALLOC_HOOK hookFunc) const
    {
      _CrtSetAllocHook(hookFunc);
    }
  };

  using allocHookHolder = std::unique_ptr<std::remove_pointer_t<_CRT_ALLOC_HOOK>, alloc_hook_reseter>;

  allocHookHolder _DefaultAllocHook;

public:
  explicit CrtAllocHook(_CRT_ALLOC_HOOK hookFunc)
    : _DefaultAllocHook(_CrtSetAllocHook(hookFunc))
  {
  }
};
