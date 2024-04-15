
#include "bzfsAPI.h"
#include "plugin_utils.h"

#include <regex>

int fd;
bool fc;
std::regex rgx("[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}");

class ircRelay : public bz_Plugin {
    public:
        virtual const char* Name();
        virtual void Init(const char* config);
        virtual void Cleanup();
        virtual void Event(bz_EventData* eventData);
        static void Start();
        static void Stop();
        static void Worker();
};
