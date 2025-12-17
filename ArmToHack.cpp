#include "ArmToHack.h"
#include <vector>
#include <cmath>

using namespace std;

ArmToHack::ArmToHack() : line_number(0), current_memory_location(16) {
    for (int i = 0; i <= 15; i++) {
        string reg = "R" + to_string(i);
        register_map[reg] = i;
    }

    register_map["FP"] = 12;
    register_map["SP"] = 13;
    register_map["LR"] = 14;
    register_map["PC"] = 15;
    
    jump_map["BEQ"] = "JEQ";  
    jump_map["BNE"] = "JNE";  
    jump_map["BGT"] = "JGT";  
    jump_map["BLT"] = "JLT";  
    jump_map["BGE"] = "JGE";  
    jump_map["BLE"] = "JLE"; 
    jump_map["BAL"] = "JMP";  
}

void ArmToHack::clearState() {
    line_number = 0;
    label_map.clear();
    hack_line_to_label_map.clear();
    variable_map.clear();
    current_memory_location = 16;
}

void ArmToHack::emitLine(const string& line) {
    if (output_stream.is_open()) {
        output_stream << line << endl;
        line_number++;
    }
}

void ArmToHack::convertFile(const string& in_filename, const string& out_filename) {
    string tmp_filename = in_filename + ".tmp";
    firstPass(in_filename, tmp_filename);
    secondPass(tmp_filename, out_filename);
}

void ArmToHack::firstPass(const string& in_filename, const string& out_filename) {
    clearState();
    
    input_stream.open(in_filename);
    output_stream.open(out_filename);
    
    if (!input_stream.is_open() || !output_stream.is_open()) {
        if (input_stream.is_open()) input_stream.close();
        if (output_stream.is_open()) output_stream.close();
        return;
    }
    
    initializeStack();
    
    string line;
    while (input_stream) {
        line = getNextLine(input_stream);
        if (line.empty()) {
            continue; 
        }
        
        string first_token = getFirstToken(line);
        string second_token = getSecondToken(line);
        
        bool is_instruction = (first_token == "MOV" || first_token == "ADD" || 
                              first_token == "SUB" || first_token == "RSB" || 
                              first_token == "CMP" || first_token == "END" ||
                              first_token == "LDR" || first_token == "STR" ||
                              first_token == "DCD" || first_token == "ASR" ||
                              jump_map.find(first_token) != jump_map.end());
        
        if (second_token == "DCD") {
            processData(line);
            continue;
        }
 
        if ((second_token.empty() || second_token[0] == ';') && !is_instruction) {
            label_map[first_token] = line_number;
            continue;
        }
        
        processInstruction(line);
    }
    
    input_stream.close();
    output_stream.close();
}

void ArmToHack::processInstruction(const string& line) {
    string instruction = getFirstToken(line);
    
    if (instruction == "MOV") {
        processMove(line);
    } else if (instruction == "ADD") {
        processAdd(line);
    } else if (instruction == "SUB") {
        processSubtract(line);
    } else if (instruction == "RSB") {
        processReverseSubtract(line);
    } else if (instruction == "CMP") {
        processCompare(line);
    } else if (instruction == "END") {
        processEnd(line);
    } else if (instruction == "BL" || jump_map.find(instruction) != jump_map.end()) {
        processBranch(line);
    } else if (instruction == "STMDA") {
        processStoreMultiple(line);
    } else if (instruction == "LDMIB") {
        processLoadMultiple(line);
    } else if (instruction == "LDR") {
        processLoad(line);
    } else if (instruction == "STR") {
        processStore(line);
    } else if (instruction == "DCD") {
        processData(line);
    } else if (instruction == "ASR") {
        processArithmeticShift(line);
    }
}

void ArmToHack::processMove(std::string line) {
    takeToken(line); 
    string dest = takeToken(line);
    string op = takeToken(line);

    evaluateOperand(op);

    if (register_map.find(dest) != register_map.end()) {
        int dest_addr = register_map[dest];
        emitLine("@" + to_string(dest_addr));
        emitLine("M=D");
    }
    
    handleProgramCounter(dest);
}

void ArmToHack::processArithmeticOp(std::string line, int opType) {
    takeToken(line);
    string dest = takeToken(line);
    string op1 = takeToken(line);
    string op2 = takeToken(line);

    evaluateOperand(op1);
    emitLine("@16");
    emitLine("M=D");

    evaluateOperand(op2);

    if (opType == 0) {
        emitLine("@16");
        emitLine("D=D+M");
    } else if (opType == 1) {
        emitLine("@16");
        emitLine("D=M-D");
    } else {
        emitLine("@16");
        emitLine("D=M-D");
    }

    if (register_map.find(dest) != register_map.end()) {
        int dest_addr = register_map[dest];
        emitLine("@" + to_string(dest_addr));
        emitLine("M=D");
    }
    
    handleProgramCounter(dest);
}

