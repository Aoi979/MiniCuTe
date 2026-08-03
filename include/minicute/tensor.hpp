#pragma once

#include "layout.hpp"

#include <array>
#include <cstddef>

namespace minicute {

template <class Iterator>
struct ViewEngine {
  using iterator = Iterator;
  using reference = decltype(*std::declval<iterator>());
  using element_type = remove_cvref_t<reference>;
  using value_type = std::remove_cv_t<element_type>;

  constexpr explicit ViewEngine(iterator ptr = {}) : storage_(ptr) {}
  constexpr iterator begin() const { return storage_; }
  constexpr iterator begin() { return storage_; }

  iterator storage_;
};

template <class T, std::size_t N>
struct ArrayEngine {
  using iterator = T*;
  using reference = T&;
  using element_type = T;
  using value_type = T;

  constexpr iterator begin() { return storage_.data(); }
  constexpr T const* begin() const { return storage_.data(); }

  std::array<T, N> storage_{};
};

namespace detail {
template <class T>
struct contains_underscore
    : std::bool_constant<is_underscore_v<remove_cvref_t<T>>> {};

template <class... Ts>
struct contains_underscore<tuple<Ts...>>
    : std::bool_constant<(contains_underscore<Ts>::value || ...)> {};

template <class T>
inline constexpr bool contains_underscore_v =
    contains_underscore<remove_cvref_t<T>>::value;
}  // namespace detail

template <class Engine, class Layout_>
struct Tensor {
  using engine_type = Engine;
  using layout_type = Layout_;
  using value_type = typename Engine::value_type;

  constexpr Tensor(Engine const& engine = {}, Layout_ const& layout = {})
      : engine_(engine), layout_(layout) {}

  constexpr decltype(auto) engine() { return (engine_); }
  constexpr decltype(auto) engine() const { return (engine_); }
  constexpr decltype(auto) layout() { return (layout_); }
  constexpr decltype(auto) layout() const { return (layout_); }
  constexpr decltype(auto) data() { return engine_.begin(); }
  constexpr decltype(auto) data() const { return engine_.begin(); }
  constexpr decltype(auto) shape() const { return layout_.shape(); }

  template <class Coord>
  constexpr decltype(auto) operator()(Coord const& coord) {
    if constexpr (detail::contains_underscore_v<Coord>) {
      auto [sub_layout, offset] = slice_and_offset(coord, layout_);
      auto ptr = data() + offset;
      return Tensor<ViewEngine<decltype(ptr)>, remove_cvref_t<decltype(sub_layout)>>{
          ViewEngine<decltype(ptr)>{ptr}, sub_layout};
    } else {
      return data()[layout_(coord)];
    }
  }

  template <class Coord>
  constexpr decltype(auto) operator()(Coord const& coord) const {
    if constexpr (detail::contains_underscore_v<Coord>) {
      auto [sub_layout, offset] = slice_and_offset(coord, layout_);
      auto ptr = data() + offset;
      return Tensor<ViewEngine<decltype(ptr)>, remove_cvref_t<decltype(sub_layout)>>{
          ViewEngine<decltype(ptr)>{ptr}, sub_layout};
    } else {
      return data()[layout_(coord)];
    }
  }

  template <class Coord0, class Coord1, class... Coords>
  constexpr decltype(auto) operator()(Coord0 const& c0, Coord1 const& c1,
                                      Coords const&... cs) {
    return (*this)(make_coord(c0, c1, cs...));
  }

  template <class Coord0, class Coord1, class... Coords>
  constexpr decltype(auto) operator()(Coord0 const& c0, Coord1 const& c1,
                                      Coords const&... cs) const {
    return (*this)(make_coord(c0, c1, cs...));
  }

  Engine engine_;
  Layout_ layout_;
};

template <class Pointer, class Layout>
  requires std::is_pointer_v<remove_cvref_t<Pointer>>
constexpr auto make_tensor(Pointer ptr, Layout const& layout)
    -> Tensor<ViewEngine<Pointer>, remove_cvref_t<Layout>> {
  return {ViewEngine<Pointer>{ptr}, layout};
}

template <class T, std::size_t N, class Layout>
constexpr auto make_tensor(std::array<T, N>& storage, Layout const& layout) {
  return make_tensor(storage.data(), layout);
}

template <class T, std::size_t N, class Layout>
constexpr auto make_tensor(std::array<T, N> const& storage, Layout const& layout) {
  return make_tensor(storage.data(), layout);
}

template <class Engine, class Layout>
constexpr decltype(auto) shape(Tensor<Engine, Layout> const& tensor) {
  return tensor.shape();
}

template <class Engine, class Layout>
constexpr decltype(auto) layout(Tensor<Engine, Layout> const& tensor) {
  return tensor.layout();
}

template <class Engine, class Layout>
constexpr decltype(auto) data(Tensor<Engine, Layout>& tensor) {
  return tensor.data();
}

template <class T, class Layout>
  requires is_static_integral_v<decltype(cosize(std::declval<Layout const&>()))>
constexpr auto make_array_tensor(Layout const& layout) {
  constexpr auto n = decltype(cosize(std::declval<Layout const&>()))::value;
  return Tensor<ArrayEngine<T, static_cast<std::size_t>(n)>,
                remove_cvref_t<Layout>>{
      ArrayEngine<T, static_cast<std::size_t>(n)>{}, layout};
}

template <class Engine, class Layout, class F>
void for_each(Tensor<Engine, Layout>& tensor, F&& f) {
  auto n = size(tensor.layout());
  for (int i = 0; i < static_cast<int>(n); ++i) {
    auto coord = idx2crd(i, tensor.shape());
    f(coord, tensor(coord));
  }
}

template <class Engine, class Layout, class Value>
void fill(Tensor<Engine, Layout>& tensor, Value const& value) {
  for_each(tensor, [&](auto const&, auto& element) { element = value; });
}

template <class Engine, class Layout>
void clear(Tensor<Engine, Layout>& tensor) {
  fill(tensor, typename Engine::value_type{});
}

template <class SrcEngine, class SrcLayout, class DstEngine, class DstLayout>
void copy(Tensor<SrcEngine, SrcLayout> const& src,
          Tensor<DstEngine, DstLayout>& dst) {
  for_each(dst, [&](auto const& coord, auto& element) { element = src(coord); });
}

}  // namespace minicute
