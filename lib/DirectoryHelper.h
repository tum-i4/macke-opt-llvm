#ifndef LIB_DIRECTORYHELPER_H_
#define LIB_DIRECTORYHELPER_H_

#include <list>
#include <string>

/**
* Checks, if dir is the path+name of a valid directory
*/
bool is_valid_directory(const char* dir);

/**
* Get a list of all files inside a directory
*/
std::list<std::string> all_files_in_directory(const char* dir);

/**
* Converts the name of an error file to the correspondig .ktest file
*/
std::string corresponding_ktest(std::string errorfile);

/**
* joins a directory and a filename to a full name with path and file
*/
std::string join(std::string directory, std::string filename);

/**
* Filters list of filenames and only returns .ktest files with a corresponding
* error report file
*/
std::list<std::string> only_ktests_triggering_errors(
    std::list<std::string> filelist);


/**
* Lists all ktest files with a corresponding error report file from a directory
*/
std::list<std::string> error_ktests_from_dir(std::string dir);

#endif  // LIB_DIRECTORYHELPER_H_
