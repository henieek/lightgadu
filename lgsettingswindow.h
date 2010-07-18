#ifndef LGSETTINGSWINDOW_H
#define LGSETTINGSWINDOW_H

#include <QDialog>

namespace Ui {
    class LGSettingsWindow;
}

class LGSettingsWindow : public QDialog
{
    Q_OBJECT

public:
    LGSettingsWindow(QWidget *parent = 0);
    ~LGSettingsWindow();
signals:
    void done(int,const char*);
private slots:
    void set();
private:
    Ui::LGSettingsWindow *ui;
};

#endif // LGSETTINGSWINDOW_H
