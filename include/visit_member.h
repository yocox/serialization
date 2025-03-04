#pragma once

#include <type_traits>

struct Any
{
  template <typename T>
  operator T();
};

template <typename T, typename... Args>
consteval auto member_count()
{
  static_assert(std::is_class_v<T>, "T must be a class type");
  static_assert(sizeof...(Args) < 5, "Only support up to 4 members");
  if constexpr (requires { T{{Args{}}..., {Any{}}}; } == false) {
    return sizeof...(Args);
  }
  else {
    return member_count<T, Args..., Any>();
  }
}

constexpr decltype(auto) visit_members(auto&& object, auto&& visitor)
{
  using type = std::remove_cvref_t<decltype(object)>;
  constexpr auto Count = member_count<type>();
  if constexpr (Count == 0) {
    return;
  }
  else if constexpr (Count == 1) {
    auto&& [a1] = object;
    visitor(a1);
    return;
  }
  else if constexpr (Count == 2) {
    auto&& [a1, a2] = object;
    visitor(a1, a2);
    return;
  }
  else if constexpr (Count == 3) {
    auto&& [a1, a2, a3] = object;
    visitor(a1, a2, a3);
    return;
  }
  else if constexpr (Count == 4) {
    auto&& [a1, a2, a3, a4] = object;
    visitor(a1, a2, a3, a4);
    return;
  }
};