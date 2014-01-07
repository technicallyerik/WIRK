#include "preferences.h"
#include "ui_preferences.h"
#include <QSettings>
#include <QDebug>
#include <QFileInfo>
#include "preferenceshelper.h"

Preferences::Preferences(QWidget *parent) : QDialog(parent), ui(new Ui::Preferences)
{
    // Setup Window
    ui->setupUi(this);

    // Get preferences
    bool useColorUsernamesPreference = PreferencesHelper::sharedInstance()->getPropertyDefault(PreferencesHelper::colorUsernamesKey, true);
    bool joinOnConnectPreference = PreferencesHelper::sharedInstance()->getPropertyDefault(PreferencesHelper::joinOnConnectKey, true);
    bool hideJoinNotificationsPreference = PreferencesHelper::sharedInstance()->getPropertyDefault(PreferencesHelper::suppressJoinNotificationsKey, false);

    // Update UI
    this->ui->colorUserNames->setChecked(useColorUsernamesPreference);
    this->ui->joinOnConnect->setChecked(joinOnConnectPreference);
    this->ui->suppressEnterLeaveNotifications->setChecked(hideJoinNotificationsPreference);
}

Preferences::~Preferences()
{
    delete ui;
}

void Preferences::accept()
{
    QSettings *settings = PreferencesHelper::sharedInstance()->getSettings();

    // Set preferences
    bool useColorUsernamesPreference = this->ui->colorUserNames->isChecked();
    bool joinOnConnectPreference = this->ui->joinOnConnect->isChecked();
    bool hideJoinNotificationsPreference = this->ui->suppressEnterLeaveNotifications->isChecked();

    // Persist
    settings->beginGroup(PreferencesHelper::displayPreferencesGroupKey);
    settings->setValue(PreferencesHelper::colorUsernamesKey, useColorUsernamesPreference);
    settings->setValue(PreferencesHelper::joinOnConnectKey, joinOnConnectPreference);
    settings->setValue(PreferencesHelper::suppressJoinNotificationsKey, hideJoinNotificationsPreference);
    settings->endGroup();
    settings->sync();

    // Close Window
    this->close();
}

