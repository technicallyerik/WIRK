#include "channelsettings.h"
#include "ui_channelsettings.h"
#include "preferenceshelper.h"

ChannelSettings::ChannelSettings(QString channelName, QString serverName, QWidget *parent) : QDialog(parent), ui(new Ui::ChannelSettings)
{
    // Setup Window
    ui->setupUi(this);
    this->channelName = channelName;
    this->serverName = serverName;
    ui->tabWidget->setTabText(0, channelName);

    // Get Preferences
    int useColorUsernames = PreferencesHelper::sharedInstance()->getPropertyForChannel(PreferencesHelper::colorUsernamesKey, channelName, serverName);
    int supressNotifications = PreferencesHelper::sharedInstance()->getPropertyForChannel(PreferencesHelper::suppressJoinNotificationsKey, channelName, serverName);
    int joinOnConnect = PreferencesHelper::sharedInstance()->getPropertyForChannel(PreferencesHelper::joinOnConnectKey, channelName, serverName);

    // Update UI
    this->populateDropdown(ui->colorUserNames, useColorUsernames);
    this->populateDropdown(ui->suppressEnterLeaveNotifications, supressNotifications);
    this->populateDropdown(ui->joinOnConnect, joinOnConnect);
}

void ChannelSettings::populateDropdown(QComboBox *dropdown, int selectedDropdownValue)
{
    dropdown->addItem("Default", QVariant((int)PreferencesHelper::Default_Option));
    dropdown->addItem("Yes", QVariant((int)PreferencesHelper::Yes_Option));
    dropdown->addItem("No", QVariant((int)PreferencesHelper::No_Option));

    int selectedIndex = dropdown->findData(QVariant(selectedDropdownValue));
    dropdown->setCurrentIndex(selectedIndex);
}

ChannelSettings::~ChannelSettings()
{
    delete ui;
}

void ChannelSettings::accept()
{
    QSettings *settings = PreferencesHelper::sharedInstance()->getSettings();

    // Set preferences
    int colorUserNames = this->ui->colorUserNames->itemData(this->ui->colorUserNames->currentIndex()).toInt();
    int suppressJoinNotificationPreference = this->ui->suppressEnterLeaveNotifications->itemData(this->ui->suppressEnterLeaveNotifications->currentIndex()).toInt();
    int joinOnConnect = this->ui->joinOnConnect->itemData(this->ui->joinOnConnect->currentIndex()).toInt();

    // Persist
    settings->beginGroup(PreferencesHelper::sharedInstance()->getChannelGroupKey(channelName, serverName));
    settings->setValue(PreferencesHelper::colorUsernamesKey, colorUserNames);
    settings->setValue(PreferencesHelper::suppressJoinNotificationsKey, suppressJoinNotificationPreference);
    settings->setValue(PreferencesHelper::joinOnConnectKey, joinOnConnect);
    settings->endGroup();
    settings->sync();

    // Close window
    this->close();
}
