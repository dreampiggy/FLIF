/*
 FLIF encoder - Free Lossless Image Format
 Copyright (C) 2010-2015  Jon Sneyers & Pieter Wuille, GPL v3+

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "flif-interface-private_enc.hpp"
#include "flif-interface_common.cpp"

FLIF_ENCODER::FLIF_ENCODER()
: interlaced(1)
, learn_repeats(3)
, acb(1)
, frame_delay(100)
, palette_size(512)
, lookback(1)
, divisor(CONTEXT_TREE_COUNT_DIV)
, min_size(CONTEXT_TREE_MIN_SUBTREE_SIZE)
, split_threshold(CONTEXT_TREE_SPLIT_THRESHOLD)
, alpha_zero_special(1)
{
}

void FLIF_ENCODER::add_image(FLIF_IMAGE* image) {
    images.push_back(image);
    if (!alpha_zero_special) image->image.alpha_zero_special = false;
}

/*!
* \return non-zero if the function succeeded
*/
int32_t FLIF_ENCODER::encode_file(const char* filename) {
    FILE *file = fopen(filename,"wb");
    if(!file)
        return 0;
    FileIO fio(file, filename);

    // TODO: need to change flif_encode() so these expensive copies can be avoided
    Images copies;
    for(size_t i = 0; i < images.size(); ++i)
        copies.push_back(images[i]->image.clone());

    std::vector<std::string> transDesc = {"PLC","YIQ","BND","PLA","PLT","ACB","DUP","FRS","FRA"};

    if(!flif_encode(fio, copies, transDesc,
        interlaced != 0 ? flifEncoding::interlaced : flifEncoding::nonInterlaced,
        learn_repeats, acb, palette_size, lookback,
        divisor, min_size, split_threshold))
        return 0;

    return 1;
}

/*!
* \return non-zero if the function succeeded
*/
int32_t FLIF_ENCODER::encode_memory(void** buffer, size_t* buffer_size_bytes) {
    BlobIO io;

    // TODO: need to change flif_encode() so these expensive copies can be avoided
    Images copies;
    for(size_t i = 0; i < images.size(); ++i)
        copies.push_back(images[i]->image.clone());

    std::vector<std::string> transDesc = {"YIQ","BND","PLA","PLT","ACB","DUP","FRS","FRA"};

    if(!flif_encode(io, copies, transDesc,
        interlaced != 0 ? flifEncoding::interlaced : flifEncoding::nonInterlaced,
        learn_repeats, acb, palette_size, lookback,
        divisor, min_size, split_threshold))
        return 0;

    *buffer = io.release(buffer_size_bytes);
    return 1;
}

//=============================================================================

/*!
Notes about the C interface:

Only use types known to C.
Use types that are unambiguous across all compilers, like uint32_t.
Each function must have it's call convention set.
Exceptions must be caught no matter what.

*/

//=============================================================================


extern "C" {

FLIF_DLLEXPORT FLIF_ENCODER* FLIF_API flif_create_encoder() {
    try
    {
        std::unique_ptr<FLIF_ENCODER> encoder(new FLIF_ENCODER());
        return encoder.release();
    }
    catch(...) {}
    return 0;
}

FLIF_DLLEXPORT void FLIF_API flif_destroy_encoder(FLIF_ENCODER* encoder) {
    // delete should never let exceptions out
    delete encoder;
}

FLIF_DLLEXPORT void FLIF_API flif_encoder_set_interlaced(FLIF_ENCODER* encoder, uint32_t interlaced) {
    try
    {
        encoder->interlaced = interlaced;
    }
    catch(...) {}
}

FLIF_DLLEXPORT void FLIF_API flif_encoder_set_learn_repeat(FLIF_ENCODER* encoder, uint32_t learn_repeats) {
    try
    {
        encoder->learn_repeats = learn_repeats;
    }
    catch(...) {}
}

FLIF_DLLEXPORT void FLIF_API flif_encoder_set_auto_color_buckets(FLIF_ENCODER* encoder, uint32_t acb) {
    try
    {
        encoder->acb = acb;
    }
    catch(...) {}
}

FLIF_DLLEXPORT void FLIF_API flif_encoder_set_palette_size(FLIF_ENCODER* encoder, int32_t palette_size) {
    try
    {
        encoder->palette_size = palette_size;
    }
    catch(...) {}
}

FLIF_DLLEXPORT void FLIF_API flif_encoder_set_lookback(FLIF_ENCODER* encoder, int32_t lookback) {
    try { encoder->lookback = lookback; }
    catch(...) { }
}
FLIF_DLLEXPORT void FLIF_API flif_encoder_set_divisor(FLIF_ENCODER* encoder, int32_t divisor) {
    try { encoder->divisor = divisor; }
    catch(...) { }
}
FLIF_DLLEXPORT void FLIF_API flif_encoder_set_min_size(FLIF_ENCODER* encoder, int32_t min_size) {
    try { encoder->min_size = min_size; }
    catch(...) { }
}
FLIF_DLLEXPORT void FLIF_API flif_encoder_set_split_threshold(FLIF_ENCODER* encoder, int32_t split_threshold) {
    try { encoder->split_threshold = split_threshold; }
    catch(...) { }
}
FLIF_DLLEXPORT void FLIF_API flif_encoder_set_alpha_zero_lossless(FLIF_ENCODER* encoder) {
    try { encoder->alpha_zero_special = 0; }
    catch(...) { }
}

FLIF_DLLEXPORT void FLIF_API flif_encoder_add_image(FLIF_ENCODER* encoder, FLIF_IMAGE* image) {
    try
    {
        encoder->add_image(image);
    }
    catch(...) {}
}

/*!
* \return non-zero if the function succeeded
*/
FLIF_DLLEXPORT int32_t FLIF_API flif_encoder_encode_file(FLIF_ENCODER* encoder, const char* filename) {
    try
    {
        return encoder->encode_file(filename);
    }
    catch(...) {}
    return 0;
}

/*!
* \return non-zero if the function succeeded
*/
FLIF_DLLEXPORT int32_t FLIF_API flif_encoder_encode_memory(FLIF_ENCODER* encoder, void** buffer, size_t* buffer_size_bytes) {
    try
    {
        return encoder->encode_memory(buffer, buffer_size_bytes);
    }
    catch(...) {}
    return 0;
}


} // extern "C"
