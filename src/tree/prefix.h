/*!
 * Copyright by Contributors 2017-2021
 * \file prefix.h
 * \brief data structure to support prefix splitting
 * \author Kai Gao
 */

#include "xgboost/base.h"
#include "xgboost/data.h"

namespace xgboost {
namespace tree {

  static constexpr int kMaxPrefixLen = 32;

  struct Prefix {
    bst_ulong pvalue {0};
    int masklen {0};

    inline bool Match(bst_ulong addr) const {
      return ((addr ^ pvalue) >> masklen) == 0;
    }

    inline bool Match(const Prefix & rhs) const {
      return (masklen >= rhs.masklen) &&
        (Match(rhs.pvalue));
    }

    inline Prefix Merge(const Prefix &rhs) {
      Prefix prefix(pvalue, masklen);
      prefix.Update(rhs);
      return prefix;
    }

    inline void Update(const Prefix &rhs) {
      if (rhs.masklen > masklen) {
        masklen = rhs.masklen;
      }
      while ((pvalue ^ rhs.pvalue) >> masklen) {
        masklen++;
      }
    }

    inline bst_float Encode() {
      return (pvalue << 6) + masklen;
    }

    inline void Decode(bst_float fvalue) {
      bst_ulong encoded = static_cast<bst_ulong>(fvalue);
      masklen = static_cast<int>(encoded & 63);
      pvalue = encoded >> 6;
    }

    Prefix() = default;

    Prefix(bst_ulong pvalue_, int masklen_)
      : pvalue(pvalue_), masklen(masklen_) {}

    Prefix(const Prefix & rhs): pvalue(rhs.pvalue), masklen(rhs.masklen) {}
  };

} // namespace tree
} // namespace xgboost
