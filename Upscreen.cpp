/*
	File:		Upscreen.cpp
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
	Function Name:	Upscreen
	Parameters:		N/A
	Return Type:	N/A

	Description:	Constructor
*/
Upscreen::Upscreen() {
	// Nothing
}

/*
	Function Name:	~Upscreen
	Parameters:		N/A
	Return Type:	N/A

	Description:	Deconstructor
*/
Upscreen::~Upscreen() {
	// Nothing
}

/*
	Function Name:	waitPress
	Parameters:		N/A
	Return Type:	N/A

	Description:	This subfunction waits for a PrintScreen keypress, and if there is any,
					it will call the function to save the screen image to a temporary folder,
					and another function will be called to upload the image file to min.us
					image hosting service.
*/
void Upscreen::waitPress() {
	MSG key = {0};																	// Variable to capture the input
	HBITMAP picH;																	// Bitmap Handle variable for the clipboard data later
	PBITMAPINFO pbinfo;																// Bitmap Information struct
	HDC hdcScreen = CreateDC(L"DISPLAY", NULL, NULL, NULL);							// Create an HDC variable for CreateBMPFile()
	std::string bmpN = "temp.bmp", pngN = "temp.png";								// string variables for the file names
	std::wstring bmpStr(bmpN.length(), L' '), pngStr(pngN.length(), L' ');			// wstring containers for conversion
	std::copy(bmpN.begin(), bmpN.end(), bmpStr.begin());							// copy bmpN to bmpStr
	std::copy(pngN.begin(), pngN.end(), pngStr.begin());							// copy pngN to pngStr
	LPTSTR bmpName = (LPTSTR)bmpStr.c_str(), pngName = (LPTSTR)pngStr.c_str();		// LPTSTR variables for the file names									// char* variables for the file names
	while (1) {																		// Loop to infinity
		RegisterHotKey(NULL, 1, MOD_WIN | MOD_NOREPEAT, 0x2C);						// Register the PrintScreen (0x2C) key as HotKey
		GetMessage(&key, NULL, 0, 0);												// Get Keyboard Input
		if (key.message == WM_HOTKEY) {												// If PrintScreen is pressed
			OpenClipboard(NULL);													// Open the Clipboard for data retrieval
            picH = (HBITMAP)GetClipboardData(CF_BITMAP);							// Get Clipboard contents
			CloseClipboard();														// Close the Clipboard of course
			pbinfo = Upscreen::CreateBitmapInfoStruct(NULL, picH);					// Get info about the bitmap			
			Upscreen::CreateBMPFile(NULL, bmpName, pbinfo, picH, hdcScreen);		// Create the bitmap file
			Upscreen::ConvertToPNG(bmpName, pngName);								// Convert the BMP to PNG
			remove(bmpN.c_str());													// Delete the BMP file
			if (Upscreen::FileExists(pngN)) {										// Check if the file exist
				Upscreen::UploadImage(pngN);										// Upload image to min.us
				remove(pngN.c_str());												// Delete the PNG file
			}
		}
	}
}

