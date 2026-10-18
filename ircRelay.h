/*
 * Copyright (C) 2024 Dirk Sarodnick
 * All rights reserved.
 */
#include "bzfsAPI.h"
#include "plugin_utils.h"

#include <map>
#include <regex>

int fd;
bool fc;
unsigned int pingCount;
unsigned int retryCount;
std::regex rgx("[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}");
std::regex tagRgx("^\\[([^\\]]+)\\] ");
std::regex prefixRgx("^\\[([^\\]]+)\\]");
std::map<std::string, std::string> pluginConfig;

class ircRelay : public bz_Plugin {
    public:
        virtual const char* Name();
        virtual void Init(const char* config);
        virtual void Cleanup();
        virtual void Event(bz_EventData* eventData);

        static void Start();
        static void Stop();

        static void ParseConfig(const char* config);
        static void ParseConfigLine(std::string line);
        static bool HasSetting(const char* key);
        static std::string GetSetting(const char* key, const char* bzdbName);

        static void Receive(const char* until);
        static void Send(std::string data, int debugLevel, const char* logText = nullptr);
        static bool Filter(std::string& message);
        static void ReplaceUmlauts(std::string& message);

        static void Wait(unsigned int seconds, unsigned int milliseconds);
        static void Worker();
};
