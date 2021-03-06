/**
 *  \file IMP/core/FixedRefiner.h
 *  \brief A particle refiner which returns a fixed set of particles
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 */

#ifndef IMPCORE_FIXED_REFINER_H
#define IMPCORE_FIXED_REFINER_H

#include <IMP/core/core_config.h>

#include <IMP/PairContainer.h>
#include <IMP/SingletonContainer.h>
#include <IMP/Refiner.h>

IMPCORE_BEGIN_NAMESPACE

//! The refiner can refine any particle by returning a fixed set
/**
 */
class IMPCOREEXPORT FixedRefiner : public Refiner {
  kernel::ParticlesTemp ps_;

 public:
  //! Store the set of particles
  FixedRefiner(const kernel::ParticlesTemp &ps);

  virtual bool get_can_refine(kernel::Particle *) const IMP_OVERRIDE;
  virtual const kernel::ParticlesTemp get_refined(kernel::Particle *) const
      IMP_OVERRIDE;
#ifndef SWIG
  using Refiner::get_refined;
#endif
  virtual kernel::ModelObjectsTemp do_get_inputs(
      kernel::Model *m, const kernel::ParticleIndexes &pis) const IMP_OVERRIDE;
  IMP_OBJECT_METHODS(FixedRefiner);
};

IMPCORE_END_NAMESPACE

#endif /* IMPCORE_FIXED_REFINER_H */
