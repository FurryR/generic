#ifndef _GENERIC_TYPE_ARRAY_
#define _GENERIC_TYPE_ARRAY_
/**
 * Generic 基于 MIT 协议开源。
 * Copyright(c) 凌/纳延 2023.
 */
#include <type_traits>

#include "void_t.hpp"
namespace generic {
template <typename...>
struct type_array;
namespace detail {
template <std::size_t I, typename... Args>
struct _at_impl;
template <std::size_t I, typename T, typename... Rest>
struct _at_impl<I, T, Rest...> : _at_impl<I - 1, Rest...> {};
template <typename T, typename... Rest>
struct _at_impl<0, T, Rest...> {
  using type = T;
};
template <typename...>
struct _concat_impl;
template <typename... Args, typename... NewArgs, typename... Rest>
struct _concat_impl<type_array<Args...>, type_array<NewArgs...>, Rest...>
    : _concat_impl<type_array<Args..., NewArgs...>, Rest...> {};
template <typename... Args>
struct _concat_impl<type_array<Args...>> {
  using type = type_array<Args...>;
};
template <typename, std::size_t, std::size_t, typename...>
struct _slice_impl;
template <typename... PushBack, std::size_t Current, std::size_t Count,
          typename T, typename... Args>
struct _slice_impl<type_array<PushBack...>, Current, Count, T, Args...>
    : std::conditional<
          (Current == 0),
          typename std::conditional<(Count == 0),
                                    _slice_impl<type_array<PushBack...>, 0, 0>,
                                    _slice_impl<type_array<PushBack..., T>, 0,
                                                Count - 1, Args...>>::type,
          _slice_impl<type_array<>, Current - 1, Count, Args...>>::type {};
template <typename... PushBack, std::size_t Current, std::size_t Count>
struct _slice_impl<type_array<PushBack...>, Current, Count> {
  using type = type_array<PushBack...>;
};
};  // namespace detail
template <template <typename> class, typename = void>
struct transform : std::false_type {};
template <template <typename> class T>
struct transform<T, void_t<typename T<void>::type>> : std::true_type {};
template <template <typename> class, typename = void>
struct concept : std::false_type{};
template <template <typename> class T>
struct concept<T, typename std::enable_if<std::is_same<decltype(T<void>::value),
                                                       bool>::value>::type>
    : std::true_type{};
template <typename... Args>
struct type_array {
  /**
   * @brief type_array 的长度。
   */
  static constexpr std::size_t size = sizeof...(Args);
  /**
   * @brief type_array 的第 I 个类型。
   *
   * @tparam I 下标。从 0 开始。
   */
  template <std::size_t I>
  using at = typename detail::_at_impl<I, Args...>::type;
  /**
   * @brief 将类型追加到 type_array 内。
   *
   * @tparam NewArgs
   */
  template <typename... NewArgs>
  using push = type_array<Args..., NewArgs...>;
  /**
   * @brief 将 type_array 中的成员切分并返回新成员。
   * 
   * @tparam Start 开始的下标。
   * @tparam Count 切分的个数。
   */
  template <std::size_t Start, std::size_t Count = (std::size_t)-1>
  using slice =
      typename detail::_slice_impl<type_array<>, Start, Count, Args...>::type;
  /**
   * @brief 将 type_array 持有的类型列表应用到 T。
   *
   * @tparam T 被应用的类。应当接受一个可变模板参数。
   */
  template <template <typename...> class T>
  using apply = T<Args...>;
};
template <typename... Args>
using slice = type_array<int, void, char, float>::slice<1, 1>;
}  // namespace generic
#endif