/**
 *  \file QuadContainerSet.h
 *  \brief Store a set of QuadContainers
 *
 *  This file is generated by a script (core/tools/make-container).
 *  Do not edit directly.
 *
 *  Copyright 2007-2011 IMP Inventors. All rights reserved.
 */

#ifndef IMPCONTAINER_QUAD_CONTAINER_SET_H
#define IMPCONTAINER_QUAD_CONTAINER_SET_H

#include "container_config.h"
#include <IMP/QuadContainer.h>
#include <IMP/container_macros.h>
#include <IMP/internal/container_helpers.h>
#include <IMP/scoped.h>

IMPCONTAINER_BEGIN_NAMESPACE

//! Stores a set of QuadContainers
/** The input sets must be disjoint. This can change if there is
    demand for it.

    \usesconstraint
*/
class IMPCONTAINEREXPORT QuadContainerSet
  : public QuadContainer
{
  IMP_CONTAINER_DEPENDENCIES(QuadContainerSet,
                             {
                               ret.insert(ret.end(),
                                          back_->quad_containers_begin(),
                                          back_->quad_containers_end());
                             });
  static QuadContainerSet* get_set(QuadContainer* c) {
    return dynamic_cast<QuadContainerSet*>(c);
  }
 public:
  //! Construct and empty set
  QuadContainerSet(Model *m,
                        std::string name="QuadContainerSet %1%");

  QuadContainerSet(const QuadContainersTemp &pc,
                        std::string name="QuadContainerSet %1%");

  bool get_contains_particle_quad(const ParticleQuad&) const;
  unsigned int get_number_of_particle_quads() const;
  ParticleQuad get_particle_quad(unsigned int i) const;
  void apply(const QuadModifier *sm);
  void apply(const QuadDerivativeModifier *sm,
             DerivativeAccumulator &da);
  double evaluate(const QuadScore *s,
                  DerivativeAccumulator *da) const;
  double evaluate_if_good(const QuadScore *s,
                          DerivativeAccumulator *da,
                          double max) const;
 template <class SM>
  void template_apply(const SM *sm,
                      DerivativeAccumulator &da) {
   for (unsigned int i=0; i< get_number_of_quad_containers(); ++i) {
     get_quad_container(i)->apply(sm, da);
   }
 }
  template <class SM>
  void template_apply(const SM *sm) {
    for (unsigned int i=0; i< get_number_of_quad_containers(); ++i) {
      get_quad_container(i)->apply(sm);
    }
  }
  template <class SS>
  double template_evaluate(const SS *s,
                           DerivativeAccumulator *da) const {
    double ret=0;
    for (unsigned int i=0; i< get_number_of_quad_containers(); ++i) {
      ret+=get_quad_container(i)->evaluate(s, da);
    }
    return ret;
  }
  template <class SS>
    double template_evaluate_if_good(const SS *s,
                                 DerivativeAccumulator *da, double max) const {
    double ret=0;
    for (unsigned int i=0; i< get_number_of_quad_containers(); ++i) {
      double cur=get_quad_container(i)->evaluate_if_good(s, da, max);
      ret+=cur;
      max-=cur;
      if (max < 0) break;
    }
    return ret;
  }

  ParticlesTemp get_contained_particles() const;
  bool get_contents_changed() const {
    for (unsigned int i=0; i< get_number_of_quad_containers(); ++i) {
      if (get_quad_container(i)->get_contents_changed()) return true;
    }
    return false;
  }
  IMP_OBJECT(QuadContainerSet);

  /** @name Methods to control the nested container

      This container merges a set of nested containers. To add
      or remove nested containers, use the methods below.
  */
  /**@{*/
  IMP_LIST_ACTION(public, QuadContainer, QuadContainers,
                  quad_container, quad_containers,
                  QuadContainer*, QuadContainers,
                  {
                    obj->set_was_used(true);
                  },{},
                  );
  /**@}*/

#ifndef IMP_DOXYGEN
  bool get_is_up_to_date() const {
    for (unsigned int i=0;
         i< get_number_of_quad_containers(); ++i) {
      if (!get_quad_container(i)->get_is_up_to_date()) return false;
    }
    return true;
  }
#endif
};


IMPCONTAINER_END_NAMESPACE

#endif  /* IMPCONTAINER_QUAD_CONTAINER_SET_H */
