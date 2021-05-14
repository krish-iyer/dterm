/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>

#include "window.h"
#include <iostream>
#include <unistd.h>
#include <functional>


//! [0]
Window::Window()
{
    btn_flag = new bool;
    btn_flag = false;
    QGroupBox *echoGroup = new QGroupBox(tr("Serial"));

    QLabel *echoLabel = new QLabel(tr(""));
    QComboBox *echoComboBox = new QComboBox;
    echoComboBox->addItem(tr("9600"));
    echoComboBox->addItem(tr("115200"));
    echoComboBox->addItem(tr("250000"));
    echoComboBox->addItem(tr("100000"));

    echoLineEdit = new QLineEdit;
    echoLineEdit->setPlaceholderText("/dev/tty*");
    echoLineEdit->setFocus();
//! [0]

    serial_console = new QPlainTextEdit;
    serial_console->setPlainText("");
    serial_console->setReadOnly(true);

    start_btn = new QPushButton("Start/ Stop", this);

//! [5]
    connect(echoComboBox, SIGNAL(activated(int)),
            this, SLOT(echoChanged(int)));

    connect(start_btn, &QPushButton::released, this, &Window::h_startBtn);

    connect(this, SIGNAL(rece_sig(QString)), this, SLOT(slot_changed(QString)));

//! [6]
    QGridLayout *echoLayout = new QGridLayout;
    echoLayout->addWidget(echoLabel, 0, 0);
    echoLayout->addWidget(echoComboBox, 0, 2);
    echoLayout->addWidget(echoLineEdit, 0, 1);
    echoLayout->addWidget(start_btn, 0,3);
    echoLayout->addWidget(serial_console,4,0,10,10);
    echoGroup->setLayout(echoLayout);
//! [6]
//!
//! [8]
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(echoGroup, 0, 0);
    setLayout(layout);

    setWindowTitle(tr("DTerm"));

}
//! [8]

//! [9]


void Window::echoChanged(int index)
{
    switch (index) {
    case 0:
        baud = 9600;
        break;
    case 1:
        baud = 115200;
        break;
    case 2:
        baud = 250000;
        break;
    case 3:
        baud = 100000;
    }
}
//! [9]

void Window::h_startBtn()
{
    if(btn_flag == false){
        serial = new Serial;
        btn_flag = true;
        std::cout<<"false"<<std::endl;
        QString qs = echoLineEdit->text();
        serial->open(qs.toUtf8().constData(),baud);
        serial->set_callback(std::bind(&Window::rece, this, std::placeholders::_1,std::placeholders::_2 ));
        thread = new QThread;
        thread = QThread::create([this]{serial->async_read_until(true);while(1){serial->io.run_one();}});
        thread->start();

    }
    else{
        std::cout<<"true"<<std::endl;
        thread->terminate();
        serial->close();
        btn_flag = false;
        delete serial;
        delete thread;
    }
}


void Window::rece(const std::string data, const size_t size){
    emit rece_sig(QString::fromUtf8(data.c_str()));
}

void Window::slot_changed(QString data){
    serial_console->moveCursor(QTextCursor::End);
    serial_console->insertPlainText(data);
}

