#pragma once
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
public:
  ThreadPool(size_t threads);
  ~ThreadPool();

  template <class F> void enqueue(F f) {
    {
      std::unique_lock<std::mutex> lock(queue_mutex);
      tasks.emplace(std::function<void()>(f));
    }
    condition.notify_one();
  }

  // Nueva función: esperar a que todas las tareas terminen
  void wait_all() {
    std::unique_lock<std::mutex> lock(queue_mutex);
    condition.wait(lock, [this]() {
      return tasks.empty() && active_tasks == 0;
    });
  }

private:
  std::vector<std::thread> workers;
  std::queue<std::function<void()>> tasks;

  std::mutex queue_mutex;
  std::condition_variable condition;
  bool stop;
  size_t active_tasks = 0;  // Nuevo contador
};
