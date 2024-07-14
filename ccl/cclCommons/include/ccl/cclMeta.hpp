#pragma once
// Metaprogramming tools

#include <algorithm>
#include <iterator>
#include <functional>
#include <memory>
#include <type_traits>
#include <cstddef>

namespace ccl::meta {

//! Basic identity class for template overloading/specializations
template<typename T>
struct Identity { using type = T; };

//! Naive propagate_const implementation aimed at unique_ptr
template<typename T>
class PropagateConst {
  // TODO: C++20 constexpr ctors
  T pointer{ nullptr };

public:
  ~PropagateConst() noexcept = default;
  PropagateConst() = default;

  PropagateConst(const PropagateConst&) = delete;
  PropagateConst& operator=(const PropagateConst&) = delete;
  PropagateConst(PropagateConst&&) noexcept = default;
  PropagateConst& operator=(PropagateConst&&) noexcept = default;

  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  PropagateConst(T&& val) noexcept 
    : pointer{ std::move(val) } {}

  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  PropagateConst(std::nullptr_t) noexcept {}

  template<typename Up,
    typename std::enable_if_t<std::is_constructible_v<T, Up&&>>>
  PropagateConst(Up&& up) // NOLINT(google-explicit-constructor, hicpp-explicit-conversions, bugprone-forwarding-reference-overload)
    : pointer{ std::forward<Up>(up) } {}

  template<typename Up,
    typename std::enable_if_t<std::is_constructible_v<T, Up&&>>>
  PropagateConst(PropagateConst<Up>&& up) // NOLINT(google-explicit-constructor, hicpp-explicit-conversions)
    : pointer{ std::move(up).pointer } {}

  template<typename Up,
    typename std::enable_if_t<std::is_constructible_v<T, Up&&>>>
  constexpr PropagateConst& operator=(Up&& up) {
    pointer = std::forward<Up>(up);
    return *this;
  }

  template<typename Up,
    typename std::enable_if_t<std::is_constructible_v<T, Up&&>>>
  constexpr PropagateConst& operator=(PropagateConst<Up>&& up) {
    pointer = std::move(up).pointer;
    return *this;
  }

public:
  using ElementType = std::remove_reference_t<decltype(*std::declval<T>())>;
  constexpr ElementType* get() {
    return pointer.get();
  }

  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  constexpr operator ElementType*() {
    return get();
  }
  constexpr ElementType& operator*() {
    return *get();
  }
  constexpr ElementType* operator->() {
    return get();
  }

  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  constexpr operator const ElementType*() const { 
    return get();
  }
  [[nodiscard]] constexpr const ElementType* get() const {
    return pointer.get();
  }
  constexpr const ElementType& operator*() const {
    return *get();
  }
  constexpr const ElementType* operator->() const {
    return get();
  }

  constexpr void swap(PropagateConst<T>& second) noexcept {
    std::swap(pointer, second.pointer);
  }

