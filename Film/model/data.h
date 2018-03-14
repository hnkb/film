#pragma once

#include "film.h"

#include <vector>

class database
{
public:
	std::vector<Film*> get();
	std::vector<Film*> get(const std::wstring query);
	void sort(int col, bool asc);
	static void save(const int FilmId, const int Rating, const int DownloadStatus);

private:
	std::vector<Film> films;
	std::vector<Film*> all_films;
	void populate();
};
