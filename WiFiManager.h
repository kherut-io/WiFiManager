/**************************************************************
   WiFiManager is a library for the ESP8266/Arduino platform
   (https://github.com/esp8266/Arduino) to enable easy
   configuration and reconfiguration of WiFi credentials using a Captive Portal
   inspired by:
   http://www.esp8266.com/viewtopic.php?f=29&t=2520
   https://github.com/chriscook8/esp-arduino-apboot
   https://github.com/esp8266/Arduino/tree/master/libraries/DNSServer/examples/CaptivePortalAdvanced
   Built by AlexT https://github.com/tzapu
   Licensed under MIT license
 **************************************************************/

#ifndef WiFiManager_h
#define WiFiManager_h

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <memory>

extern "C" {
  #include "user_interface.h"
}

const char HTTP_HEAD[] PROGMEM            = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\" name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>{v}</title>";
const char HTTP_STYLE[] PROGMEM           = "<style>html{height: 100%;}.logo{margin-top: 10px}.c, body{text-align: center; height: 100%;}body,button{font-family: monospace; cursor: pointer;}form{margin-top: 15px;}form:first-of-type{margin-top: 10px;}h1{margin-top: 25px;}.option{display: inline-block; width: 310px;}div,input{padding: 5px; font-size: 1em}input{width: 270px; height: 35px; border: none; border-radius: 25px; margin-top: 7px; outline: none; padding: 5px 20px; font-family: monospace;}button[type='submit']{width: 310px;}body{background: #360033; background: -webkit-linear-gradient(to bottom right, #0b8793, #360033); background: linear-gradient(to bottom right, #0b8793, #360033); color: #fff; margin: 0; background-repeat: no-repeat; background-attachment: fixed;}button{border-radius: 21px; background-color: transparent; color: white; border: solid 1px white; height: 42px; font-size: 12px; width: 100%; transition: 0.2s ease-out all;}button:hover{color: black; background: white;}.scan_link a{color: white; text-decoration: none; font-size: 32px; position: absolute; top: 0; left: 14px; font-family: monospace;}.wifi_link *{display: inline-block; margin-bottom: 10px;}.wifi_link a{color: white; text-decoration: none;}.wifi_link:first-of-type{margin-top: 20px;}dl{position: absolute; text-align: left; left: 50%; transform: translateX(-50%); font-size: 14px; margin-top: 20px;}dl dt{font-weight: bolder;}dl dt::after{content: ':';}dl dd{margin-bottom: 10px;}.wifi_link::after{content: ''; display: block; width: 50px; height: 1px; background: white; margin: auto;}.wifi_link:nth-last-of-type(2)::after{display: none;}.l{background: url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX/8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==') left center no-repeat; background-size: 1em}</style>";
const char HTTP_SCRIPT[] PROGMEM          = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>";
const char HTTP_HEAD_END[] PROGMEM        = "</head><body><div><a href='/'><img class='logo' src='data:image/png;base64,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'/></a>";
const char HTTP_PORTAL_OPTIONS[] PROGMEM  = "<form action='/wifi' method='get'><button class='option'>Configure WiFi</button></form> <form action='/0wifi' method='get'><button class='option'>Configure WiFi (w/o scanning)</button></form> <form action='/i' method='get'><button class='option'>Info</button></form> <form action='/r' method='post'><button class='option'>Reset</button></form>";
const char HTTP_ITEM[] PROGMEM            = "<div class='wifi_link'><a href='#p' onclick='c(this)'>{v}</a>&nbsp;<span class='{i}'>({r}%)</span></div>";
const char HTTP_FORM_START[] PROGMEM      = "<form method='get' action='wifisave'><input id='s' name='s' length=32 placeholder='SSID...'><br/><input id='p' name='p' length=64 type='password' placeholder='Password...'><br/>";
const char HTTP_FORM_PARAM[] PROGMEM      = "<br/><input id='{i}' name='{n}' maxlength={l} placeholder='{p}' value='{v}' {c}>";
const char HTTP_FORM_END[] PROGMEM        = "<br/><button type='submit'>Save</button></form>";
const char HTTP_SCAN_LINK[] PROGMEM       = "<br/><div class=\"c scan_link\"><a href=\"/wifi\">↻</a></div>";
const char HTTP_SAVED[] PROGMEM           = "<div>Credentials saved.<br />Trying to connect device to network.<br />If it fails reconnect to AP and try again.</div>";
const char HTTP_END[] PROGMEM             = "</div></body></html>";

#ifndef WIFI_MANAGER_MAX_PARAMS
#define WIFI_MANAGER_MAX_PARAMS 10
#endif

