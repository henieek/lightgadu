#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qRegisterMetaType<intVector>("intVector");
    qRegisterMetaType<qstringVector>("qstringVector");
    qRegisterMetaType<uin_t>("uin_t");
    isGoingToBeClosed=0;
    avail=0;
    away=0;
    all=0;
    ui->setupUi(this);
    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    contactListDB = new LGContactList();
    connectionThread = new LGConnectionThread;
    trayIcon = new QSystemTrayIcon();
    buddyListMenu = new LGBuddyListMenu(this);
    settingsWindow = new LGSettingsWindow(this);
    trayIcon->setIcon(QIcon("icons/offline.png"));
    trayIcon->setVisible(true);
    connect(settingsWindow,SIGNAL(done(int,const char*)),this,SLOT(setLoginData(int, const char*)));
    // rozpoczynamy czat z wybranym przez siebie kontaktem
    connect(ui->listWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(startChat(QListWidgetItem*)));
    // odbieramy dane poczatkowe o naszej liscie kontaktow z serwera gg
    connect(connectionThread,SIGNAL(notify(intVector,intVector,qstringVector)),this,SLOT(updateList(intVector,intVector,qstringVector)));
    // ktos z naszej listy zmienil status
    connect(connectionThread,SIGNAL(statusChanged(uin_t,int,char*)),this,SLOT(changeStatus(uin_t,int,char*)));
    // dostajemy nowa wiadomosc
    connect(connectionThread,SIGNAL(newMessage(uin_t,QString)),this,SLOT(getMessage(uin_t,QString)));
    // zmiana naszego statusu, po zmianie w QComboBox
    connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),connectionThread,SLOT(changeStatus(int)));
    // zmiana ikony traya na ta z aktualnego statusu
    connect(connectionThread,SIGNAL(changeStatusTrayIcon(int)),this,SLOT(changeTrayIcon(int)));
    // Uruchamiany okno ustawieñ
    connect(ui->actionUstawienia,SIGNAL(triggered()),settingsWindow,SLOT(show()));
    // zaczynamy proces dodawania kontaktu. sygnal emitowany po wybraniu "Dodaj Kontakt" z menu
    connect(ui->actionDodaj_kontakt,SIGNAL(triggered()),this,SLOT(addContact()));
    // odswiezamy nasz QListWidget, aby wyswietlal nowy kontakt. odswiezamy tez niezbedne wektory przechowujace dane kontaktow
    connect(ui->actionZako_cz,SIGNAL(triggered()),this,SLOT(closeApp()));
    // chcemy zamknac aplikacje z poziomu menu "Plik"
    connect(contactListDB,SIGNAL(contactAdded()),this,SLOT(refreshList()));
    connect(contactListDB,SIGNAL(contactRemoved()),this,SLOT(refreshList()));
    // skoro usunelismy kontakt z bazy, musimy odswiezyc nasza liste kontaktow.
    // musimy powiadomic serwer gg, ze dodalismy nowy kontakt
    connect(contactListDB,SIGNAL(ggAddNotify(uin_t)),connectionThread,SLOT(addNotify(uin_t)));
    // powiadamiamy serwer gg, ze usunelismy kontakt.
    connect(contactListDB,SIGNAL(ggRemoveNotify(uin_t)),connectionThread,SLOT(removeNotify(uin_t)));
    // wyswietlamy popup menu dla danego kontaktu
    connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(trayClicked(QSystemTrayIcon::ActivationReason)));
    // kliknieta zostala ikonka traja
    connect(ui->listWidget,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(buddyMenuRequested(const QPoint&)));
    // kliknelismy prawym przyciskiem na dowolny kontakt z listy. wyswietlamy menu.
    connect(buddyListMenu,SIGNAL(startChatSignal(QListWidgetItem*)),this,SLOT(startChat(QListWidgetItem*)));
    // rozpoczynamy czat po wybraniu "Rozpocznij rozmowe" z menu kontekstowego.
    connect(buddyListMenu,SIGNAL(removeContactSignal(QListWidgetItem*)),this,SLOT(removeContact(QListWidgetItem*)));
    setWindowTitle("lightgadu");
    readContactList();
    connectionThread->setContactList(allBuddiesNumbers);
}

MainWindow::~MainWindow()
{
    delete contactListDB;
    delete connectionThread;
    delete settingsWindow;
    delete buddyListMenu;
    delete ui;
}


