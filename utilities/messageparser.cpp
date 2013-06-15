#include "messageparser.h"
#include "server.h"
#include "ircmessage.h"
#include "channel.h"
#include "irc.h"
#include "irccommand.h"
#include "session.h"
#include "user.h"

#include <QMessageBox>

MessageParser::MessageParser(Server *parent) : QObject(parent)
{

}

void MessageParser::parse(IrcMessage *message)
{
    bool valid = message->isValid();
    IrcMessage::Type type = message->type();
    IrcMessage::Flags flags = message->flags();
    bool senderIsSelf = (flags & IrcMessage::Own);
    QString sender = message->sender().name();

    Server *server = this->getServer();
    QString currentNickname = server->getNickname();

    switch (type) {
        case IrcMessage::Invite: {
            IrcInviteMessage *invite = static_cast<IrcInviteMessage*>(message);
            QString user = invite->user();
            QString channel = invite->channel();

            QMessageBox msgBox;
            msgBox.setText(QString("%1 invited you to %2").arg(sender, channel));
            msgBox.setInformativeText("Do you want to join the channel?");
            msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
            msgBox.setDefaultButton(QMessageBox::Yes);
            msgBox.setButtonText(QMessageBox::Yes, "Join");
            msgBox.setFixedSize(360,180);
            int choice = msgBox.exec();

            switch (choice)
            {
                case QMessageBox::Yes:
                    // Join Channel
                     server->sendCommand(IrcCommand::createJoin(channel, NULL));
                    break;
                case QMessageBox::No:
                default:
                    // Do Nothing
                    break;
            }
            break;
        }

        case IrcMessage::Join: {
            IrcJoinMessage *join = static_cast<IrcJoinMessage*>(message);
            QString targetChannel = join->channel();
            if (senderIsSelf) {
                // Current user joined
                Channel *channel = this->getChannel(targetChannel);
                if(!channel) {
                    channel = server->addChannel(targetChannel, Channel::ChannelTypeNormal);
                } else {
                    channel->appendText(QString("You have joined %1").arg(targetChannel));
                }
                channel->setIsJoined(true);
            } else {
                // Another user joined
                Channel *channel = this->getChannel(targetChannel);
                channel->addUser(sender, QChar::Null);
                channel->appendText(QString("%1 has joined %2").arg(sender, targetChannel));
            }
            break;
        }

        case IrcMessage::Kick: {
            IrcKickMessage *kick = static_cast<IrcKickMessage*>(message);
            QString channel = kick->channel();
            QString user = kick->user();
            QString reason = kick->reason();
            Channel *targetChannel = server->getChannel(channel);
            if(user.compare(currentNickname, Qt::CaseInsensitive) == 0) {
                // We got kicked
                targetChannel->setIsJoined(false);
                targetChannel->appendText(QString("You have been kicked from %1 by %2 (Reason: %3)").arg(channel, user, reason));
            } else {
                // Someone else got kicked
                QString kickNote = QString("Kicked by %1 (Reason: %2)").arg(sender, reason);
                targetChannel->removeUser(user, kickNote);
            }
            break;
        }

        case IrcMessage::Mode: {
            IrcModeMessage *mode = static_cast<IrcModeMessage*>(message);
            QString target = mode->target();
            QString modeFlag = mode->mode();
            QString argument = mode->argument();

            Channel *channel = server->getChannel(target);
            if(channel) {
                if(argument.length() == 0) {
                    // Flag for a channel
                    // TODO:  Set flags on the channel object
                } else  {
                    // Flags for a user in a channel
                    User *user = channel->getUser(argument);
                    if (modeFlag.startsWith('-', Qt::CaseInsensitive))
                    {
                        user->removeMode(modeFlag.at(1));
                    }
                    else if (modeFlag.startsWith("+", Qt::CaseInsensitive))
                    {
                        user->addMode(modeFlag.at(1));
                    }
                    channel->appendText(QString("%1 sets mode: %2 %3").arg(sender, modeFlag, argument));
                }
            } else if(target.compare(currentNickname) == 0) {
                // System wide flags about ourself
                // TODO:  Set flags about ourself on the server
            }
            break;
        }

        case IrcMessage::Nick: {
            IrcNickMessage *nick = static_cast<IrcNickMessage*>(message);
            QString newNick = nick->nick();
            if(senderIsSelf) {
                // We changed nicks
                // This is handled by nickNameChanged signal
            } else {
                // Someone else changed nicks
                Server *server = this->getServer();
                server->renameUserInAllChannels(sender, newNick);
            }
            break;
        }

        case IrcMessage::Notice: {
            IrcNoticeMessage *notice = static_cast<IrcNoticeMessage*>(message);
            QString message = notice->message();
            QString target = notice->target();
            QString styledMessage = this->styleString(message);
            Channel *channel = this->getChannel(target);
            Channel *senderChannel = this->getChannel(sender);
            if(channel != NULL) {
                // Notice about channel
                channel->appendText(QString("Notice: %1").arg(styledMessage));
            } else if(senderChannel != NULL) {
                // Notice from service
                senderChannel->appendText(sender, styledMessage);
            } else if(target.compare(currentNickname, Qt::CaseInsensitive) == 0) {
                // Notice to self
                server->appendText(QString("Notice: %1").arg(styledMessage));
            } else {
                // Some other notice
                server->appendText(QString("%1 Notice: %2").arg(target, styledMessage));
            }
            break;
        }

        case IrcMessage::Numeric: {
            IrcNumericMessage *numeric = static_cast<IrcNumericMessage*>(message);
            QString parsedNumeric = this->parseNumeric(numeric);
            if(!parsedNumeric.isEmpty()) {
                Server *server = this->getServer();
                server->appendText(parsedNumeric);
            }
            break;
        }

        case IrcMessage::Part: {
            IrcPartMessage *part = static_cast<IrcPartMessage*>(message);
            QString targetChannel = part->channel();
            QString reason = part->reason();
            Channel *channel = this->getChannel(targetChannel);
            if(senderIsSelf) {
                // Current user left
                channel->setIsJoined(false);
                channel->appendText(QString("You have left %1").arg(targetChannel));
            } else {
                // Other user left
                channel->removeUser(sender, reason);
            }
            break;
        }

        case IrcMessage::Pong: {
            IrcPongMessage *pong = static_cast<IrcPongMessage*>(message);
            QString argument = pong->argument();
            break;
        }

        case IrcMessage::Ping: {
            IrcPingMessage *pong = static_cast<IrcPingMessage*>(message);
            QString argument = pong->argument();
            break;
        }

        case IrcMessage::Private: {
            // Messages from both channels and PMs from other users
            IrcPrivateMessage *pm = static_cast<IrcPrivateMessage*>(message);
            QString messageTarget = pm->target();
            QString styledMessage = this->styleString(pm->message());
            Channel *channel = this->getChannel(messageTarget);
            if(channel == NULL) {
                // Probably from another user, create a 'channel' for them
                channel = this->getChannel(sender);
                if(channel == NULL) {
                    channel = server->addChannel(sender, Channel::ChannelTypeUser);
                }
            }
            Channel::MessageType messageType = pm->isAction() ? Channel::MessageTypeEmote : Channel::MessageTypeDefault;
            channel->appendText(sender, styledMessage, messageType);
            break;
        }

        case IrcMessage::Quit: {
            IrcQuitMessage *quit = static_cast<IrcQuitMessage*>(message);
            QString reason = quit->reason();
            if (senderIsSelf) {
                // We quit
                Server *server = getServer();
                QString serverName = server->getHost();
                Session *session = server->getSession();
                session->removeServer(serverName);
            }
            else {
                // Other user quit
                server->removeUserFromAllChannels(sender, reason);
            }
            break;
        }

        case IrcMessage::Topic: {
            IrcTopicMessage *topic = static_cast<IrcTopicMessage*>(message);
            QString targetChannel = topic->channel();
            QString topicMsg = topic->topic();
            Channel *channel = this->getChannel(targetChannel);
            channel->appendText("Channel Topic", topicMsg, Channel::MessageTypeTopic);
            break;
        }

        case IrcMessage::Capability: {
            IrcCapabilityMessage *capability = static_cast<IrcCapabilityMessage*>(message);
            QString subCommand = capability->subCommand();
            QStringList capabilities = capability->capabilities();
            break;
        }

        case IrcMessage::Error: {
            IrcErrorMessage *error = static_cast<IrcErrorMessage*>(message);
            Server *server = this->getServer();
            QString styledError = this->styleString(error->error());
            server->appendText(QString("**ERROR: %1").arg(styledError));
            // TODO: Show alert?  Reconnect?
            break;
        }

        case IrcMessage::Unknown: {
            break;
        }

        default: {
            break;
        }
    }
}

