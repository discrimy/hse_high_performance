// #include <iostream>

// int main() {
//     std::cout << "Hello world!" << std::endl;
// }

#include "pngwriter.h"
#include <condition_variable>
#include <functional>
#include <queue>
#include <thread>

class ThreadPool {
public:
  void start() {
    const uint32_t num_threads = std::thread::hardware_concurrency();
    for (uint32_t i = 0; i < num_threads; i++) {
      threads.emplace_back(std::thread(&ThreadPool::ThreadLoop, this));
    }
  }
  void enqueue(const std::function<void()> &job) {
    {
      std::unique_lock<std::mutex> lock(queue_mutex);
      jobs.push(job);
    }
    mutex_condition.notify_one();
  }
  void stop() {
    {
      std::unique_lock<std::mutex> lock(queue_mutex);
      should_terminate = true;
    }
    mutex_condition.notify_all();
    for (std::thread &active_thread : threads) {
      active_thread.join();
    }
    threads.clear();
  }
  bool busy() {
    bool pool_busy;
    {
      std::unique_lock<std::mutex> lock(queue_mutex);
      pool_busy = !jobs.empty();
    }
    return pool_busy;
  }

private:
  void ThreadLoop() {
    while (true) {
      std::function<void()> job;
      {
        std::unique_lock<std::mutex> lock(queue_mutex);
        mutex_condition.wait(
            lock, [this] { return !jobs.empty() || should_terminate; });
        if (should_terminate) {
          return;
        }
        job = jobs.front();
        jobs.pop();
      }
      job();
    }
  }

  bool should_terminate = false;
  std::mutex queue_mutex;
  std::condition_variable mutex_condition;
  std::vector<std::thread> threads;
  std::queue<std::function<void()>> jobs;
};

int main() {
  const int SIZE = 8192;
  const int CHUNK_SIZE = 512; // должно нацело делиться на SIZE
  pngwriter image(SIZE, SIZE, 1.0, "result.png");

  ThreadPool pool;
  pool.start();

  int chunks = SIZE / CHUNK_SIZE;
  for (int chunk_x = 0; chunk_x < chunks; chunk_x++) {
    for (int chunk_y = 0; chunk_y < chunks; chunk_y++) {
      pool.enqueue([&image, chunk_x, chunk_y] {
        for (int pixel_x = chunk_x * CHUNK_SIZE;
             pixel_x < (chunk_x + 1) * CHUNK_SIZE; pixel_x++) {
          for (int pixel_y = chunk_y * CHUNK_SIZE;
               pixel_y < (chunk_y + 1) * CHUNK_SIZE; pixel_y++) {
            double cx = ((double)pixel_x - ((double)SIZE) / 2) / SIZE * 3.0;
            double cyi = ((double)pixel_y - ((double)SIZE) / 2) / SIZE * 3.0;

            bool draw = true;
            double zx = 0;
            double zyi = 0;
            for (int i = 0; i < 2000; i++) {
              double zx_prev = zx;
              double zyi_prev = zyi;
              zx = zx_prev * zx_prev - zyi_prev * zyi_prev + cx;
              zyi = 2 * zx_prev * zyi_prev + cyi;
              if (zx * zx + zyi * zyi > 4) {
                draw = false;
                break;
              }
            }

            if (draw) {
              image.plot(pixel_x, pixel_y, 0.0, 0.0, 0.0);
            }
          }
        }
      });
    }
  }
  
  while (pool.busy()) {
    sleep(0.5);
  }
  pool.stop();

  image.close();
  std::cout << "Done!" << std::endl;
  return 0;
}