import React, { useState, useEffect } from 'react';

export default function App() {
  const [inputText, setInputText] = useState('Huffman coding is amazing! Type your text here to see real-time compression.');
  const [stats, setStats] = useState(null);
  const [codes, setCodes] = useState({});
  const [freqMap, setFreqMap] = useState({});

  // Jab bhi text change hoga, ye function chalega
  useEffect(() => {
    if (!inputText) {
      setStats(null);
      setCodes({});
      setFreqMap({});
      return;
    }
    compressText(inputText);
  }, [inputText]);

  const compressText = (text) => {
    // 1. Frequency Calculate karna
    const freq = {};
    for (let char of text) {
      freq[char] = (freq[char] || 0) + 1;
    }
    setFreqMap(freq);

    // 2. Nodes banana
    let nodes = Object.keys(freq).map((char) => ({
      char,
      freq: freq[char],
      left: null,
      right: null,
    }));

    // Single character edge case
    if (nodes.length === 1) {
       setCodes({ [nodes[0].char]: "0" });
       setStats({
         original: text.length * 8,
         compressed: text.length,
         saved: (((text.length * 8 - text.length) / (text.length * 8)) * 100).toFixed(2)
       });
       return;
    }

    // 3. Tree banana (Min-Heap logic)
    while (nodes.length > 1) {
      nodes.sort((a, b) => a.freq - b.freq); // Sort to get minimums
      const left = nodes.shift();
      const right = nodes.shift();
      nodes.push({ char: null, freq: left.freq + right.freq, left, right });
    }

    const root = nodes[0];

    // 4. Codes Generate karna
    const generatedCodes = {};
    const traverse = (node, path) => {
      if (!node) return;
      if (node.char !== null) {
        generatedCodes[node.char] = path;
      }
      traverse(node.left, path + '0');
      traverse(node.right, path + '1');
    };
    traverse(root, '');
    setCodes(generatedCodes);

    // 5. Statistics Calculate karna
    const originalBits = text.length * 8; // 1 char = 8 bits
    let compressedBits = 0;
    for (let char of text) {
      compressedBits += generatedCodes[char].length;
    }
    
    // Header padding assumptions for realistic stats
    const headerBits = Object.keys(freq).length * 10; 
    const totalCompressed = compressedBits + headerBits;

    const savedPercent = (((originalBits - totalCompressed) / originalBits) * 100).toFixed(2);

    setStats({
      original: originalBits,
      compressed: totalCompressed,
      saved: savedPercent > 0 ? savedPercent : 0,
    });
  };

  return (
    <div className="min-h-screen bg-slate-900 text-white p-4 sm:p-8 font-sans">
      <div className="max-w-4xl mx-auto space-y-8">
        
        {/* Header Section */}
        <div className="text-center space-y-2">
          <h1 className="text-4xl font-bold text-blue-400">Huffman Compressor Web</h1>
          <p className="text-slate-400">Real-time text compression simulator</p>
        </div>

        {/* Input Text Box */}
        <div className="bg-slate-800 p-6 rounded-xl shadow-lg border border-slate-700">
          <label className="block text-sm font-medium text-slate-300 mb-2">
            Enter your text here:
          </label>
          <textarea
            className="w-full h-32 p-4 bg-slate-950 border border-slate-600 rounded-lg focus:ring-2 focus:ring-blue-500 focus:outline-none text-slate-100"
            value={inputText}
            onChange={(e) => setInputText(e.target.value)}
            placeholder="Type something to compress..."
          />
        </div>

        {/* Performance Dashboard */}
        {stats && (
          <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
            <div className="bg-slate-800 p-6 rounded-xl text-center border border-slate-700">
              <div className="text-slate-400 text-sm mb-1">Original Size</div>
              <div className="text-3xl font-bold text-red-400">{stats.original} <span className="text-lg">bits</span></div>
            </div>
            <div className="bg-slate-800 p-6 rounded-xl text-center border border-slate-700">
              <div className="text-slate-400 text-sm mb-1">Compressed Size</div>
              <div className="text-3xl font-bold text-green-400">{stats.compressed} <span className="text-lg">bits</span></div>
            </div>
            <div className="bg-slate-800 p-6 rounded-xl text-center border border-slate-700">
              <div className="text-slate-400 text-sm mb-1">Space Saved</div>
              <div className="text-3xl font-bold text-blue-400">{stats.saved}%</div>
            </div>
          </div>
        )}

        {/* Huffman Dictionary Table */}
        {Object.keys(codes).length > 0 && (
          <div className="bg-slate-800 rounded-xl overflow-hidden shadow-lg border border-slate-700">
             <div className="px-6 py-4 border-b border-slate-700 bg-slate-800">
              <h2 className="text-lg font-semibold text-slate-200">Huffman Dictionary (Codes)</h2>
            </div>
            <div className="overflow-x-auto">
              <table className="w-full text-left text-sm text-slate-300">
                <thead className="bg-slate-700 text-slate-200">
                  <tr>
                    <th className="px-6 py-3 font-medium">Character</th>
                    <th className="px-6 py-3 font-medium">Frequency</th>
                    <th className="px-6 py-3 font-medium">Huffman Code</th>
                  </tr>
                </thead>
                <tbody className="divide-y divide-slate-700">
                  {Object.keys(codes).map((char, index) => (
                    <tr key={index} className="hover:bg-slate-700/50 transition-colors">
                      <td className="px-6 py-3 font-mono">
                        {char === ' ' ? '[Space]' : char === '\n' ? '[Enter]' : char}
                      </td>
                      <td className="px-6 py-3">{freqMap[char]}</td>
                      <td className="px-6 py-3 font-mono text-green-400">{codes[char]}</td>
                    </tr>
                  ))}
                </tbody>
              </table>
            </div>
          </div>
        )}

      </div>
    </div>
  );
}