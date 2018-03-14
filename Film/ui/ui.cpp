
#include "stdafx.h"
#include "ui.h"
#include "resource.h"
#include "model/data.h"
#include "image.h"


database db;
std::vector<Film*> films;
HFONT font;
HWND edit, listview;
#define ID_EDIT 1

int SortColumn = 0;
bool SortAscending = true;


ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAIN));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;// MAKEINTRESOURCEW(IDC_FILM);
	wcex.lpszClassName = L"FilmWndCls";
	wcex.hIconSm = NULL;// LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	films = db.get();


	// create default font

	LOGFONT lf = { 0 };
	//lf.lfHeight = -13;
	lf.lfHeight = -18;
	lf.lfWeight = FW_THIN;
	lstrcpy(lf.lfFaceName, L"Segoe UI");
	font = CreateFontIndirect(&lf);


	// create main window
	
	HWND hWnd = CreateWindowW(L"FilmWndCls", L"Film", WS_OVERLAPPEDWINDOW, 800, 200, 800, 600, nullptr, nullptr, hInstance, nullptr);
	if (!hWnd) return FALSE;


	// create edit control

	edit = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL /*| WS_BORDER*/, 0, 0, 300, 32, hWnd, (HMENU)ID_EDIT, hInstance, nullptr);
	SendMessage(edit, WM_SETFONT, (WPARAM)font, 0);


	// create list view

	INITCOMMONCONTROLSEX icex;
	icex.dwICC = ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&icex);

	listview = CreateWindow(WC_LISTVIEW, L"", WS_CHILD | WS_VISIBLE | LVS_REPORT| LVS_SHOWSELALWAYS, 0, 32, 784, 521, hWnd, nullptr, hInstance, nullptr);
	InitListView();
	PopulateListView();


	// show window

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	SetFocus(edit);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmEvent = HIWORD(wParam);

		switch (LOWORD(wParam))
		{
		case ID_EDIT:
			if (wmEvent == EN_CHANGE) UpdateList();

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		//HFONT old = (HFONT)SelectObject(hdc, font);

		//std::wstring s = std::to_wstring(films.size()) + L" films found.";
		//TextOut(hdc, 10, 70, s.c_str(), (int)s.size());

		//int y = 100;
		//for (auto &f : films)
		//{
		//	s = f->Name + L" (" + f->Date.substr(0, 4) + L")" + L"--"+std::to_wstring( f->Rating);
		//	TextOut(hdc, 10, y, s.c_str(), (int)s.size());
		//	if ((y += 20) > ps.rcPaint.bottom) break;
		//}

		//SelectObject(hdc, old);
		EndPaint(hWnd, &ps);
	}
	break;

	case WM_SIZE:
	{
		RECT r;
		GetClientRect(hWnd, &r);
		MoveWindow(listview, r.left, r.top + 32, r.right - r.left, r.bottom - r.top - 40, TRUE);
		MoveWindow(edit, r.left + 0, r.top + 0, r.right - r.left - 200, 32, TRUE);
	}
	break;

	case WM_NOTIFY:
		if (((LPNMHDR)lParam)->hwndFrom == listview) return ListViewNotify((LPNMHDR)lParam);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void UpdateList()
{
	WCHAR txt[100];
	GetWindowText(edit, txt, 100);
	std::wstring s = txt;

	films = s.size() ? db.get(s) : db.get();
	/*InvalidateRect(hWnd, nullptr, TRUE);*/
	PopulateListView();
}

void EscapeProc()
{
	if (GetFocus() == edit)
	{
		WCHAR text[100];
		GetWindowText(edit, text, 100);
		if (text[0])
		{
			SetWindowText(edit, L"");
		}
		else
		{
			SendMessage(GetParent(edit), WM_CLOSE, 0, 0);
		}
	}
	else
	{
		SetFocus(edit);
	}
}


