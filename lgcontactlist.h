#ifndef LGCONTACTLIST_H
#define LGCONTACTLIST_H

#include <QObject>
#include <QtSql>
#include <QVector>
#include <iostream>
#include <libgadu.h>

class LGContactList : public QObject
{
    Q_OBJECT
public:
    LGContactList();
    ~LGContactList();
    QVector<int> getNumbers();
    QVector<std::string> getNames();
    void removeContact(QString,QString);
public slots:
    void insertContactToDb(QString,QString);
private:
    QSqlDatabase database;
    QSqlQuery getListQuery;
signals:
    void contactAdded();
    void contactRemoved();
    void ggAddNotify(uin_t);
    void ggRemoveNotify(uin_t);
};

#endif // LGCONTACTLIST_H
