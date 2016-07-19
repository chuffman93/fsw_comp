/*
 * StorageManager.h
 * Written by: Alex St. Clair
 * Created: 7/18/16
 *
 * This class shall be used as a catch-all for ensuring that the file system doesn't fill up
 */

#ifndef _STORAGEMANAGER_H
#define _STORAGEMANAGER_H

#include "core/StdTypes.h"
#include <sys/statvfs.h>

class StorageManager{
public:
	// Constructor using GPS time as threshold
	StorageManager(float threshold_in);

	// Constructor using input time as threshold
	StorageManager(int week_start, float seconds_start, float threshold_in);

	// Method called to perform a check
	int CheckAndClean(void);

	// Removes all files in weeks from weekStart to weekEnd
	void CleanFiles(uint16 weekStart, uint16 weekEnd);

	// Getters and setters
	float getThreshold() const {
		return storage_threshold;
	}
	void setThreshold(float threshold) {
		this->storage_threshold = threshold;
	}

private:
	// Checks storage, returns false if past threshold
	bool CheckStorage();

	// Cleans files using an incremented threshold, updates the member variables
	void CleanFiles(void);

	// Threshold for file deletion (percentage)
	float storage_threshold;

	// current threshold for oldest files (there should be no older files)
	int oldest_week;
	float oldest_seconds;

	/*
	 * To prevent looping, if true, functionality will be stopped, and an error will be reported to ground
	 * false: no errors
	 * true:  "error mode"
	 */
	bool errorMode;

	// file system struct
	struct statvfs svfs;
};

#endif
