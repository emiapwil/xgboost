/*!
 * Copyright by Contributors 2017-2021
 * \file prefix.h
 * \brief data structure to support prefix splitting
 * \author Kai Gao
 */

#pragma once

#include "xgboost/base.h"
#include "xgboost/data.h"

namespace xgboost {

  static constexpr int kMaxPrefixLen = 32;

  inline uint32_t clear_lower_bits(uint32_t addr, uint8_t masklen) {
    if (masklen == 32) {
      return 0;
    }
    return (addr >> masklen) << masklen;
  }

  struct Prefix {
    uint32_t pvalue {0};
    uint8_t masklen {0};

    inline uint32_t ClearLowerBits() const {
      return pvalue;
    }

    inline bool operator==(const Prefix& rhs) const {
      return (masklen == rhs.masklen) && (pvalue == rhs.pvalue);
    }

    inline bool operator!=(const Prefix& rhs) const {
      return (masklen != rhs.masklen) || (pvalue != rhs.pvalue);
    }

    inline bool Match(uint32_t addr) const {
      return pvalue == clear_lower_bits(addr, masklen);
    }

    inline bool Match(const Prefix & rhs) const {
      return (masklen >= rhs.masklen) &&
        (Match(rhs.pvalue));
    }

    inline Prefix Merge(const Prefix &rhs) const {
      Prefix prefix(pvalue, masklen);
      prefix.Update(rhs);
      return prefix;
    }

    inline void Update(const Prefix &rhs) {
      if (rhs.masklen > masklen) {
        masklen = rhs.masklen;
      }
      while (clear_lower_bits(pvalue, masklen) !=
             clear_lower_bits(rhs.pvalue, masklen)) {
        masklen++;
      }
      pvalue = clear_lower_bits(pvalue, masklen);
    }

    Prefix() = default;

    Prefix(uint32_t pvalue_, uint8_t masklen_)
      : pvalue(clear_lower_bits(pvalue_, masklen_)), masklen(masklen_) {}

    Prefix(const Prefix & rhs): pvalue(rhs.pvalue), masklen(rhs.masklen) {}
  };

} // namespace xgboost
