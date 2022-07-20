/* Copyright 2016 Pete Warden. All Rights Reserved.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 ==============================================================================*/

#include "picoproto.hpp"

namespace picoproto
{

    namespace
    {

        // To keep the dependencies down, here's a local copy of the widespread bit_cast
        // operator. This is necessary because in practice weird things can happen if
        // you just try to use reinterpret_cast.
        template<class Dest, class Source>
        inline Dest bit_cast(const Source &source)
        {
            static_assert(sizeof(Dest) == sizeof(Source), "Sizes do not match");
            Dest dest;
            memcpy(&dest, &source, sizeof(dest));
            return dest;
        }

        // These are defined in:
        // https://developers.google.com/protocol-buffers/docs/encoding
        enum WireType
        {
            WIRETYPE_VARINT = 0,
            WIRETYPE_64BIT = 1,
            WIRETYPE_LENGTH_DELIMITED = 2,
            WIRETYPE_GROUP_START = 3,
            WIRETYPE_GROUP_END = 4,
            WIRETYPE_32BIT = 5,
        };

        // Pull bytes from the stream, updating the state.
        bool ConsumeBytes(uint8_t **current, size_t how_many, size_t *remaining)
        {
            if (how_many > *remaining)
            {
                PP_LOG(ERROR) << "ReadBytes overrun!";
                return false;
            }
            *current += how_many;
            *remaining -= how_many;
            return true;
        }

        // Grabs a particular type from the byte stream.
        template<class T>
        T ReadFromBytes(uint8_t **current, size_t *remaining)
        {
            PP_CHECK(ConsumeBytes(current, sizeof(T), remaining));
            const T result = *(bit_cast<T *>(*current - sizeof(T)));
            return result;
        }

        uint64_t ReadVarInt(uint8_t **current, size_t *remaining)
        {
            uint64_t result = 0;
            bool keep_going;
            int shift = 0;
            do
            {
                const uint8_t next_number = ReadFromBytes<uint8_t>(current, remaining);
                keep_going = (next_number >= 128);
                result += (uint64_t)(next_number & 0x7f) << shift;
                shift += 7;
            } while (keep_going);
            return result;
        }

        void ReadWireTypeAndFieldNumber(uint8_t **current, size_t *remaining, uint8_t *wire_type, uint32_t *field_number)
        {
            uint64_t wire_type_and_field_number = ReadVarInt(current, remaining);
            *wire_type = wire_type_and_field_number & 0x07;
            *field_number = wire_type_and_field_number >> 3;
        }

    } // namespace

    std::string FieldTypeDebugString(enum FieldType type)
    {
        switch (type)
        {
            case FIELD_UNSET: return "UNSET"; break;
            case FIELD_UINT32: return "UINT32"; break;
            case FIELD_UINT64: return "UINT64"; break;
            case FIELD_BYTES: return "BYTES"; break;
            default: return "Unknown field type"; break;
        }
        return "Should never get here";
    }

    Field::Field(FieldType type, bool owns_data) : type(type), owns_data(owns_data)
    {
        cached_messages = nullptr;
        switch (type)
        {
            case FIELD_UINT32:
            {
                value.v_uint32 = new std::vector<uint32_t>();
            }
            break;
            case FIELD_UINT64:
            {
                value.v_uint64 = new std::vector<uint64_t>();
            }
            break;
            case FIELD_BYTES:
            {
                value.v_bytes = new std::vector<std::pair<uint8_t *, size_t>>();
                cached_messages = new std::vector<Message *>();
            }
            break;
            default:
            {
                PP_LOG(ERROR) << "Bad field type when constructing field: " << type;
            }
            break;
        }
    }

