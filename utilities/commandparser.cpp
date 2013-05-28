#include "commandparser.h"
#include "server.h"
#include "irccommand.h"

CommandParser::CommandParser(Server *parent) : QObject(parent)
{

}

IrcCommand* CommandParser::parse(QString commandStr) {
    QRegExp commandRX("^/([a-zA-Z]+) (\\S+)");
    QRegExp channelRX("([#&][^\\x07\\x2C\\s]{,200})");
    int pos = commandRX.indexIn(commandStr);
    if(pos > -1) {
        QString commandString = commandRX.cap(1);
        // /JOIN
        if(commandString.compare("join", Qt::CaseInsensitive) == 0) {
            QString channelStr = commandRX.cap(2);
            if (channelRX.indexIn(channelStr) > -1) {
                return IrcCommand::createJoin(channelStr, NULL);
            }
            // RFC says to leave all channels if 0
            else if (channelStr == "0") {
                getServer()->partAllChannels();
                return NULL;
            }
        }
        // /PART
        else if (commandString.compare("part", Qt::CaseInsensitive) == 0) {
            QString channelStr = commandRX.cap(2);
            if (channelRX.indexIn(channelStr) > -1) {
                //TODO: Get Reason
                return IrcCommand::createPart(channelStr, NULL);
            }
        }
    }
    return NULL;
}

Server* CommandParser::getServer() {
    return qobject_cast<Server *>(this->parent());
}
