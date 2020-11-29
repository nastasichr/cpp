#pragma once

namespace meta {

namespace details {

template<typename T>
struct make_unique_id {
    static constexpr void identity(){};
};

}

template<typename T>
constexpr auto make_unique_id = (&details::make_unique_id<T>::identity);

}
