/*
 * UserTask.hpp
 *
 *  Created on: Jul 7, 2026
 *      Author: karadeli
 *
 *  This and UserTask.cpp are the ONLY files the mission team edits.
 */

#ifndef USERTASKS_USERTASK_HPP_
#define USERTASKS_USERTASK_HPP_

#include "stdint.h"
#include "MissionControl.hpp"

/* missionTable[], missionTableCount and safetyConnectorReleased() are
 * declared in MissionControl.hpp and defined in UserTask.cpp.
 *
 * Index of the mission that is selected at boot (before any UART SEL). */
extern const uint8_t defaultMissionIndex;

#endif /* USERTASKS_USERTASK_HPP_ */
