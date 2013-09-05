/**
 *  \file IMP/base/declare_Object.h
 *  \brief A shared base class to help in debugging and things.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef IMPBASE_DECLARE_OBJECT_H
#define IMPBASE_DECLARE_OBJECT_H

#include <IMP/base/base_config.h>
#include <IMP/base/base_config.h>
#include "exception.h"
#include "utility_macros.h"
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include "NonCopyable.h"
#include "ref_counted_macros.h"
#include "enums.h"
#include "hash_macros.h"
#include "warning_macros.h"
#include "showable_macros.h"
#include "VersionInfo.h"
#include "utility_macros.h"
#include <IMP/base/hash.h>
#include "hash.h"
#include <boost/scoped_array.hpp>

#if !defined(IMP_HAS_CHECKS)
#error "IMP_HAS_CHECKS not defined, something is broken"
#endif
#if !defined(IMP_NONE)
#error "IMP_NONE not defined, something is broken"
#endif
#if !defined(IMP_HAS_LOG)
#error "IMP_HAS_LOG not defined, something is broken"
#endif
#if !defined(IMP_SILENT)
#error "IMP_SILENT not defined, something is broken"
#endif


#ifndef IMP_DOXYGEN
#ifndef SWIG

namespace IMP {
namespace base {
class Object;
namespace internal {
template <class R, class E>
struct RefStuff;
}
}
}
#endif
#endif


IMPBASE_BEGIN_NAMESPACE

//! Common base class for heavy weight \imp objects.
/** The base class for non value-type objects in \imp.
        Anything inheriting from IMP::Object has the following
        properties:
        - has a method Object::show() which writes one or more lines of text
          to a stream
        - has embedded information about the module and version which can be
          accessed using Object::get_version_info(). This information can be
          used to log what version of software is used to compute a result.
        - it has a local logging level which can override the global one
          allowing fine grained logging control.
        - in python, there is a method Class::get_from(Object *o) that
    attempts
          to case o to an object of type Class and throws and exception if
    it
          fails.
        - the object keeps track of whether it has been been used. See the
          IMP::Object::set_was_used() method for an explanation.
        - It is ref counted

        Objects can be outputted to standard streams using operator<<()
        which will call the Object::show() method.

        Types inheriting from Object should always be created using \c
        new in C++ and passed using pointers and stored using Pointer
        objects. Note that you have to be careful of cycles and so
        must use WeakPointer objects to break cycles. See RefCounted
        for more information on reference counting.  IMP_NEW() can
        help shorten creating a ref counted pointer. See IMP::Pointer
        for more information.

        Reference counting is a technique for managing memory and
        automatically freeing memory (destroying objects) when it
        is no longer needed. In reference counting, each object has a
    reference
        count, which tracks how many different places are using the
        object. When this count goes to 0, the object is freed.\n\n
        Python internally refence counts everything. C++, on the other hand,
        requires extra steps be taken to ensure that objects
        are reference counted properly.\n\n
        In \imp, reference counting is done through the IMP::Pointer
        and IMP::RefCounted classes. The former should be used instead of
        a raw C++ pointer when storing a pointer to any object
        inheriting from IMP::RefCounted.\n\n
        Any time one is using reference counting, one needs to be aware
        of cycles, since if, for example, object A contains an IMP::Pointer
    to
        object B and object B contains an IMP::base::Pointer to object A,
        their reference counts will never go to 0 even if both A
        and B are no longer used. To avoid this, use an
        IMP::base::WeakPointer in one of A or B.

       \headerfile Object.h "IMP/base/Object.h"

     */
class IMPBASEEXPORT Object : public NonCopyable {
  std::string name_;
  boost::scoped_array<char> quoted_name_;

  static unsigned int live_objects_;
  mutable int count_;

#if IMP_HAS_LOG != IMP_NONE
  LogLevel log_level_;
#endif

#if IMP_HAS_CHECKS >= IMP_USAGE
  CheckLevel check_level_;
  mutable bool was_owned_;
  double check_value_;
#endif

#if IMP_HAS_CHECKS >= IMP_INTERNAL
  static void add_live_object(Object* o);
  static void remove_live_object(Object* o);
#endif

  void initialize(std::string name);

