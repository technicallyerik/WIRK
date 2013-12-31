#include "channelsettings.h"
#include "ui_channelsettings.h"

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
    ui->suppressEnterLeaveNotifications->setChecked(this->shouldHideJoinNotifications());
    ui->joinOnConnect->setChecked(this->shouldJoinOnConnect());
}

ChannelSettings::~ChannelSettings()
{
    delete ui;
}

void ChannelSettings::accept()
{
    int colorUserNames = this->ui->colorUserNames->itemData(this->ui->colorUserNames->currentIndex()).toInt();
    bool suppressJoinNotificationPreference = this->ui->suppressEnterLeaveNotifications->isChecked();
    bool joinOnConnect = this->ui->joinOnConnect->isChecked();

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
    settings->beginGroup(getGroupKey());
    bool hideNotifications = settings->value(suppressJoinNotificationsKey, false).toBool();
    settings->endGroup();
    return hideNotifications;
}

bool ChannelSettings::shouldJoinOnConnect()
{
    settings->beginGroup(getGroupKey());
    bool joinOnConnect = settings->value(joinOnConnectKey, true).toBool();
    settings->endGroup();
    return joinOnConnect;
}

int ChannelSettings::shouldColorUserNames()
{
    return getPropertyValue(colorUsernamesKey);
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
