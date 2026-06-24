#include <bits/stdc++.h>
#include <chrono> // Time track karne ke liye

using namespace std;
using namespace std::chrono;

// --- STEP 1 & 2: Node Structure and Min-Heap Rules ---
struct Node {
    char data;
    int freq;
    int first_pos;
    Node *left, *right;

    Node(char d, int f, int pos) {
        data = d;
        freq = f;
        first_pos = pos;
        left = right = NULL;
    }
};

struct Compare {
    bool operator()(Node* a, Node* b) {
        if (a->freq == b->freq) return a->first_pos > b->first_pos; 
        return a->freq > b->freq; 
    }
};

unordered_map<char, string> huffmanCodes;

// --- STEP 3: Generate Codes ---
void generateCodes(Node* root, string code) {
    if (root == NULL) return;
    if (root->data != '$') huffmanCodes[root->data] = code;
    generateCodes(root->left, code + "0");
    generateCodes(root->right, code + "1");
}

// --- STEP 4: Build Tree ---
Node* buildHuffmanTree(unordered_map<char, int>& freqMap, unordered_map<char, int>& firstPosMap) {
    priority_queue<Node*, vector<Node*>, Compare> pq;
    for (auto pair : freqMap) {
        pq.push(new Node(pair.first, pair.second, firstPosMap[pair.first]));
    }
    if(pq.empty()) return NULL;

    while (pq.size() > 1) {
        Node* leftNode = pq.top(); pq.pop();
        Node* rightNode = pq.top(); pq.pop();
        int min_pos = min(leftNode->first_pos, rightNode->first_pos);
        Node* parent = new Node('$', leftNode->freq + rightNode->freq, min_pos);
        parent->left = leftNode;
        parent->right = rightNode;
        pq.push(parent);
    }
    return pq.top(); 
}

// --- Encode with Header ---
void compressFile(string inputFile, string outputFile) {
    auto start = high_resolution_clock::now(); // Timer Start

    ifstream inFile(inputFile, ios::binary);
    if (!inFile) { cout << "Error: Cannot open input file!\n"; return; }

    // Original file size nikalna
    inFile.seekg(0, ios::end);
    long long originalSize = inFile.tellg();
    inFile.seekg(0, ios::beg);

    if (originalSize == 0) {
        cout << "Error: Input file is empty!\n";
        return;
    }

    // 1. Calculate Frequency
    unordered_map<char, int> freqMap;
    unordered_map<char, int> firstPosMap;
    char ch;
    int pos = 0;
    while (inFile.get(ch)) {
        freqMap[ch]++;
        if (firstPosMap.find(ch) == firstPosMap.end()) firstPosMap[ch] = pos;
        pos++;
    }
    inFile.clear(); inFile.seekg(0); // Reset file pointer

    // 2. Build Tree & Codes
    Node* root = buildHuffmanTree(freqMap, firstPosMap);
    generateCodes(root, "");

    // 3. Write Header & Data to .bin
    ofstream outFile(outputFile, ios::binary);
    
    // Save Metadata (Header) so decoder can rebuild tree
    int uniqueChars = freqMap.size();
    outFile.write(reinterpret_cast<const char*>(&uniqueChars), sizeof(uniqueChars));
    for (auto pair : freqMap) {
        outFile.write(&pair.first, sizeof(pair.first));
        outFile.write(reinterpret_cast<const char*>(&pair.second), sizeof(pair.second));
        int fpos = firstPosMap[pair.first];
        outFile.write(reinterpret_cast<const char*>(&fpos), sizeof(fpos));
    }

    // Prepare valid bits count space
    long long totalValidBitsPos = outFile.tellp();
    long long totalValidBits = 0;
    outFile.write(reinterpret_cast<const char*>(&totalValidBits), sizeof(totalValidBits));

    // Encode actual file data
    unsigned char currentByte = 0;
    int bitCount = 0;
    while (inFile.get(ch)) {
        string code = huffmanCodes[ch];
        for (char bit : code) {
            currentByte = currentByte << 1;
            if (bit == '1') currentByte = currentByte | 1;
            bitCount++;
            totalValidBits++;

            if (bitCount == 8) {
                outFile.write(reinterpret_cast<const char*>(&currentByte), 1);
                currentByte = 0; bitCount = 0;
            }
        }
    }
    if (bitCount > 0) {
        currentByte = currentByte << (8 - bitCount);
        outFile.write(reinterpret_cast<const char*>(&currentByte), 1);
    }

    // Update total valid bits in header
    outFile.seekp(totalValidBitsPos);
    outFile.write(reinterpret_cast<const char*>(&totalValidBits), sizeof(totalValidBits));

    long long compressedSize = outFile.tellp(); // Compressed file size nikalna

    inFile.close(); outFile.close();

    auto stop = high_resolution_clock::now(); // Timer Stop
    auto duration = duration_cast<milliseconds>(stop - start);

    // --- DASHBOARD PRINT KARNA ---
    cout << "\n=============================================\n";
    cout << "      TEXT COMPRESSION STATISTICS REPORT     \n";
    cout << "=============================================\n";
    cout << " Original Text Size   : " << originalSize << " Bytes\n";
    cout << " Compressed File Size : " << compressedSize << " Bytes\n";
    double saved = ((double)(originalSize - compressedSize) / originalSize) * 100;
    cout << " Space Saved          : " << (saved > 0 ? saved : 0) << " %\n";
    cout << " Execution Time       : " << duration.count() << " ms\n";
    cout << "=============================================\n\n";
    cout << "Compression Successful! Saved as " << outputFile << "\n";
}