QString MessageParser::parseNumeric(IrcNumericMessage *message)
{
    QString formattedMessage = "";
    QStringList p = message->parameters();
    int code = message->code();
    switch (code) {
        case Irc::RPL_NAMREPLY_: { }
        case Irc::RPL_NAMREPLY: {
            QString targetChannel = p.value(2);
            QStringList users = p.value(3).split(" ");
            Channel *channel = this->getChannel(targetChannel);
            channel->addUsers(users);
            break;
        }
        case Irc::RPL_ENDOFNAMES: {
            break; // We don't care to see this
        }
        case Irc::RPL_TOPIC: {
            QString channel = p.value(1);
            QString topic = p.value(2);
            QString styledString = styleString(topic);
            this->getChannel(channel)->appendText("Channel Topic", styledString, Channel::MessageTypeTopic);
            break;
        }
        case Irc::RPL_TOPICWHOTIME: {
            break; // We don't care to see this
        }
        case Irc::RPL_WELCOME: { }
        case Irc::RPL_YOURHOST: { }
        case Irc::RPL_CREATED: { }
        case Irc::RPL_MYINFO: { }
        case Irc::RPL_ISUPPORT: { }
        case Irc::RPL_SNOMASK: { }
        case Irc::RPL_STATMEMTOT: { }
        case Irc::RPL_BOUNCE: { }
        case Irc::RPL_YOURCOOKIE: { }
        case Irc::RPL_YOURID: { }
        case Irc::RPL_SAVENICK: { }
        case Irc::RPL_ATTEMPTINGJUNC: { }
        case Irc::RPL_ATTEMPTINGREROUTE: { }
        case Irc::RPL_TRACELINK: { }
        case Irc::RPL_TRACECONNECTING: { }
        case Irc::RPL_TRACEHANDSHAKE: { }
        case Irc::RPL_TRACEUNKNOWN: { }
        case Irc::RPL_TRACEOPERATOR: { }
        case Irc::RPL_TRACEUSER: { }
        case Irc::RPL_TRACESERVER: { }
        case Irc::RPL_TRACESERVICE: { }
        case Irc::RPL_TRACENEWTYPE: { }
        case Irc::RPL_TRACECLASS: { }
        case Irc::RPL_TRACERECONNECT: { }
        case Irc::RPL_STATSLINKINFO: { }
        case Irc::RPL_STATSCOMMANDS: { }
        case Irc::RPL_STATSCLINE: { }
        case Irc::RPL_STATSNLINE: { }
        case Irc::RPL_STATSILINE: { }
        case Irc::RPL_STATSKLINE: { }
        case Irc::RPL_STATSQLINE: { }
        case Irc::RPL_STATSYLINE: { }
        case Irc::RPL_ENDOFSTATS: { }
        case Irc::RPL_UMODEIS: { }
        case Irc::RPL_MODLIST: { }
        case Irc::RPL_STATSZLINE: { }
        case Irc::RPL_STATSCOUNT: { }
        case Irc::RPL_SERVICEINFO: { }
        case Irc::RPL_ENDOFSERVICES: { }
        case Irc::RPL_SERVICE: { }
        case Irc::RPL_SERVLIST: { }
        case Irc::RPL_SERVLISTEND: { }
        case Irc::RPL_STATSVERBOSE: { }
        case Irc::RPL_STATSENGINE: { }
        case Irc::RPL_STATSIAUTH: { }
        case Irc::RPL_STATSVLINE: { }
        case Irc::RPL_STATSLLINE: { }
        case Irc::RPL_STATSUPTIME: { }
        case Irc::RPL_STATSOLINE: { }
        case Irc::RPL_STATSHLINE: { }
        case Irc::RPL_STATSSLINE: { }
        case Irc::RPL_STATSPING: { }
        case Irc::RPL_STATSBLINE: { }
        case Irc::RPL_STATSDEFINE: { }
        case Irc::RPL_STATSDEBUG: { }
        case Irc::RPL_STATSDLINE: { }
        case Irc::RPL_LUSERCLIENT: { }
        case Irc::RPL_LUSEROP: { }
        case Irc::RPL_LUSERUNKNOWN: { }
        case Irc::RPL_LUSERCHANNELS: { }
        case Irc::RPL_LUSERME: { }
        case Irc::RPL_ADMINME: { }
        case Irc::RPL_ADMINLOC1: { }
        case Irc::RPL_ADMINLOC2: { }
        case Irc::RPL_ADMINEMAIL: { }
        case Irc::RPL_TRACELOG: { }
        case Irc::RPL_TRACEPING: { }
        case Irc::RPL_TRYAGAIN: { }
        case Irc::RPL_LOCALUSERS: { }
        case Irc::RPL_GLOBALUSERS: { }
        case Irc::RPL_START_NETSTAT: { }
        case Irc::RPL_NETSTAT: { }
        case Irc::RPL_END_NETSTAT: { }
        case Irc::RPL_PRIVS: { }
        case Irc::RPL_SILELIST: { }
        case Irc::RPL_ENDOFSILELIST: { }
        case Irc::RPL_NOTIFY: { }
        case Irc::RPL_ENDNOTIFY: { }
        case Irc::RPL_VCHANEXIST: { }
        case Irc::RPL_VCHANLIST: { }
        case Irc::RPL_VCHANHELP: { }
        case Irc::RPL_GLIST: { }
        case Irc::RPL_ENDOFGLIST: { }
        case Irc::RPL_ENDOFACCEPT: { }
        case Irc::RPL_ENDOFJUPELIST: { }
        case Irc::RPL_FEATURE: { }
        case Irc::RPL_GLIST_HASH: { }
        case Irc::RPL_CHANINFO_USERS: { }
        case Irc::RPL_CHANINFO_CHOPS: { }
        case Irc::RPL_CHANINFO_VOICES: { }
        case Irc::RPL_CHANINFO_AWAY: { }
        case Irc::RPL_CHANINFO_OPERS: { }
        case Irc::RPL_CHANINFO_BANNED: { }
        case Irc::RPL_CHANINFO_BANS: { }
        case Irc::RPL_CHANINFO_INVITE: { }
        case Irc::RPL_CHANINFO_INVITES: { }
        case Irc::RPL_CHANINFO_KICK: { }
        case Irc::RPL_CHANINFO_KICKS: { }
        case Irc::RPL_END_CHANINFO: { }
        case Irc::RPL_NONE: { }
        case Irc::RPL_AWAY: { }
        case Irc::RPL_USERHOST: { }
        case Irc::RPL_ISON: { }
        case Irc::RPL_TEXT: { }
        case Irc::RPL_UNAWAY: { }
        case Irc::RPL_NOWAWAY: { }
        case Irc::RPL_WHOISREGNICK: { }
        case Irc::RPL_NOTIFYACTION: { }
        case Irc::RPL_NICKTRACE: { }
        case Irc::RPL_WHOISSVCMSG: { }
        case Irc::RPL_WHOISUSER: { }
        case Irc::RPL_WHOISSERVER: { }
        case Irc::RPL_WHOISOPERATOR: { }
        case Irc::RPL_WHOWASUSER: { }
        case Irc::RPL_ENDOFWHO: { }
        case Irc::RPL_WHOISCHANOP: { }
        case Irc::RPL_WHOISIDLE: { }
        case Irc::RPL_ENDOFWHOIS: { }
        case Irc::RPL_WHOISCHANNELS: { }
        case Irc::RPL_WHOISVIRT: { }
        case Irc::RPL_LISTSTART: { }
        case Irc::RPL_LIST: { }
        case Irc::RPL_LISTEND: { }
        case Irc::RPL_CHANNELMODEIS: { }
        case Irc::RPL_UNIQOPIS: { }
        case Irc::RPL_NOCHANPASS: { }
        case Irc::RPL_CHPASSUNKNOWN: { }
        case Irc::RPL_CHANNEL_URL: { }
        case Irc::RPL_CREATIONTIME: { }
        case Irc::RPL_WHOWAS_TIME: { }
        case Irc::RPL_NOTOPIC: { }        
        case Irc::RPL_LISTUSAGE: { }
        case Irc::RPL_CHANPASSOK: { }
        case Irc::RPL_BADCHANPASS: { }
        case Irc::RPL_INVITING: { }
        case Irc::RPL_SUMMONING: { }
        case Irc::RPL_INVITED: { }
        case Irc::RPL_INVITELIST: { }
        case Irc::RPL_ENDOFINVITELIST: { }
        case Irc::RPL_EXCEPTLIST: { }
        case Irc::RPL_ENDOFEXCEPTLIST: { }
        case Irc::RPL_VERSION: { }
        case Irc::RPL_WHOREPLY: { }
        case Irc::RPL_WHOSPCRPL: { }
        case Irc::RPL_KILLDONE: { }
        case Irc::RPL_CLOSING: { }
        case Irc::RPL_CLOSEEND: { }
        case Irc::RPL_LINKS: { }
        case Irc::RPL_ENDOFLINKS: { }
        case Irc::RPL_BANLIST: { }
        case Irc::RPL_ENDOFBANLIST: { }
        case Irc::RPL_ENDOFWHOWAS: { }
        case Irc::RPL_INFO: { }
        case Irc::RPL_MOTD: { }
        case Irc::RPL_INFOSTART: { }
        case Irc::RPL_ENDOFINFO: { }
        case Irc::RPL_MOTDSTART: { }
        case Irc::RPL_ENDOFMOTD: { }
        case Irc::RPL_KICKEXPIRED: { }
        case Irc::RPL_BANEXPIRED: { }
        case Irc::RPL_KICKLINKED: { }
        case Irc::RPL_BANLINKED: { }
        case Irc::RPL_YOUREOPER: { }
        case Irc::RPL_REHASHING: { }
        case Irc::RPL_YOURESERVICE: { }
        case Irc::RPL_MYPORTIS: { }
        case Irc::RPL_NOTOPERANYMORE: { }
        case Irc::RPL_QLIST: { }
        case Irc::RPL_ENDOFQLIST: { }
        case Irc::RPL_ALIST: { }
        case Irc::RPL_ENDOFALIST: { }
        case Irc::RPL_TIME: { }
        case Irc::RPL_USERSSTART: { }
        case Irc::RPL_USERS: { }
        case Irc::RPL_ENDOFUSERS: { }
        case Irc::RPL_NOUSERS: { }
        case Irc::RPL_HOSTHIDDEN: { }
        case Irc::ERR_UNKNOWNERROR: { }
        case Irc::ERR_NOSUCHNICK: { }
        case Irc::ERR_NOSUCHSERVER: { }
        case Irc::ERR_NOSUCHCHANNEL: { }
        case Irc::ERR_CANNOTSENDTOCHAN: { }
        case Irc::ERR_TOOMANYCHANNELS: { }
        case Irc::ERR_WASNOSUCHNICK: { }
        case Irc::ERR_TOOMANYTARGETS: { }
        case Irc::ERR_NOSUCHSERVICE: { }
        case Irc::ERR_NOORIGIN: { }
        case Irc::ERR_NORECIPIENT: { }
        case Irc::ERR_NOTEXTTOSEND: { }
        case Irc::ERR_NOTOPLEVEL: { }
        case Irc::ERR_WILDTOPLEVEL: { }
        case Irc::ERR_BADMASK: { }
        case Irc::ERR_TOOMANYMATCHES: { }
        case Irc::ERR_LENGTHTRUNCATED: { }
        case Irc::ERR_UNKNOWNCOMMAND: { }
        case Irc::ERR_NOMOTD: { }
        case Irc::ERR_NOADMININFO: { }
        case Irc::ERR_FILEERROR: { }
        case Irc::ERR_NOOPERMOTD: { }
        case Irc::ERR_TOOMANYAWAY: { }
        case Irc::ERR_EVENTNICKCHANGE: { }
        case Irc::ERR_NONICKNAMEGIVEN: { }
        case Irc::ERR_ERRONEUSNICKNAME: { }
        case Irc::ERR_NICKNAMEINUSE: { }
        case Irc::ERR_SERVICENAMEINUSE: { }
        case Irc::ERR_SERVICECONFUSED: { }
        case Irc::ERR_NICKCOLLISION: { }
        case Irc::ERR_UNAVAILRESOURCE: { }
        case Irc::ERR_NICKTOOFAST: { }
        case Irc::ERR_TARGETTOOFAST: { }
        case Irc::ERR_SERVICESDOWN: { }
        case Irc::ERR_USERNOTINCHANNEL: { }
        case Irc::ERR_NOTONCHANNEL: { }
        case Irc::ERR_USERONCHANNEL: { }
        case Irc::ERR_NOLOGIN: { }
        case Irc::ERR_SUMMONDISABLED: { }
        case Irc::ERR_USERSDISABLED: { }
        case Irc::ERR_NONICKCHANGE: { }
        case Irc::ERR_NOTIMPLEMENTED: { }
        case Irc::ERR_NOTREGISTERED: { }
        case Irc::ERR_IDCOLLISION: { }
        case Irc::ERR_NICKLOST: { }
        case Irc::ERR_HOSTILENAME: { }
        case Irc::ERR_ACCEPTFULL: { }
        case Irc::ERR_ACCEPTEXIST: { }
        case Irc::ERR_ACCEPTNOT: { }
        case Irc::ERR_NOHIDING: { }
        case Irc::ERR_NOTFORHALFOPS: { }
        case Irc::ERR_NEEDMOREPARAMS: { }
        case Irc::ERR_ALREADYREGISTERED: { }
        case Irc::ERR_NOPERMFORHOST: { }
        case Irc::ERR_PASSWDMISMATCH: { }
        case Irc::ERR_YOUREBANNEDCREEP: { }
        case Irc::ERR_YOUWILLBEBANNED: { }
        case Irc::ERR_KEYSET: { }
        case Irc::ERR_INVALIDUSERNAME: { }
        case Irc::ERR_LINKSET: { }
        case Irc::ERR_LINKCHANNEL: { }
        case Irc::ERR_CHANNELISFULL: { }
        case Irc::ERR_UNKNOWNMODE: { }
        case Irc::ERR_INVITEONLYCHAN: { }
        case Irc::ERR_BANNEDFROMCHAN: { }
        case Irc::ERR_BADCHANNELKEY: { }
        case Irc::ERR_BADCHANMASK: { }
        case Irc::ERR_NOCHANMODES: { }
        case Irc::ERR_BANLISTFULL: { }
        case Irc::ERR_BADCHANNAME: { }
        case Irc::ERR_NOULINE: { }
        case Irc::ERR_NOPRIVILEGES: { }
        case Irc::ERR_CHANOPRIVSNEEDED: { }
        case Irc::ERR_CANTKILLSERVER: { }
        case Irc::ERR_RESTRICTED: { }
        case Irc::ERR_UNIQOPRIVSNEEDED: { }
        case Irc::ERR_NONONREG: { }
        case Irc::ERR_CHANTOORECENT: { }
        case Irc::ERR_TSLESSCHAN: { }
        case Irc::ERR_VOICENEEDED: { }
        case Irc::ERR_NOOPERHOST: { }
        case Irc::ERR_NOSERVICEHOST: { }
        case Irc::ERR_NOFEATURE: { }
        case Irc::ERR_BADFEATURE: { }
        case Irc::ERR_BADLOGTYPE: { }
        case Irc::ERR_BADLOGSYS: { }
        case Irc::ERR_BADLOGVALUE: { }
        case Irc::ERR_ISOPERLCHAN: { }
        case Irc::ERR_CHANOWNPRIVNEEDED: { }
        case Irc::ERR_UMODEUNKNOWNFLAG: { }
        case Irc::ERR_USERSDONTMATCH: { }
        case Irc::ERR_GHOSTEDCLIENT: { }
        case Irc::ERR_USERNOTONSERV: { }
        case Irc::ERR_SILELISTFULL: { }
        case Irc::ERR_TOOMANYWATCH: { }
        case Irc::ERR_BADPING: { }
        case Irc::ERR_INVALID_ERROR: { }
        case Irc::ERR_BADEXPIRE: { }
        case Irc::ERR_DONTCHEAT: { }
        case Irc::ERR_DISABLED: { }
        case Irc::ERR_NOINVITE: { }
        case Irc::ERR_ADMONLY: { }
        case Irc::ERR_OPERONLY: { }
        case Irc::ERR_LISTSYNTAX: { }
        case Irc::ERR_WHOSYNTAX: { }
        case Irc::ERR_WHOLIMEXCEED: { }
        case Irc::ERR_QUARANTINED: { }
        case Irc::ERR_REMOTEPFX: { }
        case Irc::ERR_PFXUNROUTABLE: { }
        case Irc::ERR_BADHOSTMASK: { }
        case Irc::ERR_HOSTUNAVAIL: { }
        case Irc::ERR_USINGSLINE: { }
        case Irc::ERR_STATSSLINE: { }
        case Irc::RPL_LOGON: { }
        case Irc::RPL_LOGOFF: { }
        case Irc::RPL_WATCHOFF: { }
        case Irc::RPL_WATCHSTAT: { }
        case Irc::RPL_NOWON: { }
        case Irc::RPL_NOWOFF: { }
        case Irc::RPL_WATCHLIST: { }
        case Irc::RPL_ENDOFWATCHLIST: { }
        case Irc::RPL_WATCHCLEAR: { }
        case Irc::RPL_ISOPER: { }
        case Irc::RPL_ISLOCOP: { }
        case Irc::RPL_ISNOTOPER: { }
        case Irc::RPL_ENDOFISOPER: { }
        case Irc::RPL_DCCSTATUS: { }
        case Irc::RPL_DCCLIST: { }
        case Irc::RPL_ENDOFDCCLIST: { }
        case Irc::RPL_DCCINFO: { }
        case Irc::RPL_ENDOFO: { }
        case Irc::RPL_SETTINGS: { }
        case Irc::RPL_ENDOFSETTINGS: { }
        case Irc::RPL_DUMPING: { }
        case Irc::RPL_DUMPRPL: { }
        case Irc::RPL_EODUMP: { }
        case Irc::RPL_TRACEROUTE_HOP: { }
        case Irc::RPL_TRACEROUTE_START: { }
        case Irc::RPL_MODECHANGEWARN: { }
        case Irc::RPL_CHANREDIR: { }
        case Irc::RPL_SERVMODEIS: { }
        case Irc::RPL_OTHERUMODEIS: { }
        case Irc::RPL_ENDOF_GENERIC: { }
        case Irc::RPL_WHOWASDETAILS: { }
        case Irc::RPL_WHOISSECURE: { }
        case Irc::RPL_UNKNOWNMODES: { }
        case Irc::RPL_CANNOTSETMODES: { }
        case Irc::RPL_LUSERSTAFF: { }
        case Irc::RPL_TIMEONSERVERIS: { }
        case Irc::RPL_NETWORKS: { }
        case Irc::RPL_YOURLANGUAGEIS: { }
        case Irc::RPL_LANGUAGE: { }
        case Irc::RPL_WHOISSTAFF: { }
        case Irc::RPL_WHOISLANGUAGE: { }
        case Irc::RPL_HELPSTART: { }
        case Irc::RPL_HELPTXT: { }
        case Irc::RPL_ENDOFHELP: { }
        case Irc::RPL_ETRACEFULL: { }
        case Irc::RPL_ETRACE: { }
        case Irc::RPL_KNOCK: { }
        case Irc::RPL_KNOCKDLVR: { }
        case Irc::ERR_TOOMANYKNOCK: { }
        case Irc::ERR_CHANOPEN: { }
        case Irc::ERR_KNOCKONCHAN: { }
        case Irc::ERR_KNOCKDISABLED: { }
        case Irc::RPL_TARGUMODEG: { }
        case Irc::RPL_TARGNOTIFY: { }
        case Irc::RPL_UMODEGMSG: { }
        case Irc::RPL_ENDOFOMOTD: { }
        case Irc::ERR_NOPRIVS: { }
        case Irc::RPL_TESTMARK: { }
        case Irc::RPL_TESTLINE: { }
        case Irc::RPL_NOTESTLINE: { }
        case Irc::RPL_XINFO: { }
        case Irc::RPL_XINFOSTART: { }
        case Irc::RPL_XINFOEND: { }
        case Irc::ERR_CANNOTDOCOMMAND: { }
        case Irc::ERR_CANNOTCHANGEUMODE: { }
        case Irc::ERR_CANNOTCHANGECHANMODE: { }
        case Irc::ERR_CANNOTCHANGESERVERMODE: { }
        case Irc::ERR_CANNOTSENDTONICK: { }
        case Irc::ERR_UNKNOWNSERVERMODE: { }
        case Irc::ERR_SERVERMODELOCK: { }
        case Irc::ERR_BADCHARENCODING: { }
        case Irc::ERR_TOOMANYLANGUAGES: { }
        case Irc::ERR_NOLANGUAGE: { }
        case Irc::ERR_TEXTTOOSHORT: { }
        case Irc::ERR_NUMERIC_ERR: { }
        default:
            formattedMessage = p.join(" ");
            QString codeString = QString::number(code);

            // Remove your name if it's at the beginning of the string
            Server *server = this->getServer();
            QString username = server->getUsername();
            QRegExp usernameRX("^(" + username + ")(.*)");
            int pos = usernameRX.indexIn(formattedMessage);
            if(pos > -1) {
                formattedMessage = usernameRX.cap(2);
            }

            // Strip html, make links, etc
            formattedMessage = this->styleString(formattedMessage);

            // Prepend message code for debugging
            formattedMessage = QString("[%1] %2").arg(codeString, formattedMessage);

            break;
    }

    return formattedMessage;
}

