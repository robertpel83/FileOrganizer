// FileOrganizer.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <fstream>
#include <string>
#include <limits>
#include <stdexcept>
#include <vector>
#include <memory>
#include <regex>
#include <sstream>
#include <ctime>
#include <string>
#include <filesystem>
#include <chrono>
#include <unordered_set>
#include <stack>
#include <vector>
#include <iostream> // std::cout
#include <fstream>  // std::ifstream
#include <vector>   // std::vector
#include <iomanip>  // std::setprecision
#include <functional>


#include <sstream>
#include <cstdint>


#include "atlbase.h"
#include "atlstr.h"
#include "comutil.h"

#include <io.h>
#include <fcntl.h>

#include <dirent.h>

#include "crc32.h"
#include "md5.h"
#include "sha1.h"
#include "sha256.h"
#include "keccak.h"
#include "sha3.h"



using std::runtime_error;
using std::string;
using std::vector;
using std::wstring;


//#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#include <Shlobj.h> // SHGetKnownFolderPath
#include <clocale>  // std::setlocale 
#include <io.h>     // _setmode
#include <fcntl.h>  // _O_U16TEXT



#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <unistd.h>
#endif

//#ifdef WIN32
//#define wstat _wstat
//#define stat _stat
//#endif

#define _CRT_SECURE_NO_WARNINGS 1

#include "libexif/exif-data.h"
#include "libexif/exif-system.h"

#include "TinyEXIF.h"



class FileDataEntry
{
public:
    wstring nameAndPath;
    wstring path;
    wstring name;
    __int64 size = 0;
    __int64 createdTime = 0;
    __int64 modifiedTime = 0;
    wstring createdDateString;
    wstring modifiedDateString;
    wstring fileNameDateString;
    wstring crc32;
    wstring md5;
    wstring sha1;
    wstring sha2;
    wstring keccak;
    wstring sha3;



    bool operator < (const FileDataEntry& f) const
    {
        return (size < f.size);
    }
    bool operator > (const FileDataEntry& f) const
    {
        return (size > f.size);
    }
    
};
vector<FileDataEntry> fileDataEntries;

std::chrono::steady_clock::time_point start;
bool isWindows = false;



std::string convertWideToANSI(const std::wstring& wstr)
{
    int count = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), (int)wstr.length(), NULL, 0, NULL, NULL);
    std::string str(count, 0);
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
    return str;
}

std::wstring convertAnsiToWide(const std::string& str)
{
    int count = MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.length(), NULL, 0);
    std::wstring wstr(count, 0);
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.length(), &wstr[0], count);
    return wstr;
}

std::string convertWideToUtf8(const std::wstring& wstr)
{
    int count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), NULL, 0, NULL, NULL);
    std::string str(count, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
    return str;
}

std::wstring convertUtf8ToWide(const std::string& str)
{
    int count = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), NULL, 0);
    std::wstring wstr(count, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), &wstr[0], count);
    return wstr;
}

std::wstring convertStringToWStringUsingFilesystem(std::string s)
{
    return std::filesystem::path(s).wstring();
}

std::wstring convertWStringToStringUsingFilesystem(std::wstring w)
{
    return std::filesystem::path(w).wstring();
}


//std::unordered_set<wstring> recursiveDirectoryIteratorIncrementPaths;
//for whatever reason, recursive_directory_iterator cannot read "All Users" folder, but directory_iterator can. weird.
void recursiveDirectoryIteratorIncrement(const wstring startpath, _int64& filecount)
{
    std::error_code ec;
    std::filesystem::recursive_directory_iterator v = std::filesystem::recursive_directory_iterator(startpath, std::filesystem::directory_options::skip_permission_denied, ec);
   
    //iterator based iteration
    for (;
        v != end(v); //end(v) is equal to default constructor std::filesystem::recursive_directory_iterator end_itr;
        v.increment(ec)
        )
    {
        // if the current index is needed:
        //std::filesystem::recursive_directory_iterator::difference_type i = std::distance(begin(v), it);

        if (ec)
        {
            std::wcout << L"Increment error " << convertUtf8ToWide(ec.message()) << std::endl;
            ec.clear();
        }
        else
        {
            try {
                // access element as *it
                if ((*v).is_regular_file()) 
                { 
                    filecount++; 

                    FileDataEntry f;
                    f.nameAndPath = (*v).path().wstring();
                    f.name = (*v).path().filename().wstring();
                    f.path = (*v).path().parent_path().wstring();
                    fileDataEntries.push_back(f);

                }
                //if ((*v).is_directory())
                //{
                    //recursiveDirectoryIteratorIncrementPaths.insert((*v).path().wstring());
                //}
            }
            catch (std::filesystem::filesystem_error& e)
            {
                std::error_code x = e.code();
                //std::wcout << L"Filesystem error " << ConvertUtf8ToWide(e.what()) << std::endl;
            }
        }
    }
}




void directoryIteratorRecursive(const std::filesystem::path& dir_path,_int64 &filecount)
{
    if (!exists(dir_path)) 
    { 
        std::wcout << dir_path.c_str() << L" does not exist" << std::endl;
        return; 
    }

    std::stack<wstring> directories;

    directories.push(dir_path);

    while (!directories.empty())
    {
        wstring path = directories.top();

        directories.pop();

        std::error_code ec;
        std::filesystem::directory_iterator itr(path, std::filesystem::directory_options::skip_permission_denied, ec);

        for (;
            itr != end(itr);  //end(itr) is equal to default constructor std::filesystem::recursive_directory_iterator end_itr;
            itr.increment(ec))
        {
            if (ec)
            {
                std::wcout << L"Increment error " << convertUtf8ToWide(ec.message()) << std::endl;
                ec.clear();
            }
            else
            {
                try {
                    if (is_directory(itr->status()))
                    {
                        directories.push(itr->path().wstring());
                    }
                    else if (is_regular_file(itr->status()))//.type() == std::filesystem::file_type::regular) //!= std::filesystem::file_type::not_found)//if (itr->path().filename() == file_name) // see below
                    {
                        filecount++;

                        FileDataEntry f;
                        f.nameAndPath = itr->path().wstring();
                        f.name = itr->path().filename().wstring();
                        f.path = path;
                        fileDataEntries.push_back(f);
                    }
                }
                catch (std::filesystem::filesystem_error& e)
                {
                    std::error_code x = e.code();
                    //std::wcout << L"Filesystem error " << ConvertUtf8ToWide(e.what()) << std::endl;
                }
            }
        }
    }
    
}


// Find files recursively 
void direntScanDirectory(const wstring startPath, _int64& filecount)//wchar_t* dirname
{
    std::stack<wstring> directories;
    directories.push(startPath);

    while (!directories.empty())
    {
        wstring path = directories.top();
        directories.pop();

        _WDIR* dir = _wopendir(path.c_str());

        if (dir != NULL)
        {
            struct _wdirent* ent;

            while ((ent = _wreaddir(dir)) != NULL)
            {
                switch (ent->d_type)
                {
                case DT_REG:
                    {
                        filecount++;

                        FileDataEntry f;
                        f.nameAndPath = path + L"\\" + ent->d_name;
                        f.name = ent->d_name;
                        f.path = path;
                        fileDataEntries.push_back(f);
                    }
                    break;
                case DT_DIR:
                    // Scan sub-directory recursively
                    if (wcscmp(ent->d_name, L".") != 0
                        && wcscmp(ent->d_name, L"..") != 0)
                    {
                        wstring nextPath = path + wstring(L"/") + wstring(ent->d_name);
                        directories.push(nextPath);
                    }
                    break;

                default:
                    // Do not device entries
                    //NOP
                    ;
                }
            }
            //if(ent==NULL)std::wcout << L"Dirent Error ReadDir == NULL after " << dirname << L"/" << lastEntName << std::endl;

            wclosedir(dir);
        }

    }
}



//std::unordered_set<wstring> findFirstFilePaths;

void ListFilesWindowsFindFirstFile(const wstring originalPath, _int64 &filecount)
{

    HANDLE hFind = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA ffd;
    
    std::stack<wstring> directories;

    directories.push(originalPath);

    while (!directories.empty())
    {
        wstring path = directories.top();
        wstring pathWithTrailingDelimiterAndWildcard = path + L"\\*";
        directories.pop();

        hFind = FindFirstFileExW(pathWithTrailingDelimiterAndWildcard.c_str(), FindExInfoBasic, &ffd, FindExSearchNameMatch, NULL, FIND_FIRST_EX_LARGE_FETCH);
        if (hFind == INVALID_HANDLE_VALUE)
        {
            DWORD error = GetLastError();

            if (error == ERROR_ACCESS_DENIED);// std::wcout << L"FindFirstFileExW Error " << error << L" ERROR_ACCESS_DENIED for " << path << std::endl;
            else std::wcout << L"FindFirstFileExW Error " << error << L" for " << path <<std::endl;
        }
        else
        {
            do
            {
                if (wcscmp(ffd.cFileName, L".") != 0 &&
                    wcscmp(ffd.cFileName, L"..") != 0)
                {
                    if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
                        //&&
                        //(ffd.dwFileAttributes & FILE_ATTRIBUTE_DEVICE) == 0 &&
                        //(ffd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) == 0 &&
                        //(ffd.dwFileAttributes & FILE_ATTRIBUTE_SPARSE_FILE) == 0 &&
                        //(ffd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) == 0 &&
                        //(ffd.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY) == 0 &&
                        //(ffd.dwFileAttributes & FILE_ATTRIBUTE_VIRTUAL) == 0 &&
                        //(ffd.dwFileAttributes & FILE_ATTRIBUTE_OFFLINE) == 0 &&
                        //(ffd.dwFileAttributes & FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS) == 0 &&
                        //(ffd.dwFileAttributes & FILE_ATTRIBUTE_RECALL_ON_OPEN) == 0
                        //)
                    {
                        directories.push(path + L"\\" + ffd.cFileName);
                        //findFirstFilePaths.insert(path + L"\\" + ffd.cFileName);
                    }
                    else
                    {
                        filecount++;
                        FileDataEntry f;
                        f.nameAndPath = path + L"\\" + ffd.cFileName;
                        f.name = ffd.cFileName;
                        f.path = path;
                        fileDataEntries.push_back(f);
                    }
                }
            } while (FindNextFileW(hFind, &ffd) != 0);

            DWORD error = GetLastError();
            if (error != ERROR_SUCCESS && error != ERROR_NO_MORE_FILES)
            {
                std::wcout << L"FindNextFileW Error: " << error << std::endl;
            }

            FindClose(hFind);
            hFind = INVALID_HANDLE_VALUE;
        }
    }
}



__int64 getFileSizeWindows(FileDataEntry* f)//wstring fileNameAndPath)
{
    HANDLE hFile = CreateFileW(f->nameAndPath.c_str(), GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        DWORD error = GetLastError();
        if (error != ERROR_SUCCESS && error != ERROR_NO_MORE_FILES)
        {
            if (error == ERROR_PATH_NOT_FOUND)std::wcout << L"CreateFileW Error: " << error << L" ERROR_PATH_NOT_FOUND " << f->nameAndPath.c_str() << std::endl;
            else if (error == ERROR_ACCESS_DENIED)std::wcout << L"CreateFileW Error: " << error << L" ERROR_ACCESS_DENIED " << f->nameAndPath.c_str() << std::endl;
            else if (error == ERROR_SHARING_VIOLATION)std::wcout << L"CreateFileW Error: " << error << L" ERROR_SHARING_VIOLATION " << f->nameAndPath.c_str() << std::endl;
            else if (error == ERROR_CANT_ACCESS_FILE)std::wcout << L"CreateFileW Error: " << error << L" ERROR_CANT_ACCESS_FILE " << f->nameAndPath.c_str() << std::endl;
            else std::wcout << L"CreateFileW Error: " << error << L" " << f->nameAndPath.c_str() << std::endl;
        }
        return -1;
    }

    LARGE_INTEGER size;
    if (!GetFileSizeEx(hFile, &size))
    {
        DWORD error = GetLastError();
        if (error != ERROR_SUCCESS && error != ERROR_NO_MORE_FILES)
        {
            if (error == ERROR_PATH_NOT_FOUND)std::wcout << L"GetFileSizeEx Error: " << error << L" ERROR_PATH_NOT_FOUND " << f->nameAndPath.c_str() << std::endl;
            else if (error == ERROR_ACCESS_DENIED)std::wcout << L"GetFileSizeEx Error: " << error << L" ERROR_ACCESS_DENIED " << f->nameAndPath.c_str() << std::endl;
            else if (error == ERROR_SHARING_VIOLATION)std::wcout << L"GetFileSizeEx Error: " << error << L" ERROR_SHARING_VIOLATION " << f->nameAndPath.c_str() << std::endl;
            else if (error == ERROR_CANT_ACCESS_FILE)std::wcout << L"GetFileSizeEx Error: " << error << L" ERROR_CANT_ACCESS_FILE " << f->nameAndPath.c_str() << std::endl;
            else std::wcout << L"GetFileSizeEx Error: " << error << L" " << f->nameAndPath.c_str() << std::endl;
        }
        CloseHandle(hFile);
        return -1; // error condition, could call GetLastError to find out more
    }

    CloseHandle(hFile);

    f->size = size.QuadPart;
    return size.QuadPart;
}

