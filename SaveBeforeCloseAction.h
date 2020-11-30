#ifndef SAVEBEFORECLOSEACTION_H
#define SAVEBEFORECLOSEACTION_H

#include <QWidgetAction>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QRadioButton>


class SaveBeforeCloseAction : public QWidgetAction{

    public :
            SaveBeforeCloseAction():
    QWidgetAction(NULL)
            {
                YesButton = new QRadioButton(NULL);
                YesButton->setText("Yes");
                NoButton = new QRadioButton(NULL);
                NoButton->setText("No");
                QHBoxLayout *lay1 = new QHBoxLayout();
                QHBoxLayout *lay2 = new QHBoxLayout();
                lay1->addWidget(YesButton);
                lay2->addWidget(NoButton);

                QWidget* pWidget = new QWidget(NULL);
                QVBoxLayout* pLayout = new QVBoxLayout();
                pLayout->addLayout(lay1);
                pLayout->addLayout(lay2);
                pWidget->setLayout(pLayout);
                setDefaultWidget(pWidget);
            }
    public:
    QRadioButton *YesButton;
    QRadioButton *NoButton;

};
#endif // SAVEBEFORECLOSEACTION_H
