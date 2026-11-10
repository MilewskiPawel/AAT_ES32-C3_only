#if defined(AAT_BACKPACK)

#include "devwifi_proxies.h"
#include "module_aat.h"
#include "options.h"

void WebAatAppendConfig(ArduinoJson::JsonDocument &json)
{
    auto aat = json["config"].createNestedObject("aat");
    aat["satmin"] = config.GetAatSatelliteHomeMin();
    aat["azim_sff"] = config.GetAatAzimuthServoFastFlip();
    aat["servosmoo"] = config.GetAatServoSmooth();
    aat["servomode"] = config.GetAatServoMode();
    aat["project"] = config.GetAatProject();
    aat["azim_center"] = config.GetAatCenterDir();
    aat["azim_min"] = config.GetAatServoLow(0);
    aat["azim_max"] = config.GetAatServoHigh(0);
    aat["elev_min"] = config.GetAatServoLow(1);
    aat["elev_max"] = config.GetAatServoHigh(1);

    char uidStr[24] = {0};
    uint8_t *addr = config.GetGroupAddress();
    bool allZero = true;
    for (int i = 0; i < 6; i++)
    {
        if (addr[i] != 0) { allZero = false; break; }
    }
    if (!allZero)
    {
        snprintf(uidStr, sizeof(uidStr), "%u,%u,%u,%u,%u,%u",
            addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
    }
    else
    {
        strlcpy(uidStr, "not set", sizeof(uidStr));
    }
    aat["uid"] = uidStr;

    // VBat
    auto vbat = json["config"].createNestedObject("vbat");
    vbat["offset"] = config.GetVbatOffset();
    vbat["scale"] = config.GetVbatScale();
    vbat["vbat"] = vrxModule.getVbat();
}

void WebAatConfig(AsyncWebServerRequest *request)
{
    // Servos
    if (request->hasArg("servosmoo"))
        config.SetAatServoSmooth(request->arg("servosmoo").toInt());
    if (request->hasArg("azim_sff"))
        config.SetAatAzimuthServoFastFlip(request->arg("azim_sff").toInt());
    else
        config.SetAatAzimuthServoFastFlip(0);
    if (request->hasArg("servomode"))
        config.SetAatServoMode(request->arg("servomode").toInt());
    if (request->hasArg("azim_center"))
        config.SetAatCenterDir(request->arg("azim_center").toInt());
    if (request->hasArg("azim_min"))
        config.SetAatServoLow(0, request->arg("azim_min").toInt());
    if (request->hasArg("azim_max"))
        config.SetAatServoHigh(0, request->arg("azim_max").toInt());
    if (request->hasArg("elev_min"))
        config.SetAatServoLow(1, request->arg("elev_min").toInt());
    if (request->hasArg("elev_max"))
        config.SetAatServoHigh(1, request->arg("elev_max").toInt());
    // VBat
    if (request->hasArg("vbat_offset"))
        config.SetVbatOffset(request->arg("vbat_offset").toInt());
    if (request->hasArg("vbat_scale"))
        config.SetVbatScale(request->arg("vbat_scale").toInt());
    // UID / Group Address (hardcoded in firmware)
    (void)request;
    {
        const uint8_t addr[6] = {156, 79, 171, 221, 42, 88};
        config.SetGroupAddress(addr);
        memcpy(firmwareOptions.uid, addr, 6);
        firmwareOptions.hasUID = true;
    }
    // Servo Target Override (must be set after azim_center because bearing relies on center)
    if (request->hasArg("bear"))
        vrxModule.overrideTargetBearing(request->arg("bear").toInt());
    if (request->hasArg("elev"))
        vrxModule.overrideTargetElev(request->arg("elev").toInt());
    // Satellite Config
    if (request->hasArg("satmin"))
        config.SetAatSatelliteHomeMin(request->arg("satmin").toInt());

    const char *response;
    if (request->arg("commit").toInt() == 1)
    {
        response = "Saved";
        config.Commit();
    }
    else
        response = "Modified";

    request->send(200, "text/plain", response);
}

void WebAatInit(AsyncWebServer &server)
{
    server.on("/aatconfig", WebAatConfig);
}

#endif /* defined(AAT_BACKPACK) */