/*
	Function Name:	CreateBMPFile
	Parameters:		HWND		hwnd			- Window Handle
					LPTSTR		pszFile			- Absolute path/filename
					PBITMAPINFO	pbi				- Bitmap information
					HBITMAP		hBMP			- Bitmap Handle
					HDC			hDC				- Device context handle
	Return Type:	N/A

	Description:	I do not take credit on writing this function, More details can be found here
					http://msdn.microsoft.com/en-us/library/dd145119
*/
void Upscreen::CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi, HBITMAP hBMP, HDC hDC) 
 { 
    HANDLE hf;					// file handle  
    BITMAPFILEHEADER hdr;       // bitmap file-header  
    PBITMAPINFOHEADER pbih;     // bitmap info-header  
    LPBYTE lpBits;              // memory pointer  
    DWORD dwTotal;              // total count of bytes  
    DWORD cb;                   // incremental count of bytes  
    BYTE *hp;                   // byte pointer  
    DWORD dwTmp; 

    pbih = (PBITMAPINFOHEADER) pbi; 
    lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

    // Retrieve the color table (RGBQUAD array) and the bits  
    // (array of palette indices) from the DIB.  
    GetDIBits(hDC, hBMP, 0, (WORD) pbih->biHeight, lpBits, pbi, DIB_RGB_COLORS);

    // Create the .BMP file.  
    hf = CreateFile(pszFile, 
                   GENERIC_READ | GENERIC_WRITE, 
                   (DWORD) 0, 
                    NULL, 
                   CREATE_ALWAYS, 
                   FILE_ATTRIBUTE_NORMAL, 
                   (HANDLE) NULL); 

    hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"  
    // Compute the size of the entire file.  
    hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + 
                 pbih->biSize + pbih->biClrUsed 
                 * sizeof(RGBQUAD) + pbih->biSizeImage); 
    hdr.bfReserved1 = 0; 
    hdr.bfReserved2 = 0; 

    // Compute the offset to the array of color indices.  
    hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + 
                    pbih->biSize + pbih->biClrUsed 
                    * sizeof (RGBQUAD); 

    // Copy the BITMAPFILEHEADER into the .BMP file.  
    WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), (LPDWORD) &dwTmp,  NULL);

    // Copy the BITMAPINFOHEADER and RGBQUAD array into the file.  
    WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof (RGBQUAD), (LPDWORD) &dwTmp, ( NULL));

    // Copy the array of color indices into the .BMP file.  
    dwTotal = cb = pbih->biSizeImage; 
    hp = lpBits; 
    WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL);

    // Close the .BMP file.  
    CloseHandle(hf);

    // Free memory.  
    GlobalFree((HGLOBAL)lpBits);
}

/*
	Function Name:	CreateBitmapInfoStruct
	Parameters:		HWND		hwnd			- Window Handle
					HBITMAP		hBMP			- Bitmap Handle
	Return Type:	PBITMAPINFO

	Description:	I do not take credit on writing this function, More details can be found here
					http://msdn.microsoft.com/en-us/library/dd145119
*/
PBITMAPINFO Upscreen::CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp)
{ 
    BITMAP bmp; 
    PBITMAPINFO pbmi;
    WORD    cClrBits;

    // Retrieve the bitmap color format, width, and height.  
    GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp);

    // Convert the color format to a count of bits.  
    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel); 
    if (cClrBits == 1) 
        cClrBits = 1; 
    else if (cClrBits <= 4) 
        cClrBits = 4; 
    else if (cClrBits <= 8) 
        cClrBits = 8; 
    else if (cClrBits <= 16) 
        cClrBits = 16; 
    else if (cClrBits <= 24) 
        cClrBits = 24; 
    else cClrBits = 32; 

    // Allocate memory for the BITMAPINFO structure. (This structure  
    // contains a BITMAPINFOHEADER structure and an array of RGBQUAD  
    // data structures.)  

     if (cClrBits < 24) 
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
                    sizeof(BITMAPINFOHEADER) + 
                    sizeof(RGBQUAD) * (1<< cClrBits)); 

     // There is no RGBQUAD array for these formats: 24-bit-per-pixel or 32-bit-per-pixel 

     else 
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
                    sizeof(BITMAPINFOHEADER)); 

    // Initialize the fields in the BITMAPINFO structure.  

    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
    pbmi->bmiHeader.biWidth = bmp.bmWidth; 
    pbmi->bmiHeader.biHeight = bmp.bmHeight; 
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes; 
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel; 
    if (cClrBits < 24) 
        pbmi->bmiHeader.biClrUsed = (1<<cClrBits); 

    // If the bitmap is not compressed, set the BI_RGB flag.  
    pbmi->bmiHeader.biCompression = BI_RGB; 

    // Compute the number of bytes in the array of color  
    // indices and store the result in biSizeImage.  
    // The width must be DWORD aligned unless the bitmap is RLE 
    // compressed. 
    pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) /8
                                  * pbmi->bmiHeader.biHeight; 
    // Set biClrImportant to 0, indicating that all of the  
    // device colors are important.
     pbmi->bmiHeader.biClrImportant = 0;
     return pbmi;
 }

