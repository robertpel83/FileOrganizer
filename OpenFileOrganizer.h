#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_OpenFileOrganizer.h"

class OpenFileOrganizer : public QMainWindow
{
    Q_OBJECT

public:
    OpenFileOrganizer(QWidget *parent = nullptr);
    ~OpenFileOrganizer();

private:
    Ui::OpenFileOrganizerClass ui;
};
