#include "fileopenerrorwindow.h"
#include "ui_fileopenerrorwindow.h"

FileOpenErrorWindow::FileOpenErrorWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileOpenErrorWindow)
{
    ui->setupUi(this);
}

FileOpenErrorWindow::~FileOpenErrorWindow()
{
    delete ui;
}
