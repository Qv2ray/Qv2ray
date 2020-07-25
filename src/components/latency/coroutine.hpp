#pragma once
namespace detail
{

    struct coroutine_ref;

} // namespace detail

struct coroutine
{
    coroutine() : line_(0)
    {
    }
    bool await_ready() const
    {
        return line_ == -1;
    }

  private:
    friend struct detail::coroutine_ref;
    int line_;
};

namespace detail
{

    struct coroutine_ref
    {
        coroutine_ref(coroutine &c) : line_(c.line_), modified_(false)
        {
        }
        ~coroutine_ref()
        {
            if (!modified_)
                line_ = -1;
        }
        operator int() const
        {
            return line_;
        }
        int &operator=(int v)
        {
            modified_ = true;
            return line_ = v;
        }

      private:
        void operator=(coroutine_ref const &);

        int &line_;
        bool modified_;
    };

} // namespace detail

#define co_enter(c)                                                                                                                             \
    switch (::detail::coroutine_ref _coro_value = c)                                                                                            \
    case -1:                                                                                                                                    \
        if (_coro_value)                                                                                                                        \
        {                                                                                                                                       \
            goto terminate_coroutine;                                                                                                           \
        terminate_coroutine:                                                                                                                    \
            _coro_value = -1;                                                                                                                   \
            goto bail_out_of_coroutine;                                                                                                         \
        bail_out_of_coroutine:                                                                                                                  \
            break;                                                                                                                              \
        }                                                                                                                                       \
        else                                                                                                                                    \
        case 0:

#define __co_yield_impl(n)                                                                                                                      \
    for (_coro_value = (n);;)                                                                                                                   \
        if (_coro_value == 0)                                                                                                                   \
        {                                                                                                                                       \
            case (n):; break;                                                                                                                   \
        }                                                                                                                                       \
        else                                                                                                                                    \
            switch (_coro_value ? 0 : 1)                                                                                                        \
                for (;;) case -1:                                                                                                               \
                    if (_coro_value)                                                                                                            \
                        goto terminate_coroutine;                                                                                               \
                    else                                                                                                                        \
                        for (;;) case 1:                                                                                                        \
                            if (_coro_value)                                                                                                    \
                                goto bail_out_of_coroutine;                                                                                     \
                            else                                                                                                                \
                            case 0:

#define co_yield __co_yield_impl(__LINE__)
#define coro(f)                                                                                                                                 \
    [this, ptr = (std::enable_shared_from_this<T>::shared_from_this())](auto &&e, auto &&h) {                                                   \
        f(std::forward<decltype(e)>(e), std::forward<decltype(h)>(h));                                                                          \
    }
