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
void test_MockHAL(void);

int main(){
	cout << "Entering Unittest Framework" << endl;
	test_ACPManager();
	test_Monitor();
	//test_SPIManager();
	//test_GPIOManager();
	test_MockHAL();

	cout << "Exiting Unittest Framework" << endl;
	return 0;
}
