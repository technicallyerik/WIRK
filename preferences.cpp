#include "preferences.h"
#include "ui_preferences.h"
#include <QSettings>
#include <QDebug>
#include <QFileInfo>

const QString Preferences::settingsFileName = "preferences.ini";
const QString Preferences::userPrefsName = "user preferences";

Preferences::Preferences(QWidget *parent) : QDialog(parent), ui(new Ui::Preferences)
{
    ui->setupUi(this);
    settings = new QSettings(settingsFileName, QSettings::IniFormat, this);

    settings->beginGroup(userPrefsName);
    bool colorUserNames = settings->value("colorusernames", true).toBool();
    settings->endGroup();

    this->ui->colorUserNames->setChecked(colorUserNames);
}

Preferences::~Preferences()
{
    delete ui;
}

void Preferences::accept()
{
    bool colorUserNames = this->ui->colorUserNames->isChecked();
    settings->beginGroup(userPrefsName);
    settings->setValue("colorusernames", colorUserNames);
    settings->endGroup();

    this->close();
}

