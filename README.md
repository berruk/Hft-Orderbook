# Hft-Orderbook

## Comparison of data structures' performances as a container for an HFT Orderbook.
Parsing Borsa Istanbul orders captured in a pcap file with Wireshark, then using [ITCH protocol](https://www.borsaistanbul.com/files/bistech-itch-protocol-specification.pdf) and storing in 4 different containers to measure performance of each. <br />
Average run times came out to be: <br />
### 1.Linked list  <br />
3.017533142857143 s
### 2.Hash  <br />
2.9160078571428576 s
### 3.Rbt <br />
2.9219570666666668 s
### 4.Heap
2.9788327333333333 s
<br />

Input file is not uploaded due to data privacy. <br />
