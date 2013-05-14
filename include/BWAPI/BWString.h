#pragma once

#include "Vectorset.h"

namespace BWAPI
{
  class BWString : public Vectorset<char>
  {
  public:
    BWString(size_t initialSize = 32);
    BWString(const BWString &other);
    BWString(BWString &&other);

    BWString(const char *pszStr);

  };
};
