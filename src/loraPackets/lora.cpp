#include "lora.h"

//#include "packets.h"

const int udpPort = 1234;

int udpSocket;
typedef bool boolean;
typedef unsigned char byte;

static const int CHANNEL = 0;

char message[256];

bool sx1272 = true;

byte receivedbytes;

enum sf_t { SF7=7, SF8, SF9, SF10, SF11, SF12 };

// SX1272 - Raspberry connections
int ssPin = 8; //GPIO8
int dio0  = 4; //GPIO4
int RST   = 22; //GPIO22

// Set spreading factor (SF7 - SF12)
sf_t sf = SF7;

// Set center frequency
uint32_t  freq = 927500000; // (927.5) Mhz
byte hello[32] = "HELLO";

void die(const char *s)
{
    perror(s);
    exit(1);
}

void selectreceiver()
{
    digitalWrite(ssPin, LOW);
}

void unselectreceiver()
{
    digitalWrite(ssPin, HIGH);
}

byte readReg(byte addr)
{
    unsigned char spibuf[2];

    selectreceiver();
    spibuf[0] = addr & 0x7F;
    spibuf[1] = 0x00;
    wiringPiSPIDataRW(CHANNEL, spibuf, 2);
    unselectreceiver();

    return spibuf[1];
}

void writeReg(byte addr, byte value)
{
    unsigned char spibuf[2];

    spibuf[0] = addr | 0x80;
    spibuf[1] = value;
    selectreceiver();
    wiringPiSPIDataRW(CHANNEL, spibuf, 2);

    unselectreceiver();
}

static void opmode (uint8_t mode) {
    writeReg(REG_OPMODE, (readReg(REG_OPMODE) & ~OPMODE_MASK) | mode);
}

static void opmodeLora() {
    uint8_t u = OPMODE_LORA;
    if (sx1272 == false)
        u |= 0x8;   // TBD: sx1276 high freq
    writeReg(REG_OPMODE, u);
}

void SetupLoRa()
{

    digitalWrite(RST, HIGH);
    delay(100);
    digitalWrite(RST, LOW);
    delay(100);

    byte version = readReg(REG_VERSION);

    if (version == 0x22) {
        // sx1272
        printf("SX1272 detected, starting.\n");
        sx1272 = true;
    } else {
        // sx1276?
        digitalWrite(RST, LOW);
        delay(100);
        digitalWrite(RST, HIGH);
        delay(100);
        version = readReg(REG_VERSION);
        if (version == 0x12) {
            // sx1276
            printf("SX1276 detected, starting.\n");
            sx1272 = false;
        } else {
            printf("Unrecognized transceiver.\n");
            //printf("Version: 0x%x\n",version);
            exit(1);
        }
    }

    opmode(OPMODE_SLEEP);

    // set frequency
    uint64_t frf = ((uint64_t)freq << 19) / 32000000;
    writeReg(REG_FRF_MSB, (uint8_t)(frf>>16) );
    writeReg(REG_FRF_MID, (uint8_t)(frf>> 8) );
    writeReg(REG_FRF_LSB, (uint8_t)(frf>> 0) );

    writeReg(REG_SYNC_WORD, 0x34); // LoRaWAN public sync word

    if (sx1272) {
        if (sf == SF11 || sf == SF12) {
            writeReg(REG_MODEM_CONFIG,0x0B);
        } else {
            writeReg(REG_MODEM_CONFIG,0x0A);
        }
        writeReg(REG_MODEM_CONFIG2,(sf<<4) | 0x04);
    } else {
        if (sf == SF11 || sf == SF12) {
            writeReg(REG_MODEM_CONFIG3,0x0C);
        } else {
            writeReg(REG_MODEM_CONFIG3,0x04);
        }
        writeReg(REG_MODEM_CONFIG,0x72);
        writeReg(REG_MODEM_CONFIG2,(sf<<4) | 0x04);
    }

    if (sf == SF10 || sf == SF11 || sf == SF12) {
        writeReg(REG_SYMB_TIMEOUT_LSB,0x05);
    } else {
        writeReg(REG_SYMB_TIMEOUT_LSB,0x08);
    }
    writeReg(REG_MAX_PAYLOAD_LENGTH,0x80);
    writeReg(REG_PAYLOAD_LENGTH,PAYLOAD_LENGTH);
    writeReg(REG_HOP_PERIOD,0xFF);
    writeReg(REG_FIFO_ADDR_PTR, readReg(REG_FIFO_RX_BASE_AD));

    writeReg(REG_LNA, LNA_MAX_GAIN);

}

