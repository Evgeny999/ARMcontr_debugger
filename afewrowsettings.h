#ifndef AFEWROWSETTINGS_H
#define AFEWROWSETTINGS_H

#include <QDialog>
#include <QTableWidgetItem>

namespace Ui {
    class AFewRowSettings;
}

class AFewRowSettings : public QDialog
{
    Q_OBJECT

public:
    explicit AFewRowSettings(QWidget *parent = 0);
    ~AFewRowSettings();
    Ui::AFewRowSettings *ui;
    QList<QTableWidgetItem*> RowsInfoList;
};

#endif // AFEWROWSETTINGS_H
