#include "msg/a/msg_a.h"
#include "msg/b/msg_b.h"

#include "msg/base/base_msg.h"
#include "registry/types.h"
#include "serializer/types.h"

#include <iostream>
#include <memory>

inline B make_test_B() {
  B b;

  // scalar
  b.v = 3.1415926f;

  // map<uint32_t, string>
  b.map_uint_str.emplace(1u, "one");
  b.map_uint_str.emplace(42u, "forty-two");
  b.map_uint_str.emplace(100u, "hundred");

  // vector<int64_t>
  b.vec_int64.push_back(0);
  b.vec_int64.push_back(-1);
  b.vec_int64.push_back(1234567890123LL);
  b.vec_int64.push_back(-9876543210987LL);

  // unordered_map<SomeName, string>
  {
    SomeName k1;
    k1.f = 1.0f;
    k1.d = 2.0;
    k1.b = true;
    k1.arr[0] = 1;
    k1.arr[1] = 2;
    k1.arr[2] = 3;
    b.umap_somename_str.emplace(k1, "first");

    SomeName k2;
    k2.f = -3.5f;
    k2.d = 42.42;
    k2.b = false;
    k2.arr[0] = 7;
    k2.arr[1] = 8;
    k2.arr[2] = 9;
    b.umap_somename_str.emplace(k2, "second");

    SomeName k3;
    k3.f = 0.0f;
    k3.d = -0.125;
    k3.b = true;
    k3.arr[0] = -1;
    k3.arr[1] = 0;
    k3.arr[2] = 1;
    b.umap_somename_str.emplace(k3, "third");
  }

  return b;
}


int main() {
  /*
  {
    SerializeBuffer buf;
    {
      std::unique_ptr<MsgBase> pBase = std::make_unique<A>(23);
      serialize_msg(buf, pBase);
      std::cout << *pBase << "\n";
    }
    size_t off = 0;
    auto msg = deserialize_msg(buf, off);

    std::cout << *msg << "\n";
  }*/
    {
    SerializeBuffer buf;
    {
      std::unique_ptr<MsgBase> pBase = std::make_unique<B>(make_test_B());
      serialize_msg(buf, pBase);
      std::cout << *pBase << "\n";
    }
    size_t off = 0;
    auto msg = deserialize_msg(buf, off);

    std::cout << *msg << "\n";
  }
  return 0;
}