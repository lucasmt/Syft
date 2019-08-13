#ifndef LOGGER_H
#define LOGGER_H

#include <chrono>
#include <memory>

#include "cuddObj.hh"

#include "SkolemFunction.hpp"

#include "DFA.h"
#include "SyftMgr.h"
#include "SymbolicDFA.h"

class Logger
{
public:

  virtual void record_message(const std::string& message) = 0;
  virtual void record_dfas(const std::vector<DFA>& dfas) = 0;
  virtual void record_symbolic_dfas(const std::vector<SymbolicDFA>& dfas,
                                    const SyftMgr& mgr) = 0;
  virtual void record_vars(const SyftMgr& mgr) = 0;
  virtual void record_winning_states(const BDD& winning_states) = 0;
  virtual void record_strategy(const std::vector<SkolemFunction>& strategy,
                               const SyftMgr& mgr) = 0;
  virtual void record_result(bool realizable) = 0;
  virtual void start_timer() = 0;
  virtual void record_elapsed_time() = 0;

  static std::unique_ptr<Logger> build(const std::string& logging_option);
};

class NullLogger : public Logger
{
public:

  void record_message(const std::string& message) {}
  void record_dfas(const std::vector<DFA>& dfas) {}
  void record_symbolic_dfas(const std::vector<SymbolicDFA>& dfas,
                            const SyftMgr& mgr) {}
  void record_vars(const SyftMgr& mgr) {}
  void record_winning_states(const BDD& winning_states) {}
  void record_strategy(const std::vector<SkolemFunction>& strategy,
                       const SyftMgr& mgr) {}
  void record_result(bool realizable) {}
  void start_timer() {}
  void record_elapsed_time() {}
};

class PrettyLogger : public Logger
{
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time;

public:

  PrettyLogger();

  void record_message(const std::string& message);
  void record_dfas(const std::vector<DFA>& dfas);
  void record_symbolic_dfas(const std::vector<SymbolicDFA>& dfas,
                            const SyftMgr& mgr);
  void record_vars(const SyftMgr& mgr);
  void record_winning_states(const BDD& winning_states);
  void record_strategy(const std::vector<SkolemFunction>& strategy,
                       const SyftMgr& mgr);
  void record_result(bool realizable);
  void start_timer();
  void record_elapsed_time();
};

#endif // LOGGER_H
