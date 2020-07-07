# Entity Component System

This is a playground for implementing an entity component system in C++. The API is inspired by [entt](https://github.com/skypjack/entt).

Currently, only this functionality is implemented:
```cpp
ecs::registry r;
ecs::entity e = r.create_entity();
r.emplace<some_struct>(e, args...);
/* etc */
r.for_each<some_struct>([](some_struct& data) { /* functionality */ });
```
So only iterating over one component is implemented right now. Also there is no way to delete an entity. I want to implement these in the future.

# Building

To build the library you need [conan](https://conan.io/)(to fetch dependencies) and cmake(to actually build it):
```sh
mkdir build && cd build
conan install ..
cmake ..
# cmake -DBUILD_TESTS=ON .. # to build the tests as well
cmake --build .
```