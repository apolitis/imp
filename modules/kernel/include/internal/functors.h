/**
 *  \file IMP/functor.h    \brief Various important functionality
 *                                       for implementing decorators.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#ifndef IMPKERNEL_INTERNAL_FUNCTORS_H
#define IMPKERNEL_INTERNAL_FUNCTORS_H

#include "../kernel_config.h"
#include "../Model.h"

IMP_BEGIN_INTERNAL_NAMESPACE

template <class Pred, bool Sense>
class PredicateEquals {
  OwnerPointer<const Pred> p_;
  Pointer<Model> m_;
  int v_;
 public:
  typedef typename Pred::IndexArgument argument_type;
  typedef bool result_type;
  PredicateEquals(const Pred *p,
                  Model *m, int v): p_(p), m_(m), v_(v){}
  bool operator()(const argument_type &t) const {
    if (Sense) {
      return p_->get_value_index(m_, t)==v_;
    } else {
      return p_->get_value_index(m_, t)!=v_;
    }
  }
};

IMP_END_INTERNAL_NAMESPACE

#endif  /* IMPKERNEL_INTERNAL_FUNCTORS_H */