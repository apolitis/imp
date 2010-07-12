/**
 *  \file AllBipartitePairContainer.cpp   \brief A list of ParticlePairs.
 *
 *  This file is generated by a script (core/tools/make-container).
 *  Do not edit directly.
 *
 *  Copyright 2007-2010 IMP Inventors. AllBipartite rights reserved.
 *
 */

#include "IMP/container/AllBipartitePairContainer.h"
#include <IMP/core/internal/DifferenceSingletonContainer.h>
#include <IMP/PairModifier.h>
#include <IMP/SingletonModifier.h>
#include <IMP/PairScore.h>
#include <IMP/SingletonScore.h>
#include <algorithm>


#define FOREACH(expr)                                                   \
  unsigned int sza=a_->get_number_of_particles();                       \
  unsigned int szb=b_->get_number_of_particles();                       \
  for (unsigned int i=0; i< sza; ++i) {                                 \
    Particle *a= a_->get_particle(i);                                   \
    for (unsigned int j=0; j< szb; ++j) {                               \
      ParticlePair p(a, b_->get_particle(j));                           \
      expr;                                                             \
    }                                                                   \
  }


IMPCONTAINER_BEGIN_NAMESPACE

AllBipartitePairContainer
::AllBipartitePairContainer( SingletonContainer *a,
                             SingletonContainer *b,
                             bool):
  a_(a), b_(b){
}

AllBipartitePairContainer
::AllBipartitePairContainer( SingletonContainer *a,
                             SingletonContainer *b):
  a_(a), b_(b),
  deps_(new DependenciesScoreState(this), a->get_model()){
  IMP_NEW(IMP::core::internal::DifferenceSingletonContainer, olda,
          (a_,
           a_->get_removed_singletons_container()));
  IMP_NEW(IMP::core::internal::DifferenceSingletonContainer, oldb,
          (b_,
           b_->get_removed_singletons_container()));

  Pointer<PairContainerSet> removed
    = PairContainerSet::create_untracked_container();
  {
    Pointer<PairContainer> all
      = AllBipartitePairContainer
      ::create_untracked_container(a_->get_removed_singletons_container(),
                                   b_->get_removed_singletons_container());
    removed->add_pair_container(all);
    Pointer<PairContainer> leftr
      = AllBipartitePairContainer
      ::create_untracked_container(a_->get_removed_singletons_container(),
                                   oldb);
    removed->add_pair_container(leftr);
    Pointer<PairContainer> rightr
      = AllBipartitePairContainer::create_untracked_container(olda,
                                   b_->get_removed_singletons_container());
    removed->add_pair_container(rightr);
  }
  Pointer<PairContainerSet> added
    = PairContainerSet::create_untracked_container();
  {
    Pointer<PairContainer> all
      =AllBipartitePairContainer
      ::create_untracked_container(a_->get_added_singletons_container(),
                                   b_->get_added_singletons_container());
    added->add_pair_container(all);
    Pointer<PairContainer> leftr
      = AllBipartitePairContainer
      ::create_untracked_container(a_->get_added_singletons_container(),
                                   oldb);
    added->add_pair_container(leftr);
    Pointer<PairContainer> rightr
      = AllBipartitePairContainer
      ::create_untracked_container(olda,
                                   b_->get_added_singletons_container());
    added->add_pair_container(rightr);
  }
  set_added_and_removed_containers(added, removed);
}

unsigned int
AllBipartitePairContainer::get_number_of_particle_pairs() const {
  return a_->get_number_of_particles()*b_->get_number_of_particles();
}

ParticlePair AllBipartitePairContainer
::get_particle_pair(unsigned int i) const {
  unsigned int a= i%a_->get_number_of_particles();
  unsigned int b= i/a_->get_number_of_particles();
  return ParticlePair(a_->get_particle(a), b_->get_particle(b));
}

bool
AllBipartitePairContainer
::get_contains_particle_pair(const ParticlePair &p) const {
  return a_->get_contains_particle(p[0])
      && b_->get_contains_particle(p[1]);
}

void AllBipartitePairContainer::do_show(std::ostream &out) const {
  IMP_CHECK_OBJECT(this);
  out << "container " << *a_ << " and " << *b_ << std::endl;
}


ParticlesTemp AllBipartitePairContainer::get_contained_particles() const {
  ParticlesTemp ret= a_->get_contained_particles();
  ParticlesTemp b= b_->get_contained_particles();
  ret.insert(ret.end(), b.begin(), b.end());
  return ret;
}

bool AllBipartitePairContainer::get_contained_particles_changed() const {
  return a_->get_contained_particles_changed()
    || b_->get_contained_particles_changed();
}



IMP_PAIR_CONTAINER_METHODS_FROM_FOREACH(AllBipartitePairContainer);



IMPCONTAINER_END_NAMESPACE
