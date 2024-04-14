
#include "bzfsAPI.h"
#include "plugin_utils.h"

class ircRelay : public bz_Plugin {
    public:
        virtual const char* Name();
        virtual void Init(const char* config);
        virtual void Cleanup();
        virtual void Event(bz_EventData* eventData);
        static void Start();
        static void Stop();
        static void Worker();
    protected:
        static std::string ircAddress;
        static std::string ircChannel;
        static std::string ircNick;
    private:
        volatile static int fd;
        volatile static bool world_up;
        volatile static bool plugin_up;
};
