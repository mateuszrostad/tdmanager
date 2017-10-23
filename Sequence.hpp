/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Sequence.hpp
 * Author: mmr
 *
 * Created on 22. september 2016, 09:35
 */

#ifndef SEQUENCE_HPP
#define SEQUENCE_HPP

#include <cstddef> // std::size_t

template <std::size_t... S>
struct Sequence
{
	constexpr Sequence() = default;
	constexpr Sequence(const Sequence<S...>&) {}
};

template <std::size_t N, std::size_t... S>
struct SequenceGenerator : SequenceGenerator<N-1, N-1, S...>
{
	constexpr SequenceGenerator() = default;
	constexpr SequenceGenerator(const SequenceGenerator<N, S...>&) {}
};

template <std::size_t... S>
struct SequenceGenerator<0, S...> : Sequence<S...>
{
	constexpr SequenceGenerator() = default;
	constexpr SequenceGenerator(const SequenceGenerator<S...>&) {}
};


/*
 * // Example:
 * // SequenceGenerator<3> generates:
 * // SequenceGenerator<3> : SequenceGenerator<2, 2> : SequenceGenerator<1, 1, 2> : SequenceGenerator<0, 0, 1, 2> : Sequence<0, 1, 2> {}
 *
 * // Thus:
 * template<std::size_t ...S>
 * void foo(Sequence<S...>, ...)
 * {
 *     //Do magic with S...
 * }
 * 
 * // may be called with implicit template parameter as follows:
 * foo(SequenceGenerator<N>, ...);
 * // thus generating indices {0, 1, ... N-1} that can be used variadically
 * 
 * // So, say we receive a std::tuple<Args...> (with a template parameter pack Args...) that we want to expandand to a parameter pack for a function func:
 * template<std::size_t... S>
 * void foo(Sequence<S...>, std::tuple<Args...> args)
 * {
 *     func(std::get<S>(args)...);
 * }
 * 
 * // may be called as follows:
 * typedef std::tuple<Args...> T;
 * T args;
 * foo(SequenceGenerator<std::tuple_size<T>::value>, args);
 * 
 * // This can be further encapsulated
 * void callWithTuple(T args)
 * {
 *     foo(SequenceGenerator<std::tuple_size<T>::value>, args);
 * }
*/

#endif /* SEQUENCE_HPP */