class WiFiManagerParameter {
  public:
    /** 
        Create custom parameters that can be added to the WiFiManager setup web page
        @id is used for HTTP queries and must not contain spaces nor other special characters
    */
    WiFiManagerParameter(const char *custom);
    WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length);
    WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);
    ~WiFiManagerParameter();

    const char *getID();
    const char *getValue();
    const char *getPlaceholder();
    int         getValueLength();
    const char *getCustomHTML();
  private:
    const char *_id;
    const char *_placeholder;
    char       *_value;
    int         _length;
    const char *_customHTML;

    void init(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);

    friend class WiFiManager;
};


class WiFiManager
{
  public:
    WiFiManager();
    ~WiFiManager();

    boolean       autoConnect();
    boolean       autoConnect(char const *apName, char const *apPassword = NULL);

    //if you want to always start the config portal, without trying to connect first
    boolean       startConfigPortal();
    boolean       startConfigPortal(char const *apName, char const *apPassword = NULL);

    // get the AP name of the config portal, so it can be used in the callback
    String        getConfigPortalSSID();

    void          resetSettings();

    //sets timeout before webserver loop ends and exits even if there has been no setup.
    //useful for devices that failed to connect at some point and got stuck in a webserver loop
    //in seconds setConfigPortalTimeout is a new name for setTimeout
    void          setConfigPortalTimeout(unsigned long seconds);
    void          setTimeout(unsigned long seconds);

    //sets timeout for which to attempt connecting, useful if you get a lot of failed connects
    void          setConnectTimeout(unsigned long seconds);


    void          setDebugOutput(boolean debug);
    //defaults to not showing anything under 8% signal quality if called
    void          setMinimumSignalQuality(int quality = 8);
    //sets a custom ip /gateway /subnet configuration
    void          setAPStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);
    //sets config for a static IP
    void          setSTAStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);
    //called when AP mode and config portal is started
    void          setAPCallback( void (*func)(WiFiManager*) );
    //called when settings have been changed and connection was successful
    void          setSaveConfigCallback( void (*func)(void) );
    //adds a custom parameter, returns false on failure
    bool          addParameter(WiFiManagerParameter *p);
    //if this is set, it will exit after config, even if connection is unsuccessful.
    void          setBreakAfterConfig(boolean shouldBreak);
    //if this is set, try WPS setup when starting (this will delay config portal for up to 2 mins)
    //TODO
    //if this is set, customise style
    void          setCustomHeadElement(const char* element);
    //if this is true, remove duplicated Access Points - defaut true
    void          setRemoveDuplicateAPs(boolean removeDuplicates);

  private:
    std::unique_ptr<DNSServer>        dnsServer;
    std::unique_ptr<ESP8266WebServer> server;

    //const int     WM_DONE                 = 0;
    //const int     WM_WAIT                 = 10;

    //const String  HTTP_HEAD = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/><title>{v}</title>";

    void          setupConfigPortal();
    void          startWPS();

    const char*   _apName                 = "no-net";
    const char*   _apPassword             = NULL;
    String        _ssid                   = "";
    String        _pass                   = "";
    unsigned long _configPortalTimeout    = 0;
    unsigned long _connectTimeout         = 0;
    unsigned long _configPortalStart      = 0;

    IPAddress     _ap_static_ip;
    IPAddress     _ap_static_gw;
    IPAddress     _ap_static_sn;
    IPAddress     _sta_static_ip;
    IPAddress     _sta_static_gw;
    IPAddress     _sta_static_sn;

    int           _paramsCount            = 0;
    int           _minimumQuality         = -1;
    boolean       _removeDuplicateAPs     = true;
    boolean       _shouldBreakAfterConfig = false;
    boolean       _tryWPS                 = false;

    const char*   _customHeadElement      = "";

    //String        getEEPROMString(int start, int len);
    //void          setEEPROMString(int start, int len, String string);

    int           status = WL_IDLE_STATUS;
    int           connectWifi(String ssid, String pass);
    uint8_t       waitForConnectResult();

    void          handleRoot();
    void          handleWifi(boolean scan);
    void          handleWifiSave();
    void          handleInfo();
    void          handleReset();
    void          handleNotFound();
    void          handle204();
    boolean       captivePortal();
    boolean       configPortalHasTimeout();

    // DNS server
    const byte    DNS_PORT = 53;

    //helpers
    int           getRSSIasQuality(int RSSI);
    boolean       isIp(String str);
    String        toStringIp(IPAddress ip);

    boolean       connect;
    boolean       _debug = true;

    void (*_apcallback)(WiFiManager*) = NULL;
    void (*_savecallback)(void) = NULL;

    int                    _max_params;
    WiFiManagerParameter** _params;

    template <typename Generic>
    void          DEBUG_WM(Generic text);

    template <class T>
    auto optionalIPFromString(T *obj, const char *s) -> decltype(  obj->fromString(s)  ) {
      return  obj->fromString(s);
    }
    auto optionalIPFromString(...) -> bool {
      DEBUG_WM("NO fromString METHOD ON IPAddress, you need ESP8266 core 2.1.0 or newer for Custom IP configuration to work.");
      return false;
    }
};

#endif
