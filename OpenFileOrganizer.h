#include <chrono>
#include <cstdint>
#include <ctime>
#include <filesystem>
#include <fstream>  // std::ifstream
#include <functional>
#include <iomanip>  // std::setprecision
#include <iostream> // std::cout
#include <limits>
#include <memory>
#include <regex>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <streambuf>
#include <string.h>
#include <string>
#include <unordered_set>
#include <vector>   // std::vector
#include <wchar.h>


#include "atlbase.h"
#include "atlstr.h"
#include "comutil.h"

#include <cwctype>


//void ok();


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

#include <dirent.h>

#include "crc32.h"
#include "md5.h"
#include "sha1.h"
#include "sha256.h"
#include "keccak.h"
#include "sha3.h"

#include "libexif/exif.h"
#include "libexif/exif-data.h"
#include "libexif/exif-system.h"

#include "TinyEXIF.h"

#include "imageinfo.hpp"
#define II_DISABLE_HEADER_CACHE

#include "jpeglib.h"
#include <setjmp.h>


#include "sqlite3.h"



std::string convertWideToANSI(const std::wstring& wstr);


std::wstring convertAnsiToWide(const std::string& str);


std::string convertWideToUtf8(const std::wstring& wstr);


std::wstring convertUtf8ToWide(const std::string& str);


class MyDate
{
public:



	MyDate(wstring dateString)
	{

	}

	
};


class DateData
{
public:

	wstring dateString = L"";
	int year = 0;
	int month = 0;
	int day = 0;
	int hour = 0;
	int minute = 0;
	int second = 0;

	DateData(wstring dateString)
	{
		this->dateString = dateString;

		string str = convertWideToUtf8(dateString);
	
		size_t k = str.find_first_of("0123456789");

		{std::istringstream is(str.substr(k, 4));
		is >> year; }

		{std::istringstream is(str.substr(k + 5, 2));
		is >> month; }

		{std::istringstream is(str.substr(k + 8, 2));
		is >> day; }
		if (str.length() > k + 11) 
		{

			{std::istringstream is(str.substr(k + 11, 2));
			is >> hour; }

			{std::istringstream is(str.substr(k + 14, 2));
			is >> minute; }

			{std::istringstream is(str.substr(k + 17, 2));
			is >> second; }
		}

		//year = std::stoi(dateString.substr(0, 4));//2023-01-01 01:01:01
		//month = std::stoi(dateString.substr(5, 2));
		//day = std::stoi(dateString.substr(8, 2));
		//hour = std::stoi(dateString.substr(11, 2));
		//minute = std::stoi(dateString.substr(14, 2));
		//second = std::stoi(dateString.substr(17, 2));
		//this->dateString = dateString;

	}

	DateData(int year,
			int month,
			int day,
			int hour=0,
			int minute=0,
			int second=0
	)
	{
		this->year = year;
		this->month = month;
		this->day = day;
		this->hour = hour;
		this->hour = hour;
		this->minute = minute;
		this->second = second;

		wchar_t buffer[100];
		swprintf(buffer, 100, L"%04d-%02d-%02d %02d:%02d:%02d\n", year, month, day, hour, minute, second);
		this->dateString = wstring(buffer);
	}

	wstring wcout()
	{
		return L"" + std::to_wstring(year) + L"-" + std::to_wstring(month) + L"-" + std::to_wstring(day) + L" " + std::to_wstring(hour) + L":" + std::to_wstring(minute) + L":" + std::to_wstring(second);
		//std::wcout << year << L"-" << month << L"-" << day << L" " << hour << L":" << minute << L":" << second << std::endl;
	}

	static bool compareDates(const DateData* a, const DateData* b)
	{
		if (a->year < b->year)return true;
		if (a->year > b->year)return false;
		if (a->month < b->month)return true;
		if (a->month > b->month)return false;
		if (a->day < b->day)return true;
		if (a->day > b->day)return false;
		if (a->hour < b->hour)return true;
		if (a->hour > b->hour)return false;
		if (a->minute < b->minute)return true;
		if (a->minute > b->minute)return false;
		if (a->second < b->second)return true;
		if (a->second > b->second)return false;
		return false;
		//return (*a < *b); 
	}
	//bool operator() (const DateData* i, const DateData* j)
	//{
	//	if (i->year <   j->year)return true;
	//	if (i->year >   j->year)return false;
	//	if (i->month <  j->month)return true;
	//	if (i->month >  j->month)return false;
	//	if (i->day <    j->day)return true;
	//	if (i->day >    j->day)return false;
	//	if (i->hour <   j->hour)return true;
	//	if (i->hour >   j->hour)return false;
	//	if (i->minute < j->minute)return true;
	//	if (i->minute > j->minute)return false;
	//	if (i->second < j->second)return true;
	//	if (i->second > j->second)return false;
	//	return false;
	//}
	//bool operator< (const DateData* j)
	//{ 
	//	if (year <   j->year)return true;
	//	if (year >   j->year)return false;
	//	if (month <  j->month)return true;
	//	if (month >  j->month)return false;
	//	if (day <    j->day)return true;
	//	if (day >    j->day)return false;
	//	if (hour <   j->hour)return true;
	//	if (hour >   j->hour)return false;
	//	if (minute < j->minute)return true;
	//	if (minute > j->minute)return false;
	//	if (second < j->second)return true;
	//	if (second > j->second)return false;
	//	return false;
	//}
	//bool operator> (const DateData* j)
	//{
	//	if (year <   j->year)return false;
	//	if (year >   j->year)return true;
	//	if (month <  j->month)return false;
	//	if (month >  j->month)return true;
	//	if (day <    j->day)return false;
	//	if (day >    j->day)return true;
	//	if (hour <   j->hour)return false;
	//	if (hour >   j->hour)return true;
	//	if (minute < j->minute)return false;
	//	if (minute > j->minute)return true;
	//	if (second < j->second)return false;
	//	if (second > j->second)return true;
	//	return true;
	//}

};


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
	wstring exifDateString;
	wstring crc32;
	wstring md5;
	wstring sha1;
	wstring sha2;
	wstring keccak;
	wstring sha3;

	static bool comparePtrToFileDataEntry(FileDataEntry* a, FileDataEntry* b) { return (*a < *b); }

	bool operator < (const FileDataEntry& f) const
	{
		return (size < f.size);
	}
	bool operator > (const FileDataEntry& f) const
	{
		return (size > f.size);
	}
};





