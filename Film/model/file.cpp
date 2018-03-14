
#include "stdafx.h"
#include "file.h"

#include <algorithm>
#include <locale>


::Resolution File::UnknownResolution = { 0, L"Unknown", false };

::Resolution File::StandardResolutions[] = {
	{ 3840, L"2160", true },
	{ 1920, L"1080", true },
	{ 1280, L"720",  true },
	//{ 1024, L"576",  true },
	//{ 853,  L"480p and smaller", false },
	{ 1024, L"576p and smaller", false },
	UnknownResolution
};


File::File(MSXML2::IXMLDOMNodePtr node)
{
	Filename = node->attributes->getNamedItem(L"Filename")->text;
	Filesize = std::stoull(std::wstring(node->attributes->getNamedItem(L"Filesize")->text));
	Duration = std::stod(std::wstring(node->attributes->getNamedItem(L"Duration")->text));
	Width = std::stoi(std::wstring(node->attributes->getNamedItem(L"Width")->text));
	Height = std::stoi(std::wstring(node->attributes->getNamedItem(L"Height")->text));
	DisplayAspectRatio = std::stod(std::wstring(node->attributes->getNamedItem(L"DisplayAspectRatio")->text));
	ScanType = node->attributes->getNamedItem(L"ScanType")->text;
}

//::Resolution Alternate(double ww)
//{
//	std::pair<::Resolution, double> best = { File::UnknownResolution, std::numeric_limits<double>::infinity() }; // resolution, error
//	for (auto& r : File::StandardResolutions)
//	{
//		//double err = std::abs(r.Width - (DisplayAspectRatio < 1.5 ? (Width * (16.0 / 9.0) / DisplayAspectRatio) : Width));
//		double err = std::abs(r.Width - ww);
//		if (err < best.second) best = { r, err };
//	}
//
//	return best.first;
//}

::Resolution File::Resolution() const
{
	double ww = std::min(Height*DisplayAspectRatio, (double)Width);
	if (DisplayAspectRatio < 1.5) ww = ww*(16. / 9.) / DisplayAspectRatio;
	//auto xx = Alternate(ww);


	std::pair<::Resolution, double> best = { UnknownResolution, std::numeric_limits<double>::infinity() }; // resolution, error
	for (auto& r : StandardResolutions)
	{
		//double err = std::abs(r.Width - (DisplayAspectRatio < 1.5 ? (Width * (16.0 / 9.0) / DisplayAspectRatio) : Width));
		double err = std::abs(r.Width - ww);
		if (err < best.second) best = { r, err };
	}

	//if (xx.Width != best.first.Width) MessageBox(0, (Filename + L"\r\nAlternate: " + xx.Name + L"\r\nOriginal: " + best.first.Name).c_str(), 0, 0);

	if (ScanType != L"Progressive") best.first.Width--;
	best.first.Name += best.first.AppendScanType ? (ScanType == L"Progressive" ? L"p" : L"i") : L"";

	if (best.first.Width >= 1280 && Bitrate() < 3500)
	{
		best.first.Width -= 2;
		best.first.Name += L" (low bitrate)";
	}

	return best.first;
}

std::wstring File::Source() const
{
	auto name = Filename;
	std::transform(name.begin(), name.end(), name.begin(), ::tolower);

	const std::map<std::wstring, std::vector<LPWSTR>> tags = {
		{ L"Blu-ray", { L"bluray", L"hddvd", L"dtheater", L"bdrip", L"brrip" } },
		{ L"DVD", { L"dvdrip" } },
		{ L"Video on demand", { L"web-dl", L"webrip" } },
		{ L"TV", { L"hdtv", L"sdtv", L"dvbrip", L"tvrip" } }
	};

	for (auto& t : tags)
		if (std::any_of(t.second.begin(), t.second.end(), [&name](const LPWSTR& q) { return name.find(q) != std::string::npos; }))
			return t.first;

	return L"Unknown";
}

