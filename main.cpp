#include <concepts>
#include <type_traits>

using namespace std;

namespace oeobia {
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

using Zero = O;
using One = S<O>;
using Two = S<S<O>>;
using Three = S<S<S<O>>>;

// prev
template <typename N> struct prev {};
template <typename N> struct prev<S<N>> { using result = N; };

// plus
template <typename N1, typename N2> struct plus {};
template <typename N> struct plus<O, N> { using result = N; };
template <typename N1, typename N2> struct plus<S<N1>, N2> {
  using result = S<typename plus<N1, N2>::result>;
};

// minus
template <typename N1, typename N2> struct minus {};
template <typename N> struct minus<O, N> { using result = O; };
template <typename N> struct minus<N, O> { using result = N; };
template <typename N1, typename N2> struct minus<S<N1>, S<N2>> {
  using result = typename minus<N1, N2>::result;
};

// mult
template <typename N1, typename N2> struct mult {};
template <typename N> struct mult<O, N> { using result = O; };
template <typename N> struct mult<N, O> { using result = O; };
template <typename N1, typename N2> struct mult<S<N1>, N2> {
  using result = typename plus<typename mult<N1, N2>::result, N2>::result;
};

// bool
struct True {};
struct False {};

// and_bool
template <typename B1, typename B2> struct andb {};
template <typename B>
requires same_as<B, True> || same_as<B, False>
struct andb<True, B> {
  using result = B;
};
template <typename B>
requires same_as<B, True> || same_as<B, False>
struct andb<False, B> {
  using result = False;
};

// or_bool
template <typename B1, typename B2> struct orb {};
template <typename B>
requires same_as<B, True> || same_as<B, False>
struct orb<True, B> {
  using result = True;
};
template <typename B>
requires same_as<B, True> || same_as<B, False>
struct orb<False, B> {
  using result = B;
};

// neg_bool
template <typename B> struct negb {};
template <> struct negb<True> { using result = False; };
template <> struct negb<False> { using result = True; };

// ind_nat_to_lit
template <typename N> struct ind_nat_to_lit {
  enum { value = 1 + ind_nat_to_lit<typename prev<N>::result>::value };
};

template <> struct ind_nat_to_lit<O> {
  enum { value = 0 };
};

// eq_nat
template <typename N1, typename N2> struct eqn {};
template <> struct eqn<O, O> { using result = True; };
template <typename N> struct eqn<O, S<N>> { using result = False; };
template <typename N> struct eqn<S<N>, O> { using result = False; };
template <typename N1, typename N2> struct eqn<S<N1>, S<N2>> {
  using result = typename eqn<N1, N2>::result;
};

// le_nat
template <typename N1, typename N2> struct len {};
template <> struct len<O, O> { using result = True; };
template <typename N> struct len<O, S<N>> { using result = True; };
template <typename N> struct len<S<N>, O> { using result = False; };
template <typename N1, typename N2> struct len<S<N1>, S<N2>> {
  using result = typename len<N1, N2>::result;
};

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

void test() {
  using test_map = record<X, Y, record<W, Z, record<V, nil, empty>>>;
  using test_list1 = cons<U, cons<U, nil>>;
  using test_list2 = cons<U, nil>;
  using test_list3 = cons<U, cons<U, cons<U, nil>>>;

  static_assert(ind_nat_to_lit<Two>::value == 2, "");
  static_assert(is_same_v<eqn<Three, One>::result, False>, "");
  static_assert(is_same_v<eqn<Three, Three>::result, True>, "");
  static_assert(is_same_v<eqn<plus<One, Two>::result, Three>::result, True>,
                "");
  static_assert(is_same_v<eqn<minus<Three, One>::result, Two>::result, True>,
                "");
  static_assert(is_same_v<len<Two, Three>::result, True>, "");
  static_assert(
      is_same_v<eqn<mult<Three, One>::result, Three>::result, True>, "");
  static_assert(
      is_same_v<
          eqn<mult<Two, Three>::result, plus<Three, Three>::result>::result,
          True>,
      "");
  static_assert(ind_nat_to_lit<length<test_list1>::result>::value == 2, "");
  static_assert(is_same_v<app<test_list1, test_list2>::result, test_list3>, "");
  static_assert(is_same_v<find<W, test_map>::result, Some<Z>>, "");
  static_assert(is_same_v<find<U, test_map>::result, None>, "");
  static_assert(is_same_v<find<V, test_map>::result, Some<nil>>, "");
}
} // namespace oeobia

int main() { oeobia::test(); }
