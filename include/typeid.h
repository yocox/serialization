#pragma once

inline int generate_type_id()
{
  static int value = 0;
  return value++;
}

template <class T>
int type_id()
{
  static int value = generate_type_id();
  return value;
}