LRESULT ListViewNotify(const LPNMHDR lParam)
{
	switch (lParam->code)
	{
	case LVN_GETDISPINFO:
	{
		NMLVDISPINFO* plvdi = (NMLVDISPINFO*)lParam;
		switch (plvdi->item.iSubItem)
		{
		case 0: StringCchCopy(plvdi->item.pszText, plvdi->item.cchTextMax, films[plvdi->item.iItem]->Name.c_str()); return TRUE;
		case 1: StringCchCopy(plvdi->item.pszText, plvdi->item.cchTextMax, films[plvdi->item.iItem]->DisplayReleaseDate.c_str()); return TRUE;
		case 2: StringCchCopy(plvdi->item.pszText, plvdi->item.cchTextMax, films[plvdi->item.iItem]->AspectRatio.Name.c_str()); return TRUE;
		case 3: StringCchCopy(plvdi->item.pszText, plvdi->item.cchTextMax, films[plvdi->item.iItem]->Resolution.Name.c_str()); return TRUE;
		case 4: StringCchCopy(plvdi->item.pszText, plvdi->item.cchTextMax, films[plvdi->item.iItem]->Source.c_str()); return TRUE;
		case 5: StringCchCopy(plvdi->item.pszText, plvdi->item.cchTextMax, films[plvdi->item.iItem]->DownloadStatus.c_str()); return TRUE;
		}
		return TRUE;
	}

	case LVN_COLUMNCLICK:
	{
		int col = ((NMLISTVIEW*)lParam)->iSubItem;
		if (SortColumn == col)
			SortAscending = !SortAscending;
		else
		{
			SortColumn = col;
			SortAscending = true;
		}
		db.sort(SortColumn, SortAscending);
		UpdateList();
		break;
	}

	case LVN_ITEMCHANGED:
	{
		int i = ((LPNMLISTVIEW)lParam)->iItem;
		films[i]->Selected = ListView_GetItemState(listview, i, LVIS_SELECTED) != 0;
		break;
	}

	case NM_DBLCLK:
	{
		int i = ((LPNMITEMACTIVATE)lParam)->iItem;
		if (i > -1) films[i]->Play();
		break;
	}

	case NM_RCLICK:
	{
		int i = ((LPNMITEMACTIVATE)lParam)->iItem;
		if (i > -1)
		{
			POINT pt = ((LPNMITEMACTIVATE)lParam)->ptAction;
			ClientToScreen(listview, &pt);

			HMENU menu = CreatePopupMenu();
			AppendMenu(menu, MF_STRING, 1, L"&Play");
			AppendMenu(menu, MF_STRING, 2, L"&Open location");
			AppendMenu(menu, MF_SEPARATOR, 0, nullptr);
			if (films[i]->Wikipedia.size()) AppendMenu(menu, MF_STRING, 3, L"&Wikipedia");
			if (films[i]->IMDb.size()) AppendMenu(menu, MF_STRING, 4, L"&IMDb");
			AppendMenu(menu, MF_SEPARATOR, 0, nullptr);
			AppendMenu(menu, MF_STRING, 5, L"&Rate...");
			SetMenuDefaultItem(menu, 1, FALSE);

			switch (TrackPopupMenuEx(menu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_NONOTIFY | TPM_RETURNCMD | TPM_LEFTBUTTON, pt.x, pt.y, GetParent(listview), nullptr))
			{
			case 1: films[i]->Play(); break;
			case 2: films[i]->OpenLocation(); break;
			case 3: ShellExecute(nullptr, nullptr, films[i]->Wikipedia.c_str(), nullptr, nullptr, SW_SHOW); break;
			case 4: ShellExecute(nullptr, nullptr, (std::wstring(L"http://www.imdb.com/title/") + films[i]->IMDb + L"/").c_str(), nullptr, nullptr, SW_SHOW); break;
			case 5: Rate(GetParent(listview), films[i]); break;
			}

			DestroyMenu(menu);
		}
		break;
	}

	default:
		break;
	}

	return FALSE;
}

