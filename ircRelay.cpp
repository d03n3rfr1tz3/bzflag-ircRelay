/*
 * Copyright (C) 2024 Dirk Sarodnick
 * All rights reserved.
 */
#include "ircRelay.h"

#include "bzfsAPI.h"
#include "plugin_utils.h"

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#include <io.h>
#include <windows.h>
DWORD WINAPI PingThread(LPVOID lpParameter) { ircRelay::Ping(lpParameter); };
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

BZ_PLUGIN(ircRelay)

const char* ircRelay::Name() {
    return "IRC Relay";
}

void ircRelay::Init(const char* config) {
    bz_debugMessage(1, "Initializing ircRelay custom plugin");
    run = true;

    Register(bz_eWorldFinalized);
    Register(bz_eFilteredChatMessageEvent);
    Register(bz_ePlayerJoinEvent);
    Register(bz_ePlayerPartEvent);

    bz_registerCustomBZDBString("_ircAddress", "0.0.0.0", 0, false);
    bz_registerCustomBZDBString("_ircChannel", "", 0, false);
    bz_registerCustomBZDBString("_ircNick", "", 0, false);

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
    DWORD thread;
    CreateThread(0, 0, PingThread, NULL, 0, &thread);
#else
    pthread_t thread;
    pthread_create(&thread, NULL, ircRelay::Ping, NULL);
#endif

    bz_debugMessage(1, "Initialized ircRelay custom plugin");
}

void ircRelay::Start() {
    bz_debugMessage(1, "Starting ircRelay custom plugin");

    if (bz_BZDBItemExists("_ircAddress")) ircAddress = bz_getBZDBString("_ircAddress"); else return;
    if (bz_BZDBItemExists("_ircChannel")) ircChannel = bz_getBZDBString("_ircChannel"); else return;
    if (bz_BZDBItemExists("_ircNick")) ircNick = bz_getBZDBString("_ircNick"); else return;
    if (ircAddress == "0.0.0.0") { bz_debugMessage(1, "Starting ircRelay custom plugin skipped, because address is still 0.0.0.0"); return; }
    if (ircChannel == "") { bz_debugMessage(1, "Starting ircRelay custom plugin skipped, because channel is still empty"); return; }
    if (ircNick == "") { bz_debugMessage(1, "Starting ircRelay custom plugin skipped, because nick is still empty"); return; }
    if (fd != 0) { bz_debugMessage(1, "Starting ircRelay custom plugin skipped, because its already running"); return; }

    fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;

    dest_addr.sin_addr.s_addr = inet_addr(ircAddress.c_str());//address of irc server
    dest_addr.sin_port = htons(6667);//port of irc server

    memset(&(dest_addr.sin_zero), '\0', 8);


    if (connect(fd, (struct sockaddr*)&dest_addr, sizeof(struct sockaddr)) != 0) {
        bz_debugMessage(1, "Connecting to irc server.");
    }
    else {
        bz_debugMessage(1, "Connection to irc server failed.");
    }

    char recv_buf[1024];
    int r_len;

    std::string str1 = "NICK " + ircNick + "\r\n";
    std::string str2 = "USER ircRelay 0 * :" + ircNick + "\r\n";
    std::string str3 = "JOIN #" + ircChannel + "\r\n";

    // recieve something before sending
    r_len = read(fd, recv_buf, 1024);
    recv_buf[r_len] = '\0';
    std::string message = recv_buf;

    // send username, join channel
    write(fd, str1.c_str(), str1.size());
    bz_debugMessage(2, str1.c_str());
    write(fd, str2.c_str(), str2.size());
    bz_debugMessage(2, str2.c_str());
    write(fd, str3.c_str(), str3.size());
    bz_debugMessage(2, str3.c_str());

    bz_debugMessage(1, "Started ircRelay custom plugin");
}