#include <QtWidgets/QMainWindow>
#include "ui_OpenFileOrganizer.h"
#include <QPlainTextEdit>
#include <qthread.h>
#include <QFileDialog>

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

	static void QMessageOutput(QtMsgType, const QMessageLogContext&, const QString& msg);

	

private:
	Ui::OpenFileOrganizerClass ui;

	// MessageHandler for display and ThreadLogStream for redirecting cout
	MessageHandler* msgHandler = Q_NULLPTR;
	ThreadLogStream* m_qd;

	const QString errorString = "error";

private slots:
	void handleStartButton();
	void handleClearButton();
	void handleAddDirButton();
	void handleRemoveDirButton();

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
	vector<FileDataEntry*> fileDataEntries;
	vector<vector<FileDataEntry*>*> duplicates;
	std::chrono::steady_clock::time_point start;
	std::chrono::steady_clock::time_point firststart;
	bool isWindows = false;

	vector<wstring> dirsToSearch;

	bool scanSubDirs = true;

	QString QfileTypesString;
	vector<wstring> fileTypesList;
	//wstring startpath = L"F:\\_games\\";

	//#define DB_LOCATION L"locate.db"
	//FILE* db;
	//QPlainTextEdit* plainTextEdit;
	//QDebugStream* qout;

public slots:
	void process();
signals:
	void finished();

private:

	void recursiveDirectoryIteratorIncrement(const wstring startpath, _int64& filecount);
	void directoryIteratorRecursive(const std::filesystem::path& dir_path, _int64& filecount);
	void direntScanDirectory(const wstring startPath, _int64& filecount);
	void listFilesWindowsFindFirstFile(const wstring originalPath, _int64& filecount);
	__int64 getFileSizeWindows(FileDataEntry* f);
	BY_HANDLE_FILE_INFORMATION getFileInformationByHandleWindows(wstring fileNameAndPath);
	__int64 getFileSizeFromHandleFileInformationWindows(FileDataEntry* f);
	WIN32_FILE_ATTRIBUTE_DATA getFileAttributesWindows(wstring fileNameAndPath);
	__int64 getFileSizeFromFileAttributesWindows(FileDataEntry* f);
	long long getFileSizeWStat(FileDataEntry* f);
	long long getFileSizeFStat(int _FileHandle);
	uintmax_t getFileSizeFilesystem(FileDataEntry* f);
	uintmax_t getFileSizeFilesystemDirectoryEntry(FileDataEntry* f);
	long long getFileSizeWithIfstream(wstring name);
	void getSizeForAllFiles();
	void getFileCreatedModifiedDateWindows(FileDataEntry* f);
	void getFileCreatedModifiedDateFilesystem(FileDataEntry* f);
	void getFileCreatedModifiedDateWStat(FileDataEntry* f);
	void getCreatedAndLastModifiedDateForAllFiles();
	void getFastHash(FileDataEntry* f);
	void getFastHashForAllFiles();
	void getDateFromFilenameForAllFiles();
	bool extractDateMM_DD_YYorYYYY(const std::string& s, int& d, int& m, int& y);
	void getDatesFromEXIFDataForAllFiles();
	bool doesFilenameMatchFilter(wstring name);

};






	//void recursiveDirectoryIteratorIncrement(const wstring startpath, _int64& filecount);
	//void directoryIteratorRecursive(const std::filesystem::path& dir_path, _int64& filecount);
	//void direntScanDirectory(const wstring startPath, _int64& filecount);
	//void listFilesWindowsFindFirstFile(const wstring originalPath, _int64& filecount);
	//__int64 getFileSizeWindows(FileDataEntry* f);
	//BY_HANDLE_FILE_INFORMATION getFileInformationByHandleWindows(wstring fileNameAndPath);
	//__int64 getFileSizeFromHandleFileInformationWindows(FileDataEntry* f);
	//WIN32_FILE_ATTRIBUTE_DATA getFileAttributesWindows(wstring fileNameAndPath);
	//__int64 getFileSizeFromFileAttributesWindows(FileDataEntry* f);
	//long long getFileSizeWStat(FileDataEntry* f);
	//long long getFileSizeFStat(int _FileHandle);
	//uintmax_t getFileSizeFilesystem(FileDataEntry* f);
	//uintmax_t getFileSizeFilesystemDirectoryEntry(FileDataEntry* f);
	//long long getFileSizeWithIfstream(wstring name);
	//void getSizeForAllFiles();
	//void getFileCreatedModifiedDateWindows(FileDataEntry* f);
	//void getFileCreatedModifiedDateFilesystem(FileDataEntry* f);
	//void getFileCreatedModifiedDateWStat(FileDataEntry* f);
	//void getCreatedAndLastModifiedDateForAllFiles();
	//void getFastHash(FileDataEntry* f);
	//void getFastHashForAllFiles();
	//void getDateFromFilenameForAllFiles();
	//bool extractDateMM_DD_YYorYYYY(const std::string& s, int& d, int& m, int& y);
	//void getDatesFromEXIFDataForAllFiles();
