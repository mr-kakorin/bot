#ifndef THREADSAFEQ
#define THREADSAFEQ

#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>


template<typename T>
class ThreadSafeQueue {
public:
  T pop() {
    std::unique_lock<std::mutex> mlock(_mutex);
    if (!_queue.empty()) {
      auto item = _queue.front();
      _queue.pop();
      return item;
    }
    return nullptr;
  }

  void push(T item) {
    std::unique_lock<std::mutex> mlock(_mutex);
    _queue.push(item);
    mlock.unlock();
  }

  bool isEmpty() {
    return _queue.empty();
  }

protected:
  std::queue<T> _queue;
  std::mutex _mutex;
};

struct LogData {
  std::string msg;
  int32_t user_id;
};

#endif
