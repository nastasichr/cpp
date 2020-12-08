# Experiments with C++

This is a collection of experiments with C++ exploring topics such as
*metaprogramming*, *non-standard containers*.
Most of the code in here should be good for embedded systems, i.e,
no exception, no RTTI, no dynamic memory, no std/boost containers.

This project is not intended as a library, but it is rather my personal space to have fun with C++.
I think better implementation of the same or similar features can be found around.
However, if you find this useful or interesting and want to get in touch, please do.

The code is currently based on C++17.

Build this project on Gitpod [here](https://www.gitpod.io/#https://github.com/nastasichr/cpp).

## Content
These are some helpers defined in this project that might come in handy:

- `make_unique_id<T>` : Generate a unique ID from a type;
- `type_list<T...>` : List of types with few useful meta-function, including a `for_each` to apply other ones;
- `type_map<...>` : Map metaclass for value-type, type-value and type-type mappings;
- `parse_function<F>` : Extracts return type and argument types, allowing for an (optional) external meta-function to be applied;
- `sized_function<size, R(Args..)>` : alternative to std::function backed by fixed-size static storage;
- `type_string<char...>` : compile-time value-list and string in the form of template non-type parameters, allowing pure compile-time computation;
- `any_of<T...>` : type erasure similar to std::variant to allow easy type-based publish-subscriber patterns;

## Performance

### Compiler explorer at godbolt.org

- [`type_string`](https://godbolt.org/z/7Wrd4d)
- [`dispatch_queue` and `static_dispatch_queue`](https://godbolt.org/z/sseKb8)

---
### Disclaimer
The code in here has been written based on my personal experiences with C++.
It is therefore possible that snippets of the code in here
will be insipired by clever answers from stack overflow or the like thereof.
No copyright or intellectual property would have been infringed, though,
to the best of my knowledge.
