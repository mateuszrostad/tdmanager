/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CallWithTuple.hpp
 * Author: mmr
 *
 * Created on 22. september 2016, 10:19
 */

#ifndef CALLWITHTUPLE_HPP
#define CALLWITHTUPLE_HPP

#include <functional>
#include <tuple>
#include "Sequence.hpp"


template<typename T, typename... Args, std::size_t... S>
T callWithTupleExpansion(std::function<T(Args...)> const & func, std::tuple<Args...> args, Sequence<S...>)
{
	return func(std::get<S>(args)...);
}


template<typename T, typename... Args>
T callWithTuple(std::function<T(Args...)> const & func, std::tuple<Args...> const & args)
{
	typedef std::tuple<Args...> Tuple;
	typedef std::tuple_size<Tuple> Sizer;

	return callWithTupleExpansion(func, args, SequenceGenerator<Sizer::value>{});
}


template<typename T, typename... Args>
T callWithTuple(T(*func)(Args...), std::tuple<Args...> const & args)
{
	typedef std::tuple<Args...> Tuple;
	typedef std::tuple_size<Tuple> Sizer;

	return callWithTupleExpansion(std::function<T(Args...)>(func), args, SequenceGenerator<Sizer::value>{});
}


#endif /* CALLWITHTUPLE_HPP */

