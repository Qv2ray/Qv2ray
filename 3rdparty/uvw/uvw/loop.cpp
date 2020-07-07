#ifdef UVW_AS_LIB
#include "loop.h"
#endif

#include "config.h"


namespace uvw {


UVW_INLINE Loop::Loop(std::unique_ptr<uv_loop_t, Deleter> ptr) noexcept
    : loop{std::move(ptr)}
{}


UVW_INLINE std::shared_ptr<Loop> Loop::create() {
    auto ptr = std::unique_ptr<uv_loop_t, Deleter>{new uv_loop_t, [](uv_loop_t *l) {
        delete l;
    }};

    auto loop = std::shared_ptr<Loop>{new Loop{std::move(ptr)}};

    if(uv_loop_init(loop->loop.get())) {
        loop = nullptr;
    }

    return loop;
}


UVW_INLINE std::shared_ptr<Loop> Loop::create(uv_loop_t *loop) {
    auto ptr = std::unique_ptr<uv_loop_t, Deleter>{loop, [](uv_loop_t *) {}};
    return std::shared_ptr<Loop>{new Loop{std::move(ptr)}};
}


UVW_INLINE std::shared_ptr<Loop> Loop::getDefault() {
    static std::weak_ptr<Loop> ref;
    std::shared_ptr<Loop> loop;

    if(ref.expired()) {
        auto def = uv_default_loop();

        if(def) {
            auto ptr = std::unique_ptr<uv_loop_t, Deleter>(def, [](uv_loop_t *) {});
            loop = std::shared_ptr<Loop>{new Loop{std::move(ptr)}};
        }

        ref = loop;
    } else {
        loop = ref.lock();
    }

    return loop;
}


UVW_INLINE Loop::~Loop() noexcept {
    if(loop) {
        close();
    }
}


UVW_INLINE void Loop::close() {
    auto err = uv_loop_close(loop.get());
    return err ? publish(ErrorEvent{err}) : loop.reset();
}


template<Loop::Mode mode>
bool Loop::run() noexcept {
    auto utm = static_cast<std::underlying_type_t<Mode>>(mode);
    auto uvrm = static_cast<uv_run_mode>(utm);
    return (uv_run(loop.get(), uvrm) == 0);
}


UVW_INLINE bool Loop::alive() const noexcept {
    return !(uv_loop_alive(loop.get()) == 0);
}


UVW_INLINE void Loop::stop() noexcept {
    uv_stop(loop.get());
}


UVW_INLINE int Loop::descriptor() const noexcept {
    return uv_backend_fd(loop.get());
}


UVW_INLINE std::pair<bool, Loop::Time> Loop::timeout() const noexcept {
    auto to = uv_backend_timeout(loop.get());
    return std::make_pair(to == -1, Time{to});
}


UVW_INLINE Loop::Time Loop::now() const noexcept {
    return Time{uv_now(loop.get())};
}


UVW_INLINE void Loop::update() const noexcept {
    return uv_update_time(loop.get());
}


UVW_INLINE void Loop::walk(std::function<void(BaseHandle &)> callback) {
    // remember: non-capturing lambdas decay to pointers to functions
    uv_walk(loop.get(), [](uv_handle_t *handle, void *func) {
        BaseHandle &ref = *static_cast<BaseHandle *>(handle->data);
        std::function<void(BaseHandle &)> &f = *static_cast<std::function<void(BaseHandle &)> *>(func);
        f(ref);
    }, &callback);
}


UVW_INLINE void Loop::fork() noexcept {
    auto err = uv_loop_fork(loop.get());

    if(err) {
        publish(ErrorEvent{err});
    }
}


UVW_INLINE void Loop::data(std::shared_ptr<void> uData) {
    userData = std::move(uData);
}


UVW_INLINE const uv_loop_t *Loop::raw() const noexcept {
    return loop.get();
}


UVW_INLINE uv_loop_t *Loop::raw() noexcept {
    return const_cast<uv_loop_t *>(const_cast<const Loop *>(this)->raw());
}


// explicit instantiations

template bool Loop::run<Loop::Mode::DEFAULT>() noexcept;
template bool Loop::run<Loop::Mode::ONCE>() noexcept;
template bool Loop::run<Loop::Mode::NOWAIT>() noexcept;


}
