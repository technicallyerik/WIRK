#include "preferences.h"
#include "ui_preferences.h"
#include <QSettings>
#include <QDebug>
#include <QFileInfo>
#include "preferenceshelper.h"

Preferences::Preferences(QWidget *parent) : QDialog(parent), ui(new Ui::Preferences)
{
    ui->setupUi(this);

    QSettings *settings = PreferencesHelper::sharedInstance()->getSettings();

    // Get 'Should use user colors' Preference
    bool useColorUsernamesPreference = PreferencesHelper::sharedInstance()->getShouldUseColorUsernames();
    this->ui->colorUserNames->setChecked(useColorUsernamesPreference);
}

Preferences::~Preferences()
{
    delete ui;
}

void Preferences::accept()
{
    QSettings *settings = PreferencesHelper::sharedInstance()->getSettings();

    // Set 'Should user user colors' Preference
    bool useColorUsernamesPreference = this->ui->colorUserNames->isChecked();
    settings->beginGroup(PreferencesHelper::displayPreferencesGroupKey);
    settings->setValue("colorusernames", useColorUsernamesPreference);
    settings->endGroup();

    settings->sync();
    this->close();
}

