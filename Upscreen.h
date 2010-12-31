/*
	File:		Upscreen.h
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

#ifndef UPSCREEN_H
#define UPSCREEN_H
											
/*
	Header Files
*/
#include <WinSock2.h>												// Windows Socket Library
#include <windows.h>												// Windows API
#include <atlimage.h>												// For the CImage class
#include <string>													// C++ string library
#include <json_spirit\json_spirit_reader_template.h>				// JSON Parser by John W. Wilkinson

/*
	Some Variables
*/
#define BSIZE	1024

/*
	Upscreen Class
*/
class Upscreen {
public:
	Upscreen();														// Constructor
	virtual ~Upscreen();											// Deconstructor
	void waitPress();												// Function prototype/declaration for the waitPress() function
	
private:
	void CreateBMPFile(HWND, LPTSTR, PBITMAPINFO, HBITMAP, HDC);	// Function prototype/declaration for CreateBMPFile() function
	PBITMAPINFO CreateBitmapInfoStruct(HWND, HBITMAP);				// Function prototype/declaration for CreateBMPFile() function
	void ConvertToPNG(LPTSTR&, LPTSTR&);							// Function prototype/declaration for ConvertToPNG() function
	void UploadImage(std::string&);									// Function prototype/declaration for UploadImage() function
	bool FileExists(std::string&);									// Function prototype/declaration for FileExists() function
	std::wstring s2ws(const std::string& s);						// Function prototype/declaration for s2ws() function
};

/*
	HttpUp Class
*/
class HttpUp {
public:
	std::string UserAgent;											// UserAgent string

	HttpUp();														// Constructor
	virtual ~HttpUp();												// Deconstructor
	void SetUserAgent(std::string);									// Function prototype/declaration for SetUserAgent() function
	std::string FPost(std::string&, std::string&, std::string&);	// Function prototype/declaration for Post() function
	std::string Get(std::string&);									// Function prototype/declaration for Get() function

private:
	void errdie(std::string);											// Function prototype/declaration for errdie() function
	void split(std::string&, std::vector<std::string>&, const char&);	// Function prototype/declaration for split() function
	std::wstring s2ws(const std::string& s);							// Function prototype/declaration for s2ws() function
	std::string trim(std::string&);										// Function prototype/declaration for trim() function
	std::string ltrim(std::string&);									// Function prototype/declaration for ltrim() function
	std::string rtrim(std::string&);									// Function prototype/declaration for rtrim() function
};

/*
	Minus API
*/
class Minus {
public:
	std::string editorId, id, readerId;								// Editor ID and Image ID

	Minus();														// Constructor
	virtual ~Minus();												// Deconstructor
	void CreateGallery();											// Function prototype/declaration for CreateGallery() function
	void UploadItem(std::string&);									// Function prototype/declaration for UploadItem() function

private:
	std::string key;										// Variables for the API
};

#endif