  constexpr bool operator==(const PropagateConst& rhs) const noexcept {
    return pointer == rhs.pointer;
  }
  constexpr bool operator!=(const PropagateConst& rhs) const noexcept {
    return pointer != rhs.pointer;
  }
  constexpr bool operator==(std::nullptr_t) const noexcept {
    return pointer == nullptr;
  }
  constexpr bool operator!=(std::nullptr_t) const noexcept {
    return pointer != nullptr;
  }
};

//! Unique pointer propagating const wrapper alias
template<typename T>
using UniqueCPPtr = PropagateConst<std::unique_ptr<T>>;

//! Grouping multiple overloaded lambas
/*
  see https://www.youtube.com/watch?v=XjOVebhUOY around 20:30
*/
template <typename ...Ops>
struct Overloads : Ops... {
  using Ops::operator()...;
};
template <typename ...Ops>
Overloads(Ops...)->Overloads<Ops...>;

//! Apply function object to tuples created from elements container
template <typename Container,
  typename Function,
  typename Value = typename Container::value_type>
  void ForEachPair(const Container& input, const Function& Fn) {
  std::for_each(begin(input), end(input), [&input, &Fn](const Value& el1) {
    std::for_each(begin(input), end(input), [&el1, &Fn](const Value& el2) {
      Fn(el1, el2);
    });
  });
}

//! CRTP implementation
template <typename Base, template<typename> class crtpType>
struct crtp {
  [[nodiscard]] Base& BaseT() noexcept { return static_cast<Base&>(*this); }
  [[nodiscard]] const Base& BaseT() const noexcept { return static_cast<Base const&>(*this); }
};

//! Abstract strong type
/* 
  see https://github.com/joboccara/NamedType
*/
template <typename T>
struct Incrementable : crtp<T, Incrementable> {
  T& operator+=(const T& other) { this->BaseT().get() += other.get(); return this->BaseT(); }
};

template <typename T>
struct PreIncrementable : crtp<T, PreIncrementable> {
  T& operator++() { ++this->BaseT().get(); return this->BaseT(); }
};

template <typename T>
struct Addable : crtp<T, Addable> {
  T operator+(const T& other) const { return T(this->BaseT().get() + other.get()); }
};

template <typename T>
struct Subtractable : crtp<T, Subtractable> {
  T operator-(const T& other) const { return T(this->BaseT().get() - other.get()); }
};

template <typename T>
struct Multiplicable : crtp<T, Multiplicable> {
  T operator*(const T& other) const { return T(this->BaseT().get() * other.get()); }
};

template <typename T>
struct Comparable : crtp<T, Comparable> {
  bool operator==(const T& other) const noexcept { return this == &other || other.get() == this->BaseT().get(); }
  bool operator!=(const T& other) const noexcept { return !(*this == other); }
};

template <typename T>
struct Orderable : crtp<T, Orderable> {
  bool operator<(const T& other) const noexcept { return this->BaseT().get() < other.get(); }
  bool operator>(const T& other) const noexcept { return other.get() < this->BaseT().get(); }
  bool operator<=(const T& other) const noexcept { return !(other.get() < this->BaseT().get()); }
  bool operator>=(const T& other) const noexcept { return !(*this < other); }
  bool operator==(const T& other) const noexcept { return !(*this < other) && !(other.get() < this->BaseT().get()); }
  bool operator!=(const T& other) const noexcept { return !(*this == other); }
};

#ifdef _MSC_VER
  #pragma warning( push )
  #pragma warning( disable : 26482 26446) // Note: ignore bounds warning when trying to address massive of static values
#endif

//! Iterable enumeration
template <typename T, T... args>
class EnumIter {
  size_t pos{ 0 };
  
  static constexpr T values[] = { args... }; // NOLINT: static C array
  static constexpr size_t enumSize = sizeof...(args);

public:
  EnumIter() = default;
  explicit EnumIter(T val) 
    : pos(static_cast<size_t>(std::distance(&values[0], find(&values[0], &values[enumSize], val)))) {}

public:
  // stl compatibility
  using iterator_category = std::input_iterator_tag;
  using value_type = T;
  using difference_type = std::ptrdiff_t;
  using pointer = const T*;
  using reference = const T&;

  EnumIter end() noexcept { EnumIter result{}; result.pos = enumSize; return result; }
  EnumIter begin() noexcept { return EnumIter{}; }
  static constexpr size_t size() { return enumSize; }

  const T& operator*() const noexcept { return values[pos]; }
  EnumIter& operator--() noexcept { --pos; return *this; }
  EnumIter& operator++() noexcept { ++pos; return *this; }

  bool operator==(const EnumIter& rhs) const noexcept { return pos == rhs.pos; }
  bool operator!=(const EnumIter& rhs) const noexcept { return pos != rhs.pos; }
};

#ifdef _MSC_VER
  #pragma warning( pop )
#endif

//! Abstract mutating facet
/*
  Note: facets are not movable and not copyable because of Core&.
  Can use std::reference_wrapper instead if needed
*/
template<typename Core>
struct MutatorFacet {
  Core& core;

protected:
  ~MutatorFacet() noexcept = default;

public:
  MutatorFacet(const MutatorFacet&) = delete;
  MutatorFacet& operator=(const MutatorFacet&) = delete;

