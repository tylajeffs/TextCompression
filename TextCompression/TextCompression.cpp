#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <map>

using namespace std;

class Node {
public:
    char ch;
    int freq;
    bool isLeafNode;
    Node* left, * right;

    Node() {
    }
    //If Node is created with only 2 parameter, the node is a leafnode
    Node(char ch, int freq) {
        this->ch = ch;
        this->freq = freq;
        this->left = nullptr;
        this->right = nullptr;
        this->isLeafNode = true;

    }
    //If Node is created with only 4 parameter,which means it has child node in left and right, the node is a parent node 
    Node(char ch, int freq, Node* left, Node* right) {
        this->ch = ch;
        this->freq = freq;
        this->left = left;
        this->right = right;
        this->isLeafNode = false;
    }
};

//For sorting Node in ascending order based on Frequency
struct LessFreq
{
    inline bool operator() (const Node* struct1, const Node* struct2)
    {
        return (struct1->freq < struct2->freq);
    }
};

//Get Total Number of Bits
int computeBits(map<char, int> freq, map<char, string> huffmanCode) {
    int totalBits = 0;
    for (auto pair : huffmanCode) {
        totalBits += pair.second.length() * freq[pair.first];
    }
    return totalBits;
}

//Get Encoded Binary String
string getBinaryString(map<char, string> huffmanCode, string plaintext) {
    string binary;
    for (int i = 0; i < plaintext.size(); i++) {
        binary += huffmanCode[plaintext[i]];
    }
    return binary;
}

//Traverse to each Node in Huffman Tree starting from root node going to the leaf node to get the huffman code
void getHuffmanCode(Node* root, string str, map<char, string>& huffmanCode) {
    //If root is Null then return.
    if (!root) {
        return;
    }
    //If Node is a leafNode, which means it is the last node, save the str as huffman code
    if (root->isLeafNode) {
        huffmanCode[root->ch] = str;
    }
    else {

        getHuffmanCode(root->left, str + "0", huffmanCode); //Node is still a parent node, traverse going left and concatinate 0 to code
        getHuffmanCode(root->right, str + "1", huffmanCode);//Node is still a parent node, traverse going right and concatinate 0 to code
    }
}

vector<Node*> nodesArray;
vector<Node*> nodesQueue;
void buildHuffmanTree(string plaintext, string filename)
{
    //Key Value Pair Map where Key is the Character and Value is the Frequency
    //Iterate through each character in plain text and get each character frequency.
    //If char key in freq[char], increment value by 1
    map<char, int> freq;
    for (int i = 0; i < plaintext.size(); i++) {
        freq[plaintext[i]]++;
    }

    //Iterate through all char in freq key value pair
    //This will insert each char as leaf node (isLeafNode = true) in nodesArray then insert them in nodesQueue
    for (auto pair : freq) {
        //pair.first = Key/Char while pair.second = Value/Frequency count
        Node newNode(pair.first, pair.second);
        nodesArray.push_back(new Node(pair.first, pair.second));
        Node* temp = nodesArray[nodesArray.size() - 1];
        nodesQueue.push_back(temp);
    }

    //Iterate through the queue until only the root node remains which is nodesQueue[0] where nodesQueue size = 0
    while (nodesQueue.size() != 1) {
        //Sort the queue by frequency in ascending order
        sort(nodesQueue.begin(), nodesQueue.end(), LessFreq());
        Node* left = nodesQueue[0]; // extract minimum node (lowest freq in queue) and put it to left variable
        nodesQueue.erase(nodesQueue.begin()); // delete the minimum node from queue
        Node* right = nodesQueue[0];  // extract minimum node (lowest freq in queue) and put it to right variable
        nodesQueue.erase(nodesQueue.begin()); // delete the minimum node from queue
        int sum = left->freq + right->freq; //add left and right node frequency

        Node newNode('\0', sum, left, right); //Create New Node where isLeafNode = false which means it is a parent node
        nodesArray.push_back(new Node('\0', sum, left, right)); //insert to nodeArray
        Node* temp = nodesArray[nodesArray.size() - 1]; //get the newly inserted node in nodeArray
        nodesQueue.push_back(temp); //insert to queue
    } //If only the root node remain, while loop stop

    //Get the last remaining node in queue and use it as root node
    Node* root = nodesQueue[0];

    //Get HuffManCode Code 
    map<char, string> huffmanCode;
    getHuffmanCode(root, "", huffmanCode);

    //Create File for output
    ofstream MyFile(filename + ".zip301");
    for (auto pair : huffmanCode) {
        if (pair.first == '\n') {
            MyFile << pair.second << " newline" << endl;
        }
        else if (pair.first == ' ') {
            MyFile << pair.second << " space" << endl;
        }
        else if (pair.first == '\r') {
            MyFile << pair.second << " return" << endl;
        }
        else if (pair.first == '\t') {
            MyFile << pair.second << " tab" << endl;
        }
        else {
            MyFile << pair.second << " " << pair.first << endl;
        }
    }
    MyFile << "*****" << endl;
    int totalBytes = computeBits(freq, huffmanCode); // get total bits
    MyFile << totalBytes << endl;

    string binary = getBinaryString(huffmanCode, plaintext);

    int getCount = binary.length() / 8; //get the number of 8 bits binary that we can get from encoded binary string
    int lastSubstringLength = binary.length() % 8; //check if last substring is exactly 8 bits
    int j = 0;
    for (int i = 0; i < getCount; i++) {
        char ch;
        int num = stoi(binary.substr(j, 8), 0, 2); //convert 8bits sub binary to int
        ch = num; //convert int to char or raw bytes
        MyFile << ch; //print to file
        j += 8;
    }
    //If last sub binary is not 8 bits. Add succeeding 0 to the end
    if (lastSubstringLength > 0) {
        int neededZero = 8 - lastSubstringLength;
        string lastBinary = binary.substr(j, lastSubstringLength);
        for (int i = 0; i < neededZero; i++) {
            lastBinary = lastBinary + "0";
        }
        char ch;
        int num = stoi(binary.substr(j, 8), 0, 2); //convert 8bits sub binary to int
        ch = num;//convert int to char or raw bytes
        MyFile << ch;
    }
    cout << "Zip301 Generated.\n";
}

int main(int argc, char** argv)
{
    //Catches error conditions Check if filename included as parameter
    if (argc != 2) {
        cout << "You need to include a filename on the command line.\n";
        return -1;
    }

    string filename = argv[1];//Fetches the file name
    fstream file(filename, ios::in);

    string delimiter = ".";
    string name = filename.substr(0, filename.find(delimiter));
    string extension = filename.substr(1, filename.find(delimiter));

    //Catches error conditions Check if file exist
    if (!file) {
        cout << "Error opening file: " << filename << endl;
        return -1;
    }
    string input;//store the inputs or the lines from the file
    string file_contents;
    //getline(file, input);
    bool firstLine = true;
    while (!file.eof())
    {
        getline(file, input);
        //Check if the line being read is NOT the first line
        if (!firstLine) {
            //if Not first line in file add \n for newline
            file_contents.push_back('\n');
        }
        else { //If First line in file, make firstline to false
            firstLine = false;
        }
        //Add to file_content to get the whole plain text in file
        file_contents += input;
    }

    buildHuffmanTree(file_contents, name);
    file.close();
    return 0;
}