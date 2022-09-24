#include "OpenFileOrganizer.h"

void go();

OpenFileOrganizer::OpenFileOrganizer(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    //QDebugStream qout(std::wcout, ui.scanDirsPlainTextEdit);
    QDebugStream qout(std::wcout, ui.consoleOutputPlainTextEdit);

    std::wcout << L"Hello this is a test" << std::endl;
    std::wcout << L"Hello this is a test" << std::endl;
    std::wcout << L"Hello this is a test" << std::endl;
    std::wcout << L"Hello this is a test" << std::endl;
    std::wcout << L"Hello this is a test" << std::endl;


    // Connect button signal to appropriate slot
    connect(ui.startPushButton, &QPushButton::released, this, &OpenFileOrganizer::handleButton);

}

OpenFileOrganizer::~OpenFileOrganizer()
{}




void OpenFileOrganizer::handleButton()
{
    go();
}