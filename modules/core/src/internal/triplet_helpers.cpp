/**
 *  \file ListTripletContainer.cpp   \brief A list of ParticleTripletsTemp.
 *
 *  This file is generated by a script (core/tools/make-container).
 *  Do not edit directly.
 *
 *  Copyright 2007-2011 IMP Inventors. All rights reserved.
 *
 */

#include <IMP/core/internal/triplet_helpers.h>
#include <IMP/TripletModifier.h>
#include <IMP/TripletScore.h>

IMPCORE_BEGIN_INTERNAL_NAMESPACE

void ListLikeTripletContainer
::do_show(std::ostream &out) const {
  out << "contains " << data_.size() << std::endl;
}



IMPCORE_END_INTERNAL_NAMESPACE