/*
	Function Name:	ConvertToPNG
	Parameters:		LPTSTR		fileName			- Bitmap source filepath
					LPTSTR		destName			- JPG destination filepath
	Return Type:	N/A

	Description:	This uses the ATL/MFC class CImage to load the bitmap image, and save it as
					a PNG compressed image. Basically this is used to reduce the file size of the
					image. Uploading a 5MB image in a slow connection takes ages. Also, why PNG?
					PNG is for screenshots.
*/
void Upscreen::ConvertToPNG(LPTSTR &sourceName, LPTSTR &destName) {
	CImage img;					// Declare a CImage variable
	img.Load(sourceName);		// Load the bitmap
	img.Save(destName);			// Save it as PNG
}

/*
	Function Name:	UploadImage
	Parameters:		std::string	fileName			- PNG source filepath
	Return Type:	N/A

	Description:	This function basically uploads the given file to the image hosting service
					min.us
*/
void Upscreen::UploadImage(std::string &fileName) {
	Minus uploader;																	// Create instance of the Minus API
	uploader.CreateGallery();														// Retrieve variables for the API
	uploader.UploadItem(fileName);													// Upload the file
	ShellExecute(NULL, NULL, 
				 Upscreen::s2ws("http://min.us/m" + uploader.editorId).c_str(), 
				 NULL, NULL, SW_SHOWNORMAL);										// Open the editor page in browser
}

/*
	Function Name:	FileExists
	Parameters:		std::string	fileName			- File source name
	Return Type:	bool

	Description:	This function checks if a file exists with GetFileAttributes()
*/
bool Upscreen::FileExists(std::string &fName) {
	if (GetFileAttributes(Upscreen::s2ws(fName).c_str()) == 0xFFFFFFFF)					// Check the attributes
		return false;
	return true;
}

/*
	Function Name:	s2ws
	Parameters:		const std::string	s					- String to convert
	Return Type:	std::wstring

	Description:	This function here converts the given std::string to std::wstring
					Source: http://social.msdn.microsoft.com/Forums/en/Vsexpressvc/thread/0f749fd8-8a43-4580-b54b-fbf964d68375
*/
std::wstring Upscreen::s2ws(const std::string& s) {
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}

/*
	Function Name:	HttpUp
	Parameters:		N/A
	Return Type:	N/A

	Description:	Constructor
*/
HttpUp::HttpUp() {
	UserAgent = "Ruel's HttpUp Class 0.1 - ruel.me";					// If the UserAgent is unset, this will be the default
}

/*
	Function Name:	~HttpUp
	Parameters:		N/A
	Return Type:	N/A

	Description:	Deconstructor
*/
HttpUp::~HttpUp() {
	// Nothing
}

/*
	Function Name:	~HttpUp
	Parameters:		std::string	ua						- UserAgent argument
	Return Type:	N/A

	Description:	Sets the UserAgent of the HttpUp class
*/
void HttpUp::SetUserAgent(std::string ua) {
	UserAgent = ua;
}

