#pragma once

#include "GE_pch.h"
#include "miniz.h"

#include "Runtime/core/Hash.h"

#include "Runtime/core/Log/LogSystem.h"

namespace GE
{
    namespace Packing
    {
        inline void CompressData(uchar* content, uint64 size, uchar** compressed, uint64& cmp_size)
        {
            uLong expected_size = compressBound(size);
            *compressed         = new uchar[expected_size];
            GE_CORE_CHECK(compress(*compressed, &expected_size, (const uchar*)content, size) == MZ_OK,
                          "[CompressData]: Compression failed");
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
            GE_CORE_CHECK(uncompress(*decompressed, &true_size, (const uchar*)content, size) == MZ_OK,
                          "[DecompressData]: Decompression failed");
            GE_CORE_CHECK(true_size == decmp_size, "[DecompressData]: size mismatch");
        }
    } // namespace Packing
} // namespace GE