#ifndef LIB_DIRECTORYHELPER_H_
#define LIB_DIRECTORYHELPER_H_

#include <list>
#include <string>

/**
* Checks, if dir is the path+name of a valid directory
*/
bool is_valid_directory(const char* dir);

/**
 * Checks, if file exists and is readable
 */
bool is_valid_file(const char* file);

/**
 * Check, if fullString ends with ending
 */
bool hasEnding(std::string const& fullString, std::string const& ending);

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
std::list<std::pair<std::string, std::string>> only_ktests_triggering_errors(
    std::list<std::string> filelist);


/**
* Lists all pairs of error with corresponding ktest file from a directory
*/
std::list<std::pair<std::string, std::string>> errors_and_ktests_from_dir(
    std::string dir);

#endif  // LIB_DIRECTORYHELPER_H_
