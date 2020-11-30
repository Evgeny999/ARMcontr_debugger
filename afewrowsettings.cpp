#include "afewrowsettings.h"
#include "ui_afewrowsettings.h"

AFewRowSettings::AFewRowSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AFewRowSettings)
{
    ui->setupUi(this);
}

AFewRowSettings::~AFewRowSettings()
{
    delete ui;
}