/*
	Function Name:	FPost
	Parameters:		std::string	url					- URL of the the post action
					std::string postData			- the post data to be posted
					std::string fileName			- file name of the post item
	Return Type:	std::string

	Description:	Performs an HTTP POST request to the given URL using the given
					post data. This is customized, and set to upload a file.
*/
std::string HttpUp::FPost(std::string &url, std::string &postData, std::string &fileName) {
	std::string response, request, rp1, rp2, hostname, uri, fileSize, boundary;					// String variables (too many to mention)
	std::vector<std::string> urldata;															// Vector for holding the url parts
	std::vector<char> rqVec;																	// Chat vector for the binary data
	std::stringstream out,dw;																	// For conversion
	int flz, lenres, sock, port = 80;															// For the response length, socket, and port
	char buffer[BSIZE], *bdata = NULL;															// Buffer with 1024 size
	struct sockaddr_in serveraddr;																// sockaddr_in struct
	struct hostent *host;																		// hostent struct
	DWORD nbRead = 0;																			// DWORD for number of bytes read
	WSADATA wsaData;																			// WSADATA variable (duh)
	HANDLE fh = CreateFile(s2ws(fileName).c_str(),												// Open the file
								GENERIC_READ,
								FILE_SHARE_READ,
								NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL, NULL);																
	flz = GetFileSize(fh, NULL);																// Get file size
	bdata = (char *)malloc(flz);																// Allocate space with the size of the file
	if (!ReadFile(fh, bdata, flz, &nbRead, NULL))												// Read PNG file
		HttpUp::errdie("File Access Error");
	CloseHandle(fh);																			// Close the file handle
	HttpUp::split(url, urldata, '/');															// Split the URL string with the '/' delimeter
	hostname = HttpUp::trim(urldata[2]);														// The third element is the host http:(0)/(1)/domain.com(2)/
	uri = "/";																					// Initialize the relative URI
	for (int i = 3; i < (int)urldata.size() ; i++) {											// Loop through the remaining vector parts
		if (i == urldata.size() - 1)															// If it's the last element
			uri += HttpUp::trim(urldata[i]);													// do not append a trailing slash
		else																					// If it's not the last element
			uri += HttpUp::trim(urldata[i]) + "/";												// append a trailing slash
	}
	uri = trim(uri);
	boundary = "-----rueldotme";																// Boundary string
	rp1 += boundary + "\r\n";																	// Form data
	rp1 += "Content-Disposition: form-data; name=\"file\"; filename=\"" + fileName + "\"\r\n";
	rp1 += "Content-Type: image/png\r\n";
	rp1 += "\r\n";
	rp2 += "\r\n--" + boundary + "--\r\n";														// Form data (end)
	out << (flz + rp1.length() + rp2.length() - 1);												// Content Length int to string
	fileSize = out.str();																		// Store it in a string variable
	request += "POST " + uri + "?" + postData + " HTTP/1.1\r\n";								// These lines here are the HTTP headers to be sent
	request += "Host: " + hostname + "\r\n";
	request += "Connection: keep-alive\r\n";
	request += "Content-Length: " + fileSize + "\r\n";
	request += "Content-Type: multipart/form-data; boundary=" + boundary + "\r\n";
	request += "Accept: */*\r\n";
	request += "User-Agent: " + UserAgent + "\r\n";
	request += "Accept-Encoding: gzip,deflate,sdch\r\n";
	request += "Accept-Language: en-US,en;q=0.8\r\n";
	request += "Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.3\r\n\r\n";
	rqVec.assign(request.begin(), request.end());												// Put HTTP headers at the start of vector
	std::copy(rp1.begin(), rp1.end(), std::back_inserter<std::vector<char> >(rqVec));			// Form data boundary start
	std::copy(bdata, bdata + flz - 1, std::back_inserter<std::vector<char> >(rqVec));			// PNG Data
	std::copy(rp2.begin(), rp2.end(), std::back_inserter<std::vector<char> >(rqVec));			// Form data boundary end
	delete [] bdata;																			// Delete the contents of bdata
	bdata = NULL;																				// Set the bdata pointer to NU::
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)												// Winsock INIT
		HttpUp::errdie("WSAStartup Error");
	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)									// Opens socket
		HttpUp::errdie("socket() Error");
	host = gethostbyname(hostname.c_str());														// gethostbyname function for later
	memset(&serveraddr, 0, sizeof(serveraddr));													// Fills the memory block for serveraddr
	serveraddr.sin_port = htons((unsigned short) port);											// Sets the port. You can change it above
	serveraddr.sin_family = AF_INET;															// Sets sin_family to AF_INET
	serveraddr.sin_addr = *((in_addr *)host->h_addr);											// Sets the s_addr, basically its the IP Address
	if (connect(sock, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)					// Let's try to connect
		HttpUp::errdie("connect() Error");
	if (send(sock, &rqVec[0], rqVec.size(), 0) != rqVec.size())									// Send the request
		HttpUp::errdie("send() Error");

	lenres = BSIZE;																				// Set the response lenght to the buffer size
	while (lenres == BSIZE) {
		lenres = recv(sock, (char*)&buffer, BSIZE, 0);											// Get the response
		if (lenres > 0)
			response += std::string(buffer).substr(0, lenres);									// Store it in a string variable
    }
	response = HttpUp::trim(response.substr(response.find("\r\n\r\n")));						// Separate the response body from the header

	closesocket(sock);																			// Disconnect
	WSACleanup();																				// Let's be tidy

	return response;
}

