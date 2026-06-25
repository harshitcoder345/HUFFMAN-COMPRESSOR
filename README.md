 Huffman Text Compressor (C++ & React)

An interactive, end-to-end Text Compression tool built using the Huffman Coding Algorithm. This project features a high-performance C++ Command Line Interface (CLI) and a modern, real-time React Web Application with live statistics.

Key Features

-Real-time Compression: See immediate results and compression ratios as you type.

-Performance Dashboard: Visualizes Original Size (bits), Compressed Size (bits), and exact Space Saved (%).

-Huffman Dictionary Table: Dynamically generates and displays binary codes (0s & 1s) for each character.

-Binary File Export (C++): Real-world compression that converts text files into actual compact .bin files and restores them perfectly.

🛠️ Tech Stack

-Frontend: React.js, Tailwind CSS (loaded via CDN)

-Backend: C++ (using STL, Standard File I/O, and Priority Queues)

-Algorithm: Huffman Coding (greedy algorithm using Min-Heap)

🚀 How to Run the Project

1. React Frontend (Web App)

Navigate to the web folder:
cd huffman-web

Install dependencies:
npm install

Start the development server:
npm start

2. C++ Backend (CLI Tool)

Compile the program:
g++ huffman_compressor.cpp -o compressor

Compress a text file:
.\compressor -c input.txt compressed.bin

Decompress back to text:
.\compressor -d compressed.bin output.txt

📂 Repository Structure

/huffman-web - Core React application files (UI & Simulator)

huffman_compressor.cpp - Optimized C++ backend file with execution-time and file-size trackers.

huffman.cpp - Basic tree building & core algorithm logic.

.gitignore - Pre-configured to prevent large binaries and unnecessary folders from being uploaded.
