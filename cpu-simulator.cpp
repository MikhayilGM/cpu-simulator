#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>
#include <map>
#include <utility>

template <typename T>
void Debug(T smt)
{
    std::cout << smt << std::endl;
    return;
}

typedef int* CPU_REG;

struct CPUREG
{
    int R1 {0};
    int R2 {0};
    int R3 {0};
    int R4 {0};
    int R5 {0};
    int R6 {0};
    int R7 {0};
    int R8 {0};
    int R9 {0};
    int R10 {0};
} CPUR;

enum REGISTERS
{
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    R8,
    R9,
    R10
};

enum class op_code
{
    MOV,
    ADD,
    SUB,
    MUL,
    DIV,
    INC,
    DEC,
    AND,
    OR,
    CMP,
    JMP,
    JE,
    JNE,
    JA,
    JAE,
    JB,
    JBE,
};

struct instructions
{
    op_code instruction;
    CPU_REG reg1 {nullptr};
    CPU_REG reg2 {nullptr};
    int num {-1};
    bool is_num {false};
    size_t label_line {100000000};
};


std::map<std::string, op_code> StrOpcode;
std::map<std::string, int> StrReg; 
std::map<int, instructions> code;
std::map<std::string, size_t> labels;
std::pair<int, int> compare;

bool is_number(const std::string& s)
{
    for (size_t i = 0; i < s.size(); ++i)
    {
        if (!(s[i] >= '0' && s[i] <= '9'))
        {
            return false;
        }
        
    }
    return true;
}

std::vector<std::string> split(const std::string& str, const std::string& spliter)
{
    std::vector<std::string> vec;
    std::string tmp;

    for(size_t i = 0; i < str.size(); i++)
    {
        bool smt = 1;
        for (size_t j = 0; j < spliter.size(); j++)
        {
            if(spliter[j] == str[i])
            {
                if(tmp.size() != 0)
                {
                    vec.push_back(tmp);
                    tmp.clear();
                }
                smt = 0;
                break;
            }
        }

        if(smt)
        {
            tmp.push_back(str[i]);
        }
    }

    vec.push_back(tmp);
    return vec;
}

void toLowerCase(std::string& str)
{

    for (size_t i = 0; i < str.size(); i++)
    {
        if(str[i] >= 'A' && str[i] <= 'Z')
        {
            str[i] += 32; 
        } 
    }
    
}

op_code strToOpcode(const std::string& line)
{
    auto it = StrOpcode.find(line);
    return it -> second;
}

CPU_REG strToReg(const std::string& line)
{
    auto it = StrReg.find(line);
    CPU_REG tmp = reinterpret_cast<CPU_REG>(&CPUR);
    return ( tmp + (it -> second));
}

size_t strToLabel(const std::string& line)
{
    auto it = labels.find(line);
    return it -> second;
}

int strToInt(const std::string& num)
{
    int int_num = 0;
    int degree = 1;
    for (size_t i = 0; i < num.size(); ++i)
    {
        int_num += (num[num.size() - i - 1] - '0')*degree;
        degree *= 10;
    }

    return int_num;
    
}

instructions find_instruction(size_t key)
{
    auto it = code.find(key);
    return it -> second;
}

void find_labels(std::vector<std::vector<std::string>>& iline) // find labels, push them them to "labels" map and delete them from iline
{
    size_t j = 0;

    for (size_t i = 0; i < iline.size(); ++i)
    {
        if(iline[i].size() == 1)
        {
            if(iline[i][0][iline[i][0].size() - 1] == ':')
            {
                iline[i][0].erase(iline[i][0].begin() + iline[i][0].size() - 1);
                std::cout << "i=" << i << " j=" << j << std::endl; 
                labels.insert({iline[i][0], j});
                iline.erase(iline.begin() + i);
                --i;
                continue;
            }
        }

        ++j;
    }  

}

void mov(int& first, int& second)
{
    first = second;
}

void add(int& first, int& second)
{
    first += second;
}

void sub(int& first, int& second)
{
    first -= second;
}

void mul(int& first, int& second)
{
    first *= second;
}

void diiv(int& first, int& second)
{
    if(second != 0)
        first /= second;
}

void inc(int& first)
{
    ++first;
}

void dec(int& first)
{
    --first;
}

void and_func(int& first, int& second)
{
    first = first & second;
}

void or_func(int& first, int& second)
{
    first = first | second;
}

void cmp(int& first, int& second)
{
    compare.first = first;
    compare.second = second;
}

void jmp(size_t& jump_line, size_t& instruction_line)
{
    --jump_line;
    instruction_line = jump_line;
}

void je(size_t& jump_line, size_t& instruction_line)
{
    --jump_line;
    if(compare.first == compare.second)
    {
        instruction_line = jump_line;
    }
}

void jne(size_t& jump_line, size_t& instruction_line)
{
    --jump_line;
    if(compare.first != compare.second)
    {
        instruction_line = jump_line;
    }
}

