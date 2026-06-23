#pragma once

# include<string>
# include<vector>

#ifndef file_lists
typedef std::vector<std::string> file_lists;
// 扩展名不要带'.'
file_lists ScanNSortDirectory(const std::string &path, const std::string &extension);
file_lists ScanNSortDirectory( const std::string &searchdir );
#endif
