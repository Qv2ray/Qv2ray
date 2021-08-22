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

/*
 See the README for full details, but this module lets you read in protobuf
 encoded files with a minimal code footprint.

 It doesn't create classes for each kind of message it encounters, it just has a
 single Message interface that lets you access the members of the protobuf as a
 key/value store. This loses a lot of the convenience of type-checked classes,
 but it does mean that very little code is needed to access data from files.

 As a simple example, if you had read a `bytes_size` long file into `bytes` that
 contained a TensorFlow GraphDef proto:

 Message graph_def;
 graph_def.ParseFromBytes(bytes, bytes_size);

 You can then access the different fields of the GraphDef using the field
 numbers assigned in the .proto file:

 std::vector<picoproto::Message*> nodes = graph_def.GetMessageArray(1);

 One big difference between this minimal approach and normal protobufs is that
 the calling code has to already know the field number and type of any members
 it's trying to access. Here I know that the `node` field is number 1, and that
 it should contain a repeated list of NodeDefs. Since they are not primitive
 types like numbers or strings, they are accessed as an array of Messages.

 Here are the design goals of this module:
  - Keep the code size tiny (single-digit kilobytes on most platforms).
  - Minimize memory usage (for example allow in-place references to byte data).
  - Provide a simple, readable implementation that can be ported easily.
  - Deserialize all saved protobuf files into a usable representation.
  - No dependencies other than the standard C++ library.
  - No build-time support (e.g. protoc) required.

 Here's what it's explicitly not offering:
  - Providing a readable and transparent way of accessing serialized data.
  - Saving out data to protobuf format.

*/

#ifndef INCLUDE_PICOPROTO_H
#define INCLUDE_PICOPROTO_H

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <vector>

// To keep dependencies minimal, some bare-bones macros to make logging easier.
#define PP_LOG(X) PP_LOG_##X
#define PP_LOG_INFO std::cerr << __FILE__ << ":" << __LINE__ << " - INFO: "
#define PP_LOG_WARN std::cerr << __FILE__ << ":" << __LINE__ << " - WARN: "
#define PP_LOG_ERROR std::cerr << __FILE__ << ":" << __LINE__ << " - ERROR: "
#define PP_CHECK(X)                                                                                                                                                      \
    if (!(X))                                                                                                                                                            \
    PP_LOG(ERROR) << "PP_CHECK(" << #X << ") failed. "

namespace picoproto
{

    // These roughly correspond to the wire types used to save data in protobuf
    // files. The best reference to understand the full format is:
    // https://developers.google.com/protocol-buffers/docs/encoding
    // Because we don't know the bit-depth of VarInts, they're always stored as
    // uint64 values, which is why there's no specific type for them.
    enum FieldType
    {
        FIELD_UNSET,
        FIELD_UINT32,
        FIELD_UINT64,
        FIELD_BYTES,
    };

    // Gives a readable name for the field type for logging purposes.
    std::string FieldTypeDebugString(enum FieldType type);

    // Forward declare the main message class, since fields can contain them.
    class Message;

    // Fields are the building blocks of messages. They contain the values for each
    // data member, and handle all the allocation and deallocation of storage.
    // It's unlikely you'll want to access this class directly, since you'll
    // normally want to use Message below to pull typed values.
    class Field
    {
      public:
        // You need to specify the type of a Field on creation, so that the right
        // storage can be set up for the values. You also need to indicate whether the
        // underlying memory will be around for the lifetime of the message (in which
        // case no copies are needed) or whether the class should make copies and take
        // ownership in case the data goes away.
        Field(FieldType type, bool owns_data);
        Field(const Field &other);
        ~Field();

