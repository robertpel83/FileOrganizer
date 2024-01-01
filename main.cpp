#include "OpenFileOrganizer.h"



int main(int argc, char* argv[])
{
    //Code Page Identifiers
    //Unicode UTF-16, little endian byte order (BMP of ISO 10646)
    const char CP_UTF_16LE[] = ".1200";
    setlocale(LC_ALL, CP_UTF_16LE);
    //Prepare for unicode output 
    int setmodestatus = _setmode(_fileno(stdout), _O_U16TEXT);

    //might need for linux?
    //setlocale(LC_ALL, "en.UTF-8")

    QApplication a(argc, argv);
    OpenFileOrganizer w;
    w.show();

    qInstallMessageHandler(OpenFileOrganizer::QMessageOutput);

    return a.exec();

}
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
//	PSTR lpCmdLine, int nCmdShow)
////int main(int argc, char* argv[])
//{
//	ok();
//}