    Field::Field(const Field &other) : type(other.type), owns_data(other.owns_data)
    {
        switch (type)
        {
            case FIELD_UINT32:
            {
                value.v_uint32 = new std::vector<uint32_t>(*other.value.v_uint32);
            }
            break;
            case FIELD_UINT64:
            {
                value.v_uint64 = new std::vector<uint64_t>(*other.value.v_uint64);
            }
            break;
            case FIELD_BYTES:
            {
                if (owns_data)
                {
                    value.v_bytes = new std::vector<std::pair<uint8_t *, size_t>>();
                    for (std::pair<uint8_t *, size_t> data_info : *other.value.v_bytes)
                    {
                        uint8_t *new_data = new uint8_t[data_info.second];
                        std::copy_n(data_info.first, data_info.second, new_data);
                        value.v_bytes->push_back({ new_data, data_info.second });
                    }
                }
                else
                {
                    value.v_bytes = new std::vector<std::pair<uint8_t *, size_t>>(*other.value.v_bytes);
                }
                cached_messages = new std::vector<Message *>();
                cached_messages->reserve(other.cached_messages->size());
                for (Message *other_cached_message : *other.cached_messages)
                {
                    Message *cached_message;
                    if (other_cached_message)
                    {
                        cached_message = new Message(*other_cached_message);
                    }
                    else
                    {
                        cached_message = nullptr;
                    }
                    cached_messages->push_back(cached_message);
                }
            }
            break;
            default:
            {
                PP_LOG(ERROR) << "Bad field type when constructing field: " << type;
            }
            break;
        }
    }

    Field::~Field()
    {
        switch (type)
        {
            case FIELD_UINT32: delete value.v_uint32; break;
            case FIELD_UINT64: delete value.v_uint64; break;
            case FIELD_BYTES:
            {
                if (owns_data)
                    for (std::pair<uint8_t *, size_t> data_info : *value.v_bytes)
                        delete[] data_info.first;
                delete value.v_bytes;

                for (Message *cached_message : *cached_messages)
                    if (cached_message)
                        delete cached_message;
                delete cached_messages;
                break;
            }
            default: PP_LOG(ERROR) << "Bad field type when destroying field: " << type; break;
        }
    }

    Message::Message() : Message(true){};

    Message::Message(bool copy_arrays) : copy_arrays(copy_arrays){};

    Message::Message(const Message &other) : field_map(other.field_map), fields(other.fields), copy_arrays(other.copy_arrays){};

    Message::~Message(){};

    bool Message::ParseFromBytes(uint8_t *bytes, size_t bytes_size)
    {
        uint8_t *current = bytes;
        size_t remaining = bytes_size;
        while (remaining > 0)
        {
            uint8_t wire_type;
            uint32_t field_number;
            ReadWireTypeAndFieldNumber(&current, &remaining, &wire_type, &field_number);
            switch (wire_type)
            {
                case WIRETYPE_VARINT:
                {
                    Field *field = AddField(field_number, FIELD_UINT64);
                    const uint64_t varint = ReadVarInt(&current, &remaining);
                    field->value.v_uint64->push_back(varint);
                    break;
                }
                case WIRETYPE_64BIT:
                {
                    Field *field = AddField(field_number, FIELD_UINT64);
                    const uint64_t value = ReadFromBytes<uint64_t>(&current, &remaining);
                    field->value.v_uint64->push_back(value);
                    break;
                }
                case WIRETYPE_LENGTH_DELIMITED:
                {
                    Field *field = AddField(field_number, FIELD_BYTES);
                    const uint64_t size = ReadVarInt(&current, &remaining);
                    uint8_t *data;
                    if (copy_arrays)
                    {
                        data = new uint8_t[size];
                        std::copy_n(current, size, data);
                        field->owns_data = true;
                    }
                    else
                    {
                        data = current;
                        field->owns_data = false;
                    }
                    field->value.v_bytes->push_back({ data, size });
                    field->cached_messages->push_back(nullptr);
                    current += size;
                    remaining -= size;
                    break;
                }
                case WIRETYPE_GROUP_START:
                {
                    PP_LOG(INFO) << field_number << ": GROUPSTART" << std::endl;
                    PP_LOG(ERROR) << "Unhandled wire type encountered";
                    break;
                }
                case WIRETYPE_GROUP_END:
                {
                    PP_LOG(INFO) << field_number << ": GROUPEND" << std::endl;
                    PP_LOG(ERROR) << "Unhandled wire type encountered";
                    break;
                }
                case WIRETYPE_32BIT:
                {
                    Field *field = AddField(field_number, FIELD_UINT32);
                    const uint32_t value = ReadFromBytes<uint32_t>(&current, &remaining);
                    field->value.v_uint32->push_back(value);
                    break;
                }
                default:
                {
                    PP_LOG(ERROR) << "Unknown wire type encountered: " << static_cast<int>(wire_type) << " at offset" << (bytes_size - remaining);
                    return false;
                    break;
                }
            }
        }
        return true;
    }

