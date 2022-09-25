#pragma once



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




static std::string convertWideToANSI(const std::wstring& wstr)
{
    int count = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), (int)wstr.length(), NULL, 0, NULL, NULL);
    std::string str(count, 0);
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
    return str;
}

static std::wstring convertAnsiToWide(const std::string& str)
{
    int count = MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.length(), NULL, 0);
    std::wstring wstr(count, 0);
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.length(), &wstr[0], count);
    return wstr;
}

static std::string convertWideToUtf8(const std::wstring& wstr)
{
    int count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), NULL, 0, NULL, NULL);
    std::string str(count, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
    return str;
}

static std::wstring convertUtf8ToWide(const std::string& str)
{
    int count = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), NULL, 0);
    std::wstring wstr(count, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), &wstr[0], count);
    return wstr;
}

static std::wstring convertStringToWStringUsingFilesystem(std::string s)
{
    return std::filesystem::path(s).wstring();
}

static std::wstring convertWStringToStringUsingFilesystem(std::wstring w)
{
    return std::filesystem::path(w).wstring();
}







#include <QtWidgets/QMainWindow>
#include "ui_OpenFileOrganizer.h"


#include "stdio.h"
#include "iostream"

#include <iostream>
#include <streambuf>
#include <string>
#include <QPlainTextEdit>
#include <qthread.h>





class QDebugStream : public std::wstreambuf
{
public:
    QDebugStream(std::wostream& stream, QPlainTextEdit* text_edit)
        : m_stream(stream)
    {
        log_window = text_edit;
        m_old_buf = stream.rdbuf();
        stream.rdbuf(this);
    }
    ~QDebugStream()
    {

        // output anything that is left
        if (!m_string.empty()) 
        {
            QString qs;
            qs += m_string;
            log_window->moveCursor(QTextCursor::End);
            log_window->appendPlainText(qs);
        }

        m_stream.rdbuf(m_old_buf);
    }

protected:
    virtual int_type overflow(int_type v)
    {

        if (v == '\n')
        {
            QString qs;
            qs += m_string;
            log_window->moveCursor(QTextCursor::End);
            log_window->appendPlainText(qs);
            m_string.erase(m_string.begin(), m_string.end());
        }
        else
            m_string += v;

        return v;
    }

    virtual std::streamsize xsputn(const wchar_t* p, std::streamsize n)
    {
        m_string.append(p, p + n);
        //QString qs;
        //qs.fromStdWString(m_string);
        //qs += m_string;
        
        //log_window->appendPlainText(qs);
        //m_string.erase();
        //return n;

        int pos = 0;
        while (pos != std::wstring::npos)
        {
            pos = (int)(m_string.find(L"\n"));
            if (pos != std::wstring::npos)
            {
                std::wstring tmp(m_string.begin(), m_string.begin() + pos);

                QString qs;
                qs += tmp;
                log_window->moveCursor(QTextCursor::End);
                log_window->appendPlainText(qs);

                m_string.erase(m_string.begin(), m_string.begin() + pos + 1);
            }
        }

        return n;
    }

private:
    std::wostream& m_stream;
    std::wstreambuf* m_old_buf;
    std::wstring m_string;


    QPlainTextEdit* log_window;
};

class MessageHandler : public QObject
{
    Q_OBJECT
public:
    MessageHandler(QPlainTextEdit* textEdit, QObject* parent = Q_NULLPTR) : QObject(parent), m_textEdit(textEdit) {}

public slots:
    void catchMessage(QString msg)
    {
        this->m_textEdit->appendPlainText(msg);
    }
    
private:
    QPlainTextEdit* m_textEdit;
};



