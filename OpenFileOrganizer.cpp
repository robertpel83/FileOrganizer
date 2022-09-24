#include "OpenFileOrganizer.h"

void go();

OpenFileOrganizer::OpenFileOrganizer(QWidget *parent)
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


    // Connect button signal to appropriate slot
    connect(ui.startPushButton, &QPushButton::released, this, &OpenFileOrganizer::handleButton);
    connect(ui.startPushButton, &QPushButton::clicked, this, &OpenFileOrganizer::handleButton);

}

OpenFileOrganizer::~OpenFileOrganizer()
{}



void OpenFileOrganizer::handleButton()
{

   


    QThread* workerThread = new QThread;
    ThreadWorker* worker = new ThreadWorker(ui.consoleOutputPlainTextEdit);
    
    worker->moveToThread(workerThread);
    connect(workerThread, &QThread::started, worker, &ThreadWorker::doWork);
    //connect(worker, &QThread::finished, workerThread, &ThreadWorker::quit);
    //connect(worker, &QThread::finished, worker, &ThreadWorker::deleteLater);
    //connect(workerThread, &QThread::finished, workerThread, &ThreadWorker::deleteLater);
    workerThread->start();

}