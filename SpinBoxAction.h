#ifndef SPINBOXACTION_H
#define SPINBOXACTION_H
#include <QWidgetAction>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>

class PCStringAction : public QWidgetAction{

    public :
            PCStringAction():
    QWidgetAction(NULL)
            {
                QWidget* pWidget = new QWidget(NULL);
                QHBoxLayout* pLayout = new QHBoxLayout();
                pLabel = new QLineEdit(NULL);
                butt =new QPushButton(tr("SetPc"),NULL);
                ResumeHultButton = new QPushButton(tr("Start/Stop"));
                pLayout->addWidget(ResumeHultButton);
                pLayout->addWidget(pLabel);
                pLayout->addWidget(butt);
                pWidget->setLayout(pLayout);
                setDefaultWidget(pWidget);
            }
     public:
    QLineEdit* pLabel;
    QPushButton* butt;
    QPushButton* ResumeHultButton;
    bool HaltResumeControlVar;
};

#endif // SPINBOXACTION_H
