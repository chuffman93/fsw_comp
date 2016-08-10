#include <core/StdTypes.h>
#include <gtest/gtest.h>
#include <unistd.h>
#include <util/FileHandler.h>

/* testfilehandler0004.cpp
 * Created on: Jul 07, 2014
 * Author: Trevor Arrasmith
 * \brief Test logAppend in FileHandler
 */

#include <cstdio>
#include <string>

using namespace std;
using namespace AllStar::Core;

extern const char * writeDir;

TEST(TestFileHandler, FileExistsInTruDat)
{
	{
		FileHandler *fh = dynamic_cast<FileHandler *>(Factory::GetInstance(FILE_HANDLER_SINGLETON));

		string errLogFullPath = writeDir;
		string modeLogFullPath = writeDir;
		errLogFullPath.append("Error_Log.dat");
		modeLogFullPath.append("Mode_Log.dat");

		// MAY NEED SUDO ACCESS TO RUN. YOU CAN DO THIS BY RUNNING
		// "sudo ./Test" in the correct directory, instead of running it through eclipse
		if (unlink(errLogFullPath.c_str()) < 0)
		{
			cout << "failed to delete Error_Log.dat file" << endl;
		}
		if (unlink(modeLogFullPath.c_str()) < 0)
		{
			cout << "failed to delete Mode_Log.dat file" << endl;
		}


		fh->LogAppend(LOG_ERROR, 0xff, 0xfe);
		ASSERT_TRUE(fh->FileExistsInTruDat(errLogFullPath));

		fh->LogAppend(LOG_MODE, 0xbe, 0xef);
		ASSERT_TRUE(fh->FileExistsInTruDat(modeLogFullPath));
	}
}
