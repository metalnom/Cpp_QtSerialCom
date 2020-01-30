#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    connect(ui->pb_connect, SIGNAL(pressed()), this, SLOT(connectButton()));
    connect(ui->pb_exit, SIGNAL(pressed()), this, SLOT(exitButton()));

    m_serialSettings.baudRate = 9600;
    m_serialSettings.dataBits = QSerialPort::Data8;
    m_serialSettings.parity = QSerialPort::NoParity;
    m_serialSettings.stopBits = QSerialPort::OneStop;
    m_serialSettings.flowControl = QSerialPort::NoFlowControl;

    const auto infos = QSerialPortInfo::availablePorts();

    for(const QSerialPortInfo &info: infos) {
        ui->cb_device->addItem(info.portName());
    }

    m_serial = new QSerialPort(this);
    connect(m_serial, SIGNAL(error(QSerialPort::SerialPortError)),
            this, SLOT(error(QSerialPort::SerialPortError)));
    connect(m_serial, SIGNAL(readyRead()), this, SLOT(readData()));
}

void Widget::connectButton() {
    if(m_serial->isOpen()) {
        return;
    }

    QString devName = ui->cb_device->currentText().trimmed();
    m_serialSettings.portname = devName;
    m_serial->setPortName(m_serialSettings.portname);
    m_serial->setBaudRate(m_serialSettings.baudRate);
    m_serial->setDataBits(m_serialSettings.dataBits);
    m_serial->setParity(m_serialSettings.parity);
    m_serial->setStopBits(m_serialSettings.stopBits);
    m_serial->setFlowControl(m_serialSettings.flowControl);

    if(!m_serial->open(QIODevice::ReadWrite)) {
        ui->te_rcvmsg->insertPlainText("error message: ");
        ui->te_rcvmsg->insertPlainText(m_serial->errorString());
//        qDebug() << "error message: " << m_serial->errorString();
    }
    ui->pb_connect->setText("CONNECTED");
}

void Widget::exitButton() {
    Widget::close();
}

void Widget::error(QSerialPort::SerialPortError err) {
    if(err == QSerialPort::ResourceError) {
        ui->te_rcvmsg->insertPlainText("Critical Error: ");
        ui->te_rcvmsg->insertPlainText(m_serial->errorString());
//        qDebug() << "Critical Error: " << m_serial->errorString();
        m_serial->close();
    }
}

void Widget::readData() {
    const QString data = m_serial->readAll();
    ui->te_rcvmsg->insertPlainText(data);
    ui->te_rcvmsg->moveCursor(QTextCursor::End);
}

Widget::~Widget()
{
    delete ui;
}

