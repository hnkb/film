#pragma once

#include <string>
#import <msxml6.dll>
#include "file.h"


struct AspectRatio
{
	double Ratio;
	std::wstring Name;
};


class Film
{
private:
	/*for sqlce database*/public:
	void UpdateQuality();
	
	std::wstring SeriePath;
	std::wstring FolderName;

public:
	Film() {}
	Film(MSXML2::IXMLDOMNodePtr node);
	
	int FilmId;

	std::wstring Name;
	std::wstring SearchName;
	std::wstring ReleaseDate;
	std::wstring DisplayReleaseDate;
	int Rating;
	std::wstring DownloadStatus;
	
	std::wstring Wikipedia;
	std::wstring IMDb;

	std::vector<File> Files;
	std::wstring Poster;

	::Resolution Resolution;
	::AspectRatio AspectRatio;
	std::wstring Source;

	static std::map<std::wstring, std::wstring> months;
	static std::vector<::AspectRatio> StandardAspectRatios;
	static ::AspectRatio UnknownAspectRatio;

	const File* GetFirstFile() const;
	std::wstring GetFullPath() const;
	void OpenLocation() const;
	void Play() const;

	bool Selected = false;
};