    Field *Message::AddField(int32_t number, enum FieldType type)
    {
        Field *field = GetField(number);
        if (!field)
        {
            fields.push_back(Field(type, copy_arrays));
            field = &fields.back();
            field_map.insert({ number, fields.size() - 1 });
        }
        return field;
    }

    Field *Message::GetField(int32_t number)
    {
        if (field_map.count(number) == 0)
            return nullptr;
        return &(fields[field_map[number]]);
    }

    Field *Message::GetFieldAndCheckType(int32_t number, enum FieldType type)
    {
        Field *field = GetField(number);
        PP_CHECK(field) << "No field for " << number;
        PP_CHECK(field->type == type) << "For field " << number << " wanted type " << FieldTypeDebugString(type) << " but found " << FieldTypeDebugString(field->type);
        return field;
    }

    int32_t Message::GetInt32(int32_t number)
    {
        Field *field = GetFieldAndCheckType(number, FIELD_UINT32);
        uint32_t first_value = (*(field->value.v_uint32))[0];
        int32_t zig_zag_decoded = static_cast<int32_t>((first_value >> 1) ^ (-(first_value & 1)));
        return zig_zag_decoded;
    }

    int64_t Message::GetInt64(int32_t number)
    {
        Field *field = GetFieldAndCheckType(number, FIELD_UINT64);
        uint64_t first_value = (*(field->value.v_uint64))[0];
        int64_t zig_zag_decoded = static_cast<int64_t>((first_value >> 1) ^ (-(first_value & 1)));
        return zig_zag_decoded;
    }

    uint32_t Message::GetUInt32(int32_t number)
    {
        Field *field = GetFieldAndCheckType(number, FIELD_UINT32);
        uint32_t first_value = (*(field->value.v_uint32))[0];
        return first_value;
    }

    uint64_t Message::GetUInt64(int32_t number)
    {
        Field *field = GetFieldAndCheckType(number, FIELD_UINT64);
        uint64_t first_value = (*(field->value.v_uint64))[0];
        return first_value;
    }

    int64_t Message::GetInt(int32_t number)
    {
        Field *field = GetField(number);
        PP_CHECK(field) << "No field for " << number;
        PP_CHECK((field->type == FIELD_UINT32) || (field->type == FIELD_UINT64))
            << "For field " << number << " wanted integer type but found " << FieldTypeDebugString(field->type);
        switch (field->type)
        {
            case FIELD_UINT32: return GetInt32(number); break;
            case FIELD_UINT64: return GetInt64(number); break;
            default:
            {
                // Should never get here.
            }
            break;
        }
        // Should never get here.
        return 0;
    }

    bool Message::GetBool(int32_t number)
    {
        return (GetInt(number) != 0);
    }

    float Message::GetFloat(int32_t number)
    {
        uint32_t int_value = GetUInt32(number);
        float float_value = *(bit_cast<float *>(&int_value));
        return float_value;
    }

    double Message::GetDouble(int32_t number)
    {
        uint64_t int_value = GetUInt64(number);
        return *(bit_cast<double *>(&int_value));
    }

    std::pair<uint8_t *, size_t> Message::GetBytes(int32_t number)
    {
        Field *field = GetFieldAndCheckType(number, FIELD_BYTES);
        std::pair<uint8_t *, size_t> first_value = (*(field->value.v_bytes))[0];
        return first_value;
    }

