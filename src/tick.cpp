/**
 *
 * @file tick.cpp
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

/* #region SELF_HEADER */
#include "tick.h"
/* #endregion */

/* #region CLASS_DEFINITION */
Tick::Tick(){}
Tick::Tick(unsigned long durationMs){
    this->_durationMs = durationMs;
}
void Tick::begin(){
    this->_cancel = false;
}
void Tick::stop(){
    this->_cancel = true;
}
void Tick::onTick(tListGenCbFnTick fn){
    this->_onTick = fn;
}
void Tick::handle(){
    if(this->_cancel) return;
    if((millis() - this->_timer) > this->_durationMs){
        this->_timer = millis(); 
        if(this->_onTick) this->_onTick();
    }
}
/* #endregion */