/**
\brief Patch script SYB boilerplate

As any .inl file this should be included just once in onece and only one .cpp
file for dynamic library or executable.
*/
#ifndef SYB_GENERIC_DIFF_INL
#define SYB_GENERIC_DIFF_INL

#include "Syb/GenericDiff.h"
#include "Syb/PolySpec.h"

namespace qx {
  namespace syb {
    DiffItem::~DiffItem(){}
    
    ACCEPTABLE_INSTANCE(DiffItem,diff::MergeArray);
    ACCEPTABLE_INSTANCE(DiffItem,diff::MergeHistArray);
    ACCEPTABLE_INSTANCE(DiffItem,diff::MergeRecord);
    ACCEPTABLE_INSTANCE(DiffItem,diff::Update);
  }
}

#endif
