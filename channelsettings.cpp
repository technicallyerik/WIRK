#include "channelsettings.h"
#include "ui_channelsettings.h"
#include "preferenceshelper.h"

const QString ChannelSettings::suppressJoinNotificationsKey = QString("suppressjoinnotification");
const QString ChannelSettings::joinOnConnectKey = QString("joinOnConnect");
const QString ChannelSettings::colorUsernamesKey = QString("colorusernames");

ChannelSettings::ChannelSettings(QString channelName, QString serverName, QWidget *parent) : QDialog(parent), ui(new Ui::ChannelSettings)
{
    ui->setupUi(this);
    this->channelName = channelName;
    this->serverName = serverName;
    ui->tabWidget->setTabText(0, channelName);
    settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "com.flashforwardlabs", "wirk", this);

    this->populateDropdown(ui->colorUserNames, colorUsernamesKey);
    this->populateDropdown(ui->suppressEnterLeaveNotifications, suppressJoinNotificationsKey);
    this->populateDropdown(ui->joinOnConnect, joinOnConnectKey);
}

ChannelSettings::~ChannelSettings()
{
    delete ui;
}

void ChannelSettings::accept()
{
    int colorUserNames = this->ui->colorUserNames->itemData(this->ui->colorUserNames->currentIndex()).toInt();
    int suppressJoinNotificationPreference = this->ui->suppressEnterLeaveNotifications->itemData(this->ui->suppressEnterLeaveNotifications->currentIndex()).toInt();
    int joinOnConnect = this->ui->joinOnConnect->itemData(this->ui->joinOnConnect->currentIndex()).toInt();

    settings->beginGroup(getGroupKey());
    settings->setValue(colorUsernamesKey, colorUserNames);
    settings->setValue(suppressJoinNotificationsKey, suppressJoinNotificationPreference);
    settings->setValue(joinOnConnectKey, joinOnConnect);
    settings->endGroup();

    settings->sync();
    this->close();
}

QString ChannelSettings::getGroupKey()
{
    return QString("%1/%2").arg(this->serverName, this->channelName);
}

bool ChannelSettings::shouldHideJoinNotifications()
{
    int hideJoinNotificationsSetting = getPropertyValue(suppressJoinNotificationsKey);
    switch (hideJoinNotificationsSetting)
    {
        case Default_Option:
            return PreferencesHelper::sharedInstance()->getShouldHideJoinNotifications();
            break;
        case Yes_Option:
            return true;
            break;
        case No_Option:
            return false;
            break;
        default:
            return PreferencesHelper::sharedInstance()->getShouldHideJoinNotifications();
            break;
    }
}

bool ChannelSettings::shouldJoinOnConnect()
{
    int joinOnConnectSetting = getPropertyValue(joinOnConnectKey);
    switch (joinOnConnectSetting)
    {
        case Default_Option:
            return PreferencesHelper::sharedInstance()->getShouldJoinOnConnect();
            break;
        case Yes_Option:
            return true;
            break;
        case No_Option:
            return false;
            break;
        default:
            return PreferencesHelper::sharedInstance()->getShouldJoinOnConnect();
            break;
    }
}

bool ChannelSettings::shouldColorUserNames()
{
    int channelSetting = getPropertyValue(colorUsernamesKey);
    switch (channelSetting)
    {
        case Default_Option:
            return PreferencesHelper::sharedInstance()->getShouldUseColorUsernames();
            break;
        case Yes_Option:
            return true;
            break;
        case No_Option:
            return false;
            break;
        default:
            return PreferencesHelper::sharedInstance()->getShouldUseColorUsernames();
            break;
    }
}

int ChannelSettings::getPropertyValue(QString propertyKey)
{
    settings->beginGroup(getGroupKey());
    int propertyValue = settings->value(propertyKey, (int)Default_Option).toInt();
    settings->endGroup();
    return propertyValue;
}

void ChannelSettings::populateDropdown(QComboBox *dropdown, QString propertyValueKey)
{
    dropdown->addItem("Default", QVariant((int)Default_Option));
    dropdown->addItem("Yes", QVariant((int)Yes_Option));
    dropdown->addItem("No", QVariant((int)No_Option));

    int selectedDropdownValue = getPropertyValue(propertyValueKey);
    int selectedIndex = dropdown->findData(QVariant(selectedDropdownValue));
    dropdown->setCurrentIndex(selectedIndex);
}
