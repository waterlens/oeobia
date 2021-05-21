#include <concepts>
#include <type_traits>

using namespace std;
// aux
struct U {};
struct V {};
struct W {};
struct X {};
struct Y {};
struct Z {};

// nat
struct O {};
template <typename N> struct S {};

// prev
template <typename N> struct prev {};
template <typename N> struct prev<S<N>> { using result = N; };

// bool
struct True {};
struct False {};

// ind_nat_to_lit
template <typename N> struct ind_nat_to_lit {
  enum { value = 1 + ind_nat_to_lit<typename prev<N>::result>::value };
};

template <> struct ind_nat_to_lit<O> {
  enum { value = 0 };
};

// eqn
template <typename N1, typename N2> struct eqn {
  using result = typename eqn<typename prev<N1>::result,
                              typename prev<N2>::result>::result;
};
template <> struct eqn<O, O> { using result = True; };
template <typename N> struct eqn<O, S<N>> { using result = False; };
template <typename N> struct eqn<S<N>, O> { using result = False; };

// list
struct nil {};
template <typename X, typename L> struct cons {};

// length
template <typename L> struct length {};
template <> struct length<nil> { using result = O; };
template <typename X, typename L> struct length<cons<X, L>> {
  using result = S<typename length<L>::result>;
};

// append
template <typename L1, typename L2> struct app {};
template <typename L> struct app<nil, L> { using result = L; };
template <typename H, typename T, typename L> struct app<cons<H, T>, L> {
  using result = cons<H, typename app<T, L>::result>;
};

// option
struct None {};
template <typename X> struct Some {};

// partial map
struct empty {};
template <typename K, typename V, typename M> struct record {};

// update
template <typename K, typename V, typename M> struct update {
  using result = record<K, V, M>;
};

// find
template <typename K, typename M> struct find {};
template <typename K> struct find<K, empty> { using result = None; };
template <typename K, typename V, typename M> struct find<K, record<K, V, M>> {
  using result = Some<V>;
};
template <typename K1, typename K2, typename V, typename M>
struct find<K1, record<K2, V, M>> {
  using result = typename find<K1, M>::result;
};

int main() {
  static_assert(ind_nat_to_lit<S<S<O>>>::value == 2, "");
  static_assert(is_same_v<eqn<S<S<S<O>>>, S<O>>::result, False>, "");
  static_assert(is_same_v<eqn<S<S<S<O>>>, S<S<S<O>>>>::result, True>, "");
  static_assert(
      ind_nat_to_lit<length<cons<U, cons<U, nil>>>::result>::value == 2, "");
  static_assert(is_same_v<app<cons<U, cons<U, nil>>, cons<U, nil>>::result,
                          cons<U, cons<U, cons<U, nil>>>>,
                "");
  static_assert(
      is_same_v<
          find<W, record<X, Y, record<W, Z, record<V, nil, empty>>>>::result,
          Some<Z>>,
      "");
  static_assert(
      is_same_v<
          find<U, record<X, Y, record<W, Z, record<V, nil, empty>>>>::result,
          None>,
      "");
  static_assert(
      is_same_v<
          find<V, record<X, Y, record<W, Z, record<V, nil, empty>>>>::result,
          Some<nil>>,
      "");
}
