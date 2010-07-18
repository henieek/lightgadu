#include "lgaddcontactwindow.h"
#include "ui_lgaddcontactwindow.h"

LGAddContactWindow::LGAddContactWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LGAddContactWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("Dodaj kontakt"));
    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(add()));
    connect(ui->buttonBox,SIGNAL(rejected()),this,SLOT(close()));
}

LGAddContactWindow::~LGAddContactWindow()
{
    delete ui;
}

void LGAddContactWindow::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void LGAddContactWindow::add() {
    emit added(ui->lineEdit->text(),ui->lineEdit_2->text()); // ten sygnal bedzie wykorzystywany do przekazania danych do bazy danych
    close();
    delete this;
}