    std::string Message::GetString(int32_t number)
    {
        Field *field = GetFieldAndCheckType(number, FIELD_BYTES);
        std::pair<uint8_t *, size_t> first_value = (*(field->value.v_bytes))[0];
        std::string result(first_value.first, first_value.first + first_value.second);
        return result;
    }

    Message *Message::GetMessage(int32_t number)
    {
        Field *field = GetFieldAndCheckType(number, FIELD_BYTES);
        Message *cached_message = field->cached_messages->at(0);
        if (!cached_message)
        {
            std::pair<uint8_t *, size_t> first_value = (*(field->value.v_bytes))[0];
            cached_message = new Message(copy_arrays);
            cached_message->ParseFromBytes(first_value.first, first_value.second);
            field->cached_messages->at(0) = cached_message;
        }
        return cached_message;
    }

    std::vector<int32_t> Message::GetInt32Array(int32_t number)
    {
        std::vector<uint64_t> raw_array = GetUInt64Array(number);
        std::vector<int32_t> result;
        result.reserve(raw_array.size());
        for (uint64_t raw_value : raw_array)
        {
            int32_t zig_zag_decoded = static_cast<int32_t>((raw_value >> 1) ^ (-(raw_value & 1)));
            result.push_back(zig_zag_decoded);
        }
        return result;
    }

    std::vector<int64_t> Message::GetInt64Array(int32_t number)
    {
        std::vector<uint64_t> raw_array = GetUInt64Array(number);
        std::vector<int64_t> result;
        result.reserve(raw_array.size());
        for (uint64_t raw_value : raw_array)
        {
            int64_t zig_zag_decoded = static_cast<int64_t>((raw_value >> 1) ^ (-(raw_value & 1)));
            result.push_back(zig_zag_decoded);
        }
        return result;
    }

    std::vector<uint32_t> Message::GetUInt32Array(int32_t number)
    {
        std::vector<uint64_t> raw_array = GetUInt64Array(number);
        std::vector<uint32_t> result;
        result.reserve(raw_array.size());
        for (uint64_t raw_value : raw_array)
        {
            result.push_back(static_cast<uint32_t>(raw_value));
        }
        return result;
    }

    std::vector<uint64_t> Message::GetUInt64Array(int32_t number)
    {
        std::vector<uint64_t> result;
        Field *field = GetField(number);
        if (!field)
        {
            return result;
        }
        if (field->type == FIELD_UINT64)
        {
            result.reserve(field->value.v_uint64->size());
            for (uint64_t value : *field->value.v_uint64)
            {
                result.push_back(static_cast<uint64_t>(value));
            }
        }
        else if (field->type == FIELD_UINT32)
        {
            result.reserve(field->value.v_uint32->size());
            for (uint32_t value : *field->value.v_uint32)
            {
                result.push_back(static_cast<uint64_t>(value));
            }
        }
        else if (field->type == FIELD_BYTES)
        {
            for (std::pair<uint8_t *, size_t> data_info : *field->value.v_bytes)
            {
                uint8_t *current = data_info.first;
                size_t remaining = data_info.second;
                while (remaining > 0)
                {
                    const uint64_t varint = ReadVarInt(&current, &remaining);
                    result.push_back(static_cast<int64_t>(varint));
                }
            }
        }
        else
        {
            PP_LOG(ERROR) << "Expected field type UINT32, UINT64, or BYTES but got " << FieldTypeDebugString(field->type);
        }
        return result;
    }

    std::vector<bool> Message::GetBoolArray(int32_t number)
    {
        std::vector<uint64_t> raw_array = GetUInt64Array(number);
        std::vector<bool> result;
        result.reserve(raw_array.size());
        for (uint64_t raw_value : raw_array)
        {
            result.push_back(raw_value != 0);
        }
        return result;
    }

