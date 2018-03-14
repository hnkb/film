#pragma once

#include "model/film.h"


ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

void UpdateList();
void EscapeProc();

void InitListView();
void PopulateListView();
LRESULT ListViewNotify(const LPNMHDR lParam);
std::map<int, int> GenerateMap();

void Rate(HWND parent, Film* film);
