#include "channelsettings.h"
#include "ui_channelsettings.h"

const QString ChannelSettings::suppressJoinNotificationsKey = QString("suppressjoinnotification");

ChannelSettings::ChannelSettings(QString channelName, QString serverName, QWidget *parent) : QDialog(parent), ui(new Ui::ChannelSettings)
{
    ui->setupUi(this);
    this->channelName = channelName;
    this->serverName = serverName;
    ui->tabWidget->setTabText(0, channelName);
    settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "com.flashforwardlabs", "wirk", this);

    ui->suppressEnterLeaveNotifications->setChecked(this->shouldHideJoinNotifications());
}

ChannelSettings::~ChannelSettings()
{
    delete ui;
}

void ChannelSettings::accept()
{
    bool suppressJoinNotificationPreference = this->ui->suppressEnterLeaveNotifications->isChecked();
    settings->beginGroup(getGroupKey());
    settings->setValue(suppressJoinNotificationsKey, suppressJoinNotificationPreference);
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
