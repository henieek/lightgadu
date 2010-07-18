#include "lgconnectionthread.h"

LGConnectionThread::LGConnectionThread() {
    sesja_gg = new gg_session;
    zdarzenie_gg = new gg_event;
    pingTimer = new QTimer(this);
    connect(pingTimer,SIGNAL(timeout()),this,SLOT(ping()));
}

LGConnectionThread::~LGConnectionThread() {
    delete zdarzenie_gg;
    delete sesja_gg;
}

void LGConnectionThread::run() {
    QString message;
    uin_t sender;
    sesja_gg = gg_login(&p);
    fd_set rd, wr, ex;\
    while(1) {       
      FD_ZERO(&rd);
      FD_ZERO(&wr);
      FD_ZERO(&ex);
      if((sesja_gg->check & GG_CHECK_READ)) FD_SET(sesja_gg->fd, &rd);
      if ((sesja_gg->check & GG_CHECK_WRITE)) FD_SET(sesja_gg->fd, &wr);
      FD_SET(sesja_gg->fd, &ex);
      if (select(sesja_gg->fd + 1, &rd, &wr, &ex, NULL) == -1) std::cout << "BLAD!\n";
      if (FD_ISSET(sesja_gg->fd, &ex)) std::cout << "BLAD!\n";
      if ((FD_ISSET(sesja_gg->fd, &rd) || FD_ISSET(sesja_gg->fd, &wr))) { // No to jedziem ;-)
        zdarzenie_gg = gg_watch_fd(sesja_gg);
        switch(zdarzenie_gg->type) {
                case GG_EVENT_CONN_FAILED:
                        // wiadomo
                        break;
                case GG_EVENT_CONN_SUCCESS:
                        // wiadomo
                        pingTimer->start(180000);
                        gg_notify(sesja_gg,contactList,numberOfContacts);
                        break;
                case GG_EVENT_NOTIFY60:
                        // to sie robi przy logowaniu sie do sieci gg. dostajemy dane o naszej liscie kontaktow
                        for(int i=0;zdarzenie_gg->event.notify60[i].uin;i++) {
                            availNumbers.push_back(zdarzenie_gg->event.notify60[i].uin);
                            availNumbersStatus.push_back(zdarzenie_gg->event.notify60[i].status);
                            availNumbersDesc.push_back(QString::fromUtf8(zdarzenie_gg->event.notify60[i].descr));
                        }
                        emit notify(availNumbers,availNumbersStatus,availNumbersDesc);
                        emit changeStatusTrayIcon(0); // ustawiamy status dostepny w trayu.
                    break;
                case GG_EVENT_STATUS60:
                    // ktos z naszej listy zmienil status
                      emit statusChanged(zdarzenie_gg->event.status60.uin,zdarzenie_gg->event.status60.status,zdarzenie_gg->event.status60.descr);
                    break;
                case GG_EVENT_MSG:
                    // przychodzi nowa wiadomosc
                    sender = zdarzenie_gg->event.msg.sender;
                    message = QString::fromUtf8((const char*)zdarzenie_gg->event.msg.message);
                    if(sender!=0) emit newMessage(sender,message);
                   break;
                default:
                        break;
        } // koniec switcha
      } // koniec if((FD_ISSET....
    } // koniec while'a
    delete zdarzenie_gg;
// to jest watek
}

void LGConnectionThread::setContactList(QVector<int> contacts) {
    int i;
    contactList = new uin_t[contacts.size()];
    for(i=0;i<contacts.size();i++) {
        contactList[i] = contacts[i];
    }
    numberOfContacts = i-1;
}

void LGConnectionThread::send(uin_t recv, std::string message) {
    gg_send_message(sesja_gg,GG_CLASS_CHAT,recv,(const unsigned char*)message.c_str());
}

void LGConnectionThread::changeStatus(int status) {
    switch(status) {
    case 0:
        gg_change_status(sesja_gg,GG_STATUS_AVAIL);
        break;
    case 1:
        gg_change_status(sesja_gg,GG_STATUS_BUSY);
        break;
    case 2:
        gg_change_status(sesja_gg,GG_STATUS_INVISIBLE);
        break;
    case 3:
        gg_change_status(sesja_gg,GG_STATUS_NOT_AVAIL);
        break;
    default:
        break;
    }
    emit changeStatusTrayIcon(status);
}

/*
ping-pong function. we have to ping server to say him: hey, look, i'm still connected!
ping function is executed every three minutes.
*/

void LGConnectionThread::ping() {
    gg_ping(sesja_gg);
    pingTimer->start(180000);
}

/*
  addNotify to slot wykonywany, gdy dodalismy liste kontaktow. dostajemy go od bazy danych. polaczenie nastepuje w mainwindow.cpp
*/

void LGConnectionThread::addNotify(uin_t number) {
    gg_add_notify(sesja_gg,number);
}

/*
 Podobnie jak addNotify, tyle ¿e dla usuwania powiadamiania dla danego numeru. Polaczony z sygnalem w niemal identyczny sposob.
 */

void LGConnectionThread::removeNotify(uin_t number) {
    gg_remove_notify(sesja_gg,number);
}

void LGConnectionThread::setLoginParams(int number, const char* password) {
    memset(&p,'\0',sizeof(p));
    p.uin = number; // wpisz tutaj swój numer gg
    p.password = const_cast<char*>(password); // wpisz tutaj swoje has³o
    p.async = 1;
    p.status = GG_STATUS_AVAIL;
}
