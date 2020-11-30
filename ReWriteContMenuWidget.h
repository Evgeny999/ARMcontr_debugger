#ifndef REWRITECONTMENUWIDGET_H
#define REWRITECONTMENUWIDGET_H
#include <QWidgetAction>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QDebug>
#include <QApplication>
#include <QClipboard>

class ReWriteContMenuWidget : public QWidgetAction{

    Q_OBJECT
    public :
            ReWriteContMenuWidget():
    QWidgetAction(NULL)
            {
                QWidget* pWidget = new QWidget(NULL);
                QHBoxLayout* pLayout = new QHBoxLayout();
                CurrenrValueLine = new QLineEdit(NULL);
                SaveToBufferText = new QTextEdit(NULL);
                ChangeButton =new QPushButton(tr("OK"),NULL);
                AddressLabel=new QLabel(NULL);
                AddressLabel->setFixedWidth(AddressLabel->fontMetrics().width('0')*15);
                SaveToBufferButton = new QPushButton(tr("SaveToBuf"),NULL);
                connect(SaveToBufferButton,SIGNAL(clicked()),this,SLOT(SaveToBuffer()));
                UserCostomNameLabel = new QLabel(NULL);
                pLayout->addWidget(UserCostomNameLabel);
                pLayout->addWidget(AddressLabel);
                pLayout->addWidget(CurrenrValueLine);
                pLayout->addWidget(ChangeButton);
                pLayout->addWidget(SaveToBufferButton);
                pWidget->setLayout(pLayout);
                setDefaultWidget(pWidget);
            }
     public:
    QLineEdit* CurrenrValueLine;
    QPushButton* ChangeButton;
    QLabel *AddressLabel;
    QLabel *UserCostomNameLabel;
    QPushButton *SaveToBufferButton;
    QString UserCustomName;
    QTextEdit *SaveToBufferText;
    int ChangingAddress;

    void SetParam()
    {
       CurrenrValueLine->clear();
       UserCostomNameLabel->setText(UserCustomName);
       AddressLabel->setText(QString().sprintf("0x%08X",ChangingAddress));
    }
    void SetAddress(int ClickedAddress)
    {
        this->ChangingAddress=ClickedAddress;
    }
    void SetCustomName(QString UserCustomName)
    {
        this->UserCustomName.clear();
        this->UserCustomName=UserCustomName;
    }
public slots:
    void SaveToBuffer()
    {
        QClipboard *cb = QApplication::clipboard();
        cb->clear();
        cb->setText(SaveToBufferText->text());
    }
};

#endif // REWRITECONTMENUWIDGET_H
