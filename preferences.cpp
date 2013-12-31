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
    bool joinOnConnectPreference = PreferencesHelper::sharedInstance()->getShouldJoinOnConnect();
    bool hideJoinNotificationsPreference = PreferencesHelper::sharedInstance()->getShouldHideJoinNotifications();

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

    // Set 'Should user user colors' Preference
    bool useColorUsernamesPreference = this->ui->colorUserNames->isChecked();
    bool joinOnConnectPreference = this->ui->joinOnConnect->isChecked();
    bool hideJoinNotificationsPreference = this->ui->suppressEnterLeaveNotifications->isChecked();

    settings->beginGroup(PreferencesHelper::displayPreferencesGroupKey);
    settings->setValue("colorusernames", useColorUsernamesPreference);
    settings->setValue("joinOnConnect", joinOnConnectPreference);
    settings->setValue("suppressjoinnotification", hideJoinNotificationsPreference);
    settings->endGroup();

    settings->sync();
    this->close();
}

