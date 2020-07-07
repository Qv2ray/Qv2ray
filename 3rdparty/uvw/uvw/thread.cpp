#ifdef UVW_AS_LIB
#include "thread.h"
#endif

#include "config.h"


namespace uvw {


UVW_INLINE Thread::Thread(ConstructorAccess ca, std::shared_ptr<Loop> ref, Task t, std::shared_ptr<void> d) noexcept
    : UnderlyingType{ca, std::move(ref)}, data{std::move(d)}, task{std::move(t)}
{}


UVW_INLINE void Thread::createCallback(void *arg) {
    Thread &thread = *(static_cast<Thread *>(arg));
    thread.task(thread.data);
}


UVW_INLINE Thread::Type Thread::self() noexcept {
    return uv_thread_self();
}


UVW_INLINE bool Thread::equal(const Thread &tl, const Thread &tr) noexcept {
    return !(0 == uv_thread_equal(tl.get(), tr.get()));
}


UVW_INLINE Thread::~Thread() noexcept {
    join();
}


UVW_INLINE bool Thread::run() noexcept {
    return (0 == uv_thread_create(get(), &createCallback, this));
}


UVW_INLINE bool Thread::run(Flags<Options> opts, std::size_t stack) noexcept {
    uv_thread_options_t params{opts, stack};
    return (0 == uv_thread_create_ex(get(), &params, &createCallback, this));
}


UVW_INLINE bool Thread::join() noexcept {
    return (0 == uv_thread_join(get()));
}


UVW_INLINE ThreadLocalStorage::ThreadLocalStorage(UnderlyingType<ThreadLocalStorage, uv_key_t>::ConstructorAccess ca, std::shared_ptr<Loop> ref) noexcept
    :UnderlyingType{ca, std::move(ref)}
{
    uv_key_create(UnderlyingType::get());
}


UVW_INLINE ThreadLocalStorage::~ThreadLocalStorage() noexcept {
    uv_key_delete(UnderlyingType::get());
}


UVW_INLINE uv_once_t *Once::guard() noexcept {
    static uv_once_t once = UV_ONCE_INIT;
    return &once;
}


UVW_INLINE Mutex::Mutex(UnderlyingType<Mutex, uv_mutex_t>::ConstructorAccess ca, std::shared_ptr<Loop> ref, bool recursive) noexcept
    : UnderlyingType{ca, std::move(ref)}
{
    if(recursive) {
        uv_mutex_init_recursive(get());
    } else {
        uv_mutex_init(get());
    }
}


UVW_INLINE Mutex::~Mutex() noexcept {
    uv_mutex_destroy(get());
}


UVW_INLINE void Mutex::lock() noexcept {
    uv_mutex_lock(get());
}


UVW_INLINE bool Mutex::tryLock() noexcept {
    return (0 == uv_mutex_trylock(get()));
}


UVW_INLINE void Mutex::unlock() noexcept {
    uv_mutex_unlock(get());
}


UVW_INLINE RWLock::RWLock(UnderlyingType<RWLock, uv_rwlock_t>::ConstructorAccess ca, std::shared_ptr<Loop> ref) noexcept
    : UnderlyingType{ca, std::move(ref)}
{
    uv_rwlock_init(get());
}


UVW_INLINE RWLock::~RWLock() noexcept {
    uv_rwlock_destroy(get());
}


UVW_INLINE void RWLock::rdLock() noexcept {
    uv_rwlock_rdlock(get());
}


UVW_INLINE bool RWLock::tryRdLock() noexcept {
    return (0 == uv_rwlock_tryrdlock(get()));
}


UVW_INLINE void RWLock::rdUnlock() noexcept {
    uv_rwlock_rdunlock(get());
}


UVW_INLINE void RWLock::wrLock() noexcept {
    uv_rwlock_wrlock(get());
}


UVW_INLINE bool RWLock::tryWrLock() noexcept {
    return (0 == uv_rwlock_trywrlock(get()));
}


UVW_INLINE void RWLock::wrUnlock() noexcept {
    uv_rwlock_wrunlock(get());
}


UVW_INLINE Semaphore::Semaphore(UnderlyingType<Semaphore, uv_sem_t>::ConstructorAccess ca, std::shared_ptr<Loop> ref, unsigned int value) noexcept
    : UnderlyingType{ca, std::move(ref)}
{
    uv_sem_init(get(), value);
}


UVW_INLINE Semaphore::~Semaphore() noexcept {
    uv_sem_destroy(get());
}


UVW_INLINE void Semaphore::post() noexcept {
    uv_sem_post(get());
}


UVW_INLINE void Semaphore::wait() noexcept {
    uv_sem_wait(get());
}


UVW_INLINE bool Semaphore::tryWait() noexcept {
    return (0 == uv_sem_trywait(get()));
}


UVW_INLINE Condition::Condition(UnderlyingType<Condition, uv_cond_t>::ConstructorAccess ca, std::shared_ptr<Loop> ref) noexcept
    : UnderlyingType{ca, std::move(ref)}
{
    uv_cond_init(get());
}


UVW_INLINE Condition::~Condition() noexcept {
    uv_cond_destroy(get());
}


UVW_INLINE void Condition::signal() noexcept {
    uv_cond_signal(get());
}


UVW_INLINE void Condition::broadcast() noexcept {
    uv_cond_broadcast(get());
}


UVW_INLINE void Condition::wait(Mutex &mutex) noexcept {
    uv_cond_wait(get(), mutex.get());
}


UVW_INLINE bool Condition::timedWait(Mutex &mutex, uint64_t timeout) noexcept {
    return (0 == uv_cond_timedwait(get(), mutex.get(), timeout));
}


UVW_INLINE Barrier::Barrier(UnderlyingType<Barrier, uv_barrier_t>::ConstructorAccess ca, std::shared_ptr<Loop> ref, unsigned int count) noexcept
    : UnderlyingType{ca, std::move(ref)}
{
    uv_barrier_init(get(), count);
}


UVW_INLINE Barrier::~Barrier() noexcept {
    uv_barrier_destroy(get());
}


UVW_INLINE bool Barrier::wait() noexcept {
    return (0 == uv_barrier_wait(get()));
}


}