void ArmToHack::processAdd(std::string line) {
    processArithmeticOp(line, 0);
}

void ArmToHack::processSubtract(std::string line) {
    processArithmeticOp(line, 1);
}

void ArmToHack::processReverseSubtract(std::string line) {
    takeToken(line); 
    string dest = takeToken(line);
    string op1 = takeToken(line);
    string op2 = takeToken(line);

    evaluateOperand(op2);
    emitLine("@16");
    emitLine("M=D");

    evaluateOperand(op1);

    emitLine("@16");
    emitLine("D=M-D");

    if (register_map.find(dest) != register_map.end()) {
        int dest_addr = register_map[dest];
        emitLine("@" + to_string(dest_addr));
        emitLine("M=D");
    }
    
    handleProgramCounter(dest);
}

void ArmToHack::processCompare(std::string line) {
    takeToken(line); 
    string op1 = takeToken(line);
    string op2 = takeToken(line);

    evaluateOperand(op1);
    emitLine("@16");
    emitLine("M=D");

    evaluateOperand(op2);

    emitLine("@16");
    emitLine("D=M-D");
}

void ArmToHack::processEnd(const string& line) {
    int jump_target = line_number + 1; 
    emitLine("@" + to_string(jump_target));
    emitLine("0;JMP");
}

void ArmToHack::evaluateOperand(const string& token) {
    if (register_map.find(token) != register_map.end()) {
        int addr = register_map[token];
        emitLine("@" + to_string(addr));
        emitLine("D=M");
        return;
    }

    if (token.size() > 1 && token[0] == '#') {
        string num_str = token.substr(1); 

        if (num_str[0] == '+') {
            num_str = num_str.substr(1);
        }

        int value = stoi(num_str);

        if (value < 0) {
            emitLine("@" + to_string(-value));  
            emitLine("D=A");
            emitLine("D=-D");
        } else {
            emitLine("@" + to_string(value));
            emitLine("D=A");
        }
        return;
    }
}

void ArmToHack::handleProgramCounter(const string& regRd) {
    if (regRd == "PC" || regRd == "R15") {
        emitLine("@R15");
        emitLine("A=M");  
        emitLine("0;JMP");
    }
}

void ArmToHack::processBranch(std::string line) {
    string instruction = takeToken(line);  
    string label = takeToken(line);
    
    if (instruction == "BL") {
        int return_address = line_number + 6;
        emitLine("@" + to_string(return_address));
        emitLine("D=A");
        emitLine("@R14"); 
        emitLine("M=D");
        
        if (label_map.find(label) != label_map.end()) {
            int target_line = label_map[label];
            emitLine("@" + to_string(target_line));
            emitLine("0;JMP");
        } else {
            int current_line = line_number; 
            emitLine("@-1");
            hack_line_to_label_map[current_line] = label;  
            emitLine("0;JMP");
        }
        return;
    }
    
    if (jump_map.find(instruction) == jump_map.end()) {
        return;  
    }
    
    string hack_jump = jump_map[instruction];
    
    if (label_map.find(label) != label_map.end()) {
        int target_line = label_map[label];
        emitLine("@" + to_string(target_line));
        if (instruction == "BAL") {
            emitLine("0;JMP");
        } else {
            emitLine("D;" + hack_jump);
        }
    } else {
        int current_line = line_number;  
        emitLine("@-1");
        hack_line_to_label_map[current_line] = label;  
        
        if (instruction == "BAL") {
            emitLine("0;JMP");
        } else {
            emitLine("D;" + hack_jump);
        }
    }
}

void ArmToHack::initializeStack() {
    const int STACK_START = 16380;   
    emitLine("@" + std::to_string(STACK_START));
    emitLine("D=A");
    emitLine("@13");  
    emitLine("M=D");
}

void ArmToHack::processStoreMultiple(std::string line) {
    takeToken(line);
    std::string rn = takeToken(line); 
    if (!rn.empty() && rn.back() == '!')
        rn.pop_back();

    if (register_map.find(rn) == register_map.end())
        return;

    int rn_addr = register_map[rn];
   
    std::vector<int> regs;
    std::string reg = takeToken(line);
    while (!reg.empty()) {
        removeChars(reg, "{}");
        if (!reg.empty() && register_map.find(reg) != register_map.end()) {
            regs.push_back(register_map[reg]);
        }
        if (reg.find('}') != std::string::npos || line.empty())
            break;
        reg = takeToken(line);
    }

    for (int r_addr : regs) {
        emitLine("@" + std::to_string(r_addr));
        emitLine("D=M");
        emitLine("@" + std::to_string(rn_addr));
        emitLine("A=M");
        emitLine("M=D");

        emitLine("@" + std::to_string(rn_addr));
        emitLine("D=M");
        emitLine("@1");
        emitLine("D=D-A");
        emitLine("@" + std::to_string(rn_addr));
        emitLine("M=D");
    }
}

