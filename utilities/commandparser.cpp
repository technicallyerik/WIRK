#include "commandparser.h"
#include "server.h"
#include "irccommand.h"
#include "channel.h"

CommandParser::CommandParser(QObject *parent) : QObject(parent)
{

}

IrcCommand *CommandParser::parse(QString commandStr, Server* currentServer, Channel* currentChannel)
{
    QStringList commandBlock = commandStr.trimmed().split(" ");
    QString commandString = commandBlock.takeFirst().mid(1);

    // /ADMIN
    if (commandString.compare("admin", Qt::CaseInsensitive) == 0)
    {
        QString server = (!commandBlock.isEmpty()) ? commandBlock.at(0) : NULL;
        return IrcCommand::createAdmin(server);
    }

    // /AWAY
    else if (commandString.compare("away", Qt::CaseInsensitive) == 0)
    {
        QString mesg = (!commandBlock.isEmpty()) ? commandBlock.join(" ") : NULL;
        return IrcCommand::createAway(mesg);
    }

    // /CAP

    // CtcpReply

    // CtcpRequest

    // /INFO
    else if (commandString.compare("info", Qt::CaseInsensitive) == 0)
    {
        QString serverStr = (!commandBlock.isEmpty()) ? commandBlock.join(" ") : NULL;
        return IrcCommand::createInfo(serverStr);
    }

    // /INVITE
    else if (commandString.compare("invite", Qt::CaseInsensitive) == 0)
    {
        if(commandBlock.count() > 0) {
            QString nickname = commandBlock.takeFirst();
            if(commandBlock.count() > 0) {
                QString channel = commandBlock.takeFirst();
                return IrcCommand::createInvite(nickname, channel);
            } else if(currentChannel != NULL) {
                QString channel = currentChannel->getName();
                return IrcCommand::createInvite(nickname, channel);
            }
        }
    }

    // /JOIN
    else if(commandString.compare("join", Qt::CaseInsensitive) == 0)
    {
        if(commandBlock.count() > 0) {
            QStringList channels = commandBlock.takeFirst().split(',');
            QStringList passwords = QStringList();
            if(channels.first().compare("0") == 0) {
                currentServer->partAllChannels();
                return NULL;
            }
            if(commandBlock.count() > 0) {
                passwords = commandBlock.takeFirst().split(',');
            }
            return IrcCommand::createJoin(channels, passwords);
        }
    }

    // /KICK
    else if(commandString.compare("kick", Qt::CaseInsensitive) == 0)
    {
        if(commandBlock.count() >= 3 && currentChannel == NULL) {
            // Channel, User, Comment
            QStringList channels = commandBlock.takeFirst().split(",");
            QStringList users = commandBlock.takeFirst().split(",");
            QString reason = commandBlock.join(" ").trimmed();
            for (int i = 0; i < users.count(); i++) {
                QString channel = channels.at(0);
                if(channels.count() == users.count()) {
                    channel = channels.at(i);
                }
                QString user = users.at(i);
                IrcCommand *command = IrcCommand::createKick(channel, user, reason);
                currentServer->sendCommand(command);
            }
        } else if(commandBlock.count() > 2 && currentChannel != NULL) {
            // Users and Comment
            QStringList users = commandBlock.takeFirst().split(",");
            QString reason = commandBlock.join(" ").trimmed();
            QString channel = currentChannel->getName();
            foreach (QString user, users) {
                IrcCommand *command = IrcCommand::createKick(channel, user, reason);
                currentServer->sendCommand(command);
            }
        } else if(commandBlock.count() == 2 && currentChannel == NULL) {
            // Channels and Users
            QStringList channels = commandBlock.takeFirst().split(",");
            QStringList users = commandBlock.takeFirst().split(",");
            for (int i = 0; i < users.count(); i++) {
                QString channel = channels.at(0);
                if(channels.count() == users.count()) {
                    channel = channels.at(i);
                }
                QString user = users.at(i);
                IrcCommand *command = IrcCommand::createKick(channel, user, NULL);
                currentServer->sendCommand(command);
            }
        } else if(commandBlock.count() == 1 && currentChannel != NULL) {
            // Users
            QStringList users = commandBlock.takeFirst().split(",");
            QString channel = currentChannel->getName();
            foreach (QString user, users) {
                IrcCommand *command = IrcCommand::createKick(channel, user, NULL);
                currentServer->sendCommand(command);
            }
        }
    }

    // /KNOCK
    else if (commandString.compare("knock", Qt::CaseInsensitive) == 0)
    {
        if (commandBlock.count() > 1)
        {
            QString channel = commandBlock.takeFirst();
            QString msg = commandBlock.join(" ").trimmed();
            return IrcCommand::createKnock(channel, msg);
        }
        else if (commandBlock.count() == 1)
        {
            return IrcCommand::createKnock(commandBlock.at(0));
        }
    }

    // /LIST
    else if (commandString.compare("list", Qt::CaseInsensitive) == 0)
    {
        QStringList channelList = (!commandBlock.isEmpty()) ? commandBlock.takeFirst().split(",") : QStringList();
        QString server = (!commandBlock.isEmpty()) ? commandBlock.join(" ") : NULL;
        return IrcCommand::createList(channelList,server);
    }

    // /ME
    else if(commandString.compare("me", Qt::CaseInsensitive) == 0)
    {
        if(commandBlock.count() > 0 && currentChannel != NULL) {
            QString msg = commandBlock.join(" ").trimmed();
            currentChannel->appendText(currentServer->getNickname(), msg, Channel::MessageTypeEmote);
            return IrcCommand::createCtcpAction(currentChannel->getName(), msg);
        }
    }

    // /MODE
    else if(commandString.compare("mode", Qt::CaseInsensitive) == 0)
    {
        if (commandBlock.count() > 2) {
            return IrcCommand::createMode(commandBlock.at(0), commandBlock.at(1), commandBlock.at(2));
        }
        else if (commandBlock.count() > 1)
        {
            return IrcCommand::createMode(commandBlock.at(0), commandBlock.at(1));
        }
        else if (commandBlock.count() == 1)
        {
            return IrcCommand::createMode(commandBlock.at(0));
        }
    }

    // /MOTD
    else if (commandString.compare("motd", Qt::CaseInsensitive) == 0)
    {
        QString server = commandBlock.count() > 0 ? commandBlock.at(0) : NULL;
        return IrcCommand::createMotd(server);
    }

    // /MSG
    else if (commandString.compare("msg", Qt::CaseInsensitive) == 0)
    {
        if(commandBlock.count() > 1) {
            QString nickname = commandBlock.takeFirst();
            QString msg = commandBlock.join(" ").trimmed();
            Channel *channel = currentServer->getChannel(nickname);
            if (channel == NULL) {
                if(Channel::isChannel(nickname)) {
                    channel = currentServer->addChannel(nickname, Channel::ChannelTypeNormal);
                } else {
                    channel = currentServer->addChannel(nickname, Channel::ChannelTypeUser);
                }
            }
            QString ourName = currentServer->getNickname();
            channel->appendText(ourName, msg);
            return IrcCommand::createMessage(nickname, msg);
        }
    }

    // /NAMES
    else if (commandString.compare("names", Qt::CaseInsensitive) == 0)
    {
        // TODO: After playing around with this, we have to figure out how to diffentiate between this command
        // and a user actually joining the channel, due to weirdness with the client thinking a user
        // actually joined the channel. Commenting this out for now.
        /*if (commandBlock.count() > 0)
        {
            QStringList channels = commandBlock.at(0).split(",", QString::SkipEmptyParts);
            QString server = commandBlock.count() > 1 ? commandBlock.at(1) : NULL;
            return IrcCommand::createNames(channels, server);
        }
        else
        {
            return IrcCommand::createNames();
        }*/
    }

    // /NICK
    else if (commandString.compare("nick", Qt::CaseInsensitive) == 0)
    {
        return IrcCommand::createNick(commandBlock.join(" ").trimmed());
    }

    // /NOTICE
    else if (commandString.compare("notice", Qt::CaseInsensitive) == 0)
    {
        if(commandBlock.count() > 1) {
            QString target = commandBlock.takeFirst();
            QString msg = commandBlock.join(" ").trimmed();
            return IrcCommand::createNotice(target, msg);
        }
    }

    // /PART
    else if (commandString.compare("part", Qt::CaseInsensitive) == 0)
    {
        if(commandBlock.count() > 0) {
            QString channelStr = commandBlock.takeFirst();
            QString reason = (!commandBlock.isEmpty()) ? commandBlock.join(" ") : NULL;
            return IrcCommand::createPart(channelStr, reason);
        }
    }

    // /QUIT
    else if (commandString.compare("quit", Qt::CaseInsensitive) == 0)
    {
        QString reason = (!commandBlock.isEmpty()) ? commandBlock.join(" ") : NULL;
        return IrcCommand::createQuit(reason);
    }

    // /QUOTE
    else if (commandString.compare("quote", Qt::CaseInsensitive) == 0)
    {
        return IrcCommand::createQuote(commandBlock);
    }

    // /STATS
    else if (commandString.compare("stats", Qt::CaseInsensitive) == 0)
    {
        if (commandBlock.count() > 1)
        {
            return IrcCommand::createStats(commandBlock.at(0), commandBlock.at(1));
        }
        else if (commandBlock.count() == 1)
        {
            return IrcCommand::createStats(commandBlock.at(0));
        }
        else
        {
            return IrcCommand::createStats("");
        }
    }

    // /TIME
    else if (commandString.compare("time", Qt::CaseInsensitive) == 0)
    {
        if (commandBlock.count() > 0)
        {
            return IrcCommand::createTime(commandBlock.at(0));
        }
        else
        {
            return IrcCommand::createTime();
        }
    }

    // /TOPIC
    else if (commandString.compare("topic", Qt::CaseInsensitive) == 0)
    {
        if (commandBlock.count() > 1)
        {
            return IrcCommand::createTopic(commandBlock.at(0), commandBlock.at(1));
        }
        else if (commandBlock.count() == 1)
        {
            return IrcCommand::createTopic(commandBlock.at(0));
        }
    }

    // /TRACE

    // /USERS

    // /VERSION
    else if (commandString.compare("version", Qt::CaseInsensitive) == 0)
    {
        if (commandBlock.count() > 0)
        {
            return IrcCommand::createVersion(commandBlock.at(0));
        }
        else
        {
            return IrcCommand::createVersion();
        }
    }

    // /WHO
    else if (commandString.compare("who", Qt::CaseInsensitive) == 0)
    {
        if (commandBlock.count() > 1)
        {
            bool operators = commandBlock.at(1).trimmed().toLower() == "o";
            return IrcCommand::createWho(commandBlock.at(0), operators);
        }
        else if (commandBlock.count() == 1)
        {
            return IrcCommand::createWho(commandBlock.at(0));
        }
    }

    // /WHOIS
    else if (commandString.compare("whois", Qt::CaseInsensitive) == 0)
    {
        if (commandBlock.count() > 0)
        {
            return IrcCommand::createWhois(commandBlock.at(0));
        }
    }

    // /WHOWAS
    else if (commandString.compare("whowas", Qt::CaseInsensitive) == 0)
    {
        if (commandBlock.count() > 1)
        {
            bool *isInt = new bool;
            int count = commandBlock.at(1).toInt(isInt);
            if (isInt)
            {
                delete isInt;
                return IrcCommand::createWhowas(commandBlock.at(0), count);
            }
            else
            {
                delete isInt;
                return IrcCommand::createWhowas(commandBlock.at(0));
            }
        }
        else if (commandBlock.count() == 1)
        {
            return IrcCommand::createWhowas(commandBlock.at(0));
        }
    }

    return NULL;
}
