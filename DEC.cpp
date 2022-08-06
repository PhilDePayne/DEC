#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#define LENGTH 16
#define WIDTH 8

using namespace std;

string readFileBytes(string fileName)
{
    std::ifstream input(fileName, std::ios::binary);

    std::vector<char> bytes(
        (std::istreambuf_iterator<char>(input)),
        (std::istreambuf_iterator<char>()));

    string ret(bytes.begin(), bytes.end());

    return ret;
}

void setBit(char byte, int pos, bool value) {

    switch (value) {

    case(1):
        byte |= 0x01 << pos;
        break;

    case(0):
        byte &= ~(0x01 << pos);
        break;
    }

}

char* charToBin(char c) {

    static char bin[CHAR_BIT + 1] = { 0 };
    int i;

    for (i = CHAR_BIT - 1; i >= 0; i--)
    {
        bin[i] = (c % 2) + '0';
        c /= 2;
    }

    return bin;
}

char binStr2Char(string bin) {

    char ret;

    ret = static_cast<char>(stoi(bin, nullptr, 2));

    return ret;

}

vector<bool> parity(char msg, int tab[][LENGTH]) {

    int sum = 0;
    vector<bool> ret;

    for (int i = 0; i < WIDTH; i++) {

        sum = 0;

        for (int j = 7; j >= 0; j--) {

            sum += (int)((msg >> (7 - j)) & 0x1) * tab[i][j];

        }

        ret.push_back(sum % 2 == 1);
    }

    return ret;
}

void encode(string msg, int tab[][LENGTH]) {

    ofstream file;
    file.open("encoded");

    for (size_t i = 0; i < msg.length(); i++) {

        vector<bool> check = parity(msg[i], tab);
        file << charToBin(msg[i]);
        for (const auto& e : check) file << e;

    }

}

vector<bool> multiply(string sub, int tab[][LENGTH]) {

    vector<bool> ret;
    int sum = 0;

    for (int i = 0; i < WIDTH; i++) {

        for (int j = 0; j < LENGTH; j++) {

            sum += (int)(sub[j] - 48) * tab[i][j];
        }

        ret.push_back(sum % 2);

        sum = 0;
    }

    return ret;

}

bool isZero(vector<bool> v) {

    for (const auto& e : v) if (e) return false;

    return true;

}

vector<bool> getRow(int tab[][LENGTH], int i) {

    vector<bool> ret;

    for (int j = 0; j < WIDTH; j++) {

        ret.push_back((bool)tab[j][i]);

    }

    return ret;

}

vector<bool> sum(vector<bool> m1, vector<bool> m2) {

    vector<bool> ret;

    for (int i = 0; i < WIDTH; i++) {

        ret.push_back((m1[i] + m2[i]) % 2);

    }


    return ret;

}

/*
Returns TRUE if 2 errors occured, FALSE otherwise.
*/

bool findError(vector<bool> sub, int tab[][LENGTH], int& pos1, int& pos2) {

    for (int i = 0; i < LENGTH; i++) {

        if (sub == getRow(tab, i)) {

            pos1 = i;

            return false;

        }

    }

    for (int i = 0; i < LENGTH; i++) {

        for (int j = i + 1; j < LENGTH; j++) {

            if (sub == sum(getRow(tab, i),getRow(tab,j))) {

                pos1 = i;

                pos2 = j;

                return true;

            }

        }

    }

    return false;

}

void decode(string msg, int tab[][LENGTH]) {

    ofstream fl;

    fl.open("corrected", ios::out);

    for (int i = 0; i < msg.length() / LENGTH; i++) {

        string substring = msg.substr(LENGTH * i, LENGTH);

        vector<bool> check = multiply(substring, tab);

        if (!isZero(check)) {

            int pos1, pos2;

            bool doubleError = false;

            if (findError(check, tab, pos1, pos2)) {

                substring[pos1] == '1' ? substring[pos1] = '0' : substring[pos1] = '1';
                substring[pos2] == '1' ? substring[pos2] = '0' : substring[pos2] = '1';

            }
            else {

                substring[pos1] == '1' ? substring[pos1] = '0' : substring[pos1] = '1';

            }

        }

        fl << binStr2Char(substring.substr(0, 8));

    }

}

int main()
{

    int tab1[WIDTH][LENGTH] = {
    {0,1,0,1,1,0,0,1,1,0,0,0,0,0,0,0},
    {1,0,1,0,1,1,0,0,0,1,0,0,0,0,0,0},
    {0,1,0,1,0,1,1,0,0,0,1,0,0,0,0,0},
    {0,0,1,0,1,0,1,1,0,0,0,1,0,0,0,0},
    {1,0,0,1,0,1,0,1,0,0,0,0,1,0,0,0},
    {1,1,0,0,1,0,1,0,0,0,0,0,0,1,0,0},
    {0,1,1,0,0,1,0,1,0,0,0,0,0,0,1,0},
    {1,0,1,1,0,0,1,0,0,0,0,0,0,0,0,1},
    };
    size_t msgLen = 0;

    string byteMessage;

    string file;

    cout << "Enter the name of the file you wish to encode\n";

    cin >> file;

    byteMessage = readFileBytes(file);

    encode(byteMessage, tab1);

    cin.ignore();

    cout << "Simulate a transmission error by changing up to 2 digits in every 8 in \"encoded\", then press a key to continue\n";

    getchar();

    fstream encoded;

    encoded.open("encoded", ios::in);

    string encodedMessage;

    encoded >> encodedMessage;

    decode(encodedMessage, tab1);

    encoded.close();

}