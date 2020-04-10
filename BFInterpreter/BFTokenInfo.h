//
// Created by Stijn on 09/04/2020.
//

#ifndef BRAINFCKINTERPRETER_BFTOKENINFO_H
#define BRAINFCKINTERPRETER_BFTOKENINFO_H

#include <string>
#include <map>
#include <utility>

#include "../toml/toml.hpp"
#include "../BFInstructionType.h"

class BFTokenInfo
{
public:
    explicit BFTokenInfo(toml::value data)
    {
        _data = std::move(data);
        const auto& tokenDefs = toml::find(_data, "BFTokenDefinitions");
        
        DataPointerIncrementToken = toml::find<std::string>(tokenDefs, std::string("DataPointerIncrement"));
        DataPointerDecrementToken = toml::find<std::string>(tokenDefs, std::string("DataPointerDecrement"));

        IncrementCellValueToken = toml::find<std::string>(tokenDefs, std::string("IncrementCellValue"));
        DecrementCellValueToken = toml::find<std::string>(tokenDefs, std::string("DecrementCellValue"));

        WriteCharacterToken = toml::find<std::string>(tokenDefs, std::string("WriteCharacter"));
        ReadCharacterToken = toml::find<std::string>(tokenDefs, std::string("ReadCharacter"));

        LoopBeginToken = toml::find<std::string>(tokenDefs, std::string("LoopBegin"));
        LoopEndToken = toml::find<std::string>(tokenDefs, std::string("LoopEnd"));

        try
        {
            Delimiter = toml::find<std::string>(tokenDefs, "Delimiter");
        }
        catch (...)
        {
            Delimiter = "";
        }
        
        InitTokens();
    }
    
    explicit BFTokenInfo(std::string &path) : BFTokenInfo(toml::parse(path))
    {}
    
    std::string DataPointerIncrementToken;
    std::string DataPointerDecrementToken;
    
    std::string IncrementCellValueToken;
    std::string DecrementCellValueToken;
    
    std::string WriteCharacterToken;
    std::string ReadCharacterToken;
    
    std::string LoopBeginToken;
    std::string LoopEndToken;
    
    std::string Delimiter;
    
    std::map<std::string, BFInstructionType> Tokens;
    
    static BFTokenInfo Default()
    {
        BFTokenInfo result = BFTokenInfo();
        result.DataPointerIncrementToken = std::string(1, dPtrIncr);
        result.DataPointerDecrementToken = std::string(1, dPtrDecr);

        result.IncrementCellValueToken = std::string(1, IncrPtrVal);
        result.DecrementCellValueToken = std::string(1, DecrPtrVal);

        result.WriteCharacterToken = std::string(1, cWritePtrVal);
        result.ReadCharacterToken = std::string(1, cReadPtrVal);

        result.LoopBeginToken = std::string(1, LoopBegin);
        result.LoopEndToken = std::string(1, LoopEnd);
        
        result.Delimiter = "";
        
        result.InitTokens();
        return result;
    }
private:
    BFTokenInfo() = default;
    
    void InitTokens()
    {
        Tokens = {
                std::pair(DataPointerIncrementToken, dPtrIncr),
                std::pair(DataPointerDecrementToken, dPtrDecr),

                std::pair(IncrementCellValueToken, IncrPtrVal),
                std::pair(DecrementCellValueToken, DecrPtrVal),

                std::pair(WriteCharacterToken, cWritePtrVal),
                std::pair(ReadCharacterToken, cReadPtrVal),

                std::pair(LoopBeginToken, LoopBegin),
                std::pair(LoopEndToken, LoopEnd)
        };
    }
    
    toml::value _data;
};

#endif //BRAINFCKINTERPRETER_BFTOKENINFO_H
