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

// eq_bool
template <typename B1, typename B2> struct eqb { using result = True; };
template <> struct eqb<True, False> { using result = False; };
template <> struct eqb<False, True> { using result = False; };

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
template <typename OPT>
requires(!same_as<OPT, None>) struct unwrap {
};
template <typename V> struct unwrap<Some<V>> { using result = V; };

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

// aexp
template <typename N> struct ANum {};
template <typename ID> struct AId {};
template <typename A1, typename A2> struct APlus {};
template <typename A1, typename A2> struct AMinus {};
template <typename A1, typename A2> struct AMult {};

// aeval
template <typename ST, typename A> struct aeval {};
template <typename ST, typename N> struct aeval<ST, ANum<N>> {
  using result = N;
};

template <typename ST, typename ID> struct aeval<ST, AId<ID>> {
  using result = typename unwrap<typename find<ID, ST>::result>::result;
};

template <typename ST, typename A1, typename A2>
struct aeval<ST, APlus<A1, A2>> {
  using result = typename plus<typename aeval<ST, A1>::result,
                               typename aeval<ST, A2>::result>::result;
};

template <typename ST, typename A1, typename A2>
struct aeval<ST, AMinus<A1, A2>> {
  using result = typename minus<typename aeval<ST, A1>::result,
                                typename aeval<ST, A2>::result>::result;
};
template <typename ST, typename A1, typename A2>
struct aeval<ST, AMult<A1, A2>> {
  using result = typename mult<typename aeval<ST, A1>::result,
                               typename aeval<ST, A2>::result>::result;
};
// bexp
struct BTrue {};
struct BFalse {};
template <typename B1, typename B2> struct BEq {};
template <typename B1, typename B2> struct BLe {};
template <typename B> struct BNot {};
template <typename B1, typename B2> struct BAnd {};

// beval
template <typename ST, typename B> struct beval {};
template <typename ST> struct beval<ST, BTrue> { using result = True; };
template <typename ST> struct beval<ST, BFalse> { using result = False; };
template <typename ST, typename A1, typename A2> struct beval<ST, BEq<A1, A2>> {
  using result = typename eqn<typename aeval<ST, A1>::result,
                              typename aeval<ST, A2>::result>::result;
};
template <typename ST, typename A1, typename A2> struct beval<ST, BLe<A1, A2>> {
  using result = typename len<typename aeval<ST, A1>::result,
                              typename aeval<ST, A2>::result>::result;
};
template <typename ST, typename B> struct beval<ST, BNot<B>> {
  using result = typename negb<typename beval<ST, B>::result>::result;
};
template <typename ST, typename B1, typename B2>
struct beval<ST, BAnd<B1, B2>> {
  using result = typename andb<typename beval<ST, B1>::result,
                               typename beval<ST, B2>::result>::result;
};

// cexp
struct CSkip {};
template <typename ID, typename A> struct CAss {};
template <typename C1, typename C2> struct CSeq {};
template <typename B, typename C1, typename C2> struct CIf {};
template <typename B, typename C> struct CWhile {};

// ceval
template <typename ST, typename C> struct ceval {};
template <typename ST> struct ceval<ST, CSkip> { using result = ST; };
template <typename ST, typename C1, typename C2>
struct ceval<ST, CAss<C1, C2>> {
  using result = typename ceval<typename ceval<ST, C1>::result, C2>::result;
};
template <typename ST, typename COND, typename C1, typename C2>
struct cond_eval {};
template <typename ST, typename C1, typename C2>
struct cond_eval<ST, True, C1, C2> {
  using result = typename ceval<ST, C1>::result;
};
template <typename ST, typename C1, typename C2>
struct cond_eval<ST, False, C1, C2> {
  using result = typename ceval<ST, C2>::result;
};
template <typename ST, typename B, typename C1, typename C2>
struct ceval<ST, CIf<B, C1, C2>> {
  using result =
      typename cond_eval<ST, typename beval<ST, B>::result, C1, C2>::result;
};
template <typename ST, typename B, typename C> struct ceval<ST, CWhile<B, C>> {
  using result =
      typename cond_eval<ST, typename beval<ST, B>::result,
                         CAss<C, ceval<ST, CWhile<B, C>>>, CSkip>::result;
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
  static_assert(is_same_v<eqn<mult<Three, One>::result, Three>::result, True>,
                "");
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
  static_assert(
      is_same_v<beval<update<X, Three, empty>::result,
                      BAnd<BTrue, BNot<BLe<AId<X>, ANum<Two>>>>>::result,
                True>,
      "");
}
} // namespace oeobia

int main() { oeobia::test(); }
