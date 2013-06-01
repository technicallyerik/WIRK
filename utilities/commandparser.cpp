#include "commandparser.h"
#include "server.h"
#include "irccommand.h"
#include "channel.h"

CommandParser::CommandParser(Server *parent) : QObject(parent)
{

}

IrcCommand *CommandParser::parse(QString commandStr)
{
    QRegExp commandRX("^/([a-zA-Z]+) (\\S+)( .+)?");
    QRegExp channelRX("([#&][^\\x07\\x2C\\s]{,200})");
    int pos = commandRX.indexIn(commandStr);
    if(pos > -1) {
        QString commandString = commandRX.cap(1);
        // /ADMIN

        // /AWAY

        // /CAP

        // CtcpAction

        // CtcpReply

        // CtcpRequest

        // /INFO

        // /INVITE
        if (commandString.compare("invite", Qt::CaseInsensitive) == 0)
        {
            QString nickname = commandRX.cap(2);
            QString channel = commandRX.cap(3);
            return IrcCommand::createInvite(nickname, channel);
        }
        // /JOIN
        else if(commandString.compare("join", Qt::CaseInsensitive) == 0)
        {
            QString channelStr = commandRX.cap(2);
            QStringList passwords = commandRX.cap(3).trimmed().split(',');
            QStringList channels;
            int pos = 0;
            while ((pos = channelRX.indexIn(channelStr, pos)) != -1)
            {
                channels << channelRX.cap(1);
                pos += channelRX.matchedLength();
            }
            if (channels.count() > 0) {
                return IrcCommand::createJoin(channels, passwords);
            }
            // RFC says to leave all channels if 0
            // Some servers support this, some don't, so we're doing this client side
            else if (channelStr == "0") {
                getServer()->partAllChannels();
            }
        }
        // /KICK

        // /KNOCK

        // /LIST

        // /MODE

        // /MOTD

        // /MSG
        else if (commandString.compare("msg", Qt::CaseInsensitive) == 0)
        {
            QString nickname = commandRX.cap(2);
            QString msg = commandRX.cap(3).trimmed();
            Server *server = this->getServer();
            Channel *channel = server->getChannel(nickname);
            if (channel == NULL) {
                channel = server->addChannel(nickname);
            }
            QString ourName = server->getNickname();
            channel->appendText(ourName, msg);
            return IrcCommand::createMessage(nickname, msg);
        }

        // /NAMES

        // /NICK

        // /NOTICE

        // /PART
        else if (commandString.compare("part", Qt::CaseInsensitive) == 0)
        {
            QString channelStr = commandRX.cap(2);
            if (channelRX.indexIn(channelStr) > -1) {
                //TODO: Get Reason
               return IrcCommand::createPart(channelStr, NULL);
            }
        }

        // /QUIT

        // /QUOTE

        // /STATS

        // /TIME

        // /TOPIC

        // /TRACE

        // /USERS

        // /VERSION

        // /WHO

        // /WHOIS

        // /WHOWAS
    }
    return NULL;
}

Server* CommandParser::getServer() {
    return qobject_cast<Server *>(this->parent());
}
