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
#include "stdio.h"
#include "iostream"
#include <iostream>
#include <streambuf>
#include <string>

#include <io.h>
#include <fcntl.h>
#include "atlbase.h"
#include "atlstr.h"
#include "comutil.h"

#include <cwctype>

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
	wstring exifDateString;
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





#include <QtWidgets/QMainWindow>
#include "ui_OpenFileOrganizer.h"
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

	static void QMessageOutput(QtMsgType, const QMessageLogContext&, const QString& msg);

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
	vector<FileDataEntry*> fileDataEntries;
	vector<vector<FileDataEntry*>*> duplicates;
	std::chrono::steady_clock::time_point start;
	std::chrono::steady_clock::time_point firststart;
	bool isWindows = false;
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

};


