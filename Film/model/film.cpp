
#include "stdafx.h"
#include "film.h"


Film::Film(MSXML2::IXMLDOMNodePtr node)
{
	Name = node->attributes->getNamedItem(L"Name")->text;
	ReleaseDate = node->attributes->getNamedItem(L"ReleaseDate")->text;
	DisplayReleaseDate = months[ReleaseDate.substr(5, 2)] + L" " + ReleaseDate.substr(0, 4);
	Rating = std::stoi(std::wstring(node->attributes->getNamedItem(L"Rating")->text));
	DownloadStatus = node->attributes->getNamedItem(L"DownloadStatus")->text;
	
	if (node->attributes->getNamedItem(L"Wikipedia") != nullptr) Wikipedia = node->attributes->getNamedItem(L"Wikipedia")->text;
	if (node->attributes->getNamedItem(L"IMDb") != nullptr) IMDb = node->attributes->getNamedItem(L"IMDb")->text;

	FolderName = node->attributes->getNamedItem(L"FolderName")->text;
	SeriePath = std::wstring(node->parentNode->nodeName) == L"Serie" ? std::wstring(node->parentNode->attributes->getNamedItem(L"FolderName")->text) + L"\\" : L"";

	auto fnl = node->selectNodes(L"File");
	if (fnl)
		for (long i = 0; i < fnl->length; i++)
			Files.push_back(File(fnl->item[i]));

	UpdateQuality();
}

void Film::UpdateQuality()
{
	Source = L"Unknown";
	Resolution = File::UnknownResolution;
	AspectRatio = UnknownAspectRatio;

	auto f = GetFirstFile();
	if (f == nullptr || f->Height == 0 || f->Width == 0 || f->DisplayAspectRatio == 0) return;

	AspectRatio = *std::min_element(StandardAspectRatios.begin(), StandardAspectRatios.end(), [&f](const ::AspectRatio& a, const ::AspectRatio& b) { return std::abs(f->DisplayAspectRatio - a.Ratio) < std::abs(f->DisplayAspectRatio - b.Ratio); });
	Resolution = f->Resolution();
	Source = f->Source();
}

const File* Film::GetFirstFile() const
{
	if (!Files.size()) return nullptr;
	return &Files[0];

	//var f = Files.FirstOrDefault(q = > string.IsNullOrWhiteSpace(q.Title));
	//if (f == null) f = Files.FirstOrDefault(q = > q.Title == "Disc 1");
	//if (f == null) f = Files.FirstOrDefault();
	//return f;
}

std::wstring Film::GetFullPath() const
{
	return L"D:\\Collections\\Film\\" + SeriePath + FolderName;
}

void Film::OpenLocation() const
{
	ShellExecute(NULL, NULL, GetFullPath().c_str(), NULL, NULL, SW_SHOW);
}

void Film::Play() const
{
	auto f = GetFirstFile();
	if (f) ShellExecute(NULL, NULL, (GetFullPath() + L"\\" + f->Filename).c_str(), NULL, NULL, SW_SHOW);
}


std::map<std::wstring, std::wstring> Film::months = {
	{ L"01", L"Jan" },{ L"02", L"Feb" },{ L"03", L"Mar" },{ L"04", L"Apr" },{ L"05", L"May" },{ L"06", L"Jun" },
	{ L"07", L"Jul" },{ L"08", L"Aug" },{ L"09", L"Sep" },{ L"10", L"Oct" },{ L"11", L"Nov" },{ L"12", L"Dec" }
};

::AspectRatio Film::UnknownAspectRatio = { 0, L"Unknown" };
std::vector<::AspectRatio> Film::StandardAspectRatios = { { 2.55, L"2.55:1" },{ 2.35, L"2.35:1" },{ 1.85, L"1.85:1" },{ 1.78, L"16:9" },{ 1.33, L"4:3" },UnknownAspectRatio };
