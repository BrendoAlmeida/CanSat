#include "packets.h"

namespace std{
    vector<unsigned char> packets::readImageToBytes(const string& filename)
    {
        ifstream file(filename, ios::binary);
        vector<unsigned char> bytes;

        if (file.is_open()) {
            bytes.assign((istreambuf_iterator<char>(file)), (istreambuf_iterator<char>()));
            file.close();
            return bytes;
        }

        cout << "Erro ao abrir a imagem" << filename << endl;
        return bytes;
    }

    vector<vector<unsigned char>> packets::splitBytesIntoPackets(const vector<unsigned char>& bytes)
    {
        vector<vector<unsigned char>> packets;
        const size_t packetSize = 243;

        for (size_t i = 0; i < bytes.size(); i += packetSize) {
            vector<unsigned char> packet(bytes.begin() + i, bytes.begin() + i + min(packetSize, bytes.size() - i));
            packets.push_back(packet);
        }

        return packets;
    }

    void packets::joinPackets(vector<vector<unsigned char>> packets)
    {
        ofstream file("../testeReescrito.jpg", ios::binary);

        for (const auto& packet : packets) {
            file.write(reinterpret_cast<const char*>(packet.data()), packet.size());
        }
        file.close();
    }

    void packets::splitPackets()
    {
        const experimental::filesystem::path filePath{"../../cameraOS/imagens/compressed"};
        for (auto const& dir_entry : experimental::filesystem::recursive_directory_iterator{filePath} {
            std::cout << dir_entry << '\n';
            vector<unsigned char> imageBytes = readImageToBytes(dir_entry);
            vector<vector<unsigned char>> packets = splitBytesIntoPackets(imageBytes);

            cout << "Imagem lida com sucesso. Número de pacotes: " << packets.size() << endl;

            for (const auto& packet : packets) {
                cout << "Pacote de " << packet.size() << " bytes" << endl;
            }

            joinPackets(packets);
        }
    }
}