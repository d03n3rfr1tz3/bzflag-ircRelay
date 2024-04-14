
#include "bzfsAPI.h"
#include "plugin_utils.h"

std::string ircAddress;
std::string ircChannel;
std::string ircNick;

class ircRelay : public bz_Plugin {
    public:
        virtual const char* Name();
        virtual void Init(const char* config);
        virtual void Cleanup();
        virtual void Event(bz_EventData* eventData);
        static void Start();
        static void Stop();
        static void* Ping(void* t);
    private:
        static int fd;
        static bool run;
};
