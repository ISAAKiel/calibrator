/**
 * @file parallel.h
 *
 * \brief Minimal parallel for-loop on std::thread.
 */

#ifndef _parallel_h_
#define _parallel_h_

#include <algorithm>
#include <cstddef>
#include <thread>
#include <vector>

/**
 * Calls fn(i) for i in [0, n), distributed over all hardware threads.
 * fn must be safe to call concurrently for different i.
 */
template <typename Fn>
void parallel_for(size_t n, Fn fn) {
  unsigned n_threads = std::thread::hardware_concurrency();
  if (n_threads == 0) n_threads = 1;
  // Thread start-up is not worth it for a handful of items.
  if (n < 2 * (size_t)n_threads) n_threads = n < 2 ? 1 : (unsigned)(n / 2);

  if (n_threads <= 1) {
    for (size_t i = 0; i < n; i++) fn(i);
    return;
  }
  std::vector<std::thread> threads;
  const size_t chunk = (n + n_threads - 1) / n_threads;
  for (unsigned t = 0; t < n_threads; t++) {
    const size_t b = t * chunk, e = std::min(n, b + chunk);
    if (b >= e) break;
    threads.emplace_back([b, e, &fn] { for (size_t i = b; i < e; i++) fn(i); });
  }
  for (auto& th : threads) th.join();
}

#endif
