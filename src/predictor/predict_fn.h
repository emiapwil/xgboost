/*!
 * Copyright 2021 by XGBoost Contributors
 */
#ifndef XGBOOST_PREDICTOR_PREDICT_FN_H_
#define XGBOOST_PREDICTOR_PREDICT_FN_H_
#include "../common/categorical.h"
#include "xgboost/tree_model.h"
#include "xgboost/prefix.h"

namespace xgboost {
namespace predictor {
template <bool has_missing, bool has_categorical>
inline XGBOOST_DEVICE bst_node_t
GetNextNode(const RegTree::Node &node, const bst_node_t nid, float fvalue,
            bool is_missing, RegTree::CategoricalSplitMatrix const &cats) {
  if (has_missing && is_missing) {
    return node.DefaultChild();
  } else {
    if (has_categorical && common::IsCat(cats.split_type, nid)) {
      auto node_categories = cats.categories.subspan(cats.node_ptr[nid].beg,
                                                     cats.node_ptr[nid].size);
      return Decision(node_categories, common::AsCat(fvalue))
                 ? node.LeftChild()
                 : node.RightChild();
    } else if (cats.split_type[nid] == FeatureType::kIPAddr) {
      Prefix prefix(node.SplitPrefix(), node.SplitMaskLen());
      LOG(DEBUG) << &node;
      LOG(DEBUG) << std::hex << node.sindex_;
      uint32_t addr = static_cast<uint32_t>(fvalue);
      LOG(DEBUG) << "prefix: " << prefix.pvalue << "/"
                 << 32 - prefix.masklen << " match " << addr
                 << " = " << prefix.Match(addr)
                 << " at node #" << nid;
      return node.LeftChild() + !(prefix.Match(addr));
    } else {
      return node.LeftChild() + !(fvalue < node.SplitCond());
    }
  }
}
}      // namespace predictor
}      // namespace xgboost
#endif  // XGBOOST_PREDICTOR_PREDICT_FN_H_
