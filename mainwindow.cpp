#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButton, SIGNAL( clicked() ),this, SLOT (sendPressed()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sendPressed()
{
    // change the text
    QString ipaddress = ui->lineEdit->text();
    if (ipaddress == "")
    {
        stat("IP address missing.");
        return;
    }
    QString portno = ui->lineEdit_2->text();
    if (portno == "")
    {
        stat("Port missing.");
        return;
    }

    const char* hostname=ipaddress.toStdString().c_str(); /* localhost */
    const char* portname=portno.toStdString().c_str();
    struct addrinfo hints;
    memset(&hints,0,sizeof(hints));
    hints.ai_family=AF_UNSPEC;
    hints.ai_socktype=SOCK_DGRAM;
    hints.ai_protocol=0;
    hints.ai_flags=AI_ADDRCONFIG;
    struct addrinfo* res=0;
    int err=getaddrinfo(hostname,portname,&hints,&res);
    if (err!=0) {
        stat("Failed to resolve remote socket address.");
        return;
    }
    int fd=socket(res->ai_family,res->ai_socktype,res->ai_protocol);
    if (fd==-1) {
        stat("Socket error."); return;
    }

    const char* content = ui->plainTextEdit->toPlainText().toStdString().c_str();
    if (sendto(fd,content,sizeof(content),0,
        res->ai_addr,res->ai_addrlen)==-1) {
        stat("Error sending packet."); return;
    }

    ui->statusBar->showMessage("Done!", 5000);
}

void MainWindow::stat(const char* str)
{
    ui->statusBar->showMessage(str);
}
