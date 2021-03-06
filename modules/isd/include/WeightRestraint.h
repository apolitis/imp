/**
 *  \file isd/WeightRestraint.h
 *  \brief Put description here
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef IMPISD_WEIGHT_RESTRAINT_H
#define IMPISD_WEIGHT_RESTRAINT_H
#include <IMP/isd/Scale.h>
#include "isd_config.h"
#include <IMP/kernel/Restraint.h>
#include <IMP/restraint_macros.h>

IMPISD_BEGIN_NAMESPACE
/** A restraint for in-vivo ensemble FRET data
 */

class IMPISDEXPORT  WeightRestraint : public kernel::Restraint
{
    kernel::Particle *w_;
    Float wmin_;
    Float wmax_;
    Float kappa_;

public:

  //! Create the restraint.
  /** kernel::Restraints should store the particles they are to act on,
      preferably in a Singleton or PairContainer as appropriate.
   */

  WeightRestraint(kernel::Particle *w, Float wmin, Float wmax, Float kappa);

  /** This macro declares the basic needed methods: evaluate and show
   */
  virtual double
  unprotected_evaluate(IMP::kernel::DerivativeAccumulator *accum)
     const IMP_OVERRIDE;
  virtual IMP::kernel::ModelObjectsTemp do_get_inputs() const IMP_OVERRIDE;
  IMP_OBJECT_METHODS(WeightRestraint);

};

IMPISD_END_NAMESPACE

#endif  /* IMPISD_WEIGHT_RESTRAINT_H */
