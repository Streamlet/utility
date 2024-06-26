//------------------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   scope_exit.h
//    Author:      Streamlet
//    Create Time: 2016-09-30
//    Description: implememted like ScopeGuard in Loki library.
//
//------------------------------------------------------------------------------

#pragma once

#include <functional>

namespace xl {

class scope_guard_impl_base {
public:
  scope_guard_impl_base() : dismissed(false) {
  }
  scope_guard_impl_base(scope_guard_impl_base &that) : dismissed(that.dismissed) {
    that.dismiss();
  }
  ~scope_guard_impl_base() {
  }

protected:
  template <typename J>
  static void static_execute(J &j) {
    if (!j.dismissed) {
      j.execute();
    }
  }

public:
  void dismiss() const {
    dismissed = true;
  }

private:
  mutable bool dismissed;
};

typedef const scope_guard_impl_base &scope_guard;

template <typename F>
class scope_guard_impl : public scope_guard_impl_base {
public:
  scope_guard_impl(F fn) : scope_guard_impl_base(), m_fn(fn) {
  }
  ~scope_guard_impl() {
    static_execute(*this);
  }
  void execute() {
    m_fn();
  }

private:
  F m_fn;
};

template <typename F>
inline scope_guard_impl<F> make_guard(F f) {
  return scope_guard_impl<F>(f);
}
} // namespace xl

#define XL_BLOCK_EXIT_CONN_(s, t) s##t
#define XL_BLOCK_EXIT_CONN(s, t) XL_BLOCK_EXIT_CONN_(s, t)
#define XL_ON_BLOCK_EXIT(...)                                                                                          \
  ::xl::scope_guard XL_BLOCK_EXIT_CONN(sg, __LINE__) = ::xl::make_guard(::std::bind(__VA_ARGS__))
