/*
 * TaskManager.hpp
 *
 *  Created on: Mar 20, 2026
 *      Author: karadeli
 */

#ifndef TASKMANAGER_TASKMANAGER_HPP_
#define TASKMANAGER_TASKMANAGER_HPP_

#include "actuatorController.h"

class TaskManager {
public:
	TaskManager();
	bool start;
	bool end;
	bool running;
	bool createFile;
	bool endFile;
	bool writeFile;

	void startTask();
};


#endif /* TASKMANAGER_TASKMANAGER_HPP_ */