        enum FieldType type;
        // I know, this isn't very OOP, but the simplicity of keeping track of a type
        // and deciding how to initialize and access the data based on that persuaded
        // me this was the best approach. The `value` member contains whatever data
        // the field should be holding.
        union
        {
            std::vector<uint32_t> *v_uint32;
            std::vector<uint64_t> *v_uint64;
            std::vector<std::pair<uint8_t *, size_t>> *v_bytes;
        } value;
        // One of the drawbacks of not requiring .proto files ahead of time is that I
        // don't know if a length-delimited field contains raw bytes, strings, or
        // sub-messages. The only time we know that a field should be interpreted as a
        // message is when client code requests it in that form. Because parsing can
        // be costly, here we cache the results of any such calls for subsequent
        // accesses.
        std::vector<Message *> *cached_messages;
        // If this is set, then the object will allocate its own storage for
        // length-delimited values, and copy from the input stream. If you know the
        // underlying data will be around for the lifetime of the message, you can
        // save memory and copies by leaving this as false.
        bool owns_data;
    };

    // The main interface for loading and accessing serialized protobuf data.
    class Message
    {
      public:
        // If you're not sure about the lifetime of any binary data you're reading
        // from, just call this default constructor.
        Message();
        // In the case when you're sure the lifetime of the byte stream you'll be
        // decoding is longer than the lifetime of the message, you can set
        // copy_arrays to false. This is especially useful if you have a memory
        // mapped file to read from containing large binary blobs, since you'll skip
        // a lot of copying and extra allocation.
        Message(bool copy_arrays);
        Message(const Message &other);
        ~Message();

        // Populates fields with all of the data from this stream of bytes.
        // You can call this repeatedly with new messages, and the results will be
        // merged together.
        bool ParseFromBytes(uint8_t *binary, size_t binary_size);

        // These are the accessor functions if you're expecting exactly one value in a
        // field. As discussed above, the burden is on the client code to know the
        // field number and type of each member it's trying to access, and so pick the
        // correct accessor function.
        // If the field isn't present, this will raise an error, so if it's optional
        // you should use the array accessors below.
        int32_t GetInt32(int32_t number);
        int64_t GetInt64(int32_t number);
        uint32_t GetUInt32(int32_t number);
        uint64_t GetUInt64(int32_t number);
        int64_t GetInt(int32_t number);
        bool GetBool(int32_t number);
        float GetFloat(int32_t number);
        double GetDouble(int32_t number);
        std::pair<uint8_t *, size_t> GetBytes(int32_t number);
        std::string GetString(int32_t number);
        Message *GetMessage(int32_t number);

        // If you're not sure if a value will be present, or if it is repeated, you
        // should call these array functions. If no such field has been seen, then the
        // result will be an empty vector, otherwise you'll get back one or more
        // entries.
        std::vector<int32_t> GetInt32Array(int32_t number);
        std::vector<int64_t> GetInt64Array(int32_t number);
        std::vector<uint32_t> GetUInt32Array(int32_t number);
        std::vector<uint64_t> GetUInt64Array(int32_t number);
        std::vector<bool> GetBoolArray(int32_t number);
        std::vector<float> GetFloatArray(int32_t number);
        std::vector<double> GetDoubleArray(int32_t number);
        std::vector<std::pair<uint8_t *, size_t>> GetByteArray(int32_t number);
        std::vector<std::string> GetStringArray(int32_t number);
        std::vector<Message *> GetMessageArray(int32_t number);

        // It's unlikely you'll want to access fields directly, but here's an escape
        // hatch in case you do have to manipulate them more directly.
        Field *GetField(int32_t number);

      private:
        // Inserts a new field, updating all the internal data structures.
        Field *AddField(int32_t number, enum FieldType type);

        Field *GetFieldAndCheckType(int32_t number, enum FieldType type);

        // Maps from a field number to an index in the `fields` vector.
        std::map<int32_t, size_t> field_map;
        // The core list of fields that have been parsed.
        std::vector<Field> fields;
        bool copy_arrays;
    };

} // namespace picoproto

#endif // INCLUDE_PICOPROTO_H