    std::vector<float> Message::GetFloatArray(int32_t number)
    {
        std::vector<float> result;
        Field *field = GetField(number);
        if (!field)
        {
            return result;
        }
        if (field->type == FIELD_UINT32)
        {
            result.reserve(field->value.v_uint32->size());
            for (uint32_t value : *field->value.v_uint32)
            {
                result.push_back(bit_cast<float>(value));
            }
        }
        else if (field->type == FIELD_BYTES)
        {
            for (std::pair<uint8_t *, size_t> data_info : *field->value.v_bytes)
            {
                uint8_t *current = data_info.first;
                size_t remaining = data_info.second;
                while (remaining > 0)
                {
                    const uint64_t varint = ReadVarInt(&current, &remaining);
                    const uint32_t varint32 = static_cast<uint32_t>(varint & 0xffffffff);
                    result.push_back(bit_cast<float>(varint32));
                }
            }
        }
        else
        {
            PP_LOG(ERROR) << "Expected field type UINT32 or BYTES but got " << FieldTypeDebugString(field->type);
        }
        return result;
    }

    std::vector<double> Message::GetDoubleArray(int32_t number)
    {
        std::vector<double> result;
        Field *field = GetField(number);
        if (!field)
        {
            return result;
        }
        if (field->type == FIELD_UINT64)
        {
            result.reserve(field->value.v_uint64->size());
            for (uint64_t value : *field->value.v_uint64)
            {
                result.push_back(bit_cast<double>(value));
            }
        }
        else if (field->type == FIELD_BYTES)
        {
            for (std::pair<uint8_t *, size_t> data_info : *field->value.v_bytes)
            {
                uint8_t *current = data_info.first;
                size_t remaining = data_info.second;
                while (remaining > 0)
                {
                    const uint64_t varint = ReadVarInt(&current, &remaining);
                    result.push_back(bit_cast<double>(varint));
                }
            }
        }
        else
        {
            PP_LOG(ERROR) << "Expected field type UINT64 or BYTES but got " << FieldTypeDebugString(field->type);
        }
        return result;
    }

    std::vector<std::pair<uint8_t *, size_t>> Message::GetByteArray(int32_t number)
    {
        std::vector<std::pair<uint8_t *, size_t>> result;
        Field *field = GetField(number);
        if (!field)
        {
            return result;
        }
        if (field->type == FIELD_BYTES)
        {
            result.reserve(field->value.v_bytes->size());
            for (std::pair<uint8_t *, size_t> data_info : *field->value.v_bytes)
            {
                result.push_back(data_info);
            }
        }
        else
        {
            PP_LOG(ERROR) << "Expected field type BYTES but got " << FieldTypeDebugString(field->type);
        }
        return result;
    }

    std::vector<std::string> Message::GetStringArray(int32_t number)
    {
        std::vector<std::string> result;
        Field *field = GetField(number);
        if (!field)
            return result;
        if (field->type == FIELD_BYTES)
        {
            result.reserve(field->value.v_bytes->size());
            for (std::pair<uint8_t *, size_t> data_info : *field->value.v_bytes)
            {
                result.push_back(std::string(data_info.first, data_info.first + data_info.second));
            }
        }
        else
        {
            PP_LOG(ERROR) << "Expected field type BYTES but got " << FieldTypeDebugString(field->type);
        }
        return result;
    }

    std::vector<Message *> Message::GetMessageArray(int32_t number)
    {
        std::vector<Message *> result;
        Field *field = GetField(number);
        if (!field)
            return result;

        if (field->type == FIELD_BYTES)
        {
            result.reserve(field->value.v_bytes->size());
            for (size_t i = 0; i < field->value.v_bytes->size(); ++i)
            {
                Message *cached_message = field->cached_messages->at(i);
                if (!cached_message)
                {
                    std::pair<uint8_t *, size_t> value = field->value.v_bytes->at(i);
                    cached_message = new Message(copy_arrays);
                    cached_message->ParseFromBytes(value.first, value.second);
                    field->cached_messages->at(i) = cached_message;
                }
                result.push_back(cached_message);
            }
        }
        else
        {
            PP_LOG(ERROR) << "Expected field type BYTES but got " << FieldTypeDebugString(field->type);
        }
        return result;
    }

} // namespace picoproto
