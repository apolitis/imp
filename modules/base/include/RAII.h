/**
 *  \file IMP/base/RAII.h    \brief Basic types used by IMP.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#ifndef IMPBASE_RAI_I_H
#define IMPBASE_RAI_I_H

#include "base_config.h"
#include "NonCopyable.h"

IMPBASE_BEGIN_NAMESPACE
/** An RAII class (Resource Aquisition Is Initialization) provides a way of
    temporarily changing something and undoing the change when the RAII object
    is destroyed. Examples include reference counting and changing the log level
    where you want to do something (increase the amount of logging), but ensure
    that when your function exits, things are put back to how they were before.

    See the \ref raii "RAII section" in the introduction for more information.

*/
class RAII: public NonCopyable {
protected:
  RAII(){}
};
IMPBASE_END_NAMESPACE

#endif  /* IMPBASE_RAI_I_H */