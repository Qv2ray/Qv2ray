#ifdef UVW_AS_LIB
#include "work.h"
#endif

#include <utility>

#include "config.h"


namespace uvw {


UVW_INLINE WorkReq::WorkReq(ConstructorAccess ca, std::shared_ptr<Loop> ref, InternalTask t)
    : Request{ca, std::move(ref)}, task{t}
{}


UVW_INLINE void WorkReq::workCallback(uv_work_t *req) {
    static_cast<WorkReq*>(req->data)->task();
}


UVW_INLINE void WorkReq::queue() {
    invoke(&uv_queue_work, parent(), get(), &workCallback, &defaultCallback<WorkEvent>);
}


}

