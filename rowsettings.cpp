#include "rowsettings.h"
#include "ui_rowsettings.h"

RowSettings::RowSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RowSettings)
{
    ui->setupUi(this);
}

RowSettings::~RowSettings()
{
    delete ui;
}
