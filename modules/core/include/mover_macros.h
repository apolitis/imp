/**
 *  \file IMP/core/mover_macros.h    \brief Backwards compatibility.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef IMPCORE_MOVER_MACROS_H
#define IMPCORE_MOVER_MACROS_H

#include <IMP/base/deprecation_macros.h>
#include <IMP/core/core_config.h>
#include "Mover.h"

IMPCORE_DEPRECATED_HEADER(2.1, "Use MonteCarloMover")

/** \deprecated_at{2.1} Use IMP::core::MonteCarloMover
 */
#define IMP_MOVER(Name)                                                      \
  IMP_DEPRECATED_MACRO(2.1, "You should move to IMP::core::MonteCarloMover") \
      IMP_IMPLEMENT(virtual kernel::ParticlesTemp propose_move(Float size)); \
  IMP_IMPLEMENT(virtual void reset_move());                                  \
  IMP_IMPLEMENT(virtual kernel::ParticlesTemp get_output_particles() const); \
  IMP_OBJECT_NO_WARNING(Name)

#endif /* IMPCORE_MOVER_MACROS_H */
