#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    QObject::connect(&SSL_socket_, &QAbstractSocket::connected, this, &Widget::doConnected);
    QObject::connect(&SSL_socket_, &QAbstractSocket::disconnected, this, &Widget::doDisconnected);
    QObject::connect(&SSL_socket_, &QAbstractSocket::readyRead, this, &Widget::doReadyRead);
    QObject::connect(&SSL_socket_, &QSslSocket::stateChanged, this, &Widget::updateUI);

    QObject::connect(&TCP_socket_, &QAbstractSocket::connected, this, &Widget::doConnected);
    QObject::connect(&TCP_socket_, &QAbstractSocket::disconnected, this, &Widget::doDisconnected);
    QObject::connect(&TCP_socket_, &QAbstractSocket::readyRead, this, &Widget::doReadyRead);
    QObject::connect(&TCP_socket_, &QTcpSocket::stateChanged, this, &Widget::updateUI);

}

Widget::~Widget()
{
    delete ui;
}

void Widget::doConnected()
{
    QString msg;

    if(ui->cbSSL->isChecked()){
        msg = "Connected to " + SSL_socket_.peerAddress().toString() + "\r\n";
    }
    else {
        msg = "Connected to " + TCP_socket_.peerAddress().toString() + "\r\n";
    }

    ui->pteMessage->insertPlainText(msg);
}

void Widget::doDisconnected()
{
    QString msg;

    if(ui->cbSSL->isChecked()){
        msg = "Disconnected to " + SSL_socket_.peerAddress().toString() + "\r\n";
    }
    else {
        msg = "Disconnected to " + TCP_socket_.peerAddress().toString() + "\r\n";
    }

    ui->pteMessage->insertPlainText(msg);
}

void Widget::doReadyRead()
{
    QByteArray ba;

    if(ui->cbSSL->isChecked()){
        ba = SSL_socket_.readAll();
    }
    else {
        ba = TCP_socket_.readAll();
    }

    ui->pteMessage->insertPlainText(ba);
}

void Widget::updateUI(QAbstractSocket::SocketState state)
{
    ui->pbDisconnect->setEnabled(state == QAbstractSocket::ConnectedState);
    ui->pbConnect->setDisabled(state == QAbstractSocket::ConnectedState);
    ui->pbSend->setEnabled(state == QAbstractSocket::ConnectedState);
}

void Widget::on_pbConnect_clicked()
{
    if(ui->cbSSL->isChecked()){
        SSL_socket_.connectToHostEncrypted(ui->leHost->text(), ui->lePort->text().toUShort());
    }
    else{
        TCP_socket_.connectToHost(ui->leHost->text(), ui->lePort->text().toUShort());
    }
}


void Widget::on_pbDisconnect_clicked()
{
    TCP_socket_.close();
    SSL_socket_.close();
}


void Widget::on_pbSend_clicked()
{
    if(ui->cbSSL->isChecked()){
        SSL_socket_.write(ui->pteSend->toPlainText().toUtf8());
    }
    else{
        TCP_socket_.write(ui->pteSend->toPlainText().toUtf8());
    }
}


void Widget::on_pbClear_clicked()
{
    ui->pteMessage->clear();
}