class ThreadLogStream : public QObject, public std::wstreambuf
{
    Q_OBJECT
public:
    ThreadLogStream(std::wostream& stream, QObject* parent = Q_NULLPTR) :QObject(parent), m_stream(stream)
    {
        m_old_buf = stream.rdbuf();
        stream.rdbuf(this);
    }
    ~ThreadLogStream()
    {
        // output anything that is left
        if (!m_string.empty())
        {
            QString qs;
            qs += m_string;
            emit sendLogString(qs);// QString::fromStdWString(m_string));
        }
        m_stream.rdbuf(m_old_buf);
    }
protected:
    virtual int_type overflow(int_type v)
    {
        if (v == '\n')
        {
            QString qs;
            qs += m_string;
            emit sendLogString(qs);// QString::fromStdString(m_string));
            m_string.erase(m_string.begin(), m_string.end());
        }
        else
            m_string += v;
        return v;
    }
    virtual std::streamsize xsputn(const wchar_t* p, std::streamsize n)
    {
        m_string.append(p, p + n);
        size_t pos = 0;
        while (pos != std::wstring::npos)
        {
            pos = m_string.find(L"\n");
            if (pos != std::wstring::npos)
            {
                std::wstring tmp(m_string.begin(), m_string.begin() + pos);

                QString qs;
                qs += tmp;

                emit sendLogString(qs);// QString::fromStdString(tmp));
                m_string.erase(m_string.begin(), m_string.begin() + pos + 1);
            }
        }
        return n;
    }
private:
    std::wostream& m_stream;
    std::wstreambuf* m_old_buf;
    std::wstring m_string;
signals:
    void sendLogString(const QString& str);
};



class OpenFileOrganizer : public QMainWindow
{
    Q_OBJECT

public:
    OpenFileOrganizer(QWidget* parent = nullptr);
    ~OpenFileOrganizer();

    static  void QMessageOutput(QtMsgType, const QMessageLogContext&, const QString& msg);


private:
    Ui::OpenFileOrganizerClass ui;

    // MessageHandler for display and ThreadLogStream for redirecting cout
    
    MessageHandler* msgHandler = Q_NULLPTR;
    ThreadLogStream* m_qd;

    const QString errorString = "error";

private slots:
    void handleButton();

};





class Worker : public QObject
{
    Q_OBJECT
public:
    
    Worker()//QPlainTextEdit *consoleOutputPlainTextEdit)
    {
        
        //plainTextEdit = consoleOutputPlainTextEdit;
         //qout = new QDebugStream(std::wcout, plainTextEdit);
         //std::wcout << L"Hello this is a test" << std::endl;

        //db = NULL;
    }
    vector<FileDataEntry> fileDataEntries;
    std::chrono::steady_clock::time_point start;
    bool isWindows = false;

//#define DB_LOCATION L"locate.db"
    //FILE* db;
    //QPlainTextEdit* plainTextEdit;
    //QDebugStream* qout;


public slots:
    void process();
signals:
    void finished();
    //void error(QString err);


private:

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




    void directoryIteratorRecursive(const std::filesystem::path& dir_path, _int64& filecount)
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

    void ListFilesWindowsFindFirstFile(const wstring originalPath, _int64& filecount)
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
                else std::wcout << L"FindFirstFileExW Error " << error << L" for " << path << std::endl;
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
        max += 1;
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
        max += 1;

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

    void getFileCreatedModifiedDateWStat(FileDataEntry* f)
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


