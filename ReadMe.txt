========================================================================
    Upscreen 0.1 rev 0
========================================================================

// General Description

Upscreen is a screenshot utility using the Min.us API.

This program gets information from the clipboard, if it's an
image file, saves it as a bitmap, converts it to PNG, and uploads
the image to the min.us image hosting service. After uploading
it will open the editor page at the default web browser.

/// Usage

This program is built mainly for screenshots, though this can be used on any image.
If you want to upload a screenshot, press the PrtScn key (get screenshot),
and Win + PrtScn key (upload screenshot).

If you want to use this on a regular image (on the web), copy the image to clipboard
and again, press Win + PrtScn key to upload.

/// Compilation and Testing

This is compiled, built, and tested on MSVC++ 2010

/// Header Files

Upscreen.h	->	Header file containing the classes and pre-processor

/// Source Files

main.cpp	->	Main source file containing the main function.
Upscreen.cpp	->	Function definitions.

/// License

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

(c) Copyright 2010 Ruel Pagayon
ruel@ruel.me
http://ruel.me