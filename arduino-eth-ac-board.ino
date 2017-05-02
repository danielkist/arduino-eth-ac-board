#include <EtherCard.h>
#define STATIC 1  // set to 1 to disable DHCP (adjust myip/gwip values below)

#if STATIC
// ethernet interface ip address
static byte myip[] = { 10, 0, 1, 200 };
// gateway ip address
static byte gwip[] = { 10, 0, 1, 1 };
#endif

// ethernet mac address - must be unique on your network
static byte mymac[] = { 0x74, 0x69, 0x69, 0x2D, 0x30, 0x31 };

byte Ethernet::buffer[500]; // tcp/ip send and receive buffer

const char energyON[] PROGMEM =
  "HTTP/1.0 200 Ok\r\n"
  "Content-Type: application/json\r\n"
  "\r\n"
  "{'status':'ONLINE'}";

const char energyOFF[] PROGMEM =
  "HTTP/1.0 200 Ok\r\n"
  "Content-Type: application/json\r\n"
  "\r\n"
  "{'status':'OFFLINE'}";

int GREEN_LED = 9;
int RED_LED = 8;

boolean powerON = false;

void setup() {
  Serial.begin(57600);
  Serial.println("\n[backSoon]");


  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
    Serial.println( "Failed to access Ethernet controller");
#if STATIC
  ether.staticSetup(myip, gwip);
#else
  if (!ether.dhcpSetup())
    Serial.println("DHCP failed");
#endif

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);
  ether.printIp("DNS: ", ether.dnsip);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
}

void loop() {
  // wait for an incoming TCP packet, but ignore its contents
  if (ether.packetLoop(ether.packetReceive())) {
    if (powerON) {
      memcpy_P(ether.tcpOffset(), energyON, sizeof energyON);
      ether.httpServerReply(sizeof energyON - 1);
    } else {
      memcpy_P(ether.tcpOffset(), energyOFF, sizeof energyOFF);
      ether.httpServerReply(sizeof energyOFF - 1);
    }
  }
  checkEnergy();
  delay(100);
}

bool checkEnergy() {
  int t = analogRead(1);
  Serial.println(t);
  if (t > 50) {
    powerON = true;
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
  } else {
    powerON = false;
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
  }
}

