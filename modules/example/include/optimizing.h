/**
 *  \file IMP/example/optimizing.h
 *  \brief A simple unary function.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef IMPEXAMPLE_OPTIMIZING_H
#define IMPEXAMPLE_OPTIMIZING_H

#include <IMP/example/example_config.h>
#include <IMP/base/log.h>
#include <IMP/base_types.h>
#include <IMP/base/enums.h>
#include <IMP/core/MonteCarloMover.h>
#include <IMP/base/enums.h>

IMPEXAMPLE_BEGIN_NAMESPACE
/** Create a serial mover from a list of core::XYZR particles.
 */
IMPEXAMPLEEXPORT core::MonteCarloMover *create_serial_mover(
    const kernel::ParticlesTemp &ps);

/** Take a set of core::XYZR particles and relax them relative to a set of
    restraints. Excluded volume is handle separately, so don't include it
    in the passed list of restraints.

    \include optimize_balls.cpp
*/
IMPEXAMPLEEXPORT void optimize_balls(
    const kernel::ParticlesTemp &ps,
    const kernel::RestraintsTemp &rs = kernel::RestraintsTemp(),
    const PairPredicates &excluded = PairPredicates(),
    const OptimizerStates &opt_states = OptimizerStates(),
    base::LogLevel ll = base::DEFAULT);

IMPEXAMPLE_END_NAMESPACE

#endif /* IMPEXAMPLE_OPTIMIZING_H */