// --- Decode from Standalone File ---
void decompressFile(string compressedFile, string outputFile) {
    auto start = high_resolution_clock::now(); // Timer Start

    ifstream inFile(compressedFile, ios::binary);
    if (!inFile) { cout << "Error: Cannot open compressed file!\n"; return; }

    // 1. Read Header and Rebuild Tree
    int uniqueChars;
    inFile.read(reinterpret_cast<char*>(&uniqueChars), sizeof(uniqueChars));
    
    unordered_map<char, int> freqMap;
    unordered_map<char, int> firstPosMap;
    for (int i = 0; i < uniqueChars; i++) {
        char ch; int freq, fpos;
        inFile.read(&ch, sizeof(ch));
        inFile.read(reinterpret_cast<char*>(&freq), sizeof(freq));
        inFile.read(reinterpret_cast<char*>(&fpos), sizeof(fpos));
        freqMap[ch] = freq;
        firstPosMap[ch] = fpos;
    }

    Node* root = buildHuffmanTree(freqMap, firstPosMap);
    
    long long totalValidBits;
    inFile.read(reinterpret_cast<char*>(&totalValidBits), sizeof(totalValidBits));

    // 2. Decode Bits
    ofstream outFile(outputFile, ios::binary);
    unsigned char currentByte;
    Node* curr = root;
    long long bitsRead = 0;

    while (inFile.read(reinterpret_cast<char*>(&currentByte), 1)) {
        for (int i = 7; i >= 0; i--) {
            if (bitsRead >= totalValidBits) break;
            int bit = (currentByte >> i) & 1;
            
            if (bit == 0) curr = curr->left;
            else curr = curr->right;

            if (curr->left == NULL && curr->right == NULL) {
                outFile.put(curr->data);
                curr = root; 
            }
            bitsRead++;
        }
    }
    inFile.close(); outFile.close();

    auto stop = high_resolution_clock::now(); // Timer Stop
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "Decompression Successful! Text Saved as " << outputFile << "\n";
    cout << "Execution Time       : " << duration.count() << " ms\n";
}

// --- COMMAND LINE INTERFACE ---
int main(int argc, char* argv[]) {
    if (argc != 4) {
        cout << "Usage:\n";
        cout << "To Compress:   ./compressor -c input.txt compressed.bin\n";
        cout << "To Decompress: ./compressor -d compressed.bin output.txt\n";
        return 1;
    }

    string mode = argv[1];
    string inputFile = argv[2];
    string outputFile = argv[3];

    // Text file check (Sirf text files allow karne ke liye)
    if (mode == "-c" && inputFile.find(".txt") == string::npos) {
        cout << "Error: Sirf .txt files allowed hain!\n";
        return 1;
    }

    if (mode == "-c") {
        cout << "Compressing text file...\n";
        compressFile(inputFile, outputFile);
    } else if (mode == "-d") {
        cout << "Decompressing file...\n";
        decompressFile(inputFile, outputFile);
    } else {
        cout << "Invalid mode! Use -c for compression or -d for decompression.\n";
    }

    return 0;
}