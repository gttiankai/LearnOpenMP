
#include <omp.h>
#include <iostream>

static long num_steps = 100000000;

#define NUM_THREADS 4

void naive_pi() {
  double start_time, run_time;
  double pi = 0.0f;
  double step = 1.0 / (double)num_steps;
  // omp_set_num_threads(NUM_THREADS);
  start_time = omp_get_wtime();
  double sum = 0.0f;
  for (int i = 0; i < num_steps; ++i) {
    double x = (i + 0.5) * step;
    sum = sum + 4.0 / (1.0 + x * x);
  }
  pi += step * sum;

  run_time = (omp_get_wtime() - start_time);
  printf("========Naive PI==========\n");
  printf("the π value: %f\ncost time: %f s\n", pi, run_time);
}

/**
 * 1 thread cost time: 65.83 ms
 * 2 thread cost time: 107.78 ms
 * 3 thread cost time: 112.49 ms
 * 4 thread cost time: 41.56 ms
 *
 * 1 thread(with pad) cost time: 39.05 ms
 * 2 thread(with pad) cost time: 68.77 ms
 * 3 thread(with pad) cost time: 44.79 ms
 * 4 thread(with pad) cost time: 42.25 ms
 *omp_get_wtime
 * */

void omp_pi() {
  double sum[NUM_THREADS];
  double start_time, run_time;
  double pi = 0.0f;
  double step = 1.0 / (double)num_steps;
  omp_set_num_threads(NUM_THREADS);
  start_time = omp_get_wtime();
  int threads_number = 0;
#pragma omp parallel
  {
    int i;
    double x;
    int id = omp_get_thread_num();
    int n_threads = omp_get_num_threads();
    if (id == 0) {
      // 有可能你申请到的线程数,并不是你设置的最大的线程数
      threads_number = n_threads;
    }
    for (i = id, sum[id] = 0.0; i < num_steps; i += n_threads) {
      x = (i + 0.5) * step;
      sum[id] += 4.0 / (1.0 + x * x);
    }
  }
  for (int i = 0; i < threads_number; ++i) {
    pi += sum[i];
  }
  pi = pi * step;
  run_time = (omp_get_wtime() - start_time);
  printf("========OMP PI==========\n");
  printf("the π value: %f\nOpenMP cost time: %f s\n", pi, run_time);
}

void omp_pi_without_false_sharing() {
  double sum = 0.0f;
  double start_time, run_time;
  double pi = 0.0f;
  double step = 1.0 / (double)num_steps;
  omp_set_num_threads(NUM_THREADS);
  start_time = omp_get_wtime();
  int threads_number = 0;
#pragma omp parallel
  {
    int i;
    double x;
    int id = omp_get_thread_num();
    int n_thrds = omp_get_num_threads();
    for (i = id; i < num_steps; i += n_thrds) {
      x = (i + 0.5) * step;
      double value = 4.0 / (1.0 + x * x);
#pragma omp critical  //  这个地方产生了竞争条件,所以性能很慢
      sum += value;
    }
  }
  pi = sum * step;
  run_time = (omp_get_wtime() - start_time);
  printf("========OMP PI==========\n");
  printf("the π value: %f\nOpenMP(without false sharing) cost time: %f s\n", pi, run_time);
}

void omp_pi_critical() {
  double start_time, run_time;
  double pi = 0.0f;
  double step = 1.0 / (double)num_steps;
  omp_set_num_threads(NUM_THREADS);
  start_time = omp_get_wtime();
#pragma omp parallel
  {
    int i;
    double sum = 0.0f;
    double x;
    int id = omp_get_thread_num();
    int n_thrds = omp_get_num_threads();
    for (i = id; i < num_steps; i += n_thrds) {
      x = (i + 0.5) * step;
      sum += 4.0 / (1.0 + x * x);
    }
#pragma omp critical
    pi += sum * step;
  }
  run_time = (omp_get_wtime() - start_time);
  printf("========OMP Critical PI==========\n");
  printf("the π value: %f\nOpenMP(Critical) cost time: %f s\n", pi, run_time);
}

void omp_for_pi() {
  double start_time, run_time;
  double pi = 0.0f;
  double step = 1.0 / (double)num_steps;
  omp_set_num_threads(NUM_THREADS);
  start_time = omp_get_wtime();
#pragma omp parallel
  {
    double sum = 0.0f;
#pragma omp for
    for (int i = 0; i < num_steps; ++i) {
      double x = (i + 0.5) * step;
      sum = sum + 4.0 / (1.0 + x * x);
    }
#pragma omp critical
    pi += step * sum;
  };

  run_time = (omp_get_wtime() - start_time);
  printf("========OMP for PI==========\n");
  printf("the π value: %f\ncost time: %f s\n", pi, run_time);
}

void omp_loop() {
  double start_time, run_time;
  double pi = 0.0f;
  double step = 1.0 / (double)num_steps;
  double sum = 0.0f;
  start_time = omp_get_wtime();
#pragma omp parallel
  {
#pragma omp for reduction(+ : sum)
    for (int i = 0; i < num_steps; ++i) {
      double x = (i + 0.5) * step;
      sum = sum + 4.0 / (1.0 + x * x);
    }
  }
  pi += step * sum;

    run_time = (omp_get_wtime() - start_time);
    printf("========PI omp with loop ==========\n");
    printf("the π value: %f\ncost time: %f s\n", pi, run_time);

#pragma omp parallel for reduction(+ : sum)
  for (int i = 0; i < num_steps; ++i) {
    double x = (i + 0.5) * step;
    sum = sum + 4.0 / (1.0 + x * x);
  }

  pi += step * sum;

  run_time = (omp_get_wtime() - start_time);
  printf("========PI omp with reduction ==========\n");
  printf("the π value: %f\ncost time: %f s\n", pi, run_time);
}

int main() {
  naive_pi();
  omp_pi();
  // omp_pi_without_false_sharing();
  omp_pi_critical();
  omp_for_pi();
  omp_loop();
}