#ifndef LGCHATWINDOW_H
#define LGCHATWINDOW_H

#include <QDialog>
#include <QCloseEvent>

#include <libgadu.h>

namespace Ui {
    class LGChatWindow;
}

class LGChatWindow : public QDialog {
    Q_OBJECT
public:
    LGChatWindow(QWidget *parent = 0);
    ~LGChatWindow();
    void setUserData(QString,int);
    int getNumber();
    QString getName();
    void displayMessage(QString);
protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *);

private:
    Ui::LGChatWindow *ui;
    QString contactName;
    int contactNumber;
    QString enableEmoticons(QString);
private slots:
    void sendMessage();
signals:
    void msgSent(uin_t,std::string);
    void closed(LGChatWindow*);
};

#endif // LGCHATWINDOW_H
