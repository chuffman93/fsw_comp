/*
 * testmacros.h
 *
 *  Created on: Dec 5, 2017
 *      Author: cyborg9
 */

#ifndef TESTMACROS_H_
#define TESTMACROS_H_

#include <iostream>
#include <vector>
using namespace std;

#define FAILIF(cond, failstr) do{ \
		if(cond){ \
			cout << failstr << endl; \
			return; \
		} \
		cout << "."; \
	}while(0);

#define VECTOROFDATA(name, type, ...) \
		type initializer_##name[] = {__VA_ARGS__}; \
		vector<type> name(initializer_##name, initializer_##name + sizeof(initializer_##name)/sizeof(type));

#define PROMPT(prompt_str) do{ \
		std::cout << prompt_str << std::endl; \
		std::cout.flush(); \
		std::cin.ignore(); \
	}while(0);




#endif /* TESTMACROS_H_ */
