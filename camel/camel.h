#include <array>
#include <cstddef>
#include <string>
#include <cstring>
#pragma once

template <std::size_t N>
constexpr auto camelConstexpr(const char (&array)[N]) {
  constexpr std::size_t OUT = (N - 1) * 2 + 1; // exclude '\0'
  std::array<char, OUT> out{};

  std::size_t p = 0;
  for (std::size_t i = 0; i < N - 1; ++i) {
    out[p++] = array[i];
    out[p++] = '_';
  }
  out[p] = '\0';
  return out;
}
inline std::string camelRuntime(const char *array) {
  std::string returnString;
  size_t len = std::strlen(array);
  returnString.reserve(len * 2);

  for (const char *p = array; *p != '\0'; ++p) {
    returnString.push_back(*p);
    returnString.push_back('_');
  }

  return returnString;
}
#define OBJ_TO_STR_CONSTEXPR(__OBJ) camelConstexpr(#__OBJ).data()
#define OBJ_TO_STR_RUNTIME(__OBJ) camelRuntime(#__OBJ).c_str()