
#include "stdafx.h"
#include "data.h"

#include <algorithm>
#include <iomanip>
#include <functional>
#import <msxml6.dll>


const LPWSTR database_filename = L"C:\\Users\\Hani\\Code\\Collections\\Films\\bin\\Release\\Films.Models.Context.sdf";


void OpenRawOledb()
{
	CComPtr<IRowset> rs;

	// Open rowset
	{
		CComPtr<IDBInitialize> db_init;
		if (FAILED(db_init.CoCreateInstance(CLSID_SQLSERVERCE))) MessageBox(0, L"Failed", 0, MB_ICONERROR);

		// Set data source property
		{
			DBPROP rgProps[1];
			rgProps[0].dwPropertyID = DBPROP_INIT_DATASOURCE;
			rgProps[0].dwOptions = DBPROPOPTIONS_REQUIRED;
			rgProps[0].vValue.vt = VT_BSTR;
			rgProps[0].vValue.bstrVal = SysAllocString(L"C:\\Users\\Hani\\Desktop\\Films.Models.Context.sdf");
			//if (!(rgProps[0].vValue.bstrVal)) { hr = E_OUTOFMEMORY; goto Exit; }

			DBPROPSET rgPropSets[1];
			rgPropSets[0].rgProperties = rgProps;
			rgPropSets[0].cProperties = sizeof(rgProps) / sizeof(DBPROP);
			rgPropSets[0].guidPropertySet = DBPROPSET_DBINIT;

			CComQIPtr<IDBProperties> db_prop(db_init);
			if (FAILED(db_prop->SetProperties(sizeof(rgPropSets) / sizeof(DBPROPSET), rgPropSets))) MessageBox(0, L"Failed", 0, MB_ICONERROR);
		}

		if (FAILED(db_init->Initialize())) MessageBox(0, L"Failed", 0, MB_ICONERROR);

		// Use command to open row set
		if (0)
		{
			CComPtr<IDBCreateCommand> db_cc;

			CComQIPtr<IDBCreateSession> db_create_session(db_init);
			if (FAILED(db_create_session->CreateSession(nullptr, IID_IDBCreateCommand, (IUnknown**)&db_cc))) MessageBox(0, L"Failed", 0, MB_ICONERROR);
		}

		// Read complete table
		{
			CComPtr<IOpenRowset> db_or;
			if (FAILED(CComQIPtr<IDBCreateSession>(db_init)->CreateSession(nullptr, IID_IOpenRowset, (IUnknown**)&db_or))) MessageBox(0, L"Failed", 0, MB_ICONERROR);

			DBID table_id;
			table_id.eKind = DBKIND_NAME;
			table_id.uName.pwszName = L"Films";
			//table_id.uName.ulPropid = PROPID_ACC_NAME;

			if (FAILED(db_or->OpenRowset(nullptr, &table_id, nullptr, IID_IRowset, 0, nullptr, (IUnknown**)&rs))) MessageBox(0, L"Failed", 0, MB_ICONERROR);
		}
	}

	// Read data
	{
		DBCOUNTITEM count;
		HROW all[2000];
		int sum = 0;
		do {
			rs->GetNextRows(DB_NULL_HCHAPTER, 0, 2000, &count, (HROW**)&all);
			rs->ReleaseRows(count, all, nullptr, nullptr, nullptr);
			sum++;
		} while (count);

		MessageBox(0, std::to_wstring(sum).c_str(), 0, 0);
	}

}
void OpenAtlOledb(std::vector<Film>& films)
{
	class CFilmRow
	{
	public:
		int FilmId;
		WCHAR Name[1000];
		WCHAR ReleaseDate[20];
		int Rating;
		int DownloadStatus;
		WCHAR Wikipedia[1000];
		WCHAR IMDb[20];
		WCHAR FolderName[1000];
		WCHAR OriginalName[1000];
		WCHAR SeriePath[1000];

		BEGIN_COLUMN_MAP(CFilmRow)
			COLUMN_ENTRY(1, FilmId)
			COLUMN_ENTRY(2, Name)
			COLUMN_ENTRY(3, ReleaseDate)
			COLUMN_ENTRY(4, Rating)
			COLUMN_ENTRY(5, DownloadStatus)
			COLUMN_ENTRY(6, Wikipedia)
			COLUMN_ENTRY(7, IMDb)
			COLUMN_ENTRY(8, FolderName)
			COLUMN_ENTRY(9, OriginalName)
			COLUMN_ENTRY(10, SeriePath)
		END_COLUMN_MAP()
	};
	class CFileRow
	{
	public:
		WCHAR Filename[1000];
		INT64 Filesize;
		double Duration;
		int Width;
		int Height;
		double DisplayAspectRatio;
		WCHAR ScanType[20];

		BEGIN_COLUMN_MAP(CFileRow)
			COLUMN_ENTRY(1, Filename)
			COLUMN_ENTRY(2, Filesize)
			COLUMN_ENTRY(3, Duration)
			COLUMN_ENTRY(4, Width)
			COLUMN_ENTRY(5, Height)
			COLUMN_ENTRY(6, DisplayAspectRatio)
			COLUMN_ENTRY(7, ScanType)
		END_COLUMN_MAP()
	};
	class CPosterRow
	{
	public:
		WCHAR Filename[1000];

		BEGIN_COLUMN_MAP(CPosterRow)
			COLUMN_ENTRY(1, Filename)
		END_COLUMN_MAP()
	};

	std::wstring DownloadStatusNames[]{ L"None", L"Queued", L"Unavailable" };

	CDataSource ds;
	CSession ss;

	// Open database file and session
	{
		DBPROP rgProps[1];
		rgProps[0].dwPropertyID = DBPROP_INIT_DATASOURCE;
		rgProps[0].dwOptions = DBPROPOPTIONS_REQUIRED;
		rgProps[0].vValue.vt = VT_BSTR;
		rgProps[0].vValue.bstrVal = SysAllocString(database_filename);
		//if (!(rgProps[0].vValue.bstrVal)) { hr = E_OUTOFMEMORY; goto Exit; }

		DBPROPSET rgPropSets[1];
		rgPropSets[0].rgProperties = rgProps;
		rgPropSets[0].cProperties = sizeof(rgProps) / sizeof(DBPROP);
		rgPropSets[0].guidPropertySet = DBPROPSET_DBINIT;

		if (FAILED(ds.Open(CLSID_SQLSERVERCE, rgPropSets))) MessageBox(0, L"Failed", 0, MB_ICONERROR);

		SysFreeString(rgProps[0].vValue.bstrVal);
	}
	if (FAILED(ss.Open(ds))) MessageBox(0, L"Failed", 0, MB_ICONERROR);

	CCommand<CAccessor<CFilmRow>> cmd;
	CCommand<CAccessor<CFileRow>> fc;
	CCommand<CAccessor<CPosterRow>> fp;

	if (FAILED(cmd.Open(ss, L"SELECT [Films].[FilmId], [Films].[Name], [Films].[ReleaseDate], [Films].[Rating], [Films].[DownloadStatus], [Films].[Wikipedia], [Films].[IMDb], [Films].[FolderName], [Films].[OriginalName], [Series].[FolderName] FROM [Films] LEFT OUTER JOIN [Series] ON [Films].[Serie_SerieId]=[Series].[SerieId]")))
		MessageBox(0, L"Failed", 0, MB_ICONERROR);
	while (cmd.MoveNext() == S_OK)
	{
		Film f;

		f.FilmId = cmd.FilmId;
		f.Name = cmd.Name;
		f.SearchName = f.Name + L" " + cmd.OriginalName;

		cmd.ReleaseDate[11] = 0;
		f.ReleaseDate = cmd.ReleaseDate;
		f.DisplayReleaseDate = Film::months[f.ReleaseDate.substr(5, 2)] + L" " + f.ReleaseDate.substr(0, 4);
		f.Rating = cmd.Rating;
		f.DownloadStatus = DownloadStatusNames[cmd.DownloadStatus];

		f.Wikipedia = cmd.Wikipedia;
		f.IMDb = cmd.IMDb;

		f.FolderName = cmd.FolderName;
		f.SeriePath = cmd.SeriePath[0] ? std::wstring(cmd.SeriePath) + L"\\" : L"";

		if (FAILED(fc.Open(ss, (std::wstring(L"SELECT [Filename], [Filesize], [Duration], [Width], [Height], [DisplayAspectRatio], [ScanType] FROM [Files] WHERE [Files].[Film_FilmId]=") + std::to_wstring(cmd.FilmId)).c_str()))) MessageBox(0, L"Failed", 0, MB_ICONERROR);
		while (fc.MoveNext() == S_OK)
		{
			File ff;
			ff.Filename = fc.Filename;
			ff.Filesize = fc.Filesize;
			ff.Duration = fc.Duration;
			ff.Width = fc.Width;
			ff.Height = fc.Height;
			ff.DisplayAspectRatio = fc.DisplayAspectRatio;
			ff.ScanType = fc.ScanType;
			f.Files.push_back(ff);
		}
		fc.Close();

		if (FAILED(fp.Open(ss, (std::wstring(L"SELECT [Filename] FROM [Posters] WHERE [Posters].[Film_FilmId]=") + std::to_wstring(cmd.FilmId)).c_str()))) MessageBox(0, L"Failed", 0, MB_ICONERROR);
		if (fp.MoveNext() == S_OK) f.Poster = fp.Filename;
		fp.Close();

		f.UpdateQuality();

		films.push_back(f);
	}

	cmd.Close();
	cmd.ReleaseCommand();

	ss.Close();
	ds.Close();
}

