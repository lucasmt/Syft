#ifndef OPTIONAL_H
#define OPTIONAL_H

#include <experimental/optional>

namespace my {

template <class T>
using optional = std::experimental::optional<T>;

using nullopt_t = std::experimental::nullopt_t;

nullopt_t nullopt = std::experimental::nullopt;
}

#endif // OPTIONAL_H
