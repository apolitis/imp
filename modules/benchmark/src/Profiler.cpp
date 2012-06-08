/** \file utility.cpp Benchmarking utilties
 *
 * Copyright 2007-2012 IMP Inventors. All rights reserved.
 */

#include <IMP/benchmark/Profiler.h>
#if defined(IMP_BENCHMARK_USE_GPERFTOOLS)
#include <gperftools/profiler.h>
#elif defined(IMP_BENCHMARK_USE_GOOGLEPERFTOOLS)
#include <google/profiler.h>
#endif

#include <boost/format.hpp>
#include <IMP/base/log_macros.h>
#include <IMP/base/check_macros.h>
#include <IMP/base/utility.h>

IMPBENCHMARK_BEGIN_NAMESPACE

#if defined(IMP_BENCHMARK_USE_GPERFTOOLS)\
  || defined(IMP_BENCHMARK_USE_GOOGLEPERFTOOLS)
void Profiler::start(std::string name) {
  ProfilerStart(base::get_unique_name(name).c_str());
}
void Profiler::stop(){
  ProfilerStop();
}

#else
void Profiler::start(std::string) {
  IMP_WARN("GProfTools were not found, no profiling available.\n");
}
void Profiler::stop(){}
#endif

IMPBENCHMARK_END_NAMESPACE