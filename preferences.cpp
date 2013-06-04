#include "preferences.h"
#include "ui_preferences.h"
#include <QSettings>
#include <QDebug>
#include <QFileInfo>

const QString Preferences::settingsFileName = "wirk.ini";

Preferences::Preferences(QWidget *parent) : QDialog(parent), ui(new Ui::Preferences)
{
    ui->setupUi(this);
    settings = new QSettings(settingsFileName, QSettings::IniFormat);
    readSettings();
}

Preferences::~Preferences()
{
    delete ui;
    delete settings;
}

void Preferences::accept()
{
    this->writeSettings();
    this->close();
}

void Preferences::writeSettings()
{
    //QSettings settings(settingsFileName, QSettings::IniFormat);

    QString address = ui->server->text();
    QString port = ui->port->text();
    QString realName = ui->realName->text();
    QString nickname = ui->nickname->text();
    QString username = ui->username->text();
    QString password = ui->password->text();
    bool useSSL = ui->useSSL->isChecked();

    settings->beginGroup("server");
    settings->setValue("address", address);
    settings->setValue("port", port);
    settings->setValue("realname", realName);
    settings->setValue("nickname", nickname);
    settings->setValue("username", username);
    settings->setValue("password", password);
    settings->setValue("usessl", useSSL);
    settings->endGroup();
}

void Preferences::readSettings()
{
    //QSettings settings(settingsFileName, QSettings::IniFormat);

    settings->beginGroup("server");
    QString address = settings->value("address").toString();
    QString port = settings->value("port").toString();
    QString realName = settings->value("realname").toString();
    QString nickname = settings->value("nickname").toString();
    QString username = settings->value("username").toString();
    QString password = settings->value("password").toString();
    bool useSSL = settings->value("usessl", false).toBool();
    settings->endGroup();

    ui->server->setText(address);
    ui->port->setText(port);
    ui->realName->setText(realName);
    ui->nickname->setText(nickname);
    ui->username->setText(username);
    ui->password->setText(password);
    ui->useSSL->setChecked(useSSL);
}
