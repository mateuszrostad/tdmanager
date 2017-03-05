/* 
 * File:   Factory.hpp
 * Author: mmr
 *
 * Created on 15. september 2016, 20:22
 */

#ifndef FACTORY_HPP
#define FACTORY_HPP

#include <string>
#include <map>
#include <functional>

#include <iostream>


class Manufacturable {};


struct Factory
{

	typedef std::string Key;
	
	template <typename ...Args>
	struct Creator
	{
		typedef Manufacturable*(Type)(Args...);
	};
	
	template <typename ...Args>
	struct Map
	{
		typedef typename Creator<Args...>::Type CreatorType;
		typedef std::function<CreatorType> CreatorFunc;
		typedef std::map<Key, CreatorFunc> MapType;

		static MapType& get()
		{
			static MapType factoryMap;
			return factoryMap;
		}
	};
	
	template <typename ...Args>
	static Manufacturable* make(Key key, Args... args)
	{
		//std::cout << key << " " << sizeof...(args) << std::endl;
		return Map<Args...>::get()[key](args...);
	}

};


template <typename T, typename ...Args>
class FactoryRegistrar
{

private:

	typedef typename Factory::Map<Args...>::CreatorFunc CreatorFunc;
	
	static Manufacturable* make(Args ...args)
	{
		//std::cout << "FactoryRegistrar::make(...) " << std::endl;
		return new T(args...);
	}

public:

	FactoryRegistrar(Factory::Key key)
	{
		Factory::Map<Args...>::get()[key] = CreatorFunc(&FactoryRegistrar<T, Args...>::make);
	}

	
};


#endif /* FACTORY_HPP */
