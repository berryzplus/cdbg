// -*- mode: c++ -*-

#if defined(_MSVC_LANG) && _MSVC_LANG < 202002L

#include <CodeCoverage/CodeCoverage.h>

// Exclude all the code from a particular files:
// see https://docs.microsoft.com/ja-jp/visualstudio/test/using-code-coverage-to-determine-how-much-code-is-being-tested
ExcludeSourceFromCodeCoverage(Exclusion1, L"*\\VC\\Tools\\MSVC\\*\\include\\*");
ExcludeSourceFromCodeCoverage(Exclusion2, L"*\\Windows Kits\\10\\Include\\*");
ExcludeSourceFromCodeCoverage(Exclusion3, L"*\\gtest\\*");
ExcludeSourceFromCodeCoverage(Exclusion4, L"*\\test\\*");
ExcludeSourceFromCodeCoverage(Exclusion5, L"*\\gmock\\*");

#endif // #if defined(_MSVC_LANG) && _MSVC_LANG < 202002L
