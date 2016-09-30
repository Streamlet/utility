//------------------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   scope_exit.h
//    Author:      Streamlet
//    Create Time: 2016-09-30
//    Description: 
//
//------------------------------------------------------------------------------

#ifndef __SCOPE_EXIT_H_0B6274B9_A726_481D_BC4F_66C571E352FF_INCLUDED__
#define __SCOPE_EXIT_H_0B6274B9_A726_481D_BC4F_66C571E352FF_INCLUDED__


#include <functional>

namespace stdex
{
    class scope_guard_impl_base {
    public:
        scope_guard_impl_base() : dismissed(false) { }
        scope_guard_impl_base(scope_guard_impl_base &that) :  dismissed(that.dismissed) {
            that.dismiss();
        }
        ~scope_guard_impl_base() { }
    protected:
        template <typename J>
        static void static_execute(J &j) {
            if (!j.dismissed)
                j.execute();
        }
    public:
        void dismiss() const {
            dismissed = true;
        }
    private:
        mutable bool dismissed;
    };

    typedef const scope_guard_impl_base& scope_guard;

    template <typename F>
    class scope_guard_impl : public scope_guard_impl_base {
    public:
        scope_guard_impl(F fn) : scope_guard_impl_base(), m_fn(fn) { }
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
    inline scope_guard_impl<F> make_guard(F f)
    {
        return scope_guard_impl<F>(f);
    }
}

#define STDEX_BLOCK_EXIT_CONN_(s, t)     s##t
#define STDEX_BLOCK_EXIT_CONN(s, t)      STDEX_BLOCK_EXIT_CONN_(s, t)
#define STDEX_ON_BLOCK_EXIT(...) ::stdex::scope_guard STDEX_BLOCK_EXIT_CONN(sg, __LINE__) = ::stdex::make_guard(::std::bind(__VA_ARGS__))

#endif // #ifndef __SCOPE_EXIT_H_0B6274B9_A726_481D_BC4F_66C571E352FF_INCLUDED__
