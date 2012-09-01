/**
 *  \file IMP/declare_CLASSNAMEContainer.h
 *  \brief A container for CLASSNAMEs.
 *
 *  BLURB
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 */

#ifndef IMPKERNEL_DECLARE_HEADERNAME_CONTAINER_H
#define IMPKERNEL_DECLARE_HEADERNAME_CONTAINER_H

#include "kernel_config.h"
#include "internal/IndexingIterator.h"
#include "declare_Particle.h"
#include "container_base.h"
#include "internal/container_helpers.h"
#include "DerivativeAccumulator.h"
#include "internal/OwnerPointer.h"
#include "ParticleTuple.h"
#include <IMP/base/ref_counted_macros.h>
#include <IMP/base/check_macros.h>
#include <IMP/base/Pointer.h>
#include <IMP/base/InputAdaptor.h>
#include <IMP/base/utility_macros.h>


IMP_BEGIN_NAMESPACE
class CLASSNAMEModifier;
class CLASSNAMEDerivativeModifier;
class CLASSNAMEScore;

//! A shared container for CLASSNAMEs
/** Stores a searchable shared collection of CLASSNAMEs.
    \headerfile CLASSNAMEContainer.h "IMP/CLASSNAMEContainer.h"
    \implementationwithoutexample{CLASSNAMEContainer, IMP_HEADERNAME_CONTAINER}
 */
class IMPEXPORT CLASSNAMEContainer : public Container
{
  IMP_PROTECTED_CONSTRUCTOR(CLASSNAMEContainer, (Model *m,
                           std::string name="CLASSNAMEContainer %1%"), );
public:
  typedef VARIABLETYPE ContainedType;
  typedef PLURALVARIABLETYPE ContainedTypes;
  typedef PLURALINDEXTYPE ContainedIndexTypes;
  typedef INDEXTYPE ContainedIndexType;
  /** \note This function may be linear. Be aware of the complexity
      bounds of your particular container.
   */
  virtual bool get_contains_FUNCTIONNAME(ARGUMENTTYPE v) const =0;

  PLURALVARIABLETYPE get_FUNCTIONNAMEs() const {
    return IMP::internal::get_particle(get_model(),
                                       get_indexes());
  }
#ifndef IMP_DOXGEN
  //! return the number of CLASSNAMEs in the container
  /** \note this isn't always constant time
   */
  virtual unsigned int get_number_of_FUNCTIONNAMEs() const {
    return get_number();
  }
  /** Return the ith VARIABLETYPE of the container.*/
  virtual VARIABLETYPE get_FUNCTIONNAME(unsigned int i) const {
    return get(i);
  }

#endif

  //! Apply a SingletonModifier to the contents
  virtual void apply(const CLASSNAMEModifier *sm) const=0;
  //! Apply a SingletonModifier to the contents
  virtual void apply(const CLASSNAMEDerivativeModifier *sm,
                     DerivativeAccumulator &da) const=0;

  //! Evaluate a score on the contents
  virtual double evaluate(const CLASSNAMEScore *s,
                          DerivativeAccumulator *da) const=0;

  //! Evaluate a score on the contents
  virtual double evaluate_if_good(const CLASSNAMEScore *s,
                                  DerivativeAccumulator *da,
                                  double max) const=0;

#ifndef IMP_DOXYGEN
  VARIABLETYPE get(unsigned int i) const {
    return IMP::internal::get_particle(get_model(),
                                       get_indexes()[i]);
  }
  PLURALVARIABLETYPE get() const {
    return IMP::internal::get_particle(get_model(),
                                       get_indexes());
  }
  /** Return true if the container contains the passed VARIABLETYPE.*/
  bool get_contains(ARGUMENTTYPE v) const {
    return get_contains_FUNCTIONNAME(v);
  }
  /** Return true if the container contains the passed VARIABLETYPE.*/
  virtual bool get_contains_index(INDEXTYPE v) const {
    return get_contains_FUNCTIONNAME(IMP::internal
                                     ::get_particle(get_model(),
                                                    v));
  }
  unsigned int get_number() const {return get_indexes().size();}
  virtual PLURALINDEXTYPE get_indexes() const=0;
  virtual PLURALINDEXTYPE get_all_possible_indexes() const=0;
#ifndef SWIG
  virtual bool get_provides_access() const {return false;}
  virtual const PLURALINDEXTYPE& get_access() const {
    IMP_THROW("Object not implemented properly.", base::IndexException);
  }


  template <class Modifier>
  void template_apply(const Modifier *sm,
                      DerivativeAccumulator &da) const {
    apply(sm, da);
  }
  template <class Modifier>
      void template_apply(const Modifier *sm) const {
    apply(sm);
  }
  template <class Score>
      double template_evaluate(const Score *s,
                               DerivativeAccumulator *da) const {
    return evaluate(s, da);
  }
  template <class Score>
      double template_evaluate_if_good(const Score *s,
                                       DerivativeAccumulator *da,
                                       double max) const {
    return evaluate_if_good(s, da, max);
  }


#endif
#endif

  IMP_REF_COUNTED_NONTRIVIAL_DESTRUCTOR(CLASSNAMEContainer);
};


/** This class allows either a list or a container to be
    accepted as input.
*/
class IMPEXPORT CLASSNAMEContainerAdaptor:
#if !defined(SWIG) && !defined(IMP_DOXYGEN)
public base::Pointer<CLASSNAMEContainer>
#else
public base::InputAdaptor
#endif
{
  typedef base::Pointer<CLASSNAMEContainer> P;
 public:
  CLASSNAMEContainerAdaptor(){}
  CLASSNAMEContainerAdaptor(CLASSNAMEContainer *c);
  template <class C>
  CLASSNAMEContainerAdaptor(base::internal::PointerBase<C> c): P(c){}
  CLASSNAMEContainerAdaptor(const PLURALVARIABLETYPE &t,
                          std::string name="CLASSNAMEContainerAdaptor%1%");
};


IMP_END_NAMESPACE

#endif  /* IMPKERNEL_DECLARE_HEADERNAME_CONTAINER_H */