void InitListView()
{

	// Define columns

	LVCOLUMN col = {};
	col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM | LVCF_FMT;

	std::map<LPWSTR, std::pair<int, int>> cols = { {L"Name", { 300, LVCFMT_LEFT }}, { L"Release Date", { 80, LVCFMT_RIGHT } }, { L"Aspect ratio", { 80, LVCFMT_CENTER } }, { L"Resolution", { 110, LVCFMT_CENTER } }, { L"Source", { 110, LVCFMT_CENTER } },{ L"Download",{ 80, LVCFMT_CENTER } } };

	for (auto& c : cols)
	{
		col.pszText = c.first;
		col.cx = c.second.first;
		col.fmt = c.second.second;
		ListView_InsertColumn(listview, col.iSubItem++, &col);
	}


	// Define groups

	LVGROUP group = {};
	group.mask = LVGF_HEADER | LVGF_GROUPID | LVGF_STATE;
	group.cbSize = sizeof(LVGROUP);
	group.state = group.stateMask = LVGS_COLLAPSIBLE;
	group.pszHeader = L"Group";
	for (auto& n : { 0,1,50,60,100 }) { group.iGroupId = n; ListView_InsertGroup(listview, -1, &group); }

	ListView_SetExtendedListViewStyle(listview, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
	//ListView_EnableGroupView(listview, TRUE);
}

void UpdateGroups()
{
	LVGROUP group = {};
	group.mask = LVGF_HEADER;
	group.cbSize = sizeof(LVGROUP);
	group.pszHeader = new WCHAR[32];

	std::map<int, std::wstring> names = { { 0, L"New" },{ 1, L"Seen long ago" },{ 50, L"Undecided" },{ 60, L"Unacceptable" },{ 100, L"Good" } };
	auto counts = GenerateMap();
	for (auto& n : names)
	{
		StringCchCopy(group.pszHeader, 32, (n.second + L" (" + std::to_wstring(counts[n.first]) + +L" films)").c_str());
		ListView_SetGroupInfo(listview, n.first, &group);
	}

	delete group.pszHeader;
}

void PopulateListView()
{
	SendMessage(listview, WM_SETREDRAW, FALSE, 0);
	ListView_DeleteAllItems(listview);
	UpdateGroups();

	LVITEM i = {};
	i.mask = LVIF_TEXT | LVIF_GROUPID | LVIF_STATE;
	i.pszText = LPSTR_TEXTCALLBACK;
	for (i.iItem = 0; i.iItem < films.size(); i.iItem++)
	{
		i.state = i.stateMask = films[i.iItem]->Selected ? LVIS_SELECTED : 0;
		i.iGroupId = films[i.iItem]->Rating;
		ListView_InsertItem(listview, &i);
	}

	SendMessage(listview, WM_SETREDRAW, TRUE, 0);
}

std::map<int, int> GenerateMap()
{
	std::map<int, int> counts = { {0,0},{1,0},{50,0},{60,0},{100,0} };
	for (auto& f : films) counts[f->Rating]++;
	return counts;
}


int RateDownloadStatus;
int RateRating;

INT_PTR CALLBACK RateDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	std::map<std::wstring, int> DownloadStatus{ { L"None",0},{L"Queued",1},{L"Unavailable",2} };
	std::vector<std::pair<int, std::wstring>> Ratings { { 0, L"New" },{ 1, L"Seen long ago" },{ 50, L"Undecided" },{ 60, L"Unacceptable" },{ 100, L"Good" } };
	static HBITMAP poster = nullptr;

	if (message == WM_INITDIALOG)
	{
		Film* f = (Film*)lParam;

		SetWindowText(hDlg, (std::wstring(L"Rate ") + f->Name).c_str());
		SetDlgItemText(hDlg, IDC_NAME, f->Name.c_str());
		SetDlgItemText(hDlg, IDC_RELEASE_DATE, f->DisplayReleaseDate.c_str());
		SendDlgItemMessage(hDlg, IDC_NAME, WM_SETFONT, (WPARAM)font, 0);

		for (auto& s : DownloadStatus) SendDlgItemMessage(hDlg, IDC_QUALITY, CB_ADDSTRING, NULL, (LPARAM)s.first.c_str());
		SendDlgItemMessage(hDlg, IDC_QUALITY, CB_SETCURSEL, DownloadStatus[f->DownloadStatus], NULL);

		for (int i = 0; i < Ratings.size(); i++)
		{
			SendDlgItemMessage(hDlg, IDC_RATING, CB_ADDSTRING, NULL, (LPARAM)Ratings[i].second.c_str());
			if (Ratings[i].first == f->Rating) SendDlgItemMessage(hDlg, IDC_RATING, CB_SETCURSEL, i, NULL);
		}

		poster = LoadHBitmap(std::wstring(L"D:\\Collections\\Film\\[Posters]\\") + f->Poster);
		SendDlgItemMessage(hDlg, IDC_POSTER, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)poster);

		return TRUE;
	}
	else if (message == WM_COMMAND)
	{
		switch (LOWORD(wParam))
		{
		case IDOK:
			RateRating = Ratings[SendDlgItemMessage(hDlg, IDC_RATING, CB_GETCURSEL, 0, 0)].first;
			RateDownloadStatus = (int)SendDlgItemMessage(hDlg, IDC_QUALITY, CB_GETCURSEL, 0, 0);

		case IDCANCEL:
			DeleteObject(poster);
			EndDialog(hDlg, wParam);
			return TRUE;
		}
	}

	return FALSE;
}

void Rate(HWND parent, Film* film)
{
	LPCWSTR DownloadStatus[] { L"None", L"Queued", L"Unavailable" };

	if (DialogBoxParam(nullptr, MAKEINTRESOURCE(IDD_RATE), parent, RateDlgProc, (LPARAM)film) == IDOK)
	{
		film->DownloadStatus = DownloadStatus[RateDownloadStatus];
		film->Rating = RateRating;

		database::save(film->FilmId, RateRating, RateDownloadStatus);

		db.sort(SortColumn, SortAscending);
		UpdateList();
	}
}

