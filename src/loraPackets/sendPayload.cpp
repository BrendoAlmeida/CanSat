#include "sendPayload.h"

vector<unsigned char> sendPayload::readImageToBytes(const string& filename)
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

vector<vector<unsigned char>> sendPayload::splitBytesIntoPackets(const vector<unsigned char>& bytes)
{
    vector<vector<unsigned char>> packets;
    const size_t packetSize = 248;

    for (size_t i = 0; i < bytes.size(); i += packetSize) {
        vector<unsigned char> packet(bytes.begin() + i, bytes.begin() + i + min(packetSize, bytes.size() - i));
        packets.push_back(packet);
    }

    return packets;
}

vector<string> sendPayload::listFiles(const string& folderPath) {
    vector<string> files;
    DIR* dir = opendir(folderPath.c_str());
    if (dir != nullptr) {
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

string sendPayload::getLastFile(const vector<string>& files) {
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

string sendPayload::getImage(){
    const string folderPath = "/home/Embauba/CanSat/cameraOS/imagens/compressed";

    vector<string> files = listFiles(folderPath);
    string lastFile = getLastFile(files);

    return lastFile;
}

void sendPayload::sendImage(string url) {
    if (url == "") return;

    vector<unsigned char> imageBytes = this->readImageToBytes(url);
    vector<vector<unsigned char>> packets = this->splitBytesIntoPackets(imageBytes);

    if (packets.size() == 0) return;

    cout << "Image read successfully. Number of packets: " << packets.size() << endl;


    for (size_t i = 0; i < packets.size(); ++i) {
        const auto& packet = packets[i];

        // Criar o identificador de pacote no formato "001/045"
        std::stringstream ss;
        ss << setw(3) << setfill('0') << (i + 1) << "/" << setw(3) << setfill('0') << packets.size();
        std::string identifier = ss.str();

        // Converter o identificador para um vetor de bytes
        vector<unsigned char> identifierBytes(identifier.begin(), identifier.end());

        // Criar um novo vetor de bytes que contém o identificador seguido pelos dados do pacote original
        vector<unsigned char> packetWithIdentifier;
        packetWithIdentifier.insert(packetWithIdentifier.end(), identifierBytes.begin(), identifierBytes.end());
        packetWithIdentifier.insert(packetWithIdentifier.end(), packet.begin(), packet.end());

        // Criar um objeto LoRaPacket com o novo vetor de bytes
        LoRaPacket p(packetWithIdentifier.data(), packetWithIdentifier.size());

        cout << packetWithIdentifier.data() << endl;

        // Transmitir o pacote
        lora->transmitPacket(&p);
    }
}

sendPayload::sendPayload() {
    printf("Creating UDP socket...\n");
    udpSocket = socket(AF_INET, SOCK_DGRAM, 0);

    if (udpSocket == -1) {
        perror("socket");
        return;
    }
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(udpPort);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(udpSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("bind");
        return;
    }
    printf("UDP listener started on port %d\n", udpPort);

    this->lora = new LoRa(SPI_CHANNEL, SS_PIN, DIO0_PIN, RST_PIN);
    if (lora->begin()) {
        printf("LoRa setup successful: chipset version 0x%02x\n", lora->version());
        lora->setFrequency(927500000)
                ->setTXPower(17)
                ->setSpreadFactor(LoRa::SF_7)
                ->setBandwidth(LoRa::BW_500k)
                ->setCodingRate(LoRa::CR_45)
                ->setSyncWord(0x33)
                ->setHeaderMode(LoRa::HM_EXPLICIT)
                ->enableCRC();
        printf("  TX power     : %d dB\n", lora->getTXPower());
        printf("  Frequency    : %d Hz\n", lora->getFrequency());
        printf("  Spread factor: %d\n", lora->getSpreadFactor());
        printf("  Bandwidth    : %d Hz\n", lora->bw[lora->getBandwidth()]);
        printf("  Coding Rate  : 4/%d\n", lora->getCodingRate() + 4);
        printf("  Sync word    : 0x%02x\n", lora->getSyncWord());
        printf("  Header mode  : %s\n", lora->getHeaderMode() == LoRa::HM_IMPLICIT ? "Implicit" : "Explicit");
        printf("Transmitting packet\n");
    }
}

void sendPayload::getSensorData() {
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int bytesRead = recvfrom(udpSocket, this->sensorData, sizeof(this->sensorData) - 1, 0, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (bytesRead == -1) {
        perror("recvfrom");
    } else {
        this->sensorData[bytesRead] = '\0';
        printf("Received from %s: %s\n", inet_ntoa(clientAddr.sin_addr), this->sensorData);
    }
}

void sendPayload::start() {
    while(true){
//        for (int i = 0; i < 16; ++i) {
//            this->getSensorData();
//            this->sendMsg(this->sensorData);
//        }
        this->sendImage(this->getImage());
    }
}