boolean receive(char *payload) {
    // clear rxDone
    writeReg(REG_IRQ_FLAGS, 0x40);

    int irqflags = readReg(REG_IRQ_FLAGS);

    //  payload crc: 0x20
    if((irqflags & 0x20) == 0x20)
    {
        printf("CRC error\n");
        writeReg(REG_IRQ_FLAGS, 0x20);
        return false;
    } else {

        byte currentAddr = readReg(REG_FIFO_RX_CURRENT_ADDR);
        byte receivedCount = readReg(REG_RX_NB_BYTES);
        receivedbytes = receivedCount;

        writeReg(REG_FIFO_ADDR_PTR, currentAddr);

        for(int i = 0; i < receivedCount; i++)
        {
            payload[i] = (char)readReg(REG_FIFO);
        }
    }
    return true;
}

void receivepacket() {

    long int SNR;
    int rssicorr;

    if(digitalRead(dio0) == 1)
    {
        if(receive(message)) {
            byte value = readReg(REG_PKT_SNR_VALUE);
            if( value & 0x80 ) // The SNR sign bit is 1
            {
                // Invert and divide by 4
                value = ( ( ~value + 1 ) & 0xFF ) >> 2;
                SNR = -value;
            }
            else
            {
                // Divide by 4
                SNR = ( value & 0xFF ) >> 2;
            }

            if (sx1272) {
                rssicorr = 139;
            } else {
                rssicorr = 157;
            }

            printf("Packet RSSI: %d, ", readReg(0x1A)-rssicorr);
            printf("RSSI: %d, ", readReg(0x1B)-rssicorr);
            printf("SNR: %li, ", SNR);
            printf("Length: %i", (int)receivedbytes);
            printf("\n");
            printf("Payload: %s\n", message);

        } // received a message

    } // dio0=1
}

static void configPower (int8_t pw) {
    if (sx1272 == false) {
        // no boost used for now
        if(pw >= 17) {
            pw = 15;
        } else if(pw < 2) {
            pw = 2;
        }
        // check board type for BOOST pin
        writeReg(RegPaConfig, (uint8_t)(0x80|(pw&0xf)));
        writeReg(RegPaDac, readReg(RegPaDac)|0x4);

    } else {
        // set PA config (2-17 dBm using PA_BOOST)
        if(pw > 17) {
            pw = 17;
        } else if(pw < 2) {
            pw = 2;
        }
        writeReg(RegPaConfig, (uint8_t)(0x80|(pw-2)));
    }
}


static void writeBuf(byte addr, byte *value, byte len) {
    unsigned char spibuf[256];
    spibuf[0] = addr | 0x80;
    for (int i = 0; i < len; i++) {
        spibuf[i + 1] = value[i];
    }
    selectreceiver();
    wiringPiSPIDataRW(CHANNEL, spibuf, len + 1);
    unselectreceiver();
}

void txlora(byte *frame, byte datalen) {

    //byte frame[256]; // Make sure this is large enough to hold the JSON string and other data    
    // Convert the JSON string to bytes
    //strncpy((char *)frame, jsonString, datalen);

    // set the IRQ mapping DIO0=TxDone DIO1=NOP DIO2=NOP
    writeReg(RegDioMapping1, MAP_DIO0_LORA_TXDONE|MAP_DIO1_LORA_NOP|MAP_DIO2_LORA_NOP);
    // clear all radio IRQ flags
    writeReg(REG_IRQ_FLAGS, 0xFF);
    // mask all IRQs but TxDone
    writeReg(REG_IRQ_FLAGS_MASK, ~IRQ_LORA_TXDONE_MASK);

    // initialize the payload size and address pointers
    writeReg(REG_FIFO_TX_BASE_AD, 0x00);
    writeReg(REG_FIFO_ADDR_PTR, 0x00);
    writeReg(REG_PAYLOAD_LENGTH, datalen);

    // download buffer to the radio FIFO
    writeBuf(REG_FIFO, frame, datalen);
    // now we actually start the transmission
    opmode(OPMODE_TX);

    printf("send: %s\n", frame);
}

