#pragma once

#include "typeid.h"
#include "visit_member.h"

#include <cassert>
#include <functional>
#include <istream>
#include <map>
#include <ostream>
#include <type_traits>
#include <typeinfo>

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
  OutputArchive& serialize(const T& v)
  {
    // s_.write(reinterpret_cast<const char*>(&v), sizeof(v));
    s_ << v << ' ';
    return *this;
  }

  template <typename T>
  OutputArchive& serialize(const T* const& p)
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
      this->serialize(name);
      p->serialize(*this);
    }
    return *this;
  }

  OutputArchive& serialize(const std::string& v)
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
    requires std::is_class_v<std::remove_cvref_t<T>> && (!std::is_same_v<std::remove_cvref_t<T>, std::string>)
  OutputArchive& serialize(T&& v)
  {
    visit_members(v, [&](auto i) { this->serialize(i); });
    return *this;
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
  InputArchive& deserialize(const T& v);

  template <typename T>
    requires std::is_arithmetic_v<T>
  InputArchive& deserialize(T& v)
  {
    // s_.read(reinterpret_cast<char*>(&v), sizeof(v));
    s_ >> v;
    char c;
    s_.get(c);
    return *this;
  }

  InputArchive& deserialize(std::string& v)
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
  InputArchive& deserialize(T*& p)
  {
    int ptr_id;
    this->deserialize(ptr_id);
    if (ptr_id == -1) {
      p = nullptr;
      return *this;
    }
    std::string name;
    this->deserialize(name);
    if (ptr_id_to_obj_.size() == ptr_id) {
      p = static_cast<T*>(factory_[name]());
      ptr_id_to_obj_.push_back(p);
      p->deserialize(*this);
    }
    else {
      p = static_cast<T*>(ptr_id_to_obj_[ptr_id]);
    }
    return *this;
  }

private:
  // target stream
  std::istream& s_;
  // map of index to their pointer
  std::vector<void*> ptr_id_to_obj_;
};
