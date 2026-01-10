
#include "camel/camel.h"
#include <string>

int main() {

  [[maybe_unused]] std::array<int, 10> elems{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  [[maybe_unused]] std::string e{OBJ_TO_STR_CONSTEXPR(elems)};
  [[maybe_unused]] std::string f{OBJ_TO_STR_RUNTIME(elems)};

  return 0;
}
