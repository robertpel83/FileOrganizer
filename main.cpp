

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




#include "OpenFileOrganizer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{

    //Code Page Identifiers
    const char CP_UTF_16LE[] = ".1200";
    setlocale(LC_ALL, CP_UTF_16LE);
    //Prepare for unicode output 
    int setmodestatus = _setmode(_fileno(stdout), _O_U16TEXT);

    QApplication a(argc, argv);
    OpenFileOrganizer w;
    w.show();
    return a.exec();
}
