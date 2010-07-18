#ifndef LGCONNECTIONTHREAD_H
#define LGCONNECTIONTHREAD_H

#include <QThread>
#include <iostream>
#include <QVector>
#include <QTimer>

#include <libgadu.h>

typedef QVector<int> intVector;
typedef QVector<QString> qstringVector;

class LGConnectionThread : public QThread
{
   Q_OBJECT
public:
    LGConnectionThread();
    ~LGConnectionThread();
    void setContactList(QVector<int>);
    void setLoginParams(int,const char*);
public slots:
    void send(uin_t,std::string);
    void changeStatus(int);
    void addNotify(uin_t);
    void removeNotify(uin_t);
signals:
    void notify(intVector,intVector,qstringVector);
    void statusChanged(uin_t,int,char*);
    void changeStatusTrayIcon(int);
    void newMessage(uin_t,QString);
private:
    gg_session *sesja_gg;
    gg_event *zdarzenie_gg;
    gg_login_params p;
    intVector availNumbers;
    intVector availNumbersStatus;
    qstringVector availNumbersDesc;
    uin_t *contactList;
    int numberOfContacts;
    QTimer *pingTimer;
private slots:
    void ping();
protected:
    void run();
};

#endif // LGCONNECTIONTHREAD_H