// region PacotesImagens
vector<unsigned char> lora::readImageToBytes(const string& filename)
{
    ifstream file(filename, ios::binary);
    vector<unsigned char> bytes;

    if (file.is_open()) {
        bytes.assign((istreambuf_iterator<char>(file)), (istreambuf_iterator<char>()));
        file.close();
        return bytes;
    }

    cout << "Erro ao abrir a imagem " << filename << endl;
    return bytes;
}

vector<vector<unsigned char>> lora::splitBytesIntoPackets(const vector<unsigned char>& bytes)
{
    vector<vector<unsigned char>> packets;
    const size_t packetSize = 250;

    for (size_t i = 0; i < bytes.size(); i += packetSize) {
        vector<unsigned char> packet(bytes.begin() + i, bytes.begin() + i + min(packetSize, bytes.size() - i));
        packets.push_back(packet);
    }

    return packets;
}

vector<string> lora::listFiles(const string& folderPath) {
    vector<string> files;
    DIR* dir = opendir(folderPath.c_str());
    if (dir != nullptr) {
        printf("teste");
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            string fileName = entry->d_name;
            if (entry->d_type == DT_REG) {
                files.push_back(folderPath + "/" + fileName);
            }
        }
        closedir(dir);
    }
    return files;
}

string lora::getLastFile(const vector<string>& files) {
    if (files.empty()) {
        return "";
    }

    string lastFile = files[0];
    for (const auto& file : files) {
        if (file > lastFile) {
            lastFile = file;
        }
    }
    return lastFile;
}

void lora::splitPackets() {
    const string folderPath = "/home/Embauba/CanSat/cameraOS/imagens/compressed";

    vector<string> files = listFiles(folderPath);
    string lastFile = getLastFile(files);

    cout << "Last file: " << lastFile << '\n';
    vector<unsigned char> imageBytes = readImageToBytes(lastFile);
    vector<vector<unsigned char>> packets = splitBytesIntoPackets(imageBytes);

    cout << "Image read successfully. Number of packets: " << packets.size() << endl;

//    ofstream fileTeste("teste.jp2", ios::binary);

    for (const auto& packet : packets) {
//        fileTeste.write(reinterpret_cast<const char *>(packet.data()), packet.size());
        byte jsonPayload[256];
        memcpy(jsonPayload, packet.data(), packet.size());
        txlora(jsonPayload, packet.size());
        delay(250);
    }
}
// endregion

int lora::start() {
    printf("Creating UDP socket...\n");
    udpSocket = socket(AF_INET, SOCK_DGRAM, 0);

    if (udpSocket == -1) {
        perror("socket");
        return 1;
    }
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(udpPort);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(udpSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("bind");
        return 1;
    }
    printf("UDP listener started on port %d\n", udpPort);
    //Lora sender
    pinMode(ssPin, OUTPUT);
    pinMode(dio0, INPUT);
    pinMode(RST, OUTPUT);
    wiringPiSPISetup(CHANNEL, 500000);
    SetupLoRa();
    printf("Send packets at SF%i on %.6lf Mhz.\n", sf,(double)freq/1000000);
    printf("------------------\n");

    int contador = 0;
    while(1) {
        lora::splitPackets();

        printf("%d\n", contador);

        //UDP receiving string
        char buffer[256]; //256 is the right size. Changed for test.
        byte jsonPayload[256]; //256 is the right size
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int bytesRead = recvfrom(udpSocket, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (bytesRead == -1) {
            perror("recvfrom");
        } else {
            buffer[bytesRead] = '\0';
            printf("Received from %s: %s\n", inet_ntoa(clientAddr.sin_addr), buffer);
        }
        memcpy(jsonPayload, buffer, strlen(buffer));
        // Convert the JSON payload length to byte (assuming jsonString.length() is less than 256)
        byte payloadLength = static_cast<byte>(strlen(buffer));
        opmodeLora();
        // enter standby mode (required for FIFO loading))
        opmode(OPMODE_STANDBY);
        writeReg(RegPaRamp, (readReg(RegPaRamp) & 0xF0) | 0x08); // set PA ramp-up time 50 uSec
        configPower(20);
        txlora(jsonPayload, payloadLength);
        delay(250);
        contador++;
        if (contador == 10){
            contador = 0;
            lora::splitPackets();
        }
    }

    //if (argc > 2)
    //strncpy((char *)buffer, argv[2], sizeof(buffer));

    //while(1) {
    //txlora(jsonDataBytes, strlen((char *)jsonDataBytes));
    //    txlora(buffer, strlen(buffer)); // Send the JSON string
    //delay(5000);
    //}


    return (0);
}