/*
	Function Name:	Get
	Parameters:		std::string	url					- URL of the get request
	Return Type:	std::string

	Description:	Performs an HTTP GET request to the given URL.
*/
std::string HttpUp::Get(std::string &url) {
	std::string response, request, hostname, uri;												// String variables (too many to mention)
	std::vector<std::string> urldata;															// Vector for holding the url parts
	int lenres, sock, port = 80;																// For the response length, socket, and port
	char buffer[BSIZE];																			// Buffer with 1024 size
	struct sockaddr_in serveraddr;																// sockaddr_in struct
	struct hostent *host;																		// hostent struct
	WSADATA wsaData;																			// WSADATA variable (duh)

	HttpUp::split(url, urldata, '/');															// Split the URL string with the '/' delimeter
	hostname = HttpUp::trim(urldata[2]);														// The third element is the host http:(0)/(1)/domain.com(2)/
	uri = "/";																					// Initialize the relative URI
	for (int i = 3; i < (int)urldata.size() ; i++) {											// Loop through the remaining vector parts
		if (i == urldata.size() - 1)															// If it's the last element
			uri += HttpUp::trim(urldata[i]);													// do not append a trailing slash
		else																					// If it's not the last element
			uri += HttpUp::trim(urldata[i]) + "/";												// append a trailing slash
	}

	request += "GET " + uri + " HTTP/1.1\r\n";													// These lines here are the HTTP headers to be sent
	request += "Host: " + hostname + "\r\n";
	request += "User-Agent: " + UserAgent + "\r\n";
	request += "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n";
	request += "Accept-Language: en-us,en;q=0.5\r\n";
	request += "Accept-Encoding: gzip,deflate\r\n";
	request += "Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n";
	request += "Keep-Alive: 115\r\n";
	request += "Connection: keep-alive\r\n";
	request += "\r\n";																			// HTTP headers ends here

	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)												// Winsock INIT
		HttpUp::errdie("WSAStartup Error");
	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)									// Opens socket
		HttpUp::errdie("socket() Error");

	host = gethostbyname(hostname.c_str());														// gethostbyname function for later
	memset(&serveraddr, 0, sizeof(serveraddr));													// Fills the memory block for serveraddr
	serveraddr.sin_port = htons((unsigned short) port);											// Sets the port. You can change it above
	serveraddr.sin_family = AF_INET;															// Sets sin_family to AF_INET
	serveraddr.sin_addr = *((in_addr *)host->h_addr);											// Sets the s_addr, basically its the IP Address
	
	if (connect(sock, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)					// Let's try to connect
		HttpUp::errdie("connect() Error");
	if (send(sock, request.c_str(), request.length(), 0) != request.length())					// Send the request
		HttpUp::errdie("send() Error");

	lenres = BSIZE;																				// Set the response lenght to the buffer size
	while (lenres == BSIZE) {
		lenres = recv(sock, (char*)&buffer, BSIZE, 0);											// Get the response
		if (lenres > 0)
			response += std::string(buffer).substr(0, lenres);									// Store it in a string variable
    }

	response = HttpUp::trim(response.substr(response.find("\r\n\r\n")));						// Separate the response body from the header

	closesocket(sock);																			// Disconnect
	WSACleanup();																				// Let's be tidy
	return response;
}

/*
	Function Name:	Minus
	Parameters:		std::string	errstr							- Error string to output
	Return Type:	N/A

	Description:	Provides an Error MessageBox and Exits the application
*/
void HttpUp::errdie(std::string errstr) {
	MessageBox(NULL, HttpUp::s2ws(errstr).c_str(), L"Error", MB_OK);
	exit(-1);
}


