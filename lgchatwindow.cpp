#include "lgchatwindow.h"
#include "ui_lgchatwindow.h"
#include <iostream>

LGChatWindow::LGChatWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LGChatWindow)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_QuitOnClose,false);
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(sendMessage()));
}

LGChatWindow::~LGChatWindow()
{
    delete ui;
}

void LGChatWindow::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void LGChatWindow::setUserData(QString name, int number = 0) {
    contactName = name;
    contactNumber = number;
    setWindowTitle("Rozmowa z " + name + " (" + QString::number(number) + ")");
}

int LGChatWindow::getNumber() {
    return contactNumber;
}

QString LGChatWindow::enableEmoticons(QString message) {
    message.replace(":)","<img src=\"emots/smile.png\">");
    message.replace(":(","<img src=\"emots/frown.png\">");
    message.replace(":P","<img src=\"emots/tongue.png\">");
    message.replace(":p","<img src=\"emots/tongue.png\">");
    message.replace(";P","<img src=\"emots/tongue.png\">");
    message.replace(";p","<img src=\"emots/tongue.png\">");
    message.replace(":*","<img src=\"emots/kiss.png\">");
    message.replace(":-*","<img src=\"emots/kiss.png\">");
    message.replace(":D","<img src=\"emots/grin.png\">");
    message.replace(":-D","<img src=\"emots/grin.png\">");
    message.replace("<3","<img src=\"emots/heart.png\">");
    message.replace(";)","<img src=\"emots/wink.png\">");
    message.replace(";-)","<img src=\"emots/wink.png\">");
    message.replace(":o","<img src=\"emots/gasp.png\">");
    message.replace(":O","<img src=\"emots/gasp.png\">");
    return message;
}

void LGChatWindow::sendMessage() {
    QString message = enableEmoticons(ui->plainTextEdit->toPlainText());
    if(!ui->textBrowser->toPlainText().isEmpty()) {
        ui->textBrowser->setHtml(ui->textBrowser->toHtml() + "\n<b><font color=\"blue\">Ja: </font></b>" + message);
    }
    else {
        ui->textBrowser->setHtml("<b><font color=\"blue\">Ja:</font></b> " + message);
    }
    emit msgSent(contactNumber,ui->plainTextEdit->toPlainText().toStdString());
    ui->plainTextEdit->clear();
}

void LGChatWindow::displayMessage(QString message) {
    message = enableEmoticons(message);
    ui->textBrowser->setHtml(ui->textBrowser->toHtml() + "\n<b><font color=\"red\">" + contactName + ":</font></b> " + message);
}

void LGChatWindow::closeEvent(QCloseEvent *a) {
    emit closed(this);
    delete this;
}

QString LGChatWindow::getName() {
    return contactName;
}
