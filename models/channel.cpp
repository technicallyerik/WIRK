#include "channel.h"
#include "server.h"
#include "user.h"
#include "session.h"

Channel::Channel(QString inName, QStandardItem *inMenuItem, Server *parent) : QObject(parent)
{
    text = "<body>";
    users = new QStandardItemModel(this);
    menuItem = inMenuItem;
    this->setName(inName);
}

QString Channel::getName() {
    return name;
}

void Channel::setName(QString inName) {
    name = inName;
    menuItem->setText(inName);
}

QString Channel::getText() {
    return text;
}

void Channel::appendText(QString inText) {
    QDateTime currentTime = QDateTime::currentDateTime();
    QString currentTimeStr = currentTime.toString("h:mmap, MMM d, yyyy");

    QString tableRow = "<table class=\"msg-info\" width=\"100%\"><tr>";
            tableRow += "<td class=\"col-name\" width=\"140\"></td>";
            tableRow += "<td class=\"col-message\"><p class=\"message\">" + inText + "</p></td>";
            tableRow += "<td class=\"col-meta\" width=\"125\"><h6 class=\"metainfo\">" + currentTimeStr + "</h6></td>";
            tableRow += "</tr></table>";
    text += tableRow;
    QString channelName = this->getName();
    Server *server = this->getServer();
    QString serverName = server->getHost();
    Session *session = server->getSession();
    session->emitMessageReceived(serverName, channelName, tableRow);
}


void Channel::appendText(QString sender, QString inText, bool isEmote) {
    QString currentUser = this->getServer()->getNickname();
    bool textContainsUser = inText.contains(currentUser, Qt::CaseInsensitive);
    QDateTime currentTime = QDateTime::currentDateTime();
    QString currentTimeStr = currentTime.toString("h:mmap, MMM d, yyyy");

    QString tableRow = "";
    if (isEmote)
    {
        tableRow = "<table class=\"msg-emote\" width=\"100%\"><tr>";
    }
    else if (textContainsUser)
    {
        tableRow = "<table class=\"msg-mentioned\" width=\"100%\"><tr>";
    }
    else
    {
        tableRow = "<table width=\"100%\"><tr>";
    }
        tableRow += "<th class=\"col-name\" width=\"140\" align=\"right\"><span class=\"user\">" + sender + "</span></th>";
        tableRow += "<td class=\"col-message\"><p class=\"message\">" + inText + "</p></td>";
        tableRow += "<td class=\"col-meta\" width=\"125\"><h6 class=\"metainfo\">" + currentTimeStr +"</h6></td>";
        tableRow += "</tr></table>";
    text += tableRow;
    QString channelName = this->getName();
    Server *server = this->getServer();
    QString serverName = server->getHost();
    Session *session = server->getSession();
    session->emitMessageReceived(serverName, channelName, tableRow);
}

QStandardItemModel* Channel::getUsers() {
    return users;
}

void Channel::addUsers(QStringList inUsers) {
    for (int i = 0; i < inUsers.count(); i++) {
        QString newUserName = inUsers[i];
        this->addUser(newUserName);
    }
}

void Channel::addUser(QString inUser) {
    QStandardItem *newMenuItem = new QStandardItem();
    User *newUser = new User(inUser.toLower(), newMenuItem, this);
    newMenuItem->setData(QVariant::fromValue<User*>(newUser), Qt::UserRole);
    users->appendRow(newMenuItem);
    users->sort(0);
}

void Channel::removeUser(QString inUser) {
    QList<QStandardItem*> foundUsers = users->findItems(inUser.toLower());
    if(foundUsers.count() == 1) {
        QStandardItem *user = foundUsers[0];
        int row = user->row();
        users->removeRow(row);
    }
}

User* Channel::getUser(QString inUser) {
    QList<QStandardItem*> foundUsers = users->findItems(inUser.toLower());
    if(foundUsers.count() == 1) {
        QStandardItem *user = foundUsers[0];
        QVariant data = user->data(Qt::UserRole);
        return data.value<User*>();
    }
    return NULL;
}

Server* Channel::getServer() {
    return qobject_cast<Server *>(this->parent());
}