void ircRelay::Stop() {
    bz_debugMessage(1, "Stopping ircRelay custom plugin");

    close(fd);
    fd = 0;

    bz_debugMessage(1, "Stopped ircRelay custom plugin");
}

void ircRelay::Cleanup() {
    bz_debugMessage(1, "Cleaning ircRelay custom plugin");
    run = false;

    Stop();
    Flush();
    bz_removeCustomBZDBVariable("_ircAddress");
    bz_removeCustomBZDBVariable("_ircChannel");
    bz_removeCustomBZDBVariable("_ircNick");

    bz_debugMessage(1, "Cleaned ircRelay custom plugin");
}

void ircRelay::Event(bz_EventData* eventData) {
    switch (eventData->eventType) {
        case bz_eWorldFinalized: {
            // This event is called when the world is done loading. This event contains no data and is only used for notification purposes.
            Start();
        }
        break;
        case bz_eFilteredChatMessageEvent: {
            // This event is called for each chat message the server receives; after the server or any plug-ins have done filtering
            bz_ChatEventData_V2* data = (bz_ChatEventData_V2*)eventData;

            // Data
            // ----
            // (int)             from        - The player ID sending the message.
            // (int)             to          - The player ID that the message is to if the message is to an individual, or a broadcast. If the message is a broadcast the id will be `BZ_ALLUSERS`.
            // (bz_eTeamType)    team        - The team the message is for if it not for an individual or a broadcast. If it is not a team message the team will be eNoTeam.
            // (bz_ApiString)    message     - The filtered final text of the message.
            // (bz_eMessageType) messageType - The type of message being sent.
            // (double)          eventTime   - The time of the event.

            bz_BasePlayerRecord* speaker = bz_getPlayerByIndex(data->from);
            if (speaker != NULL) {
                std::string message = data->message.c_str(); //um... yeah
                std::string player = speaker->callsign.c_str();
                int team = speaker->team;
                std::string colorcode;

                if (message.substr(0, 1) != "/") {//no slash commands
                    if (team == 0)
                        colorcode = "\00307"; // rogue

                    else if (team == 1)
                        colorcode = "\00304"; // red team

                    else if (team == 2)
                        colorcode = "\00303"; // green team

                    else if (team == 3)
                        colorcode = "\00302"; // blue team

                    else if (team == 4)
                        colorcode = "\00306"; // purple team

                    else if (team == 5)
                        colorcode = "\00310"; // observer

                    else if (team == 6)
                        colorcode = "\00314"; // rabbit

                    if (message != "bzadminping") {// if message is not a bzadminping
                        std::string subtotal = colorcode + player + ":  " + "\017" + message;
                        std::string total = "PRIVMSG #" + ircChannel + " :" + subtotal + "\r\n";
                        write(fd, total.c_str(), strlen(total.c_str()));
                        bz_debugMessage(2, total.c_str());
                        break;
                    }
                }
            }
        }
        break;

        case bz_ePlayerJoinEvent: {
            // This event is called each time a player joins the game
            bz_PlayerJoinPartEventData_V1* data = (bz_PlayerJoinPartEventData_V1*)eventData;

            // Data
            // ----
            // (int)                  playerID  - The player ID that is joining
            // (bz_BasePlayerRecord*) record    - The player record for the joining player
            // (double)               eventTime - Time of event.

            bz_BasePlayerRecord* joiner = data->record;
            if (joiner != NULL) {
                std::string callsign = joiner->callsign.c_str();
                std::string ip = joiner->ipAddress.c_str();
                int team = joiner->team;
                std::string colorcode;
                std::string player_team;

                if (team == 0) {
                    colorcode = "\00307"; // rogue
                    player_team = "rogue";
                }
                else if (team == 1) {
                    colorcode = "\00304"; // red team
                    player_team = "red player";
                }
                else if (team == 2) {
                    colorcode = "\00303"; // green team
                    player_team = "green player";
                }
                else if (team == 3) {
                    colorcode = "\00302"; // blue team
                    player_team = "blue player";
                }
                else if (team == 4) {
                    colorcode = "\00306"; // purple team
                    player_team = "purple player";
                }
                else if (team == 5) {
                    colorcode = "\00310"; // observer
                    player_team = "observer";
                }
                else if (team == 6) {
                    colorcode = "\00314"; // rabbit
                    player_team = "rabbit";
                }
                else {
                    colorcode = "\017";
                }
                if (callsign != "") {//send it only it is not a list server ping
                    std::string subtotal = colorcode + callsign + "\017" + " joined as a " + player_team + " from " + ip;

                    if (player_team == "observer") {
                        subtotal = colorcode + callsign + "\017" + " joined as a " + player_team + " from " + ip;
                    }

                    std::string total = "PRIVMSG #" + ircChannel + " :" + subtotal + "\r\n";
                    write(fd, total.c_str(), strlen(total.c_str()));
                    bz_debugMessage(2, total.c_str());
                    break;
                }
            }
        }
        break;

        case bz_ePlayerPartEvent: {
            // This event is called each time a player leaves a game
            bz_PlayerJoinPartEventData_V1* data = (bz_PlayerJoinPartEventData_V1*)eventData;

            // Data
            // ----
            // (int)                  playerID  - The player ID that is leaving
            // (bz_BasePlayerRecord*) record    - The player record for the leaving player
            // (bz_ApiString)         reason    - The reason for leaving, such as a kick or a ban
            // (double)               eventTime - Time of event.

            bz_BasePlayerRecord* leaver = data->record;
            if (leaver != NULL) {
                std::string callsign = leaver->callsign.c_str();
                int team = leaver->team;
                std::string colorcode;

                if (team == 0)
                    colorcode = "\00307"; // rogue

                else if (team == 1)
                    colorcode = "\00304"; // red team

                else if (team == 2)
                    colorcode = "\00303"; // green team

                else if (team == 3)
                    colorcode = "\00302"; // blue team

                else if (team == 4)
                    colorcode = "\00306"; // purple team

                else if (team == 5)
                    colorcode = "\00310"; // observer

                else if (team == 6)
                    colorcode = "\00314"; // rabbit

                else
                    colorcode = "\017";

                if (callsign != "") {//send it only it is not a list server ping
                    std::string subtotal = colorcode + callsign + "\017" + " left the game";
                    std::string total = "PRIVMSG #" + ircChannel + " :" + subtotal + "\r\n";
                    write(fd, total.c_str(), strlen(total.c_str()));
                    bz_debugMessage(2, total.c_str());
                    break;
                }
            }
        }
        break;

        default:
            break;
    }
}

void* ircRelay::Ping(void* t) {
    while (run) {
        if (fd == 0) {
            ircRelay::Start();
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
            Sleep(1000);
#else
            sleep(1000);
#endif
            continue;
        }

        char recv_buf[1025];
        int r_len = read(fd, recv_buf, 1024);
        recv_buf[r_len] = '\0';

        std::string data = recv_buf;

        if (data.find("PRIVMSG", 0) != std::string::npos) { // send only privmsgs
            std::string::size_type pos = data.find(":", 0);
            std::string::size_type endpos = data.find("!", 0);
            std::string username = data.substr(pos + 1, endpos - 1);

            pos = data.find(":", 1); // find the other :
            endpos = data.size();
            std::string message = data.substr(pos + 1, endpos);

            std::string total = username + ": " + message;

            bz_sendTextMessage(BZ_SERVER, BZ_ALLUSERS, total.c_str());
            bz_debugMessage(2, total.c_str());
        }

        if (data.substr(0, 4) == "PING") {
            std::string ping = data;
            std::string pongdata = ping.substr(5, ping.size());

            std::string pong = "PONG " + pongdata + "\n";

            write(fd, pong.c_str(), pong.size());
        }
    }
}
