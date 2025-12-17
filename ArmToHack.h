#ifndef ARMTOHACK_H_
#define ARMTOHACK_H_

#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include "token_io.h"

class ArmToHack {
private:
    std::ifstream input_stream;
    std::ofstream output_stream;
    int line_number;
    std::map<std::string, int> register_map;
    std::map<std::string, std::string> jump_map; 
    std::map<std::string, int> label_map;  
    std::map<int, std::string> hack_line_to_label_map;
    std::map<std::string, int> variable_map; 
    int current_memory_location;  
    void evaluateOperand(const std::string& token);
    void handleProgramCounter(const std::string& regRd);
    void processBranch(std::string line);
    void secondPass(const std::string& in_filename, const std::string& out_filename);
    void processArithmeticOp(std::string line, int opType);
    void computeAddressAndStore(std::string base, std::string offset, int srcAddr, bool isLoad, int destAddr);

public:
    ArmToHack();
    void clearState();
    void emitLine(const std::string& line);
    void convertFile(const std::string& in_filename, const std::string& out_filename);
    void firstPass(const std::string& in_filename, const std::string& out_filename);
    void processInstruction(const std::string& line);
    void processMove(std::string line);
    void processAdd(std::string line);
    void processSubtract(std::string line);
    void processReverseSubtract(std::string line);
    void processCompare(std::string line);
    void processEnd(const std::string& line);
    void processStoreMultiple(std::string line);
    void processLoadMultiple(std::string line);
    void processLoad(std::string line);
    void processStore(std::string line);
    void processData(std::string line);
    void processArithmeticShift(std::string line);
    void initializeStack();
};

#endif
