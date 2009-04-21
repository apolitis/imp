/**
 *  \file QuadraticClosePairsFinder.cpp
 *  \brief Test all pairs.
 *
 *  Copyright 2007-9 Sali Lab. All rights reserved.
 *
 */

#include "IMP/core/QuadraticClosePairsFinder.h"
#include "IMP/core/XYZDecorator.h"
#include <cmath>

IMPCORE_BEGIN_NAMESPACE

QuadraticClosePairsFinder::QuadraticClosePairsFinder(){}

QuadraticClosePairsFinder::~QuadraticClosePairsFinder(){}

void QuadraticClosePairsFinder
::add_close_pairs(SingletonContainer *ca,
                  SingletonContainer *cb,
                  Float distance,
                  FloatKey radius_key,
                  FilteredListPairContainer *out) const {
  for (SingletonContainer::ParticleIterator it = ca->particles_begin();
       it != ca->particles_end(); ++it) {
    for (SingletonContainer::ParticleIterator it2 = cb->particles_begin();
         it2 != cb->particles_end(); ++it2) {
      if (get_are_close(*it, *it2, distance, radius_key)) {
        out->add_particle_pair(ParticlePair(*it, *it2));
      }
    }
  }
}

void QuadraticClosePairsFinder
::add_close_pairs(SingletonContainer *c,
                  Float distance,
                  FloatKey radius_key,
                  FilteredListPairContainer *out) const {
  IMP_LOG(TERSE, "Adding close pairs from "
          << c->get_number_of_particles() << " particles." << std::endl);
  for (SingletonContainer::ParticleIterator it = c->particles_begin();
       it != c->particles_end(); ++it) {
    for (SingletonContainer::ParticleIterator it2 = c->particles_begin();
       it2 != it; ++it2) {
      if (get_are_close(*it, *it2, distance, radius_key)) {
        out->add_particle_pair(ParticlePair(*it, *it2));
      }
    }
  }
}


bool QuadraticClosePairsFinder::get_are_close(Particle *a, Particle *b,
                                              Float distance,
                                              FloatKey rk) const {
  XYZDecorator da(a);
  XYZDecorator db(b);
  Float ra= get_radius(a, rk);
  Float rb= get_radius(b, rk);
  Float sr= ra+rb;
  for (unsigned int i=0; i< 3; ++i) {
    double delta=std::abs(da.get_coordinate(i) - db.get_coordinate(i));
    if (delta - sr >= distance) {
      return false;
    }
  }
  return true;
}

IMPCORE_END_NAMESPACE
