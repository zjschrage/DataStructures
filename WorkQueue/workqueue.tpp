#include "workqueue.h"

#include <iostream>

namespace ZLib {

    template <typename WorkItem, typename Handler>
    WorkQueue<WorkItem, Handler>::WorkQueue(Handler* handler) : handler_(handler) {}

    template <typename WorkItem, typename Handler>
    WorkQueue<WorkItem, Handler>::~WorkQueue() {
        workQueueRunning_.store(false, std::memory_order_release);
        queueIdleCV_.notify_one();
        workThread_.join();
    }

    template <typename WorkItem, typename Handler>
    bool WorkQueue<WorkItem, Handler>::Running() const {
        return workQueueRunning_.load(std::memory_order_acquire);
    }

    template <typename WorkItem, typename Handler>
    void WorkQueue<WorkItem, Handler>::Start() {

        auto processQueue = [this] {
            while (Running()) {
                std::unique_lock<std::mutex> lk(cvNotifyLock_);
                queueIdleCV_.wait(lk, [this]{ return !queue_.empty() || !Running(); });

                if (!Running())
                    break;

                {
                    std::scoped_lock{ queueLock_ };

                    handler_->ProcessWorkItem(std::move(queue_.front()));
                    queue_.pop();
                }
            }
        };

        workQueueRunning_.store(true, std::memory_order_release);
        workThread_ = std::thread(processQueue);
    }

    template <typename WorkItem, typename Handler>
    void WorkQueue<WorkItem, Handler>::Push(WorkItem&& workItem) {
        std::scoped_lock{ queueLock_ };
        std::unique_lock<std::mutex> lk(cvNotifyLock_);
        queue_.push(std::move(workItem));
        queueIdleCV_.notify_one();
    }

}