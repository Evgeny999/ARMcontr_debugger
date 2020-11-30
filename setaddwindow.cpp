#include "setaddwindow.h"
#include "ui_setaddwindow.h"
#include "celllist1.h"

setAddWindow::setAddWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::setAddWindow)
{
    ui->setupUi(this);
   //ui->lineEdit->setInputMask("HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH");
  // ui->lineEdit->setValidator(new QRegExpValidator(QRegExp("[A-Fa-f1-9]{6}"),this));
}

setAddWindow::~setAddWindow()
{
    delete ui;
}