void ja(size_t& jump_line, size_t& instruction_line)
{
    --jump_line;
    if(compare.first > compare.second)
    {
        instruction_line = jump_line;
    }
}

void jae(size_t& jump_line, size_t& instruction_line)
{
    --jump_line;
    if(compare.first >= compare.second)
    {
        instruction_line = jump_line;
    }
}

void jb(size_t& jump_line, size_t& instruction_line)
{
    --jump_line;
    if(compare.first < compare.second)
    {
        instruction_line = jump_line;
    }
}

void jbe(size_t& jump_line, size_t& instruction_line)
{
    --jump_line;
    if(compare.first <= compare.second)
    {
        instruction_line = jump_line;
    }
}

void translate(const std::string& filename)
{
    std::ifstream file(filename);
    std::vector<std::vector<std::string>> iline; // instruction line
    while(!file.eof())
    {

        char asd[64];
        file.getline(asd, 64, '\n');
        std::string dsa(asd);
        toLowerCase(dsa);
        iline.push_back(split(dsa, "    ,"));
    }

    find_labels(iline);
    
    for (size_t i = 0, j = 0; i < iline.size(); i++)
    {
        instructions tmp;
        tmp.instruction = strToOpcode(iline[i][0]);
        bool is_label = 0;
        switch(tmp.instruction)
        {
            case op_code::MOV:
                tmp.reg1 = strToReg(iline[i][1]);
                if(is_number(iline[i][2]))
                {
                    tmp.is_num = true;
                    tmp.num = strToInt(iline[i][2]);
                }
                else
                {
                    tmp.is_num = false;
                    tmp.reg2 = strToReg(iline[i][2]);
                }
                break;

            case op_code::ADD:
                tmp.reg1 = strToReg(iline[i][1]);

                if(is_number(iline[i][2]))
                {
                    tmp.is_num = true;
                    tmp.num = strToInt(iline[i][2]);
                }
                else
                {
                    tmp.is_num = false;
                    tmp.reg2 = strToReg(iline[i][2]);
                }
                break;

            case op_code::SUB:
                tmp.reg1 = strToReg(iline[i][1]);

                if(is_number(iline[i][2]))
                {
                    tmp.is_num = true;
                    tmp.num = strToInt(iline[i][2]);
                }
                else
                {
                    tmp.is_num = false;
                    tmp.reg2 = strToReg(iline[i][2]);
                }
                break;
                
            case op_code::MUL:
                tmp.reg1 = strToReg(iline[i][1]);

                if(is_number(iline[i][2]))
                {
                    tmp.is_num = true;
                    tmp.num = strToInt(iline[i][2]);
                }
                else
                {
                    tmp.is_num = false;
                    tmp.reg2 = strToReg(iline[i][2]);
                }
                break;

            case op_code::DIV:
                tmp.reg1 = strToReg(iline[i][1]);

                if(is_number(iline[i][2]))
                {
                    tmp.is_num = true;
                    tmp.num = strToInt(iline[i][2]);
                }
                else
                {
                    tmp.is_num = false;
                    tmp.reg2 = strToReg(iline[i][2]);
                }
                break;

            case op_code::INC:
                tmp.reg1 = strToReg(iline[i][1]);  
                break;

            case op_code::DEC:
                tmp.reg1 = strToReg(iline[i][1]);
                break;

            case op_code::AND:
                tmp.reg1 = strToReg(iline[i][1]);

                if(is_number(iline[i][2]))
                {
                    tmp.is_num = true;
                    tmp.num = strToInt(iline[i][2]);
                }
                else
                {
                    tmp.is_num = false;
                    tmp.reg2 = strToReg(iline[i][2]);
                }
                break;

            case op_code::OR:
                tmp.reg1 = strToReg(iline[i][1]);

                if(is_number(iline[i][2]))
                {
                    tmp.is_num = true;
                    tmp.num = strToInt(iline[i][2]);
                }
                else
                {
                    tmp.is_num = false;
                    tmp.reg2 = strToReg(iline[i][2]);
                }
                break;

            case op_code::CMP:
                if(is_number(iline[i][1]))
                {
                    tmp.is_num = true;
                    tmp.num = strToInt(iline[i][1]);
                }
                else
                {
                    tmp.is_num = false;
                    tmp.reg1 = strToReg(iline[i][1]);
                }

                if(is_number(iline[i][2]))
                {
                    tmp.is_num = true;
                    tmp.num = strToInt(iline[i][2]);
                }
                else
                {
                    tmp.is_num = false;
                    tmp.reg2 = strToReg(iline[i][2]);
                }
                break;

            case op_code::JMP:
                tmp.label_line = strToLabel(iline[i][1]);
                break;

            case op_code::JE:
                tmp.label_line = strToLabel(iline[i][1]);
                break;

            case op_code::JNE:
                tmp.label_line = strToLabel(iline[i][1]);
                break;

            case op_code::JA:
                tmp.label_line = strToLabel(iline[i][1]);
                break;

            case op_code::JAE:
                tmp.label_line = strToLabel(iline[i][1]);
                break;

            case op_code::JB:
                tmp.label_line = strToLabel(iline[i][1]);
                break;

            case op_code::JBE:
                tmp.label_line = strToLabel(iline[i][1]);
                break;
                
            default:
                break;
        }

        code.insert({i, tmp});
    }
}

