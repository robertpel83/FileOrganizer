#include "OpenFileOrganizer.h"

OpenFileOrganizer::OpenFileOrganizer(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	//QDebugStream qout(std::wcout, ui.scanDirsPlainTextEdit);
   // QDebugStream qout(std::wcout, ui.consoleOutputPlainTextEdit);

	//std::wcout << L"Hello this is a test" << std::endl;
	//std::wcout << L"Hello this is a test" << std::endl;
	//std::wcout << L"Hello this is a test" << std::endl;
	//std::wcout << L"Hello this is a test" << std::endl;
	//std::wcout << L"Hello this is a test" << std::endl;


	//Set up ThreadLogStream, which redirect cout to signal sendLogString
	//Set up MessageHandler, wgucg catch message from sendLogString and Display

	m_qd = new ThreadLogStream(std::wcout); //Redirect Console output to QTextEdit
	this->msgHandler = new MessageHandler(ui.consoleOutputPlainTextEdit, this);
	connect(m_qd, &ThreadLogStream::sendLogString, msgHandler, &MessageHandler::catchMessage);




	//Connect button signal to appropriate slot
	connect(ui.startPushButton, &QPushButton::released, this, &OpenFileOrganizer::handleStartButton);
	connect(ui.addDirPushButton, &QPushButton::released, this, &OpenFileOrganizer::handleAddDirButton);
	connect(ui.removeDirPushButton, &QPushButton::released, this, &OpenFileOrganizer::handleRemoveDirButton);
	//connect(ui.startPushButton, &QPushButton::clicked, this, &OpenFileOrganizer::handleButton);

}

OpenFileOrganizer::~OpenFileOrganizer()
{}

void OpenFileOrganizer::QMessageOutput(QtMsgType, const QMessageLogContext&, const QString& msg)
{
	std::wcout << msg.toStdWString() << std::endl;
}

void OpenFileOrganizer::handleStartButton()
{
	if (ui.listWidget->size().height() == 0)
	{
		QWidget* popup = new QWidget(this, Qt::Popup | Qt::Dialog);

	

		popup->setWindowModality(Qt::WindowModal);
		popup->show();

		return;
	}


	QThread* thread = new QThread;
	Worker* worker = new Worker();

	worker->dirsToSearch.clear();

	for (int i = 0; i < ui.listWidget->count(); i++)
	{
		worker->dirsToSearch.push_back(ui.listWidget->item(i)->text().toStdWString());
	}

	worker->moveToThread(thread);
	//connect( worker, &error, this, &OpenFileOrganizer::errorString);
	connect(thread, &QThread::started, worker, &Worker::process);
	connect(worker, &Worker::finished, thread, &QThread::quit);
	connect(worker, &Worker::finished, worker, &Worker::deleteLater);
	connect(thread, &QThread::finished, thread, &QThread::deleteLater);
	thread->start();

}

void OpenFileOrganizer::handleAddDirButton()
{
	//QFileDialog dialog(this);
	//dialog.setFileMode(QFileDialog::AnyFile);

	QUrl dirUrl = QFileDialog::getExistingDirectoryUrl(this, "Select Target Directory", QUrl("clsid:0AC0837C-BBF8-452A-850D-79D08E667CA7"), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);


	//wstring dir = dirUrl.toString().replace(QString("file:\\\\\\"), QString("")).replace(QString("/"), QString("\\\\")).toStdWString();
	QString s = dirUrl.toString().replace(QString("file:///"), QString("")).replace(QString("/"), QString("\\\\"));
	if (s.endsWith(QString("\\")) == false)s.append(QString("\\\\"));

	ui.listWidget->addItem(s);
	//QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),"C:\\",QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

}

void OpenFileOrganizer::handleRemoveDirButton()
{

	QList<QListWidgetItem*> items = ui.listWidget->selectedItems();

	for (int i = 0; i < ui.listWidget->count(); i++)
	for (int j = 0; j < items.size(); j++)
	{
		if(items.at(j) == ui.listWidget->item(i))
		ui.listWidget->takeItem(i);
	}

}

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

void content_foreach_func(ExifEntry* entry, void* UNUSED(callback_data))
{
	char buf[2000];
	exif_entry_get_value(entry, buf, sizeof(buf));
	std::wcout << convertUtf8ToWide(exif_tag_get_name(entry->tag)) << L" " <<
		convertUtf8ToWide(exif_format_get_name(entry->format)) << L" " <<
		entry->size << L" " <<
		(int)(entry->components) << L" " <<
		convertUtf8ToWide(exif_entry_get_value(entry, buf, sizeof(buf))) << std::endl;
}

void data_foreach_func(ExifContent* content, void* callback_data)
{
	static unsigned content_count;
	std::wcout << exif_content_get_ifd(content) << std::endl;
	exif_content_foreach_entry(content, content_foreach_func, callback_data);
	++content_count;
}



//C++ subdirectory file list using recursiveDirectoryIteratorIncrement
//std::unordered_set<wstring> recursiveDirectoryIteratorIncrementPaths;
//for whatever reason, recursive_directory_iterator cannot read "All Users" folder, but directory_iterator can. weird.
void Worker::recursiveDirectoryIteratorIncrement(const wstring startpath, _int64& filecount)
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

					FileDataEntry* f = new FileDataEntry();
					(*f).nameAndPath = (*v).path().wstring();
					(*f).name = (*v).path().filename().wstring();
					(*f).path = (*v).path().parent_path().wstring();
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



//C++ subdirectory file list using directoryIteratorIncrement
void Worker::directoryIteratorRecursive(const std::filesystem::path& dir_path, _int64& filecount)
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

						FileDataEntry *f = new FileDataEntry();
						f->nameAndPath = itr->path().wstring();
						f->name = itr->path().filename().wstring();
						f->path = path;
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

//C subdirectory file list using dirent.h library
// Find files recursively 
void Worker::direntScanDirectory(const wstring startPath, _int64& filecount)//wchar_t* dirname
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

					FileDataEntry* f = new FileDataEntry();
					f->nameAndPath = path + L"\\" + ent->d_name;
					f->name = ent->d_name;
					f->path = path;
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


//C++ subdirectory file list using Windows FindFirstFileExW
//std::unordered_set<wstring> findFirstFilePaths;
void Worker::listFilesWindowsFindFirstFile(const wstring originalPath, _int64& filecount)
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
						FileDataEntry* f = new FileDataEntry();
						f->nameAndPath = path + L"\\" + ffd.cFileName;
						f->name = ffd.cFileName;
						f->path = path;
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



__int64 Worker::getFileSizeWindows(FileDataEntry* f)//wstring fileNameAndPath)
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

BY_HANDLE_FILE_INFORMATION Worker::getFileInformationByHandleWindows(wstring fileNameAndPath)
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

__int64 Worker::getFileSizeFromHandleFileInformationWindows(FileDataEntry* f)//wstring fileNameAndPath)
{
	BY_HANDLE_FILE_INFORMATION info = getFileInformationByHandleWindows(f->nameAndPath);
	__int64 max = MAXDWORD;
	max += 1;
	__int64 size = (info.nFileSizeHigh * (max)) + info.nFileSizeLow;
	f->size = size;
	return size;
}

WIN32_FILE_ATTRIBUTE_DATA Worker::getFileAttributesWindows(wstring fileNameAndPath)
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

			fileInfo.nFileSizeHigh = 0;
			fileInfo.nFileSizeLow = 0;
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
__int64 Worker::getFileSizeFromFileAttributesWindows(FileDataEntry* f)//wstring fileNameAndPath)
{
	WIN32_FILE_ATTRIBUTE_DATA info = getFileAttributesWindows(f->nameAndPath);
	__int64 max = MAXDWORD;
	max += 1;
	__int64 size = (info.nFileSizeHigh * (max)) + info.nFileSizeLow;
	f->size = size;
	return size;
}

long long Worker::getFileSizeWStat(FileDataEntry* f)//std::wstring filename)
{
	struct _stat64 stat_buf;
	long long rc = _wstat64(f->nameAndPath.c_str(), &stat_buf);
	long long size = rc == 0 ? stat_buf.st_size : -1;
	f->size = size;
	return size;
}


long long Worker::getFileSizeFStat(int _FileHandle)
{
	struct _stat64 stat_buf;
	long long rc = _fstat64(_FileHandle, &stat_buf);
	return rc == 0 ? stat_buf.st_size : -1;
}

uintmax_t Worker::getFileSizeFilesystem(FileDataEntry* f)//std::filesystem::path p)
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

uintmax_t Worker::getFileSizeFilesystemDirectoryEntry(FileDataEntry* f)//std::filesystem::directory_entry d)
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


long long Worker::getFileSizeWithIfstream(wstring name)
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


