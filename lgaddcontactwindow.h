#ifndef LGADDCONTACTWINDOW_H
#define LGADDCONTACTWINDOW_H

#include <QDialog>

namespace Ui {
    class LGAddContactWindow;
}

class LGAddContactWindow : public QDialog {
    Q_OBJECT
public:
    LGAddContactWindow(QWidget *parent = 0);
    ~LGAddContactWindow();
public slots:
    void add();
protected:
    void changeEvent(QEvent *e);

private:
    Ui::LGAddContactWindow *ui;
signals:
    void added(QString,QString);
};

#endif // LGADDCONTACTWINDOW_H
