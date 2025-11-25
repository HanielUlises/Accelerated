#ifndef PARALLEL_SUM_H
#define PARALLEL_SUM_H

#include <cstddef>

double parallel_sum(const double* data, size_t size, int num_threads);

#endif