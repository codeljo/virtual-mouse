#pragma once
#ifndef FILES_H
#define FILES_H

#include <algorithm>
#include <filesystem>
#include <vector>

class Files {
public:

	static bool exists(std::string_view path) {
		return std::filesystem::exists(path);
	}

	static std::vector<std::string> get_files(std::string_view path, bool sort=true) {
		std::vector<std::string> items;
		for (const auto& item : std::filesystem::directory_iterator{path}) {
			if (!item.is_directory()) {
				items.push_back(item.path().string());
			}
		}
		if (sort) { std::sort(items.begin(), items.end()); }
		return items;
	}

};

#endif  /* FILES_H */