# Chip 8 Emulator
## Installation
First, clone the repository
```
git clone https://github.com/Herotank1234/chip8-emulator.git
`````
Then change directory into the repository
```
cd chip8-emulator
```
Make a directory called *build* and run cmake inside it
```
mkdir build
cd build
cmake ..
```
This should create an executable which can be run by
```
./chip_8_emulator
```

## Usage
To run a Chip 8 ROM, use
```
./chip_8_emulator <PATH TO ROM>
```
For more information on flags to toggle quirks, use
```
./chip_8_emulator --help
```