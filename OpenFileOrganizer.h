#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_OpenFileOrganizer.h"


#include "stdio.h"
#include "iostream"

#include <iostream>
#include <streambuf>
#include <string>
#include <QPlainTextEdit>

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
            pos = m_string.find(L"\n");
            if (pos != std::wstring::npos)
            {
                std::wstring tmp(m_string.begin(), m_string.begin() + pos);

                QString qs;
                qs += tmp;

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




class OpenFileOrganizer : public QMainWindow
{
    Q_OBJECT

public:
    OpenFileOrganizer(QWidget *parent = nullptr);
    ~OpenFileOrganizer();

private:
    Ui::OpenFileOrganizerClass ui;

private slots:
    void handleButton();

};