  int compare(const Object& o) const {
    if (&o < this)
      return 1;
    else if (&o > this)
      return -1;
    else
      return 0;
  }
  IMP_REF_COUNTED_NONTRIVIAL_DESTRUCTOR(Object);

 protected:
  //! Construct an object with the given name
  /** An instance of "%1%" in the string will be replaced by a unique
      index.
   */
  Object(std::string name);

  /** \deprecated_at{2.1} Provide a name. */
  IMPBASE_DEPRECATED_FUNCTION_DECL(2.1)
    Object();

 public:
  // needed for python to make sure all wrapper objects are equivalent
  IMP_HASHABLE_INLINE(Object, return boost::hash_value(this););

  //! Set the logging level used in this object
  /** Each object can be assigned a different log level in order to,
      for example, suppress messages for verbose and uninteresting
      objects. If set to DEFAULT, the global level as returned by
      IMP::get_log_level() is used, otherwise
      the local one is used. Methods in classes inheriting from
      Object should start with IMP_OBJECT_LOG to change the log
      level to the local one for this object and increase
      the current indent.

      \see IMP::base::set_log_level()
      \see IMP::base::get_log_level()
   */
  void set_log_level(LogLevel l);

  /** Each object can be assigned a different check level too.
   */
  void set_check_level(CheckLevel l) {
    IMP_CHECK_VARIABLE(l);
#if IMP_HAS_CHECKS != IMP_NONE
    check_level_ = l;
#endif
  }

#ifndef IMP_DOXYGEN
  LogLevel get_log_level() const {
#if IMP_HAS_LOG == IMP_SILENT
    return SILENT;
#else
    return log_level_;
#endif
  }
  CheckLevel get_check_level() const {
#if IMP_HAS_CHECKS == IMP_NONE
    return NONE;
#else
    return check_level_;
#endif
  }
#endif  // IMP_DOXYGEN

#ifndef IMP_DOXYGEN
  void _debugger_show() const { show(std::cout); }

  //! Return a string version of the object, can be used in the debugger
  std::string get_string() const {
    std::ostringstream oss;
    show(oss);
    return oss.str();
  }
#endif  // IMP_DOXYGEN

  //! Get information about the module and version of the object
  virtual VersionInfo get_version_info() const {
    return VersionInfo("none", "none");
  }

  /** @name Names
       All objects have names to aid in debugging and inspection
       of the state of the system. These names are not necessarily unique
       and should not be used to store data or as keys into a table. Use
       the address of the object instead since objects cannot be copied.
       @{
   */
  const std::string& get_name() const { return name_; }
#if !defined(IMP_DOXYGEN) && !defined(SWIG)
  const char* get_quoted_name_c_string() const { return quoted_name_.get(); }
#endif
  void set_name(std::string name);
  virtual std::string get_type_name() const { return "unknown object type"; }
  /* @} */

  /** \imp provides warnings when objects are never used before they are
      destroyed. Examples of use include adding an IMP::Restraint to an
      IMP::kernel::Model. If an object is not properly marked as used, or your
      code is the one using it, call set_was_used(true) on the object.
  */
  void set_was_used(bool tf) const {
    IMP_CHECK_VARIABLE(tf);
#if IMP_HAS_CHECKS >= IMP_USAGE
    was_owned_ = tf;
#endif
  }

  IMP_SHOWABLE(Object);

#ifndef IMP_DOXYGEN
  void _on_destruction();
#endif

#ifndef IMP_DOXYGEN
  // Return whether the object already been freed
  bool get_is_valid() const {
#if IMP_HAS_CHECKS == IMP_NONE
    return true;
#else
    return static_cast<int>(check_value_) == 111111111;
#endif
  }

  unsigned int get_ref_count() const { return count_; }
  static unsigned int get_number_of_live_objects() {
    // for debugging purposes only
    return live_objects_;
  }
  bool get_is_shared() const { return count_ > 1; }
#endif // IMP_DOXYGEN

  /** Objects can have internal caches. This method resets them returning
      the object to its just-initialized state.
  */
  virtual void clear_caches() {}

  /** Overide this method to take action on destruction. */
  virtual void do_destroy() {}
};

IMPBASE_END_NAMESPACE

#endif /* IMPBASE_DECLARE_OBJECT_H */