void Worker::getSizeForAllFiles()
{
	__int64 size = 0;

	if (isWindows)
	{
		//Very slow 1863s 494k files
		if (false)
		{
			std::wcout << L"getFileSizeWindows" << std::endl;
			start = std::chrono::steady_clock::now();
			for (long long i = 0; i < fileDataEntries.size(); i++)
			{
				FileDataEntry* f = fileDataEntries[i];
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
			for (long long i = 0; i < fileDataEntries.size(); i++)
			{
				FileDataEntry* f = fileDataEntries[i];
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
			for (long long i = 0; i < fileDataEntries.size(); i++)
			{
				FileDataEntry* f = fileDataEntries[i];
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
			for (long long i = 0; i < fileDataEntries.size(); i++)
			{
				FileDataEntry* f = fileDataEntries[i];
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
			for (long long i = 0; i < fileDataEntries.size(); i++)
			{
				FileDataEntry* f = fileDataEntries[i];
				size += getFileSizeFilesystem(f);
			}
			std::wcout << "Total size:" << size << " bytes (" << std::fixed << std::setprecision(2) << (((double)size) / 1024. / 1024. / 1024.) << " GBytes)" << std::endl;
			std::wcout << L"Took " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start) << std::endl;
		}
	}


}



void Worker::getFileCreatedModifiedDateWindows(FileDataEntry* f)
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
		swprintf(buffer, 100, L"%04d-%02d-%02d %02d:%02d:%02d", systemTime.wYear, systemTime.wMonth, systemTime.wDay, systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
		createdDateString = wstring(buffer);
	}

	{
		SYSTEMTIME systemTime;
		FileTimeToSystemTime(&mt, &systemTime);
		wchar_t buffer[100];
		swprintf(buffer, 100, L"%04d-%02d-%02d %02d:%02d:%02d", systemTime.wYear, systemTime.wMonth, systemTime.wDay, systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
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

void Worker::getFileCreatedModifiedDateFilesystem(FileDataEntry* f)
{
	//Creation time can't be accessed through std::filesystem
}

void Worker::getFileCreatedModifiedDateWStat(FileDataEntry* f)
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
			wcsftime(buffer, 100, L"%Y-%m-%d %H:%M:%S", gmtime(&result.st_ctime));
			createdDateString = wstring(buffer);
		}

		{
			wchar_t buffer[100];
			wcsftime(buffer, 100, L"%Y-%m-%d %H:%M:%S", gmtime(&result.st_mtime));
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
void Worker::getCreatedAndLastModifiedDateForAllFiles()
{

	if (isWindows)
	{
		//12s 494k files
		std::wcout << L"getFileCreatedModifiedDateWindows" << std::endl;
		start = std::chrono::steady_clock::now();
		for (long long i = 0; i < fileDataEntries.size(); i++)
		{
			FileDataEntry* f = fileDataEntries[i];
			getFileCreatedModifiedDateWindows(f);
		}
		std::wcout << L"Took " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start) << std::endl;
	}
	else
	if (isWindows == false)
	{
		//both of these are pretty comparable
		//23s 494k files
		std::wcout << L"getFileCreatedModifiedDateWStat" << std::endl;
		start = std::chrono::steady_clock::now();
		for (long long i = 0; i < fileDataEntries.size(); i++)
		{
			FileDataEntry* f = fileDataEntries[i];
			getFileCreatedModifiedDateWStat(f);
		}
		std::wcout << L"Took " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start) << std::endl;
	}

}

void Worker::getFastHash(FileDataEntry* f)
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


void Worker::getFastHashForAllFiles()
{

	//It makes an extra pass of the file list to create a 'fasthash'. The fasthash uses small samples of the file(16 kilobytes),
	//taken from the beginning, end, and three places in the middle; then does a duplicate check based on the hash of the samples.
	//This is very quick for large files, and it helps eliminate the vast majority of potential duplicates very quickly, as
	//most files will have different samples. Most other duplicate finders omit this step, but it really speeds things up.

	for (long long i = 0; i < fileDataEntries.size(); i++)
	{
		FileDataEntry* f = fileDataEntries[i];

		getFastHash(f);

	}
}


void Worker::getDateFromFilenameForAllFiles()
{


	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	time_t tt = std::chrono::system_clock::to_time_t(now);
	tm utc_tm = *gmtime(&tt);
	tm local_tm = *localtime(&tt);
	int currentYear = utc_tm.tm_year + 1900;
	//std::wcout << L"Current year: " << currentYear << std::endl;




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
		"[^0-9a-ln-xzA-LN-XZ]"//not a number, not a letter except for m or M or y or Y
		"(?!00)"//month cannot be 00
		"(?!1[3-9])"//month cannot be 13-19
		"[01][0-9]"//month only begins with 0 or 1
		"[^0-9a-ce-ln-zA-CE-LN-Z]"//not a number, not a letter except for m or M or d or D
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
		"[^0-9a-xzA-XZ]"//not a number, not a letter except for y or Y
		"("
		+ regex_yyyy_mm_dd +
		")"
		"[^0-9a-ce-gi-zA-CE-GI-Z]"//not a number, not a letter except for d or D or h H
		"|"
		"[^0-9a-xzA-XZ]"//not a number, not a letter except for y or Y
		"("
		+ regex_yyyy_mm_dd + //at the end beginning with non number
		")"
		"$"
		;

	std::regex yyyy_mm_dd(syyyy_mm_dd);


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
		"[^0-9a-xzA-XZ]"//not a number, not a letter except for y or Y
		"("
		+ regex_yyyymmdd +
		")"
		"[^0-9a-ce-gi-zA-CE-GI-Z]"//not a number, not a letter except for d or D or h H
		"|"
		"[^0-9a-xzA-XZ]"//not a number, not a letter except for y or Y
		"("
		+ regex_yyyymmdd + //at the end beginning with non number
		")"
		"$"
		;



	
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
		"[^0-9a-ce-ln-zA-CE-LN-Z]"//not a number, not a letter except for m or M or d or D
		"(?!00)"//day cannot be 00
		"(?!3[3-9])"//day cannot be 33-39
		"[0123][0-9]"//day only begins with 0 1 2 3
		"[^0-9a-ce-xzA-CE-XZ]"//not a number, not a letter except for d or D or y or Y
		"[12]?[90]?[012789][0-9]"//year [19-20]70-29
		;
	std::string smm_dd_yy =
		"^"//beginning of file
		"("
		+ regex_mm_dd_yy +
		")"
		"|"//or
		"[^0-9a-ln-zA-LN-Z]"//not a number, not a letter except for m or M
		"("
		+ regex_mm_dd_yy +
		")"
		"[^0-9a-gi-xzA-GI-XZ]"//not a number, not a letter except for y or Y or h H
		"|"
		"[^0-9a-ln-zA-LN-Z]"//not a number, not a letter except for m or M
		"("
		+ regex_mm_dd_yy + //at the end beginning with non number
		")"
		"$"
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

	//for (auto& s : strs)
	for (long long i = 0; i < fileDataEntries.size(); i++)
	{
		FileDataEntry* f = fileDataEntries[i];
		string s(convertWideToUtf8(f->name));

		int y = 0;
		int m = 0;
		int d = 0;
		
		std::smatch matches;
		std::regex_search(s, matches, yyyy_mm_dd);

		if (!matches.empty())
		{
			string str;
			for (int n = 0; n < matches.size(); n++)
				str = str + matches[n].str();

			{
				size_t k = str.find_first_of("0123456789");

				std::istringstream is(str.substr(k, 4));
				if (is >> y)
				{
					if (y < 1900)
					{
						if (y > 70)y += 1900;
						if (y < 50)y += 2000;
					}
				}

				{
					std::istringstream is(str.substr(k + 5, 2));
					is >> m;
				}
				{
					std::istringstream is(str.substr(k + 8, 2));
					is >> d;
				}
				if (y <= currentYear)
				{
					std::wcout << L"yyyy_mm_dd " << convertUtf8ToWide(str) << L" found in " << f->name << std::endl;
					std::wcout << y << L" " << m << L" " << d << std::endl;
				}
				else
				{
					y = 0;
					m = 0;
					d = 0;
				}
			}
		}
		else
		{

			std::regex_search(s, matches, yyyymmdd);
			if (!matches.empty())
			{
				string str;
				for (int n = 0; n < matches.size(); n++)
					str = str + matches[n].str();

				{
					size_t k = str.find_first_of("0123456789");

					std::istringstream is(str.substr(k, 4));
					if (is >> y)
					{
						if (y < 1900)
						{
							if (y > 70)y += 1900;
							if (y < 50)y += 2000;
						}
					}

					{
						std::istringstream is(str.substr(k + 4, 2));
						is >> m;
					}
					{
						std::istringstream is(str.substr(k + 6, 2));
						is >> d;
					}

					if (y <= currentYear)
					{
						std::wcout << L"yyyymmdd " << convertUtf8ToWide(str) << L" found in " << f->name << std::endl;
						std::wcout << y << L" " << m << L" " << d << std::endl;
					}
					else
					{
						y = 0;
						m = 0;
						d = 0;
					}
				}

			}
			else
			{
				std::regex_search(s, matches, mm_dd_yy);
				if (!matches.empty())
				{
					string str;
					for (int n = 0; n < matches.size(); n++)
						str = str + matches[n].str();

					{

						size_t k = str.find_first_of("0123456789");

						{
							std::istringstream is(str.substr(k, 2));
							is >> m;
						}

						{
							std::istringstream is(str.substr(k + 3, 2));
							is >> d;
						}

						if (str.substr(k + 6, 2) == "19" || str.substr(k + 6, 2) == "20")
						{
							std::istringstream is(str.substr(k + 6, 4));
							is >> y;
						}
						else
						{
							std::istringstream is(str.substr(k + 6, 2));
							if (is >> y)
							{
								if (y < 1900)
								{
									if (y > 70)y += 1900;
									if (y < 50)y += 2000;
								}
							}

							//TODO: this is the sketchiest detected date and i need to see how common it actually is

						}

						if (y <= currentYear)
						{
							std::wcout << L"mm dd [yy]yy " << convertUtf8ToWide(str) << L" found in " << f->name << std::endl;
							std::wcout << y << L" " << m << L" " << d << std::endl;
						}
						else
						{
							y = 0;
							m = 0;
							d = 0;
						}
					}
				}
				else
				{
					bool foundyyyy = false;
					std::regex_search(s, matches, yyyy);
					if (!matches.empty())
					{
						string str;
						for (int n = 0; n < matches.size(); n++)
							str = str + matches[n].str();

						{
							//std::wcout << L"yyyy " << n << L" " << convertUtf8ToWide(str) << std::endl;

							size_t k = str.find_first_of("12");//yyyy must start with 1 or 2

							if (str.substr(k, 1) != "1" && str.substr(k, 1) != "2")
							{
								//can't parse? this should never happen because the regex
							}
							else
							{
								std::istringstream is(str.substr(k, 4));
								is >> y;

								if (y <= currentYear)
								{
									foundyyyy = true;
								}
								else
								{
									y = 0;
								}
							}
						}
					}

					if (foundyyyy == true)
					{

						std::regex_search(s, matches, monthmondaythday);
						if (!matches.empty())
						{
							string str;
							for (int n = 0; n < matches.size(); n++)
								str = str + matches[n].str();

							{

								if (str.find("Jan") != string::npos)m = 1;
								if (str.find("Feb") != string::npos)m = 2;
								if (str.find("Mar") != string::npos)m = 3;
								if (str.find("Apr") != string::npos)m = 4;
								if (str.find("May") != string::npos)m = 5;
								if (str.find("Jun") != string::npos)m = 6;
								if (str.find("Jul") != string::npos)m = 7;
								if (str.find("Aug") != string::npos)m = 8;
								if (str.find("Sep") != string::npos)m = 9;
								if (str.find("Oct") != string::npos)m = 10;
								if (str.find("Nov") != string::npos)m = 11;
								if (str.find("Dec") != string::npos)m = 12;

								size_t k = str.find_first_of("0123456789");
								{
									if (str.find_first_of("0123456789", k + 1) == k + 1)
									{
										std::istringstream is(str.substr(k, 2));
										is >> d;
									}
									else
									{
										std::istringstream is(str.substr(k, 1));
										is >> d;
									}
								}

								if (y <= currentYear)
								{
									std::wcout << L"monthmondaythday " << convertUtf8ToWide(str) << L" found in " << f->name << std::endl;
									std::wcout << y << L" " << m << L" " << d << std::endl;
								}
								else
								{
									y = 0;
									m = 0;
									d = 0;
								}
							}
						}
						else
						{

							std::regex_search(s, matches, daythdaymonthmon);
							if (!matches.empty())
							{
								string str;

								for (int n = 0; n < matches.size(); n++)
									str = str + matches[n].str();

								{
									size_t k = str.find_first_of("0123456789");

									if (str.find_first_of("0123456789", k + 1) == k + 1)
									{
										std::istringstream is(str.substr(k, 2));
										is >> d;
									}
									else
									{
										std::istringstream is(str.substr(k, 1));
										is >> d;
									}

									if (str.find("Jan") != string::npos)m = 1;
									if (str.find("Feb") != string::npos)m = 2;
									if (str.find("Mar") != string::npos)m = 3;
									if (str.find("Apr") != string::npos)m = 4;
									if (str.find("May") != string::npos)m = 5;
									if (str.find("Jun") != string::npos)m = 6;
									if (str.find("Jul") != string::npos)m = 7;
									if (str.find("Aug") != string::npos)m = 8;
									if (str.find("Sep") != string::npos)m = 9;
									if (str.find("Oct") != string::npos)m = 10;
									if (str.find("Nov") != string::npos)m = 11;
									if (str.find("Dec") != string::npos)m = 12;

									if (y <= currentYear)
									{
										std::wcout << L"daythdaymonthmon " << convertUtf8ToWide(str) << L" found in " << f->name << std::endl;
										std::wcout << y << L" " << m << L" " << d << std::endl;
									}
									else
									{
										y = 0;
										m = 0;
										d = 0;
									}
								}
							}
						}
					}
						
				}
			}
		}
		
		if (y != 0 && m != 0 && d != 0)
		{
			wchar_t buffer[100];
			swprintf(buffer, 100, L"%04d-%02d-%02d %02d:%02d:%02d", y, m, d, 0, 0, 0);
			f->fileNameDateString = wstring(buffer);
			
		}
	}
}






// function expects the string in format dd/mm/yyyy:
bool Worker::extractDateMM_DD_YYorYYYY(const std::string& s, int& d, int& m, int& y)
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



void Worker::getDatesFromEXIFDataForAllFiles()
{

	//if file is an image get exif data, timestamps
	//look at all possible timestamp values in exif


	std::string regex_yyyy_mm_dd_hh_mm_ss =
		"(?!1[0-8])"//not 10xx-18xx
		"(?!19[0-7])"//not 190x-197x
		"(?!2[3-9])"//not 23xx
		"[12][09][0-9][0-9]"//1000-2999
		"[^0-9a-ln-xzA-LN-XZ]"//not a number, not a letter except for m or M or y or Y
		"(?!00)"//month cannot be 00
		"(?!1[3-9])"//month cannot be 13-19
		"[01][0-9]"//month only begins with 0 or 1
		"[^0-9a-ce-ln-zA-CE-LN-Z]"//not a number, not a letter except for m or M or d or D
		"(?!00)"//day cannot be 00
		"(?!3[3-9])"//day cannot be 33-39
		"[0123][0-9]"//day only begins with 0 1 2 3
		"[^0-9a-ce-gi-zA-CE-GI-Z]"//not a number, not a letter except for h or H or d or D
		;

	std::regex yyyy_mm_dd_hh_mm_ss(regex_yyyy_mm_dd_hh_mm_ss);

	std::string regex_yyyy_mm_dd =
		"(?!1[0-8])"//not 10xx-18xx
		"(?!19[0-7])"//not 190x-197x
		"(?!2[3-9])"//not 23xx
		"[12][09][0-9][0-9]"//1000-2999
		"[^0-9a-ln-xzA-LN-XZ]"//not a number, not a letter except for m or M or y or Y
		"(?!00)"//month cannot be 00
		"(?!1[3-9])"//month cannot be 13-19
		"[01][0-9]"//month only begins with 0 or 1
		"[^0-9a-ce-ln-zA-CE-LN-Z]"//not a number, not a letter except for m or M or d or D
		"(?!00)"//day cannot be 00
		"(?!3[3-9])"//day cannot be 33-39
		"[0123][0-9]"//day only begins with 0 1 2 3
		;

	std::regex yyyy_mm_dd(regex_yyyy_mm_dd);

	////libexif
	//for (long long i = 0; i < fileDataEntries.size(); i++)
	//{
	//	FileDataEntry* f = fileDataEntries[i];
	//
	//	std::wstring lowerName = f->name;
	//	transform(
	//		lowerName.begin(), lowerName.end(),
	//		lowerName.begin(),
	//		towlower);
	//
	//	if (lowerName.find(L".jpg") != std::wstring::npos || 
	//		lowerName.find(L".jpeg") != std::wstring::npos 
	//		)
	//	{
	//		ExifData* d;
	//		d = exif_data_new_from_file(convertWideToUtf8(f->nameAndPath).c_str());
	//		if (!d)
	//		{
	//			std::wcout << L"Could not load data from " << f->name << std::endl;
	//		}
	//		else
	//		{
	//			void* callback_data = NULL;
	//
	//			exif_data_foreach_content(d, data_foreach_func, callback_data);
	//
	//			ExifMnoteData* mn = exif_data_get_mnote_data(d);
	//			if (mn) 
	//			{
	//				char buf[2000];
	//				int i;
	//				int num = exif_mnote_data_count(mn);
	//				//std::wcout << L"  MakerNote" << std::endl;
	//				for (i = 0; i < num; ++i) 
	//				{
	//					//if (exif_mnote_data_get_value(mn, i, buf, sizeof(buf))) 
	//					{
	//						//const char* 
							//string name(exif_mnote_data_get_name(mn, i));
	//						//unsigned int id = exif_mnote_data_get_id(mn, i);
	//						//if (!name)
	//							//name = "(unknown)";
	//
	//							//if(
	//							//	name=="DateTime"||
	//							//	name=="DateTimeOriginal"||
	//							//	name=="DateTimeDigitized"||
	//							//	name=="Date"||
	//							//	name=="Time"||
	//							//	name=="GPSDateStamp"||
	//							//	name=="GPSTimeStamp"
	//							//)
	//
	//						//std::wcout << convertUtf8ToWide(name) << std::endl;
	//						//std::wcout << convertUtf8ToWide(buf) << std::endl;
	//					}
	//				}
	//			}
	//
	//			exif_data_unref(d);
	//		}
	//	}
	//}

	//DateTime
	//DateTimeOriginal
	//DateTimeDigitized
	//Date
	//Time
	//GPSDateStamp
	//GPSTimeStamp
	for (long long i = 0; i < fileDataEntries.size(); i++)
	{
		FileDataEntry* f = fileDataEntries[i];
		std::wstring lowerName = f->name;
		transform(
			lowerName.begin(), lowerName.end(),
			lowerName.begin(),
			towlower);

		if (
			lowerName.find(L".jpg") != std::wstring::npos ||
			lowerName.find(L".jpeg") != std::wstring::npos ||
			lowerName.find(L".png") != std::wstring::npos
			)
		{
			ImageInfo info = getImageInfo(convertWideToUtf8(f->nameAndPath));
			std::wcout << L"File: " << f->nameAndPath << std::endl;
			if (info.getErrorCode() != II_ERR_OK)
			{
				std::wcout << L"Error: " << info.getErrorMsg() << std::endl;
			}
			else
			{
				std::wcout << L"Format: " << info.getFormat() << std::endl;
				std::wcout << L"Ext: " << info.getExt() << std::endl;
				std::wcout << L"Full Ext: " << info.getFullExt() << std::endl;
				std::wcout << L"Width: " << info.getWidth() << std::endl;
				std::wcout << L"Height: " << info.getHeight() << std::endl;
				std::wcout << L"Mimetype: " << info.getMimetype() << std::endl;
				std::wcout << L"Entries: " << std::endl;

				for (const auto& entrySize : info.getEntrySizes())
				{
					std::wcout << L"Width: " << entrySize[0] << std::endl;
					std::wcout << L"Height: " << entrySize[1] << std::endl;
				}
			}
		}
	}

	//TinyEXIF
	for (long long i = 0; i < fileDataEntries.size(); i++)
	{
		FileDataEntry* f = fileDataEntries[i];
		std::wstring lowerName = f->name;
		transform(
			lowerName.begin(), lowerName.end(),
			lowerName.begin(),
			towlower);

		if (
			lowerName.find(L".jpg") != std::wstring::npos ||
			lowerName.find(L".jpeg") != std::wstring::npos ||
			lowerName.find(L".png") != std::wstring::npos 
			)
		{
			//open a stream to read just the necessary parts of the image file
			std::ifstream stream(f->nameAndPath, std::ios::binary);
			if (stream)
			{
				// parse image EXIF and XMP metadata
				TinyEXIF::EXIFInfo imageEXIF(stream);
				if (!imageEXIF.Fields)
				{

				}

				//if (!imageEXIF.DateTime.empty())
				//	std::wcout << L"DateTime " << convertUtf8ToWide(imageEXIF.DateTime) << std::endl;
				//if (!imageEXIF.DateTimeOriginal.empty())
				//	std::wcout << L"DateTimeOriginal " << convertUtf8ToWide(imageEXIF.DateTimeOriginal) << std::endl;
				//if (!imageEXIF.DateTimeDigitized.empty())
				//	std::wcout << L"DateTimeDigitized " << convertUtf8ToWide(imageEXIF.DateTimeDigitized) << std::endl;
				
				string dateString = "";
				if (!imageEXIF.DateTime.empty() && imageEXIF.DateTime.length()>4)
				{
					dateString = imageEXIF.DateTime;
				}
				else if (!imageEXIF.DateTimeOriginal.empty() && imageEXIF.DateTimeOriginal.length() > 4)
				{
					dateString = imageEXIF.DateTimeOriginal;
				}
				else if (!imageEXIF.DateTimeDigitized.empty() && imageEXIF.DateTimeDigitized.length() > 4)
				{
					dateString = imageEXIF.DateTimeDigitized;
				}

				if (dateString.length() > 4)
				{
					int y = 0;
					int m = 0;
					int d = 0;
					int hour = 0;
					int min = 0;
					int sec = 0;


					string str = string(dateString);

						size_t k = str.find_first_of("0123456789");

						{std::istringstream is(str.substr(k, 4));
						is >> y; }

						{std::istringstream is(str.substr(k + 5, 2));
						is >> m; }
							
						{std::istringstream is(str.substr(k + 8, 2));
						is >> d; }

						{std::istringstream is(str.substr(k + 11, 2));
						is >> hour; }

						{std::istringstream is(str.substr(k + 14, 2));
						is >> min; }

						{std::istringstream is(str.substr(k + 17, 2));
						is >> sec; }
							
						if (y != 0 && m != 0 && d != 0)
						{
							wchar_t buffer[100];
							swprintf(buffer, 100, L"%04d-%02d-%02d %02d:%02d:%02d\n", y, m, d, hour, min, sec);
							f->exifDateString = wstring(buffer);
						}

				}
				else
				{
					//std::wcout << L"Bad exif date string " << convertUtf8ToWide(dateString) << std::endl;
				}

			}
		}
	}

}

//bool compareFiles(const std::wstring& p1, const std::wstring& p2) {
//	std::ifstream f1(p1, std::ifstream::binary | std::ifstream::ate);
//	std::ifstream f2(p2, std::ifstream::binary | std::ifstream::ate);
//
//	if (f1.fail() || f2.fail()) {
//		return false; //file problem
//	}
//
//	if (f1.tellg() != f2.tellg()) {
//		return false; //size mismatch
//	}
//
//	//seek back to beginning and use std::equal to compare contents
//	f1.seekg(0, std::ifstream::beg);
//	f2.seekg(0, std::ifstream::beg);
//	return std::equal(std::istreambuf_iterator<char>(f1.rdbuf()),
//		std::istreambuf_iterator<char>(),
//		std::istreambuf_iterator<char>(f2.rdbuf()));
//

bool compareFiles(const std::wstring& p1, const std::wstring& p2) {
	std::ifstream f1(p1, std::ifstream::binary | std::ifstream::ate);
	std::ifstream f2(p2, std::ifstream::binary | std::ifstream::ate);

	if (f1.fail() || f2.fail()) {
		return false; //file problem
	}

	long long size = f1.tellg();

	if (size != f2.tellg()) {
		return false; //size mismatch
	}

	//seek back to beginning 
	f1.seekg(0, std::ifstream::beg);
	f2.seekg(0, std::ifstream::beg);

	f1.close();
	f2.close();

	char* buffer1 = new char[size](0);
	char* buffer2 = new char[size](0);
	
	try
	{
		if (std::ifstream is{ p1, std::ios::in | std::ios::binary })
		{
			is.exceptions(is.failbit);
			is.read(buffer1, size);
			is.exceptions(is.failbit);
			std::size_t numBytesRead = size_t(is.gcount());
			//std::wcout << L"File size " << f1->size << std::endl;
			//std::wcout << L"numBytesRead " << numBytesRead << std::endl;
			is.close();
		}
	}
	catch (const std::ios_base::failure& e)
	{
		std::wcout << L"Caught an ios_base::failure" << e.what() << std::endl;
	}
	
	try
	{
		if (std::ifstream is{ p2, std::ios::in | std::ios::binary })
		{
			is.exceptions(is.failbit);
			is.read(buffer2, size);
			is.exceptions(is.failbit);
			std::size_t numBytesRead = size_t(is.gcount());
			//std::wcout << L"File size " << f->size << std::endl;
			//std::wcout << L"numBytesRead " << numBytesRead << std::endl;
			is.close();
		}
	}
	catch (const std::ios_base::failure& e)
	{
		std::wcout << L"Caught an ios_base::failure" << e.what() << std::endl;
	}

	bool result = memcmp(buffer1, buffer2, size);

	delete[] buffer1;
	delete[] buffer2;

	if (result == true) 
	{
		return false;  // Files are not equal
	}
	return true;
}

void Worker::process()
//void ok()
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


	firststart = std::chrono::steady_clock::now();
	std::wcout << L"listFilesWindowsFindFirstFile" << std::endl;

	QueryPerformanceCounter(&li);
	CounterStart = li.QuadPart;
	start = std::chrono::steady_clock::now();
	filecount = 0;

	for (int i = 0; i < dirsToSearch.size(); i++)
	{
		listFilesWindowsFindFirstFile(dirsToSearch.at(i), filecount);
	}

	std::wcout << L"Files found: " << filecount << std::endl;

	QueryPerformanceCounter(&li);
	msPassed = (_int64)(double(li.QuadPart - CounterStart) / PCFreq);
	//std::wcout << L"Time passed QueryPerformanceCounter: " << msPassed << std::endl;
	std::wcout << L"Took " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start) << std::endl;






	getSizeForAllFiles();

	getCreatedAndLastModifiedDateForAllFiles();

	if(false)
		getFastHashForAllFiles();

	//if(false)
		getDateFromFilenameForAllFiles();

	//if(false)
		getDatesFromEXIFDataForAllFiles();




	//You can use std::swap to swap two values.
	//And also you may want to compare to std::sort(which is typically an introsort : a quick sort + insertion sort for small sizes),
	//std::stable_sort(typically a merge sort), and std::make_heap + std::sort_heap(heap sort)
	{
		//sort by file size

		std::wcout << L"Start sort by size" << std::endl;
		start = std::chrono::steady_clock::now();

		//std::sort(fileDataEntries.begin(), fileDataEntries.end(), FileDataEntry::comparePtrToFileDataEntry);//56s 494k files
		std::stable_sort(fileDataEntries.begin(), fileDataEntries.end(), FileDataEntry::comparePtrToFileDataEntry);//53s 494k files

		std::wcout << L"Took " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start) << std::endl;
	}




	//for each entry, check to see if there are duplicates ahead
	{
		std::wcout << L"Check duplicates" << std::endl;
		start = std::chrono::steady_clock::now();
		for (long long i = 0; i < fileDataEntries.size(); i++)
		{

			FileDataEntry* f1 = fileDataEntries[i];
			

			if (f1->size > 1024 * 1024)//10kb
			{
				for (long long j = i + 1; j < fileDataEntries.size(); j++)
				{
					FileDataEntry* f2 = fileDataEntries[j];

					if (f1->size != f2->size)
					{
						j = (long long)(fileDataEntries.size());
						break;
					}
					else
					{
						//got a match

						//check hash
						if (f1->crc32.empty())getFastHash(f1);
						if (f2->crc32.empty())getFastHash(f2);

						//compare hash
						if (
							//(
							//	f1->crc32.empty()  == false &&
							//	f1->md5.empty()    == false &&
							//	f1->sha1.empty()   == false &&
							//	f1->sha2.empty()   == false &&
							//	f1->keccak.empty() == false &&
							//	f1->sha3.empty()   == false &&
							//
							//	f2->crc32.empty() == false  &&
							//	f2->md5.empty() == false    &&
							//	f2->sha1.empty() == false   &&
							//	f2->sha2.empty() == false   &&
							//	f2->keccak.empty() == false &&
							//	f2->sha3.empty() == false
							//	)
							//&&
							(
								f1->crc32 == f2->crc32 ||
								f1->md5 == f2->md5 ||
								f1->sha1 == f2->sha1 ||
								f1->sha2 == f2->sha2 ||
								f1->keccak == f2->keccak ||
								f1->sha3 == f2->sha3
								)
							)
						{
							//std::wcout << L"Possible match: " << f1->nameAndPath << L" " << f2->nameAndPath << L" " << f1->size << std::endl;

							if
								(
									f1->crc32 != f2->crc32 ||
									f1->md5 != f2->md5 ||
									f1->sha1 != f2->sha1 ||
									f1->sha2 != f2->sha2 ||
									f1->keccak != f2->keccak ||
									f1->sha3 != f2->sha3
									)
							{
								std::wcout << L"One or more hash matches but not all in " << f1->name << L" and " << f2->name << std::endl;
							}
							else
							{
								//std::wcout << L"All hashes match!" << std::endl;

								bool doByteComparison = false;

								bool isDuplicate = false;

								if (f1->modifiedTime != f2->modifiedTime)
								{
									std::wcout << L"Same size (" << f1->size << "), hashes match, but different modified time, doing byte comparison for " << f1->name << " and " << f2->name << std::endl;
									doByteComparison = true;
								}

								if (f1->name != f2->name)
								{
									std::wcout << L"Same size (" << f1->size << "), hashes match, but different name, doing byte comparison for " << f1->name << " and " << f2->name << std::endl;
									doByteComparison = true;
								}

								//std::wcout << L"Same size (" << f1->size << "), hashes match, doing byte comparison for " << f1->name << " and " << f2->name << std::endl;
								//doByteComparison = true;

								if (doByteComparison == true)
								{


									if (f1->size == f2->size)
									{


										//mm_file
										// instruct the kernel that we will read the content
										// of the file sequentially
										//int advice = mm::advice::sequential;
										//
										//// read the stream as uint16_t integers
										//mm::file_source<uint16_t> f1in(f1->nameAndPath, advice);
										//
										////std::cout << "mapped " << f1in.bytes() << " bytes " << "for " << f1in.size() << " integers" << std::endl;
										//
										//auto const* data = f1in.data();
										//for (uint32_t i = 0; i != f1in.size(); ++i) 
										//{
										//	std::cout << "read " << data[i] << std::endl;
										//}
										//
										//f1in.close();


										//boost io memory mapping
										//io::mapped_file_source mf1("test.1");
										//io::mapped_file_source mf2("test.2");
										//
										//if (f1.size() == f2.size()
										//	&& std::equal(f1.data(), f1.data() + f1.size(), f2.data())
										//	)
										//	std::cout << "The files are equal\n";
										//else
										//	std::cout << "The files are not equal\n";


										//char* buffer1 = new char[f1->size](0);
										//char* buffer2 = new char[f2->size](0);
										//
										//try
										//{
										//	if (std::ifstream is{ f1->nameAndPath, std::ios::in | std::ios::binary })
										//	{
										//		is.exceptions(is.failbit);
										//		is.read(buffer1, f1->size);
										//		is.exceptions(is.failbit);
										//		std::size_t numBytesRead = size_t(is.gcount());
										//		//std::wcout << L"File size " << f1->size << std::endl;
										//		//std::wcout << L"numBytesRead " << numBytesRead << std::endl;
										//		is.close();
										//	}
										//}
										//catch (const std::ios_base::failure& e)
										//{
										//	std::wcout << L"Caught an ios_base::failure" << e.what() << std::endl;
										//}
										//
										//try
										//{
										//	if (std::ifstream is{ f2->nameAndPath, std::ios::in | std::ios::binary })
										//	{
										//		is.exceptions(is.failbit);
										//		is.read(buffer2, f2->size);
										//		is.exceptions(is.failbit);
										//		std::size_t numBytesRead = size_t(is.gcount());
										//		//std::wcout << L"File size " << f->size << std::endl;
										//		//std::wcout << L"numBytesRead " << numBytesRead << std::endl;
										//		is.close();
										//	}
										//}
										//catch (const std::ios_base::failure& e)
										//{
										//	std::wcout << L"Caught an ios_base::failure" << e.what() << std::endl;
										//}
										//
										//bool byteMatch = true;
										//for (long long a = 0; a < f1->size; a++)
										//{
										//	if (buffer1[a] != buffer2[a])
										//	{
										//		byteMatch = false;
										//		std::wcout << L"Byte comparison failed at index " << a << L"/" << f1->size << L" in " << f1->name << L" and " << f2->name << ", not a match!" << std::endl;
										//		a = f1->size;
										//		break;
										//	}
										//}
										//
										//delete[] buffer1;
										//delete[] buffer2;

										bool byteMatch = true;
										byteMatch = compareFiles(f1->nameAndPath, f2->nameAndPath);

										if (byteMatch == true)
										{
											//std::wcout << L"Absolutely definitely a duplicate!" << std::endl;
											isDuplicate = true;
										}
									}
									else
									{
										//std::wcout << L"File size does not match" << std::endl;
									}

								}
								else
								{
									//std::wcout << L"Probably a duplicate but only checked size, filename, and hashes" << std::endl;
									isDuplicate = true;
								}


								if (isDuplicate == true)
								{

									//store certain duplicates together in a data structure

									//get each vector in duplicates
									//look at file pointers and compare with f1, if it exists in that vector then add f2 to that vector
									//(f1 would have been f2 in the previous comparison)
									bool foundFileInDupes = false;
									for (int b = 0; b < duplicates.size(); b++)
									{
										vector<FileDataEntry*>* v = duplicates[b];


										if (std::find(v->begin(), v->end(), f1) != v->end())
										{
											foundFileInDupes = true;

											//found 1
											if (std::find(v->begin(), v->end(), f2) != v->end())
											{

												//found 1 and 2

											}
											else
											{
												//didn't find 2, add it
												v->push_back(f2);
											}

										}
										else
										{
											//didn't find 1

											if (std::find(v->begin(), v->end(), f2) != v->end())
											{

												std::wcout << L"Should never happen" << std::endl;

												//found 2 but not 1, add 1
												foundFileInDupes = true;

												v->push_back(f1);

											}
											else
											{
												//not found 1 or 2 in this node
											}

										}

									}

									//otherwise create a new vector and add to dupes
									if (foundFileInDupes == false)
									{
										vector<FileDataEntry*>* v = new vector<FileDataEntry*>();
										v->push_back(f1);
										v->push_back(f2);
										duplicates.push_back(v);
									}
								}
							}
						}
					}
				}
			}
		}

		std::wcout << L"Took " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start) << std::endl;

	}


	std::wcout << L"Found " << fileDataEntries.size() << L" files." << std::endl;
	std::wcout << L"Found " << duplicates.size() << L" unique files with duplicates." << std::endl;

	long long totalDupes = 0;
	for (long long b = 0; b < duplicates.size(); b++)
	{
		vector<FileDataEntry*>* v = duplicates[b];
		totalDupes += v->size();
	}

	std::wcout << L"Found " << totalDupes << L" total duplicates." << std::endl;





	
	for (long long b = 0; b < duplicates.size(); b++)
	{
		vector<FileDataEntry*>* v = duplicates[b];
		
		std::wcout << L"All names for duplicate " << b << L": " << std::endl;

		for (long long c = 0; c < v->size(); c++)
		{
			std::wcout << (*v)[c]->name << std::endl;
		}

		std::wcout << L"Size for duplicate " << b << L": " << (*v)[0]->size << L" bytes" << std::endl;

		vector<wstring> dateStrings;

		std::wcout << L"All dates for duplicate " << b << L": " << std::endl;
		for (long long c = 0; c < v->size(); c++)
		{
			if ((*v)[c]->createdDateString.length() > 2) 
			{ 
				//std::wcout << (*v)[c]->createdDateString << std::endl;

				dateStrings.push_back(((*v)[c]->createdDateString));
			}
			if ((*v)[c]->modifiedDateString.length() > 2) 
			{ 
				//std::wcout << (*v)[c]->modifiedDateString << std::endl;

				dateStrings.push_back(((*v)[c]->modifiedDateString));
			}
			if ((*v)[c]->fileNameDateString.length() > 2) 
			{ 
				//std::wcout << (*v)[c]->fileNameDateString << std::endl;

				dateStrings.push_back(((*v)[c]->fileNameDateString));
			}
			if ((*v)[c]->exifDateString.length() > 2) 
			{ 
				//std::wcout << (*v)[c]->exifDateString << std::endl;

				dateStrings.push_back(((*v)[c]->exifDateString));
			}
		}

		
		vector<DateData*> dates;
		
		for (long long i = 0; i < dateStrings.size(); i++)
		{
			DateData*d = new DateData(dateStrings[i]);
			dates.push_back(d);

			std::wcout << dateStrings[i] << std::endl;
			//std::wcout << d->wcout() << std::endl;
		}

		//count instances of identical dates, with and without timestamp
		//determine most likely correct date

		std::stable_sort(dates.begin(), dates.end(), DateData::compareDates);

		std::wcout << L"Sorted dates for duplicate " << b << L": " << std::endl;

		for (long long i = 0; i < dates.size(); i++)
		{
			std::wcout << dates[i]->wcout() << std::endl;
		}

		std::wcout << L"Earliest date for duplicate " << b << L": " << dates[0]->wcout() << std::endl;

	}




	//connect gui elements, do tooltips and info and options and stuff

















	//todo:


	//compare all dates, try to decide correct date
	//maybe set that date as created for dupes that don't have it
	//compare dates and filenames for duplicates


	//store all info in database
	//identify files based on filesize and hash?







	//maybe get gtk working


	





	//use library to compare images with fuzzy comparison, different resolution comparison, 
	//use different methods to ensure accuracy




	//check file hashes to online databases
	


























	//keep all filenames and dates, compare with filename date, exif dates, determine most correct timestamp for image and duplicates



	//fix file extensions for wrong image type





	//option minimum size
	//size of fast hash chunks (or percentage?)
	//do byte comparison










	//keep higher quality version, optionally delete others

	//rename files with most accurate date, optionally reset timestamps
	//store all filenames and dates etc in metadata


	//save and load database with metadata



	//Windows Projected File System (ProjFS)
	//maybe make fake filesystem with images renamed on the fly by date, object, etc.


	//Alternate data stream(ADS)
	//Main article : Fork(file system)
	//Alternate data streams allow more than one data stream to be associated with a filename(a fork), 
	//using the format "filename:streamname" (e.g., "text.txt:extrastream").
	//NTFS Streams were introduced in Windows NT 3.1, to enable Services for Macintosh(SFM) to store resource forks.
	// Although current versions of Windows Server no longer include SFM, third - party Apple Filing Protocol(AFP) 
	// products(such as GroupLogic's ExtremeZ-IP) still use this feature of the file system. Very small ADSs 
	// (named "Zone.Identifier") are added by Internet Explorer and recently[when?] by other browsers to mark 
	// files downloaded from external sites as possibly unsafe to run; the local shell would then require user 
	// confirmation before opening them.[57] When the user indicates that they no longer want this confirmation dialog, 
	// this ADS is deleted.
	//Alternate streams are not listed in Windows Explorer, and their size is not included in the file's size. 
	// When the file is copied or moved to another file system without ADS support the user is warned that alternate 
	// data streams cannot be preserved. No such warning is typically provided if the file is attached to an e-mail, 
	// or uploaded to a website. Thus, using alternate streams for critical data may cause problems. 
	// Microsoft provides a tool called Streams[58] to view streams on a selected volume. 
	// Starting with Windows PowerShell 3.0, it is possible to manage ADS natively with six cmdlets: 
	// Add-Content, Clear-Content, Get-Content, Get-Item, Remove-Item, Set-Content.[59]
	//Malware has used alternate data streams to hide code.[60] As a result, malware scanners 
	//and other special tools now check for alternate data streams.


	//macOS supports listing, [17] getting, [18] setting, [19] and removing[20] extended attributes from files or 
	// directories using a Linux - like API.From the command line, these abilities are exposed through the xattr utility.[21]
	//Since macOS 10.5, files originating from the web are marked with com.apple.quarantine via extended file attributes.
	//[22] In some older versions of macOS(such as Mac OS X 10.6), user space extended attributes were not preserved on save in 
	//common Cocoa applications(TextEdit, Preview etc.)

	//The Linux kernel allows extended attribute to have names of up to 255 bytes and values of up to 64 KiB, 
	//[14] as do XFSand ReiserFS, but ext2 / 3 / 4 and btrfs impose much smaller limits, requiring all the 
	// attributes(names and values) of one file to fit in one "filesystem block" (usually 4 KiB).Per POSIX.1e, 
	// [citation needed] the names are required to start with one of security, system, trusted, and user 
	// plus a period.This defines the four namespaces of extended attributes.[15]
	//Extended attributes can be accessedand modified using the getfattrand setfattr commands from the attr 
	//package on most distributions.[16] The APIs are called getxattrand setxattr.


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

	//video file metadata, online lookup? like picard? look for duplicates with approx same length and offer to remove lesser quality? rename with quality/codec etc in filename? get year released etc?
	//music file metadata
	//pdf and document metadata





	//somehow make part of filesystem or explorer extension, run system wide search

	//switch to linux, test program, embed in gnome?



	std::wcout << L"Finished." << std::endl;
	std::wcout << L"Took " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - firststart) << std::endl;


	//emit finished();

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











