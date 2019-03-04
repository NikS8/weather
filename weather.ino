/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
                                                        weather.ino 
                                  Copyright © 2019, Zigfred & Nik.S
20.02.2019 v1
21.02.2019 v2 Количество свободной памяти
04.03.2019 v3 Время работы после старта или рестарта
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*****************************************************************************\
http-cервер weather выдает данные: 
    цифровые:
      датчик температуры и влажности DHT 
/*****************************************************************************/
//    Скетч для ATmega168.   Работает
//  Скетч использует 14186 байт (99%) памяти устройства.
//  Глобальные переменные используют 545 байт (53%) динамической памяти,
//  оставляя 479 байт для локальных переменных.
//
#include <SimpleDHT.h>
#include <Ethernet2.h>

byte mac[] = {
    0xDE, 0xAD, 0xBE, 0xEF, 0xFA, 0xED};
IPAddress ip(192, 168, 1, 112);
EthernetServer httpServer(40112);

#define dataPinSensor1 8
#define dataPinSensor2 7
SimpleDHT22 dht1(dataPinSensor1);
SimpleDHT22 dht2(dataPinSensor2);

float temperatureIn = 22.22;
float humidityIn = 40;
float temperatureOut = 4;
float humidityOut = 80;
#define secondsDay 86400

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
            setup
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void setup()
{
    Ethernet.begin(mac, ip);
    httpServer.begin();
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
            loop
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void loop()
{
    int err = SimpleDHTErrSuccess;
    if ((err = dht1.read2(&temperatureIn, &humidityIn, NULL))
         != SimpleDHTErrSuccess)
    {
        delay(200);
        return;
    }
    if ((err = dht2.read2(&temperatureOut, &humidityOut, NULL))
         != SimpleDHTErrSuccess)
    {
        delay(200);
        return;
    }
    EthernetClient reqClient = httpServer.available();
    if (!reqClient) return;

    while (reqClient.available())   reqClient.read();
  
    reqClient.println("HTTP/1.1 200 OK");
    reqClient.println("Content-Type: application/json");
    reqClient.println();
    reqClient.print("{\n\"weather\":\"ATmega168\",\n\"data\":{\n\t\"home *C\":");
       
    reqClient.print(temperatureIn);
    reqClient.print(",\n\t\"home H\":");
    
    reqClient.print((int)humidityIn);
    reqClient.print(",\n\t\"street *C\":");

    reqClient.print((int)temperatureOut);
    reqClient.print(",\n\t\"street H\":");

    reqClient.print((int)humidityOut);
    reqClient.print("\n\t }");

    reqClient.print(",\n\"freeRam\":");
    reqClient.print(freeRam());

    reqClient.print(",\n\"upTime\":\"");
    reqClient.print(upTime(millis()));

    reqClient.print("\"\n}");

    reqClient.stop();
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
            Время работы после старта или рестарта
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
String upTime(uint32_t lasttime)    {   

  String lastStartTime;
  lasttime /= 1000;
  
  if (lasttime > secondsDay) {
 //   int lasthour = lasttime/secondsDay;
    lastStartTime.concat(lasttime/secondsDay);
    lastStartTime.concat("d ");
    lasttime -= ((lasttime/secondsDay)*secondsDay);
    }
  if (lasttime > 3600) {
    if (lasttime/3600<10) { lastStartTime.concat("0"); }
  lastStartTime.concat(lasttime/3600);
  lastStartTime.concat(":");
  }
  if (lasttime/60%60<10) { lastStartTime.concat("0"); }
lastStartTime.concat((lasttime/60)%60);
lastStartTime.concat(":");
  if (lasttime%60<10) { lastStartTime.concat("0"); }
lastStartTime.concat(lasttime%60);

return lastStartTime;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
            Количество свободной памяти
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int freeRam()
{
  extern int __heap_start, *__brkval;
  int v;
  return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
            end
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/