#include <string>
#include <map>
#include "token_io.h"
#include "ArmToHack.h"

using namespace std;

int main() {
    ArmToHack translator;
    
    translator.convertFile("test/program1.arm", "test/program1.asm");
    translator.convertFile("test/program2.arm", "test/program2.asm");
    translator.convertFile("test/program3.arm", "test/program3.asm");
    translator.convertFile("test/program4.arm", "test/program4.asm");
    translator.convertFile("test/program5.arm", "test/program5.asm");
    translator.convertFile("test/program6.arm", "test/program6.asm");
    translator.convertFile("test/program7.arm", "test/program7.asm");
    translator.convertFile("test/program8.arm", "test/program8.asm");
    translator.convertFile("test/program9.arm", "test/program9.asm");
    translator.convertFile("test/program10.arm", "test/program10.asm");
    translator.convertFile("test/program11.arm", "test/program11.asm");
    translator.convertFile("test/program12.arm", "test/program12.asm");
    translator.convertFile("test/program13.arm", "test/program13.asm");
    translator.convertFile("test/program14.arm", "test/program14.asm");
    translator.convertFile("test/program15.arm", "test/program15.asm");
    translator.convertFile("test/program16.arm", "test/program16.asm");
    translator.convertFile("test/program17.arm", "test/program17.asm");
    translator.convertFile("test/program18.arm", "test/program18.asm");
    translator.convertFile("test/program19.arm", "test/program19.asm");
    translator.convertFile("test/program20.arm", "test/program20.asm");
    translator.convertFile("test/program21.arm", "test/program21.asm");
    translator.convertFile("test/program22.arm", "test/program22.asm");
    translator.convertFile("test/program23.arm", "test/program23.asm");
    translator.convertFile("test/programASR1.arm", "test/programASR1.asm");
    translator.convertFile("test/programASR2.arm", "test/programASR2.asm");
    translator.convertFile("test/programASR3.arm", "test/programASR3.asm");
    
    return 0;
}