BY_HANDLE_FILE_INFORMATION getFileInformationByHandleWindows(wstring fileNameAndPath)
{
    {
        HANDLE hFile = CreateFileW(fileNameAndPath.c_str(), GENERIC_READ,
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE)
        {
            DWORD error = GetLastError();
            if (error != ERROR_SUCCESS && error != ERROR_NO_MORE_FILES)
            {
                if (error == ERROR_PATH_NOT_FOUND)std::wcout << L"CreateFileW Error: " << error << L" ERROR_PATH_NOT_FOUND " << fileNameAndPath.c_str() << std::endl;
                else if (error == ERROR_ACCESS_DENIED)std::wcout << L"CreateFileW Error: " << error << L" ERROR_ACCESS_DENIED " << fileNameAndPath.c_str() << std::endl;
                else if (error == ERROR_SHARING_VIOLATION)std::wcout << L"CreateFileW Error: " << error << L" ERROR_SHARING_VIOLATION " << fileNameAndPath.c_str() << std::endl;
                else if (error == ERROR_CANT_ACCESS_FILE)std::wcout << L"CreateFileW Error: " << error << L" ERROR_CANT_ACCESS_FILE " << fileNameAndPath.c_str() << std::endl;
                else std::wcout << L"CreateFileW Error: " << error << L" " << fileNameAndPath.c_str() << std::endl;
            }
        }

        BY_HANDLE_FILE_INFORMATION handleFileInfo;
        GetFileInformationByHandle(hFile, &handleFileInfo);
        CloseHandle(hFile);

        return handleFileInfo;
        /*
            DWORD dwFileAttributes;
            FILETIME ftCreationTime;
            FILETIME ftLastAccessTime;
            FILETIME ftLastWriteTime;
            DWORD dwVolumeSerialNumber;
            DWORD nFileSizeHigh;
            DWORD nFileSizeLow;
            DWORD nNumberOfLinks;
            DWORD nFileIndexHigh;
            DWORD nFileIndexLow;
        */
    }
}

__int64 getFileSizeFromHandleFileInformationWindows(FileDataEntry* f)//wstring fileNameAndPath)
{
    BY_HANDLE_FILE_INFORMATION info = getFileInformationByHandleWindows(f->nameAndPath);
    __int64 max = MAXDWORD;
    max+=1;
    __int64 size = (info.nFileSizeHigh * (max)) + info.nFileSizeLow;
    f->size = size;
    return size;
}

WIN32_FILE_ATTRIBUTE_DATA getFileAttributesWindows(wstring fileNameAndPath)
{
    WIN32_FILE_ATTRIBUTE_DATA fileInfo;
    if (!GetFileAttributesExW(fileNameAndPath.c_str(), GetFileExInfoStandard, &fileInfo))
    {
        DWORD error = GetLastError();
        if (error != ERROR_SUCCESS && error != ERROR_NO_MORE_FILES)
        {
            if (error == ERROR_PATH_NOT_FOUND)std::wcout << L"GetFileAttributesExW Error: " << error << L" ERROR_PATH_NOT_FOUND " << fileNameAndPath.c_str() << std::endl;
            else if (error == ERROR_ACCESS_DENIED)std::wcout << L"GetFileAttributesExW Error: " << error << L" ERROR_ACCESS_DENIED " << fileNameAndPath.c_str() << std::endl;
            else if (error == ERROR_SHARING_VIOLATION)std::wcout << L"GetFileAttributesExW Error: " << error << L" ERROR_SHARING_VIOLATION " << fileNameAndPath.c_str() << std::endl;
            else if (error == ERROR_CANT_ACCESS_FILE)std::wcout << L"GetFileAttributesExW Error: " << error << L" ERROR_CANT_ACCESS_FILE " << fileNameAndPath.c_str() << std::endl;
            else std::wcout << L"GetFileAttributesExW Error: " << error << L" " << fileNameAndPath.c_str() << std::endl;
        }
    }
    /*
        DWORD dwFileAttributes;
        FILETIME ftCreationTime;
        FILETIME ftLastAccessTime;
        FILETIME ftLastWriteTime;
        DWORD nFileSizeHigh;
        DWORD nFileSizeLow;
    */
    return fileInfo;
}
__int64 getFileSizeFromFileAttributesWindows(FileDataEntry* f)//wstring fileNameAndPath)
{
    WIN32_FILE_ATTRIBUTE_DATA info = getFileAttributesWindows(f->nameAndPath);
    __int64 max = MAXDWORD;
        max += 1;
    __int64 size = (info.nFileSizeHigh * (max)) + info.nFileSizeLow;
    f->size = size;
    return size;
}

long long getFileSizeWStat(FileDataEntry* f)//std::wstring filename)
{
    struct _stat64 stat_buf;
    int rc = _wstat64(f->nameAndPath.c_str(), &stat_buf);
    long long size = rc == 0 ? stat_buf.st_size : -1;
    f->size = size;
    return size;
}


long long getFileSizeFStat(int _FileHandle)
{
    struct _stat64 stat_buf;
    int rc = _fstat64(_FileHandle, &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

uintmax_t getFileSizeFilesystem(FileDataEntry* f)//std::filesystem::path p)
{
    std::error_code ec;
    uintmax_t size = std::filesystem::file_size(std::filesystem::path(f->nameAndPath), ec);
    if (ec)
    {
        std::wcout << L"file_size error " << convertUtf8ToWide(ec.message()) << L" " << f->nameAndPath << std::endl;
        ec.clear();
    }
    return size;
}

uintmax_t getFileSizeFilesystemDirectoryEntry(FileDataEntry* f)//std::filesystem::directory_entry d)
{
    std::error_code ec;
    uintmax_t size = std::filesystem::directory_entry(f->nameAndPath).file_size(ec);
    if (ec)
    {
        std::wcout << L"file_size error " << convertUtf8ToWide(ec.message()) << L" " << f->nameAndPath << std::endl;
        ec.clear();
    }
    return size;
}


long long getFileSizeWithIfstream(wstring name)
{
    std::ifstream file;
    file.open(name, std::ios::in | std::ios::binary);
    file.ignore(LLONG_MAX);// std::numeric_limits<std::streamsize>::max());
    std::streamsize length = file.gcount();
    file.clear();   //  Since ignore will have set eof.
    file.seekg(0, std::ios_base::beg);
    file.close();
    return length;
}


void getSizeForAllFiles()
{
    __int64 size = 0;

    if (isWindows)
    {
        //Very slow 1863s 494k files
        if (false)
        {
            std::wcout << L"getFileSizeWindows" << std::endl;
            start = std::chrono::steady_clock::now();
            for (int i = 0; i < fileDataEntries.size(); i++)
            {
                FileDataEntry* f = &(fileDataEntries[i]);
                size += getFileSizeWindows(f);
            }
            std::wcout << "Total size:" << size << " bytes (" << std::fixed << std::setprecision(2) << (((double)size) / 1024. / 1024. / 1024.) << " GBytes)" << std::endl;
            std::wcout << L"Took " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start) << std::endl;
        }

        //22s 494k files
        if (false)
        {
            std::wcout << L"getFileSizeFromHandleFileInformationWindows" << std::endl;
            start = std::chrono::steady_clock::now();
            size = 0;
            for (int i = 0; i < fileDataEntries.size(); i++)
            {
                FileDataEntry* f = &(fileDataEntries[i]);
                size += getFileSizeFromHandleFileInformationWindows(f);
            }
            std::wcout << "Total size:" << size << " bytes (" << std::fixed << std::setprecision(2) << (((double)size) / 1024. / 1024. / 1024.) << " GBytes)" << std::endl;
            std::wcout << L"Took " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start) << std::endl;
        }

        //5.1s 494k files
        //if (false)
        {
            std::wcout << L"getFileSizeFromFileAttributesWindows" << std::endl;
            start = std::chrono::steady_clock::now();
            size = 0;
            for (int i = 0; i < fileDataEntries.size(); i++)
            {
                FileDataEntry* f = &(fileDataEntries[i]);
                size += getFileSizeFromFileAttributesWindows(f);
            }
            std::wcout << "Total size:" << size << " bytes (" << std::fixed << std::setprecision(2) << (((double)size) / 1024. / 1024. / 1024.) << " GBytes)" << std::endl;
            std::wcout << L"Took " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start) << std::endl;
        }


    }


    if (isWindows == false)
    {
        //17s 494k files
        if (false)
        {
            std::wcout << L"getFileSizeWStat" << std::endl;
            start = std::chrono::steady_clock::now();
            size = 0;
            for (int i = 0; i < fileDataEntries.size(); i++)
            {
                FileDataEntry* f = &(fileDataEntries[i]);
                size += getFileSizeWStat(f);
            }
            std::wcout << "Total size:" << size << " bytes (" << std::fixed << std::setprecision(2) << (((double)size) / 1024. / 1024. / 1024.) << " GBytes)" << std::endl;
            std::wcout << L"Took " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start) << std::endl;
        }


        //5.4s 494k files
        //if (false)
        {
            std::wcout << L"getFileSizeFilesystem" << std::endl;
            start = std::chrono::steady_clock::now();
            size = 0;
            for (int i = 0; i < fileDataEntries.size(); i++)
            {
                FileDataEntry* f = &(fileDataEntries[i]);
                size += getFileSizeFilesystem(f);
            }
            std::wcout << "Total size:" << size << " bytes (" << std::fixed << std::setprecision(2) << (((double)size) / 1024. / 1024. / 1024.) << " GBytes)" << std::endl;
            std::wcout << L"Took " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start) << std::endl;
        }
    }

    
}



