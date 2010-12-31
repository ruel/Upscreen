/*
	File:		main.cpp
	Copyright (c) 2010 Ruel Pagayon <ruel@ruel.me>


	This file is part of Upscreen.

    Upscreen is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Upscreen is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Upscreen.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Upscreen.h"

/*
	Basically, this will wait for a PrintScreen keypress. This is the main function, and the sub function
	below holds all other subfunctions. 
*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	Upscreen us;				// Create an instance of our class Upscreen
	us.waitPress();				// Call the Upscreen::waitPress() function.
	return 0;					// End Of Code (EOC)
}