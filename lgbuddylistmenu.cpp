#include "lgbuddylistmenu.h"

LGBuddyListMenu::LGBuddyListMenu(QWidget *parent) :
    QMenu(parent)
{
    this->startChat = new QAction(tr("Rozpocznij rozmowe"),this);
    this->showInfo = new QAction(tr("Dane profilu"),this);
    this->blockContact = new QAction(tr("Zablokuj"),this);
    this->removeContact = new QAction(tr("Usun z listy"),this);
    insertStartActions();
    connect(startChat,SIGNAL(triggered()),this,SLOT(startChatClicked()));
    connect(showInfo,SIGNAL(triggered()),this,SLOT(showInfoClicked()));
    connect(blockContact,SIGNAL(triggered()),this,SLOT(blockContactClicked()));
    connect(removeContact,SIGNAL(triggered()),this,SLOT(removeContactClicked()));
}

LGBuddyListMenu::~LGBuddyListMenu() {
    delete startChat;
    delete showInfo;
    delete blockContact;
    delete removeContact;
}

void LGBuddyListMenu::insertStartActions() { // dodajemy poczatkowe akcje.

    this->addAction(startChat);
    this->addAction(showInfo);
    this->addAction(blockContact);
    this->addAction(removeContact);
}

void LGBuddyListMenu::whoWasSelected(QListWidgetItem *item) {
    clickedItem = item;
}

void LGBuddyListMenu::startChatClicked() {
    emit startChatSignal(clickedItem);
}

void LGBuddyListMenu::showInfoClicked() {
    emit showInfoSignal(clickedItem);
}

void LGBuddyListMenu::blockContactClicked() {
    emit blockContactSignal(clickedItem);
}

void LGBuddyListMenu::removeContactClicked() {
    emit removeContactSignal(clickedItem);
}