void instructions_execute()
{
    for (size_t i = 0; i < code.size(); ++i)
    {
        instructions cur = find_instruction(i);

        switch(cur.instruction)
        {
            case op_code::MOV:
                if(cur.is_num)
                {
                    mov(*cur.reg1, cur.num);
                }
                else
                {
                    mov(*cur.reg1, *cur.reg2);
                }
                break;

            case op_code::ADD:
                if(cur.is_num)
                {
                    add(*cur.reg1, cur.num);
                }
                else
                {
                    add(*cur.reg1, *cur.reg2);
                }
                Debug(CPUR.R9);
                break;

            case op_code::SUB:
                if(cur.is_num)
                {
                    sub(*cur.reg1, cur.num);
                }
                else
                {
                    sub(*cur.reg1, *cur.reg2);
                }
                break;
                
            case op_code::MUL:
                if(cur.is_num)
                {
                    mul(*cur.reg1, cur.num);
                }
                else
                {
                    mul(*cur.reg1, *cur.reg2);
                }
                break;

            case op_code::DIV:
                if(cur.is_num)
                {
                    diiv(*cur.reg1, cur.num);
                }
                else
                {
                    diiv(*cur.reg1, *cur.reg2);
                }
                break;

            case op_code::INC:
                inc(*cur.reg1);
                break;

            case op_code::DEC:
                dec(*cur.reg1);
                break;

            case op_code::AND:
                if(cur.is_num)
                {
                    and_func(*cur.reg1, cur.num);
                }
                else
                {
                    and_func(*cur.reg1, *cur.reg2);
                }
                break;

            case op_code::OR:
                if(cur.is_num)
                {
                    or_func(*cur.reg1, cur.num);
                }
                else
                {
                    or_func(*cur.reg1, *cur.reg2);
                }
                break;

            case op_code::CMP:
                if(cur.is_num)
                {
                    cmp(*cur.reg1, cur.num);
                }
                else
                {
                    cmp(*cur.reg1, *cur.reg2);
                }
                break;

            case op_code::JMP:
                jmp(cur.label_line, i);
                break;

            case op_code::JE:
                je(cur.label_line, i);
                break;

            case op_code::JNE:
                jne(cur.label_line, i);
                break;

            case op_code::JA:
                ja(cur.label_line, i);
                break;

            case op_code::JAE:
                jae(cur.label_line, i);
                break;

            case op_code::JB:
                jb(cur.label_line , i);
                continue;
                break;

            case op_code::JBE:
                jbe(cur.label_line, i);
                break;
                
            default:
                break;
        }

    }
    
}


void execute(const std::string& file)
{

    Debug("Start of Execute");

    StrOpcode.insert({"mov", op_code::MOV});
    StrOpcode.insert({"add", op_code::ADD});
    StrOpcode.insert({"sub", op_code::SUB});
    StrOpcode.insert({"mul", op_code::MUL});
    StrOpcode.insert({"div", op_code::DIV});
    StrOpcode.insert({"inc", op_code::INC});
    StrOpcode.insert({"and", op_code::AND});
    StrOpcode.insert({"or", op_code::OR});
    StrOpcode.insert({"cmp", op_code::CMP});
    StrOpcode.insert({"jmp", op_code::JMP});
    StrOpcode.insert({"je", op_code::JE});
    StrOpcode.insert({"jne", op_code::JNE});
    StrOpcode.insert({"ja", op_code::JA});
    StrOpcode.insert({"jae", op_code::JAE});
    StrOpcode.insert({"jb", op_code::JB});
    StrOpcode.insert({"jbe", op_code::JBE});

    StrReg.insert({"r1", REGISTERS::R1});
    StrReg.insert({"r2", REGISTERS::R2});
    StrReg.insert({"r3", REGISTERS::R3});
    StrReg.insert({"r4", REGISTERS::R4});
    StrReg.insert({"r5", REGISTERS::R5});
    StrReg.insert({"r6", REGISTERS::R6});
    StrReg.insert({"r7", REGISTERS::R7});
    StrReg.insert({"r8", REGISTERS::R8});
    StrReg.insert({"r9", REGISTERS::R9});
    StrReg.insert({"r10", REGISTERS::R10});

    translate(file);

    instructions_execute();
    
}

int main(int argc, char* argv[])
{
    execute(argv[1]);
    return 0;
}