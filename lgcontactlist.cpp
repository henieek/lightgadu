#include "lgcontactlist.h"

LGContactList::LGContactList()
{
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setHostName("localhost");
    database.setDatabaseName("contacts.db");
    database.open();
    getListQuery = QSqlQuery(database);
    getListQuery.prepare("SELECT number,name FROM data ORDER BY name");
}

LGContactList::~LGContactList() {
    database.close();
}

QVector<int> LGContactList::getNumbers() {
    getListQuery.exec();
    QVector<int> list;
    while(getListQuery.next()) list.push_back(getListQuery.value(0).toString().toInt());
    list.push_back(getListQuery.value(0).toString().toInt()); // ostatni element - inaczej nie dziala. to co pare linijek nizej
    return list;
}

QVector<std::string> LGContactList::getNames() {
    getListQuery.exec();
    QVector <std::string> list;
    while(getListQuery.next()) list.push_back(getListQuery.value(1).toString().toStdString());
    list.push_back(getListQuery.value(1).toString().toStdString()); // ostatni element musi byc poza petla. bo inaczej sie nie wczyta.
    return list;
}

void LGContactList::insertContactToDb(QString name, QString number) {
    QSqlQuery insertQuery(database);
    insertQuery.prepare("INSERT INTO data VALUES (" + number + ",'" + name + "')");
    if(insertQuery.exec()) {
        emit contactAdded(); // wysylamy sygnal do mainwindow, aby umozliwic odswiezanie listy kontaktow
        emit ggAddNotify(number.toUInt()); // kazemy lgconnectionthread wyslac do serwera gg_add_notify(uin_t); poleczenie nastepuje w mainwindow.cpp
    }
    else {
        // tutaj musimy wyemitowac sygnal mowiacy, ze cos sie skopalo i nie udalo sie dodac kontaktu do bazy. trzeba dac jakis QMessageBox.
    }
}

void LGContactList::removeContact(QString contactName, QString number) {
    QSqlQuery removeQuery(database);
    removeQuery.prepare("DELETE FROM data WHERE name='" + contactName + "'");
    if(removeQuery.exec()) {
        emit contactRemoved();
        emit ggRemoveNotify(number.toUInt());
    }
}