                    int n = (int)(matches[0].str().find_first_of("0123456789"));

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
                        is >> m;
                    }
                    {
                        std::istringstream is(matches[0].str().substr(n + 8, 2));
                        is >> d;
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

                        int n = (int)(matches[0].str().find_first_of("0123456789"));


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
                            is >> m;
                        }
                        {
                            std::istringstream is(matches[0].str().substr(n + 6, 2));
                            is >> d;
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

                            int n = (int)(matches[0].str().find_first_of("0123456789"));

                            {
                                std::istringstream is(matches[0].str().substr(n, 2));
                                is >> m;
                            }

                            {
                                std::istringstream is(matches[0].str().substr(n + 3, 2));
                                is >> d;
                            }

                            if (matches[0].str().substr(n + 6, 2) == "19" || matches[0].str().substr(n + 6, 2) == "20")
                            {
                                std::istringstream is(matches[0].str().substr(n + 6, 4));
                                is >> y;
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

                                int n = (int)(matches[0].str().find_first_of("0123456789"));

                                if (matches[0].str().substr(n, 1) != "1" && matches[0].str().substr(n, 1) != "2")
                                {
                                    if (matches[0].str().substr(n + 1, 1) != "1" && matches[0].str().substr(n + 1, 1) != "2")
                                    {
                                        //can't parse?
                                    }
                                    else
                                    {
                                        std::istringstream is(matches[0].str().substr(n + 1, 4));
                                        is >> y;
                                    }
                                }
                                else
                                {
                                    std::istringstream is(matches[0].str().substr(n, 4));
                                    is >> y;
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

                                    int n = (int)(matches[0].str().find_first_of("0123456789"));
                                    {
                                        if (matches[0].str().find_first_of("0123456789", n + 1) == n + 1)
                                        {
                                            std::istringstream is(matches[0].str().substr(n, 2));
                                            is >> d;
                                        }
                                        else
                                        {
                                            std::istringstream is(matches[0].str().substr(n, 1));
                                            is >> d;
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

                                        int n = (int)(matches[0].str().find_first_of("0123456789"));

                                        if (matches[0].str().find_first_of("0123456789", n + 1) == n + 1)
                                        {
                                            std::istringstream is(matches[0].str().substr(n, 2));
                                            is >> d;
                                        }
                                        else
                                        {
                                            std::istringstream is(matches[0].str().substr(n, 1));
                                            is >> d;
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


    void go()
    {

#ifdef WIN32
        isWindows = true;
#else
        isWindows = false;
#endif

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


        wstring startpath = L"C:\\";
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

        if (false)
            getFastHashForAllFiles();

        if (false)
            getDateFromFilenameForAllFiles();

        if (false)
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

                if (size > 1024 * 1024)
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
                                //std::wcout << L"Possible match: " << fileDataEntries[i].nameAndPath << L" " << fileDataEntries[j].nameAndPath << L" " << fileDataEntries[i].size << std::endl;

                                if (fileDataEntries[i].name == fileDataEntries[j].name)
                                {
                                    //std::wcout << L"Same filename " << fileDataEntries[i].name << std::endl; 
                                }
                                else
                                {
                                    std::wcout << L"Different filename " << fileDataEntries[i].name << L" " << fileDataEntries[j].name << std::endl;
                                }

                                if (
                                    fileDataEntries[i].crc32 == fileDataEntries[j].crc32 &&
                                    fileDataEntries[i].md5 == fileDataEntries[j].md5 &&
                                    fileDataEntries[i].sha1 == fileDataEntries[j].sha1 &&
                                    fileDataEntries[i].sha2 == fileDataEntries[j].sha2 &&
                                    fileDataEntries[i].keccak == fileDataEntries[j].keccak &&
                                    fileDataEntries[i].sha3 == fileDataEntries[j].sha3
                                    )
                                {
                                    //std::wcout << L"All hashes match!" << std::endl;
                                }
                                else
                                {
                                    std::wcout << L"Hashes different" << std::endl;

                                    //match comparisons run full hash and byte comparison

                                    //TODO: check full hash

                                    //TODO: do byte comparison




                                }
                            }
                        }
                        else
                        {
                            j = (int)(fileDataEntries.size());
                            break;
                        }
                    }
            }
        }







        //video file metadata
        //music file metadata
        //pdf and document metadata




        //use library to compare images with fuzzy comparison, different resolution comparison, use different methods to ensure accuracy



        //keep all filenames and dates, compare with filename date, exif dates, determine most correct timestamp for image and duplicates



        //keep higher quality version, optionally delete others

        //rename files with most accurate date, optionally reset timestamps
        //store all filenames and dates etc in metadata


        //save and load database with metadata




        //gui
        //filter by file type
        //add directories, subdirectories
        //sort by size, name, matches, type
        //match in same directory
        //operations (rename, remove duplicates, merge metadata)

        //file renamer


        //ocr

        //ai image object classification

        //ai ocr

        //ai video comparison, object classification








        //somehow make part of filesystem or explorer extension, run system wide search

        //switch to linux, test program, embed in gnome?




    }

};


