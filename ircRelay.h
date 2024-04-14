
#include "bzfsAPI.h"
#include "plugin_utils.h"

std::string ircAddress;
std::string ircChannel;
std::string ircNick;

int fd;
void* respondPing(void*);

class ircRelay : public bz_Plugin {
public:
    virtual const char* Name();
    virtual void Init(const char* config);
    virtual void Startup();
    virtual void Cleanup();
    virtual void Event(bz_EventData* eventData);
};
