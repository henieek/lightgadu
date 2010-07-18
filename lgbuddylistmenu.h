#ifndef LGBUDDYLISTMENU_H
#define LGBUDDYLISTMENU_H

#include <QMenu>
#include <QList>
#include <QListWidget>

#include <libgadu.h>
#include <iostream>

class LGBuddyListMenu : public QMenu
{
    Q_OBJECT
public:
    LGBuddyListMenu(QWidget *parent = 0);
    ~LGBuddyListMenu();
    void whoWasSelected(QListWidgetItem*);
private:
    QAction* startChat;
    QAction* showInfo;
    QAction* blockContact;
    QAction* removeContact;
    QListWidgetItem* clickedItem;
    void insertStartActions();
private slots:
    void startChatClicked();
    void showInfoClicked();
    void blockContactClicked();
    void removeContactClicked();
signals:
    void startChatSignal(QListWidgetItem*);
    void showInfoSignal(QListWidgetItem*);
    void blockContactSignal(QListWidgetItem*);
    void removeContactSignal(QListWidgetItem*);
public slots:

};

#endif // LGBUDDYLISTMENU_H