void database::populate()
{
	films.clear();

	//MSXML2::IXMLDOMDocumentPtr xml;
	//if (SUCCEEDED(xml.CreateInstance(__uuidof(MSXML2::DOMDocument60), nullptr, CLSCTX_INPROC_SERVER)))
	//{
	//	try
	//	{
	//		xml->async = VARIANT_FALSE;
	//		xml->validateOnParse = VARIANT_FALSE;
	//		xml->resolveExternals = VARIANT_FALSE;

	//		if (xml->load(L"C:\\Users\\Hani\\Desktop\\film.xml") == VARIANT_TRUE)
	//		{
	//			auto pnl = xml->selectNodes(L"/Films/Store//Film");
	//			if (pnl)
	//			{
	//				for (long i = 0; i < pnl->length; i++)
	//				{
	//					films.push_back(Film(pnl->item[i]));
	//				}
	//			}
	//		}
	//	}
	//	catch (_com_error error)
	//	{
	//		MessageBox(0, L"Some error occured during data import", L"Film", MB_ICONERROR);
	//	}
	//}

	OpenAtlOledb(films);

	sort(0, true);
	//MessageBox(0, (std::to_wstring(films.size()) + L" films loaded").c_str(), L"Film", MB_ICONINFORMATION);
}

std::vector<Film*> database::get()
{
	if (!films.size()) populate();
	return all_films;
}

