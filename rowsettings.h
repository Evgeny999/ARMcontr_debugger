#ifndef ROWSETTINGS_H
#define ROWSETTINGS_H

#include <QDialog>

namespace Ui {
    class RowSettings;
}

class RowSettings : public QDialog
{
    Q_OBJECT

public:
    explicit RowSettings(QWidget *parent = 0);
    ~RowSettings();
    Ui::RowSettings *ui;
public slots:
    void closeEvent(QCloseEvent *e);
signals:
    void send_to_cellList();
};

#endif // ROWSETTINGS_H
