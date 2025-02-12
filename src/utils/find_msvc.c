#include "utils/common.h"

#ifdef _MSC_VER

#include "utils/find_msvc.h"

#include <stdio.h>

#include "utils/dirent.h"

#define MSVC_BASE_PATH "/Program Files (x86)/Microsoft Visual Studio/"
#define WINKIT_BASE_PATH "/Program Files (x86)/Windows Kits/"


int is_numeric(const struct dirent *ent)
{
	return atoi(ent->d_name); // don't use this function if you expect a file named "0" ?
}

char *get_highest_ver(char *directory, int (*filter)(const struct dirent *))
{
	struct dirent **files;
	int num_files = scandir(directory, &files, filter, versionsort);
	if (num_files < 0)
	{
		error_exit("ERROR - Failed to autodetect MSVC libpaths\n");
	}
	char *path_ret = (char *)malloc(260);
	strcpy_s(path_ret, 260, files[num_files - 1]->d_name);
	for (int i = 0; i < num_files; i++) free(files[i]);
	free(files);
	return path_ret;
}

/**
 * @returns PathPair containing paths to .../MSVC/[version]/lib/x64 and .../MSVC/[v]/atlmfc/lib/x64
 */
PathPair get_latest_available_vs_path()
{
	char ver_name[16] = "";

	char *highest_ver = get_highest_ver(MSVC_BASE_PATH, is_numeric);
	strncpy_s(ver_name, 16, highest_ver, 4);
	free(highest_ver);

	char newpath[260];
	snprintf(newpath, 260, "%s%s/BuildTools/VC/Tools/MSVC/", MSVC_BASE_PATH, ver_name);

	highest_ver = get_highest_ver(newpath, NULL);
	strcat_s(newpath, 260, highest_ver);
	free(highest_ver);

	PathPair ret = { 0 };
	snprintf(ret.first, 260, "%s/lib/x64", newpath);
	snprintf(ret.second, 260, "%s/atlmfc/lib/x64", newpath);

	return ret;
}

/**
 * @returns PathPair containing paths to /Program Files (x86)/Windows Kits/[version]/Lib/[version]/ucrt/x64 and .../[version]/um/x64
 */
PathPair find_winkit_path()
{
	// windows version
	char win_ver_major[16] = "";
	char *highest_ver = get_highest_ver(WINKIT_BASE_PATH, is_numeric);
	strcpy_s(win_ver_major, 16, highest_ver);
	free(highest_ver);

	// windows kit version? or something
	char newpath[260] = "";
	sprintf_s(newpath, 260, "%s%s/Lib/", WINKIT_BASE_PATH, win_ver_major);
	highest_ver = get_highest_ver(newpath, NULL);
	strcat_s(newpath, 260, highest_ver);
	free(highest_ver);

	PathPair ret = { 0 };
	snprintf(ret.first, 260, "%s/ucrt/x64", newpath);
	snprintf(ret.second, 260, "%s/um/x64", newpath);

	return ret;
}

#endif //defined(_MSC_VER)