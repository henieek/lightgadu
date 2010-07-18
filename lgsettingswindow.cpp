#include "lgsettingswindow.h"
#include "ui_lgsettingswindow.h"

LGSettingsWindow::LGSettingsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LGSettingsWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Ustawienia"));
    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(set()));
    connect(ui->buttonBox,SIGNAL(rejected()),this,SLOT(close()));
}

LGSettingsWindow::~LGSettingsWindow()
{
    delete ui;
}

void LGSettingsWindow::set() {
    emit done(ui->lineEdit->text().toInt(),ui->lineEdit_2->text().toStdString().c_str());
}
