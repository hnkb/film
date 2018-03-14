#pragma once

#include <string>
#import <msxml6.dll>

struct Resolution
{ 
	int Width;
	std::wstring Name;
	bool AppendScanType;
};


class File
{
public:
	File() {}
	File(MSXML2::IXMLDOMNodePtr node);

	std::wstring Filename;
	unsigned long long Filesize;
	double Duration;

	int Width;
	int Height;
	double DisplayAspectRatio;
	std::wstring ScanType;

	double Bitrate() const { return (double)Filesize / Duration / 1000. * 8; }
	::Resolution Resolution() const;
	std::wstring Source() const;


	static ::Resolution UnknownResolution;
	static ::Resolution StandardResolutions[];
};

