/**
 *  \file IMP/kernel/Optimizer.h     \brief Base class for all optimizers.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef IMPKERNEL_OPTIMIZER_H
#define IMPKERNEL_OPTIMIZER_H

#include <IMP/kernel/kernel_config.h>
#include "base_types.h"
#include <IMP/base/Object.h>
#include "utility.h"
#include "Model.h"
#include "Particle.h"
#include "ModelObject.h"
#include <IMP/base/Pointer.h>
#include "OptimizerState.h"
#include <IMP/base/Vector.h>
#include <limits>
#include <cmath>

IMPKERNEL_BEGIN_NAMESPACE

//! Base class for all optimizers.
/** An optimizer attempts to improve the current configuration of the
    Model by moving particles around so as to lower the score.

    The Optimizer maintains a list of OptimizerStates which are
    updated each time the conformation is changed.

    The optimizers have one key method Optimizer::optimize() which takes
    the number of steps to perform. The optimizers can have other
    stopping conditions as appropriate.

    A typical optimization loop proceeds by:
    - the optimizer calls Model::evaluate() to compute the score
    (and possibly the derivatives) of the
    current conformation of the Model.
    - the optimizer uses this information to update the optimizeable
    parameters of the Particles contained in the Model.
*/
class IMPKERNELEXPORT Optimizer : public ModelObject {
  mutable Floats widths_;
  base::Pointer<Model> my_model_;
  double min_score_;
  bool stop_on_good_score_;
  mutable base::Pointer<ScoringFunction> cache_;
  base::Pointer<ScoringFunction> scoring_function_;

  static void set_optimizer_state_optimizer(OptimizerState *os, Optimizer *o);

 protected:
#ifndef IMP_DOXYGEN
  void set_is_optimizing_states(bool tf) const;
#endif
  ModelObjectsTemp get_optimizer_state_inputs() const;
  virtual ModelObjectsTemp do_get_inputs() const IMP_OVERRIDE {
    return get_optimizer_state_inputs();
  }
  //! don't return anything here to avoid pointless dependencies
  virtual ModelObjectsTemp do_get_outputs() const { return ModelObjectsTemp(); }

 public:
  Optimizer(kernel::Model *m, std::string name = "Optimizer %1%");

  //! Optimize the model for up to max_steps iterations
  /** Optimize the model

      @param[in] max_steps The maximum number of iterations of the
      optimizer to perform. Increasing this number will generally make
      the optimizer spend more time searching for a solution, but
      beyond that, the details of what changes will vary.

      @return The final score.
  */
  double optimize(unsigned int max_steps);

  /** Optimization can be stopped if all the thresholds in the Model are
      satisfied. */
  void set_stop_on_good_score(bool tf) { stop_on_good_score_ = tf; }
  bool get_stop_on_good_score() const { return stop_on_good_score_; }
  //! Return the score found in the last evaluate
  double get_last_score() const { return cache_->get_last_score(); }

  //! Return the scoring function that is being used
  ScoringFunction *get_scoring_function() const {
    if (scoring_function_)
      return scoring_function_;
    else if (cache_)
      return cache_;
    else
      return cache_ = get_model()->create_model_scoring_function();
  }

  /** @name States

      The stored OptimizerState objects are updated each time the
      Optimizer decides to accept a new configuration of the Model.
      To manipulate the list of optimizer states use the methods below.
  */
  /**@{*/
  IMP_LIST_ACTION(public, OptimizerState, OptimizerStates, optimizer_state,
                  optimizer_states, OptimizerState *, OptimizerStates,
                  set_optimizer_state_optimizer(obj, this);
                  , {},
                  { Optimizer::set_optimizer_state_optimizer(obj, nullptr); });
  /**@}*/

  /** By default, the Optimizer uses the scoring function provided by
      the model, but you can use another scoring function instead.
  */
  virtual void set_scoring_function(ScoringFunctionAdaptor sf);

  IMP_REF_COUNTED_NONTRIVIAL_DESTRUCTOR(Optimizer);

  /** \deprecated_at{2.1} Do not use as it is not reliably supported. */
  IMPKERNEL_DEPRECATED_METHOD_DECL(2.1)
  void set_score_threshold(double s);
  /** \deprecated_at{2.1} Do not use as it is not reliably supported. */
  IMPKERNEL_DEPRECATED_METHOD_DECL(2.1)
  double get_score_threshold() const;

  // swig needs this at the end for some reason I don't understand
 protected:
  //! override this function to do actual optimization
  virtual double do_optimize(unsigned int ns) = 0;
  //! Update optimizer states, should be called at each successful step
  /** Make sure the scoring function restraints are up to date before this is
      called (eg by calling evaluate).*/
  void update_states() const;
};

IMP_OBJECTS(Optimizer, Optimizers);

IMPKERNEL_END_NAMESPACE

#endif /* IMPKERNEL_OPTIMIZER_H */
