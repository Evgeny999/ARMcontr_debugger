#include "cellvaluechangewindow.h"
#include "ui_cellvaluechangewindow.h"

CellValueChangeWindow::CellValueChangeWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CellValueChangeWindow)
{
    ui->setupUi(this);
}

CellValueChangeWindow::~CellValueChangeWindow()
{
    delete ui;
}