/*
	Function Name:	split
	Parameters:		std::string					string		- String to split
					std::vector<std::string>	tokens		- Vector to hold the data
					const char					delim		- Delimeter
	Return Type:	N/A

	Description:	This splits the string with the given delimeter.
*/
void HttpUp::split(std::string &string, std::vector<std::string> &tokens, const char &delim) {
	std::string ea;
	std::stringstream stream(string);
	while(getline(stream, ea, delim))
		tokens.push_back(ea);
}

/*
	Function Name:	s2ws
	Parameters:		const std::string	s					- String to convert
	Return Type:	std::wstring

	Description:	This function here converts the given std::string to std::wstring
					Source: http://social.msdn.microsoft.com/Forums/en/Vsexpressvc/thread/0f749fd8-8a43-4580-b54b-fbf964d68375
*/
std::wstring HttpUp::s2ws(const std::string& s) {
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}


/*
	Function Name:	trim
	Parameters:		std::string	s					- String to trim
	Return Type:	std::string

	Description:	This little function trims both ends of the string
					Credits: http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring/217605#217605
*/
std::string HttpUp::trim(std::string &s) {
        return ltrim(rtrim(s));
}

/*
	Function Name:	ltrim
	Parameters:		std::string	s					- String to trim
	Return Type:	std::string

	Description:	This function trims the left part of the string
					Credits: http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring/217605#217605
*/
std::string HttpUp::ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

/*
	Function Name:	rtrim
	Parameters:		std::string	s					- String to trim
	Return Type:	std::string

	Description:	This trims the right end of the string
					Credits: http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring/217605#217605
*/
std::string HttpUp::rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

/*
	Function Name:	Minus
	Parameters:		N/A
	Return Type:	N/A

	Description:	Constructor
*/
Minus::Minus() {
	// Nothing
}

/*
	Function Name:	~Minus
	Parameters:		N/A
	Return Type:	N/A

	Description:	Deconstructor
*/
Minus::~Minus() {
	// Nothing
}

/*
	Function Name:	CreateGallery
	Parameters:		N/A
	Return Type:	N/A

	Description:	Creates a gallery for uploading. This will fetch the URL
					http://min.us/api/CreateGallery and retrieves the editor_id,
					reader_id, and the key. These will be used in uploading.
*/
void Minus::CreateGallery() {
	HttpUp browser;												// Create and instance of the HttpUp class
	browser.SetUserAgent("Upscreen Uploader - Ruel.me");		// Set the useragent
	std::string url = "http://min.us/api/CreateGallery";		// Define the URL for this API call
	std::string retStr = browser.Get(url);						// Make a request
	json_spirit::mValue jval;									// Variable that will generate the json values 
	json_spirit::read_string(retStr, jval);						// Read json from the response string
	readerId = jval.get_obj()["reader_id"].get_str();			// Get the reader_id
	editorId = jval.get_obj()["editor_id"].get_str();			// Get the editor_id
	key = jval.get_obj()["key"].get_str();						// Get the key
}

/*
	Function Name:	CreateGallery
	Parameters:		std::string	fileName
	Return Type:	N/A

	Description:	Uploads the image with the filename given.
*/
void Minus::UploadItem(std::string &fileName) {
	HttpUp browser;												// Create and instance of the HttpUp class
	browser.SetUserAgent("Upscreen Uploader - Ruel.me");		// Set the useragent
	std::string url = "http://min.us/api/UploadItem/";			// Define the URL for this API call
	std::string postData = "editor_id=" + editorId				// Initialize the postdata to be passed
						 + "&key="+ key
						 + "&filename=" + fileName;
	std::string retStr = browser.FPost(url, postData, fileName);// Make a request
	json_spirit::mValue jval;									// Variable that will generate the json values 
	json_spirit::read_string(retStr, jval);						// Read json from the response string
	id = jval.get_obj()["id"].get_str();						// Get the id
}