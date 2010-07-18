#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QListWidgetItem>
#include <QSystemTrayIcon>
#include <QVector>
#include <fstream>

#include "lgcontactlist.h"
#include "lgchatwindow.h"
#include "lgconnectionthread.h"
#include "lgaddcontactwindow.h"
#include "lgbuddylistmenu.h"
#include "lgsettingswindow.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void changeEvent(QEvent*);
    void closeEvent(QCloseEvent*);
private:
    bool isGoingToBeClosed;
    int avail;
    int away;
    int all;
    Ui::MainWindow *ui;
    LGConnectionThread *connectionThread;
    QVector <QString> openedBuddies;
    QVector <std::string> allBuddiesNames;
    QVector <int> allBuddiesNumbers;
    QVector <LGChatWindow*> openedWindows;
    QVector <QListWidgetItem*> listItems;
    QVector <std::string> listItemsNames;
    QVector <int> availNumbers;
    QVector <int> availStatuses;
    QVector <QString> availDescs;
    LGContactList *contactListDB;
    QSystemTrayIcon *trayIcon;
    QPoint lastPosition;
    LGBuddyListMenu *buddyListMenu;
    LGSettingsWindow *settingsWindow;
private slots:
    int getNumberOfContact(QString);
    std::string getNameOfContact(int);
    void updateList(intVector,intVector,qstringVector);
    void readContactList();
    void startChat(QListWidgetItem*);
    void changeStatus(uin_t,int,char*);
    void getMessage(uin_t,QString);
    void removeChatWindow(LGChatWindow*);
    void addContact();
    void refreshList();
    LGChatWindow * findProperWindow(int);
    void changeTrayIcon(int);
    void trayClicked(QSystemTrayIcon::ActivationReason);
    void buddyMenuRequested(const QPoint&);
    void closeApp();
    void removeContact(QListWidgetItem*);
    void setLoginData(int,const char*);
};

#endif // MAINWINDOW_H
