/**
 *
 * @file tick.h
 * @date 26.11.2021
 * @author Harvy Aronales Costiniano
 *
 * Copyright (c) 2021 Harvy Aronales Costiniano. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */
#ifndef __TICK_H_
#define __TICK_H_

/* #region EXTERNAL_DEPENDENCY */
#include <Arduino.h>
#include <memory>
/* #endregion */

/* #region INTERNAL_DEPENDENCY */

/* #endregion */

/* #region GLOBAL_VARIABLES */
typedef std::function<void()> tListGenCbFnTick;
/* #endregion */


/* #region CLASS_DECLARATION */
class Tick{
    public:
        Tick();
        Tick(unsigned long durationMs);
        void begin();
        void stop();
        void onTick(tListGenCbFnTick fn);
        void handle();                                                  // This should be call on the loop
    private:
        unsigned long _timer = millis();
        unsigned long _durationMs = 1000;
        bool _cancel = true;                
        tListGenCbFnTick _onTick;
};
/* #endregion Common class */

#include "tick-impl.h"

#endif
