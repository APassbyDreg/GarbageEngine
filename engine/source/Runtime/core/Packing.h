#pragma once

#include "GE_pch.h"
#include "miniz.h"

#include "Runtime/core/Hash.h"

#include "Runtime/function/Log/LogSystem.h"

namespace GE
{
    namespace Packing
    {

#define MZ_CHECK(call) \
    do \
    { \
        int status = call; \
        if (status != MZ_OK) \
        { \
            GE_CORE_ERROR("[Packing] Error calling {}: {}", #call, zError(status)); \
        } \
    } while (0)

        inline void CompressData(uchar* content, uint64 size, uchar** compressed, uint64& cmp_size)
        {
            uLong expected_size = compressBound(size);
            *compressed         = new uchar[expected_size];
            MZ_CHECK(compress(*compressed, &expected_size, (const uchar*)content, size));
            cmp_size = expected_size;

            if (cmp_size > size)
            {
                // use uncompressed format
                delete[] * compressed;
                *compressed = new uchar[size];
                memcpy(*compressed, content, size);
                cmp_size = size;
            }
        }

        inline void DecompressData(uchar* content, uint64 size, uchar** decompressed, uint64& decmp_size)
        {
            uLong true_size = decmp_size;
            MZ_CHECK(uncompress(*decompressed, &true_size, (const uchar*)content, size));
            GE_CORE_ASSERT(true_size == decmp_size, "[DecompressData]: size mismatch");
        }
    } // namespace Packing
} // namespace GE