void getFileCreatedModifiedDateWindows(FileDataEntry* f)
{
    WIN32_FILE_ATTRIBUTE_DATA info = getFileAttributesWindows(f->nameAndPath);
    FILETIME ct = info.ftCreationTime;
    FILETIME mt = info.ftLastWriteTime;
    
    __int64 createdTime = 0;
    __int64 modifiedTime = 0;
    wstring createdDateString;
    wstring modifiedDateString;

    __int64 max = MAXDWORD;
    max+=1;
        
    createdTime = (ct.dwHighDateTime * (max)) + ct.dwLowDateTime;
    modifiedTime = (mt.dwHighDateTime * (max)) + mt.dwLowDateTime;
    
    
    {
        SYSTEMTIME systemTime;
        FileTimeToSystemTime(&ct, &systemTime);
        wchar_t buffer[100];
        swprintf(buffer, 100, L"%04d-%02d-%02d %02d:%02d:%02d\n", systemTime.wYear, systemTime.wMonth, systemTime.wDay, systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
        createdDateString = wstring(buffer);
    }

    {
        SYSTEMTIME systemTime;
        FileTimeToSystemTime(&mt, &systemTime);
        wchar_t buffer[100];
        swprintf(buffer, 100, L"%04d-%02d-%02d %02d:%02d:%02d\n", systemTime.wYear, systemTime.wMonth, systemTime.wDay, systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
        modifiedDateString = wstring(buffer);
    }

    f->createdTime = createdTime;
    f->modifiedTime = modifiedTime;
    f->createdDateString = createdDateString;
    f->modifiedDateString = modifiedDateString;

    //std::wcout << "Created time:" << createdTime << std::endl;
    //std::wcout << "Modified time:" << modifiedTime << std::endl;
    //std::wcout << "Created date:" << createdDateString;// << std::endl;
    //std::wcout << "Modified date:" << modifiedDateString;// << std::endl;
}

void getFileCreatedModifiedDateFilesystem(FileDataEntry* f)
{
    //Creation time can't be accessed through std::filesystem
}

void getFileCreatedModifiedDateWStat(FileDataEntry *f)
{
    time_t createdTime = 0;
    time_t modifiedTime = 0;
    wstring createdDateString;
    wstring modifiedDateString;

    struct _stat64 result;
    if (_wstat64(f->nameAndPath.c_str(), &result) == 0)
    {
        //long long size = result.st_size;
        createdTime = result.st_ctime;
        modifiedTime = result.st_mtime;

        {
            wchar_t buffer[100];
            wcsftime(buffer, 100, L"%Y-%m-%d %H:%M:%S\n", gmtime(&result.st_ctime));
            createdDateString = wstring(buffer);
        }

        {
            wchar_t buffer[100];
            wcsftime(buffer, 100, L"%Y-%m-%d %H:%M:%S\n", gmtime(&result.st_mtime));
            modifiedDateString = wstring(buffer);
        }

        //createdDateString = convertUtf8ToWide(ctime(&result.st_ctime));
        //modifiedDateString = convertUtf8ToWide(ctime(&result.st_mtime));

        f->createdTime = createdTime;
        f->modifiedTime = modifiedTime;
        f->createdDateString = createdDateString;
        f->modifiedDateString = modifiedDateString;

        //std::wcout << "Created time:" << createdTime << std::endl;
        //std::wcout << "Modified time:" << modifiedTime << std::endl;
        //std::wcout << "Created date:" << createdDateString;// << std::endl;
        //std::wcout << "Modified date:" << modifiedDateString;// << std::endl;

    }
}

//create and modify date
void getCreatedAndLastModifiedDateForAllFiles()
{

    if (isWindows)
    {
        //12s 494k files
        std::wcout << L"getFileCreatedModifiedDateWindows" << std::endl;
        start = std::chrono::steady_clock::now();
        for (int i = 0; i < fileDataEntries.size(); i++)
        {
            FileDataEntry* f = &(fileDataEntries[i]);
            getFileCreatedModifiedDateWindows(f);
        }
        std::wcout << L"Took " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start) << std::endl;
    }

    if (isWindows == false)
    {
        //both of these are pretty comparable
        //23s 494k files
        std::wcout << L"getFileCreatedModifiedDateWStat" << std::endl;
        start = std::chrono::steady_clock::now();
        for (int i = 0; i < fileDataEntries.size(); i++)
        {
            FileDataEntry* f = &(fileDataEntries[i]);
            getFileCreatedModifiedDateWStat(f);
        }
        std::wcout << L"Took " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start) << std::endl;
    }

}

void getFastHash(FileDataEntry* f)
{
    if (f->crc32.empty() == false)return;
    if (f->md5.empty() == false)return;
    if (f->sha1.empty() == false)return;
    if (f->sha2.empty() == false)return;
    if (f->keccak.empty() == false)return;
    if (f->sha3.empty() == false)return;

    bool computeCrc32 = 1;
    bool computeMd5 = 1;
    bool computeSha1 = 1;
    bool computeSha2 = 1;
    bool computeKeccak = 1;
    bool computeSha3 = 1;

    CRC32  digestCrc32;
    MD5    digestMd5;
    SHA1   digestSha1;
    SHA256 digestSha2;
    Keccak digestKeccak(Keccak::Keccak256);
    SHA3   digestSha3(SHA3::Bits256);



    int chunkSize = 1024 * 16;
    int bufferSize = chunkSize * 5;
    if (f->size > bufferSize)
    {
        char* buffer = new char[bufferSize];

        try
        {
            if (std::ifstream is{ f->nameAndPath, std::ios::in | std::ios::binary })
            {
                is.exceptions(is.failbit);

                std::size_t numBytesRead = 0;

                is.seekg((f->size / 5) * 0);
                is.exceptions(is.failbit);
                is.read(&buffer[chunkSize * 0], chunkSize);
                is.exceptions(is.failbit);

                numBytesRead += size_t(is.gcount());

                is.seekg((f->size / 5) * 1);
                is.exceptions(is.failbit);
                is.read(&buffer[chunkSize * 1], chunkSize);
                is.exceptions(is.failbit);

                numBytesRead += size_t(is.gcount());

                is.seekg((f->size / 5) * 2);
                is.exceptions(is.failbit);
                is.read(&buffer[chunkSize * 2], chunkSize);
                is.exceptions(is.failbit);

                numBytesRead += size_t(is.gcount());

                is.seekg((f->size / 5) * 3);
                is.exceptions(is.failbit);
                is.read(&buffer[chunkSize * 3], chunkSize);
                is.exceptions(is.failbit);

                numBytesRead += size_t(is.gcount());

                is.seekg((f->size - chunkSize));
                is.exceptions(is.failbit);
                is.read(&buffer[chunkSize * 4], chunkSize);
                is.exceptions(is.failbit);

                numBytesRead += size_t(is.gcount());

                //std::wcout << L"Buffer size " << bufferSize << std::endl;
                //std::wcout << L"numBytesRead " << numBytesRead << std::endl;

                is.close();

                if (computeCrc32)   digestCrc32.add(buffer, numBytesRead);
                if (computeMd5)     digestMd5.add(buffer, numBytesRead);
                if (computeSha1)    digestSha1.add(buffer, numBytesRead);
                if (computeSha2)    digestSha2.add(buffer, numBytesRead);
                if (computeKeccak)  digestKeccak.add(buffer, numBytesRead);
                if (computeSha3)    digestSha3.add(buffer, numBytesRead);


            }
        }
        catch (const std::ios_base::failure& e)
        {
            std::cout << "Caught an ios_base::failure.\n"
                << "Explanatory string: " << e.what() << '\n'
                << "Error code: " << e.code() << '\n';
        }

        delete[] buffer;
    }
    else
    {
        char* buffer = new char[f->size];

        try
        {
            if (std::ifstream is{ f->nameAndPath, std::ios::in | std::ios::binary })
            {
                is.exceptions(is.failbit);

                is.read(buffer, f->size);

                is.exceptions(is.failbit);

                std::size_t numBytesRead = size_t(is.gcount());

                //std::wcout << L"File size " << f->size << std::endl;
                //std::wcout << L"numBytesRead " << numBytesRead << std::endl;

                is.close();

                if (computeCrc32)   digestCrc32.add(buffer, numBytesRead);
                if (computeMd5)     digestMd5.add(buffer, numBytesRead);
                if (computeSha1)    digestSha1.add(buffer, numBytesRead);
                if (computeSha2)    digestSha2.add(buffer, numBytesRead);
                if (computeKeccak)  digestKeccak.add(buffer, numBytesRead);
                if (computeSha3)    digestSha3.add(buffer, numBytesRead);

            }
        }
        catch (const std::ios_base::failure& e)
        {
            std::wcout << L"Caught an ios_base::failure" << e.what() << std::endl;
        }

        delete[] buffer;
    }

    //show results
    //if (computeCrc32) std::wcout << L"CRC32:      " << convertUtf8ToWide(digestCrc32.getHash()) << std::endl;
    //if (computeMd5)   std::wcout << L"MD5:        " << convertUtf8ToWide(digestMd5.getHash()) << std::endl;
    //if (computeSha1)  std::wcout << L"SHA1:       " << convertUtf8ToWide(digestSha1.getHash()) << std::endl;
    //if (computeSha2)  std::wcout << L"SHA2/256:   " << convertUtf8ToWide(digestSha2.getHash()) << std::endl;
    //if (computeKeccak)std::wcout << L"Keccak/256: " << convertUtf8ToWide(digestKeccak.getHash()) << std::endl;
    //if (computeSha3)  std::wcout << L"SHA3/256:   " << convertUtf8ToWide(digestSha3.getHash()) << std::endl;

    //f->size = size;
    f->crc32 = convertUtf8ToWide(digestCrc32.getHash());
    f->md5 = convertUtf8ToWide(digestMd5.getHash());
    f->sha1 = convertUtf8ToWide(digestSha1.getHash());
    f->sha2 = convertUtf8ToWide(digestSha2.getHash());
    f->keccak = convertUtf8ToWide(digestKeccak.getHash());
    f->sha3 = convertUtf8ToWide(digestSha3.getHash());


    //if (db)
    //{
    //    // Output line to file 
    //    fwprintf(db, L"%s\n", f.name.c_str());
    //    fwprintf(db, L"%s\n", f.path.c_str());
    //    fwprintf(db, L"%llu\n", f.size);
    //    fwprintf(db, L"%llu\n", f.createdTime);
    //    fwprintf(db, L"%llu\n", f.modifiedTime);
    //    fwprintf(db, L"%s\n", f.createdDate.c_str());
    //    fwprintf(db, L"%s\n", f.modifiedDate.c_str());
    //    fwprintf(db, L"%s\n", f.crc32.c_str());
    //    fwprintf(db, L"%s\n", f.md5.c_str());
    //    fwprintf(db, L"%s\n", f.sha1.c_str());
    //    fwprintf(db, L"%s\n", f.sha2.c_str());
    //    fwprintf(db, L"%s\n", f.keccak.c_str());
    //    fwprintf(db, L"%s\n", f.sha3.c_str());
    //    fwprintf(db, L";");
    //}
    //else
    //{
    //    wprintf(L"Database not open\n");
    //    exit(EXIT_FAILURE);
    //}
}


void getFastHashForAllFiles()
{

    //It makes an extra pass of the file list to create a 'fasthash'. The fasthash uses small samples of the file(16 kilobytes),
    //taken from the beginning, end, and three places in the middle; then does a duplicate check based on the hash of the samples.
    //This is very quick for large files, and it helps eliminate the vast majority of potential duplicates very quickly, as
    //most files will have different samples. Most other duplicate finders omit this step, but it really speeds things up.

    for (int i = 0; i < fileDataEntries.size(); i++)
    {
        FileDataEntry* f = &(fileDataEntries[i]);

        getFastHash(f);

    }
}