void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::readContactList() {
    allBuddiesNames.clear();
    allBuddiesNumbers.clear();
    allBuddiesNames = contactListDB->getNames();
    allBuddiesNumbers = contactListDB->getNumbers();
}

void MainWindow::updateList(QVector<int>numbers, QVector<int>statuses, QVector<QString>descs) {
    avail = 0;
    away = 0;
    all = 0;
    ui->listWidget->clear();
    if(availNumbers!= numbers) availNumbers = numbers;
    if(availStatuses != statuses) availStatuses = statuses;
    if(availDescs != descs) availDescs = descs;
    int vectorSize = (allBuddiesNames.size()-1), currentItemStatus;
    for(int i=0;i<vectorSize;i++) {
        QListWidgetItem *item = new QListWidgetItem();
        item->setText(QString::fromStdString(allBuddiesNames[i]));
        int isOnline  = availNumbers.indexOf(allBuddiesNumbers[i]);
        if(isOnline == -1) {
            item->setIcon(QIcon("icons/offline.png"));
            currentItemStatus=0;
            ui->listWidget->insertItem(i,item);
        }
        else {
            if(availStatuses[isOnline] == GG_STATUS_AVAIL || availStatuses[isOnline] == GG_STATUS_AVAIL_DESCR) {
                item->setToolTip(availDescs[isOnline]);
                item->setIcon(QIcon("icons/available.png"));
                ui->listWidget->insertItem(avail,item);
                avail++;
                away++;
            }
            if(availStatuses[isOnline] == GG_STATUS_BUSY || availStatuses[isOnline] == GG_STATUS_BUSY_DESCR) {
                item->setToolTip(availDescs[isOnline]);
                item->setIcon(QIcon("icons/away.png"));
                ui->listWidget->insertItem(away,item);
                away++;
            }
            if(availStatuses[isOnline] == GG_STATUS_INVISIBLE || availStatuses[isOnline] == GG_STATUS_INVISIBLE_DESCR) {
                item->setToolTip(availDescs[isOnline]);
                item->setIcon(QIcon("icons/invisible.png"));
                ui->listWidget->insertItem(i,item);
            }
            if(availStatuses[isOnline] == GG_STATUS_NOT_AVAIL || availStatuses[isOnline] == GG_STATUS_NOT_AVAIL_DESCR) {
                item->setToolTip(availDescs[isOnline]);
                item->setIcon(QIcon("icons/offline.png"));
                ui->listWidget->insertItem(i,item);
            }
        }
        listItems.push_back(item);
        listItemsNames.push_back(item->text().toStdString());
        all++;
    }
}

void MainWindow::changeStatus(uin_t number, int status, char *desc) {
    int wasOnline = availNumbers.indexOf(number);
    if(wasOnline == -1) {
        availNumbers.push_back(number);
        availStatuses.push_back(status);
        availDescs.push_back(QString::fromAscii(desc));
    }
    else {
        availStatuses[wasOnline] = status;
        availDescs[wasOnline] = QString::fromAscii(desc);
    }
    updateList(availNumbers,availStatuses,availDescs);
}

void MainWindow::getMessage(uin_t sender, QString message) {
    QString name = QString::fromStdString(getNameOfContact(sender));
    trayIcon->showMessage("Nowa wiadomosc od " + name,message);
    if(!findProperWindow(sender)) {
        // otwieramy nowe okno
        LGChatWindow *newChatWindow = new LGChatWindow;
        connect(newChatWindow,SIGNAL(msgSent(uin_t,std::string)),connectionThread,SLOT(send(uin_t,std::string)));
        connect(newChatWindow,SIGNAL(closed(LGChatWindow*)),this,SLOT(removeChatWindow(LGChatWindow*)));
        newChatWindow->setUserData(name,sender);
        if(name != "Nieznajomy") openedBuddies.push_back(name);
        openedWindows.push_back(newChatWindow);
        newChatWindow->show();
        newChatWindow->displayMessage(message);
    }
    else {
        LGChatWindow *ChatWindow = findProperWindow(sender);
        ChatWindow->displayMessage(message);
        // korzystamy z juz otwartego okna
    }
}

