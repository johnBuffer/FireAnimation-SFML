#pragma once

namespace pez
{

template <typename TContainer1, typename TContainer2, typename TCallback>
void zip(TContainer1& container1, TContainer2& container2, TCallback&& callback)
{
    assert(container1.size() == container2.size());
    size_t const count{container1.size()};
    for (size_t i{0}; i < count; ++i) {
        callback(container1[i], container2[i]);
    }
}

} // namespace pez