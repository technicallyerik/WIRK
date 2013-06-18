#include "preferences.h"
#include "ui_preferences.h"
#include <QSettings>
#include <QDebug>
#include <QFileInfo>

Preferences::Preferences(QSettings *settings, QWidget *parent) : QDialog(parent), ui(new Ui::Preferences)
{
    ui->setupUi(this);
    this->settings = settings;
}

Preferences::~Preferences()
{
    delete ui;
}

void Preferences::accept()
{

}