/*


https://www.alldup.info/alldup_help/search_similar_pictures.php
AllDup
Comparison Method
The comparison methods aHash, bHash, dHash and pHash enables you to find similar or almost identical pictures by using a percent 
match lower than 100%. If you want find exactly the same pictures you have to use a percent match of 100% or the comparison 
methods MD5/SHA. An overview of the recognition rate of the comparison methods aHash, bHash, dHash and pHash can be found here.
aHash
The comparison method aHash (Average Hash) resizes the image to 8x8 or 16x16 pixel. The image will be converted to grayscale 
and the average color of the all image pixel will be calculated. Now the checksum will be created based on whether the pixel's 
value is greater than the average color of the image. The checksum can be 64-bit (8x8 pixel) or 256-bit (16x16 pixel).
bHash
The comparison method bHash (Blockhash) resizes the image to 128x128, 256x256 or 512x512 pixel. The image will be divided into a 
block matrix and the median value of all blocks will be calculated to create the checksum. The checksum can be 64-bit or 256-bit. 
The options "Fast" and "Precise" enables you to influence the accuracy of the checksum calculation.
dHash
The comparison method dHash (Difference Hash) resizes the image to 8x8 or 16x16 pixel. The image will be converted to grayscale 
and the relative change in brightness intensity between two pixels will be calculated to create the checksum. The checksum can be 
64-bit (8x8 pixel) or 256-bit (16x16 pixel).
pHash
The comparison method pHash (Perceptual Hash) resizes the image to 32x32 pixel. The image will be converted to grayscale and the 
64-bit checksum will be calculated based on the frequency spectrum data and a distance calculation doing a discrete cosine transform (DCT).
MD5, SHA
These comparison methods enables you to find exactly the same pictures. The following comparison methods are available:
MD5 (128-Bit)
SHA-1 (160-Bit)
SHA-2 (256-Bit)
SHA-2 (384-Bit)
SHA-2 (512-Bit)
For more information on the aHash, bHash, dHash, and pHash comparison methods, see articles Testing different image hash functions 
and Detection of Duplicate Images Using Image Hash Functions.


Hints
The following files will be automatically excluded from the search:
Files with a size of 0 bytes
Pictures with a width or height smaller than the specified compare size
Corrupted, invalid or incomplete pictures (*)
Files with a blocked read access (*)
(*) An error message will be shown at the log pane.

Percentage Match
Here you can specify the minimum percentage matching of two pictures. The calculated percentage matching between two pictures 
will be shown at the column Match. The percentage always refers to the reference picture of a group which will be shown in a different text color.


Picture Area
This option enables you to specify the picture area to be used to create the checksum. The following options are available:
entire picture
area at the upper left corner
area at the upper right corner
area at the lower left corner
area at the lower right corner
When using the options 2, 3, 4 or 5, the size of the picture area can be specified in pixel. For example, a value of 16 pixel 
will be using a picture area with a width and height of 16 pixel at the selected corner.


Compare Size
Here you can specify the maximum width and height of the pictures to be compared. A lower compare size finds more similar 
pictures and speeds up the comparison time. A higher compare size finds more identical pictures and less similar pictures 
and of course needs more time to compare them.


Checksum
The size of the checksum in bits is displayed here. The checksum can only be changed when using the bHash comparison method.


Compare only pictures with the same properties
This option will be performed before the option Detect picture modifications.
File Name
This option enables you to compare only pictures with the same file name.
File Extension
This option enables you to compare only pictures with the same file extension.
Width and Height
This option enables you to compare only pictures with the same width and height.
Orientation
This option enables you to compare only pictures with the same orientation (portrait or landscape).
Aspect Ratio
This option enables you to compare only pictures with the same aspect ratio. The calculation of the aspect ratio is done by the 
formula "width divided by height". The result of the calculation is truncated to one decimal place. A picture with 1920x1080 pixel has an aspect ratio of "1.7".

Detect picture modifications (slower)
This option enables you to detect different picture modifications when comparing two pictures. For this purpose, each picture 
modification will be performed with the picture to be compared and in each case an additional checksum will be created. 
The following picture modifications can be detected:
Rotated 90° to the right
Rotated 180° to the right
Rotated 90° to the left
Flipped horizontally
Rotated 90° to the right and flipped horizontally
Flipped vertically
Rotated 90° to the left and flipped horizontally


Recognition Rate Test
We have carried out tests with various comparison methods to determine the recognition rate for different image changes. 
For this purpose, 29 copies of a JPEG image (1600x1200px, 606KB) were created and their color, size and format changed. 
The tests always compared the original JPEG image and a modified copy of the image.
The following comparison methods were used in the test:
aHash, compare size: 16x16, checksum: 256-bit
bHash (fast), compare size: 256x256, checksum: 256-bit
dHash, compare size: 16x16, checksum: 256-bit
pHash, compare size: 32x32, checksum: 64-bit

https://content-blockchain.org/research/testing-different-image-hash-functions/
TESTING DIFFERENT IMAGE HASH FUNCTIONS
For the image-ID component of the ISCC – that is, the content ID of image files – we need a hash function which, 
for minor changes to the file, produces an identical hash, or rather one that is as similar as possible while 
producing a small number of false positive collisions.

REQUIREMENTS
In view of the generic deployability of the ISCC, we place value on finding an algorithm that has a moderate 
computation requirements and that is easy to implement. Calculation duration can easily get out of hand when 
processing images.For example, a file sized 1920×1080 contains 2,073,600 pixels which might need to be processed 
individually. To further narrow down the number of eligible hash functions, we have defined image modifications
against which our hash should ideally be robust:

Should be robust against:

Changes in brightness (5% – 20%)
Changes in contrast (5% – 20%)
Gamma correction
Watermark
JPEG compression (5% – 20%)
Scaling (50% – 300%)
Grayscale
Should be partially robust against:

Salt and Pepper
Multiplicative noise
Cropping (1% – 5%)
Gaussian smoothing (5% – 20%)
Color adjustments
Does not have to be robust against:

Rotation
Skewing
ELIGIBLE HASH FUNCTIONS
Except for block hashing, we have implemented all hash functions and used no third party libraries other than PIL. 
Firstly we use PIL to convert the images to grayscale. According to its documentation PIL uses ITU-R 601-2 luma 
transform for this. Secondly, we are using PIL to scale the images, which is described here. For resampling we use 
bicubic interpolation, which is described here.

The sample picture used in this document was taken from Morguefile.com.

AVERAGE HASH
The average hash algorithm first converts the input image to grayscale and then scales it down. 
In our case, as we want to generate a 64 bit hash, the image is scaled down to 8×8 pixels.

Next, the average of all gray values of the image is calculated and then the pixels are examined one by one from left to right. 
If the gray value is larger than the average, a 1 is added to the hash, otherwise a 0.

Source image
Grayscale image
Gray value image, size 8×8
Comparison to average
Hash: 0000000000010000000000000010000001000010100001101111111111111110
BLOCKHASH
The block hash algorithm divides the image into blocks and generates a value for each block, either 1 or 0. 
These values are combined serially from left to right into a hash. As we need a 64 bit hash, we divide the image into 64 blocks.

As the block hash algorithm does neither grayscale conversion nor does it scale the image down, it was 
initially rather slow, especially when processing larger images. However, we were able to solve this problem 
by scaling down the input image to 256×256 pixels; thus, every block still has 16 pixels, but the calculation 
duration was improved considerably. Additionally, we first converted each image to grayscale.

Source image
Grayscale image
Gray value image, size 256×256
Partitioning of the 64 blocks
Block results
Hash: 0011100010011100000011100110001101000011100001110100001011100110

DIFFERENCE HASH
Similar to the average hash algorithm, the difference hash algorithm initially generates a grayscale image from the 
input image, which in our case is then scaled down to 9×8 pixels. From each row, the first 8 pixels are examined 
serially from left to right and compared to their neighbor to the right, which, analogous to the average hash algorithm, results in a 64 bit hash.

Source image
Grayscale image
Gray value image, size 9×8
Comparison to the neighbor to the right
Hash: 1111000000110000101110001100111010000110010011001000111010001110

MEDIAN HASH
The median hash algorithm works similar to the average hash algorithm, except that it does not compare to the average but to the median.

Source image
Grayscale image
Gray value image, size 8×8
Comparison to median
Hash: 0000000010011100000011000110001101000010100001111111111111111111

PERCEPTUAL HASH
The perceptual hash algorithm, too, initially calculates the gray value image and scales it down. 
In our case, we desire a factor of 4, which is why we scaled down to 8*4×8*4, that is, a 32×32 image.

To this image we apply a discrete cosine transform, first per row and afterwards per column.

Discrete cosine transform:
The pixels with high frequencies are now located in the upper left corner, which is why we crop the image to the upper left 8×8 pixels. 
Next, we calculate the median of the gray values in this image and generate, analogous to the median hash algorithm, a hash value from the image.

Source image
Grayscale image
Gray value image, size 32×32
After DCT by row (normalized)
After DCT by column (normalized)
Upper left corner (highest frequency)
Comparison to median
Hash: 1010010010101101100110011011001101100010100100000111011010101110

WAVELET HASH
Analogous to the average hash algorithm, the wavelet hash algorithm also generates a gray value image sized 8×8. 
Next, a two-dimensional wavelet transform is applied to the image. Our tests have revealed that the results 
improve when we set the top row to 0, that is, to black and re-apply the wavelet transform three times. 
We took this idea from the image hash library imagehash.

Next, analogous to the perceptual hash algorithm, each pixel is compared to the median and the hash is calculated.

Source image
Grayscale image
Gray value image, size 8×8
First wavelet transform
Second wavelet transform
Third wavelet transform
Fourth wavelet transform
Comparison to median
Hash: 0000010110111001111110011111101111111010000000000000011100000101

We took the block hash from the library blockhash-python, implemented the average hash, difference hash, 
perceptual hash and wavelet hash algorithms based on the imagehash library and developed the median hash algorithm ourselves.

OUR TESTS
We tested the hash functions against each other using the image database Caltech101 which contains 9143 images. 
For every image we created 10 individual test images with slight, randomized modifications.

We have:
increased and decreased brightness
increased and decreased contrast
added a watermark
converted the image to grayscale
scaled it down
cropped the borders
applied Gaussian smoothing
applied JPEG compression
This resulted in 100,584 test images. Next, the individual hashes for each image were calculated and stored along with 
the image number and the type of error (i. e., the modification applied) in an ElasticSearch index.

We conducted three different tests on each hash:
Grouping by image and counting the number of instances where a hash was calculated that differed from that of the source image.
Grouping by image and calculating the average Hamming distance of a modified image from the source image.
Grouping by hash and counting the number of collisions, that is, the number of images that were assigned the same 
hash value despite them not belonging to the same source image.
RESULTS
DIFFERENT HASH THAN THE SOURCE IMAGE
Type of error	aHash	bHash	dHash	mHash	pHash	wHash
Gaussian smoothing	1828 (20.0%)	1241 (13.6%)	3807 (41.6%)	2122 (23.2%)	786 (8.6%)	971 (10.6%)
Grayscale	0 (0%)	0 (0%)	0 (0%)	0 (0%)	0 (0%)	0 (0%)
Increased brightness	3874 (42.4%)	3206 (35.1%)	5357 (58.6%)	3451 (37.7%)	3986 (43.6%)	2844 (31.1%)
Decreased brightness	1717 (18.8%)	809 (8.8%)	4935 (54.0%)	2115 (37.7%)	1030 (11.3%)	1420 (15.5%)
JPEG compression	455 (5.0%)	598 (6.5%)	1616 (17.7%)	658 (7.2%)	546 (6.0%)	514 (5.6%)
Increased contrast	2559 (28.0%)	2062 (22.6%)	4568 (50.0%)	2474 (27.1%)	2460 (26.9%)	2197 (24.0%)
Decreased contrast	2056 (22.5%)	766 (8.4%)	5223 (51.1%)	2154 (23.6%)	1063 (11.6%)	2026 (22.2%)
Scaled	554 (6.1%)	1297 (14.2%)	2091 (22.9%)	851 (9.3%)	664 (7.3%)	614 (6.7%)
Watermark	3600 (39.4%)	5046 (55.2%)	7167 (78.4%)	4029 (44.1%)	4242 (46.4%)	2896 (31.7%)
Cropped	8543 (93.4%)	8750 (95.7%)	9075 (99.2%)	8514 (93.1%)	9088 (99.4%)	7840 (85.7%)
Total	25,186 (25.0%)	23,775 (23.6%)	43,839 (43.6%)	26,368 (26.2%)	23,865 (23.7%)	21,322 (21.2%)
AVERAGE HAMMING DISTANCE
Type of error

We also tried to combine the hashes with AND, OR, XOR or with a similarity hash, but the results were at best 
as good as those of the best of the hashes thus combined.

As the perceptual hash showed good results in overall deviations and the average Hamming distance while 
producing few false positives and being twice as fast as the block hash algorithm, we opted for the perceptual hash algorithm.


https://towardsdatascience.com/detection-of-duplicate-images-using-image-hash-functions-4d9c53f04a75
Detection of Duplicate Images Using Image Hash Functions
Automate the search for (near-)identical photos with the Python library undouble
Searching for (near-)identical photos across your entire system can be a tedious effort; you need to click across 
thousands of photos, and then decide for each photo whether it is a “similar” one. The most straightforward 
approach to detect duplicates would be on file size or filename. However, photos are usually derived from different 
sources such as mobile devices, social media apps, which leads to differences in file size, name, but also in resolution, 
scaling, compression, and brightness. Hash functions are ideal to detect (near-)identical photos because of the robustness 
against minor changes. I will summarize the concepts of hash functions, followed by a hands-on tutorial to demonstrate the 
steps of detecting duplicates while minimizing the number of false-positive collisions. All results are derived using the Python library undouble.

Photo by Jon Tyson on Unsplash
Visual similar but numerical different.
Two images can be visually similar but numerical different. Numerical differences can be caused by various reasons 
such as the use of social media apps which may change the brightness, contrast, gamma corrections, compression, 
resolution, and/or scaling. As an example, sending an image using Whatsapp results in a reduced resolution (Figure 1). 
Note that the amount of reduction may differ across devices and user-defined settings.

Figure 1. Visually similar photos but with numerical differences. A. Most left is the source image. 
B. The middle is after sending it with Whatsapp. C. The right image is the difference between the two images. 
Photo by Louis-Philippe Poitras on Unsplash
From a visual perspective, it is hard to see any changes between the source and the Whatsapp image 
but when we subtract the two images, the differences become clear (Figure 1C). In case you only have a 
few images on disk that needs to be undoubled, it is easy to pick those with the best resolution. 
However, when you occasionally or perhaps yearly dump all your images to disk, it becomes a time-consuming task, 
and even more challenging when family/friends/co-workers also share their almost-similar moments with you.

The question is not whether you have duplicate photos, but where they are.

Approaches such as sorting on file size or image subtraction would thus fail. There are solutions though; 
Hash functions! Hash functions are robust against small changes in brightness, contrast, gamma corrections, 
compression, scaling, and/or resolution, and therefore ideal to detect (near-)identical images. 
There are many applications for hash function usage, such as in digital forensics, copyright enforcement, 
and more generically for disk space reduction and thus undoubling.

The undouble library searches for duplicate images.
The undouble library can be used to detect identical images across an entire system or any input directory. 
Given the input data, preprocessing is performed on the images, the hashes are computed, and images are 
grouped based on the image hash. To organize the images on disk without performing manual actions, the move 
functionality will move the identical images except for the image with the largest resolution (that one is copied). 
A schematic overview of the taken steps is shown below.


Schematic overview of the steps taken in undouble for grouping of images on the image hash. Image from the author.
In the following sections, I will describe in more detail the pre-processing steps, hash functions, 
grouping results, and plotting functionalities, such as the image hash.

Image Hash Functions.
A hash function transforms or maps the input data to a string with a fixed-length which can be seen as 
the “fingerprint” or “signature” of the input data; the image hash. Thus a good hash function should be 
fully determined by the input data or in our case, the image. There are various hash functions, such as; 
Average hash, Perceptual hash, Difference hash, Haar wavelet hash, Daubechies wavelet hash, HSV color hash, 
and Crop-resistant hash. Each hash function has specific properties to make it robust against certain changes such as 
brightness, contrast, gamma, correction, watermark, compression, scaling, and grayscaling.

A image hash function maps an image to a short string called image hash, and can be used for image authentication or as a digital fingerprint.

Nevertheless, it can occur that two visually different images get the same image hash, which is called a collision. 
More than one example is shown with the 101 objects dataset but let's start evaluating the robustness of the most 
well-known hash functions. The robustness is evaluated by changing the brightness (-50%, -20%, +20% +50%), contrast (-50%, -20%, +20% +50%), 
scaling, and compression (png, jpg) for a single image. In total, 10 different alterations for the “cat and dog” image (Figure 2) are created. 
Note that this will not evaluate collisions but the influence of brightness and contrast across different hash functions. 
All hash functions are evaluated with the python library undouble, which in its turn uses functionalities of the image hash library imagehash [3].


Figure 2. Four examples of changes in alterations in brightness and contrast. Photo by Louis-Philippe Poitras on Unsplash
Pre-processing before computing the hash.
Before we can determine the image hash, pre-processing steps are required; 1. decolorize, 2. normalize the pixel values, 
and 3. scale the image. The reasoning for decolorizing is that the information required to “recognize” an image is 
readily present in the grayscale channel. In addition, the reduction from 24 bits per pixel for RGB to 8 bits per 
pixel is computationally more attractive; time-wise and memory-wise. The next step is to downsample/scale the image 
to a smaller size. Most often, a 64-bit hash is chosen, which simply means that the image is downsampled to 8 x 8 pixels. 
Below is shown an example of how the image hash is derived using Python for various hash functions.










https://github.com/ermig1979/AntiDupl
AntiDupl.NET

How does the program makes recommendations to remove duplicate files?

If a duplicate pair of images in the same format, but the difference is less than half of the threshold image difference, 
the program recommends to remove the image, which has the smaller the size of picture and/or smaller file size. 
If the program will meet two identical images, then it will recommend to remove the one which is located in the Remove Paths. 
If a picture with better quality will be in the directory to delete, the program recommends move it to place of picture in the normal directory.

Could you briefly describe the algorithm for image comparison, which is used in your program?
In simplified form, my algorithm of image comparison comprises the following steps:
Resizing all the images to the same size (by default, this size is 32x32).
The discarding the color information (conversion to grayscale image).
The finding of mean square differences for each pair of reduced grayscale images.
Comparison of the mean square difference with threshold - if the difference is less than the threshold, it is supposed that these images are similar.

Search rotated and mirror image dupls - Enabling this option allows you to search rotated and mirrored duplicate images. 
It has to do more comparisons; therefore search is slowed down eight times. By default, this option is disabled.

Control image size - if this options is enabled then the program take into account the size of pictures. 
For example, the picture sizes 1024x768 and 800x600 will be different, even if they are to be shown the same thing. By default, this option is disabled.

Control image type - if this options is enabled then the program take into account the type of image in comparison. 
For example, images in PNG and GIF formats will be different, regardless of their contents. The default option is disabled.

Control image width/height ratio - when you turn on this option, the program checks the ratio of width and height of images. 
For example, this allows us not to compare the square and rectangular images among themselves, which greatly accelerates the search. 
The degree of difference ratio of the width and height of images is governed by the parameter Image width/height ratio precision, 
located on the Advanced tab options (see below). By default, this option is enabled.

Algorithm of comparing of images - algorithm by means of which pictures are compared. In standard fast algorithm - 'Mean square difference' 
calculate a mean square deviation of brightness for each couple of images. He fast, but has big percent of false positives for pictures 
with a big uniform background and can not find strongly changed images. When comparing pictures by means of an index of structural similarity 
('SSIM') can find duplicates among strongly changed pictures, and also images with a big uniform background are better compared, but calculation 
more slowly. Also 'SSIM' is very sensitive to the normalized size of image. It is better not to use him at the sizes less than 128x128. The default 
setting is 'Mean square difference'. This parameter is duplicated on the Tool Bar.

Threshold difference - or value mean square deviations of brightness of pictures or value of an index of structural similarity in 
case of which value below value determined by this parameter, they are considered as duplicates. value determined by this parameter, 
then the pictures are duplicates. When setting this option at a low level, the program can not detect some of the duplicates. If it is 
equal to zero then program will be able to find only fully identical images. If he set too high, then increase the likelihood of false 
positives. Also worth noting that the increase in this parameter leads to a slower speed of the algorithm and vice versa. It can vary 
in the range from 0 to 15% for algorithm 'Mean square difference' and from 0 to 50% for algorithm 'SSIM'. By default, this parameter 
is equal to 5% for algorithm 'Mean square difference' and 30% for algorithm 'SSIM'. This parameter is duplicated on the Tool Bar.

Minimal image width/height - sets the minimum width/height of images is necessary that they will be checked for the presence of 
defects or duplicates. This option allows you to exclude from the search too small images. The default setting is 64.

Maximal image width/height - sets the maximum width/height of images is necessary that they will be checked for the presence of 
defects or duplicates. This option allows you to exclude from the search too large images. The default setting is 8192.

Compare images inside one directory - when this option is enabled, the program compares with each other the pictures located 
both in one directory and in the different. Otherwise, the program will compare among themselves only the images which aren't 
lying in one directory. By default, this option is disabled.

Compare images from one path of search with one another - when this option is enabled, the program compares one with another 
pictures that are located in the same search path. Otherwise, the program will only compare the images from different search 
paths. By default, this option is disabled.

Check on defect - option specifies whether you want to check the images for defects. By default, this option is enabled.

Check on blockiness - the option defines, whether it is necessary to check pictures for existence of artifacts of 
compression of JPEG - a blockiness. The blockiness is defined by the greatest amount of gradients of brightness 
for extreme pixels of units 8x8. By default, this option is disable.

Blockiness threshold - percent of the amount of gradients in case of which the image is considered the defective. 
The chess black-and-white grid will have 100% with a size of cell of 8 pixels. The counted value will testify not 
necessarily to the worst visual quality in comparison with the picture with smaller value and vice versa. 
By default this parameter is equal 10.

Check on blockiness only on not Jpeg - the option defines, whether it is necessary to check pictures for existence 
of artifacts of compression of JPEG - a blockiness only not in Jpeg files. It allows to find only the pressed images, 
without showing the simply strongly oblate. If at the PNG or BMP file there are explicit artifacts of compression it 
means that the source file was saved in other format with certainly big size. By default, this option is disabled.

Check on bluring -the option defines, whether it is necessary to check pictures for too big blur of the image. 
Blur is calculated a gradient of the second derivative (which it is maximum on lines and edges) for the original 
image and its reduced options. Exact value is defined by method of the linear interpolation. By default, this option is disabled.

Bluring threshold - blur radius in case of which the image is considered the defective. So far the algorithm is 
incomplete and high blur for landscape images is shown. By default this parameter is equal 4.

Image width/height ration precision - option determines the accuracy which must be respected by the ratio of 
width/height of the compared images. It can be 1/8, 1/16, 1/32 or 1/64. The default value is 1/32.

Compare threads count - option specifies the number of threads of comparison, which are created when searching for 
duplicates. It can take values Auto (number of threads is determined by the program), 1, ..., the number of processor 
cores in the system. The default value is Auto.

Load thread count - option specifies the number of threads of image loading, which are created when searching. It can 
take values Auto (number of threads is determined by the program), 1, ..., the number of processor cores in the system. The default value is Auto.

Normalized image size - this parameter can be described as follows: because images can be absolutely arbitrary size, 
then the correct comparison of their need to lead to a common denominator. To do this, all images are scaled to the 
same size and converted into 8-bit gray. Then have these normalized images are compared with each other and stored in a 
database. This option determines the size of these images. The more normalized the size of images, the higher the accuracy 
of the program. Adverse effects of this increase will be decrease of speed of the program, and large requirements to the 
memory and the free disk space (to store the database of previously scanned images). Can take the values 
16x16, 32x32, 64x64 or 128x128. The default value is 32�32.

Width of ignored frame of image - This parameter determines the width of frame of the image, the contents of 
which will be ignored during the comparison of images. This option is relevant if you want to compare pictures 
with frames of different colors or small inscription on the edge. By default, this value is 0 %.

Use libjpeg-turbo - a flag to use libjpeg-turbo library to improve performance of loading of JPEG images. 
Otherwise, they will be deleted permanently. By default, this option is enabled.





https://github.com/qarmin/czkawka
Czkawka
Similar Images
It is a tool for finding similar images that differ e.g. in watermark, size etc.

The tool first collects images with specific extensions that can be checked - 
[".jpg", ".jpeg", ".png", ".bmp", ".tiff", ".tif", ".pnm", ".tga", ".ff", ".gif", ".jif", ".jfi", ".ico", ".webp", ".avif"].

Next cached data is loaded from file to prevent hashing twice the same file.
The cache which points to non-existing data, by default is deleted automatically.

Then a perceptual hash is created for each image which isn't available in cache.

Cryptographic hash (used for example in ciphers) for similar inputs gives completely different outputs:
11110 ==> AAAAAB
11111 ==> FWNTLW
01110 ==> TWMQLA

Perceptual hash at similar inputs, gives similar outputs:
11110 ==> AAAAAB
11111 ==> AABABB
01110 ==> AAAACB

Computed hash data is then thrown into a special tree that allows to compare hashes using Hamming distance.

Next these hashes are saved to file, to be able to open images without needing to hash it more times.

Finally, each hash is compared with the others and if the distance between them is less than the maximum 
distance specified by the user, the images are considered similar and thrown from the pool of images to be searched.

It is possible to choose one of 5 types of hashes - Gradient, Mean, VertGradient, Blockhash, DoubleGradient.
Before calculating hashes usually images are resized with specific algorithm(Lanczos3, Gaussian, CatmullRom, 
Triangle, Nearest) to e.g. 8x8 or 16x16 image(allowed sizes - 8x8, 16x16, 32x32, 64x64), which allows simplifying 
later computations. Both size and filter can be adjusted in application.

Each configuration saves results to different cache files to save users from invalid results.
Some images broke hash functions and create hashes full of 0 or 255, so these images are silently excluded from 
end results(but still are saved to cache).

You can test each algorithm with provided CLI tool, just put to folder test.jpg file and run inside this command 
czkawka_cli tester -i
Faster compare option allows to only once compare results, so checking should work a lot of faster when using higher 
number of similarity.

Some tidbits:
Smaller hash size not always means that calculating it will take more time
Blockhash is the only algorithm that don't resize images before hashing
Nearest resize algorithm can be faster even 5 times than any other available but provide worse results




https://github.com/arsenetar/dupeguru
https://dupeguru.voltaicideas.net/help/en/scan.html#picture-blocks
dupeguru
Picture blocks
dupeGuru Picture mode stands apart of its two friends. Its scan types are completely different. 
The first one is its “Contents” scan, which is a bit too generic, hence the name we use here, “Picture blocks”.

We start by opening every picture in RGB bitmap mode, then we “blockify” the picture. 
We create a 15x15 grid and compute the average color of each grid tile. This is the “picture analysis” phase. 
It’s very time consuming and the result is cached in a database (the “picture cache”).

Once we’ve done that, we can start comparing them. Each tile in the grid (an average color) is compared to its 
corresponding grid on the other picture and a color diff is computer (it’s simply a sum of the difference of R, G and B on each side). 
All these sums are added up to a final “score”.

If that score is smaller or equal to 100 - threshold, we have a match.

A threshold of 100 adds an additional constraint that pictures have to be exactly the same 
(it’s possible, due to averaging, that the tile comparison yields 0 for pictures that aren’t exactly the same, 
but since “100%” suggests “exactly the same”, we discard those ocurrences). If you want to get pictures that are very, 
very similar but still allow a bit of fuzzy differences, go for 99%.

This second part of the scan is CPU intensive and can take quite a bit of time. This task has been made to take advatange of 
multi-core CPUs and has been optimized to the best of my abilities, but the fact of the matter is that, due to the fuzziness of the 
task, we still have to compare every picture to every other, making the algorithm quadratic (if N is the number of pictures to compare,
the number of comparisons to perform is N*N).

This algorithm is very naive, but in the field, it works rather well. If you master a better algorithm and want to improve 
dupeGuru, by all means, let me know!






https://cresstone.com/apps/DupeKill/
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




https://www.digitalvolcano.co.uk/dcfeatures.html
Duplicate Cleaner feature list
Image Mode
Match similar images		x	x
Match exact images (Ignoring metadata)		x	x
Match rotated or flipped images		x	x
Match by Image tags	Includes date taken, Unique ID, User comment & more.	x	x
Match similar Image tags		x
Select image metadata to display in results	Customize your results list.	x
Match by camera date/time taken	With a user definable tolerance in seconds.	x
Match by geographical distance between images		x	x
All popular image formats supported	.bmp .gif .jpg/.jpeg/.jpe .heic .ico .png .emf .dib .tif/.tiff .webp	x
Camera RAW Image formats supported	.raw, .cr2, .nef, .dng, .arw, .crw, .kdc, .mos, .srw, .orf, .raf, .rw2, .x3f, .3fr, .mrw, .dcr, .erf, .mef, .pef, .sr2	x	x
Audio Mode
Match by similar sounding audio	Match by first 15 seconds (quick), two minutes or whole track.	x	x
Match by exact audio data (ignoring tags)		x	x
Match by song duration (with tolerance)		x
Match by audio tags	Artist, Title, Album, TrackNo,TrackCount, DiscNo, DiscCount, Year, Genre, Comment, BitRate, SampleRate, Channels, 
MusicBrainzArtistID, MusicBrainzReleaseID,MusicBrainzTrackID, AmazonID (ASIN), AcoustID	x	x
Select audio metadata tags to display in results	Customize your results list.	x
Most popular audio formats supported	.mp3 .ogg .wav .wma .flac .m4a .m4p .aac	x	x
Video Mode
Match by similar video frames	Match by the similarity of series of video frames from throughout the video.	x
Match by video thumbnail	Match by a thumbnail taken from the start of the video.	x
Match by similar sounding audio soundtrack	Match by first 15 seconds (quick), first two minutes or whole soundtrack.	x
Match by exact audio data, video data or both (ignoring metadata)		x
Match by video duration (with tolerance)		x
Match by video metadata tags	Title, Recorded date, Director,Performer, Genre, Episode, Format, Rotation, Has Audio, Has Cover Art	x
Most popular video formats supported	mov, avi, mp4, mpg, wmv, flv, 3pg, asf, 3g2, webm, mkv, vob, m4v, mts, sec	x
Other Match Criteria
Match any combination of the below in addition to Regular, Image or Audio mode.
Same file name		x	x	x
Similar file name		x	x
Same file extension		x	x	x
Match copied file names		x		x
Same file size	An additional file size tolerance can be specified	x	x	x
Same created date/time	An additional date/time tolerance can be specified	x	x	x
Same modified date/time	An additional date/time tolerance can be specified	x	x	x
Same drive		x
Same folder name	Folder name match depth can be specified	x	x
Same folder name (relative to search base)		x
Ignore duplicate groups within same folder	Duplicates can be EXCLUDED from the final list where ALL the duplicates in that group are in the same folder.	x	x
Other scanning features
Scan inside Zip files.		x	x
Scan inside other archive formats	rar, gz, tar, bz2, tgz, 7z, tar, jar	x
Ignore zero size files		x	x
Count hard-links to file		x	x
Specify folders to ignore		x	x	x
Specify folders to protect		x	x	x
Search for duplicates that match a master folder/drive		x	x
Search for duplicates that must match other folders/drives only		x	x
Search for duplicates that must match within this folder only		x
File removal
Delete files.	Recycle bin optional	x	x	x
Move or Copy files		x	x
Rename files		x	x
Create shorcuts to file		x
Create symbolic links to file	aka Soft links or symlinks	x
Hard-link files		x	x
Create batch files	Export a batch file based on your marked list with customizable code	x
Automatic empty folder removal		x	x
Search filters
Limit search to certain file types.		x	x
Limit search within a file size range		x	x
Limit search to a date range		x	x
Limit search to image/movie dimensions	Image/Video modes	x
Built-in search filter library - add your own.		x	x
Duplicate browsing & selecting
Browse duplicates by detail list		x	x	x
Browse duplicates by thumbnail view	Supported thumbnails include all image formats and audio artwork	x	x
Group, filter and sort files		x	x
Search within list		x	x
Create filter from text search		x
Selection Assistant for help marking duplicate groups	See manual for full list of Selection Assistant functions	x	x
Selection Assistant for duplicate folders		x	x
Built-in image preview tool for visual comparison of images		x	x
Video preview		x
Built-in audio player for previewing files		x	x
Safety features
Don't scan system files/folders.		x	x	x
Skip hidden folders		x
Don't follow NTFS mountpoints & junctions		x	x	x
Skip hard-linked files		x	x	x
Check if all duplicates in group marked for removal		x	x	x
Flag folders to protect before starting		x	x	x
Miscellaneous
Launch scans using command line parameters.		x	x
Export CSV file of duplicate files/folders










(AI)
https://github.com/visualdatabase/fastdup
FastDup is a tool for gaining insights from a large image collection. It can find anomalies, duplicate and near duplicate images,
clusters of similarity, learn the normal behavior and temporal interactions between images. It can be used for smart subsampling
of a higher quality dataset, outlier removal, novelty detection of new information to be sent for tagging. FastDup scales to
millions of images running on CPU only.
From the authors of GraphLab and Turi Create.

Compute Image Statistics
Compute image statistics and visualize the results, using food-101 dataset

Identify duplicates
Duplicates and near duplicates identified in MS-COCO and Imagenet-21K dataset

Find corrupted and broken images
Thousands of broken ImageNet images that have confusing labels of real objects.

Find outliers
IMDB-WIKI outliers (data goal is for face recognition, gender and age classification)

Find similar persons
Can you tell how many different persons?

Find wrong labels
Wrong labels in the Imagenet-21K dataset.

Fun labels in the Imagenet-21K dataset

Coming soon: image graph search (please reach out if you like to beta test)

Upcoming new features: image graph search!

Results on Key Datasets (full results here)
We have thoroughly tested fastdup across various famous visual datasets. Ranging from pillar Academic datasets to Kaggle competitions. 
A key finding we have made using FastDup is that there are ~1.2M (!) duplicate images on the ImageNet-21K dataset, out of which 104K 
pairs belong both to the train and to the val splits (this amounts to 20% of the validation set). This is a new unknown result!
Full results are below. * train/val splits are taken from https://github.com/Alibaba-MIIL/ImageNet21 .

Dataset	Total Images	cost [$]	spot cost [$]	processing [sec]	Identical pairs	Anomalies
imagenet21k-resized	11,582,724	4.98	1.24	11,561	1,194,059	Anomalies Wrong Labels
imdb-wiki	514,883	0.65	0.16	1,509	187,965	View
places365-standard	2,168,460	1.01	0.25	2,349	93,109	View
herbarium-2022-fgvc9	1,050,179	0.69	0.17	1,598	33,115	View
landmark-recognition-2021	1,590,815	0.96	0.24	2,236	2,613	View
visualgenome	108,079	0.05	0.01	124	223	View
iwildcam2021-fgvc9	261,428	0.29	0.07	682	54	View
coco	163,957	0.09	0.02	218	54	View
sku110k	11,743	0.03	0.01	77	7	View
Experiments presented are on a 32 core Google cloud machine, with 128GB RAM (no GPU required).
All experiments could be also reproduced on a 8 core, 32GB machine (excluding Imagenet-21K).
We run on the full ImageNet-21K dataset (11.5M images) to compare all pairs of images in less than 3 hours WITHOUT a GPU (with Google cloud cost of 5$).
Quick Installation
Python 3.7, 3.8, 3.9
Supported OS: Ubuntu 20.04, Ubuntu 18.04, Debian 10, Mac OSX M1, Mac OSX Intel, Windows 10 Server.
# upgrade pip to its latest version
python3.XX -m pip install -U pip
# install fastdup
python3.XX -m pip install fastdup
Where XX is your python version. For Windows, CentOS 7.X, RedHat 4.8 and other older Linux see our Insallation instructions.

Running the code
import fastdup
fastdup.run(input_dir="/path/to/your/folder", work_dir='out', nearest_neighbors_k=5, turi_param='ccthreshold=0.96')    #main running function.
fastdup.create_duplicates_gallery('out/similarity.csv', save_path='.')     #create a visual gallery of found duplicates
fastdup.create_outliers_gallery('out/outliers.csv',   save_path='.')       #create a visual gallery of anomalies
fastdup.create_components_gallery('out', save_path='.')                    #create visualiaiton of connected components
fastdup.create_stats_gallery('out', save_path='.', metric='blur')          #create visualization of images stastics (for example blur)
fastdup.create_similarity_gallery('out', save_path='.',get_label_func=lambda x: x.split('/')[-2])     #create visualization of top_k 
similar images assuming data have labels which are in the folder name
fastdup.create_aspect_ratio_gallery('out', save_path='.')                  #create aspect ratio gallery
Full documentation is here

alt text Working on the Food-101 dataset. Detecting identical pairs, similar-pairs (search) and outliers (non-food images..)

Getting started examples
Finding duplicates, outliers and connected components in the Food-101 dataset, including Tensorboard Projector visualization - Google Colab
Visualizing and understanding a new dataset, looking at dats outliers and label outliers, Training a baseline KNN classifier 
and getting to accuracy of 0.99 by removing confusing labels
Finding wrong lables via image similarity
Computing image statistics
Using your own onnx model for extraction
Getting started on a Kaggle dataset
Analyzing video of the MEVA dataset - Google Colab
Working with multipe labels per image
Tensorboard Projector integration is explained in our Colab notebook

We build upon several excellent open source tools. Microsoft's ONNX Runtime, Facebook's Faiss, 
Open CV, Pillow Resize, Apple's Turi Create, Minio, Amazon's awscli, TensorBoard, scikit-learn.





https://www.sentex.ca/~mwandel/finddupe/
finddupe
The match candidates are clustered according to the signature of the first 32k, then checked byte for byte.
https://github.com/jeremitu/finddupe (unicode port)
I ported finddupe to Unicode (actually UTF-16) and MinGW as version 1.24. I used tchar.h wrapper, 
so ASCII build is still possible. It works for me, but some more testing is desirable.
https://github.com/thomas694/finddupe
I added the following features to finddupe:
multiple reference directories that shall not be touched (v1.24)
unicode support (v1.25)
alert message if order of options is wrong (v1.26)
support for ignoring files by patterns (v1.26)
checking for NTFS file system in batch and hardlink mode (v1.27)
performance optimizations (especially for very large amounts of files) (v1.28)
It works for me, but some more testing is desirable.
I've updated the project to use Visual Studio 2019.



https://sourceforge.net/projects/imagecomparator/
Image Comparator is an easy tool that finds duplicate image files in your system. 
In order to do that it looks your images pixel by pixel; therefore even if two images have different formats and file sizes(MB/KB) 
Image Comparator finds them. Supported formats: jpg, bmp, png, gif, tif, ico. Requirements: .NET Framework 4.0. Works only on Windows.




https://github.com/idealo/imagededup
imagededup is a python package that simplifies the task of finding exact and near duplicates in an image collection.
This package provides functionality to make use of hashing algorithms that are particularly good at finding exact 
duplicates as well as convolutional neural networks which are also adept at finding near duplicates. 
An evaluation framework is also provided to judge the quality of deduplication for a given dataset.

Following details the functionality provided by the package:
Finding duplicates in a directory using one of the following algorithms:
Convolutional Neural Network (CNN)
Perceptual hashing (PHash)
Difference hashing (DHash)
Wavelet hashing (WHash)
Average hashing (AHash)
Generation of encodings for images using one of the above stated algorithms.
Framework to evaluate effectiveness of deduplication given a ground truth mapping.
Plotting duplicates found for a given image file.

In order to find duplicates in an image directory using perceptual hashing, following workflow can be used:

Import perceptual hashing method

from imagededup.methods import PHash
phasher = PHash()
Generate encodings for all images in an image directory

encodings = phasher.encode_images(image_dir='path/to/image/directory')
Find duplicates using the generated encodings

duplicates = phasher.find_duplicates(encoding_map=encodings)
Plot duplicates obtained for a given file (eg: 'ukbench00120.jpg') using the duplicates dictionary

from imagededup.utils import plot_duplicates
plot_duplicates(image_dir='path/to/image/directory',
				duplicate_map=duplicates,
				filename='ukbench00120.jpg')

The complete code for the workflow is:


from imagededup.methods import PHash
phasher = PHash()

# Generate encodings for all images in an image directory
encodings = phasher.encode_images(image_dir='path/to/image/directory')

# Find duplicates using the generated encodings
duplicates = phasher.find_duplicates(encoding_map=encodings)

# plot duplicates obtained for a given file using the duplicates dictionary
from imagededup.utils import plot_duplicates
plot_duplicates(image_dir='path/to/image/directory',
				duplicate_map=duplicates,
				filename='ukbench00120.jpg')





https://github.com/melifaro11/ImageDupFinder
ImageDupFinder
ImageDupFinder is a simple C++ Qt application, which demonstrates the usage of perceptual hashing alrorithms to searching duplicate images in directories.
Introduction
A perceptual hash is a fingerprint of an image, that can be used to compare images by calculating the Hamming distance 
(which basically means counting the number of different individual bits). ImageDupFinder application implementiert 3 different hashing algorithms:
aHash
dHash
pHash




https://pypi.org/project/ImageHash/

https://github.com/bunchesofdonald/photohash (forked to imagehash)
PhotoHash
This was mainly created just for my own use and education. It's a perceptual hash algorithm, used to find if two images are similar.

Installation
pip install PhotoHash
Usage
average_hash
Returns the hash of the image using an average hash algorithm. This algorithm compares each pixel in the image to the average value of all the pixels.:

import photohash
hash = photohash.average_hash('/path/to/myimage.jpg')
distance
Returns the hamming distance between the average_hash of the given images.:

import photohash
distance = photohash.distance('/path/to/myimage.jpg', '/path/to/myotherimage.jpg')
is_look_alike
Returns a boolean of whether or not the photos look similar.:

import photohash
similar = photohash.is_look_alike('/path/to/myimage.jpg', '/path/to/myotherimage.jpg')
is_look_alike also takes an optional tolerance argument that defines how strict the comparison should be.:

import photohash
similar = photohash.is_look_alike('/path/to/myimage.jpg', '/path/to/myimage.jpg', tolerance=3)
hash_distance
Returns the hamming distance between two hashes of the same length:

import photohash
hash_one = average_hash('/path/to/myimage.jpg')
hash_two = average_hash('/path/to/myotherimage.jpg')
distance = photohash.hash_distance(hash_one, hash_two)
hashes_are_similar
Returns a boolean of whether or not the two hashes are within the given tolerance. Same as is_look_alike, but takes hashes instead of image paths:

import photohash
hash_one = average_hash('/path/to/myimage.jpg')
hash_two = average_hash('/path/to/myotherimage.jpg')
similar = photohash.hashes_are_similar(hash_one, hash_two)
hashes_are_similar also takes the same optional tolerance argument that is_look_alike does.

https://github.com/JohannesBuchner/imagehash
ImageHash
An image hashing library written in Python. ImageHash supports:

Average hashing
Perceptual hashing
Difference hashing
Wavelet hashing
HSV color hashing (colorhash)
Crop-resistant hashing
Travis Coveralls

Rationale
Image hashes tell whether two images look nearly identical. This is different from cryptographic hashing algorithms 
(like MD5, SHA-1) where tiny changes in the image give completely different hashes. In image fingerprinting, we 
actually want our similar inputs to have similar output hashes as well.

The image hash algorithms (average, perceptual, difference, wavelet) analyse the image structure on 
luminance (without color information). The color hash algorithm analyses the color distribution and 
black & gray fractions (without position information).

https://github.com/mattsta/imagehash
My changes/fixes/improvements to imagehash. Likely not merge-able upstream due to reformatting, but feel free to copy anything useful.





https://github.com/yingqichao/Qt-Image-Hash-with-multiple-methods (opencv)
A Qt tool for Image Hashing with some perceptual hashing algorithms.
Qt-Image-Hash-with-multiple-methods
A Qt-based image hash system that implements 5 methods for efficient detection of similar images from an image database, given a host image.
You have to include opencv_world340.dll to start the project, which also requires support of opencv-contrib.
How to use:
Select Folder(database that contains images) and host image -> Select Hash Method(at the bottom of GUI, or use the default pHash) -> 
Generate Hash Code for all images in the folder(press F5) -> Get the most similar images by comparison of Hash Code(press F6).
References for the methods implemented in this program:
Perceptual Image Hash(pHash) -> "A Visual Model-Based Perceptual Image Hash for Content Authentication", https://ieeexplore.ieee.org/document/7050251
Simplified Image Hash -> https://blog.csdn.net/sinat_26917383/article/details/78582064
Otsu's Method -> https://www.aliyun.com/zixun/wenji/1305502.html
Structual Similarity(SSIM) -> "Image quality assessment: from error visibility to structural similarity", https://ieeexplore.ieee.org/document/1284395
Local HSV Histogram -> https://blog.csdn.net/zhuiqiuk/article/details/54945624





https://github.com/aetilius/pHash
https://www.phash.org
pHash - the open source perceptual hash library
What is pHash?
pHash is an open source software library released under the GPLv3 license that implements several perceptual hashing algorithms, 
and provides a C-like API to use those functions in your own programs. pHash itself is written in C++.

Fixed Length MH Image Hash
We have developed a new image hash based on the Marr wavelet that computes a perceptual hash 
based on edge information with particular emphasis on corners. It has been shown that the human visual 
system makes special use of certain retinal cells to distinguish corner-like stimuli. It is the belief that this corner 
information can be used to distinguish digital images that motivates this approach. Basically, the edge information 
attained from the wavelet is compressed into a fixed length hash of 72 bytes. Binary quantization allows for relatively fast 
hamming distance computation between hashes. The following scatter plot shows the results on our standard corpus of images. 
The first plot shows the distances between each image and its attacked counterpart (e.g. the intra distances). 
The second plot shows the inter distances between altogether different images. While the hash is not designed to handle rotated images, 
notice how slight rotations still generally fall within a threshold range and thus can usually be matched as identical. 
However, the real advantage of this hash is for use with our mvp tree indexing structure. Since it is more descriptive than the dct hash 
(being 72 bytes in length vs. 8 bytes for the dct hash), there are much fewer false matches retrieved for image queries.
This new hash is available in version 0.8.0.

DCT Variable Length Video Hash
A variable length DCT video hash is included in the pHash library as of v0.6. 
It consists of the dct image hash applied to a select number of key frames chosen from the original image sequence. 
The key frames are selected using an adaptive thresholding technique and are based upon a standard framerate, so as not to be fooled by alterations in the frame rate.
Below are two graphs to show the results on ten different videos and their altered counterparts.
For this case, the alteration consists of inserting a few blank frames at the beginning and end of each file. 
We hope to include some more types of alterations soon. The first plot shows the similarity between each image and its altered counterpart, 
the "intras", and the second plot shows the comparisons between altogether different ("dissimilar") images. There does appear to be a clear threshold line that can be applied.

Hash Storage
As of version 0.5, the pHash library supports a hash storage technique that allows for quick and easy access to a 
database of hash values. Simply put, the anticipated sequence runs something like this:
Build a database of hash values from a corpus of files.
Go to step 3 or 4.
Add new hashes from a group of files.
Query the database with a query file.
Go to step 2.
The technique stores the hash values, or "data points" into a tree-like structure according to relative distances from 
chosen vantage points of the data set. This way the problem of storage and lookup of high dimensional data is avoided and 
the number of distance computations that must be made per query is minimized. The user need only specify the distance function.

Features:
Adjustable page sizes to accomodate any hash type or size.
Configurable tree parameters, e.g. branch factor, leaf capacity, etc.
Use of callback function enabling the user to provide a custom distance function.
Use of a special file format for persistent storage.
So far, preliminary test results reveal a 300% improvement over that of linear search, and the additional storage required 
amounts to less than 0.05% of the space required for entire database.

DCT Image Hash
The discrete cosine transform (DCT) is an efficient means to compute a hash from frequency spectrum data, and the 
distance calculation is relatively simple. While it is insufficient to consider image similarity in any semantically 
meaningful way, it does provide a hash as an ID for an image, and is robust against minor distortions, like small rotations, 
blurring and compression. The graphs below show the hamming distances (i.e. the number of bits that differ in the 64-bit hash) for 
two scenarios: the intra distances where the source images are from the same source only one is a distorted version of the other, 
and inter distances, where the two compared images are altogether different images. The main point is that a threshold of twenty-two, T=22, 
can be applied to determine if two images are indeed the same source image.
Note: Please ignore the x-axis on the second table. The x-axis is merely a list of comparisons between the specific images.

Radial Image Hash
This method tries to take into account geometric features of the image in extracting a hash value. The idea is to generate a 
feature vector from the variances of 180 lines drawn through the center of the image, and then compact the feature vector 
with the discrete cosine tranform (DCT). Images can be compared by looking for correlations between the images' hash values. 
Where the correlation exceeds a threshold level, the image can be judged to be the same image. From the graphs, a good threshold 
might be 0.91, but as you can see, there are a couple of comparisons that do fall into a gray area.

Histogram-based Image Hash
While the histogram-based method is not provided in pHash, it is shown here merely for comparison purposes. 
Here, the histogram of the image is used as a feature vector for the hash, and comparisons are performed as above, using the cross correlation function.
The next set of graphs show the intra and inter PCC values of the Histogram-based feature vector. 
While the intra PCC values are all above a threshold ~0.84, the inter PCC values show no clear threshold, 
indicating the histogram based approach would lead to a large number of false positives (e.g. images said to be a match but are truly not).

Bark Audio Hash
This hashing method for audio signals extracts a feature vector for every frame of audio from the bark scale 
frequency spectrum - that is, it only considers those frequencies to which the human auditory system is most 
sensitive. Here, frames are 0.21 seconds of audio and are 50% overlapping to minimize the impact of time shifts 
in audio files. As shown in the graphs, there is a clear difference between the inter and intra file comparisons. 
The distance plots are the distances between two audio signals. A comparison between two audio hashes necessarily 
produces a vector of distances that mark the distances at potential matching points, as the audio signals could 
concievably be matched anywhere in the signals. Thus, the distance that is plotted mark the point at which the 
distance is least. The confidence score is a rating of how good the match can be considered to be. The attacks, or 
distortions on the signal for the intra comparisons are mp3 compression at 32kbps and 128kbps, and telephone simulated filtering.

Development Guide
Hash Functions
First of all, it is important to mention that a perceptual hash (phash) is a signature of an underlying media source file's 
perceptual content. I say this because it is important to remember what it is not. It cannot do image or audio recognition of 
certain phrases or image features. It probably cannot even detect similar artifacts from two different source files - e.g. 
two different photographs of the same person. Although it may detect a similarity if the lighting and camera angles are nearly 
identical. Perceptual hashes are mainly for detecting duplicates of the same files in a way that the standard cryptographic hashes 
generally fail. Since the cryptographic hash algorithms compute the signatures from the specific byte stream in the file, it generally 
returns very different hashes depending on which file format is used and when minor image processing alterations are applied. 
Perceptual hashing aims to return a signature that is robust against such distortions, such that a distance measure between hashes 
returns a relatively close distance for similar hashes. Thus a threshold can be applied to determine if two files are the same or different.
So far, pHash is capable of computing hashes for audio, video and image files. You start by computing hash values for the media files. 
The following are the function prototypes for the three methods:

int ph_dct_imagehash(const char *file, ulong64 &hash);
ulong64* ph_dct_videohash(const char *file, int &Length);
uint32_t* ph_audiohash(float *buf, int N, int sr, int &nbFrames);

The image hash is returned in the hash parameter. Audio hashes are returned as an array of uint32_t types with the nbFrames 
parameter indicating the buffer length. Video hashes are returned as an array of uint64_t types with the Length parameter 
indicating the number of elements in the array. You must free the memory returned by ph_audiohash() and ph_dct_videohash() 
with free() when you are finished using it. For the audio hash, you must read the data into the buffer first. You do so with this function:

float* ph_readaudio(const char *filename, int sr, int channels, int &N);

(It is recommended you use sr as 8000 and just one channel.) N will be the length of your returned buffer. 
You must free the memory returned by ph_readaudio() with free() when you are finished using it (usually after a call to ph_audiohash()).

There are image hash functions that use the radial hash projections method, rather than the discrete cosine 
transform (dct), but their results have not shown to be as good as the dct.

Once you have the hashes for two files, you can compare them. The functions you use to compare two files are as follows:

int ph_hamming_distance(ulong64 hasha, ulong64 hashb);
double* ph_audio_distance_ber(uint32_t *hasha, int Na, uint32_t *hashb, int Nb, float threshold, int block_size, int &Nc);

The hamming distance function can be used for both video and image hashes. For audio distance, the threshold should be 
around 0.30 (0.25 to 0.35), the block_size should be 256. The block_size is just the number of blocks of uint32 types to 
compare at a time in computing the bit error rate (ber) between two hashes. It returns a double buffer of length Nc, which is 
a confidence vector. It basically gives a confidence rating that indicates the similarity of two hashes at various positions of 
alignment. The maximum of this confidence vector should be a fairly good indication of similarity, a value of 0.5 being the threshold. 
You must free the memory returned by ph_audio_distance_ber() with free() when you are finished using it.

If you want to use the radial hashing method for images, the function for getting the hash is here:

ph_image_digest(const char *file, double sigma, double gamma, Digest &dig, N);

Use values sigma=1.0 and gamma=1.0 for now. N indicates the number of lines to project through the center for 0 to 180 degrees 
orientation. Use 180. Be sure to declare a digest before calling the function, like this:

Digest dig;
ph_image_digest(filename, 1.0, 1.0, dig, 180);

The function returns -1 if it fails. This standard will be found in most of the functions in the pHash library.

To compare two radial hashes, a peak of cross correlation is determined between two hashes:

int ph_crosscorr(Digest &x, Digest &y, double &pcc, double threshold=0.90);

The peak of cross correlation between the two vectors is returned in the pcc parameter.

MVP Hash Storage
Using the mvp functions for hash storage is fairly straightforward - the basic idea being to build, add and 
query the db, and there are three functions in the api to do just that:

MVPRetCode ph_save_mvptree(MVPFile *m, DP **points, int nbpoints);
MVPRetCode ph_add_mvptree(MVPFile *m, DP *new_dp);
MVPRetCode ph_query_mvptree(MVPFile *m, DP *query, int knearest, float radius, DP **results, int *count);

The functions are all documented in the pHash.h header file, but MVPRetCode is simply an enumerated type to 
indicate an error message. Zero indicates success; nonzero values indicate error. A datapoint, or DP is just a 
pHash structure to hold a file name and hash value.

First, you will need an MVPFile struct initialized with appropriate values.

MVPFile mfile;
mfile.branchfactor = 2;
mfile.pathlength = 5;
mfile.leafcapacity = 25;

Or, you can just use the void ph_init_mvpfile(MVPFile *m) function to initiate the fields to those values. 
You only need to set these three members if you wish to experiment with other values. You will also need to set these fields:

mfile.hash_type = (HashType)type
mfile.hashdist = funcCB

Obviously, the values here will depend on what hash you are using and what distance function you want for the callback. 
The callback must follow this form:

float hashdist(DP *dpA, DP *dpB);

In order to build the db, you will need an array of datapoints. The pHash function char** ph_readfilenames(const char* dirname, int &N) will 
read the files from a given directory and give you the list of filenames. The reference parameter, N will tell you how many files there are. 
From there you can loop through the files, create a hash for each file, and store the hash and filename in the new datapoint struct. 
Use DP *dp = ph_malloc_datapoint(m.pathlength, m.hash_type) to get a pointer to a new DP struct. Be sure to assign the filename, hash 
and hash_length to the respective fields in the DP.

The pointers to the newly created dp's can be stored in an array of pointers to the datapoints. This is important, because when points 
are sorted into the file format, the actual datapoints are never reassigned, just the pointers to those original datapoints.

In the examples directory in the download, you will find the following files: build_mvptree.cpp, add_mvptree.cpp, query_mvptree.cpp. 
These should demonstrate how to do each operation for the dct image hash. You will need a directory of images with at least 28 images - 
i.e. more images than it takes to fill a leaf node of the tree structure, or three greater than the mfile.leafcapacity. 
You will also find three similarly named files for the audio hash function.



https://github.com/saolaolsson/pixiple
What's Pixiple?
Pixiple is a Windows application that searches your files for images that are similar in pixel and metadata content and 
presents you with a sorted list of similar image pairs to compare.

Unlike similar programs, Pixiple will attempt to find not only duplicate images but images that have something in 
common, either in their general appearance or their metadata, and may belong together. 
Pixiple also lets you compare two images (with synchronised zooming and panning) and check for minute quality 
differences (with the "swap images" button).

Features
Uses both pixel and metadata content (dates, location, camera) to find related images.
Fast, multi-threaded processing.
Minimalist, resizable, localised, DPI-aware UI.
Portable: single file, no settings saved, no installation required.
Free, open source, no restrictions, no nonsense.
Optimised for cows.
Misfeatures
Support for image file formats supported by Windows Imaging Component only (PNG, JPEG, GIF, TIFF, BMP).
No caching of previous results, so all images must be processed every new scan.
No installer.

What's similar?
Pixiple will easily detect images that are identical, have identical pixel content, are uniformly resized, 
flipped, rotated (90, 180, 270 degrees), are cropped along a single edge, or have minor differences in pixel content.

Pixiple is less well able to detect similar images with significant changes to pixel content (cropping or 
change of brightness, contrast, saturation, etc).

File metadata (name, size, date, format) is ignored when detecting similarity. 
By default, image paths are also ignored.




https://tn123.org/simimages/
SimilarImages
SimilarImages is a utility program to analyze and search large media collections (images/videos) for near duplicates, 
meaning media showing the same thing but e.g. in different file formats, compression levels, sizes. SimilarImages first analyzes a file, 
generating a color/location footprint of a normalized thumbnail image of a file, and then compares these footprints. 
Analyzation results will be cached and stored on disk, so that subsequent runs become faster.

SimilarImages is a rather old tool I started developing a long time ago. While it does the job at hand fairly well, 
due to the legacy code base it is not feasible to develop new features or significantly improved matching without a major effort, 
one which I cannot provide at the moment. Hence there will be no significant new features, incl. Windows 64-bit (32-bit runs on 64-bit Windows), 
Mac OSX or Linux versions. You might however have luck with WINE.

How does SimilarImages find those similar images?
It uses a somewhat modified version of the "Quadrant RGB" algorithm and futhermore considers some other meta-information provided by the source files.
This basicially means: the images will be split into quadrants and average RGB values are computed.
Pairs that have pretty "close" avg. numbers are considered being "similar".




https://qiplex.com/software/speedy-duplicate-finder/#all-features
Extremely fast
duplicate file finder!
All Features
Blazingly Fast
Scanning 1000 files takes
less than a second!

Disk Support
You can search for duplicates not only
in folders but on entire disk!

Intuitively Simple
It is so simple as 1 2 3.
For all ages and for all users!

Multiple Folders
You can find duplicates
in many folders at the same time!

Smart Filters
Easily filter group of duplicates
by Kind, by Extension or by Size

Get & Go
Doesn't require installation!
Good to go as soon as downloaded!

Easy Review
Review your duplicates easily.

Smart Search
Search for duplicates
faster!

Truly Versatile
Supports major desktop platforms:
Windows, macOS and Linux!


https://github.com/theophanemayaud/video-simili-duplicate-cleaner/
https://theophanemayaud.github.io/video-simili-duplicate-cleaner/
Video simili duplicate cleaner
Find and remove duplicates : Video simili duplicate cleaner is a program that can find duplicate or simply similar video files. 
Video simili duplicate cleaner compares the actual video content regardless of different format or compression used, while 
other software only find identical files (digital video fingerprinting).

Features
Find similar videos, even if they are not exactly the same (different metadata, different compression, different sizes...)
Simple, easy to use graphical user interface for comparing videos.
Supports all widely used video formats.
Manually select videos to remove, or use automatic selection criteria.
Safely move duplicates to trash (you must empty your trash afterwards to really delete the videos), or to a folder of your choosing.
Specify locked folder, to make sure the videos within are never moved.
Multithreaded operation, makes full use of all available CPU threads.
Employs two powerful image comparison methods: pHash (perceptual hashing) and SSIM (structural similarity).
Uses caching for blazing fast second runs.
macOS: supports Apple Photos, to save more space.
Cross-platform and open source development: source code available for compiling on Windows/Linux/macOS.

pHash / SSIM:
pHash is a fast and accurate algorithm for finding duplicate videos.
SSIM is even better at finding matches (less false positives especially, not necessarily more matches). Noticeably slower than pHash.

SSIM block size:
A smaller value means that the thumbnail is analyzed as smaller, separate images. Note: selecting the value 2 will be quite slow.

Comparison threshold:
When comparing two videos, a comparison value is generated. If the value is below this threshold, videos are considered a match.
threshold:
A threshold that is too low or too high will either display videos that don’t match or none at all.

Raise / lower threshold by:
These two options increase/decrease the selected comparison threshold when two videos have almost same length. 
It will therefore be more or less likely that they are reported as duplicates.




https://github.com/GoFigure-LANL/VisHash
Visual hash for comparing visual similarity of images; which works on a wide variety of image types such as photos, logos, figures, diagrams and drawings.
VisHash
This repository is the official implementation of VisHash: Visual Similarity Preserving Image Hashing for Diagram Retrieval.
Requirements
PIL or pillow
scikit-image
matplotlib
cairosvg (optional: necessary for reading SVG images)
Calculate image hashes
calc_vishash.py --dataname example --image_path /path/to/images

Input is a directory containing images. A hash will be computed for each image. 
Two files are written: filenames_example.csv and signatures_example.npy, so that the order of the hashes (signatures) 
matches the order given in filenames_example.csv. Any unreadable or unsupported files will generate a log message, 
but processing on other images will continue.

Find duplicate images
calc_matches.py --postfix example --threshold 0.2

This script will read filenames_example.csv and signatures_example.npy (such as produced by calc_vishash.py). 
The distance between pairs of image hashes is calculated, then filtered based on the given threshold to output 
a list of matches in matches_example.csv.

Evaluation
Evaluation of the algorithm is performed by finding the closest matches in a given dataset, and then evaluating 
(by hand) whether these matches are correct. The results of this analysis are used to generate a plot of precision 
versus number of retrieved pairs.

calc_mindist_allpairs.py --postfix example --n_matches 1000

The above example will generate a list of 1000 image-pairs with the lowest distance. The script will read filenames_example.csv 
and signatures_example.npy (such as produced by calc_vishash.py); and write mindist_example.csv with the list of 1000 matches.

calc_query_matches.py --postfix example -k 5

The above match-per-query example will generate a list of the top-5 matches (hashes with lowest distance) for each 
of the hashes in the given array of hashes. The script will read filenames_example.csv and signatures_example.npy 
(such as produced by calc_vishash.py); and write topk_example.csv with the list of top-5 matches per hash.




http://www.visipics.info/index.php?title=Main_Page
Welcome to VisiPics
VisiPicsInterface.jpg
If you get too many pictures on your harddrive, downloaded or photographied, from several different sources, 
it may happen that you have many duplicates. In that case you need a quick and easy to use program that finds and deletes all your duplicates.

VisiPics does more than just look for identical files, it goes beyond checksums to look for similar pictures and 
does it all with a simple user interface. First, you select the root folder or folders to find and catalogue all of your pictures. 
It then applies five image comparison filters in order to measure how close pairs of images on the hard drive are.

Visipic is a program that is considerably faster than any other commercial product and has an interface that let you 
do other things while Visipics automatically finds your duplicated images. It will detect two different resolution files 
of the same picture as a duplicate, or the same picture saved in different formats, or duplicates where only minor cosmetic changes have taken place.

All detected duplicates are shown side by side with pertinent information such as file name, type and size being displayed. 
Its auto-select mode let you choose if you want to keep the higher resolution picture, space-saving filetype, smaller filesize 
or all of the above. If you are insecure over what you really want to delete you can manually select the images you don't want 
to keep and delete them yourself.

Intelligence
VisiPics doesn't detects some duplicates
I teach VisiPics to solve different kind of problems. For exemple, two pictures can have a different contrasts, 
a title on one, a decorative border on the other, etc... I do my best to teach it everything, but some problems are 
just too complex to program. For example strong rotations, cropped parts and line Art against its colored version are 
three frequent problems VisiPics isn't able to solve (yet). If you can, send those duplicates to my email, I will use them to tune the algorithm.

Some people told me about wavelets to solve the strong rotations and cropped pictures, I won't implement this algorithm because:
It requires a lot of computing power, and would slow down VisiPics way too much
Also it increases dramatically the number of false-positives, which I don't want
I have duplicate animated GIFs, but VisiPics doesn't detect them
VisiPics doesn't work on animations and never will. Try a duplicate files finder for those instead.

Some of my PNG files are black
VisiPics doesn't compare transparency (aka Alpha channel), so it converts transparent parts to black. 
The picture will look strange, but it shouldn't affect VisiPics' ability to find those duplicates.





https://www.mindgems.com/products/VS-Duplicate-Image-Finder/VSDIF-About.htm
Duplicate photo finder "looks" at your photos to find similarities. Find look-alike and duplicate images in a 
folder, computer, network, or find duplicates in Adobe Lightroom. Organize photos by eliminating useless 
repeating pictures - save valuable disk space. Identify similar and duplicate images even if they are edited, rotated, flipped, or color-corrected.

Specify the percent of image similarity that will be used during the scan. It can broaden your results or 
specify an exact match varying only in image format or size.

After the scan process is finished the program shows all the duplicate images in groups and can automatically mark the 
smaller resolution and/or smaller file size images to be deleted - the lower quality ones.

Find similar images
Find exact duplicate images
Find similar and duplicate EXIF tags
Preview images
Auto-check options
Locate files in Windows Explorer
Move/delete duplicate images
Open images with their associated program
Quick Check/Uncheck
Save/load projects
Compare folders
Scan network and mapped drives
Disable preserve group integrity on sort
Filtering options
Automatically delete empty folders
Export results to CSV or XML
Disable folder for self-scan
Camera RAW images support
HDPhoto images support
DICOM images support
WebP images support
HEIC images support
Match full path in "Exclude Files" dialog
Command-line features
Unlimited licenses within a company/organization
Scan Adobe Lightroom catalogs








https://github.com/ImageMagick/ImageMagick
ImageMagick is a software suite to create, edit, compose, or convert
  bitmap images. It can read and write images in a variety of formats (over
  200) including PNG, JPEG, GIF, HEIC, TIFF, DPX, EXR, WebP, Postscript,
  PDF, and SVG. Use ImageMagick to resize, flip, mirror, rotate, distort,
  shear and transform images, adjust image colors, apply various special
  effects, or draw text, lines, polygons, ellipses and Bezier curves.

  ImageMagick utilize multiple computational threads to increase performance
  and can read, process, or write mega-, giga-, or tera-pixel image sizes.

  Here are just a few examples of what ImageMagick can do:

	  * Format conversion: convert an image from one format to another (e.g.
		PNG to JPEG).
	
	  * Image identification: describe the format and attributes of an image.
	
	  * Morphology of shapes: extract features, describe shapes and recognize
		patterns in images.
	
	  * Discrete Fourier transform: implements the forward and inverse DFT.
	  * Perceptual hash: maps visually identical images to the same or similar
		hash-- useful in image retrieval, authentication, indexing, or copy
		detection as well as digital watermarking.
	
	  * Threads of execution support: ImageMagick is thread safe and most
		internal algorithms are OpenMP-enabled to take advantage of speed-ups
		offered by multicore processor chips.
	
	  * Heterogeneous distributed processing: certain algorithms are
		OpenCL-enabled to take advantage of speed-ups offered by executing in
		concert across heterogeneous platforms consisting of CPUs, GPUs, and
		other processors.
	
  Want more performance from ImageMagick? Try these options:

	* add more memory to your system, see the pixel cache;
	* add more cores to your system, see threads of execution support;
	* reduce lock contention with the tcmalloc memory allocation library;
	* push large images to a solid-state drive, see large image support.

  If these options are prohibitive, you can reduce the quality of the image
  results. The default build is Q16 HDRI. If you disable HDRI, you use
  half the memory and instead of predominantly floating point operations,
  you use the typically more efficient integer operations. The tradeoff
  is reduced precision and you cannot process out of range pixel values
  (e.g. negative). If you build the Q8 non-HDRI version of ImageMagick,
  you again reduce the memory requirements in half-- and once again there
  is a tradeoff, even less precision and no out of range pixel values. For
  a Q8 non-HDRI build of ImageMagick, use these configure script options:
  --with-quantum-depth=8 --disable-hdri.








https://hackerfactor.com/blog/index.php%3F/archives/432-Looks-Like-It.html
So how do you create a perceptual hash? There are a couple of common algorithms, but none are very complicated. 
(I'm always surprised that the most common algorithms even work, because they seem too simple!) 
One of the simplest hashes represents a basic average based on the low frequencies.

With pictures, high frequencies give you detail, while low frequencies show you structure. 
A large, detailed picture has lots of high frequencies. A very small picture lacks details, so it is all low frequencies. 
To show how the Average Hash algorithm works, I'll use a picture of actress Alyson Hannigan.


Reduce size. The fastest way to remove high frequencies and detail is to shrink the image. 
In this case, shrink it to 8x8 so that there are 64 total pixels. Don't bother keeping the aspect ratio, just crush it down to fit an 8x8 square. 
This way, the hash will match any variation of the image, regardless of scale or aspect ratio.

Reduce color. The tiny 8x8 picture is converted to a grayscale. This changes the hash from 64 pixels (64 red, 64 green, and 64 blue) to 64 total colors.
Average the colors. Compute the mean value of the 64 colors.
Compute the bits. This is the fun part. Each bit is simply set based on whether the color value is above or below the mean.
Construct the hash. Set the 64 bits into a 64-bit integer. The order does not matter, just as long as you are consistent. 
(I set the bits from left to right, top to bottom using big-endian.)
 =  = 8f373714acfcf4d0
The resulting hash won't change if the image is scaled or the aspect ratio changes. Increasing or decreasing the brightness or contrast, 
or even altering the colors won't dramatically change the hash value. And best of all: this is FAST!

If you want to compare two images, construct the hash from each image and count the number of bit positions that are different. 
(This is a Hamming distance.) A distance of zero indicates that it is likely a very similar picture (or a variation of the same picture). 
A distance of 5 means a few things may be different, but they are probably still close enough to be similar. But a distance of 10 or more? 
That's probably a very different picture.

Getting Funky With pHash
While the Average Hash is quick and easy, it may be too rigid of a comparison. For example, it can generate false-misses if there is a 
gamma correction or a color histogram is applied to the image. This is because the colors move along a non-linear scale -- changing where 
the "average" is located and therefore changing which bits are above/below the average.

A more robust algorithm is used by pHash. (I use my own variation of the algorithm, but it's the same concept.) 
The pHash approach extends the average approach to the extreme, using a discrete cosine transform (DCT) to reduce the frequencies.
Reduce size. Like Average Hash, pHash starts with a small image. However, the image is larger than 8x8; 32x32 is a good size. 
This is really done to simplify the DCT computation and not because it is needed to reduce the high frequencies.

Reduce color. The image is reduced to a grayscale just to further simplify the number of computations.
Compute the DCT. The DCT separates the image into a collection of frequencies and scalars. 
While JPEG uses an 8x8 DCT, this algorithm uses a 32x32 DCT.
Reduce the DCT. While the DCT is 32x32, just keep the top-left 8x8. Those represent the lowest frequencies in the picture.
Compute the average value. Like the Average Hash, compute the mean DCT value (using only the 8x8 DCT low-frequency values and 
excluding the first term since the DC coefficient can be significantly different from the other values and will throw off the average). 
Thanks to David Starkweather for the added information about pHash. He wrote: "the dct hash is based on the low 2D DCT coefficients 
starting at the second from lowest, leaving out the first DC term. This excludes completely flat image information (i.e. solid colors) 
from being included in the hash description."
Further reduce the DCT. This is the magic step. Set the 64 hash bits to 0 or 1 depending on whether each of the 64 DCT values is above or 
below the average value. The result doesn't tell us the actual low frequencies; it just tells us the very-rough relative scale of the frequencies to the mean. 
The result will not vary as long as the overall structure of the image remains the same; this can survive gamma and color histogram adjustments without a problem.
Construct the hash. Set the 64 bits into a 64-bit integer. The order does not matter, just as long as you are consistent. 
To see what this fingerprint looks like, simply set the values (this uses +255 and -255 based on whether the bits are 1 or 0) and 
convert from the 32x32 DCT (with zeros for the high frequencies) back into the 32x32 image:
 = 8a0303f6df3ec8cd
At first glance, this might look like some random blobs... but look closer. There is a dark ring around her head and the dark 
horizontal line in the background (right side of the picture) appears as a dark spot.
As with the Average Hash, pHash values can be compared using the same Hamming distance algorithm. 
(Just compare each bit position and count the number of differences.)

Best in Class?
Since I do a lot of work with digital photo forensics and huge picture collections, I need a way to search for similar pictures. 
So, I created a picture search tool that uses a couple of different perceptual hash algorithms. In my unscientific but long-term-use experience, 
I have found that Average Hash is significantly faster than pHash. Average Hash is a great algorithm if you are looking for something specific. 
For example, if I have a small thumbnail of an image and I know that the big one exists somewhere in my collection, then Average Hash will find it very quickly. 
However, if there are modifications -- like text was added or a head was spliced into place, then Average Hash probably won't do the job. 
While pHash is slower, it is very tolerant of minor modifications (minor being less than 25% of the picture).

Then again, if you are running a service like TinEye, then you're not going to compute the pHash every time. 
I am certain that they have a database of pre-computed hash values. The basic comparison system is extremely fast. 
(There are some heavily optimized ways to compute a Hamming distance.) So computing the hash is a one-time cost and 
doing a million comparisons in a few seconds (on one computer) is very realistic.

Variations
There are variations to the perceptual hash algorithm that can also improve performance. For example, the image can be cropped 
before being reduced in size. This way, extra empty space around the main part of the image won't make a difference. 
Also, the image can be segmented. For example, if you have a face detection algorithm, then you can compute hashes for each face. 
(I suspect that TinEye's algorithm does something similar.)

Other variations can track general coloring (e.g., her hair is more red than blue or green, and the background is closer to 
white than black) or the relative location of lines.

When you can compare images, then you can start doing really cool things. For example, the search engine 
GazoPa [now offline] allows you to draw a picture. As with TinEye, I don't know the details about how GazoPa works. 
However, it appears to use a variation of the perceptual hash. Since the hash reduces everything down to the lowest 
frequencies, my crappy line drawing of three stick figures can be compared with other pictures -- likely matching 
photos that contain three people.

There is also the spectral hashing algorithm, which seems to have quite good performance for image retrieval. 
The big thing is to compute the hash function after a suitable image transformation.
I did not test it for retrieval tasks, but used it to query image patches (5x5 to 11x11 pixels), and it was quite impressive.
See it here:
http://www.cs.huji.ac.il/~yweiss/SpectralHashing/
I hadn't heard of spectral hashing. It really looks like a subset of the DCT hashing that pHash uses. 
In their example, they use 16 bits rather than 64 bits, and sine instead of cosine [a DST instead of a DCT], but it is the same basic concept.




AHASH

sample A (from average hash, introduction of "the waifu") is a JPEG, 69007 bytes, 500x500 pixels, 26577 colors, 67 urls.
lets just put this aside for now.

sample B (from average hash, step "1. reduce size") is a PNG, 289 bytes, 8x8 pixels, 63 colors.
text suggests B is a scaled version of A, visual inspection confirms this as quite possible.
a semi-exhaustive search over different scaling implementations and precisions finds a very
close (though not perfect) match with CImg::resize in "moving average interpolation" mode using high precision math.

the steps 2-4 come without sample data, this makes direct validation complicated.
detail on the step 2 "grayscaling" is missing entirely.
detail on steps 3+4 "conversion to bits" are present, lacking sample data one can not be sure usage of the term "mean" is
meant as detail though. the term "average" is used as well.
the wording in the similar steps 5+6 of the phash section avoids using the term "mean", using "average".
(while phash is actualy using median, not mean)

sample C (from average hash, step "5. construct the hash") is a 64bit value in string and graphical representation.
visual inspection suggests both representations may indeed show the same 64bit value.
the sample has 35 bits set to 1, which (assuming the "high precision math" indication on sample B holds) strongly suggests
there was actualy the mean value used in steps 3+4. (high precision and median will tend to yield exactly 32 bits set, low
precision and median with >= condition can yield >32)

for now, a placeholder implementation is used:
a simple CImg.RGBtoYCbCr().channel(0) conversion for the grayscaling.
a CImg.unroll("x") followed by a CImg.mean() and using (val >= mean) for determining the bit value.
the hash value X obtained this way for sample A is e7c7cbc2c4f4fae8.

comparing our own hash value X with sample C gives a hamming distance of 29, indicating a significant difference.
at this point headscratching ensued.

the implementation was checked in various different ways, all the way down to making sure the hamming distance calculation worked.
different variants of step 2 (grayscaling) were tested: classic 30+59+11, simple sum, squared sum, varying precisions...
different variants of step 3+4 (bitfolding) were tested: mean, median, varying condition (>= vs >), varying precisions...
no relevant progress was made in getting the selfcomputed hash for sample A closer to sample C.

at this point a visual comparison of samples B and C led to the discovery that there are significant differences in the overall
structure of the sample data, in particular in the left quarter of the square layout.
in sample B (and sample A) there are about 25% on the left taken up by blank wall, before the start of the circular shape.
in sample C the circle seems to clip the left and top boundary.

sir, we may have a case of "cropping the waifu"!

the validation code was reverted to the original state of high precision, YCbCr grayscaling, >= mean.
a semi-exhaustive search of possible cropping parameters applied to sample A was performed.
about 3150 matches for the hash presented as sample C were found when cropping 90-100 pixels left, 0-50 top, 0-15 bottom.
many more close matches (hamming distance < 5) were obtained in the same range.

a less-exhaustive search using low precision and median calculation found 25 matches with crops 97-102 left, 7-11 and 21-37 top, 0-6 bottom.

note the ranges given are those where matches were actualy found, the covered searchspace was ... wider.

the observed distribution of matches suggests the hashing is insensitive to about 10 pixels of cropping.
this could also be an artifact from the scaler.
so there might not have been any top/bottom cropping at all, but surely a good chunk of cropping on the left.
a larger cropping on the left with a potential smaller cropping at the top/bottom could be consistent with a
manual "click top-left and drag to bottom-right" cropping operation being performed.

possibility of cropping on the right side was not investigated mainly for reasons related to the impact on workload
of adding a fourth dimension to the search space.

different handling of pixel coordinates (mainly whether they count from 0 or from 1) between the involved cropping implementations 
(gimp, imagemagick, cimg) was identified as a potential source of error during the cropping search but
was not investigated closer since the hashing seems to be insensitive to off-by-one differences in general and enough
results matching sample C were obtained. the above pixel ranges for crops should be read as having a +/-2 error potential.

the difference between sample B and the local result obtained with CImg is small enough to be explained by differences in
two implementations of the same scaler or even by the same implementation being used with different precisions.
different implementations/precisions of converting a jpeg into RGB data could also explain the observed differences.
no closer investigation was performed.

no closer investigation of the impact of the different grayscalers on the hash was performed.
the data obtained while headscratching indicates no significant differences in the tested implementations.
a grayscaling based on image perception (yuv chan0, 30+59+11) might be better suited than the purely technical channel reductions.
the conversion to bits in step 3+4 compensates for most differences in post-grayscaling value-range, with
median-based calculation being completely insensitive and mean-based being mostly insensitive to scaling of the absolute
values while keeping the relations between the pixels.

so, it seems the lady got cropped somewhere between step 1 and 5.

various evidence in this case can be found in http://tma.no-ip.de/waifu/
of particular interest might be ...

http://tma.no-ip.de/waifu/hashimg.cpp
snapshot of the code used above.
generates average hash (AHASH) and phash (both in mean and median flavors) and RHASH (see comment 11).
littered with a lot of ifed-out debugprints that might help to understand whats going on.
comments on cimg download (no need to install) and compiling at the top.

http://tma.no-ip.de/waifu/test-crop-95-50-10.png
a cropped version of sample A that hashes to sample B for hashimg AHASH mean.

http://tma.no-ip.de/waifu/rgb4x4-cimg-vs-im.jpg
lower end of ten matches found by running the CImg generated RHASH for sample A against a database of 56M imagemagick generated vectors.
results #1 to #7 are all resaved/scaled variants of sample A.
result #8 is sample A, the vector distance of 29 is due to the different implementations used.
result #9 ... i will get to that in a minute.
result #10 indicates the lady got cropped before.
perfdata indicates it was comparing about 486mil images per second, including backend overhead and resulthandling.

http://tma.no-ip.de/waifu/result9.png
there is a clear need for you to clear her reputation by showing in a way of your choice that it was the picture and not herself being violated there.
and it only counts if you provide enough details and perhaps even 20 lines of code that can be used for validation. ;-)

and could you please provide the hash for sample A, and/or the image used for generating the hash sample C?


PHASH

ph_dct_imagehash from pHash.cpp from pHash-0.9.4 is investigated.
you can find a copy to stare at here: http://tma.no-ip.de/pHash-0.9.4/src/pHash.cpp
just search for ph_dct_imagehash, the code should be easy enough to follow. mostly.
you might also want to play around with PHASH_DEBUG in hashimg.cpp to see the gory details.

the image is loaded with a low precision type CImg<uint8_t>.
all following computations are done on higher precision CImg<float>.

the spectrum == 4 handling seems broken, img is in an undefined state at the time, the width/height/depth should probably be taken from src.

the application of meanfilter (after the conversion to grayscale) seems to result mainly in a 7x7 blur and a shift in value range.

the scaling to 32x32 (after grayscale and meanfilter) is done with CImg default of "nearest-neighbor interpolation".
without much investigation, the result looks worse than the "moving average interpolation" used in the AHASH sample.

my lack understanding of dct operation in general and the cpp-template mess that is used in the specific implementation results in me
not understanding what exactly is going on in the magic step even though i have been staring at it for a week now and
stepped through part of it in gdb.
the result in dctImage shows the expected concentration of high-absolute values in the top left section of the 32x32 grid.

from the 32x32 result of the magic step, a 8x8 subsection starting at (1,1) (while counting from 0) is taken.
this discards not just the high value at (0,0), but the whole row and column 0.

the 8x8 subsection is converted to a bit vector using the median (not mean!) value for filtering.
this results in the bits being mainly set depending on whether the output of the magic step in the position was positive or negative.
and a narrowed hash value range from the exactly-50%-bits-set coming with it.

it seems there is some potential for improvement.

only been playing around for it for some hours.
the following is just me thinking out loud, i will probably try some/all of this over the next weekend, and probably report back on it.

- try selecting from 0,0

- the blur filter has to go.
it seems to make phash perform quite horrible on scaled down versions of images.
replace blur + lowquality scaler by f.ex. the scaler used for the AHASH implementation.

- try different order of components.
like replacing the rectangular selection of 8x8 dct by the first 64 elements returned by the jpeg zig-zag pattern scaled to 32x32.
or keeping rectangular selection and going zigzag on the bitfolding.

- the mean vs median question.
i actualy like the median since it eliminates the 0,0 problem in an elegant way.
when using mean, exclude 0,0 from mean calculation but not bitcounting.

- try whether scaling before or after grayscaling makes a difference.

- set up proper test cases for "scaling" and "finding in pile" tasks.

- try to find an answer to "how far does using median restrict the hashspace".
or "how many 64 bit values with exactly 32 bit set to 1 do exist".

any suggestions?


RHASH

the image hash pointed out in comment #11 by Mr Schwartz has actualy been my workhorse for image comparison since about 2005.
as the article referenced there contains a commented implementation i will spare you the details.
not that there are a lot, it is pretty straightforward.

the hash data is 48 byte of rgb data, the image scaled to 4x4.
the comparison metric is sum of absolute differences: abs(a[0]-b[0]) + abs(a[1]-b[1]) + ... + abs(a[47]-b[47])
(or the 48 dimensional manhattan distance.)

to use the words someone had on AHASH performance: the result is better than it has any right to be.

now, some tactical magic that can be used with AHASH too.
since there is a welldefined relationship between components of the hash and areas of the input image it is possible
to apply "masks" and "transformations" at comparison time.

"masks" meaning you can do the comparison with restrictions like "ignore the bottom 25% of the image, someone put text there".
with RHASH this goes down to a granularity of color channel or just ignoring some (f.ex. the lowest two) bits of some/all values.
masks need to be implemented in the "performance" codepath of the comparison, but as all involved data has to be at the cpu for
the following distance calculation anyways the performance lost to the additional and and/or or operation(s) is small.

"transformations" meaning you can do searches for rotations in 90degree steps, mirroring, inverts.
with RHASH you get additional options like color channel swaps.
transformations are simply applied to the searchedfor vector, then a regular search with or without mask(s) is performed.

this comes at the price of having six times (48b vs 8b) the memory footprint for the vectors compared with AHASH or PHASH.

on a 2007 vintage Athlon x2 3GHz i am seeing comparison rates of about 115M vectors per second for somewhat optimized
code working on vector data that is already in memory while returning the best 10 results.

the limit on this platform for a straight single-target search is mainly the memory bandwidth.
48byte * 115M is about 5.5GByte/s, well more than the 3GB/s the athlon with its ddr2 can actualy handle.
did i mention optimizations?

on platforms with more memory bandwith than possibly useful (like core-i5, 11-12GB/s) you get about 200-250Mcps for a single core of about 3GHz.
the optimizations there are somewhat different from those for membw-bound operation.

starting to use more cpu cores for vector comparison will put any known CPU into a membw-bound situation.

one way to put excess cpu under membw-bound conditions to use is batch searches.
you take more than one reference vector and put them as close to the cpu as possible, registers or l1 cacheline.
then iterating over the database vectors (the membw-intensive part) you compare each loaded dbvector to your
multiple refvectors.
this puts the comparison back to being cpu-bound. quickly.
but it also means you are getting 1-3 additional comparisons "for free" in the same wallclock time.
now multiply by cores.

besides actual batch searching for different image vectors, this can be combined with transformations, like querying batches
of a reference image and nine of its common transformations (rotations, mirrors) as a batch of ten at a limited performance
impact that shows mostly linear scaling with batchsize beyond the "for free" point of unused cpu.



As I mentioned, my code isn't exactly the same as pHash. Among other things, I don't bother with any pre-blur. 
(Then again, I didn't look at the pHash source code, so I didn't even know they did that. :-) )
Applying a pre-blur, such as a Gaussian blur, would be just another step to reduce high frequencies. 
They could be doing it to speed up the file size reduction step. (Blur the picture, then use pixel sampling to shrink the picture. 
This could be faster than doing a pixel average to shrink the image, depending on how it is implemented.)

Gaussian smoothing is commonly used with edge detection. Most edge-detection algorithms are sensitive to noise; 
the 2-D Laplacian filter, built from a discretization of the Laplace operator, is highly sensitive to noisy environments. 
Using a Gaussian Blur filter before edge detection aims to reduce the level of noise in the image, which improves the 
result of the following edge-detection algorithm. This approach is commonly referred to as Laplacian of Gaussian, or 
LoG filtering. With more smoothing, fewer edges are detected.


I wonder if we can use Walsh-Hadamard Transform instead of DCT?
www.musicdsp.org/showone.php?id=18
Honestly, I've never heard of the Walsh-Hadamard Transform until you mentioned it. Is there a better description of what it does beyond the vague Wikipedia articles?
http://en.wikipedia.org/wiki/Fast_Walsh%E2%80%93Hadamard_transform
http://en.wikipedia.org/wiki/Walsh%E2%80%93Hadamard_transform
From what I gather, WHT is another signal decomposition function, similar to FFTs. The reason we use DCTs instead of FFTs has to 
do with the number of parameters. 8x8 values yields 64 DCT scalars but 128 FFT scalars (64 real + 64 imaginary). 
Since WHT would transform 8x8 real inputs into 64 real outputs, I don't see why it wouldn't work as a variant to the DCT approach.









https://www.hackerfactor.com/blog/?/archives/529-Kind-of-Like-That.html
About 8 months ago I wrote a blog entry on algorithms for comparing pictures. Basically, if you have a large database of pictures and 
want to find similar images, then you need an algorithm that generates a weighted comparison. In that blog entry, 
I described how two of the algorithms work:
aHash (also called Average Hash or Mean Hash). This approach crushes the image into a grayscale 8x8 image and sets the 64 bits in the 
hash based on whether the pixel's value is greater than the average color for the image.

pHash (also called "Perceptive Hash"). This algorithm is similar to aHash but use a discrete cosine transform (DCT) and compares based 
on frequencies rather than color values.
As a comment to the blog entry, David Oftedal suggested a third option that he called a "difference hash". It took me 6 months to get 
back to evaluating hash functions and dHash is a definite winner.

dHash
Like aHash and pHash, dHash is pretty simple to implement and is far more accurate than it has any right to be. As an implementation, 
dHash is nearly identical to aHash but it performs much better. While aHash focuses on average values and pHash evaluates frequency 
patterns, dHash tracks gradients. Here's how the algorithm works, using the same Alyson Hannigan image as last time:

Reduce size. The fastest way to remove high frequencies and detail is to shrink the image. In this case, shrink it to 9x8 so that 
there are 72 total pixels. (I'll get to the "why" for the odd 9x8 size in a moment.) By ignoring the size and aspect ratio, this 
hash will match any similar picture regardless of how it is stretched.
Reduce color. Convert the image to a grayscale picture. This changes the hash from 72 pixels to a total of 72 colors. 
(For optimal performance, either reduce color before scaling or perform the scaling and color reduction at the same time.)
Compute the difference. The dHash algorithm works on the difference between adjacent pixels. This identifies the relative gradient direction. 
In this case, the 9 pixels per row yields 8 differences between adjacent pixels. Eight rows of eight differences becomes 64 bits.
Assign bits. Each bit is simply set based on whether the left pixel is brighter than the right pixel. The order does not matter, 
just as long as you are consistent. (I use a "1" to indicate that P[x] < P[x+1] and set the bits from left to right, top to bottom using big-endian.)
 =  = 3a6c6565498da525
As with aHash, the resulting hash won't change if the image is scaled or the aspect ratio changes. Increasing or decreasing the 
brightness or contrast, or even altering the colors won't dramatically change the hash value. Even complex adjustments like gamma 
corrections and color profiles won't impact the result. And best of all: this is FAST! Seriously -- the slowest part of the algorithm 
is the size reduction step.

The hash values represent the relative change in brightness intensity. To compare two hashes, just count the number of bits that are 
different. (This is the Hamming distance.) A value of 0 indicates the same hash and likely a similar picture. A value greater than 
10 is likely a different image, and a value between 1 and 10 is potentially a variation.

Speed and Accuracy
From FotoForensics, we now have a testbed of over 150,000 images. I have a couple of test images that occur a known number of times. 
For example, one picture (needle) appears exactly once in the 150,000 image repository (haystack). Another picture occurs twice. 
A third test picture currently occurs 32 times.

I've used aHash, pHash, and dHash to search for the various needles in the haystack. For comparisons, I did not pre-cache any of 
the repository hash values. I also consider a cutoff value of 10 to denote a match or a miss. (If the haystack image differs from 
the needle image by more than 10 bits, then it is assumed to not match.) Here's the results so far:

No hash. This is a baseline for comparison. It loads each image into memory, and then unloads the image. This tells me how much 
time is spent just on the file access and loading. (And all images are located on an NFS-mounted directory -- so this includes 
network transfer times.) The total time is 16 minutes. Without any image comparisons, there is a minimum of 16 minutes needed just to load each image.

No hash, Scale. Every one of these hash algorithms begins by scaling the image smaller. Small pictures scale very quickly, but 
large pictures can take 10 seconds or more. Just loading and scaling the 150,000 images takes 3.75 hours. (I really need to look 
into possible methods for optimizing my bilinear scaling algorithm.)

aHash. This algorithm takes about 3.75 hours to run. In other words, it takes more time to load and scale the image than to 
run the algorithm. Unfortunately, aHash generates a huge number of false positives. It matched all of the expected images, but 
also matched about 10x more false-positives. For example, the test picture that should have matched 32 times actually matched over 
400 images. Worse: some of the misses had a difference of less than 2 bits. In general, aHash is fast but not very accurate.

pHash. This algorithm definitely performed the best with regards to accuracy. No false positives, no false negatives, and every 
match has a score of 2 or less. I'm sure that a bigger data set (or alternate needle image) will generate false matches, but the 
number of misses will likely be substantially less than aHash.

The problem with pHash is the performance. It took over 7 hours to complete. This is because the DCT computation uses lots of 
operations including cosine and sine. If I pre-compute the DCT constants, then this will drop 1-2 hours from the overall runtime. 
But applying the DCT coefficients still takes time. pHash is accurate, but not very fast.

dHash. Absolutely amazing... Very few false positives. For example, the image with two known matches ended up matching 6 
pictures total (4 false positives). The scores were: 10, 0, 8, 10, 0, and 10. The two zeros were the correct matches; all 
of the false-positive matches had higher scores. As speed goes, dHash is as fast as aHash. Well, technically it is faster 
since it doesn't need to compute the mean color value. The dHash algorithm has all the speed of aHash with very few false-positives.
Algorithm Variations
I have tried a few variations of the dHash algorithm. For example, David's initial proposal used an 8x8 image and wrapped 
the last comparison (computing the pixel difference between P[0] and P[7] for the 8th comparison). This actually performs a 
little worse than my 9x8 variation (more false-positives), but only by a little.

Storing values by row or column really doesn't make a difference. Computing both row and column hashes significantly 
reduces the number of false-positives and is comparable to pHash (almost as accurate). So it maintains speed and gains 
accuracy as the cost of requiring 128 bits for the hash.

I've also combined pHash with dHash. Basically, I use the really fast dHash as a fast filter. If dHash matches, then I 
compute the more expensive pHash value. This gives me all the speed of dHash with all the accuracy of pHash.

Finally, I realized that using dHash as a fast filter is good, but I don't need 64-bits for this computation. 
My 16-bit dHash variant uses a 6x4 reduced image. This gives me 20 difference values. Ignoring the four corners yields a 16-bit hash -- 
and has the benefit of ignoring the impact from Instagram-like vignetting (corner darkening). If I have a million different images, 
then I should expect about 15 images per 16-bit dHash. pHash can compare 15 images really quickly. At a billion images, I'm looking 
at about 15,258 image collisions and that still is a relatively small number.

I can even permit my 16-bit dHash to be sloppy; permitting any 1-bit change to match. Any computed 16-bit dHash would yield 17 possible 
dHash values to match. A million images should yield about 260 collisions, and a billion becomes about 260,000 collisions. 
At a billion images, it would be worth storing the 16-bit dHash, 64-bit dHash, and 64-bit pHash values. But a million images? 
I'm still ahead just by pre-computing the 16-bit dHash and 64-bit pHash values.

Applied Comparisons
There are two things we want out of a perceptual hash algorithm: speed and accuracy. By combining dHash with pHash, we get both. 
But even without pHash, dHash is still a significant improvement over aHash and without any notable speed difference.


I'm surprised at the claim that pHash is slow. I reimplemented pHash in about 15 lines of Python + numpy, and it benchmarks at 5534 
hashes per second on my box (a Core i7 laptop), without the resize step (which dHash also needs, of course). I've been using it on 
video, and the video decoding and resize is by far the slowest part of the pipeline.
pHash's DCT is really just an (8x32) (32x32) (32x8) matrix multiplication chain, a median step, and 64 comparisons, 
which is very fast to compute (it could probably be reduced to all 8x8 matrices too without significantly affecting the output).
On your 150k test image set, it should take about 27 seconds, a negligible amount of time compared to the 3.75 hours it took 
to scale the images. I suspect something is terribly sub-optimal about your implementation if it's 430 times slower than my Python one!
I use pHash to cross-match about 2000 frames of "dirty" video (possibly subtitled, downscaled, with compression artifacts) 
against 300k-1m frames of clean HD video. I pHash every frame of both halves (which takes about an hour or two in total - 
mostly the H) and store the hashes - this is a very small and simple bit of code.
Then I compare every one of the 2000 frames against every one of the 300k-1m frames (over a billion comparisons) and 
look for runs of frames with similar hashes (by counting the number of bits that differ and comparing it with two thresholds). 
This takes a few minutes with some optimized Python + Cython code.

With SIMD operations, the double loops could be reduced to a single loop. And with SSE2 or FPGA, it might even be no 
loops if the matrix is small enough to completely fit into the SIMD registers. This would easily become a 200%-400% 
speed improvement for the forward-DCT operation.

openCV v3 has c++ hash implementations for average, perceptual, radial, Marr Hildreth, color moment. 
openCV does not have a difference hash, but it may be added. There are also performance results for invariances on the ukbench data set.











https://github.com/Matthias-Wandel/jhead
https://www.sentex.ca/~mwandel/jhead/
Exif Jpeg header manipulation tool
Things jhead can extract from an Exif jpeg file
Time and date picture was taken
Camera make and model
Integral low-res Exif thumbnail
Shutter speed
Camera F-stop number
Flash used (yes/no)
Distance camera was focused at
Focal length and calculate 35 mm equivalent focal length
Image resolution
GPS info, if stored in image
IPTC header
XMP data
Things jhead can modify in an exif jpeg file
Up-right images according to rotation tag using jpegtran
Set or relative adjust internal exif timestamps
Fix date / time offsets in large batches of images
Re-name or date stamp files according to exif timestamp
Transfer exif headers between images
Replace thumbnails inside Exif headers
Edit jpeg comments (but not the Exif comments)
Delete exif or comment sections from jpeg images
Create new minimal exif header containing date and thumbnail






https://github.com/xiaozhuai/imageinfo
Cross platform super fast single header c++ library to get image size and format without loading/decoding.
Support avif, bmp, cur, dds, gif, hdr (pic), heic (heif), icns, ico, jp2, jpeg (jpg), jpx, ktx, png, psd, qoi, tga, tiff (tif), webp ...
The imageinfo don't get image format by file ext name, but infer by file header bytes and character.
As few I/O times as possible! Read as few bytes as possible!
Simplest Demo
auto imageInfo = getImageInfo<IIFilePathReader>("images/valid/jpg/sample.jpg");
std::cout << "File: " << file << "\n";
std::cout << "  - Error    : " << imageInfo.getErrorMsg() << "\n";
std::cout << "  - Width    : " << imageInfo.getWidth() << "\n";
std::cout << "  - Height   : " << imageInfo.getHeight() << "\n";
std::cout << "  - Format   : " << imageInfo.getFormat() << "\n";
std::cout << "  - Ext      : " << imageInfo.getExt() << "\n";
std::cout << "  - Full Ext : " << imageInfo.getFullExt() << "\n";
std::cout << "  - Mimetype : " << imageInfo.getMimetype() << "\n\n";
You can pass a file path and use IIFilePathReader,

and there are some builtin reader IIFileReader, IIFileStreamReader, IIRawDataReader

FILE *file = fopen("images/valid/jpg/sample.jpg", "rb");
auto imageInfo = getImageInfo<IIFileReader>(file);
fclose(file);
std::ifstream file("images/valid/jpg/sample.jpg", std::ios::in);
auto imageInfo = getImageInfo<IIFileStreamReader>(file);
file.close();
// Suppose we already got data and size
// void *data;
// size_t size;
auto imageInfo = getImageInfo<IIRawDataReader>(IIRawData(data, size));
If you known the file is likely a JPEG, you can provide likely format parameter to improve performance;

auto imageInfo = getImageInfo<IIFilePathReader>("images/valid/jpg/sample.jpg", II_FORMAT_JPEG);
Custom Reader
First, take a look at IIFileReader, all your need to do is define a class and implement size and read method. (not override)

class IIFileReader {
public:
	explicit IIFileReader(FILE *file) : m_file(file) {}

	inline size_t size() {
		if (m_file != nullptr) {
			fseek(m_file, 0, SEEK_END);
			return ftell(m_file);
		} else {
			return 0;
		}
	}

	inline void read(void *buf, off_t offset, size_t size) {
		fseek(m_file, offset, SEEK_SET);
		fread(buf, 1, size, m_file);
	}

private:
	FILE *m_file = nullptr;
};






https://github.com/nokiatech/heif
High Efficiency Image File Format (HEIF)
HEIF is a visual media container format standardized by the Moving Picture Experts Group (MPEG) for storage and sharing of images and image sequences.
It is based on the well-known ISO Base Media File Format (ISOBMFF) standard. HEIF Reader/Writer Engine is an implementation of HEIF standard in order to
demonstrate its powerful features and capabilities.

Please follow this link to access HEIF Web-Site. You can also check the Wiki pages for more information.
https://github.com/nokiatech/heif/wiki

About HEIF and MIAF
HEIF (High Efficiency Image File Format) is a standard developed by the Moving Picture Experts Group (MPEG) for storage and sharing of images and image sequences.
MIAF (Multi-Image Application Format) standard defines a set of additional constraints on HEIF specification, which further enables interoperability.
This site is dedicated to provide technical information, usage examples and files, source code and external links to various information sources related to HEIF and MIAF.
Some key features
Media data is encoded according to the High Efficiency Video Coding (HEVC) standard
Storage based on widely adopted ISO Base Media File Format (ISOBMFF)
Supports efficient storage of image bursts and cinemagraphs
Supports computational photography use cases
Supports both lossy and lossless image data storage
A better and easy way to distribute still images, image collections and related metadata

https://nokiatech.github.io/heif/technical.html
Introduction
High Efficiency Image File Format (HEIF, ISO/IEC 23008-12) specifies the storage of individual images, image sequences and their
metadata into a container file conforming to the ISO Base Media File Format (ISO/IEC 14496-12). HEIF includes the storage
specification of HEVC intra images and HEVC image sequences in which inter prediction is applied in a constrained manner. Use cases supported by HEIF include:
Storage of still images as well as collection of images in a single container file,
Storage of burst photos,
Storage and efficient representation of video animations and cinemagraphs.
Support for simultaneous capture of video and still images, i.e. storing still images and timed image sequences into the same file.
Storage of images derived from other images of the file, either as derived images represented by non-destructive image editing operations or as pre-computed derived images.
Storage of focal and exposure stacks into the same container file.
Support for storing other media, such as audio and timed text, into the same container file with timed image sequences and synchronizing their playback.
As HEVC provides support for various chroma formats and sample fidelities up to lossless coding, the format can serve the whole spectrum
of use cases from today's consumer devices storing images typically at 8 bits per sample to high-end professional devices with sample
fidelity and dynamic range requirements going all the way up to 16 bits per sample.

Computational photography forms a new category of use cases that can benefit from the HEIF file format.
Now a set of related images can be stored in a single file with associated metadata indicating relationships between different pictures.

HEIF specifies a structural format, from which codec-specific image formats can be derived.
HEIF also includes the specification for encapsulating images and image sequences conforming to the High Efficiency Video Coding (HEVC, ISO/IEC 23008-2 | ITU-T Rec. H.265).

In ISOBMFF, a continuous or timed media or metadata stream forms a track, whereas static media or metadata is stored as items.
Consequently, HEIF has the following basic design:

1. Still images are stored as items. Typically image items are independently coded, and do not depend on any other item in their decoding.
If predictively coded image items with coding dependencies are present, this is clearly signalled. Any number of image items can be included in the same file.
2. Image sequences are stored as tracks. An image sequence track can be indicated to be displayed either as a timed sequence or in a non-timed manner,
such as a gallery of images. An image sequence track may be used instead of image items when there is coding dependency between images.
A file may contain both image items and image sequence tracks along with other media. For example, it is possible to create a file,
which includes image items or image sequence tracks conforming to HEIF, and audio and timed text tracks conforming to any derivative format of the ISOBMFF.

Image I. ISOBMFF compliant file high level structure with meta box and items
Files conforming to ISOBMFF consist of a sequence of data structures called boxes, each containing a four-character code (4CC)
indicating the type of the box, the size of the box in terms of bytes, and the payload of the box. Boxes may be nested, i.e. a
box may contain other boxes. ISOBMFF and HEIF specify constraints on the allowed box order and hierarchy.

HEIF Related Brands and MIME Types
Files conforming to HEIF start with a FileTypeBox as defined in the ISOBMFF standard, which contains a list of brands the file complies with.
As the FileTypeBox is located at the start of the file, it provides easily accessible indications of the file contents to media players.
Each brand is identified by its unique four-character code. The specification of a brand can include requirements and constraints for
files of the brand and for file players supporting the brand. A brand included in the FileTypeBox permits a player that supports the requirements of the brand to play the file.

The brands specified in the HEIF standard are presented in Table I. The HEIF standard specifies the 'mif1' and 'msf1' structural brands.
Additionally, HEVC-specific brands are specified as listed in Table I. The dedicated brand names, 'heic' and 'hevc' indicates that HEVC Main Profile is utilized.

Table I. Brands, MIME subtypes, and file extensions for HEIF.
Brand	Coding format	Image or sequence?	MIME Type	MIME subtype	File extension
mif1	Any	image	image	heif	.heif
msf1	Any	sequence	image	heif-sequence	.heif
heic	HEVC (Main or Main Still Picture profile)	image	image	heic	.heic
heix	HEVC (Main 10 or format range extensions profile)	image	image	heic	.heic
hevc	HEVC (Main or Main Still Picture profile)	sequence	image	heic-sequence	.heic
hevx	HEVC (Main 10 or format range extensions profile)	sequence	image	heic-sequence	.heic
Internet media types, also known as MIME (Multipurpose Internet Mail Extensions) types, are used by various applications to
identify the type of a resource or a file. MIME types consist of a media type ('image' in the case of HEIF files), a subtype,
and zero or more optional parameters. For multi-purpose files, the selection of the subtype can be made on the basis of the primary use of the file.

An optional codecs MIME parameter can be present to indicate the used coding formats of the tracks and items present in the file.
The codecs MIME parameter also includes the profile-tier-level value to which an HEVC-coded image item or an image sequence track conforms.
More information about the MIME type registration and optional parameter can be found in Annex D and Annex E of the HEIF standard.

Comparison to other Image Formats
Table IX provides a comparison of the features of HEIF to other selected image formats. It can be observed that HEIF is more extensible and
comprehensive than the other compared file formats. Particularly the possibility to include other media types, the advanced multi-picture features,
and the support for non-destructive editing make HEIF more advanced than the other formats. The rich set of features make HEIF suitable for a
broad range of devices and applications, including for example burst photography.







https://github.com/s-bear/image-hash
Perceptual Image Hashing Utility
This utility produces perceptual hashes of images, which are short (relative to the size of the image) sequences of numbers 
that are similar for similar-looking images. These hashes may be used for finding duplicate or very similar images in a large dataset.

This utility has two hashing methods, a block-rank algorithm and a DCT based algorithm. Both operate on a pre-processed image, 
which is the input image scaled to 128x128 pixels, histogram equalized, and converted to grayscale.

The block-rank algorithm further reduces the image to 20x20 pixels, and folds the four quadrants of the image in to produce a 
mirror-symmetrical 10x10 image. The hash's 64 bits correspond to the central 8x8 pixels of this image. Each is ranked relative to 
its neighbors, and if greater than half the corresponding bit is set, otherwise it is zero.

The DCT based algorithm simply computes the 2D DCT of the pre-processed image, discarding the 0-frequency and all odd-frequency components. 
Each bit of the hash is set if the corresponding DCT coefficient is positive. The bits of the hash are ordered such that including fewer 
DCT terms produces a prefix of the larger hash. That is, the hash produced by imghash -d1 photo.jpg will be a prefix that from imghash -d2 photo.jpg.




https://github.com/libvips/libvips
libvips is a demand-driven, horizontally threaded image processing library. Compared to similar libraries, libvips runs quickly and uses little memory. 
libvips is licensed under the LGPL 2.1+.

It has around 300 operations covering arithmetic, histograms, convolution, morphological operations, frequency filtering, colour, resampling, 
statistics and others. It supports a large range of numeric types, from 8-bit int to 128-bit complex. Images can have any number of bands. 
It supports a good range of image formats, including JPEG, JPEG2000, JPEG-XL, TIFF, PNG, WebP, HEIC, AVIF, FITS, Matlab, OpenEXR, PDF, SVG, HDR, 
PPM / PGM / PFM, CSV, GIF, Analyze, NIfTI, DeepZoom, and OpenSlide. It can also load images via ImageMagick or GraphicsMagick, letting it work with formats like DICOM.




https://github.com/yduf/dhash
Difference Hash computation
Licensed under the GNU General Public License, version 3. (GPLv3)

Based on idea from article / 2
Behavior
try to keep aspect ratio when computing hash
use one of these 3 ratio that fit into 64bits: 8x8, 9x7, 10x6
rotate image by 90 when needed to always fit into these ratios.
use libvips thumbnail for fast reduction
image formats are those supported by libvips
output results a one line json for easier script integration


https://github.com/shuttie/libimghash
Image hashing library for C++. Main features:

Fast. Can hash 60 images/sec on lena.bmp on single core.
Supports a lot of image formats.
Simple. You need only 3 lines of code to hash an image.

A small example showing library usage:

imghash::Source data;
data.loadFile("foo.jpg");
imghash::Hasher *hasher = new imghash::SimpleHasher();
imghash::Hash hash = hasher->hash(data);
std::cout << "hash: " << hash.toHex() << "\n";
delete hasher;





https://github.com/goongong/hashingImage
Deep Supervised Hashing for Fast Image Retrieval,
Version 1.0, Copyright(c) May, 2016
Haomiao Liu, Ruiping Wang, Shiguang Shan, Xilin Chen.

Tips:
We have provided a tool for extracting network activations, you may refer to "CIFAR-10/extract_code.sh" for example usage. 
The example code extracts the real-valued network outputs, and writes them into a binary file. The first 12 floating point 
numbers correspond to the first image, and the second 12 correspond to the second image, and so on.

We have also provided a "HashingImageData" layer which corresponds to the online image pair generation scheme described in our paper. 
This scheme is extremely useful for large scale dataset, where dissimilar pairs outnumber similar pairs. Please refer to "CIFAR-10/train_test.prototxt" for example usage.

Our code was modified to support multi-label images as in NUS-WIDE, please refer to "NUS/train.txt", "NUS/test.txt", "NUS/create_NUS.sh" for example usages.



https://github.com/alangshur/perceptual-dct-hash
Perceptual image hashing on low-level bitmap images using discrete cosine transforms
Software Requirements/Libraries:
C++17
openCV (opencv4)
pkg-config (note: you may have to manually set PKG_CONFIG_PATH)
Homebrew (recommended)
Methodology
Public/private key approach:
O (Original): Original image fed into image validation
P (Private): Private key generated during initial image validation
M (Mapped): Mapped image acts as public key
H (Heuristic): Heuristic to tighten irreversability of image validation (SHA, time, user credentials, NDR image metadata)
O + P + M = H (image validation abstraction)
Mapped detection:
Spread of "tags" -- small bit patterns in pixels around image that can be detected by algorithm and correspond to private key
Hidden algorithm to determine private key from tag pattern
Images indexed in database by private key -- does detected public key entry match the public key on file?
How to encode tags in image?
Requirements: cannot (at all) modify the image appearance
Somehow get from image (that is potentially extremely modified) to the tag
Potential solution:
Given a single image mapping, use propietary information (no parameters) to discern X focal points
This is tricky: we need to assume that people can determine what pixels are changed but they can't be able to replicate the algorithm that determines the focal points
How to find focal points? Heuristic hash tree algorithm:
Start by normalizing pixel values in order to ignore slider changes
Choose random pixel (based on heuristics)
If point passes first tree hash bucket, move to second (else, restart)
Make small jump based on second-level node (if jump is out of bounds, restart)
Repeat from root of tree N more times (where N is a number that doesn't require too much processing time and the result returns around 20 focal points)
If after N repetitions, the final pixel is valid AND it's focal region is in bounds AND doesn't interfere with a previously chosen focal 
region, this will become a focal point (if not, restart) (N ~ 22)
Tree structure:
First level (root): pixel hash (binary buckets) with hidden heuristic
Edge from first to second level: based on hash of direct neighbor pixels (8 x 255R x 255G x 255B buckets) with hidden heuristic, 
move to second-level node corresponding to bucket
Second level: execute small jump from pixel given heuristically by hash bucket and first-level pixel value
Each focal point corresponds to a particular 64-bit hash based on neighboring pixel values (based on the location of focal point, 
it should "point at" a very specific group of tweaked neighboring pixels)
X should be high enough that a single cropping (of a meaningful size) should contains several focal points
Once we have a small cutout of the image, we can determine the same original focal points and their hash
The neighboring pixel group of a focal point can be run through a propetary algorithm to reveal the private key
The focal points should be chosen in a way such that any modification not in the DOFs will still reveal the same focal points and the same private hash
Algorithm normalizes RGB values (R, G, and B individually for each pixel) throughout image which prevents sliders from changing selected focal points
Focal points don't necessarily need to select a single set group of neighbors (could use each pixel in a YxY block)
Defense against photoshop (manual modification of pixel values) and cropping:
Most difficult to the defend against
Only feasible solution (that doesn't require high processing times): increase X (number of focal points) and make smart focal point decisions
Example: a 1000x1000 image can fit around 50 15x15 spaced focal point regions (each contains 225 pixels that can be slightly tweaked to create private key hash)
How to encode private key around a focal point?
Assume we have 15x15 focal point regions: this means we have 225 pixels in each region and 675 variables (one for each RGB)
Given 675 variables (that can be slightly tweaked), how can you encode a 64-bit private key?
Alternative: use 10x10 focal regions to build a 128-bit hash that will point to a bucket in a massive database that points to the card for the original image
This alternative allows us to not modify the original image
All the focal regions (based on their unique hashes) for a specific image will point to the same original photo card
Focal regions hashes should avoid all modifications not invoked by the degree of freedom (DOF)
Probabilistic model that prevents collisions of focal region hashes
Goal: reduce the size of the focal regions so that we can incorporate far more -- this will boost the effectivness of the utility in avoiding photoshop/cropping changes
Comparing images and image conversions
Perceptual hash: ignores color and develop series of weights for Hamming distance
Avoiding lossy compression due to image conversion: the images should be broken down into a universal format before processing regardless of 
whether slight modifications (from lossy copmression conversion) to the individual pixel values have taken place
Repeatedly converting to JPEG converges in amount of compression saved
Potential idea: run JPEG conversion repeatedly to an impractical image quality (that's worse then what anyone will ever use) so that if the 
actual image is converted to JPEG, it will still be run through this convergence algorithm
Do different lossy compression algorithms compress differently? Employ a system of feedback between lossy compression convergence of different compression algorithms
Main idea: we should always end up with roughly the same low-quality image before we run the focal points mapping
Using perceptual image hashing also helps with this: we can calculate Hamming viability of the individual focal regions
Problem: using slightly modified pixel values in jumps for tree algorithm -- can't trust that the converged low-quality pre-mapping image will be 
exactly the same each time (small differences in pixel values)
Solution to above problem: use bucket-based perceptual hashing around focal points to make steps in tree algorithms (slight differences in cluster of 
pixels will still allow us to move to the correct place)
Perceptual Hash Algorithm
Reduce pixel size (depending on original size)
Convert to greyscale or normalize pixel values (convert to 8-bit pixel intensity)
Compute the discrete cosine transform (Fourier-like transformation)
Reduce the DCT (signal energy lies at low frequency in upper left of domain 8 x 8 grid)
Compute the average value (not including the outlier first frequency term)
For each remaining frequency (presumably 64 of them), add a 1 bit to the integer if that frequency is greater than the mean (and a 0 otherwise)
Files
hash/
ihash.h: Defines top-level hash class
phash.h: Defines image-based perceptual hash class and utilities
dcthash.h: Defines DCT perceptual hash class and utilities
bmp.h: Defines class and utilities for converting image files into pixel grid
grid.h: Defines class and utilities for handling raw pixel grids














When I use opencv to extract feature points ,ORB is slower than SIFT.Why? Theoretically,ORB should much faster

*/






/*
MobileNets: Efficient Convolutional Neural Networks for Mobile Vision Applications
Andrew G. Howard, Menglong Zhu, Bo Chen, Dmitry Kalenichenko, Weijun Wang, Tobias Weyand, Marco Andreetto, Hartwig Adam
We present a class of efficient models called MobileNets for mobile and embedded vision applications. 
MobileNets are based on a streamlined architecture that uses depth-wise separable convolutions to build light weight 
deep neural networks. We introduce two simple global hyper-parameters that efficiently trade off between latency and accuracy. 
These hyper-parameters allow the model builder to choose the right sized model for their application based on the constraints of the problem. 
We present extensive experiments on resource and accuracy tradeoffs and show strong performance compared to other popular models on 
ImageNet classification. We then demonstrate the effectiveness of MobileNets across a wide range of applications and use cases 
including object detection, finegrain classification, face attributes and large scale geo-localization.
Subjects:	Computer Vision and Pattern Recognition (cs.CV)
Cite as:	arXiv:1704.04861 [cs.CV]
	(or arXiv:1704.04861v1 [cs.CV] for this version)

https://doi.org/10.48550/arXiv.1704.04861
Focus to learn more

*/

