#include "DirectoryHelper.h"
#include <dirent.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <utility>


bool is_valid_directory(const char* dir) {
  struct stat stbuf;
  return stat(dir, &stbuf) == 0 && S_ISDIR(stbuf.st_mode);
}


bool is_valid_file(const char* file) {
  std::ifstream ifs(file);
  return ifs.good();
}


bool hasEnding(std::string const& fullString, std::string const& ending) {
  if (fullString.length() >= ending.length()) {
    return (0 ==
            fullString.compare(fullString.length() - ending.length(),
                               ending.length(), ending));
  } else {
    return false;
  }
}


std::list<std::string> all_files_in_directory(const char* dir) {
  // Prepare the memory for the results
  std::list<std::string> result = {};
  DIR* dirp;
  struct dirent* dp;

  if ((dirp = opendir(dir)) == NULL) {
    return result;
  }

  do {
    if ((dp = readdir(dirp)) != NULL) {
      result.emplace_back(dp->d_name);
    }
  } while (dp != NULL);

  closedir(dirp);

  result.sort();
  return result;
}


std::string corresponding_ktest(std::string errorfile) {
  return errorfile.substr(0, errorfile.find(".")) + ".ktest";
}


std::string join(std::string directory, std::string filename) {
  if (directory.back() != '/') {
    directory += '/';
  }
  return directory + filename;
}


std::list<std::pair<std::string, std::string>> only_ktests_triggering_errors(
    std::list<std::string> filelist) {
  std::list<std::pair<std::string, std::string>> result = {};

  for (auto& elem : filelist) {
    if (hasEnding(elem, "ptr.err") || hasEnding(elem, "free.err") ||
        hasEnding(elem, "assert.err") || hasEnding(elem, "div.err") ||
        hasEnding(elem, "macke.err")) {
      result.emplace_back(make_pair(elem, corresponding_ktest(elem)));
    }
  }
  return result;
}

std::list<std::pair<std::string, std::string>> errors_and_ktests_from_dir(
    std::string dir) {
  std::list<std::pair<std::string, std::string>> result = {};

  for (auto& elem :
       only_ktests_triggering_errors(all_files_in_directory(dir.c_str()))) {
    result.emplace_back(
        make_pair(join(dir, elem.first), join(dir, elem.second)));
  }

  return result;
}
