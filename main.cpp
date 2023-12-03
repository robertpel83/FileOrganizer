#include "OpenFileOrganizer.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
    //Code Page Identifiers
    const char CP_UTF_16LE[] = ".1200";
    setlocale(LC_ALL, CP_UTF_16LE);
    //Prepare for unicode output 
    int setmodestatus = _setmode(_fileno(stdout), _O_U16TEXT);

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