/*void MainWindow::openNewChatWindow(QString sender_name, QString message) {
    // otwieramy nowe okno
    uin_t number = getNumberOfContact(sender_name);
    LGChatWindow *newChatWindow = new LGChatWindow;
    connect(newChatWindow,SIGNAL(msgSent(uin_t,std::string)),connectionThread,SLOT(send(uin_t,std::string)));
    connect(newChatWindow,SIGNAL(closed(LGChatWindow*)),this,SLOT(removeChatWindow(LGChatWindow*)));
    newChatWindow->setUserData(sender_name,number);
    if(sender_name != "Nieznajomy") openedBuddies.push_back(sender_name);
    openedWindows.push_back(newChatWindow);
    newChatWindow->show();
    newChatWindow->displayMessage(message);
}*/

void MainWindow::startChat(QListWidgetItem *item) {
    int number;
    number = getNumberOfContact(item->text());
    if(openedBuddies.indexOf(item->text()) == -1) {
       LGChatWindow *newChatWindow = new LGChatWindow;
       connect(newChatWindow,SIGNAL(msgSent(uin_t,std::string)),connectionThread,SLOT(send(uin_t,std::string)));
       connect(newChatWindow,SIGNAL(closed(LGChatWindow*)),this,SLOT(removeChatWindow(LGChatWindow*)));
       newChatWindow->setUserData(item->text(),number);
       openedBuddies.push_front(item->text());
       newChatWindow->show();
       openedWindows.push_back(newChatWindow);
    }
}

int MainWindow::getNumberOfContact(QString contactName) {
    int index = allBuddiesNames.indexOf(contactName.toStdString());
    if(index != -1) return allBuddiesNumbers[index];
    else return 0;
}

std::string MainWindow::getNameOfContact(int number) {
    int index = allBuddiesNumbers.indexOf(number);
    if(index != -1) return allBuddiesNames[index];
    else return "Nieznajomy";
}

LGChatWindow* MainWindow::findProperWindow(int number) {
    int i;
    int vectorSize = openedWindows.size();
    for(i=0;i<vectorSize;i++) {
        if(openedWindows[i]->getNumber() == number) break;
    }
    if(i < vectorSize) return openedWindows[i];
    else return 0;
}

void MainWindow::removeChatWindow(LGChatWindow *a) {
    int removeIndex = openedWindows.indexOf(a);
    openedWindows.remove(removeIndex);
    removeIndex = openedBuddies.indexOf(a->getName());
    openedBuddies.remove(removeIndex);
}

void MainWindow::addContact() {
    LGAddContactWindow *addContactWindow = new LGAddContactWindow(this);
    // addContactWindow po kliknieciu "OK" wyemituje sygnal added. to pozwoli przekazac do bazy dane z jego QLineEditow
    connect(addContactWindow,SIGNAL(added(QString,QString)),contactListDB,SLOT(insertContactToDb(QString,QString)));
    addContactWindow->show();
}

void MainWindow::refreshList() {
    readContactList();
    updateList(availNumbers,availStatuses,availDescs);
}

void MainWindow::changeTrayIcon(int status) {
    switch(status) {
    case 0:
        trayIcon->setIcon(QIcon("icons/available.png"));
        break;
    case 1:
        trayIcon->setIcon(QIcon("icons/away.png"));
        break;
    case 2:
        trayIcon->setIcon(QIcon("icons/invisible.png"));
        break;
    case 4:
        trayIcon->setIcon(QIcon("icons/offline.png"));
        break;
    }
}

void MainWindow::trayClicked(QSystemTrayIcon::ActivationReason a) {
    if(a == QSystemTrayIcon::Trigger) {
        if(this->isVisible()) {
            lastPosition = this->pos();
            this->setVisible(false);
        }
        else {
            this->setVisible(true);
            this->move(lastPosition);
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *e) {
    if(!isGoingToBeClosed) { // sprawdz, czy user zamknal aplikacje "krzyzykiem", czy kliknal "Zakoncz" w menu programu.
        lastPosition = this->pos();
        this->setVisible(false);
        e->ignore();
    }
}

void MainWindow::buddyMenuRequested(const QPoint& point) {
    QPoint globalPos = this->mapToGlobal(point);
    buddyListMenu->whoWasSelected(ui->listWidget->itemAt(point));
    buddyListMenu->exec(globalPos);
}

void MainWindow::removeContact(QListWidgetItem *item) {
    int number = getNumberOfContact(item->text());
    contactListDB->removeContact(item->text(),QString::number(number));
}

void MainWindow::closeApp() {
    this->isGoingToBeClosed=1;
    this->close();
}

void MainWindow::setLoginData(int number, const char *password) {
    connectionThread->setLoginParams(number,password);
    settingsWindow->close();
    connectionThread->start();
}
