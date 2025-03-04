#pragma once

#include "visit_member.h"

#include <cassert>
#include <functional>
#include <istream>
#include <map>
#include <ostream>
#include <type_traits>
#include <typeinfo>
#include <utility>

inline std::map<std::string, std::function<void*()>> factory_;

template <typename T>
void registerFactory()
{
  std::string name = typeid(T).name();
  factory_[name] = []() { return new T; };
}

class OutputArchive
{
public:
  OutputArchive(std::ostream& os)
    : s_(os)
  {}

  template <typename T>
    requires std::is_arithmetic_v<T>
  OutputArchive& serialize_primitive(const T& v)
  {
    // s_.write(reinterpret_cast<const char*>(&v), sizeof(v));
    s_ << v << ' ';
    return *this;
  }

  template <typename T>
  OutputArchive& serialize_pointer(const T* const& p)
  {
    if (p == nullptr) {
      int null = -1;
      // s_.write(reinterpret_cast<const char*>(&null), sizeof(null));
      s_ << -1 << ' ';
    }
    else {
      std::string name = typeid(*p).name();
      if (ptr_map_.find(p) == ptr_map_.end()) {
        ptr_map_[p] = ptr_map_.size();
      }
      s_ << ptr_map_[p] << ' ';
      this->operator()(name);
      this->operator()(*p);
    }
    return *this;
  }

  OutputArchive& serialize_string(const std::string& v)
  {
    // write size first, then the string
    int len = v.size();
    // s_.write(reinterpret_cast<const char*>(&len), sizeof(len));
    s_ << len << ' ';
    s_.write(v.data(), len);
    s_ << ' ';
    return *this;
  }

  template <typename T>
    requires std::is_class_v<std::remove_cvref_t<T>>
  OutputArchive& serialize_class(T&& v)
  {
    visit_members(v, [&](auto i) { this->operator()(i); });
    return *this;
  }

  /// @brief Dispatch to different serialize function based on the type of v
  /// @param v
  /// @return
  /// Use the following priority to find the correct serialize function:
  /// 1. If v.serialize(OutputArchive&) exists, call it
  /// 2. If serialize(v) exists, call it
  /// 3. If v is a pointer, call serialize_pointer(const T* const&)
  /// 4. If v is a string, call serialize_string(const std::string&)
  /// 5. If v is a class, call serialize_class(T&&)
  /// 6. If v is a primitive, call serialize_primitive(const T&)
  template <typename T>
  OutputArchive& dispatch(T&& v)
  {
    // if constexpr (requires { const_cast<std::remove_cv_t<T>>(v).serialize(*this); }) {
    //   const_cast<std::remove_cv_t<T>>(v).serialize(*this);
    // }
    if constexpr (requires { v.serialize(*this); }) {
      v.serialize(*this);
    }
    else if constexpr (requires { serialize(v); }) {
      serialize(v);
    }
    else if constexpr (std::is_pointer_v<std::decay_t<T>>) {
      serialize_pointer(v);
    }
    else if constexpr (std::is_same_v<std::remove_cvref_t<T>, std::string>) {
      serialize_string(v);
    }
    else if constexpr (std::is_class_v<std::remove_cvref_t<T>>) {
      serialize_class(std::forward<T>(v));
    }
    else if constexpr (std::is_arithmetic_v<std::remove_cvref_t<T>>) {
      serialize_primitive(v);
    }
    else {
      static_assert(std::false_type::value, "No matching serialization function found");
    }
    return *this;
  }

  template <typename T, typename... Args>
  OutputArchive& operator()(T&& v, Args&&... args)
  {
    if constexpr (sizeof...(args) == 0) {
      return this->dispatch(v);
    }
    else {
      this->dispatch(v);
      return this->operator()(std::forward<Args>(args)...);
    }
  }

private:
  // target stream
  std::ostream& s_;
  // map of pointers to their index
  std::map<const void*, int> ptr_map_;
  // map of type and their ctor
};

class InputArchive
{
public:
  InputArchive(std::istream& is)
    : s_(is)
  {}

  template <typename T>
    requires std::is_arithmetic_v<T>
  InputArchive& deserialize_primitive(T& v)
  {
    // s_.read(reinterpret_cast<char*>(&v), sizeof(v));
    s_ >> v;
    char c;
    s_.get(c);
    return *this;
  }

  InputArchive& deserialize_string(std::string& v)
  {
    // read size first, then the string
    int len;
    // s_.read(reinterpret_cast<char*>(&len), sizeof(len));
    s_ >> len;
    char c;
    s_.get(c);
    v.resize(len);
    s_.read(v.data(), len);
    s_.get(c);
    return *this;
  }

  template <typename T>
  InputArchive& deserialize_pointer(T*& p)
  {
    int ptr_id;
    this->operator()(ptr_id);
    if (ptr_id == -1) {
      p = nullptr;
      return *this;
    }
    std::string name;
    this->operator()(name);
    if (ptr_id_to_obj_.size() == ptr_id) {
      p = static_cast<T*>(factory_[name]());
      ptr_id_to_obj_.push_back(p);
      this->operator()(*p);
    }
    else {
      p = static_cast<T*>(ptr_id_to_obj_[ptr_id]);
    }
    return *this;
  }

  template <typename T>
    requires std::is_class_v<std::remove_cvref_t<T>>
  InputArchive& deserialize_class(T&& v)
  {
    visit_members(std::forward<T>(v), [&](auto& i) { this->operator()(i); });
    return *this;
  }

  /// @brief Dispatch to different serialize function based on the type of v
  /// @param v
  /// @return
  /// Use the following priority to find the correct serialize function:
  /// 1. If v.serialize(InputArchive&) exists, call it
  /// 2. If serialize(v) exists, call it
  /// 3. If v is a pointer, call serialize_pointer(const T* const&)
  /// 4. If v is a string, call serialize_string(const std::string&)
  /// 5. If v is a class, call serialize_class(T&&)
  /// 6. If v is a primitive, call serialize_primitive(const T&)
  template <typename T>
  InputArchive& dispatch(T&& v)
  {
    if constexpr (requires { std::forward<T>(v).serialize(*this); }) {
      std::forward<T>(v).serialize(*this);
    }
    else if constexpr (requires { serialize(std::forward<T>(v)); }) {
      serialize(std::forward<T>(v));
    }
    else if constexpr (std::is_pointer_v<std::decay_t<T>>) {
      deserialize_pointer(std::forward<T>(v));
    }
    else if constexpr (std::is_same_v<std::remove_cvref_t<T>, std::string>) {
      deserialize_string(std::forward<T>(v));
    }
    else if constexpr (std::is_class_v<std::remove_cvref_t<T>>) {
      deserialize_class(std::forward<T>(v));
    }
    else if constexpr (std::is_arithmetic_v<std::remove_cvref_t<T>>) {
      deserialize_primitive(std::forward<T>(v));
    }
    else {
      static_assert(std::false_type::value, "No matching deserialization function found");
    }
    return *this;
  }

  template <typename T, typename... Args>
  InputArchive& operator()(T&& v, Args&&... args)
  {
    if constexpr (sizeof...(args) == 0) {
      return this->dispatch(std::forward<T>(v));
    }
    else {
      this->dispatch(std::forward<T>(v));
      return this->operator()(std::forward<Args>(args)...);
    }
  }

private:
  // target stream
  std::istream& s_;
  // map of index to their pointer
  std::vector<void*> ptr_id_to_obj_;
};
