/**
 *  \file LeavesRefiner.cpp
 *  \brief Return the hierarchy children of a particle.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 */

#include <IMP/core/LeavesRefiner.h>

#include <IMP/core/Hierarchy.h>

IMPCORE_BEGIN_NAMESPACE

LeavesRefiner::LeavesRefiner(HierarchyTraits traits)
    : Refiner("LeavesRefiner%d"), traits_(traits) {}

bool LeavesRefiner::get_can_refine(kernel::Particle *p) const {
  return core::Hierarchy::get_is_setup(p, traits_);
}

const kernel::ParticlesTemp LeavesRefiner::get_refined(
    kernel::Particle *p) const {
  // force filling of the cache, yeah, its not good organization
  IMP_INTERNAL_CHECK(get_can_refine(p), "Trying to refine the unrefinable");
  return core::get_leaves(Hierarchy(p, traits_));
}

ModelObjectsTemp LeavesRefiner::do_get_inputs(
    kernel::Model *m, const kernel::ParticleIndexes &pis) const {
  kernel::ModelObjectsTemp ret;
  for (unsigned int i = 0; i < pis.size(); ++i) {
    ret += get_all_descendants(Hierarchy(m, pis[i], traits_));
  }
  return ret;
}

IMPCORE_END_NAMESPACE
