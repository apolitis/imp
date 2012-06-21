/**
 *  \file AddScores.h
 *  \brief A Score on the distance between a pair of particles.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 */

#ifndef IMPSCORE_FUNCTOR_ADD_SCORES_H
#define IMPSCORE_FUNCTOR_ADD_SCORES_H

#include "score_functor_config.h"
#include "Score.h"
#include <IMP/algebra/utility.h>
IMPSCOREFUNCTOR_BEGIN_NAMESPACE


/** A shift the distance by subtracting x0 and pass it to the base
    class.*/
template <class BaseDistanceScore0, class BaseDistanceScore1>
class AddScores: public Score {
  typedef BaseDistanceScore0 P0;
  typedef BaseDistanceScore1 P1;
  P0 p0_;
  P1 p1_;
public:
  AddScores(BaseDistanceScore0 p0,
            BaseDistanceScore1 p1): p0_(p0), p1_(p1){}
  template <unsigned int D>
  double get_score(Model *m, const ParticleIndexTuple<D>&pi,
                   double distance) const {
    return p0_.get_score(m,pi, distance)
      + p1_.get_score(m, pi, distance);
  }
  template <unsigned int D>
  DerivativePair get_score_and_derivative(Model *m,
                                          const ParticleIndexTuple<D>&p,
                                          double distance) const {
    DerivativePair ret0=p0_.get_score_and_derivative(m, p, distance);
    DerivativePair ret1=p1_.get_score_and_derivative(m, p, distance);
    return DerivativePair(ret0.first+ret1.first,
                          ret0.second+ret1.second);
  }
  template <unsigned int D>
  bool get_is_trivially_zero(Model *m, const ParticleIndexTuple<D>& p,
                   double squared_distance) const {
    return p0_.get_is_trivially_zero(m, p, squared_distance)
      && p1_.get_is_trivially_zero(m, p, squared_distance);
  }
  /** Return an upper bound on the distance at which the score can be
      non-zero.*/
  template <unsigned int D>
  double get_maximum_range(Model *m, const ParticleIndexTuple<D>& p) const {
    return std::max(p0_.get_maximum_range(m, p),
                    p1_.get_maximum_range(m, p));
  }
  ParticlesTemp get_input_particles(Particle *p) const {
    return p0_.get_input_particles(p)
      + p1_.get_input_particles(p);
  }
  ContainersTemp get_input_containers(Particle *p) const {
    return p0_.get_input_containers(p)
      + p1_.get_input_containers(p);
  }
  void show(std::ostream &out) const{
    p0_.show(out);
    p1_.show(out);
  }
};

IMPSCOREFUNCTOR_END_NAMESPACE

#endif  /* IMPSCORE_FUNCTOR_ADD_SCORES_H */