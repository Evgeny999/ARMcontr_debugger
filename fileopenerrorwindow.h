#ifndef FILEOPENERRORWINDOW_H
#define FILEOPENERRORWINDOW_H

#include <QDialog>

namespace Ui {
    class FileOpenErrorWindow;
}

class FileOpenErrorWindow : public QDialog
{
    Q_OBJECT

public:
    explicit FileOpenErrorWindow(QWidget *parent = 0);
    ~FileOpenErrorWindow();
     Ui::FileOpenErrorWindow *ui;
};

#endif // FILEOPENERRORWINDOW_H
