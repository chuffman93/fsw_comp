/*
 *  Diagnostics.cpp
 *
 *  Created on: September 15, 2016
 *      Author: Alex St. Clair
 *
 *  This file contains all of the tests that can be run in diagnostic mode. When tests are
 *  added, change the NUM_TESTS define statement in CDHStdTasks.cpp to reflect the new number,
 *  and then add the tests to the runDiagnostic() function
 */

#include "util/Diagnostics.h"
#include "util/Logger.h"

using namespace AllStar::Core;

void SampleTest0(bool enable){
	if(enable){
		Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
		logger->Log("Running Sample Test 0", LOGGER_LEVEL_INFO);

		FILE * fp = fopen("diagnostic_results.txt", "a+");
		char results[] = "Ran sample test 0\n";
		fwrite(results, 1, sizeof(results), fp);
		fclose(fp);
	}
}

void SampleTest1(bool enable){
	if(enable){
		Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
		logger->Log("Running Sample Test 1", LOGGER_LEVEL_INFO);

		FILE * fp = fopen("diagnostic_results.txt", "a+");
		char results[] = "Ran sample test 1\n";
		fwrite(results, 1, sizeof(results), fp);
		fclose(fp);
	}
}

void SampleTest2(bool enable){
	if(enable){
		Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
		logger->Log("Running Sample Test 2", LOGGER_LEVEL_INFO);

		FILE * fp = fopen("diagnostic_results.txt", "a+");
		char results[] = "Ran sample test 2\n";
		fwrite(results, 1, sizeof(results), fp);
		fclose(fp);
	}
}

void SampleTest3(bool enable){
	if(enable){
		Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
		logger->Log("Running Sample Test 3", LOGGER_LEVEL_INFO);

		FILE * fp = fopen("diagnostic_results.txt", "a+");
		char results[] = "Ran sample test 3\n";
		fwrite(results, 1, sizeof(results), fp);
		fclose(fp);
	}
}
