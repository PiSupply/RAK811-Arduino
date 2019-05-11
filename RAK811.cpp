/*
 * A library for controlling RAK811 LoRa radio.
 *
 * @Author Chace.cao
 * @Author john.zou
 * @Date 11/05/2017
 *
 */

#include "Arduino.h"
#include "RAK811.h"

extern "C" {
  #include "string.h"
  #include "stdlib.h"
  }

/*
  @param serial Needs to be an already opened Stream ({Software/Hardware}Serial) to write to and read from.
*/
RAK811::RAK811(Stream& serial,Stream& serial1):
_serial(serial),_serial1(serial1)
{
  _serial.setTimeout(2000);
  _serial1.setTimeout(1000);
}

String RAK811::rk_getVersion()
{
  String ret = sendRawCommand(F("at+version"));
  ret.trim();
  return ret;
}

String RAK811::rk_getBand()
{
  String ret = sendRawCommand(F("at+band"));
  ret.trim();
  return ret;
}

String RAK811::rk_getSignal()
{
  String ret = sendRawCommand(F("at+signal"));
  ret.trim();
  return ret;
}

bool RAK811::rk_setRate(int rate)
{
  String ret = sendRawCommand("at+dr=" + (String)rate);
  if (ret.startsWith("OK"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

void RAK811::rk_sleep()
{
  sendRawCommand(F("at+sleep"));
}

void RAK811::rk_reload()
{
  sendRawCommand(F("at+reload"));
}

void RAK811::rk_reset(int mode)
{
  if (mode == 1)
  {
    sendRawCommand(F("at+reset=1"));
  }
  else if(mode == 0)
  {
    sendRawCommand(F("at+reset=0"));
  }
  else
  {
    Serial.println("The mode set Error,the mode is '0'or '1'.");
  }
}

bool RAK811::rk_setWorkingMode(int mode)
{
  String ver;
  switch (mode)
  {
    case 0:
      ver = sendRawCommand(F("at+mode=0")); //Set LoRaWAN Mode.
      break;
    case 1:
      ver = sendRawCommand(F("at+mode=1")); //Set LoRaP2P Mode.
      break;
    default:
      return false;
  }
  if (ver.startsWith("OK"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool RAK811::rk_joinLoRaNetwork(int mode)
{
  String ver;
  switch (mode)
  {
    case 0:
      ver = sendRawCommand(F("at+join=otaa")); //join Network through OTAA mode.
      break;
    case 1:
      ver = sendRawCommand(F("at+join=abp")); //join Network through ABP mode.
      break;
    default:
      return false;
  }
  if (ver.startsWith("OK"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool RAK811::rk_initOTAA(String devEUI, String appEUI, String appKEY)
{
  String command = "";
  if (devEUI.length() == 16)
  {
    _devEUI = devEUI;
  }
  else
  {
    String ret = sendRawCommand(F("at+get_config=dev_eui"));
    const char* ver = ret.c_str();
    ret = &ver[2];
//  Serial.println(ret);
    if (ret.length() == 16)
    {
      _devEUI = ret;
    }
  }
  if (appEUI.length() == 16)
  {
    _appEUI = appEUI;
  }
  else
  {
    Serial.println("The parameter appEUI is set incorrectly!");
  }
  if (appKEY.length() == 32 )
  {
    _appKEY = appKEY;
  }
  else
  {
    Serial.println("The parameter appKEY is set incorrectly!");
  }
  command = "at+set_config=dev_eui:" + _devEUI + "&app_eui:" + _appEUI + "&app_key:" + _appKEY;
//  Serial.println(command);
  String ret = sendRawCommand(command);
  if (ret.startsWith("OK"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool RAK811::rk_initABP(String devADDR, String nwksKEY, String appsKEY)
{
  String command = "";
  if (devADDR.length() == 8)
  {
    _devADDR = devADDR;
  }
  else
  {
    Serial.println("The parameter devADDR is set incorrectly!");
  }
  if (nwksKEY.length() == 32)
  {
    _nwksKEY = nwksKEY;
  }
  else
  {
    Serial.println("The parameter nwksKEY is set incorrectly!");
  }
  if (appsKEY.length() == 32 )
  {
    _appsKEY = appsKEY;
  }
  else
  {
    Serial.println("The parameter appsKEY is set incorrectly!");
  }
  command = "at+set_config=dev_addr:" + _devADDR + "&nwks_key:" + _nwksKEY + "&apps_key:" + _appsKEY;
//  Serial.println(command);
  String ret = sendRawCommand(command);
  if (ret.startsWith("OK"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool RAK811::rk_sendData(int type, int port, char* datahex)
{
  String command = "";
  command = "at+send=" + (String)type + "," + port + "," + datahex;
//  Serial.println(command);
  String ret = sendRawCommand(command);
  if (ret.startsWith("OK"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

String RAK811::rk_recvData(void)
{
  _serial.setTimeout(2000);
  String ret = _serial.readStringUntil('\n');
#if defined DEBUG_MODE
  _serial1.println("-> " + ret);
#endif
  ret.trim();
  return ret;
}

bool RAK811::rk_setConfig(String Key,String Value)
{
  String command = "";
  command = "at+set_config=" + Key + ":" + Value;
//  Serial.println(command);
  String ret = sendRawCommand(command);
  if (ret.startsWith("OK"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

String RAK811::rk_getConfig(String Key)
{
  String ret = sendRawCommand("at+get_config=" + Key);
  ret.trim();
  return ret;
}

String RAK811::rk_getP2PConfig(void)
{
  String ret = sendRawCommand("at+rf_config");
  ret.trim();
  return ret;
}

bool RAK811::rk_initP2P(String FREQ, int SF, int BW, int CR, int PRlen, int PWR)
{
  String command = "";
  command = "at+rf_config=" + FREQ + "," + SF + "," + BW + "," + CR + "," + PRlen + "," + PWR;
//  Serial.println(command);
  String ret = sendRawCommand(command);
  if (ret.startsWith("OK"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool RAK811::rk_recvP2PData(int report_en)
{
  String ver;
  switch (report_en)
  {
    case 0:
      ver = sendRawCommand(F("at+rxc=0")); //
      break;
    case 1:
      ver = sendRawCommand(F("at+rxc=1")); //
      break;
    default:
      return false;
  }
  if (ver.startsWith("OK"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool RAK811::rk_sendP2PData(int CNTS, String interver, char* DATAHex)
{
  String command = "";
  command = "at+txc=" + (String)CNTS + "," + interver + "," + DATAHex;
//  Serial.println(command);
  String ret = sendRawCommand(command);
  if (ret.startsWith("OK"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool RAK811::rk_stopSendP2PData(void)
{
  String ret = sendRawCommand(F("at+tx_stop"));
  if (ret.startsWith("OK"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool RAK811::rk_stopRecvP2PData(void)
{
  String ret = sendRawCommand(F("at+rx_stop"));
  if (ret.startsWith("OK"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

String RAK811::rk_checkStatusStatistics(void)
{
  String ret = sendRawCommand(F("at+status"));
  ret.trim();
  return ret;
}

bool RAK811::rk_cleanStatusStatistics(void)
{
  String ret = sendRawCommand(F("at+status=0"));
  if (ret.startsWith("OK"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool RAK811::rk_setUARTConfig(int Baud, int Data_bits, int Parity, int Stop_bits, int Flow_ctrl)
{
  String command = "";
  command = "at+uart=" + (String)Baud + "," + Data_bits + "," + Parity + "," + Stop_bits + "," + Flow_ctrl;
//  Serial.println(command);
  String ret = sendRawCommand(command);
  if (ret.startsWith("OK"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

String RAK811::sendRawCommand(String command)
{
  delay(100);
  while(_serial.available()){
#if defined DEBUG_MODE
    _serial1.println("-> " + _serial.readStringUntil('\0'));
#else
    _serial.read();
#endif
  }
  delay(100);
  _serial.println(command);
  delay(200);
  String ret = _serial.readStringUntil('\n');
  ret.trim();
  delay(500);
#if defined DEBUG_MODE
  _serial1.println("<- " + command);
  _serial1.println("-> " + ret);
#endif
  return ret;
}
