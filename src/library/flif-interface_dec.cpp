/*
 FLIF - Free Lossless Image Format
 Copyright (C) 2010-2015  Jon Sneyers & Pieter Wuille, LGPL v3+

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "flif-interface-private_dec.hpp"
#include "flif-interface_common.cpp"

FLIF_DECODER::FLIF_DECODER()
: quality(100)
, scale(1)
, callback(NULL)
{ }

int32_t FLIF_DECODER::decode_file(const char* filename) {
    internal_images.clear();
    images.clear();

    FILE *file = fopen(filename,"rb");
    if(!file)
        return 0;
    FileIO fio(file, filename);

    if(!flif_decode(fio, internal_images, quality, scale, reinterpret_cast<uint32_t (*)(int,int)>(callback), images))
        return 0;

    images.clear();
    for (Image& image : internal_images) images.emplace_back(std::move(image));

    return 1;
}

int32_t FLIF_DECODER::decode_memory(const void* buffer, size_t buffer_size_bytes) {
    internal_images.clear();
    images.clear();

    BlobReader reader(reinterpret_cast<const uint8_t*>(buffer), buffer_size_bytes);

    if(!flif_decode(reader, internal_images, quality, scale, reinterpret_cast<uint32_t (*)(int,int)>(callback), images))
        return 0;

    images.clear();
    for (Image& image : internal_images) images.emplace_back(std::move(image));
    return 1;
}

size_t FLIF_DECODER::num_images() {
    return images.size();
}

FLIF_IMAGE* FLIF_DECODER::get_image(size_t index) {
    if(index >= images.size())
        return 0;
    if(index >= requested_images.size()) requested_images.resize(images.size());
    if (!requested_images[index].get()) requested_images[index].reset( new FLIF_IMAGE());
    if (images[index].rows()) {
        requested_images[index]->image = std::move(images[index]); // moves and invalidates images[index]
    }
    return requested_images[index].get();
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

FLIF_DLLEXPORT FLIF_DECODER* FLIF_API flif_create_decoder() {
    try
    {
        std::unique_ptr<FLIF_DECODER> decoder(new FLIF_DECODER());
        return decoder.release();
    }
    catch(...) {}
    return 0;
}

FLIF_DLLEXPORT void FLIF_API flif_destroy_decoder(FLIF_DECODER* decoder) {
    // delete should never let exceptions out
    delete decoder;
}

FLIF_DLLEXPORT void FLIF_API flif_decoder_set_quality(FLIF_DECODER* decoder, int32_t quality) {
    try
    {
        decoder->quality = quality;
    }
    catch(...) {}
}

FLIF_DLLEXPORT void FLIF_API flif_decoder_set_scale(FLIF_DECODER* decoder, uint32_t scale) {
    try
    {
        decoder->scale = scale;
    }
    catch(...) {}
}

FLIF_DLLEXPORT void FLIF_API flif_decoder_set_callback(FLIF_DECODER* decoder, uint32_t (*callback)(int quality,int bytes_read)) {
    try
    {
        decoder->callback = (void*) callback;
    }
    catch(...) {}
}

/*!
* \return non-zero if the function succeeded
*/
FLIF_DLLEXPORT int32_t FLIF_API flif_decoder_decode_file(FLIF_DECODER* decoder, const char* filename) {
    try
    {
        return decoder->decode_file(filename);
    }
    catch(...) {}
    return 0;
}

/*!
* \return non-zero if the function succeeded
*/
FLIF_DLLEXPORT int32_t FLIF_API flif_decoder_decode_memory(FLIF_DECODER* decoder, const void* buffer, size_t buffer_size_bytes) {
    try
    {
        return decoder->decode_memory(buffer, buffer_size_bytes);
    }
    catch(...) {}
    return 0;
}

FLIF_DLLEXPORT size_t FLIF_API flif_decoder_num_images(FLIF_DECODER* decoder) {
    try
    {
        return decoder->num_images();
    }
    catch(...) {}
    return 0;
}

FLIF_DLLEXPORT FLIF_IMAGE* FLIF_API flif_decoder_get_image(FLIF_DECODER* decoder, size_t index) {
    try
    {
        return decoder->get_image(index);
    }
    catch(...) {}
    return 0;
}


} // extern "C"
