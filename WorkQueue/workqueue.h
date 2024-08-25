#pragma once

#include <queue>
#include <thread>
#include <atomic>
#include <condition_variable>

namespace ZLib {

    template <typename WorkItem, typename Handler>
    class WorkQueue {
    public:
        WorkQueue(Handler* handler);
        ~WorkQueue();
        void Start();
        void Push(WorkItem&& workItem);
    
    private:
        bool Running() const;

    private:
        std::queue<WorkItem> queue_;
        std::atomic<bool> workQueueRunning_{false};
        std::thread workThread_;
        std::condition_variable queueIdleCV_;
        mutable std::mutex cvNotifyLock_;
        mutable std::mutex queueLock_;
        Handler* handler_{nullptr};
    };
}

#include "workqueue.tpp"