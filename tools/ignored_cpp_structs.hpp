// THIS FILE WILL NOT BE TOGETHER WITH BUILD PROCESSES
#ifdef QV2RAY
static_assert(false, "NO NOT INCLUDE ME"); // DO NOT TRY TO INCLUDE ME



struct ReverseObject {
    list<REVERSEObjects::BridgeObject> bridges;
    list<REVERSEObjects::PortalObject> portals;
    ReverseObject() : bridges(), portals() {}
    XTOSTRUCT(O(bridges, portals))
};
namespace REVERSEObjects
{
    struct BridgeObject {
        string tag;
        string domain;
        BridgeObject() : tag(), domain() {}
        XTOSTRUCT(O(tag, domain))
    };

    struct PortalObject {
        string tag;
        string domain;
        PortalObject() : tag(), domain() {}
        XTOSTRUCT(O(tag, domain))
    };

}
// IN: OUTBOUNDObjects namespace;
struct ProxySettingsObject {
    string tag;
    ProxySettingsObject(): tag() {}
    XTOSTRUCT(O(tag))
};

// IN: INBOUNDObjects namespace;
struct AllocateObject {
    string strategy;
    int refresh;
    int concurrency;
    AllocateObject(): strategy(), refresh(), concurrency() {}
    XTOSTRUCT(O(strategy, refresh, concurrency))
};

// IN: ROUTINGObjects
struct BalancerObject {
    string tag ;
    list<string> selector;
    BalancerObject() : tag(), selector() {}
    XTOSTRUCT(O(tag, selector))
};


struct VMessIn { // INBound;
    struct ClientObject {
        string id;
        int level;
        int alterId;
        string email;
        XTOSTRUCT(O(id, level, alterId, email))
    };
    list<ClientObject> clients;
    // detour and default will not be implemented as it's complicated...
    bool disableInsecureEncryption;
    XTOSTRUCT(O(clients, disableInsecureEncryption))
};
#endif
