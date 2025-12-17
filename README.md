# ArmToHack

A powerful ARM assembly to Hack assembly translator that converts ARM instruction set programs into Hack machine language code. This compiler enables ARM programs to run on the Hack computer platform through a sophisticated two-pass translation system.

## 🚀 Features

- **Complete ARM Instruction Support**: Translates a comprehensive subset of ARM assembly instructions
- **Two-Pass Translation**: Handles forward references and labels through an efficient two-pass compilation process
- **Memory Management**: Automatic stack initialization and memory allocation
- **Label Resolution**: Full support for labels, branches, and function calls
- **Variable Handling**: Processes data declarations (DCD) and variable assignments
- **Register Mapping**: Seamless translation between ARM and Hack register architectures

## 📋 Supported ARM Instructions

### Arithmetic Operations
- `MOV` - Move/load immediate values
- `ADD` - Addition
- `SUB` - Subtraction  
- `RSB` - Reverse subtraction
- `CMP` - Compare (sets condition flags)
- `ASR` - Arithmetic shift right

### Memory Operations
- `LDR` - Load from memory (supports immediate offsets, register offsets, and literal loads)
- `STR` - Store to memory (supports immediate and register offsets)
- `LDMIB` - Load multiple registers (increment before)
- `STMDA` - Store multiple registers (decrement after)

### Control Flow
- `BL` - Branch with link (function calls)
- `BEQ` - Branch if equal
- `BNE` - Branch if not equal
- `BGT` - Branch if greater than
- `BLT` - Branch if less than
- `BGE` - Branch if greater than or equal
- `BLE` - Branch if less than or equal
- `BAL` - Branch always (unconditional jump)
- `END` - Program termination

### Data Declarations
- `DCD` - Define constant data (arrays and variables)



## 🔧 Building

### Prerequisites
- C++11 compatible compiler (GCC, Clang, or MSVC)
- Standard C++ library

### Compilation

```bash
g++ -o main main.cpp ArmToHack.cpp token_io.cpp -std=c++11
```

Or using Clang:

```bash
clang++ -o main main.cpp ArmToHack.cpp token_io.cpp -std=c++11
```

## 💻 Usage

### Running the Test Suite

The main program automatically translates all test programs:

```bash
./main
```

This will process all ARM files in the `test/` directory and generate corresponding `.asm` files.

### Programmatic Usage

```cpp
#include "ArmToHack.h"

int main() {
    ArmToHack translator;
    
    // Translate a single ARM file to Hack assembly
    translator.convertFile("input.arm", "output.asm");
    
    return 0;
}
```

## 📖 Example Translation

### ARM Input (`example.arm`)
```arm
        MOV     R1, #5
        MOV     R2, #10
        ADD     R3, R1, R2
        STR     R3, [SP, #0]
        LDR     R4, [SP, #0]
```


## 🎯 Architecture Details

### Register Mapping

| ARM Register | Hack Address | Description                          |
| ------------ | ------------ | ------------------------------------ |
| R0-R15       | 0-15         | General purpose registers            |
| FP           | 12           | Frame pointer                        |
| SP           | 13           | Stack pointer (initialized to 16380) |
| LR           | 14           | Link register                        |
| PC           | 15           | Program counter                      |

### Memory Layout

- **Stack**: Starts at address 16380
- **Variables**: Allocated starting at address 16
- **Temporary**: Register 15 (R15) used for temporary calculations
- **Scratch**: Register 16 used for intermediate operations

### Translation Process

1. **First Pass**:
   - Parses ARM source code
   - Builds label and variable symbol tables
   - Generates intermediate Hack assembly with placeholder addresses
   - Handles forward references

2. **Second Pass**:
   - Resolves all label references
   - Replaces placeholder addresses with actual line numbers
   - Produces final executable Hack assembly

## 🔍 Key Implementation Features

- **Smart Address Calculation**: Handles immediate offsets, register offsets, and complex addressing modes
- **Conditional Branching**: Translates ARM condition codes to Hack jump instructions
- **Stack Operations**: Efficient implementation of LDMIB and STMDA for stack manipulation
- **Literal Loading**: Support for `LDR Rd, =label` syntax for loading variable addresses
- **Arithmetic Shift**: Complete ASR implementation using stack-based iterative algorithm

## 📝 Notes

- The translator uses a two-pass approach to handle forward references
- All generated code is compatible with the Hack computer architecture
- Stack pointer is automatically initialized on program start
- Comments in ARM source (starting with `;`) are preserved during parsing


**Note**: This translator implements a subset of ARM assembly instructions optimized for the Hack computer platform. For production use, additional instruction support may be required.