void ArmToHack::processLoadMultiple(std::string line) {
    takeToken(line); 
    std::string rn = takeToken(line); 
    bool write_back = false;
    if (!rn.empty() && rn.back() == '!') {
        rn.pop_back();
        write_back = true;
    }

    if (register_map.find(rn) == register_map.end())
        return;

    int rn_addr = register_map[rn];
    
    std::vector<int> regs;
    std::string reg = takeToken(line);
    while (!reg.empty()) {
        removeChars(reg, "{}");
        if (!reg.empty() && register_map.find(reg) != register_map.end()) {
            regs.push_back(register_map[reg]);
        }
        if (reg.find('}') != std::string::npos || line.empty())
            break;
        reg = takeToken(line);
    }

    for (size_t i = 0; i < regs.size(); ++i) {
        int r_addr = regs[i];
        int offset = i + 1; 

        emitLine("@" + std::to_string(rn_addr));
        emitLine("D=M");
        emitLine("@" + std::to_string(offset));
        emitLine("D=D+A");

        emitLine("A=D");
        emitLine("D=M");
        emitLine("@" + std::to_string(r_addr));
        emitLine("M=D");
    }

    if (write_back) {
        emitLine("@" + std::to_string(rn_addr));
        emitLine("D=M");
        emitLine("@" + std::to_string(regs.size()));
        emitLine("D=D+A");
        emitLine("@" + std::to_string(rn_addr));
        emitLine("M=D");
    }
}

void ArmToHack::computeAddressAndStore(std::string base, std::string offset, int srcAddr, bool isLoad, int destAddr) {
    if (!register_map.count(base))
        return;

    int baseAddr = register_map[base];

    if (offset == "LSL") {
        offset = base;
    }

    if (offset.empty() || offset[0] == '#' || offset[0] == '+' || offset[0] == '-') {
        if (offset.empty()) offset = "#0";

        removeChars(offset, "#+ ");
        int imm = 0;
        if (!offset.empty()) {
            imm = (offset[0] == '-') ? -stoi(offset.substr(1)) : stoi(offset);
        }

        emitLine("@" + std::to_string(baseAddr));
        emitLine("D=M");

        if (imm != 0) {
            emitLine("@" + std::to_string(std::abs(imm)));
            emitLine(imm > 0 ? "D=D+A" : "D=D-A");
        }

        emitLine("@15");
        emitLine("M=D");
    }
    else if (register_map.count(offset)) {
        int idxAddr = register_map[offset];
        emitLine("@" + std::to_string(baseAddr));
        emitLine("D=M");
        emitLine("@" + std::to_string(idxAddr));
        emitLine("D=D+M");
        emitLine("@15");
        emitLine("M=D");
    } else {
        emitLine("@" + std::to_string(baseAddr));
        emitLine("D=M");
        emitLine("@15");
        emitLine("M=D");
    }

    if (isLoad) {
        emitLine("@15");
        emitLine("A=M");
        emitLine("D=M");
        if (destAddr != -1) {
            emitLine("@" + std::to_string(destAddr));
            emitLine("M=D");
        }
    } else {
        emitLine("@" + std::to_string(srcAddr));
        emitLine("D=M");
        emitLine("@15");
        emitLine("A=M");
        emitLine("M=D");
    }
}

void ArmToHack::processLoad(std::string line) {
    takeToken(line);                    
    std::string rd = takeToken(line);   

    removeChars(line, " ");
    std::string operand = line;

    if (!operand.empty() && operand[0] == '=') {
        std::string label = operand.substr(1);
        removeChars(label, " ");

        auto it = variable_map.find(label);
        if (it != variable_map.end()) {
            emitLine("@" + std::to_string(it->second));
            emitLine("D=A");

            if (register_map.count(rd)) {
                emitLine("@" + std::to_string(register_map[rd]));
                emitLine("M=D");
            }
            handleProgramCounter(rd);
        }
        return;
    }

    removeChars(operand, "[] ");
    std::string base = takeToken(operand);
    std::string offset = takeToken(operand);

    int destAddr = register_map.count(rd) ? register_map[rd] : -1;
    computeAddressAndStore(base, offset, -1, true, destAddr);

    handleProgramCounter(rd);
}

