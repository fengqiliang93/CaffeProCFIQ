#include "ScanNSortDirectory.h"
#if defined( WIN32 ) || defined(WIN64)
using namespace std;
#include <windows.h>

int str_compare(const void *arg1, const void *arg2)
{
	return strcmp((*(std::string*)arg1).c_str(), (*(std::string*)arg2).c_str());
}

file_lists ScanNSortDirectory(const std::string &path, const std::string &extension)
{
	WIN32_FIND_DATA wfd;
	HANDLE hHandle;
	string searchPath, searchFile;
	file_lists vFilenames;
	int nbFiles = 0;

	searchPath = path + "\\*" + extension;
	hHandle = FindFirstFile(searchPath.c_str(), &wfd);
	if (INVALID_HANDLE_VALUE == hHandle)
	{
		fprintf(stderr, "ERROR(%s, %d): Cannot find (*.%s)files in directory %s\n",
			__FILE__, __LINE__, extension.c_str(), path.c_str());
		exit(0);
	}
	do
	{
		//. or ..
		if (wfd.cFileName[0] == '.')
		{
			continue;
		}
		// if exists sub-directory
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			continue;
		}
		else//if file
		{
			searchFile = path + "\\" + wfd.cFileName;
			vFilenames.push_back(searchFile);
			nbFiles++;
		}
	}while (FindNextFile(hHandle, &wfd));

	FindClose(hHandle);

	// sort the filenames
	qsort((void *)&(vFilenames[0]), (size_t)nbFiles, sizeof(string), str_compare);

	return vFilenames;
}

file_lists ScanNSortDirectory( const std::string &searchdir )
{
	WIN32_FIND_DATA wfd;
	HANDLE hHandle;
	string searchPath, searchFile;
	file_lists vFilenames;
	int nbFiles = 0;

	searchPath = searchdir;//path + "\\*" + extension;
	string path = searchPath.substr( 0, searchPath.rfind('\\'));
	hHandle = FindFirstFile(searchPath.c_str(), &wfd);
	if (INVALID_HANDLE_VALUE == hHandle)
	{
		fprintf(stderr, "ERROR(%s, %d): Cannot find files in searchpath %s\n",
			__FILE__, __LINE__, searchPath.c_str());
		return vFilenames;
		//exit(0);
	}
	do
	{
		//. or ..
		if (wfd.cFileName[0] == '.')
		{
			continue;
		}
		// if exists sub-directory
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			continue;
		}
		else//if file
		{
			searchFile = path + "\\" + wfd.cFileName;
			vFilenames.push_back(searchFile);
			nbFiles++;
		}
	}while (FindNextFile(hHandle, &wfd));

	FindClose(hHandle);

	// sort the filenames
	qsort((void *)&(vFilenames[0]), (size_t)nbFiles, sizeof(string), str_compare);

	return vFilenames;
}
#endif

#ifdef linux
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <fnmatch.h>
#include <algorithm>

file_lists ScanNSortDirectory(const std::string &path, const std::string &extension){
	char infile[500];
	struct dirent *ptr;    
	DIR *dir;
	file_lists vFilenames;
	dir=opendir(path.c_str());
	if (dir == NULL)
	{
		fprintf(stderr, "ERROR(%s, %d): Cannot open directory %s\n",
			__FILE__, __LINE__, path.c_str());
		return vFilenames;
	}
	while((ptr=readdir(dir))!=NULL)
	{
		// if the extname is same as input 
		if(ptr->d_name[0] == '.')
			continue;
		char *pos = strrchr( ptr->d_name, '.' );
		if( !pos || 0 != strcmp( pos+1, extension.c_str() )){
			continue;
		}

		sprintf(infile,"%s/%s", path.c_str(), ptr->d_name);
		vFilenames.push_back(infile);
	}
	closedir(dir);
	std::sort(vFilenames.begin(), vFilenames.end());
	return vFilenames;
}

file_lists ScanNSortDirectory( const std::string &searchdir ){
	char infile[500], temp[500], *cpos;
	struct dirent *ptr;    
	DIR *dir;
	file_lists vFilenames;
	strcpy( temp, searchdir.c_str() );
	cpos = strrchr( temp, '/' );
	if (cpos == NULL)
	{
		fprintf(stderr, "ERROR(%s, %d): Invalid search path %s\n",
			__FILE__, __LINE__, searchdir.c_str());
		return vFilenames;
	}
	*cpos = '\0';
	dir=opendir(temp);
	if (dir == NULL)
	{
		fprintf(stderr, "ERROR(%s, %d): Cannot open directory %s\n",
			__FILE__, __LINE__, temp);
		return vFilenames;
	}
	while((ptr=readdir(dir))!=NULL)
	{
		// if the extname is same as input 
		if(ptr->d_name[0] == '.')
			continue;
		sprintf(infile,"%s/%s", temp, ptr->d_name);
		if( 0 != fnmatch( searchdir.c_str(), infile, 0 ) ){
			continue;
		}

		vFilenames.push_back(infile);
	}
	closedir(dir);
	std::sort(vFilenames.begin(), vFilenames.end());
	return vFilenames;
}
#endif
