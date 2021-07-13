#include <array>
#include <vector>

#include "test_framework/generic_test.h"
#include "test_framework/test_failure.h"
#include "test_framework/timed_executor.h"
using std::vector;
enum class Color { kRed, kWhite, kBlue };

void DutchFlagPartition(int pivot_index, vector<Color>* A_ptr) {
  // make only one pass
  vector<Color>& A = *A_ptr;
  Color pivot = A[pivot_index];

  int smaller = 0, larger = A.size() - 1;

  for (int i = 0; i < A.size(); ++i) {
    if (A[i] < pivot) {
      std::swap(A[i], A[smaller++]);
    }
  }

  for (int j = A.size() - 1; j >= 0; --j) {
    if (A[j] > pivot) {
      std::swap(A[j], A[larger--]);
    }
  }
  return;
}

void DutchFlagPartitionClassified(int pivot_index, vector<Color> *A_ptr) {

  vector<Color> &A = *A_ptr;
  Color pivot = A[pivot_index];
  int smaller = 0, equal = 0, larger = A.size();
  /*
    bottom [0, smaller - 1]
    middle [smaller, equal - 1]
    unclassified [equal, larger - 1]
    top [larger, size(A) - 1]
  */

  while (equal < larger) {
    if (A[equal] < pivot) {
      std::swap(A[equal++], A[smaller++]);
    } else if (A[equal] == pivot) {
      ++equal;
    } else {
      std::swap(A[equal], A[--larger]);
    }
  }


  return;
}

void DutchFlagPartitionWrapper(TimedExecutor& executor, const vector<int>& A,
                               int pivot_idx) {
  vector<Color> colors;
  colors.resize(A.size());
  std::array<int, 3> count = {0, 0, 0};
  for (size_t i = 0; i < A.size(); i++) {
    count[A[i]]++;
    colors[i] = static_cast<Color>(A[i]);
  }
  Color pivot = colors[pivot_idx];

  executor.Run([&] { DutchFlagPartitionClassified(pivot_idx, &colors); });

  int i = 0;
  while (i < colors.size() && colors[i] < pivot) {
    count[static_cast<int>(colors[i])]--;
    ++i;
  }

  while (i < colors.size() && colors[i] == pivot) {
    count[static_cast<int>(colors[i])]--;
    ++i;
  }

  while (i < colors.size() && colors[i] > pivot) {
    count[static_cast<int>(colors[i])]--;
    ++i;
  }

  if (i != colors.size()) {
    throw TestFailure("Not partitioned after " + std::to_string(i) +
                      "th element");
  } else if (count != std::array<int, 3>{0, 0, 0}) {
    throw TestFailure("Some elements are missing from original array");
  }
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"executor", "A", "pivot_idx"};
  return GenericTestMain(args, "dutch_national_flag.cc",
                         "dutch_national_flag.tsv", &DutchFlagPartitionWrapper,
                         DefaultComparator{}, param_names);
}