void getDateFromFilenameForAllFiles()
{
    for (int i = 0; i < fileDataEntries.size(); i++)
    {
        FileDataEntry* f = &(fileDataEntries[i]);


        //get date from filename, detect year, detect date format, extract and convert


        //yyyy-mm-dd
        //yyyy mm dd
        //yyyy_mm_dd
        //yyyy.mm.dd  
        //yyyymmdd

        //yyyy cannot be 3xxx 1xxx 23xx 29xx 17xx
        //mm cannot be 00 13 2x
        //dd cannot be 00 33 4x
        //(?!2[3-9])not 3xxx    [12][09]\d{2}1900-2099 [^0-9a-zA-Z]any separator (?!00)(?!1[3-9])[01]\d{1}month cant be 00 or 13 [^0-9a-zA-Z]any separator (?!00)(?!3[3-9])[0123]\d{1}day cant be 00 or 33


        std::string regex_yyyy_mm_dd =
            "(?!1[0-8])"//not 10xx-18xx
            "(?!19[0-7])"//not 190x-197x
            "(?!2[3-9])"//not 23xx
            "[12][09][0-9][0-9]"//1000-2999
            "[_. ,mMyY-]"//any separator including m M y Y
            "(?!00)"//month cannot be 00
            "(?!1[3-9])"//month cannot be 13-19
            "[01][0-9]"//month only begins with 0 or 1
            "[_. ,mMdD-]"//any separator including m M d D
            "(?!00)"//day cannot be 00
            "(?!3[3-9])"//day cannot be 33-39
            "[0123][0-9]"//day only begins with 0 1 2 3
            ;

        std::string syyyy_mm_dd =
            "^"//beginning of file
            "("
            + regex_yyyy_mm_dd +
            ")"
            "|"//or
            "[^0-9]"//not a number
            "("
            + regex_yyyy_mm_dd +
            ")"
            ;


        std::string regex_yyyymmdd =
            "(?!1[0-8])"//not 10xx-18xx
            "(?!19[0-7])"//not 190x-197x
            "(?!2[3-9])"//not 23xx
            "[12][09][0-9][0-9]"//1000-2999
            "(?!00)"//month cannot be 00
            "(?!1[3-9])"//month cannot be 13-19
            "[01][0-9]"//month only begins with 0 or 1
            "(?!00)"//day cannot be 00
            "(?!3[3-9])"//day cannot be 33-39
            "[0123][0-9]"//day only begins with 0 1 2 3
            ;


        std::string syyyymmdd =
            "^"//beginning of file
            "("
            + regex_yyyymmdd +
            ")"
            "|"//or
            "[^0-9]"//not a number
            "("
            + regex_yyyymmdd +
            ")"
            ;



        std::regex yyyy_mm_dd(syyyy_mm_dd);
        std::regex yyyymmdd(syyyymmdd);

        //yyyy-mm-d?




        //mm-dd-yy
        //mm dd yy
        //mm/dd/yy
        //mm_dd_yy
        //mm.dd.yy  
        //mmddyy 
        //mm cannot be 00 13 2x
        //dd cannot be 00 33 4x
        //yy can be anything but most likely 8x 9x 0x 1x 2x, cannot be higher than current year 23 24 25
        std::string regex_mm_dd_yy =
            "(?!00)"//month cannot be 00
            "(?!1[3-9])"//month cannot be 13-19
            "[01][0-9]"//month only begins with 0 or 1
            "[_. ,mMdD-]"//any separator including m M d D
            "(?!00)"//day cannot be 00
            "(?!3[3-9])"//day cannot be 33-39
            "[0123][0-9]"//day only begins with 0 1 2 3
            "[_. ,dDyY-]"//any separator including d D y Y
            "[12]?[90]?[012789][0-9]"//year [19-20]70-29
            ;
        std::string smm_dd_yy =
            "^"//beginning of file
            "("
            + regex_mm_dd_yy +
            ")"
            "|"//or
            "[_. ,mM-]"//any separator including m M
            "("
            + regex_mm_dd_yy +
            ")"
            ;

        std::regex mm_dd_yy(smm_dd_yy);// \d{2}:\d{2}:\d{2}.\d{3}




        //mm-d-yy?



        //Dayname Monthname/Mon Day/Dayth, YYYY
        //Month/Mon Day/Dayth YYYY
        //Day/Dayth Month/Mon YYYY
        //YYYY Month Day
        //YYYY Day Month
        //first look for YYYY, if exists then look for monthname, mon, day, 8th, etc

        std::string regex_yyyy =
            "(?!2[3-9])"//not 23xx
            "(?!1[0-8])"//not 10xx-18xx
            "(?!19[0-7])"//not 190x-197x
            "[12][09][0-9][0-9]"//1000-2999
            ;

        std::string syyyy =
            "^"
            "("
            + regex_yyyy + //beginning of string
            ")"
            "[^0-9]"
            "|"
            "[^0-9]"
            "("
            + regex_yyyy + //in the middle surrounded by non numbers
            ")"
            "[^0-9]"
            "|"
            "[^0-9]"
            "("
            + regex_yyyy + //at the end beginning with non number
            ")"
            "$"
            ;

        std::regex yyyy(syyyy);// \d{2}:\d{2}:\d{2}.\d{3}
        //if found

        //month/mon dayth/day
        std::string monthmon("(January|February|March|April|May|June|July|August|September|October|November|December|Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)");
        std::string spacer("[^0-9][^0-9]?");

        std::string regexdaythday =
            "[0123]?1st"//01st 1st or 21st or 31st
            "|"
            "[0123]?2nd"//02nd 2nd 22nd 32nd
            "|"
            "(?!33)"//not 33rd
            "[0123]?3rd"//03rd 3rd 23rd
            "|"
            "(?!3[4-9])"//not 34-39th
            "[0123]?[0-9]th"//04th 9th 11th 24th 29th
            "|"
            "(?!00)"//not 00
            "(?!3[3-9])"//not 33-39
            "[0123]?[0-9]"//01-32
            ;

        std::string sdaythday =
            //"("
            //+ regexdaythday +
            //")"
            //"[^0-9]"
            //"|"
            "("
            + regexdaythday +
            ")"
            //"$"
            ;



        std::regex monthmondaythday(monthmon + spacer + sdaythday);

        //dayth/day month/mon
        std::regex daythdaymonthmon(sdaythday + monthmon);

        string strs[] = {
            "2018-08-09 09:30:34.118",
            "2018 01 01",
            "2018_12_20",
            "2018.02.02",
            "20180111",
            "01-30-99",
            "01.30.02",
            "January 1, 1990",
            "8 February 2000",
            "04 Feb 2016",
            "12th August, 2011",
            "September 02 2022",
            "December 4th, 1997" ,
            "2017 Mar 4"

        };


        string s(convertWideToUtf8(f->name));
        //if(false)
        //for (auto& s : strs)

        {
            std::smatch matches;
            std::regex_search(s, matches, yyyy_mm_dd);
            //int n = 0;

            int y = 0;
            int m = 0;
            int d = 0;

            if (!matches.empty())
            {
                for (int n = 0; n < matches.size(); n++)
                    std::wcout << L"yyyy_mm_dd " << n << L" " << convertUtf8ToWide(matches[n].str()) << std::endl;


                int n = matches[0].str().find_first_of("0123456789");

                std::istringstream is(matches[0].str().substr(n, 4));
                if (is >> y)
                {
                    if (y < 1900)
                    {
                        if (y > 70)y += 1900;
                        if (y < 50)y += 2000;
                    }
                }

                {
                    std::istringstream is(matches[0].str().substr(n + 5, 2));
                    if (is >> m);
                }
                {
                    std::istringstream is(matches[0].str().substr(n + 8, 2));
                    if (is >> d);
                }

                std::wcout << y << L" " << m << L" " << d << std::endl;


            }
            else
            {

                std::regex_search(s, matches, yyyymmdd);
                if (!matches.empty())
                {
                    for (int n = 0; n < matches.size(); n++)
                        std::wcout << L"yyyymmdd " << n << L" " << convertUtf8ToWide(matches[n].str()) << std::endl;

                    int n = matches[0].str().find_first_of("0123456789");


                    std::istringstream is(matches[0].str().substr(n, 4));
                    if (is >> y)
                    {
                        if (y < 1900)
                        {
                            if (y > 70)y += 1900;
                            if (y < 50)y += 2000;
                        }
                    }

                    {
                        std::istringstream is(matches[0].str().substr(n + 4, 2));
                        if (is >> m);
                    }
                    {
                        std::istringstream is(matches[0].str().substr(n + 6, 2));
                        if (is >> d);
                    }

                    std::wcout << y << L" " << m << L" " << d << std::endl;

                }
                else
                {
                    std::regex_search(s, matches, mm_dd_yy);
                    if (!matches.empty())
                    {
                        for (int n = 0; n < matches.size(); n++)
                            std::wcout << L"mm dd [yy]yy " << n << L" " << convertUtf8ToWide(matches[n].str()) << std::endl;

                        int n = matches[0].str().find_first_of("0123456789");

                        {
                            std::istringstream is(matches[0].str().substr(n, 2));
                            if (is >> m);
                        }

                        {
                            std::istringstream is(matches[0].str().substr(n + 3, 2));
                            if (is >> d);
                        }

                        if (matches[0].str().substr(n + 6, 2) == "19" || matches[0].str().substr(n + 6, 2) == "20")
                        {
                            std::istringstream is(matches[0].str().substr(n + 6, 4));
                            if (is >> y);
                        }
                        else
                        {
                            std::istringstream is(matches[0].str().substr(n + 6, 2));
                            if (is >> y)
                            {
                                if (y < 1900)
                                {
                                    if (y > 70)y += 1900;
                                    if (y < 50)y += 2000;
                                }
                            }
                        }

                        std::wcout << y << L" " << m << L" " << d << std::endl;

                    }
                    else
                    {
                        std::regex_search(s, matches, yyyy);
                        if (!matches.empty())
                        {
                            //for (int n = 0; n < matches.size(); n++)
                                //std::wcout << L"yyyy " << n << L" " << convertUtf8ToWide(matches[n].str()) << std::endl;

                            int n = matches[0].str().find_first_of("0123456789");

                            if (matches[0].str().substr(n, 1) != "1" && matches[0].str().substr(n, 1) != "2")
                            {
                                if (matches[0].str().substr(n + 1, 1) != "1" && matches[0].str().substr(n + 1, 1) != "2")
                                {
                                    //can't parse?
                                }
                                else
                                {
                                    std::istringstream is(matches[0].str().substr(n + 1, 4));
                                    if (is >> y);
                                }
                            }
                            else
                            {
                                std::istringstream is(matches[0].str().substr(n, 4));
                                if (is >> y);
                            }

                            std::regex_search(s, matches, monthmondaythday);
                            if (!matches.empty())
                            {
                                for (int n = 0; n < matches.size(); n++)
                                    std::wcout << L"monthmondaythday " << n << L" " << convertUtf8ToWide(matches[n].str()) << std::endl;

                                if (matches[0].str().find("Jan") != string::npos)m = 1;
                                if (matches[0].str().find("Feb") != string::npos)m = 2;
                                if (matches[0].str().find("Mar") != string::npos)m = 3;
                                if (matches[0].str().find("Apr") != string::npos)m = 4;
                                if (matches[0].str().find("May") != string::npos)m = 5;
                                if (matches[0].str().find("Jun") != string::npos)m = 6;
                                if (matches[0].str().find("Jul") != string::npos)m = 7;
                                if (matches[0].str().find("Aug") != string::npos)m = 8;
                                if (matches[0].str().find("Sep") != string::npos)m = 9;
                                if (matches[0].str().find("Oct") != string::npos)m = 10;
                                if (matches[0].str().find("Nov") != string::npos)m = 11;
                                if (matches[0].str().find("Dec") != string::npos)m = 12;

                                int n = matches[0].str().find_first_of("0123456789");
                                {
                                    if (matches[0].str().find_first_of("0123456789", n + 1) == n + 1)
                                    {
                                        std::istringstream is(matches[0].str().substr(n, 2));
                                        if (is >> d);
                                    }
                                    else
                                    {
                                        std::istringstream is(matches[0].str().substr(n, 1));
                                        if (is >> d);
                                    }
                                }

                                std::wcout << y << L" " << m << L" " << d << std::endl;
                            }
                            else
                            {

                                std::regex_search(s, matches, daythdaymonthmon);
                                if (!matches.empty())
                                {
                                    for (int n = 0; n < matches.size(); n++)
                                        std::wcout << L"daythdaymonthmon " << n << L" " << convertUtf8ToWide(matches[n].str()) << std::endl;

                                    int n = matches[0].str().find_first_of("0123456789");

                                    if (matches[0].str().find_first_of("0123456789", n + 1) == n + 1)
                                    {
                                        std::istringstream is(matches[0].str().substr(n, 2));
                                        if (is >> d);
                                    }
                                    else
                                    {
                                        std::istringstream is(matches[0].str().substr(n, 1));
                                        if (is >> d);
                                    }

                                    if (matches[0].str().find("Jan") != string::npos)m = 1;
                                    if (matches[0].str().find("Feb") != string::npos)m = 2;
                                    if (matches[0].str().find("Mar") != string::npos)m = 3;
                                    if (matches[0].str().find("Apr") != string::npos)m = 4;
                                    if (matches[0].str().find("May") != string::npos)m = 5;
                                    if (matches[0].str().find("Jun") != string::npos)m = 6;
                                    if (matches[0].str().find("Jul") != string::npos)m = 7;
                                    if (matches[0].str().find("Aug") != string::npos)m = 8;
                                    if (matches[0].str().find("Sep") != string::npos)m = 9;
                                    if (matches[0].str().find("Oct") != string::npos)m = 10;
                                    if (matches[0].str().find("Nov") != string::npos)m = 11;
                                    if (matches[0].str().find("Dec") != string::npos)m = 12;

                                    std::wcout << y << L" " << m << L" " << d << std::endl;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}


/** Callback function handling an ExifEntry. */
static void content_foreach_func(ExifEntry* entry, void* UNUSED(callback_data))
{
    char buf[2000];
    exif_entry_get_value(entry, buf, sizeof(buf));
    std::wcout << convertUtf8ToWide(exif_tag_get_name(entry->tag)) << L" " <<
        convertUtf8ToWide(exif_format_get_name(entry->format)) << L" " <<
        entry->size << L" " <<
        (int)(entry->components) << L" " <<
        convertUtf8ToWide(exif_entry_get_value(entry, buf, sizeof(buf))) << std::endl;
}


/** Callback function handling an ExifContent (corresponds 1:1 to an IFD). */
static void data_foreach_func(ExifContent* content, void* callback_data)
{
    static unsigned content_count;
    std::wcout << exif_content_get_ifd(content) << std::endl;
    exif_content_foreach_entry(content, content_foreach_func, callback_data);
    ++content_count;
}



// function expects the string in format dd/mm/yyyy:
bool extractDateMM_DD_YYorYYYY(const std::string& s, int& d, int& m, int& y) 
{
    std::istringstream is(s);
    char delimiter;
    if (is >> m >> delimiter >> d >> delimiter >> y) {

        if (y < 1900)
        {
            if (y > 70)y += 1900;
            if (y < 50)y += 2000;
        }

        struct tm t = { 0 };
        t.tm_mday = d;
        t.tm_mon = m - 1;
        t.tm_year = y - 1900;
        t.tm_isdst = -1;

        // normalize:
        time_t when = mktime(&t);
        const struct tm* norm = localtime(&when);
        // the actual date would be:
        // m = norm->tm_mon + 1;
        // d = norm->tm_mday;
        // y = norm->tm_year;
        // e.g. 29/02/2013 would become 01/03/2013

        // validate (is the normalized date still the same?):
        return (norm->tm_mday == d &&
            norm->tm_mon == m - 1 &&
            norm->tm_year == y - 1900);
    }
    return false;
}

void getDatesFromEXIFDataForAllFiles()
{

    //if file is an image get exif data, timestamps
    //look at all possible timestamp values in exif

    //libexif
    for (int i = 0; i < fileDataEntries.size(); i++)
    {
        FileDataEntry* f = &(fileDataEntries[i]);


        if (f->name.find(L".jpg") != std::wstring::npos || f->name.find(L".jpeg") != std::wstring::npos || f->name.find(L".JPG") != std::wstring::npos)
        {
            ExifData* d;
            d = exif_data_new_from_file(convertWideToUtf8(f->nameAndPath).c_str());
            if (!d)
            {
                std::wcout << L"Could not load data from " << f->name << std::endl;
            }
            else
            {
                void* callback_data = NULL;

                exif_data_foreach_content(d, data_foreach_func, callback_data);

                ExifMnoteData* mn = exif_data_get_mnote_data(d);
                if (mn) {
                    char buf[2000];
                    int i;
                    int num = exif_mnote_data_count(mn);
                    std::wcout << L"  MakerNote" << std::endl;
                    for (i = 0; i < num; ++i) {
                        if (exif_mnote_data_get_value(mn, i, buf, sizeof(buf))) {
                            const char* name = exif_mnote_data_get_name(mn, i);
                            unsigned int id = exif_mnote_data_get_id(mn, i);
                            if (!name)
                                name = "(unknown)";
                            std::wcout << convertUtf8ToWide(name) << L" " << convertUtf8ToWide(buf) << std::endl;
                        }
                    }
                }

                exif_data_unref(d);
            }
        }
    }

    //DateTime
    //DateTimeOriginal
    //DateTimeDigitized
    //Date
    //Time
    //GPSDateStamp
    //GPSTimeStamp


    //TinyEXIF
    for (int i = 0; i < fileDataEntries.size(); i++)
    {
        FileDataEntry* f = &(fileDataEntries[i]);

        if (f->name.find(L".jpg") != std::wstring::npos ||
            f->name.find(L".jpeg") != std::wstring::npos ||
            f->name.find(L".JPG") != std::wstring::npos ||
            f->name.find(L".png") != std::wstring::npos ||
            f->name.find(L".PNG") != std::wstring::npos
            )
        {
            // open a stream to read just the necessary parts of the image file
            std::ifstream stream(f->nameAndPath, std::ios::binary);
            if (stream)
            {
                // parse image EXIF and XMP metadata
                TinyEXIF::EXIFInfo imageEXIF(stream);
                if (!imageEXIF.Fields)
                {

                }

                if (!imageEXIF.DateTime.empty())
                    std::wcout << L"DateTime " << convertUtf8ToWide(imageEXIF.DateTime) << std::endl;
                if (!imageEXIF.DateTimeOriginal.empty())
                    std::wcout << L"DateTimeOriginal " << convertUtf8ToWide(imageEXIF.DateTimeOriginal) << std::endl;
                if (!imageEXIF.DateTimeDigitized.empty())
                    std::wcout << L"DateTimeDigitized " << convertUtf8ToWide(imageEXIF.DateTimeDigitized) << std::endl;

            }
        }
    }

}



#define DB_LOCATION L"locate.db"
static FILE* db = NULL;







int main(int argc, char* argv[])
{

#ifdef WIN32
    isWindows = true;
#else
    isWindows = false;
#endif

    //Code Page Identifiers
    const char CP_UTF_16LE[] = ".1200";
    setlocale(LC_ALL, CP_UTF_16LE);
    //Prepare for unicode output 
    int setmodestatus = _setmode(_fileno(stdout), _O_U16TEXT);

    //if (db == NULL)
    //{
    //    errno_t error;
    //    error = _wfopen_s(&db, DB_LOCATION, L"wt, ccs=UNICODE");
    //    if (error)
    //    {
    //        wprintf(L"Cannot open %s\n", DB_LOCATION);
    //        exit(EXIT_FAILURE);
    //    }
    //}


    wstring startpath = L"D:\\";
    _int64 filecount = 0;
    LARGE_INTEGER li;

    _int64 CounterStart = 0;
    _int64 msPassed = 0;
    
    QueryPerformanceFrequency(&li);
    double PCFreq = double(li.QuadPart) / 1000.0;





    //std::wcout << L"directoryIteratorRecursive" << std::endl;
    //
    //QueryPerformanceCounter(&li);
    //CounterStart = li.QuadPart;
    //start = std::chrono::steady_clock::now();
    //filecount = 0;
    //
    //directoryIteratorRecursive(startpath, filecount);
    //
    //std::wcout << L"Files found: " << filecount << std::endl;
    //
    //QueryPerformanceCounter(&li);
    //msPassed = (_int64)(double(li.QuadPart - CounterStart) / PCFreq);
    ////std::wcout << L"Time passed QueryPerformanceCounter: " << msPassed << std::endl;
    //std::wcout << L"Took " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start) << std::endl;


    //std::wcout << L"direntScanDirectory" << std::endl;
    //
    //QueryPerformanceCounter(&li);
    //CounterStart = li.QuadPart;
    //start = std::chrono::steady_clock::now();
    //filecount = 0;
    //
    //direntScanDirectory(startpath,filecount);
    //
    //std::wcout << L"Files found: " << filecount << std::endl;
    //
    //QueryPerformanceCounter(&li);
    //msPassed = (_int64)(double(li.QuadPart - CounterStart) / PCFreq);
    ////std::wcout << L"Time passed QueryPerformanceCounter: " << msPassed << std::endl;
    //std::wcout << L"Took " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start) << std::endl;



    std::wcout << L"ListFilesWindowsFindFirstFile" << std::endl;

    QueryPerformanceCounter(&li);
    CounterStart = li.QuadPart;
    start = std::chrono::steady_clock::now();
    filecount = 0;

    ListFilesWindowsFindFirstFile(startpath, filecount);

    std::wcout << L"Files found: " << filecount << std::endl;

    QueryPerformanceCounter(&li);
    msPassed = (_int64)(double(li.QuadPart - CounterStart) / PCFreq);
    //std::wcout << L"Time passed QueryPerformanceCounter: " << msPassed << std::endl;
    std::wcout << L"Took " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start) << std::endl;






    getSizeForAllFiles();
    
    getCreatedAndLastModifiedDateForAllFiles();

    if(false)
    getFastHashForAllFiles();

    if(false)
    getDateFromFilenameForAllFiles();

    if(false)
    getDatesFromEXIFDataForAllFiles();



    //You can use std::swap to swap two values.
    //And also you may want to compare to std::sort(which is typically an introsort : a quick sort + insertion sort for small sizes),
    //std::stable_sort(typically a merge sort), and std::make_heap + std::sort_heap(heap sort)
    {
        //sort by file size

        std::wcout << L"Start sort by size" << std::endl;
        start = std::chrono::steady_clock::now();

        //std::sort(fileDataEntries.begin(), fileDataEntries.end());//56s 494k files
        std::stable_sort(fileDataEntries.begin(), fileDataEntries.end());//53s 494k files

        std::wcout << L"Took " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start) << std::endl;
    }

    //for each entry, check to see if there are duplicates ahead
    {
        for (int i = 0; i < fileDataEntries.size(); i++)
        {
            __int64 size = fileDataEntries[i].size;

            for (int j = i + 1; j < fileDataEntries.size(); j++)
            {
                if (size == fileDataEntries[j].size)
                {
                    //got a match

                    //check hash
                    getFastHash(&(fileDataEntries[i]));
                    getFastHash(&(fileDataEntries[j]));

                    //compare hash
                    if (
                        (
                            fileDataEntries[i].crc32.empty() == false &&
                            fileDataEntries[i].md5.empty() == false &&
                            fileDataEntries[i].sha1.empty() == false &&
                            fileDataEntries[i].sha2.empty() == false &&
                            fileDataEntries[i].keccak.empty() == false &&
                            fileDataEntries[i].sha3.empty() == false &&

                            fileDataEntries[j].crc32.empty() == false &&
                            fileDataEntries[j].md5.empty() == false &&
                            fileDataEntries[j].sha1.empty() == false &&
                            fileDataEntries[j].sha2.empty() == false &&
                            fileDataEntries[j].keccak.empty() == false &&
                            fileDataEntries[j].sha3.empty() == false
                            )
                        &&
                        (
                            fileDataEntries[i].crc32 == fileDataEntries[j].crc32 ||
                            fileDataEntries[i].md5 == fileDataEntries[j].md5 ||
                            fileDataEntries[i].sha1 == fileDataEntries[j].sha1 ||
                            fileDataEntries[i].sha2 == fileDataEntries[j].sha2 ||
                            fileDataEntries[i].keccak == fileDataEntries[j].keccak ||
                            fileDataEntries[i].sha3 == fileDataEntries[j].sha3
                            )
                        )
                    {
                        std::wcout << L"Possible match: " << fileDataEntries[i].nameAndPath << L" " << fileDataEntries[j].nameAndPath << std::endl;
                    }

                    //match comparisons run full hash and byte comparison

                }
                else
                {
                    j = fileDataEntries.size();
                    break;
                }
            }

        }

    }



    




    //use library to compare images with fuzzy comparison, different resolution comparison, use different methods to ensure accuracy



    //keep all filenames and dates, compare with filename date, exif dates, determine most correct timestamp for image and duplicates

    

    //keep higher quality version, optionally delete others

    //rename files with most accurate date, optionally reset timestamps
    //store all filenames and dates etc in metadata


    //ocr

    //ai image object detection


    //file renamer


    //gui



}





        



        





    




    /*
    std::wcout << L"recursiveDirectoryIteratorIncrementPaths.size() " << recursiveDirectoryIteratorIncrementPaths.size() << std::endl;
    std::wcout << L"directoryIteratorRecursivePaths.size() " << directoryIteratorRecursivePaths.size() << std::endl;

    vector<wstring> notFoundMatch1;

    for (std::unordered_set<wstring>::const_iterator it = recursiveDirectoryIteratorIncrementPaths.cbegin(); it != recursiveDirectoryIteratorIncrementPaths.cend(); ++it)
    {
        wstring path = *it;

        std::unordered_set<wstring>::const_iterator got = directoryIteratorRecursivePaths.find(path);

        if (got == directoryIteratorRecursivePaths.end())
            notFoundMatch1.push_back(path);
    }

    vector<wstring> notFoundMatch2;

    for (std::unordered_set<wstring>::const_iterator it = directoryIteratorRecursivePaths.cbegin(); it != directoryIteratorRecursivePaths.cend(); ++it)
    {
        wstring path = *it;

        std::unordered_set<wstring>::const_iterator got = recursiveDirectoryIteratorIncrementPaths.find(path);

        if (got == recursiveDirectoryIteratorIncrementPaths.end())
            notFoundMatch2.push_back(path);
    }

    std::wcout << L"recursiveDirectoryIteratorIncrementPaths not found in directoryIteratorRecursivePaths " << notFoundMatch1.size() << std::endl;
    std::wcout << L"directoryIteratorRecursivePaths not found in recursiveDirectoryIteratorIncrementPaths " << notFoundMatch2.size() << std::endl;

    std::wcout << L"recursiveDirectoryIteratorIncrementPaths not found in directoryIteratorRecursivePaths" << std::endl;
    for (int i = 0; i < notFoundMatch1.size(); i++)
    {
        wstring path = notFoundMatch1[i];
        std::wcout << path << std::endl;
    }
    std::wcout << L"directoryIteratorRecursivePaths not found in recursiveDirectoryIteratorIncrementPaths" << std::endl;
    for (int i = 0; i < notFoundMatch2.size(); i++)
    {
        wstring path = notFoundMatch2[i];
        std::wcout << path << std::endl;
    }
    */
    //findFirstFilePaths


    //if (db)
    //{
    //    fclose(db);
    //    db = NULL;
    //}







/*
DupeKill_v1.2 ReadMe
Incredibly, I found room on the internet for yet another duplicate file remover. 
Like others, this tool compares files based on content and lists any files which have exact binary duplicates. 
What sets this one apart is that you can give it hints about which files you'd like to keep before starting a scan. 
For example; if you tell it to prefer files with the 'most descriptive name', the app look at all duplicate filenames and default to the shortest, 
most descriptive name as the one to keep. A file with a name containing "copy of", or ".1.txt" would be considered less descriptive than one without; 
and a file named "lkePic.jpg" is considered less descriptive than "Lake Pictures.jpg". This way the default keep or remove action is pre-set; 
and time you spend going though the list and making changes to keep the 'right' file is minimized. The tool has many other options to choose 
from to pick the default 'kept' file: use the newest or oldest by date, the longest or shortest filename, or specify one or more 
directories where files should always be kept or removed.

The app also makes use of a speed improvement I haven't seen anywhere else: 
It makes an extra pass of the file list to create a 'fasthash'. The fasthash uses small samples of the file (16 kilobytes), 
taken from the beginning, end, and three places in the middle; then does a duplicate check based on the hash of the samples. 
This is very quick for large files, and it helps eliminate the vast majority of potential duplicates very quickly, as 
most files will have different samples. Most other duplicate finders omit this step, but it really speeds things up.

As of beta 6 the app is fairly robust; avoiding most of the traps in this excellent reference.

Note: There's no fuzzy searching, this app only finds exact duplicates.

Usage Notes
Version 4.6.2 or better of the .net framework is required. Get it from Microsoft.

No installation; just unpack and run. A settings file and ancillary files may be created in the program folder.

Instructions:

Pick a directory where you have duplicate files and hit scan.
After the program identifies the duplicate files, you'll be given a list to choose what to keep and what to delete. 
The app makes suggestions, but you can make changes by right clicking and selecting delete or keep; 
Or by clicking the 'Action' column to toggle it. At this point scan results can be copied via the right click menu or 
exported in bulk via the 'Export Scan...' button.
Hit the 'Execute Actions' button to run the selected actions. The app will warn you about deletions, 
and there's an extra warning if you're deleting all copies of a file. 
You can make a log to see what was done. The app will reset after this.
Each duplicate will have an action automatically assigned, and you can manually set the action by 
right clicking or using the keyboard. Available actions are: Keep [spacebar], Delete [delete key], 
Link [F4 key], Recycle, or Move. The context menu option 'Mark as auto' will reset the action to whatever was 
automatically assigned. The 'Invert Action' option will toggle the action between 'Keep' and whatever 
the default removal action is (set this in settings, by default it's 'Delete').

For complex searches, the 'Look In' drop-down has an 'Advanced criteria' option. 
Advanced criterias let you include or exclude multiple folders and have filtering options, 
as well as options to specify a default 'keep' or 'remove from' folder. These criteria can 
be named and permanently saved. To create a criteria, select 'Advanced Criteria..." from 
the 'Look In' drop-down; the Edit Criteria form will the show with further options.

When using Advanced Criteria the '...' button is used to edit an existing criteria. 
To remove a saved criteria: select it in the drop-down, edit it with the '...' button, 
and hit the 'Delete this Criteria' button in the lower left.
*/




/*
How does SimilarImages find those similar images?
It uses a somewhat modified version of the "Quadrant RGB" algorithm and futhermore considers some other meta-information provided by the source files.
This basicially means: the images will be split into quadrants and average RGB values are computed.
Pairs that have pretty "close" avg. numbers are considered being "similar".
*/

/*
imagededup is a python package that simplifies the task of finding exact and near duplicates in an image collection.


This package provides functionality to make use of hashing algorithms that are particularly good at finding exact duplicates as well as convolutional neural networks which are also adept at finding near duplicates. An evaluation framework is also provided to judge the quality of deduplication for a given dataset.

Following details the functionality provided by the package:

Finding duplicates in a directory using one of the following algorithms:
Convolutional Neural Network (CNN)
Perceptual hashing (PHash)
Difference hashing (DHash)
Wavelet hashing (WHash)
Average hashing (AHash)
Generation of encodings for images using one of the above stated algorithms.
*/

/*
MobileNets: Efficient Convolutional Neural Networks for Mobile Vision Applications
Andrew G. Howard, Menglong Zhu, Bo Chen, Dmitry Kalenichenko, Weijun Wang, Tobias Weyand, Marco Andreetto, Hartwig Adam
We present a class of efficient models called MobileNets for mobile and embedded vision applications. MobileNets are based on a streamlined architecture that uses depth-wise separable convolutions to build light weight deep neural networks. We introduce two simple global hyper-parameters that efficiently trade off between latency and accuracy. These hyper-parameters allow the model builder to choose the right sized model for their application based on the constraints of the problem. We present extensive experiments on resource and accuracy tradeoffs and show strong performance compared to other popular models on ImageNet classification. We then demonstrate the effectiveness of MobileNets across a wide range of applications and use cases including object detection, finegrain classification, face attributes and large scale geo-localization.
Subjects:	Computer Vision and Pattern Recognition (cs.CV)
Cite as:	arXiv:1704.04861 [cs.CV]
    (or arXiv:1704.04861v1 [cs.CV] for this version)

https://doi.org/10.48550/arXiv.1704.04861
Focus to learn more

*/

/*

So how do you create a perceptual hash? There are a couple of common algorithms, but none are very complicated. (I'm always surprised that the most common algorithms even work, because they seem too simple!) One of the simplest hashes represents a basic average based on the low frequencies.

With pictures, high frequencies give you detail, while low frequencies show you structure. A large, detailed picture has lots of high frequencies. A very small picture lacks details, so it is all low frequencies. To show how the Average Hash algorithm works, I'll use a picture of actress Alyson Hannigan.


Reduce size. The fastest way to remove high frequencies and detail is to shrink the image. In this case, shrink it to 8x8 so that there are 64 total pixels. Don't bother keeping the aspect ratio, just crush it down to fit an 8x8 square. This way, the hash will match any variation of the image, regardless of scale or aspect ratio.

Reduce color. The tiny 8x8 picture is converted to a grayscale. This changes the hash from 64 pixels (64 red, 64 green, and 64 blue) to 64 total colors.
Average the colors. Compute the mean value of the 64 colors.
Compute the bits. This is the fun part. Each bit is simply set based on whether the color value is above or below the mean.
Construct the hash. Set the 64 bits into a 64-bit integer. The order does not matter, just as long as you are consistent. (I set the bits from left to right, top to bottom using big-endian.)
 =  = 8f373714acfcf4d0
The resulting hash won't change if the image is scaled or the aspect ratio changes. Increasing or decreasing the brightness or contrast, or even altering the colors won't dramatically change the hash value. And best of all: this is FAST!

If you want to compare two images, construct the hash from each image and count the number of bit positions that are different. (This is a Hamming distance.) A distance of zero indicates that it is likely a very similar picture (or a variation of the same picture). A distance of 5 means a few things may be different, but they are probably still close enough to be similar. But a distance of 10 or more? That's probably a very different picture.

Getting Funky With pHash
While the Average Hash is quick and easy, it may be too rigid of a comparison. For example, it can generate false-misses if there is a gamma correction or a color histogram is applied to the image. This is because the colors move along a non-linear scale -- changing where the "average" is located and therefore changing which bits are above/below the average.

A more robust algorithm is used by pHash. (I use my own variation of the algorithm, but it's the same concept.) The pHash approach extends the average approach to the extreme, using a discrete cosine transform (DCT) to reduce the frequencies.
Reduce size. Like Average Hash, pHash starts with a small image. However, the image is larger than 8x8; 32x32 is a good size. This is really done to simplify the DCT computation and not because it is needed to reduce the high frequencies.

Reduce color. The image is reduced to a grayscale just to further simplify the number of computations.
Compute the DCT. The DCT separates the image into a collection of frequencies and scalars. While JPEG uses an 8x8 DCT, this algorithm uses a 32x32 DCT.
Reduce the DCT. While the DCT is 32x32, just keep the top-left 8x8. Those represent the lowest frequencies in the picture.
Compute the average value. Like the Average Hash, compute the mean DCT value (using only the 8x8 DCT low-frequency values and excluding the first term since the DC coefficient can be significantly different from the other values and will throw off the average). Thanks to David Starkweather for the added information about pHash. He wrote: "the dct hash is based on the low 2D DCT coefficients starting at the second from lowest, leaving out the first DC term. This excludes completely flat image information (i.e. solid colors) from being included in the hash description."
Further reduce the DCT. This is the magic step. Set the 64 hash bits to 0 or 1 depending on whether each of the 64 DCT values is above or below the average value. The result doesn't tell us the actual low frequencies; it just tells us the very-rough relative scale of the frequencies to the mean. The result will not vary as long as the overall structure of the image remains the same; this can survive gamma and color histogram adjustments without a problem.
Construct the hash. Set the 64 bits into a 64-bit integer. The order does not matter, just as long as you are consistent. To see what this fingerprint looks like, simply set the values (this uses +255 and -255 based on whether the bits are 1 or 0) and convert from the 32x32 DCT (with zeros for the high frequencies) back into the 32x32 image:
 = 8a0303f6df3ec8cd
At first glance, this might look like some random blobs... but look closer. There is a dark ring around her head and the dark horizontal line in the background (right side of the picture) appears as a dark spot.
As with the Average Hash, pHash values can be compared using the same Hamming distance algorithm. (Just compare each bit position and count the number of differences.)

Best in Class?
Since I do a lot of work with digital photo forensics and huge picture collections, I need a way to search for similar pictures. So, I created a picture search tool that uses a couple of different perceptual hash algorithms. In my unscientific but long-term-use experience, I have found that Average Hash is significantly faster than pHash. Average Hash is a great algorithm if you are looking for something specific. For example, if I have a small thumbnail of an image and I know that the big one exists somewhere in my collection, then Average Hash will find it very quickly. However, if there are modifications -- like text was added or a head was spliced into place, then Average Hash probably won't do the job. While pHash is slower, it is very tolerant of minor modifications (minor being less than 25% of the picture).

Then again, if you are running a service like TinEye, then you're not going to compute the pHash every time. I am certain that they have a database of pre-computed hash values. The basic comparison system is extremely fast. (There are some heavily optimized ways to compute a Hamming distance.) So computing the hash is a one-time cost and doing a million comparisons in a few seconds (on one computer) is very realistic.

Variations
There are variations to the perceptual hash algorithm that can also improve performance. For example, the image can be cropped before being reduced in size. This way, extra empty space around the main part of the image won't make a difference. Also, the image can be segmented. For example, if you have a face detection algorithm, then you can compute hashes for each face. (I suspect that TinEye's algorithm does something similar.)

Other variations can track general coloring (e.g., her hair is more red than blue or green, and the background is closer to white than black) or the relative location of lines.

When you can compare images, then you can start doing really cool things. For example, the search engine GazoPa [now offline] allows you to draw a picture. As with TinEye, I don't know the details about how GazoPa works. However, it appears to use a variation of the perceptual hash. Since the hash reduces everything down to the lowest frequencies, my crappy line drawing of three stick figures can be compared with other pictures -- likely matching photos that contain three people.

About 8 months ago I wrote a blog entry on algorithms for comparing pictures. Basically, if you have a large database of pictures and want to find similar images, then you need an algorithm that generates a weighted comparison. In that blog entry, I described how two of the algorithms work:
aHash (also called Average Hash or Mean Hash). This approach crushes the image into a grayscale 8x8 image and sets the 64 bits in the hash based on whether the pixel's value is greater than the average color for the image.

pHash (also called "Perceptive Hash"). This algorithm is similar to aHash but use a discrete cosine transform (DCT) and compares based on frequencies rather than color values.
As a comment to the blog entry, David Oftedal suggested a third option that he called a "difference hash". It took me 6 months to get back to evaluating hash functions and dHash is a definite winner.

dHash
Like aHash and pHash, dHash is pretty simple to implement and is far more accurate than it has any right to be. As an implementation, dHash is nearly identical to aHash but it performs much better. While aHash focuses on average values and pHash evaluates frequency patterns, dHash tracks gradients. Here's how the algorithm works, using the same Alyson Hannigan image as last time:

Reduce size. The fastest way to remove high frequencies and detail is to shrink the image. In this case, shrink it to 9x8 so that there are 72 total pixels. (I'll get to the "why" for the odd 9x8 size in a moment.) By ignoring the size and aspect ratio, this hash will match any similar picture regardless of how it is stretched.
Reduce color. Convert the image to a grayscale picture. This changes the hash from 72 pixels to a total of 72 colors. (For optimal performance, either reduce color before scaling or perform the scaling and color reduction at the same time.)
Compute the difference. The dHash algorithm works on the difference between adjacent pixels. This identifies the relative gradient direction. In this case, the 9 pixels per row yields 8 differences between adjacent pixels. Eight rows of eight differences becomes 64 bits.
Assign bits. Each bit is simply set based on whether the left pixel is brighter than the right pixel. The order does not matter, just as long as you are consistent. (I use a "1" to indicate that P[x] < P[x+1] and set the bits from left to right, top to bottom using big-endian.)
 =  = 3a6c6565498da525
As with aHash, the resulting hash won't change if the image is scaled or the aspect ratio changes. Increasing or decreasing the brightness or contrast, or even altering the colors won't dramatically change the hash value. Even complex adjustments like gamma corrections and color profiles won't impact the result. And best of all: this is FAST! Seriously -- the slowest part of the algorithm is the size reduction step.

The hash values represent the relative change in brightness intensity. To compare two hashes, just count the number of bits that are different. (This is the Hamming distance.) A value of 0 indicates the same hash and likely a similar picture. A value greater than 10 is likely a different image, and a value between 1 and 10 is potentially a variation.

Speed and Accuracy
From FotoForensics, we now have a testbed of over 150,000 images. I have a couple of test images that occur a known number of times. For example, one picture (needle) appears exactly once in the 150,000 image repository (haystack). Another picture occurs twice. A third test picture currently occurs 32 times.

I've used aHash, pHash, and dHash to search for the various needles in the haystack. For comparisons, I did not pre-cache any of the repository hash values. I also consider a cutoff value of 10 to denote a match or a miss. (If the haystack image differs from the needle image by more than 10 bits, then it is assumed to not match.) Here's the results so far:

No hash. This is a baseline for comparison. It loads each image into memory, and then unloads the image. This tells me how much time is spent just on the file access and loading. (And all images are located on an NFS-mounted directory -- so this includes network transfer times.) The total time is 16 minutes. Without any image comparisons, there is a minimum of 16 minutes needed just to load each image.

No hash, Scale. Every one of these hash algorithms begins by scaling the image smaller. Small pictures scale very quickly, but large pictures can take 10 seconds or more. Just loading and scaling the 150,000 images takes 3.75 hours. (I really need to look into possible methods for optimizing my bilinear scaling algorithm.)

aHash. This algorithm takes about 3.75 hours to run. In other words, it takes more time to load and scale the image than to run the algorithm. Unfortunately, aHash generates a huge number of false positives. It matched all of the expected images, but also matched about 10x more false-positives. For example, the test picture that should have matched 32 times actually matched over 400 images. Worse: some of the misses had a difference of less than 2 bits. In general, aHash is fast but not very accurate.

pHash. This algorithm definitely performed the best with regards to accuracy. No false positives, no false negatives, and every match has a score of 2 or less. I'm sure that a bigger data set (or alternate needle image) will generate false matches, but the number of misses will likely be substantially less than aHash.

The problem with pHash is the performance. It took over 7 hours to complete. This is because the DCT computation uses lots of operations including cosine and sine. If I pre-compute the DCT constants, then this will drop 1-2 hours from the overall runtime. But applying the DCT coefficients still takes time. pHash is accurate, but not very fast.

dHash. Absolutely amazing... Very few false positives. For example, the image with two known matches ended up matching 6 pictures total (4 false positives). The scores were: 10, 0, 8, 10, 0, and 10. The two zeros were the correct matches; all of the false-positive matches had higher scores. As speed goes, dHash is as fast as aHash. Well, technically it is faster since it doesn't need to compute the mean color value. The dHash algorithm has all the speed of aHash with very few false-positives.
Algorithm Variations
I have tried a few variations of the dHash algorithm. For example, David's initial proposal used an 8x8 image and wrapped the last comparison (computing the pixel difference between P[0] and P[7] for the 8th comparison). This actually performs a little worse than my 9x8 variation (more false-positives), but only by a little.

Storing values by row or column really doesn't make a difference. Computing both row and column hashes significantly reduces the number of false-positives and is comparable to pHash (almost as accurate). So it maintains speed and gains accuracy as the cost of requiring 128 bits for the hash.

I've also combined pHash with dHash. Basically, I use the really fast dHash as a fast filter. If dHash matches, then I compute the more expensive pHash value. This gives me all the speed of dHash with all the accuracy of pHash.

Finally, I realized that using dHash as a fast filter is good, but I don't need 64-bits for this computation. My 16-bit dHash variant uses a 6x4 reduced image. This gives me 20 difference values. Ignoring the four corners yields a 16-bit hash -- and has the benefit of ignoring the impact from Instagram-like vignetting (corner darkening). If I have a million different images, then I should expect about 15 images per 16-bit dHash. pHash can compare 15 images really quickly. At a billion images, I'm looking at about 15,258 image collisions and that still is a relatively small number.

I can even permit my 16-bit dHash to be sloppy; permitting any 1-bit change to match. Any computed 16-bit dHash would yield 17 possible dHash values to match. A million images should yield about 260 collisions, and a billion becomes about 260,000 collisions. At a billion images, it would be worth storing the 16-bit dHash, 64-bit dHash, and 64-bit pHash values. But a million images? I'm still ahead just by pre-computing the 16-bit dHash and 64-bit pHash values.

Applied Comparisons
There are two things we want out of a perceptual hash algorithm: speed and accuracy. By combining dHash with pHash, we get both. But even without pHash, dHash is still a significant improvement over aHash and without any notable speed difference.
*/



//I've asked this question 5 years ago. This thread was very helpful for me back then, I came to a conclusion, 
//then I moved on with my project. It is funny that I needed something similar recently, totally unrelated to that project from the past. 
//As I was researching for possible solutions, I stumbled upon my own question :)

//The solution I chose now is based on C++11. 
//The boost libraries that Constantin mentions in his answer are now part of the standard.
//If we replace std::wstring with the new string type std::u16string, then the conversions will look like this:
/*
void convertUTF8ToUTF16CodeCvt()
{
    //UTF - 8 to UTF - 16

    std::string source;

    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    std::u16string dest = convert.from_bytes(source);

}

void convertUTF16ToUTF8CodeCvt()
{
    //UTF - 16 to UTF - 8

    std::u16string source;

    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    std::string dest = convert.to_bytes(source);
}
*/



/*
int main()
{
    const WCHAR* wText = L"Wide string";
    const char* text = convert_from_wstring(wText);
    std::cout << text << "\n";
    free(text);

    const WCHAR *wtext = convert_to_wstring("Multibyte string");
    std::wcout << wtext << "\n";
    free(wtext);

    return 0;
}
*/


/*
//If you are using C++11 or later, have a look at the std::wstring_convert class for converting between UTF strings, eg :

std::wstring convert_to_wstring(const std::string & str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> conv;
    return conv.from_bytes(str);
}

std::string convert_from_wstring(const std::wstring & wstr)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> conv;
    return conv.to_bytes(wstr);
}
//If you need to interact with other code that is based on char* / wchar_t*,
//std::string as a constructor for accepting char* inputand a c_str() method that can be used for char* output,
//and the same goes for std::wstringand wchar_t* .
*/


/*
#include <iconv.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <errno.h>

using namespace std;

const size_t BUF_SIZE = 1024;

class IConv {

    iconv_t ic_;

public:
    IConv(const char* to, const char* from)
        : ic_(iconv_open(to, from)) {
        if (iconv_t(-1) == ic_)
            throw std::runtime_error("error from iconv_open()");
    }

    ~IConv()
    {
        if (iconv_t(-1) != ic_) iconv_close(ic_);
    }

    bool convert(char* input, char* output, size_t& out_size) {
        size_t inbufsize = strlen(input) + 1;
        return
            (size_t(-1)
                != iconv(ic_, &input, &inbufsize, &output, &out_size))
            && (0U == inbufsize);
    }
};




int main(void)
{
    char str1[BUF_SIZE] = "tägelîch";
    char str2[BUF_SIZE] = "something else";
    IConv ic("UTF16LE", "ISO_8859-1");
    bool ret;
    size_t outsize = BUF_SIZE;

    ret = ic.convert(str1, str2, outsize);
    if (ret == false) {
        cout << "iconv failed: " << errno << endl;
    }
    else {
       cout << "outsize[" << outsize << "]\n";
       cout << "str1[" << str1 << "]\n";
       cout << "str2[" << str2 << "]\n";

       for ( int i = 0 ; i < (BUF_SIZE - outsize) ; ++i )
          if ( str2[i] )
             cout << "str2[" << i << "]=[" << int(str2[i]) << "]("
                << str2[i] << ")\n";
    }

    return ret ? EXIT_SUCCESS : EXIT_FAILURE;
}


6.5.2 A complete iconv example
The example below features a solution for a common problem.
Given that one knows the internal encoding used by the system for wchar_t strings,
one often is in the position to read text from a file and store it in wide character buffers.
One can do this using mbsrtowcs, but then we run into the problems discussed above.

int file2wcs (int fd, const char *charset, wchar_t *outbuf, size_t avail)
{
  char inbuf[BUFSIZ];
  size_t insize = 0;
  char *wrptr = (char *) outbuf;
  int result = 0;
  iconv_t cd;

  cd = iconv_open ("WCHAR_T", charset);
  if (cd == (iconv_t) -1)
    {
     // Something went wrong.
if (errno == EINVAL)
error(0, 0, "conversion from '%s' to wchar_t not available",
    charset);
else
perror("iconv_open");

// Terminate the output string.
*outbuf = L'\0';

return -1;
    }

    while (avail > 0)
    {
        size_t nread;
        size_t nconv;
        char* inptr = inbuf;

        // Read more input.
        nread = read(fd, inbuf + insize, sizeof(inbuf) - insize);
        if (nread == 0)
        {
            // When we come here the file is completely read.
            //   This still could mean there are some unused
            //   characters in the inbuf.  Put them back.
            if (lseek(fd, -insize, SEEK_CUR) == -1)
                result = -1;

            //Now write out the byte sequence to get into the
            //   initial state if this is necessary.
            iconv(cd, NULL, NULL, &wrptr, &avail);

            break;
        }
        insize += nread;

        //Do the conversion.
        nconv = iconv(cd, &inptr, &insize, &wrptr, &avail);
        if (nconv == (size_t)-1)
        {
            // Not everything went right.  It might only be
            //   an unfinished byte sequence at the end of the
             //  buffer.  Or it is a real problem.
            if (errno == EINVAL)
                // This is harmless.  Simply move the unused
                 //  bytes to the beginning of the buffer so that
                 //  they can be used in the next round.
                memmove(inbuf, inptr, insize);
            else
            {
                // It is a real problem.  Maybe we ran out of
                //   space in the output buffer or we have invalid
                //   input.  In any case back the file pointer to
                 //  the position of the last processed byte.
                lseek(fd, -insize, SEEK_CUR);
                result = -1;
                break;
            }
        }
    }

    // Terminate the output string.
    if (avail >= sizeof(wchar_t))
        *((wchar_t*)wrptr) = L'\0';

    if (iconv_close(cd) != 0)
        perror("iconv_close");

    return (wchar_t*)wrptr - outbuf;
}
This example shows the most important aspects of using the iconv functions.It shows how successive
calls to iconv can be used to convert large amounts of text.The user does not have to care about
stateful encodings as the functions take care of everything.

An interesting point is the case where iconv returns an errorand errno is set to EINVAL.
This is not really an error in the transformation.It can happen whenever the input character set
contains byte sequences of more than one byte for some characterand texts are not processed in one piece.
In this case there is a chance that a multibyte sequence is cut.The caller can then simply read the remainder
of the takesand feed the offending bytes together with new character from the input to iconvand continue the work.
The internal state kept in the descriptor is not unspecified after such an event as is the case with the conversion
functions from the ISO C standard.

The example also shows the problem of using wide character strings with iconv.As explained in the description of the
iconv function above, the function always takes a pointer to a char arrayand the available space is measured in bytes.
In the example, the output buffer is a wide character buffer; therefore, we use a local variable wrptr of type char*,
which is used in the iconv calls.

This looks rather innocent but can lead to problems on platforms that have tight restriction on alignment.
Therefore the caller of iconv has to make sure that the pointers passed are suitable for access of characters
from the appropriate character set.Since, in the above case, the input parameter to the function is a wchar_t pointer,
this is the case (unless the user violates alignment when computing the parameter).But in other situations, especially
when writing generic functions where one does not know what type of character set one usesand, therefore, treats text
as a sequence of bytes, it might become tricky.



*/



//__int64 FileSizeStat(const wchar_t* name)
//{
//    __stat64 buff;
//    if (_wstat64(name, &buff) != 0)
//        return -1; // error, could use errno to find out more
//
//    return buff.st_size;
//}



/*
void convertFromChar()
{
    //Example: Convert from char*
    //This example demonstrates how to convert from a char* to the string types listed above.
    //A char* string(also known as a C - style string) uses a terminating null to indicate the end of the string.
    //C - style strings usually require 1 byte per character, but can also use 2 bytes.
    //In the examples below, char* strings are sometimes referred to as multibyte character strings because of the string data
    //that results from converting from wide Unicode strings.Single byteand multibyte character(MBCS) functions can operate on char* strings.

    using namespace std;


    // Create and display a C-style string, and then use it
    // to create different kinds of strings.
    const char* orig = "Hello, World!";
    cout << orig << " (char *)" << endl;

    // newsize describes the length of the
    // wchar_t string called wcstring in terms of the number
    // of wide characters, not the number of bytes.
    size_t newsize = strlen(orig) + 1;

    // The following creates a buffer large enough to contain
    // the exact number of characters in the original string
    // in the new format. If you want to add more characters
    // to the end of the string, increase the value of newsize
    // to increase the size of the buffer.
    wchar_t* wcstring = new wchar_t[newsize];

    // Convert char* string to a wchar_t* string.
    size_t convertedChars = 0;
    mbstowcs_s(&convertedChars, wcstring, newsize, orig, _TRUNCATE);
    // Display the result and indicate the type of string that it is.
    wcout << wcstring << L" (wchar_t *)" << endl;
    delete[]wcstring;

    // Convert the C-style string to a _bstr_t string.
    _bstr_t bstrt(orig);
    // Append the type of string to the new string
    // and then display the result.
    bstrt += " (_bstr_t)";
    wcout << bstrt << endl;

    // Convert the C-style string to a CComBSTR string.
    CComBSTR ccombstr(orig);
    if (ccombstr.Append(L" (CComBSTR)") == S_OK)
    {
        CW2A printstr(ccombstr);
        cout << printstr << endl;
    }

    // Convert the C-style string to a CStringA and display it.
    CStringA cstringa(orig);
    cstringa += " (CStringA)";
    cout << cstringa << endl;

    // Convert the C-style string to a CStringW and display it.
    CStringW cstring(orig);
    cstring += " (CStringW)";
    // To display a CStringW correctly, use wcout and cast cstring
    // to (LPCTSTR).
    wcout << (LPCTSTR)cstring << endl;

    // Convert the C-style string to a basic_string and display it.
    string basicstring(orig);
    basicstring += " (basic_string)";
    cout << basicstring << endl;

    // Convert the C-style string to a System::String and display it.
    //System::String^ systemstring = gcnew String(orig);
    //systemstring += " (System::String)";
    //Console::WriteLine("{0}", systemstring);
    //delete systemstring;
}
*/


/*
void convertFromWChar()
{

    //Example: Convert from wchar_t*
    //This example demonstrates how to convert from a wchar_t* to other string types.
    //Several string types, including wchar_t*, implement wide character formats.
    //To convert a string between a multibyteand a wide character format, you can use a single function call like mbstowcs_s or a
    //constructor invocation for a class like CStringA.

    using namespace std;

    // Create a string of wide characters, display it, and then
    // use this string to create other types of strings.
    const wchar_t* orig = L"Hello, World!";
    wcout << orig << L" (wchar_t *)" << endl;

    // Convert the wchar_t string to a char* string. Record
    // the length of the original string and add 1 to it to
    // account for the terminating null character.
    size_t origsize = wcslen(orig) + 1;
    size_t convertedChars = 0;

    // Use a multibyte string to append the type of string
    // to the new string before displaying the result.
    char strConcat[] = " (char *)";
    size_t strConcatsize = (strlen(strConcat) + 1) * 2;

    // Allocate two bytes in the multibyte output string for every wide
    // character in the input string (including a wide character
    // null). Because a multibyte character can be one or two bytes,
    // you should allot two bytes for each character. Having extra
    // space for the new string isn't an error, but having
    // insufficient space is a potential security problem.
    const size_t newsize = origsize * 2;
    // The new string will contain a converted copy of the original
    // string plus the type of string appended to it.
    char* nstring = new char[newsize + strConcatsize];

    // Put a copy of the converted string into nstring
    wcstombs_s(&convertedChars, nstring, newsize, orig, _TRUNCATE);
    // append the type of string to the new string.
    _mbscat_s((unsigned char*)nstring, newsize + strConcatsize, (unsigned char*)strConcat);
    // Display the result.
    cout << nstring << endl;
    delete[]nstring;

    // Convert a wchar_t to a _bstr_t string and display it.
    _bstr_t bstrt(orig);
    bstrt += " (_bstr_t)";
    wcout << bstrt << endl;

    // Convert the wchar_t string to a BSTR wide character string
    // by using the ATL CComBSTR wrapper class for BSTR strings.
    // Then display the result.

    CComBSTR ccombstr(orig);
    if (ccombstr.Append(L" (CComBSTR)") == S_OK)
    {
        // CW2A converts the string in ccombstr to a multibyte
        // string in printstr, used here for display output.
        CW2A printstr(ccombstr);
        cout << printstr << endl;
        // The following line of code is an easier way to
        // display wide character strings:
        wcout << (LPCTSTR)ccombstr << endl;
    }

    // Convert a wide wchar_t string to a multibyte CStringA,
    // append the type of string to it, and display the result.
    CStringA cstringa(orig);
    cstringa += " (CStringA)";
    cout << cstringa << endl;

    // Convert a wide character wchar_t string to a wide
    // character CStringW string and append the type of string to it
    CStringW cstring(orig);
    cstring += " (CStringW)";
    // To display a CStringW correctly, use wcout and cast cstring
    // to (LPCTSTR).
    wcout << (LPCTSTR)cstring << endl;

    // Convert the wide character wchar_t string to a
    // basic_string, append the type of string to it, and
    // display the result.
    wstring basicstring(orig);
    basicstring += L" (basic_string)";
    wcout << basicstring << endl;

    // Convert a wide character wchar_t string to a
    // System::String string, append the type of string to it,
    // and display the result.
    //String^ systemstring = gcnew String(orig);
    //systemstring += " (System::String)";
    //Console::WriteLine("{0}", systemstring);
    //delete systemstring;
}
*/

/*
void convertFromBasicString()
{
    //Example: Convert from basic_string
    //This example demonstrates how to convert from a basic_string to other string types.

    using namespace std;

    // Set up a basic_string string.
    string orig("Hello, World!");
    cout << orig << " (basic_string)" << endl;

    // Convert a wide character basic_string string to a multibyte char*
    // string. To be safe, we allocate two bytes for each character
    // in the original string, including the terminating null.
    const size_t newsize = (orig.size() + 1) * 2;

    char* nstring = new char[newsize];
    strcpy_s(nstring, newsize, orig.c_str());
    cout << nstring << " (char *)" << endl;
    delete[]nstring;

    // Convert a basic_string string to a wide character
    // wchar_t* string. You must first convert to a char*
    // for this to work.
    const size_t newsizew = orig.size() + 1;
    size_t convertedChars = 0;
    wchar_t* wcstring = new wchar_t[newsizew];
    mbstowcs_s(&convertedChars, wcstring, newsizew, orig.c_str(), _TRUNCATE);
    wcout << wcstring << L" (wchar_t *)" << endl;
    delete[]wcstring;

    // Convert a basic_string string to a wide character
    // _bstr_t string.
    _bstr_t bstrt(orig.c_str());
    bstrt += L" (_bstr_t)";
    wcout << bstrt << endl;

    // Convert a basic_string string to a wide character
    // CComBSTR string.
    CComBSTR ccombstr(orig.c_str());
    if (ccombstr.Append(L" (CComBSTR)") == S_OK)
    {
        // Make a multibyte version of the CComBSTR string
        // and display the result.
        CW2A printstr(ccombstr);
        cout << printstr << endl;
    }

    // Convert a basic_string string into a multibyte
    // CStringA string.
    CStringA cstring(orig.c_str());
    cstring += " (CStringA)";
    cout << cstring << endl;

    // Convert a basic_string string into a wide
    // character CStringW string.
    CStringW cstringw(orig.c_str());
    cstringw += L" (CStringW)";
    wcout << (LPCTSTR)cstringw << endl;

    // Convert a basic_string string to a System::String
    //String^ systemstring = gcnew String(orig.c_str());
    //systemstring += " (System::String)";
    //Console::WriteLine("{0}", systemstring);
    //delete systemstring;
}
*/


/*
#include <locale>
#include <codecvt>
#include <string>

std::wstring convertNarrowUTF8StringToWideUTF16String(std::string narrow_utf8_source_string)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wide = converter.from_bytes(narrow_utf8_source_string);
    return wide;
}

std::string convertWideUTF16StringToNarrowUTF8String(std::wstring wide_utf16_source_string)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::string narrow = converter.to_bytes(wide_utf16_source_string);
    return narrow;

}
*/

