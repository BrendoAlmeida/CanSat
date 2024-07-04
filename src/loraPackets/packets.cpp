#include "packets.h"

using namespace std;

vector<unsigned char> packets::readImageToBytes(const string& filename)
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

vector<vector<unsigned char>> packets::splitBytesIntoPackets(const vector<unsigned char>& bytes)
{
    vector<vector<unsigned char>> packets;
    const size_t packetSize = 250;

    for (size_t i = 0; i < bytes.size(); i += packetSize) {
        vector<unsigned char> packet(bytes.begin() + i, bytes.begin() + i + min(packetSize, bytes.size() - i));
        packets.push_back(packet);
    }

    return packets;
}

vector<string> packets::listFiles(const string& folderPath) {
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

string packets::getLastFile(const vector<string>& files) {
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

void packets::splitPackets() {
    const string folderPath = "/home/Embauba/CanSat/cameraOS/imagens/compressed";

    vector<string> files = listFiles(folderPath);
    string lastFile = getLastFile(files);

    cout << "Last file: " << lastFile << '\n';
    vector<unsigned char> imageBytes = readImageToBytes(lastFile);
    vector<vector<unsigned char>> packets = splitBytesIntoPackets(imageBytes);

    cout << "Image read successfully. Number of packets: " << packets.size() << endl;

    for (const auto& packet : packets) {
//        cout << "Packet of " << packet.size() << " bytes" << endl;

    }
}