void ArmToHack::processStore(std::string line) {
    takeToken(line);
    std::string source_register = takeToken(line);
    
    if (register_map.find(source_register) == register_map.end())
        return;
    
    int source_register_addr = register_map[source_register];
    
    removeChars(line, " ");
    std::string address_expression = line;
    removeChars(address_expression, "[] ");
    
    std::string base_register_name = takeToken(address_expression);
    std::string index_expression = takeToken(address_expression);
    
    if (index_expression == "LSL") {
        index_expression = base_register_name;
    }
    
    computeAddressAndStore(base_register_name, index_expression, source_register_addr, false, -1);
}

void ArmToHack::processData(std::string line) {
    std::string var_name = takeToken(line);
    
    removeChars(var_name, " \t");
    
    takeToken(line);
    
    int start_addr = current_memory_location;
    variable_map[var_name] = start_addr;
    
    std::string value_token = takeToken(line);
    
    while (!value_token.empty()) {
        removeChars(value_token, " \t,");
        
        if (value_token.empty()) {
            value_token = takeToken(line);
            continue;
        }
        
        int value = 0;
        if (value_token[0] == '#') {
            value_token = value_token.substr(1);
        }
        bool is_negative = false;
        if (value_token[0] == '+') {
            value_token = value_token.substr(1);
        } else if (value_token[0] == '-') {
            is_negative = true;
            value_token = value_token.substr(1);
        }
        if (!value_token.empty()) {
            value = stoi(value_token);
            if (is_negative) {
                value = -value;
            }
        }
        
        if (value < 0) {
            emitLine("@" + std::to_string(-value));
            emitLine("D=A");
            emitLine("D=-D");
        } else {
            emitLine("@" + std::to_string(value));
            emitLine("D=A");
        }
        
        emitLine("@" + std::to_string(current_memory_location));
        emitLine("M=D");
        
        current_memory_location++;
        
        value_token = takeToken(line);
    }
}

void ArmToHack::processArithmeticShift(std::string line) {
    takeToken(line);

    std::string destReg = takeToken(line);
    std::string srcReg  = takeToken(line);
    std::string shift   = takeToken(line);  

    if (register_map.find(destReg) == register_map.end() ||
        register_map.find(srcReg)  == register_map.end()) {
        return;
    }

    int dest_addr = register_map[destReg];
    int src_addr  = register_map[srcReg];
    int sp_addr   = register_map["SP"];  

    emitLine("@" + std::to_string(src_addr));
    emitLine("D=M");                 
    emitLine("@" + std::to_string(sp_addr));
    emitLine("A=M");                
    emitLine("M=D");                

    emitLine("@" + std::to_string(sp_addr));
    emitLine("D=M");                 
    emitLine("@1");
    emitLine("D=D-A");              
    emitLine("A=D");
    emitLine("M=0");                
    
    int loop_start = line_number;      

    emitLine("@" + std::to_string(sp_addr));
    emitLine("A=M");                 
    emitLine("D=M");              
    emitLine("@2");
    emitLine("D=D-A");             

    int unresolved_line = line_number; 
    emitLine("@-1");
    std::string end_label = "ASR_END_" + std::to_string(unresolved_line);
    hack_line_to_label_map[unresolved_line] = end_label;
    emitLine("D;JLT");              

    emitLine("@" + std::to_string(sp_addr));
    emitLine("A=M");
    emitLine("M=D");                

    emitLine("@" + std::to_string(sp_addr));
    emitLine("D=M");                 
    emitLine("@1");
    emitLine("D=D-A");             
    emitLine("A=D");
    emitLine("M=M+1");               

    emitLine("@" + std::to_string(loop_start));
    emitLine("0;JMP");

    label_map[end_label] = line_number;

    emitLine("@" + std::to_string(sp_addr));
    emitLine("D=M");                
    emitLine("@1");
    emitLine("D=D-A");            
    emitLine("A=D");
    emitLine("D=M");               

    emitLine("@" + std::to_string(dest_addr));
    emitLine("M=D");

    handleProgramCounter(destReg);
}

void ArmToHack::secondPass(const string& in_filename, const string& out_filename) {
    ifstream input_file;
    output_stream.open(out_filename); 
    
    line_number = 0;
    
    input_file.open(in_filename);
    
    if (!input_file.is_open() || !output_stream.is_open()) {
        if (input_file.is_open()) input_file.close();
        if (output_stream.is_open()) output_stream.close();
        return;
    }
    
    int current_line = 0;
    string line;
    
    while (getline(input_file, line)) {
        if (hack_line_to_label_map.find(current_line) != hack_line_to_label_map.end()) {
            string label = hack_line_to_label_map[current_line];
            
            if (label_map.find(label) != label_map.end()) {
                int target_line = label_map[label];
                emitLine("@" + to_string(target_line));
            } else {
                emitLine("@-1");
            }
        } else {
            emitLine(line);
        }
        
        current_line++;
    }
    
    input_file.close();
    output_stream.close();  
}