std::vector<Film*> database::get(const std::wstring query)
{
	std::vector<Film*> found;

	if (!films.size()) populate();

	for (auto& f : films)
	{
		//if (f.SortKey.find(q) != std::string::npos) found.push_back(&f);
		if (FindNLSStringEx(LOCALE_NAME_SYSTEM_DEFAULT, LINGUISTIC_IGNORECASE | LINGUISTIC_IGNOREDIACRITIC | NORM_IGNORESYMBOLS, f.SearchName.c_str(), -1, query.c_str(), -1, nullptr, NULL, NULL, 0) != -1) found.push_back(&f);
	}

	return found;
}

void database::sort(int col, bool asc)
{
	// this is wrong. The mapping from arbitrary col index to actual property names must be handled in UI

	//std::map<std::pair<int, bool>, std::function<int(Film&, Film&)>> sort = {
	//	{ { 0, true  }, [](Film& a, Film& b) { return CompareStringEx(LOCALE_NAME_SYSTEM_DEFAULT, LINGUISTIC_IGNORECASE | LINGUISTIC_IGNOREDIACRITIC | SORT_DIGITSASNUMBERS, a.Name.c_str(), -1, b.Name.c_str(), -1, nullptr, nullptr, 0) < 2; } },
	//	{ { 0, false }, [](Film& a, Film& b) { return CompareStringEx(LOCALE_NAME_SYSTEM_DEFAULT, LINGUISTIC_IGNORECASE | LINGUISTIC_IGNOREDIACRITIC | SORT_DIGITSASNUMBERS, a.Name.c_str(), -1, b.Name.c_str(), -1, nullptr, nullptr, 0) > 2; } },
	//	{ { 1, true  }, [](Film& a, Film& b) { return a.ReleaseDate < b.ReleaseDate; } },
	//	{ { 1, false }, [](Film& a, Film& b) { return a.ReleaseDate > b.ReleaseDate; } },
	//	{ { 2, true  }, [](Film& a, Film& b) { return a.AspectRatio.Ratio > b.AspectRatio.Ratio; } },
	//	{ { 2, false }, [](Film& a, Film& b) { return a.AspectRatio.Ratio < b.AspectRatio.Ratio; } },
	//	{ { 3, true  }, [](Film& a, Film& b) { return a.Resolution.Width > b.Resolution.Width; } },
	//	{ { 3, false }, [](Film& a, Film& b) { return a.Resolution.Width < b.Resolution.Width; } },
	//	{ { 4, true  }, [](Film& a, Film& b) { return a.Source < b.Source; } },
	//	{ { 4, false }, [](Film& a, Film& b) { return a.Source > b.Source; } },
	//};

	//std::sort(films.begin(), films.end(), sort[{col, asc}]);

	//For stable sort, preserving previous sorts (is slow in debug)
	std::map<std::pair<int, bool>, std::function<int(const Film, const Film)>> sort = {
		{ { 0, true  }, [](const Film a, const Film b) { return CompareStringEx(LOCALE_NAME_SYSTEM_DEFAULT, LINGUISTIC_IGNORECASE | LINGUISTIC_IGNOREDIACRITIC | SORT_DIGITSASNUMBERS, a.Name.c_str(), -1, b.Name.c_str(), -1, nullptr, nullptr, 0) < 2; } },
		{ { 0, false }, [](const Film a, const Film b) { return CompareStringEx(LOCALE_NAME_SYSTEM_DEFAULT, LINGUISTIC_IGNORECASE | LINGUISTIC_IGNOREDIACRITIC | SORT_DIGITSASNUMBERS, a.Name.c_str(), -1, b.Name.c_str(), -1, nullptr, nullptr, 0) > 2; } },
		{ { 1, true  }, [](const Film a, const Film b) { return a.ReleaseDate < b.ReleaseDate; } },
		{ { 1, false }, [](const Film a, const Film b) { return a.ReleaseDate > b.ReleaseDate; } },
		{ { 2, true  }, [](const Film a, const Film b) { return a.AspectRatio.Ratio > b.AspectRatio.Ratio; } },
		{ { 2, false }, [](const Film a, const Film b) { return a.AspectRatio.Ratio < b.AspectRatio.Ratio; } },
		{ { 3, true  }, [](const Film a, const Film b) { return a.Resolution.Width > b.Resolution.Width; } },
		{ { 3, false }, [](const Film a, const Film b) { return a.Resolution.Width < b.Resolution.Width; } },
		{ { 4, true  }, [](const Film a, const Film b) { return a.Source < b.Source; } },
		{ { 4, false }, [](const Film a, const Film b) { return a.Source > b.Source; } },
		{ { 5, true  }, [](const Film a, const Film b) { return a.DownloadStatus < b.DownloadStatus; } },
		{ { 5, false }, [](const Film a, const Film b) { return a.DownloadStatus > b.DownloadStatus; } },
	};

	std::stable_sort(films.begin(), films.end(), sort[{col, asc}]);


	all_films.clear();
	for (auto& f : films) all_films.push_back(&f);
}

