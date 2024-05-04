/*
 * Copyright (C) 2024 Dirk Sarodnick
 * All rights reserved.
 */
#include "bzfsAPI.h"
#include "plugin_utils.h"

#include <regex>

int fd;
bool fc;
unsigned int pingCount;
unsigned int retryCount;
std::regex rgx("[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}");

class ircRelay : public bz_Plugin {
    public:
        virtual const char* Name();
        virtual void Init(const char* config);
        virtual void Cleanup();
        virtual void Event(bz_EventData* eventData);

        static void Start();
        static void Stop();

        static void Receive(const char* until);
        static void Send(std::string data, int debugLevel);

        static void Wait(unsigned int seconds, unsigned int milliseconds);
        static void Worker();
};
