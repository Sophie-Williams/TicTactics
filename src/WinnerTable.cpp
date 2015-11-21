/*
 * TicTacticsBot
 * /TicTacticsBot/src/WinnerTable.cpp
 *
 *  Created on: 13.07.2015
 *      Author: WorldSEnder
 */

#include "WinnerTable.h"

bool winnerTable[512] =
  { false, false, false, false, false, false, false, true, false, false, false,
      false, false, false, false, true, false, false, false, false, false,
      false, false, true, false, false, false, false, false, false, false, true,
      false, false, false, false, false, false, false, true, false, false,
      false, false, false, false, false, true, false, false, false, false,
      false, false, false, true, true, true, true, true, true, true, true, true,
      false, false, false, false, false, false, false, true, false, true, false,
      true, false, true, false, true, false, false, false, false, true, true,
      true, true, false, true, false, true, true, true, true, true, false,
      false, false, false, false, false, false, true, false, true, false, true,
      false, true, false, true, false, false, false, false, true, true, true,
      true, true, true, true, true, true, true, true, true, false, false, false,
      false, false, false, false, true, false, false, false, false, false,
      false, false, true, false, false, true, true, false, false, true, true,
      false, false, true, true, false, false, true, true, false, false, false,
      false, false, false, false, true, false, false, false, false, false,
      false, false, true, false, false, true, true, false, false, true, true,
      true, true, true, true, true, true, true, true, false, false, false,
      false, false, false, false, true, false, true, false, true, false, true,
      false, true, false, false, true, true, true, true, true, true, false,
      true, true, true, true, true, true, true, false, false, false, false,
      false, false, false, true, false, true, false, true, false, true, false,
      true, false, false, true, true, true, true, true, true, true, true, true,
      true, true, true, true, true, false, false, false, false, false, false,
      false, true, false, false, false, false, false, false, false, true, false,
      true, false, true, false, true, false, true, false, true, false, true,
      false, true, false, true, false, false, false, false, true, true, true,
      true, false, false, false, false, true, true, true, true, false, true,
      false, true, true, true, true, true, true, true, true, true, true, true,
      true, true, false, false, false, false, false, false, false, true, false,
      true, false, true, false, true, false, true, false, true, false, true,
      true, true, true, true, false, true, false, true, true, true, true, true,
      false, false, false, false, true, true, true, true, false, true, false,
      true, true, true, true, true, false, true, false, true, true, true, true,
      true, true, true, true, true, true, true, true, true, false, false, false,
      false, false, false, false, true, false, false, false, false, false,
      false, false, true, false, true, true, true, false, true, true, true,
      false, true, true, true, false, true, true, true, false, false, false,
      false, true, true, true, true, false, false, false, false, true, true,
      true, true, false, true, true, true, true, true, true, true, true, true,
      true, true, true, true, true, true, true, true, true, true, true, true,
      true, true, true, true, true, true, true, true, true, true, true, true,
      true, true, true, true, true, true, true, true, true, true, true, true,
      true, true, true, true, true, true, true, true, true, true, true, true,
      true, true, true, true, true, true, true, true, true, true, true, true,
      true, true, true, true, true, true, true, true, true, true };