QString MessageParser::styleString(QString fullMessage) {
    // Sanitize input
    fullMessage = fullMessage.toHtmlEscaped();

    // Surround links with anchor tags
    QRegExp urlRegex("((?:[a-z][\\w-]+:(?:/{1,3}|[a-z0-9%])|www\\d{0,3}[.]|[a-z0-9.\\-]+[.][a-z]{2,4}/)(?:[^\\s()<>]+|\\(([^\\s()<>]+|(\\([^\\s()<>]+\\)))*\\))+(?:\\(([^\\s()<>]+|(\\([^\\s()<>]+\\)))*\\)|[^\\s`!()\\[\\]{};:'\".,<>?«»“”‘’]))");
    fullMessage.replace(urlRegex, "<a href=\"\\1\">\\1</a>");

    // Bold instances of your name
    Server *server = this->getServer();
    QString username = server->getUsername();
    QRegExp usernameRX("(" + username + ")");
    fullMessage.replace(usernameRX, "<b>\\1</b>");

    return fullMessage;
}

Session* MessageParser::getSession() {
   Server *server = this->getServer();
   return server->getSession();
}

Server* MessageParser::getServer() {
    return qobject_cast<Server *>(this->parent());
}

Channel* MessageParser::getChannel(QString name) {
    Server *server = this->getServer();
    return server->getChannel(name);
}