void database::save(const int FilmId, const int Rating, const int DownloadStatus)
{
	CComPtr<IDBInitialize> db;
	if (FAILED(db.CoCreateInstance(CLSID_SQLSERVERCE))) MessageBox(0, L"Failed CoCreateInstance", 0, MB_ICONERROR);

	// Set data source property
	{
		DBPROP rgProps[1];
		rgProps[0].dwPropertyID = DBPROP_INIT_DATASOURCE;
		rgProps[0].dwOptions = DBPROPOPTIONS_REQUIRED;
		rgProps[0].vValue.vt = VT_BSTR;
		rgProps[0].vValue.bstrVal = SysAllocString(database_filename);
		//if (!(rgProps[0].vValue.bstrVal)) { hr = E_OUTOFMEMORY; goto Exit; }

		DBPROPSET rgPropSets[1];
		rgPropSets[0].rgProperties = rgProps;
		rgPropSets[0].cProperties = sizeof(rgProps) / sizeof(DBPROP);
		rgPropSets[0].guidPropertySet = DBPROPSET_DBINIT;

		if (FAILED(CComQIPtr<IDBProperties>(db)->SetProperties(sizeof(rgPropSets) / sizeof(DBPROPSET), rgPropSets))) MessageBox(0, L"Failed SetProperties", 0, MB_ICONERROR);
	}

	if (FAILED(db->Initialize())) MessageBox(0, L"Failed Initialize", 0, MB_ICONERROR);

	// Use command to open row set
	{
		CComPtr<IDBCreateCommand> db_cc;
		if (FAILED(CComQIPtr<IDBCreateSession>(db)->CreateSession(nullptr, IID_IDBCreateCommand, (IUnknown**)&db_cc))) MessageBox(0, L"Failed CreateSession", 0, MB_ICONERROR);

		CComPtr<ICommandText> cmd;
		if (FAILED(db_cc->CreateCommand(nullptr, __uuidof(cmd), (IUnknown**)&cmd))) MessageBox(0, L"Failed CreateCommand", 0, MB_ICONERROR);
		if (FAILED(cmd->SetCommandText(DBGUID_DEFAULT, (std::wstring(L"UPDATE [Films] SET [Rating]=") + std::to_wstring(Rating) + L", [DownloadStatus]=" + std::to_wstring(DownloadStatus) + L" WHERE [FilmId]=" + std::to_wstring(FilmId)).c_str())))
			MessageBox(0, L"Failed SetCommandText", 0, MB_ICONERROR);

		DBROWCOUNT count;
		if (FAILED(cmd->Execute(nullptr, IID_NULL, nullptr, &count, nullptr))) MessageBox(0, L"Failed Execute", 0, MB_ICONERROR);

		//MessageBox(0, (std::to_wstring(count) + L" rows affected!").c_str(), 0, 0);
	}
}

