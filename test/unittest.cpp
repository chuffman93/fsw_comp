/*
 * unittest.cpp
 *
 *  Created on: Dec 1, 2017
 *      Author: fsw
 */
#include <iostream>
using namespace std;

void test_ACPManager(void);
void test_Monitor(void);

int main(){
	cout << "Entering Unittest Framework" << endl;
	test_ACPManager();
	test_Monitor();

	cout << "Exiting Unittest Framework" << endl;
	return 0;
}
