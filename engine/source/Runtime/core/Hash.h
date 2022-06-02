#pragma once

#include "GE_pch.h"

#include "openssl/sha.h"

namespace GE
{
    inline std::string sha256(const char* content, size_t size)
    {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX    sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, content, size);
        SHA256_Final(hash, &sha256);
        int  i = 0;
        char outputBuffer[65];
        for (i = 0; i < SHA256_DIGEST_LENGTH; i++)
        {
            sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
        }
        outputBuffer[64] = 0;
        return std::string(outputBuffer);
    }

    inline std::string sha256(std::string content)
    {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX    sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, content.c_str(), content.size());
        SHA256_Final(hash, &sha256);
        int  i = 0;
        char outputBuffer[65];
        for (i = 0; i < SHA256_DIGEST_LENGTH; i++)
        {
            sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
        }
        outputBuffer[64] = 0;
        return std::string(outputBuffer);
    }
} // namespace GE