  explicit MutatorFacet(Core& core) noexcept
    : core{ core } {}
};

//! Abstract constant facet
template<typename Core>
struct ConstFacet {
  const Core& core;

protected:
  ~ConstFacet() noexcept = default;

public:
  ConstFacet(const ConstFacet&) = delete;
  ConstFacet& operator=(const ConstFacet&) = delete;

  explicit ConstFacet(const Core& core) noexcept :
    core{ core } {}
};

//! Abstract polymorphic iterator
template<typename Value>
class PolyFCIterator {
  struct ImplConcept;
  template<typename Iter>  struct ImplModel;

  std::unique_ptr<ImplConcept> impl{ nullptr };

public:
  ~PolyFCIterator() noexcept = default;
  PolyFCIterator(const PolyFCIterator<Value>& rhs)
    : impl{ rhs.impl->Clone() } {}
  PolyFCIterator& operator=(const PolyFCIterator<Value>& rhs) {
    if (this == &rhs) {
      return *this;
    }
    impl = rhs.impl->Clone();
    return *this;
  }
  PolyFCIterator(PolyFCIterator<Value>&&) noexcept = default;
  PolyFCIterator& operator=(PolyFCIterator<Value>&&) noexcept = default;

  template<typename Iter>
  explicit PolyFCIterator(const Iter& iter)
    : impl{ std::make_unique<ImplModel<Iter>>(iter) } {}

public:
  // stl compatibility
  using iterator_category = std::forward_iterator_tag;
  using value_type = Value;
  using difference_type = std::ptrdiff_t;
  using pointer = const Value*;
  using reference = const Value&;

private:
  struct ImplConcept {
    virtual ~ImplConcept() noexcept = default;
    ImplConcept() = default;
    ImplConcept(const ImplConcept&) = default;
    ImplConcept& operator=(const ImplConcept&) = default;
    ImplConcept(ImplConcept&&) noexcept = default;
    ImplConcept& operator=(ImplConcept&&) noexcept = default;

    virtual void Next()  = 0;
    [[nodiscard]] virtual const value_type& Deref() const = 0;
    [[nodiscard]] virtual bool Equal(const void* other) const = 0;
    [[nodiscard]] virtual std::unique_ptr<ImplConcept> Clone() const = 0;
    [[nodiscard]] virtual const std::type_info& Type() const noexcept = 0;
    [[nodiscard]] virtual const void* Address() const noexcept = 0;
  };

  template<typename Iter>
  struct ImplModel : public ImplConcept {
    Iter iter;

    explicit ImplModel(Iter iter) noexcept
      : iter{ std::move(iter) } {}

    void Next() override {
      std::advance(iter, 1);
    }
    [[nodiscard]] reference Deref() const override {
      return *iter;
    }

#ifdef _MSC_VER
  #pragma warning( push )
  #pragma warning( disable : 26440 ) // Note: false positive noexcept
#endif
    [[nodiscard]] bool Equal(const void* rp) const override {
      return iter == static_cast<const ImplModel*>(rp)->iter;
    }
#ifdef _MSC_VER
  #pragma warning( pop )
#endif

    [[nodiscard]] std::unique_ptr<ImplConcept> Clone() const override {
      return std::make_unique<ImplModel>(*this);
    }
    [[nodiscard]] const std::type_info& Type() const noexcept override {
      return typeid(iter);
    }
    [[nodiscard]] const void* Address() const noexcept override {
      return this;
    }
  };

public:
  reference operator*() const { return impl->Deref();  }
  pointer operator->() const { return &impl->Deref(); }
  PolyFCIterator<Value>& operator++() {
    impl->Next();
    return *this;
  }
  bool operator==(const PolyFCIterator<Value>& rhs) const {
    return impl->Type() == rhs.impl->Type() && impl->Equal(rhs.impl->Address());
  }
  bool operator!=(const PolyFCIterator<Value>& r) const {
    return !(*this == r);
  }
};

} // namespace ccl::meta
