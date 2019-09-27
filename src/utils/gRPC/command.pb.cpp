#include "command.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>

extern PROTOBUF_INTERNAL_EXPORT_command_2eproto ::google::protobuf::internal::SCCInfo<0> scc_info_Stat_command_2eproto;
namespace v2ray
{
    namespace core
    {
        namespace app
        {
            namespace stats
            {
                namespace command
                {
                    class GetStatsRequestDefaultTypeInternal
                    {
                        public:
                            ::google::protobuf::internal::ExplicitlyConstructed<GetStatsRequest> _instance;
                    } _GetStatsRequest_default_instance_;
                    class StatDefaultTypeInternal
                    {
                        public:
                            ::google::protobuf::internal::ExplicitlyConstructed<Stat> _instance;
                    } _Stat_default_instance_;
                    class GetStatsResponseDefaultTypeInternal
                    {
                        public:
                            ::google::protobuf::internal::ExplicitlyConstructed<GetStatsResponse> _instance;
                    } _GetStatsResponse_default_instance_;
                    class QueryStatsRequestDefaultTypeInternal
                    {
                        public:
                            ::google::protobuf::internal::ExplicitlyConstructed<QueryStatsRequest> _instance;
                    } _QueryStatsRequest_default_instance_;
                    class QueryStatsResponseDefaultTypeInternal
                    {
                        public:
                            ::google::protobuf::internal::ExplicitlyConstructed<QueryStatsResponse> _instance;
                    } _QueryStatsResponse_default_instance_;
                    class SysStatsRequestDefaultTypeInternal
                    {
                        public:
                            ::google::protobuf::internal::ExplicitlyConstructed<SysStatsRequest> _instance;
                    } _SysStatsRequest_default_instance_;
                    class SysStatsResponseDefaultTypeInternal
                    {
                        public:
                            ::google::protobuf::internal::ExplicitlyConstructed<SysStatsResponse> _instance;
                    } _SysStatsResponse_default_instance_;
                    class ConfigDefaultTypeInternal
                    {
                        public:
                            ::google::protobuf::internal::ExplicitlyConstructed<Config> _instance;
                    } _Config_default_instance_;
                }  // namespace command
            }  // namespace stats
        }  // namespace app
    }  // namespace core
}  // namespace v2ray
static void InitDefaultsGetStatsRequest_command_2eproto()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    {
        void *ptr = &::v2ray::core::app::stats::command::_GetStatsRequest_default_instance_;
        new (ptr) ::v2ray::core::app::stats::command::GetStatsRequest();
        ::google::protobuf::internal::OnShutdownDestroyMessage(ptr);
    }
    ::v2ray::core::app::stats::command::GetStatsRequest::InitAsDefaultInstance();
}

::google::protobuf::internal::SCCInfo<0> scc_info_GetStatsRequest_command_2eproto =
{{ATOMIC_VAR_INIT(::google::protobuf::internal::SCCInfoBase::kUninitialized), 0, InitDefaultsGetStatsRequest_command_2eproto}, {}};

static void InitDefaultsStat_command_2eproto()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    {
        void *ptr = &::v2ray::core::app::stats::command::_Stat_default_instance_;
        new (ptr) ::v2ray::core::app::stats::command::Stat();
        ::google::protobuf::internal::OnShutdownDestroyMessage(ptr);
    }
    ::v2ray::core::app::stats::command::Stat::InitAsDefaultInstance();
}

::google::protobuf::internal::SCCInfo<0> scc_info_Stat_command_2eproto =
{{ATOMIC_VAR_INIT(::google::protobuf::internal::SCCInfoBase::kUninitialized), 0, InitDefaultsStat_command_2eproto}, {}};

static void InitDefaultsGetStatsResponse_command_2eproto()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    {
        void *ptr = &::v2ray::core::app::stats::command::_GetStatsResponse_default_instance_;
        new (ptr) ::v2ray::core::app::stats::command::GetStatsResponse();
        ::google::protobuf::internal::OnShutdownDestroyMessage(ptr);
    }
    ::v2ray::core::app::stats::command::GetStatsResponse::InitAsDefaultInstance();
}

::google::protobuf::internal::SCCInfo<1> scc_info_GetStatsResponse_command_2eproto = {
    {ATOMIC_VAR_INIT(::google::protobuf::internal::SCCInfoBase::kUninitialized), 1, InitDefaultsGetStatsResponse_command_2eproto}, {
        &scc_info_Stat_command_2eproto.base,
    }
};

static void InitDefaultsQueryStatsRequest_command_2eproto()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    {
        void *ptr = &::v2ray::core::app::stats::command::_QueryStatsRequest_default_instance_;
        new (ptr) ::v2ray::core::app::stats::command::QueryStatsRequest();
        ::google::protobuf::internal::OnShutdownDestroyMessage(ptr);
    }
    ::v2ray::core::app::stats::command::QueryStatsRequest::InitAsDefaultInstance();
}

::google::protobuf::internal::SCCInfo<0> scc_info_QueryStatsRequest_command_2eproto =
{{ATOMIC_VAR_INIT(::google::protobuf::internal::SCCInfoBase::kUninitialized), 0, InitDefaultsQueryStatsRequest_command_2eproto}, {}};

static void InitDefaultsQueryStatsResponse_command_2eproto()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    {
        void *ptr = &::v2ray::core::app::stats::command::_QueryStatsResponse_default_instance_;
        new (ptr) ::v2ray::core::app::stats::command::QueryStatsResponse();
        ::google::protobuf::internal::OnShutdownDestroyMessage(ptr);
    }
    ::v2ray::core::app::stats::command::QueryStatsResponse::InitAsDefaultInstance();
}

::google::protobuf::internal::SCCInfo<1> scc_info_QueryStatsResponse_command_2eproto = {
    {ATOMIC_VAR_INIT(::google::protobuf::internal::SCCInfoBase::kUninitialized), 1, InitDefaultsQueryStatsResponse_command_2eproto}, {
        &scc_info_Stat_command_2eproto.base,
    }
};

static void InitDefaultsSysStatsRequest_command_2eproto()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    {
        void *ptr = &::v2ray::core::app::stats::command::_SysStatsRequest_default_instance_;
        new (ptr) ::v2ray::core::app::stats::command::SysStatsRequest();
        ::google::protobuf::internal::OnShutdownDestroyMessage(ptr);
    }
    ::v2ray::core::app::stats::command::SysStatsRequest::InitAsDefaultInstance();
}

::google::protobuf::internal::SCCInfo<0> scc_info_SysStatsRequest_command_2eproto =
{{ATOMIC_VAR_INIT(::google::protobuf::internal::SCCInfoBase::kUninitialized), 0, InitDefaultsSysStatsRequest_command_2eproto}, {}};

static void InitDefaultsSysStatsResponse_command_2eproto()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    {
        void *ptr = &::v2ray::core::app::stats::command::_SysStatsResponse_default_instance_;
        new (ptr) ::v2ray::core::app::stats::command::SysStatsResponse();
        ::google::protobuf::internal::OnShutdownDestroyMessage(ptr);
    }
    ::v2ray::core::app::stats::command::SysStatsResponse::InitAsDefaultInstance();
}

::google::protobuf::internal::SCCInfo<0> scc_info_SysStatsResponse_command_2eproto =
{{ATOMIC_VAR_INIT(::google::protobuf::internal::SCCInfoBase::kUninitialized), 0, InitDefaultsSysStatsResponse_command_2eproto}, {}};

static void InitDefaultsConfig_command_2eproto()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    {
        void *ptr = &::v2ray::core::app::stats::command::_Config_default_instance_;
        new (ptr) ::v2ray::core::app::stats::command::Config();
        ::google::protobuf::internal::OnShutdownDestroyMessage(ptr);
    }
    ::v2ray::core::app::stats::command::Config::InitAsDefaultInstance();
}

::google::protobuf::internal::SCCInfo<0> scc_info_Config_command_2eproto =
{{ATOMIC_VAR_INIT(::google::protobuf::internal::SCCInfoBase::kUninitialized), 0, InitDefaultsConfig_command_2eproto}, {}};

void InitDefaults_command_2eproto()
{
    ::google::protobuf::internal::InitSCC(&scc_info_GetStatsRequest_command_2eproto.base);
    ::google::protobuf::internal::InitSCC(&scc_info_Stat_command_2eproto.base);
    ::google::protobuf::internal::InitSCC(&scc_info_GetStatsResponse_command_2eproto.base);
    ::google::protobuf::internal::InitSCC(&scc_info_QueryStatsRequest_command_2eproto.base);
    ::google::protobuf::internal::InitSCC(&scc_info_QueryStatsResponse_command_2eproto.base);
    ::google::protobuf::internal::InitSCC(&scc_info_SysStatsRequest_command_2eproto.base);
    ::google::protobuf::internal::InitSCC(&scc_info_SysStatsResponse_command_2eproto.base);
    ::google::protobuf::internal::InitSCC(&scc_info_Config_command_2eproto.base);
}

::google::protobuf::Metadata file_level_metadata_command_2eproto[8];
constexpr ::google::protobuf::EnumDescriptor const **file_level_enum_descriptors_command_2eproto = nullptr;
constexpr ::google::protobuf::ServiceDescriptor const **file_level_service_descriptors_command_2eproto = nullptr;

const ::google::protobuf::uint32 TableStruct_command_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
    ~0u,  // no _has_bits_
    PROTOBUF_FIELD_OFFSET(::v2ray::core::app::stats::command::GetStatsRequest, _internal_metadata_),
    ~0u,  // no _extensions_
    ~0u,  // no _oneof_case_
    ~0u,  // no _weak_field_map_
    PROTOBUF_FIELD_OFFSET(::v2ray::core::app::stats::command::GetStatsRequest, name_),
    PROTOBUF_FIELD_OFFSET(::v2ray::core::app::stats::command::GetStatsRequest, reset_),
    ~0u,  // no _has_bits_
    PROTOBUF_FIELD_OFFSET(::v2ray::core::app::stats::command::Stat, _internal_metadata_),
    ~0u,  // no _extensions_
    ~0u,  // no _oneof_case_
    ~0u,  // no _weak_field_map_
    PROTOBUF_FIELD_OFFSET(::v2ray::core::app::stats::command::Stat, name_),
    PROTOBUF_FIELD_OFFSET(::v2ray::core::app::stats::command::Stat, value_),
    ~0u,  // no _has_bits_
    PROTOBUF_FIELD_OFFSET(::v2ray::core::app::stats::command::GetStatsResponse, _internal_metadata_),
    ~0u,  // no _extensions_
    ~0u,  // no _oneof_case_
    ~0u,  // no _weak_field_map_
    PROTOBUF_FIELD_OFFSET(::v2ray::core::app::stats::command::GetStatsResponse, stat_),
    ~0u,  // no _has_bits_
    PROTOBUF_FIELD_OFFSET(::v2ray::core::app::stats::command::QueryStatsRequest, _internal_metadata_),
    ~0u,  // no _extensions_
    ~0u,  // no _oneof_case_
    ~0u,  // no _weak_field_map_
    PROTOBUF_FIELD_OFFSET(::v2ray::core::app::stats::command::QueryStatsRequest, pattern_),
    PROTOBUF_FIELD_OFFSET(::v2ray::core::app::stats::command::QueryStatsRequest, reset_),
    ~0u,  // no _has_bits_
    PROTOBUF_FIELD_OFFSET(::v2ray::core::app::stats::command::QueryStatsResponse, _internal_metadata_),
    ~0u,  // no _extensions_
    ~0u,  // no _oneof_case_
    ~0u,  // no _weak_field_map_
    PROTOBUF_FIELD_OFFSET(::v2ray::core::app::stats::command::QueryStatsResponse, stat_),
    ~0u,  // no _has_bits_
    PROTOBUF_FIELD_OFFSET(::v2ray::core::app::stats::command::SysStatsRequest, _internal_metadata_),
    ~0u,  // no _extensions_
    ~0u,  // no _oneof_case_
    ~0u,  // no _weak_field_map_
    ~0u,  // no _has_bits_
    PROTOBUF_FIELD_OFFSET(::v2ray::core::app::stats::command::SysStatsResponse, _internal_metadata_),
    ~0u,  // no _extensions_
    ~0u,  // no _oneof_case_
    ~0u,  // no _weak_field_map_
    PROTOBUF_FIELD_OFFSET(::v2ray::core::app::stats::command::SysStatsResponse, numgoroutine_),
    PROTOBUF_FIELD_OFFSET(::v2ray::core::app::stats::command::SysStatsResponse, numgc_),
    PROTOBUF_FIELD_OFFSET(::v2ray::core::app::stats::command::SysStatsResponse, alloc_),
    PROTOBUF_FIELD_OFFSET(::v2ray::core::app::stats::command::SysStatsResponse, totalalloc_),
    PROTOBUF_FIELD_OFFSET(::v2ray::core::app::stats::command::SysStatsResponse, sys_),
    PROTOBUF_FIELD_OFFSET(::v2ray::core::app::stats::command::SysStatsResponse, mallocs_),
    PROTOBUF_FIELD_OFFSET(::v2ray::core::app::stats::command::SysStatsResponse, frees_),
    PROTOBUF_FIELD_OFFSET(::v2ray::core::app::stats::command::SysStatsResponse, liveobjects_),
    PROTOBUF_FIELD_OFFSET(::v2ray::core::app::stats::command::SysStatsResponse, pausetotalns_),
    PROTOBUF_FIELD_OFFSET(::v2ray::core::app::stats::command::SysStatsResponse, uptime_),
    ~0u,  // no _has_bits_
    PROTOBUF_FIELD_OFFSET(::v2ray::core::app::stats::command::Config, _internal_metadata_),
    ~0u,  // no _extensions_
    ~0u,  // no _oneof_case_
    ~0u,  // no _weak_field_map_
};
static const ::google::protobuf::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
    { 0, -1, sizeof(::v2ray::core::app::stats::command::GetStatsRequest)},
    { 7, -1, sizeof(::v2ray::core::app::stats::command::Stat)},
    { 14, -1, sizeof(::v2ray::core::app::stats::command::GetStatsResponse)},
    { 20, -1, sizeof(::v2ray::core::app::stats::command::QueryStatsRequest)},
    { 27, -1, sizeof(::v2ray::core::app::stats::command::QueryStatsResponse)},
    { 33, -1, sizeof(::v2ray::core::app::stats::command::SysStatsRequest)},
    { 38, -1, sizeof(::v2ray::core::app::stats::command::SysStatsResponse)},
    { 53, -1, sizeof(::v2ray::core::app::stats::command::Config)},
};

static ::google::protobuf::Message const *const file_default_instances[] = {
    reinterpret_cast<const ::google::protobuf::Message *>(&::v2ray::core::app::stats::command::_GetStatsRequest_default_instance_),
    reinterpret_cast<const ::google::protobuf::Message *>(&::v2ray::core::app::stats::command::_Stat_default_instance_),
    reinterpret_cast<const ::google::protobuf::Message *>(&::v2ray::core::app::stats::command::_GetStatsResponse_default_instance_),
    reinterpret_cast<const ::google::protobuf::Message *>(&::v2ray::core::app::stats::command::_QueryStatsRequest_default_instance_),
    reinterpret_cast<const ::google::protobuf::Message *>(&::v2ray::core::app::stats::command::_QueryStatsResponse_default_instance_),
    reinterpret_cast<const ::google::protobuf::Message *>(&::v2ray::core::app::stats::command::_SysStatsRequest_default_instance_),
    reinterpret_cast<const ::google::protobuf::Message *>(&::v2ray::core::app::stats::command::_SysStatsResponse_default_instance_),
    reinterpret_cast<const ::google::protobuf::Message *>(&::v2ray::core::app::stats::command::_Config_default_instance_),
};

::google::protobuf::internal::AssignDescriptorsTable assign_descriptors_table_command_2eproto = {
    {}, AddDescriptors_command_2eproto, "command.proto", schemas,
    file_default_instances, TableStruct_command_2eproto::offsets,
    file_level_metadata_command_2eproto, 8, file_level_enum_descriptors_command_2eproto, file_level_service_descriptors_command_2eproto,
};

const char descriptor_table_protodef_command_2eproto[] =
    "\n\rcommand.proto\022\034v2ray.core.app.stats.co"
    "mmand\".\n\017GetStatsRequest\022\014\n\004name\030\001 \001(\t\022\r"
    "\n\005reset\030\002 \001(\010\"#\n\004Stat\022\014\n\004name\030\001 \001(\t\022\r\n\005v"
    "alue\030\002 \001(\003\"D\n\020GetStatsResponse\0220\n\004stat\030\001"
    " \001(\0132\".v2ray.core.app.stats.command.Stat"
    "\"3\n\021QueryStatsRequest\022\017\n\007pattern\030\001 \001(\t\022\r"
    "\n\005reset\030\002 \001(\010\"F\n\022QueryStatsResponse\0220\n\004s"
    "tat\030\001 \003(\0132\".v2ray.core.app.stats.command"
    ".Stat\"\021\n\017SysStatsRequest\"\302\001\n\020SysStatsRes"
    "ponse\022\024\n\014NumGoroutine\030\001 \001(\r\022\r\n\005NumGC\030\002 \001"
    "(\r\022\r\n\005Alloc\030\003 \001(\004\022\022\n\nTotalAlloc\030\004 \001(\004\022\013\n"
    "\003Sys\030\005 \001(\004\022\017\n\007Mallocs\030\006 \001(\004\022\r\n\005Frees\030\007 \001"
    "(\004\022\023\n\013LiveObjects\030\010 \001(\004\022\024\n\014PauseTotalNs\030"
    "\t \001(\004\022\016\n\006Uptime\030\n \001(\r\"\010\n\006Config2\336\002\n\014Stat"
    "sService\022k\n\010GetStats\022-.v2ray.core.app.st"
    "ats.command.GetStatsRequest\032..v2ray.core"
    ".app.stats.command.GetStatsResponse\"\000\022q\n"
    "\nQueryStats\022/.v2ray.core.app.stats.comma"
    "nd.QueryStatsRequest\0320.v2ray.core.app.st"
    "ats.command.QueryStatsResponse\"\000\022n\n\013GetS"
    "ysStats\022-.v2ray.core.app.stats.command.S"
    "ysStatsRequest\032..v2ray.core.app.stats.co"
    "mmand.SysStatsResponse\"\000BL\n com.v2ray.co"
    "re.app.stats.commandP\001Z\007command\252\002\034V2Ray."
    "Core.App.Stats.Commandb\006proto3"
    ;
::google::protobuf::internal::DescriptorTable descriptor_table_command_2eproto = {
    false, InitDefaults_command_2eproto,
    descriptor_table_protodef_command_2eproto,
    "command.proto", &assign_descriptors_table_command_2eproto, 990,
};

void AddDescriptors_command_2eproto()
{
    static constexpr ::google::protobuf::internal::InitFunc deps[1] = {
    };
    ::google::protobuf::internal::AddDescriptors(&descriptor_table_command_2eproto, deps, 0);
}

// Force running AddDescriptors() at dynamic initialization time.
static bool dynamic_init_dummy_command_2eproto = []()
{
    AddDescriptors_command_2eproto();
    return true;
}
();
namespace v2ray
{
    namespace core
    {
        namespace app
        {
            namespace stats
            {
                namespace command
                {

                    // ===================================================================

                    void GetStatsRequest::InitAsDefaultInstance()
                    {
                    }
                    class GetStatsRequest::HasBitSetters
                    {
                        public:
                    };

#if !defined(_MSC_VER) || _MSC_VER >= 1900
                    const int GetStatsRequest::kNameFieldNumber;
                    const int GetStatsRequest::kResetFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

                    GetStatsRequest::GetStatsRequest()
                        : ::google::protobuf::Message(), _internal_metadata_(nullptr)
                    {
                        SharedCtor();
                        // @@protoc_insertion_point(constructor:v2ray.core.app.stats.command.GetStatsRequest)
                    }
                    GetStatsRequest::GetStatsRequest(const GetStatsRequest &from)
                        : ::google::protobuf::Message(),
                          _internal_metadata_(nullptr)
                    {
                        _internal_metadata_.MergeFrom(from._internal_metadata_);
                        name_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());

                        if (from.name().size() > 0) {
                            name_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.name_);
                        }

                        reset_ = from.reset_;
                        // @@protoc_insertion_point(copy_constructor:v2ray.core.app.stats.command.GetStatsRequest)
                    }

                    void GetStatsRequest::SharedCtor()
                    {
                        ::google::protobuf::internal::InitSCC(
                            &scc_info_GetStatsRequest_command_2eproto.base);
                        name_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
                        reset_ = false;
                    }

                    GetStatsRequest::~GetStatsRequest()
                    {
                        // @@protoc_insertion_point(destructor:v2ray.core.app.stats.command.GetStatsRequest)
                        SharedDtor();
                    }

                    void GetStatsRequest::SharedDtor()
                    {
                        name_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
                    }

                    void GetStatsRequest::SetCachedSize(int size) const
                    {
                        _cached_size_.Set(size);
                    }
                    const GetStatsRequest &GetStatsRequest::default_instance()
                    {
                        ::google::protobuf::internal::InitSCC(&::scc_info_GetStatsRequest_command_2eproto.base);
                        return *internal_default_instance();
                    }


                    void GetStatsRequest::Clear()
                    {
                        // @@protoc_insertion_point(message_clear_start:v2ray.core.app.stats.command.GetStatsRequest)
                        ::google::protobuf::uint32 cached_has_bits = 0;
                        // Prevent compiler warnings about cached_has_bits being unused
                        (void) cached_has_bits;
                        name_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
                        reset_ = false;
                        _internal_metadata_.Clear();
                    }

#if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
                    const char *GetStatsRequest::_InternalParse(const char *begin, const char *end, void *object,
                            ::google::protobuf::internal::ParseContext *ctx)
                    {
                        auto msg = static_cast<GetStatsRequest *>(object);
                        ::google::protobuf::int32 size;
                        (void)size;
                        int depth;
                        (void)depth;
                        ::google::protobuf::uint32 tag;
                        ::google::protobuf::internal::ParseFunc parser_till_end;
                        (void)parser_till_end;
                        auto ptr = begin;

                        while (ptr < end) {
                            ptr = ::google::protobuf::io::Parse32(ptr, &tag);
                            GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);

                            switch (tag >> 3) {
                                // string name = 1;
                                case 1: {
                                    if (static_cast<::google::protobuf::uint8>(tag) != 10) goto handle_unusual;

                                    ptr = ::google::protobuf::io::ReadSize(ptr, &size);
                                    GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
                                    ctx->extra_parse_data().SetFieldName("v2ray.core.app.stats.command.GetStatsRequest.name");
                                    object = msg->mutable_name();

                                    if (size > end - ptr + ::google::protobuf::internal::ParseContext::kSlopBytes) {
                                        parser_till_end = ::google::protobuf::internal::GreedyStringParserUTF8;
                                        goto string_till_end;
                                    }

                                    GOOGLE_PROTOBUF_PARSER_ASSERT(::google::protobuf::internal::StringCheckUTF8(ptr, size, ctx));
                                    ::google::protobuf::internal::InlineGreedyStringParser(object, ptr, size, ctx);
                                    ptr += size;
                                    break;
                                }

                                // bool reset = 2;
                                case 2: {
                                    if (static_cast<::google::protobuf::uint8>(tag) != 16) goto handle_unusual;

                                    msg->set_reset(::google::protobuf::internal::ReadVarint(&ptr));
                                    GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
                                    break;
                                }

                                default: {
handle_unusual:

                                    if ((tag & 7) == 4 || tag == 0) {
                                        ctx->EndGroup(tag);
                                        return ptr;
                                    }

                                    auto res = UnknownFieldParse(tag, {_InternalParse, msg},
                                                                 ptr, end, msg->_internal_metadata_.mutable_unknown_fields(), ctx);
                                    ptr = res.first;
                                    GOOGLE_PROTOBUF_PARSER_ASSERT(ptr != nullptr);

                                    if (res.second) return ptr;
                                }
                            }  // switch
                        }  // while

                        return ptr;
string_till_end:
                        static_cast<::std::string *>(object)->clear();
                        static_cast<::std::string *>(object)->reserve(size);
                        goto len_delim_till_end;
len_delim_till_end:
                        return ctx->StoreAndTailCall(ptr, end, {_InternalParse, msg},
                        {parser_till_end, object}, size);
                    }
#else  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
                    bool GetStatsRequest::MergePartialFromCodedStream(
                        ::google::protobuf::io::CodedInputStream *input)
                    {
#define DO_(EXPRESSION) if (!PROTOBUF_PREDICT_TRUE(EXPRESSION)) goto failure
                        ::google::protobuf::uint32 tag;

                        // @@protoc_insertion_point(parse_start:v2ray.core.app.stats.command.GetStatsRequest)
                        for (;;) {
                            ::std::pair<::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
                            tag = p.first;

                            if (!p.second) goto handle_unusual;

                            switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
                                // string name = 1;
                                case 1: {
                                    if (static_cast< ::google::protobuf::uint8>(tag) == (10 & 0xFF)) {
                                        DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                                                input, this->mutable_name()));
                                        DO_(::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
                                                this->name().data(), static_cast<int>(this->name().length()),
                                                ::google::protobuf::internal::WireFormatLite::PARSE,
                                                "v2ray.core.app.stats.command.GetStatsRequest.name"));
                                    } else {
                                        goto handle_unusual;
                                    }

                                    break;
                                }

                                // bool reset = 2;
                                case 2: {
                                    if (static_cast< ::google::protobuf::uint8>(tag) == (16 & 0xFF)) {
                                        DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive <
                                             bool, ::google::protobuf::internal::WireFormatLite::TYPE_BOOL > (
                                                 input, &reset_)));
                                    } else {
                                        goto handle_unusual;
                                    }

                                    break;
                                }

                                default: {
handle_unusual:

                                    if (tag == 0) {
                                        goto success;
                                    }

                                    DO_(::google::protobuf::internal::WireFormat::SkipField(
                                            input, tag, _internal_metadata_.mutable_unknown_fields()));
                                    break;
                                }
                            }
                        }

success:
                        // @@protoc_insertion_point(parse_success:v2ray.core.app.stats.command.GetStatsRequest)
                        return true;
failure:
                        // @@protoc_insertion_point(parse_failure:v2ray.core.app.stats.command.GetStatsRequest)
                        return false;
#undef DO_
                    }
#endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER

                    void GetStatsRequest::SerializeWithCachedSizes(
                        ::google::protobuf::io::CodedOutputStream *output) const
                    {
                        // @@protoc_insertion_point(serialize_start:v2ray.core.app.stats.command.GetStatsRequest)
                        ::google::protobuf::uint32 cached_has_bits = 0;
                        (void) cached_has_bits;

                        // string name = 1;
                        if (this->name().size() > 0) {
                            ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
                                this->name().data(), static_cast<int>(this->name().length()),
                                ::google::protobuf::internal::WireFormatLite::SERIALIZE,
                                "v2ray.core.app.stats.command.GetStatsRequest.name");
                            ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
                                1, this->name(), output);
                        }

                        // bool reset = 2;
                        if (this->reset() != 0) {
                            ::google::protobuf::internal::WireFormatLite::WriteBool(2, this->reset(), output);
                        }

                        if (_internal_metadata_.have_unknown_fields()) {
                            ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
                                _internal_metadata_.unknown_fields(), output);
                        }

                        // @@protoc_insertion_point(serialize_end:v2ray.core.app.stats.command.GetStatsRequest)
                    }

                    ::google::protobuf::uint8 *GetStatsRequest::InternalSerializeWithCachedSizesToArray(
                        ::google::protobuf::uint8 *target) const
                    {
                        // @@protoc_insertion_point(serialize_to_array_start:v2ray.core.app.stats.command.GetStatsRequest)
                        ::google::protobuf::uint32 cached_has_bits = 0;
                        (void) cached_has_bits;

                        // string name = 1;
                        if (this->name().size() > 0) {
                            ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
                                this->name().data(), static_cast<int>(this->name().length()),
                                ::google::protobuf::internal::WireFormatLite::SERIALIZE,
                                "v2ray.core.app.stats.command.GetStatsRequest.name");
                            target =
                                ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
                                    1, this->name(), target);
                        }

                        // bool reset = 2;
                        if (this->reset() != 0) {
                            target = ::google::protobuf::internal::WireFormatLite::WriteBoolToArray(2, this->reset(), target);
                        }

                        if (_internal_metadata_.have_unknown_fields()) {
                            target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
                                         _internal_metadata_.unknown_fields(), target);
                        }

                        // @@protoc_insertion_point(serialize_to_array_end:v2ray.core.app.stats.command.GetStatsRequest)
                        return target;
                    }

                    size_t GetStatsRequest::ByteSizeLong() const
                    {
                        // @@protoc_insertion_point(message_byte_size_start:v2ray.core.app.stats.command.GetStatsRequest)
                        size_t total_size = 0;

                        if (_internal_metadata_.have_unknown_fields()) {
                            total_size +=
                                ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
                                    _internal_metadata_.unknown_fields());
                        }

                        ::google::protobuf::uint32 cached_has_bits = 0;
                        // Prevent compiler warnings about cached_has_bits being unused
                        (void) cached_has_bits;

                        // string name = 1;
                        if (this->name().size() > 0) {
                            total_size += 1 +
                                          ::google::protobuf::internal::WireFormatLite::StringSize(
                                              this->name());
                        }

                        // bool reset = 2;
                        if (this->reset() != 0) {
                            total_size += 1 + 1;
                        }

                        int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
                        SetCachedSize(cached_size);
                        return total_size;
                    }

                    void GetStatsRequest::MergeFrom(const ::google::protobuf::Message &from)
                    {
                        // @@protoc_insertion_point(generalized_merge_from_start:v2ray.core.app.stats.command.GetStatsRequest)
                        GOOGLE_DCHECK_NE(&from, this);
                        const GetStatsRequest *source =
                            ::google::protobuf::DynamicCastToGenerated<GetStatsRequest>(
                                &from);

                        if (source == nullptr) {
                            // @@protoc_insertion_point(generalized_merge_from_cast_fail:v2ray.core.app.stats.command.GetStatsRequest)
                            ::google::protobuf::internal::ReflectionOps::Merge(from, this);
                        } else {
                            // @@protoc_insertion_point(generalized_merge_from_cast_success:v2ray.core.app.stats.command.GetStatsRequest)
                            MergeFrom(*source);
                        }
                    }

                    void GetStatsRequest::MergeFrom(const GetStatsRequest &from)
                    {
                        // @@protoc_insertion_point(class_specific_merge_from_start:v2ray.core.app.stats.command.GetStatsRequest)
                        GOOGLE_DCHECK_NE(&from, this);
                        _internal_metadata_.MergeFrom(from._internal_metadata_);
                        ::google::protobuf::uint32 cached_has_bits = 0;
                        (void) cached_has_bits;

                        if (from.name().size() > 0) {
                            name_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.name_);
                        }

                        if (from.reset() != 0) {
                            set_reset(from.reset());
                        }
                    }

                    void GetStatsRequest::CopyFrom(const ::google::protobuf::Message &from)
                    {
                        // @@protoc_insertion_point(generalized_copy_from_start:v2ray.core.app.stats.command.GetStatsRequest)
                        if (&from == this) return;

                        Clear();
                        MergeFrom(from);
                    }

                    void GetStatsRequest::CopyFrom(const GetStatsRequest &from)
                    {
                        // @@protoc_insertion_point(class_specific_copy_from_start:v2ray.core.app.stats.command.GetStatsRequest)
                        if (&from == this) return;

                        Clear();
                        MergeFrom(from);
                    }

                    bool GetStatsRequest::IsInitialized() const
                    {
                        return true;
                    }

                    void GetStatsRequest::Swap(GetStatsRequest *other)
                    {
                        if (other == this) return;

                        InternalSwap(other);
                    }
                    void GetStatsRequest::InternalSwap(GetStatsRequest *other)
                    {
                        using std::swap;
                        _internal_metadata_.Swap(&other->_internal_metadata_);
                        name_.Swap(&other->name_, &::google::protobuf::internal::GetEmptyStringAlreadyInited(),
                                   GetArenaNoVirtual());
                        swap(reset_, other->reset_);
                    }

                    ::google::protobuf::Metadata GetStatsRequest::GetMetadata() const
                    {
                        ::google::protobuf::internal::AssignDescriptors(&::assign_descriptors_table_command_2eproto);
                        return ::file_level_metadata_command_2eproto[kIndexInFileMessages];
                    }


                    // ===================================================================

                    void Stat::InitAsDefaultInstance()
                    {
                    }
                    class Stat::HasBitSetters
                    {
                        public:
                    };

#if !defined(_MSC_VER) || _MSC_VER >= 1900
                    const int Stat::kNameFieldNumber;
                    const int Stat::kValueFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

                    Stat::Stat()
                        : ::google::protobuf::Message(), _internal_metadata_(nullptr)
                    {
                        SharedCtor();
                        // @@protoc_insertion_point(constructor:v2ray.core.app.stats.command.Stat)
                    }
                    Stat::Stat(const Stat &from)
                        : ::google::protobuf::Message(),
                          _internal_metadata_(nullptr)
                    {
                        _internal_metadata_.MergeFrom(from._internal_metadata_);
                        name_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());

                        if (from.name().size() > 0) {
                            name_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.name_);
                        }

                        value_ = from.value_;
                        // @@protoc_insertion_point(copy_constructor:v2ray.core.app.stats.command.Stat)
                    }

                    void Stat::SharedCtor()
                    {
                        ::google::protobuf::internal::InitSCC(
                            &scc_info_Stat_command_2eproto.base);
                        name_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
                        value_ = PROTOBUF_LONGLONG(0);
                    }

                    Stat::~Stat()
                    {
                        // @@protoc_insertion_point(destructor:v2ray.core.app.stats.command.Stat)
                        SharedDtor();
                    }

                    void Stat::SharedDtor()
                    {
                        name_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
                    }

                    void Stat::SetCachedSize(int size) const
                    {
                        _cached_size_.Set(size);
                    }
                    const Stat &Stat::default_instance()
                    {
                        ::google::protobuf::internal::InitSCC(&::scc_info_Stat_command_2eproto.base);
                        return *internal_default_instance();
                    }


                    void Stat::Clear()
                    {
                        // @@protoc_insertion_point(message_clear_start:v2ray.core.app.stats.command.Stat)
                        ::google::protobuf::uint32 cached_has_bits = 0;
                        // Prevent compiler warnings about cached_has_bits being unused
                        (void) cached_has_bits;
                        name_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
                        value_ = PROTOBUF_LONGLONG(0);
                        _internal_metadata_.Clear();
                    }

#if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
                    const char *Stat::_InternalParse(const char *begin, const char *end, void *object,
                                                     ::google::protobuf::internal::ParseContext *ctx)
                    {
                        auto msg = static_cast<Stat *>(object);
                        ::google::protobuf::int32 size;
                        (void)size;
                        int depth;
                        (void)depth;
                        ::google::protobuf::uint32 tag;
                        ::google::protobuf::internal::ParseFunc parser_till_end;
                        (void)parser_till_end;
                        auto ptr = begin;

                        while (ptr < end) {
                            ptr = ::google::protobuf::io::Parse32(ptr, &tag);
                            GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);

                            switch (tag >> 3) {
                                // string name = 1;
                                case 1: {
                                    if (static_cast<::google::protobuf::uint8>(tag) != 10) goto handle_unusual;

                                    ptr = ::google::protobuf::io::ReadSize(ptr, &size);
                                    GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
                                    ctx->extra_parse_data().SetFieldName("v2ray.core.app.stats.command.Stat.name");
                                    object = msg->mutable_name();

                                    if (size > end - ptr + ::google::protobuf::internal::ParseContext::kSlopBytes) {
                                        parser_till_end = ::google::protobuf::internal::GreedyStringParserUTF8;
                                        goto string_till_end;
                                    }

                                    GOOGLE_PROTOBUF_PARSER_ASSERT(::google::protobuf::internal::StringCheckUTF8(ptr, size, ctx));
                                    ::google::protobuf::internal::InlineGreedyStringParser(object, ptr, size, ctx);
                                    ptr += size;
                                    break;
                                }

                                // int64 value = 2;
                                case 2: {
                                    if (static_cast<::google::protobuf::uint8>(tag) != 16) goto handle_unusual;

                                    msg->set_value(::google::protobuf::internal::ReadVarint(&ptr));
                                    GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
                                    break;
                                }

                                default: {
handle_unusual:

                                    if ((tag & 7) == 4 || tag == 0) {
                                        ctx->EndGroup(tag);
                                        return ptr;
                                    }

                                    auto res = UnknownFieldParse(tag, {_InternalParse, msg},
                                                                 ptr, end, msg->_internal_metadata_.mutable_unknown_fields(), ctx);
                                    ptr = res.first;
                                    GOOGLE_PROTOBUF_PARSER_ASSERT(ptr != nullptr);

                                    if (res.second) return ptr;
                                }
                            }  // switch
                        }  // while

                        return ptr;
string_till_end:
                        static_cast<::std::string *>(object)->clear();
                        static_cast<::std::string *>(object)->reserve(size);
                        goto len_delim_till_end;
len_delim_till_end:
                        return ctx->StoreAndTailCall(ptr, end, {_InternalParse, msg},
                        {parser_till_end, object}, size);
                    }
#else  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
                    bool Stat::MergePartialFromCodedStream(
                        ::google::protobuf::io::CodedInputStream *input)
                    {
#define DO_(EXPRESSION) if (!PROTOBUF_PREDICT_TRUE(EXPRESSION)) goto failure
                        ::google::protobuf::uint32 tag;

                        // @@protoc_insertion_point(parse_start:v2ray.core.app.stats.command.Stat)
                        for (;;) {
                            ::std::pair<::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
                            tag = p.first;

                            if (!p.second) goto handle_unusual;

                            switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
                                // string name = 1;
                                case 1: {
                                    if (static_cast< ::google::protobuf::uint8>(tag) == (10 & 0xFF)) {
                                        DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                                                input, this->mutable_name()));
                                        DO_(::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
                                                this->name().data(), static_cast<int>(this->name().length()),
                                                ::google::protobuf::internal::WireFormatLite::PARSE,
                                                "v2ray.core.app.stats.command.Stat.name"));
                                    } else {
                                        goto handle_unusual;
                                    }

                                    break;
                                }

                                // int64 value = 2;
                                case 2: {
                                    if (static_cast< ::google::protobuf::uint8>(tag) == (16 & 0xFF)) {
                                        DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive <
                                             ::google::protobuf::int64, ::google::protobuf::internal::WireFormatLite::TYPE_INT64 > (
                                                 input, &value_)));
                                    } else {
                                        goto handle_unusual;
                                    }

                                    break;
                                }

                                default: {
handle_unusual:

                                    if (tag == 0) {
                                        goto success;
                                    }

                                    DO_(::google::protobuf::internal::WireFormat::SkipField(
                                            input, tag, _internal_metadata_.mutable_unknown_fields()));
                                    break;
                                }
                            }
                        }

success:
                        // @@protoc_insertion_point(parse_success:v2ray.core.app.stats.command.Stat)
                        return true;
failure:
                        // @@protoc_insertion_point(parse_failure:v2ray.core.app.stats.command.Stat)
                        return false;
#undef DO_
                    }
#endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER

                    void Stat::SerializeWithCachedSizes(
                        ::google::protobuf::io::CodedOutputStream *output) const
                    {
                        // @@protoc_insertion_point(serialize_start:v2ray.core.app.stats.command.Stat)
                        ::google::protobuf::uint32 cached_has_bits = 0;
                        (void) cached_has_bits;

                        // string name = 1;
                        if (this->name().size() > 0) {
                            ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
                                this->name().data(), static_cast<int>(this->name().length()),
                                ::google::protobuf::internal::WireFormatLite::SERIALIZE,
                                "v2ray.core.app.stats.command.Stat.name");
                            ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
                                1, this->name(), output);
                        }

                        // int64 value = 2;
                        if (this->value() != 0) {
                            ::google::protobuf::internal::WireFormatLite::WriteInt64(2, this->value(), output);
                        }

                        if (_internal_metadata_.have_unknown_fields()) {
                            ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
                                _internal_metadata_.unknown_fields(), output);
                        }

                        // @@protoc_insertion_point(serialize_end:v2ray.core.app.stats.command.Stat)
                    }

                    ::google::protobuf::uint8 *Stat::InternalSerializeWithCachedSizesToArray(
                        ::google::protobuf::uint8 *target) const
                    {
                        // @@protoc_insertion_point(serialize_to_array_start:v2ray.core.app.stats.command.Stat)
                        ::google::protobuf::uint32 cached_has_bits = 0;
                        (void) cached_has_bits;

                        // string name = 1;
                        if (this->name().size() > 0) {
                            ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
                                this->name().data(), static_cast<int>(this->name().length()),
                                ::google::protobuf::internal::WireFormatLite::SERIALIZE,
                                "v2ray.core.app.stats.command.Stat.name");
                            target =
                                ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
                                    1, this->name(), target);
                        }

                        // int64 value = 2;
                        if (this->value() != 0) {
                            target = ::google::protobuf::internal::WireFormatLite::WriteInt64ToArray(2, this->value(), target);
                        }

                        if (_internal_metadata_.have_unknown_fields()) {
                            target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
                                         _internal_metadata_.unknown_fields(), target);
                        }

                        // @@protoc_insertion_point(serialize_to_array_end:v2ray.core.app.stats.command.Stat)
                        return target;
                    }

                    size_t Stat::ByteSizeLong() const
                    {
                        // @@protoc_insertion_point(message_byte_size_start:v2ray.core.app.stats.command.Stat)
                        size_t total_size = 0;

                        if (_internal_metadata_.have_unknown_fields()) {
                            total_size +=
                                ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
                                    _internal_metadata_.unknown_fields());
                        }

                        ::google::protobuf::uint32 cached_has_bits = 0;
                        // Prevent compiler warnings about cached_has_bits being unused
                        (void) cached_has_bits;

                        // string name = 1;
                        if (this->name().size() > 0) {
                            total_size += 1 +
                                          ::google::protobuf::internal::WireFormatLite::StringSize(
                                              this->name());
                        }

                        // int64 value = 2;
                        if (this->value() != 0) {
                            total_size += 1 +
                                          ::google::protobuf::internal::WireFormatLite::Int64Size(
                                              this->value());
                        }

                        int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
                        SetCachedSize(cached_size);
                        return total_size;
                    }

                    void Stat::MergeFrom(const ::google::protobuf::Message &from)
                    {
                        // @@protoc_insertion_point(generalized_merge_from_start:v2ray.core.app.stats.command.Stat)
                        GOOGLE_DCHECK_NE(&from, this);
                        const Stat *source =
                            ::google::protobuf::DynamicCastToGenerated<Stat>(
                                &from);

                        if (source == nullptr) {
                            // @@protoc_insertion_point(generalized_merge_from_cast_fail:v2ray.core.app.stats.command.Stat)
                            ::google::protobuf::internal::ReflectionOps::Merge(from, this);
                        } else {
                            // @@protoc_insertion_point(generalized_merge_from_cast_success:v2ray.core.app.stats.command.Stat)
                            MergeFrom(*source);
                        }
                    }

                    void Stat::MergeFrom(const Stat &from)
                    {
                        // @@protoc_insertion_point(class_specific_merge_from_start:v2ray.core.app.stats.command.Stat)
                        GOOGLE_DCHECK_NE(&from, this);
                        _internal_metadata_.MergeFrom(from._internal_metadata_);
                        ::google::protobuf::uint32 cached_has_bits = 0;
                        (void) cached_has_bits;

                        if (from.name().size() > 0) {
                            name_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.name_);
                        }

                        if (from.value() != 0) {
                            set_value(from.value());
                        }
                    }

                    void Stat::CopyFrom(const ::google::protobuf::Message &from)
                    {
                        // @@protoc_insertion_point(generalized_copy_from_start:v2ray.core.app.stats.command.Stat)
                        if (&from == this) return;

                        Clear();
                        MergeFrom(from);
                    }

                    void Stat::CopyFrom(const Stat &from)
                    {
                        // @@protoc_insertion_point(class_specific_copy_from_start:v2ray.core.app.stats.command.Stat)
                        if (&from == this) return;

                        Clear();
                        MergeFrom(from);
                    }

                    bool Stat::IsInitialized() const
                    {
                        return true;
                    }

                    void Stat::Swap(Stat *other)
                    {
                        if (other == this) return;

                        InternalSwap(other);
                    }
                    void Stat::InternalSwap(Stat *other)
                    {
                        using std::swap;
                        _internal_metadata_.Swap(&other->_internal_metadata_);
                        name_.Swap(&other->name_, &::google::protobuf::internal::GetEmptyStringAlreadyInited(),
                                   GetArenaNoVirtual());
                        swap(value_, other->value_);
                    }

                    ::google::protobuf::Metadata Stat::GetMetadata() const
                    {
                        ::google::protobuf::internal::AssignDescriptors(&::assign_descriptors_table_command_2eproto);
                        return ::file_level_metadata_command_2eproto[kIndexInFileMessages];
                    }


                    // ===================================================================

                    void GetStatsResponse::InitAsDefaultInstance()
                    {
                        ::v2ray::core::app::stats::command::_GetStatsResponse_default_instance_._instance.get_mutable()->stat_ = const_cast< ::v2ray::core::app::stats::command::Stat *>(
                                    ::v2ray::core::app::stats::command::Stat::internal_default_instance());
                    }
                    class GetStatsResponse::HasBitSetters
                    {
                        public:
                            static const ::v2ray::core::app::stats::command::Stat &stat(const GetStatsResponse *msg);
                    };

                    const ::v2ray::core::app::stats::command::Stat &
                    GetStatsResponse::HasBitSetters::stat(const GetStatsResponse *msg)
                    {
                        return *msg->stat_;
                    }
#if !defined(_MSC_VER) || _MSC_VER >= 1900
                    const int GetStatsResponse::kStatFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

                    GetStatsResponse::GetStatsResponse()
                        : ::google::protobuf::Message(), _internal_metadata_(nullptr)
                    {
                        SharedCtor();
                        // @@protoc_insertion_point(constructor:v2ray.core.app.stats.command.GetStatsResponse)
                    }
                    GetStatsResponse::GetStatsResponse(const GetStatsResponse &from)
                        : ::google::protobuf::Message(),
                          _internal_metadata_(nullptr)
                    {
                        _internal_metadata_.MergeFrom(from._internal_metadata_);

                        if (from.has_stat()) {
                            stat_ = new ::v2ray::core::app::stats::command::Stat(*from.stat_);
                        } else {
                            stat_ = nullptr;
                        }

                        // @@protoc_insertion_point(copy_constructor:v2ray.core.app.stats.command.GetStatsResponse)
                    }

                    void GetStatsResponse::SharedCtor()
                    {
                        ::google::protobuf::internal::InitSCC(
                            &scc_info_GetStatsResponse_command_2eproto.base);
                        stat_ = nullptr;
                    }

                    GetStatsResponse::~GetStatsResponse()
                    {
                        // @@protoc_insertion_point(destructor:v2ray.core.app.stats.command.GetStatsResponse)
                        SharedDtor();
                    }

                    void GetStatsResponse::SharedDtor()
                    {
                        if (this != internal_default_instance()) delete stat_;
                    }

                    void GetStatsResponse::SetCachedSize(int size) const
                    {
                        _cached_size_.Set(size);
                    }
                    const GetStatsResponse &GetStatsResponse::default_instance()
                    {
                        ::google::protobuf::internal::InitSCC(&::scc_info_GetStatsResponse_command_2eproto.base);
                        return *internal_default_instance();
                    }


                    void GetStatsResponse::Clear()
                    {
                        // @@protoc_insertion_point(message_clear_start:v2ray.core.app.stats.command.GetStatsResponse)
                        ::google::protobuf::uint32 cached_has_bits = 0;
                        // Prevent compiler warnings about cached_has_bits being unused
                        (void) cached_has_bits;

                        if (GetArenaNoVirtual() == nullptr && stat_ != nullptr) {
                            delete stat_;
                        }

                        stat_ = nullptr;
                        _internal_metadata_.Clear();
                    }

#if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
                    const char *GetStatsResponse::_InternalParse(const char *begin, const char *end, void *object,
                            ::google::protobuf::internal::ParseContext *ctx)
                    {
                        auto msg = static_cast<GetStatsResponse *>(object);
                        ::google::protobuf::int32 size;
                        (void)size;
                        int depth;
                        (void)depth;
                        ::google::protobuf::uint32 tag;
                        ::google::protobuf::internal::ParseFunc parser_till_end;
                        (void)parser_till_end;
                        auto ptr = begin;

                        while (ptr < end) {
                            ptr = ::google::protobuf::io::Parse32(ptr, &tag);
                            GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);

                            switch (tag >> 3) {
                                // .v2ray.core.app.stats.command.Stat stat = 1;
                                case 1: {
                                    if (static_cast<::google::protobuf::uint8>(tag) != 10) goto handle_unusual;

                                    ptr = ::google::protobuf::io::ReadSize(ptr, &size);
                                    GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
                                    parser_till_end = ::v2ray::core::app::stats::command::Stat::_InternalParse;
                                    object = msg->mutable_stat();

                                    if (size > end - ptr) goto len_delim_till_end;

                                    ptr += size;
                                    GOOGLE_PROTOBUF_PARSER_ASSERT(ctx->ParseExactRange(
                                    {parser_till_end, object}, ptr - size, ptr));
                                    break;
                                }

                                default: {
handle_unusual:

                                    if ((tag & 7) == 4 || tag == 0) {
                                        ctx->EndGroup(tag);
                                        return ptr;
                                    }

                                    auto res = UnknownFieldParse(tag, {_InternalParse, msg},
                                                                 ptr, end, msg->_internal_metadata_.mutable_unknown_fields(), ctx);
                                    ptr = res.first;
                                    GOOGLE_PROTOBUF_PARSER_ASSERT(ptr != nullptr);

                                    if (res.second) return ptr;
                                }
                            }  // switch
                        }  // while

                        return ptr;
len_delim_till_end:
                        return ctx->StoreAndTailCall(ptr, end, {_InternalParse, msg},
                        {parser_till_end, object}, size);
                    }
#else  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
                    bool GetStatsResponse::MergePartialFromCodedStream(
                        ::google::protobuf::io::CodedInputStream *input)
                    {
#define DO_(EXPRESSION) if (!PROTOBUF_PREDICT_TRUE(EXPRESSION)) goto failure
                        ::google::protobuf::uint32 tag;

                        // @@protoc_insertion_point(parse_start:v2ray.core.app.stats.command.GetStatsResponse)
                        for (;;) {
                            ::std::pair<::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
                            tag = p.first;

                            if (!p.second) goto handle_unusual;

                            switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
                                // .v2ray.core.app.stats.command.Stat stat = 1;
                                case 1: {
                                    if (static_cast< ::google::protobuf::uint8>(tag) == (10 & 0xFF)) {
                                        DO_(::google::protobuf::internal::WireFormatLite::ReadMessage(
                                                input, mutable_stat()));
                                    } else {
                                        goto handle_unusual;
                                    }

                                    break;
                                }

                                default: {
handle_unusual:

                                    if (tag == 0) {
                                        goto success;
                                    }

                                    DO_(::google::protobuf::internal::WireFormat::SkipField(
                                            input, tag, _internal_metadata_.mutable_unknown_fields()));
                                    break;
                                }
                            }
                        }

success:
                        // @@protoc_insertion_point(parse_success:v2ray.core.app.stats.command.GetStatsResponse)
                        return true;
failure:
                        // @@protoc_insertion_point(parse_failure:v2ray.core.app.stats.command.GetStatsResponse)
                        return false;
#undef DO_
                    }
#endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER

                    void GetStatsResponse::SerializeWithCachedSizes(
                        ::google::protobuf::io::CodedOutputStream *output) const
                    {
                        // @@protoc_insertion_point(serialize_start:v2ray.core.app.stats.command.GetStatsResponse)
                        ::google::protobuf::uint32 cached_has_bits = 0;
                        (void) cached_has_bits;

                        // .v2ray.core.app.stats.command.Stat stat = 1;
                        if (this->has_stat()) {
                            ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
                                1, HasBitSetters::stat(this), output);
                        }

                        if (_internal_metadata_.have_unknown_fields()) {
                            ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
                                _internal_metadata_.unknown_fields(), output);
                        }

                        // @@protoc_insertion_point(serialize_end:v2ray.core.app.stats.command.GetStatsResponse)
                    }

                    ::google::protobuf::uint8 *GetStatsResponse::InternalSerializeWithCachedSizesToArray(
                        ::google::protobuf::uint8 *target) const
                    {
                        // @@protoc_insertion_point(serialize_to_array_start:v2ray.core.app.stats.command.GetStatsResponse)
                        ::google::protobuf::uint32 cached_has_bits = 0;
                        (void) cached_has_bits;

                        // .v2ray.core.app.stats.command.Stat stat = 1;
                        if (this->has_stat()) {
                            target = ::google::protobuf::internal::WireFormatLite::
                                     InternalWriteMessageToArray(
                                         1, HasBitSetters::stat(this), target);
                        }

                        if (_internal_metadata_.have_unknown_fields()) {
                            target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
                                         _internal_metadata_.unknown_fields(), target);
                        }

                        // @@protoc_insertion_point(serialize_to_array_end:v2ray.core.app.stats.command.GetStatsResponse)
                        return target;
                    }

                    size_t GetStatsResponse::ByteSizeLong() const
                    {
                        // @@protoc_insertion_point(message_byte_size_start:v2ray.core.app.stats.command.GetStatsResponse)
                        size_t total_size = 0;

                        if (_internal_metadata_.have_unknown_fields()) {
                            total_size +=
                                ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
                                    _internal_metadata_.unknown_fields());
                        }

                        ::google::protobuf::uint32 cached_has_bits = 0;
                        // Prevent compiler warnings about cached_has_bits being unused
                        (void) cached_has_bits;

                        // .v2ray.core.app.stats.command.Stat stat = 1;
                        if (this->has_stat()) {
                            total_size += 1 +
                                          ::google::protobuf::internal::WireFormatLite::MessageSize(
                                              *stat_);
                        }

                        int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
                        SetCachedSize(cached_size);
                        return total_size;
                    }

                    void GetStatsResponse::MergeFrom(const ::google::protobuf::Message &from)
                    {
                        // @@protoc_insertion_point(generalized_merge_from_start:v2ray.core.app.stats.command.GetStatsResponse)
                        GOOGLE_DCHECK_NE(&from, this);
                        const GetStatsResponse *source =
                            ::google::protobuf::DynamicCastToGenerated<GetStatsResponse>(
                                &from);

                        if (source == nullptr) {
                            // @@protoc_insertion_point(generalized_merge_from_cast_fail:v2ray.core.app.stats.command.GetStatsResponse)
                            ::google::protobuf::internal::ReflectionOps::Merge(from, this);
                        } else {
                            // @@protoc_insertion_point(generalized_merge_from_cast_success:v2ray.core.app.stats.command.GetStatsResponse)
                            MergeFrom(*source);
                        }
                    }

                    void GetStatsResponse::MergeFrom(const GetStatsResponse &from)
                    {
                        // @@protoc_insertion_point(class_specific_merge_from_start:v2ray.core.app.stats.command.GetStatsResponse)
                        GOOGLE_DCHECK_NE(&from, this);
                        _internal_metadata_.MergeFrom(from._internal_metadata_);
                        ::google::protobuf::uint32 cached_has_bits = 0;
                        (void) cached_has_bits;

                        if (from.has_stat()) {
                            mutable_stat()->::v2ray::core::app::stats::command::Stat::MergeFrom(from.stat());
                        }
                    }

                    void GetStatsResponse::CopyFrom(const ::google::protobuf::Message &from)
                    {
                        // @@protoc_insertion_point(generalized_copy_from_start:v2ray.core.app.stats.command.GetStatsResponse)
                        if (&from == this) return;

                        Clear();
                        MergeFrom(from);
                    }

                    void GetStatsResponse::CopyFrom(const GetStatsResponse &from)
                    {
                        // @@protoc_insertion_point(class_specific_copy_from_start:v2ray.core.app.stats.command.GetStatsResponse)
                        if (&from == this) return;

                        Clear();
                        MergeFrom(from);
                    }

                    bool GetStatsResponse::IsInitialized() const
                    {
                        return true;
                    }

                    void GetStatsResponse::Swap(GetStatsResponse *other)
                    {
                        if (other == this) return;

                        InternalSwap(other);
                    }
                    void GetStatsResponse::InternalSwap(GetStatsResponse *other)
                    {
                        using std::swap;
                        _internal_metadata_.Swap(&other->_internal_metadata_);
                        swap(stat_, other->stat_);
                    }

                    ::google::protobuf::Metadata GetStatsResponse::GetMetadata() const
                    {
                        ::google::protobuf::internal::AssignDescriptors(&::assign_descriptors_table_command_2eproto);
                        return ::file_level_metadata_command_2eproto[kIndexInFileMessages];
                    }


                    // ===================================================================

                    void QueryStatsRequest::InitAsDefaultInstance()
                    {
                    }
                    class QueryStatsRequest::HasBitSetters
                    {
                        public:
                    };

#if !defined(_MSC_VER) || _MSC_VER >= 1900
                    const int QueryStatsRequest::kPatternFieldNumber;
                    const int QueryStatsRequest::kResetFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

                    QueryStatsRequest::QueryStatsRequest()
                        : ::google::protobuf::Message(), _internal_metadata_(nullptr)
                    {
                        SharedCtor();
                        // @@protoc_insertion_point(constructor:v2ray.core.app.stats.command.QueryStatsRequest)
                    }
                    QueryStatsRequest::QueryStatsRequest(const QueryStatsRequest &from)
                        : ::google::protobuf::Message(),
                          _internal_metadata_(nullptr)
                    {
                        _internal_metadata_.MergeFrom(from._internal_metadata_);
                        pattern_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());

                        if (from.pattern().size() > 0) {
                            pattern_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.pattern_);
                        }

                        reset_ = from.reset_;
                        // @@protoc_insertion_point(copy_constructor:v2ray.core.app.stats.command.QueryStatsRequest)
                    }

                    void QueryStatsRequest::SharedCtor()
                    {
                        ::google::protobuf::internal::InitSCC(
                            &scc_info_QueryStatsRequest_command_2eproto.base);
                        pattern_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
                        reset_ = false;
                    }

                    QueryStatsRequest::~QueryStatsRequest()
                    {
                        // @@protoc_insertion_point(destructor:v2ray.core.app.stats.command.QueryStatsRequest)
                        SharedDtor();
                    }

                    void QueryStatsRequest::SharedDtor()
                    {
                        pattern_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
                    }

                    void QueryStatsRequest::SetCachedSize(int size) const
                    {
                        _cached_size_.Set(size);
                    }
                    const QueryStatsRequest &QueryStatsRequest::default_instance()
                    {
                        ::google::protobuf::internal::InitSCC(&::scc_info_QueryStatsRequest_command_2eproto.base);
                        return *internal_default_instance();
                    }


                    void QueryStatsRequest::Clear()
                    {
                        // @@protoc_insertion_point(message_clear_start:v2ray.core.app.stats.command.QueryStatsRequest)
                        ::google::protobuf::uint32 cached_has_bits = 0;
                        // Prevent compiler warnings about cached_has_bits being unused
                        (void) cached_has_bits;
                        pattern_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
                        reset_ = false;
                        _internal_metadata_.Clear();
                    }

#if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
                    const char *QueryStatsRequest::_InternalParse(const char *begin, const char *end, void *object,
                            ::google::protobuf::internal::ParseContext *ctx)
                    {
                        auto msg = static_cast<QueryStatsRequest *>(object);
                        ::google::protobuf::int32 size;
                        (void)size;
                        int depth;
                        (void)depth;
                        ::google::protobuf::uint32 tag;
                        ::google::protobuf::internal::ParseFunc parser_till_end;
                        (void)parser_till_end;
                        auto ptr = begin;

                        while (ptr < end) {
                            ptr = ::google::protobuf::io::Parse32(ptr, &tag);
                            GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);

                            switch (tag >> 3) {
                                // string pattern = 1;
                                case 1: {
                                    if (static_cast<::google::protobuf::uint8>(tag) != 10) goto handle_unusual;

                                    ptr = ::google::protobuf::io::ReadSize(ptr, &size);
                                    GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
                                    ctx->extra_parse_data().SetFieldName("v2ray.core.app.stats.command.QueryStatsRequest.pattern");
                                    object = msg->mutable_pattern();

                                    if (size > end - ptr + ::google::protobuf::internal::ParseContext::kSlopBytes) {
                                        parser_till_end = ::google::protobuf::internal::GreedyStringParserUTF8;
                                        goto string_till_end;
                                    }

                                    GOOGLE_PROTOBUF_PARSER_ASSERT(::google::protobuf::internal::StringCheckUTF8(ptr, size, ctx));
                                    ::google::protobuf::internal::InlineGreedyStringParser(object, ptr, size, ctx);
                                    ptr += size;
                                    break;
                                }

                                // bool reset = 2;
                                case 2: {
                                    if (static_cast<::google::protobuf::uint8>(tag) != 16) goto handle_unusual;

                                    msg->set_reset(::google::protobuf::internal::ReadVarint(&ptr));
                                    GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
                                    break;
                                }

                                default: {
handle_unusual:

                                    if ((tag & 7) == 4 || tag == 0) {
                                        ctx->EndGroup(tag);
                                        return ptr;
                                    }

                                    auto res = UnknownFieldParse(tag, {_InternalParse, msg},
                                                                 ptr, end, msg->_internal_metadata_.mutable_unknown_fields(), ctx);
                                    ptr = res.first;
                                    GOOGLE_PROTOBUF_PARSER_ASSERT(ptr != nullptr);

                                    if (res.second) return ptr;
                                }
                            }  // switch
                        }  // while

                        return ptr;
string_till_end:
                        static_cast<::std::string *>(object)->clear();
                        static_cast<::std::string *>(object)->reserve(size);
                        goto len_delim_till_end;
len_delim_till_end:
                        return ctx->StoreAndTailCall(ptr, end, {_InternalParse, msg},
                        {parser_till_end, object}, size);
                    }
#else  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
                    bool QueryStatsRequest::MergePartialFromCodedStream(
                        ::google::protobuf::io::CodedInputStream *input)
                    {
#define DO_(EXPRESSION) if (!PROTOBUF_PREDICT_TRUE(EXPRESSION)) goto failure
                        ::google::protobuf::uint32 tag;

                        // @@protoc_insertion_point(parse_start:v2ray.core.app.stats.command.QueryStatsRequest)
                        for (;;) {
                            ::std::pair<::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
                            tag = p.first;

                            if (!p.second) goto handle_unusual;

                            switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
                                // string pattern = 1;
                                case 1: {
                                    if (static_cast< ::google::protobuf::uint8>(tag) == (10 & 0xFF)) {
                                        DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                                                input, this->mutable_pattern()));
                                        DO_(::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
                                                this->pattern().data(), static_cast<int>(this->pattern().length()),
                                                ::google::protobuf::internal::WireFormatLite::PARSE,
                                                "v2ray.core.app.stats.command.QueryStatsRequest.pattern"));
                                    } else {
                                        goto handle_unusual;
                                    }

                                    break;
                                }

                                // bool reset = 2;
                                case 2: {
                                    if (static_cast< ::google::protobuf::uint8>(tag) == (16 & 0xFF)) {
                                        DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive <
                                             bool, ::google::protobuf::internal::WireFormatLite::TYPE_BOOL > (
                                                 input, &reset_)));
                                    } else {
                                        goto handle_unusual;
                                    }

                                    break;
                                }

                                default: {
handle_unusual:

                                    if (tag == 0) {
                                        goto success;
                                    }

                                    DO_(::google::protobuf::internal::WireFormat::SkipField(
                                            input, tag, _internal_metadata_.mutable_unknown_fields()));
                                    break;
                                }
                            }
                        }

success:
                        // @@protoc_insertion_point(parse_success:v2ray.core.app.stats.command.QueryStatsRequest)
                        return true;
failure:
                        // @@protoc_insertion_point(parse_failure:v2ray.core.app.stats.command.QueryStatsRequest)
                        return false;
#undef DO_
                    }
#endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER

                    void QueryStatsRequest::SerializeWithCachedSizes(
                        ::google::protobuf::io::CodedOutputStream *output) const
                    {
                        // @@protoc_insertion_point(serialize_start:v2ray.core.app.stats.command.QueryStatsRequest)
                        ::google::protobuf::uint32 cached_has_bits = 0;
                        (void) cached_has_bits;

                        // string pattern = 1;
                        if (this->pattern().size() > 0) {
                            ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
                                this->pattern().data(), static_cast<int>(this->pattern().length()),
                                ::google::protobuf::internal::WireFormatLite::SERIALIZE,
                                "v2ray.core.app.stats.command.QueryStatsRequest.pattern");
                            ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
                                1, this->pattern(), output);
                        }

                        // bool reset = 2;
                        if (this->reset() != 0) {
                            ::google::protobuf::internal::WireFormatLite::WriteBool(2, this->reset(), output);
                        }

                        if (_internal_metadata_.have_unknown_fields()) {
                            ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
                                _internal_metadata_.unknown_fields(), output);
                        }

                        // @@protoc_insertion_point(serialize_end:v2ray.core.app.stats.command.QueryStatsRequest)
                    }

                    ::google::protobuf::uint8 *QueryStatsRequest::InternalSerializeWithCachedSizesToArray(
                        ::google::protobuf::uint8 *target) const
                    {
                        // @@protoc_insertion_point(serialize_to_array_start:v2ray.core.app.stats.command.QueryStatsRequest)
                        ::google::protobuf::uint32 cached_has_bits = 0;
                        (void) cached_has_bits;

                        // string pattern = 1;
                        if (this->pattern().size() > 0) {
                            ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
                                this->pattern().data(), static_cast<int>(this->pattern().length()),
                                ::google::protobuf::internal::WireFormatLite::SERIALIZE,
                                "v2ray.core.app.stats.command.QueryStatsRequest.pattern");
                            target =
                                ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
                                    1, this->pattern(), target);
                        }

                        // bool reset = 2;
                        if (this->reset() != 0) {
                            target = ::google::protobuf::internal::WireFormatLite::WriteBoolToArray(2, this->reset(), target);
                        }

                        if (_internal_metadata_.have_unknown_fields()) {
                            target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
                                         _internal_metadata_.unknown_fields(), target);
                        }

                        // @@protoc_insertion_point(serialize_to_array_end:v2ray.core.app.stats.command.QueryStatsRequest)
                        return target;
                    }

                    size_t QueryStatsRequest::ByteSizeLong() const
                    {
                        // @@protoc_insertion_point(message_byte_size_start:v2ray.core.app.stats.command.QueryStatsRequest)
                        size_t total_size = 0;

                        if (_internal_metadata_.have_unknown_fields()) {
                            total_size +=
                                ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
                                    _internal_metadata_.unknown_fields());
                        }

                        ::google::protobuf::uint32 cached_has_bits = 0;
                        // Prevent compiler warnings about cached_has_bits being unused
                        (void) cached_has_bits;

                        // string pattern = 1;
                        if (this->pattern().size() > 0) {
                            total_size += 1 +
                                          ::google::protobuf::internal::WireFormatLite::StringSize(
                                              this->pattern());
                        }

                        // bool reset = 2;
                        if (this->reset() != 0) {
                            total_size += 1 + 1;
                        }

                        int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
                        SetCachedSize(cached_size);
                        return total_size;
                    }

                    void QueryStatsRequest::MergeFrom(const ::google::protobuf::Message &from)
                    {
                        // @@protoc_insertion_point(generalized_merge_from_start:v2ray.core.app.stats.command.QueryStatsRequest)
                        GOOGLE_DCHECK_NE(&from, this);
                        const QueryStatsRequest *source =
                            ::google::protobuf::DynamicCastToGenerated<QueryStatsRequest>(
                                &from);

                        if (source == nullptr) {
                            // @@protoc_insertion_point(generalized_merge_from_cast_fail:v2ray.core.app.stats.command.QueryStatsRequest)
                            ::google::protobuf::internal::ReflectionOps::Merge(from, this);
                        } else {
                            // @@protoc_insertion_point(generalized_merge_from_cast_success:v2ray.core.app.stats.command.QueryStatsRequest)
                            MergeFrom(*source);
                        }
                    }

                    void QueryStatsRequest::MergeFrom(const QueryStatsRequest &from)
                    {
                        // @@protoc_insertion_point(class_specific_merge_from_start:v2ray.core.app.stats.command.QueryStatsRequest)
                        GOOGLE_DCHECK_NE(&from, this);
                        _internal_metadata_.MergeFrom(from._internal_metadata_);
                        ::google::protobuf::uint32 cached_has_bits = 0;
                        (void) cached_has_bits;

                        if (from.pattern().size() > 0) {
                            pattern_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.pattern_);
                        }

                        if (from.reset() != 0) {
                            set_reset(from.reset());
                        }
                    }

                    void QueryStatsRequest::CopyFrom(const ::google::protobuf::Message &from)
                    {
                        // @@protoc_insertion_point(generalized_copy_from_start:v2ray.core.app.stats.command.QueryStatsRequest)
                        if (&from == this) return;

                        Clear();
                        MergeFrom(from);
                    }

                    void QueryStatsRequest::CopyFrom(const QueryStatsRequest &from)
                    {
                        // @@protoc_insertion_point(class_specific_copy_from_start:v2ray.core.app.stats.command.QueryStatsRequest)
                        if (&from == this) return;

                        Clear();
                        MergeFrom(from);
                    }

                    bool QueryStatsRequest::IsInitialized() const
                    {
                        return true;
                    }

                    void QueryStatsRequest::Swap(QueryStatsRequest *other)
                    {
                        if (other == this) return;

                        InternalSwap(other);
                    }
                    void QueryStatsRequest::InternalSwap(QueryStatsRequest *other)
                    {
                        using std::swap;
                        _internal_metadata_.Swap(&other->_internal_metadata_);
                        pattern_.Swap(&other->pattern_, &::google::protobuf::internal::GetEmptyStringAlreadyInited(),
                                      GetArenaNoVirtual());
                        swap(reset_, other->reset_);
                    }

                    ::google::protobuf::Metadata QueryStatsRequest::GetMetadata() const
                    {
                        ::google::protobuf::internal::AssignDescriptors(&::assign_descriptors_table_command_2eproto);
                        return ::file_level_metadata_command_2eproto[kIndexInFileMessages];
                    }


                    // ===================================================================

                    void QueryStatsResponse::InitAsDefaultInstance()
                    {
                    }
                    class QueryStatsResponse::HasBitSetters
                    {
                        public:
                    };

#if !defined(_MSC_VER) || _MSC_VER >= 1900
                    const int QueryStatsResponse::kStatFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

                    QueryStatsResponse::QueryStatsResponse()
                        : ::google::protobuf::Message(), _internal_metadata_(nullptr)
                    {
                        SharedCtor();
                        // @@protoc_insertion_point(constructor:v2ray.core.app.stats.command.QueryStatsResponse)
                    }
                    QueryStatsResponse::QueryStatsResponse(const QueryStatsResponse &from)
                        : ::google::protobuf::Message(),
                          _internal_metadata_(nullptr),
                          stat_(from.stat_)
                    {
                        _internal_metadata_.MergeFrom(from._internal_metadata_);
                        // @@protoc_insertion_point(copy_constructor:v2ray.core.app.stats.command.QueryStatsResponse)
                    }

                    void QueryStatsResponse::SharedCtor()
                    {
                        ::google::protobuf::internal::InitSCC(
                            &scc_info_QueryStatsResponse_command_2eproto.base);
                    }

                    QueryStatsResponse::~QueryStatsResponse()
                    {
                        // @@protoc_insertion_point(destructor:v2ray.core.app.stats.command.QueryStatsResponse)
                        SharedDtor();
                    }

                    void QueryStatsResponse::SharedDtor()
                    {
                    }

                    void QueryStatsResponse::SetCachedSize(int size) const
                    {
                        _cached_size_.Set(size);
                    }
                    const QueryStatsResponse &QueryStatsResponse::default_instance()
                    {
                        ::google::protobuf::internal::InitSCC(&::scc_info_QueryStatsResponse_command_2eproto.base);
                        return *internal_default_instance();
                    }


                    void QueryStatsResponse::Clear()
                    {
                        // @@protoc_insertion_point(message_clear_start:v2ray.core.app.stats.command.QueryStatsResponse)
                        ::google::protobuf::uint32 cached_has_bits = 0;
                        // Prevent compiler warnings about cached_has_bits being unused
                        (void) cached_has_bits;
                        stat_.Clear();
                        _internal_metadata_.Clear();
                    }

#if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
                    const char *QueryStatsResponse::_InternalParse(const char *begin, const char *end, void *object,
                            ::google::protobuf::internal::ParseContext *ctx)
                    {
                        auto msg = static_cast<QueryStatsResponse *>(object);
                        ::google::protobuf::int32 size;
                        (void)size;
                        int depth;
                        (void)depth;
                        ::google::protobuf::uint32 tag;
                        ::google::protobuf::internal::ParseFunc parser_till_end;
                        (void)parser_till_end;
                        auto ptr = begin;

                        while (ptr < end) {
                            ptr = ::google::protobuf::io::Parse32(ptr, &tag);
                            GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);

                            switch (tag >> 3) {
                                // repeated .v2ray.core.app.stats.command.Stat stat = 1;
                                case 1: {
                                    if (static_cast<::google::protobuf::uint8>(tag) != 10) goto handle_unusual;

                                    do {
                                        ptr = ::google::protobuf::io::ReadSize(ptr, &size);
                                        GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
                                        parser_till_end = ::v2ray::core::app::stats::command::Stat::_InternalParse;
                                        object = msg->add_stat();

                                        if (size > end - ptr) goto len_delim_till_end;

                                        ptr += size;
                                        GOOGLE_PROTOBUF_PARSER_ASSERT(ctx->ParseExactRange(
                                        {parser_till_end, object}, ptr - size, ptr));

                                        if (ptr >= end) break;
                                    } while ((::google::protobuf::io::UnalignedLoad<::google::protobuf::uint64>(ptr) & 255) == 10 && (ptr += 1));

                                    break;
                                }

                                default: {
handle_unusual:

                                    if ((tag & 7) == 4 || tag == 0) {
                                        ctx->EndGroup(tag);
                                        return ptr;
                                    }

                                    auto res = UnknownFieldParse(tag, {_InternalParse, msg},
                                                                 ptr, end, msg->_internal_metadata_.mutable_unknown_fields(), ctx);
                                    ptr = res.first;
                                    GOOGLE_PROTOBUF_PARSER_ASSERT(ptr != nullptr);

                                    if (res.second) return ptr;
                                }
                            }  // switch
                        }  // while

                        return ptr;
len_delim_till_end:
                        return ctx->StoreAndTailCall(ptr, end, {_InternalParse, msg},
                        {parser_till_end, object}, size);
                    }
#else  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
                    bool QueryStatsResponse::MergePartialFromCodedStream(
                        ::google::protobuf::io::CodedInputStream *input)
                    {
#define DO_(EXPRESSION) if (!PROTOBUF_PREDICT_TRUE(EXPRESSION)) goto failure
                        ::google::protobuf::uint32 tag;

                        // @@protoc_insertion_point(parse_start:v2ray.core.app.stats.command.QueryStatsResponse)
                        for (;;) {
                            ::std::pair<::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
                            tag = p.first;

                            if (!p.second) goto handle_unusual;

                            switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
                                // repeated .v2ray.core.app.stats.command.Stat stat = 1;
                                case 1: {
                                    if (static_cast< ::google::protobuf::uint8>(tag) == (10 & 0xFF)) {
                                        DO_(::google::protobuf::internal::WireFormatLite::ReadMessage(
                                                input, add_stat()));
                                    } else {
                                        goto handle_unusual;
                                    }

                                    break;
                                }

                                default: {
handle_unusual:

                                    if (tag == 0) {
                                        goto success;
                                    }

                                    DO_(::google::protobuf::internal::WireFormat::SkipField(
                                            input, tag, _internal_metadata_.mutable_unknown_fields()));
                                    break;
                                }
                            }
                        }

success:
                        // @@protoc_insertion_point(parse_success:v2ray.core.app.stats.command.QueryStatsResponse)
                        return true;
failure:
                        // @@protoc_insertion_point(parse_failure:v2ray.core.app.stats.command.QueryStatsResponse)
                        return false;
#undef DO_
                    }
#endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER

                    void QueryStatsResponse::SerializeWithCachedSizes(
                        ::google::protobuf::io::CodedOutputStream *output) const
                    {
                        // @@protoc_insertion_point(serialize_start:v2ray.core.app.stats.command.QueryStatsResponse)
                        ::google::protobuf::uint32 cached_has_bits = 0;
                        (void) cached_has_bits;

                        // repeated .v2ray.core.app.stats.command.Stat stat = 1;
                        for (unsigned int i = 0,
                             n = static_cast<unsigned int>(this->stat_size()); i < n; i++) {
                            ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
                                1,
                                this->stat(static_cast<int>(i)),
                                output);
                        }

                        if (_internal_metadata_.have_unknown_fields()) {
                            ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
                                _internal_metadata_.unknown_fields(), output);
                        }

                        // @@protoc_insertion_point(serialize_end:v2ray.core.app.stats.command.QueryStatsResponse)
                    }

                    ::google::protobuf::uint8 *QueryStatsResponse::InternalSerializeWithCachedSizesToArray(
                        ::google::protobuf::uint8 *target) const
                    {
                        // @@protoc_insertion_point(serialize_to_array_start:v2ray.core.app.stats.command.QueryStatsResponse)
                        ::google::protobuf::uint32 cached_has_bits = 0;
                        (void) cached_has_bits;

                        // repeated .v2ray.core.app.stats.command.Stat stat = 1;
                        for (unsigned int i = 0,
                             n = static_cast<unsigned int>(this->stat_size()); i < n; i++) {
                            target = ::google::protobuf::internal::WireFormatLite::
                                     InternalWriteMessageToArray(
                                         1, this->stat(static_cast<int>(i)), target);
                        }

                        if (_internal_metadata_.have_unknown_fields()) {
                            target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
                                         _internal_metadata_.unknown_fields(), target);
                        }

                        // @@protoc_insertion_point(serialize_to_array_end:v2ray.core.app.stats.command.QueryStatsResponse)
                        return target;
                    }

                    size_t QueryStatsResponse::ByteSizeLong() const
                    {
                        // @@protoc_insertion_point(message_byte_size_start:v2ray.core.app.stats.command.QueryStatsResponse)
                        size_t total_size = 0;

                        if (_internal_metadata_.have_unknown_fields()) {
                            total_size +=
                                ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
                                    _internal_metadata_.unknown_fields());
                        }

                        ::google::protobuf::uint32 cached_has_bits = 0;
                        // Prevent compiler warnings about cached_has_bits being unused
                        (void) cached_has_bits;
                        // repeated .v2ray.core.app.stats.command.Stat stat = 1;
                        {
                            unsigned int count = static_cast<unsigned int>(this->stat_size());
                            total_size += 1UL * count;

                            for (unsigned int i = 0; i < count; i++) {
                                total_size +=
                                    ::google::protobuf::internal::WireFormatLite::MessageSize(
                                        this->stat(static_cast<int>(i)));
                            }
                        }
                        int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
                        SetCachedSize(cached_size);
                        return total_size;
                    }

                    void QueryStatsResponse::MergeFrom(const ::google::protobuf::Message &from)
                    {
                        // @@protoc_insertion_point(generalized_merge_from_start:v2ray.core.app.stats.command.QueryStatsResponse)
                        GOOGLE_DCHECK_NE(&from, this);
                        const QueryStatsResponse *source =
                            ::google::protobuf::DynamicCastToGenerated<QueryStatsResponse>(
                                &from);

                        if (source == nullptr) {
                            // @@protoc_insertion_point(generalized_merge_from_cast_fail:v2ray.core.app.stats.command.QueryStatsResponse)
                            ::google::protobuf::internal::ReflectionOps::Merge(from, this);
                        } else {
                            // @@protoc_insertion_point(generalized_merge_from_cast_success:v2ray.core.app.stats.command.QueryStatsResponse)
                            MergeFrom(*source);
                        }
                    }

                    void QueryStatsResponse::MergeFrom(const QueryStatsResponse &from)
                    {
                        // @@protoc_insertion_point(class_specific_merge_from_start:v2ray.core.app.stats.command.QueryStatsResponse)
                        GOOGLE_DCHECK_NE(&from, this);
                        _internal_metadata_.MergeFrom(from._internal_metadata_);
                        ::google::protobuf::uint32 cached_has_bits = 0;
                        (void) cached_has_bits;
                        stat_.MergeFrom(from.stat_);
                    }

                    void QueryStatsResponse::CopyFrom(const ::google::protobuf::Message &from)
                    {
                        // @@protoc_insertion_point(generalized_copy_from_start:v2ray.core.app.stats.command.QueryStatsResponse)
                        if (&from == this) return;

                        Clear();
                        MergeFrom(from);
                    }

                    void QueryStatsResponse::CopyFrom(const QueryStatsResponse &from)
                    {
                        // @@protoc_insertion_point(class_specific_copy_from_start:v2ray.core.app.stats.command.QueryStatsResponse)
                        if (&from == this) return;

                        Clear();
                        MergeFrom(from);
                    }

                    bool QueryStatsResponse::IsInitialized() const
                    {
                        return true;
                    }

                    void QueryStatsResponse::Swap(QueryStatsResponse *other)
                    {
                        if (other == this) return;

                        InternalSwap(other);
                    }
                    void QueryStatsResponse::InternalSwap(QueryStatsResponse *other)
                    {
                        using std::swap;
                        _internal_metadata_.Swap(&other->_internal_metadata_);
                        CastToBase(&stat_)->InternalSwap(CastToBase(&other->stat_));
                    }

                    ::google::protobuf::Metadata QueryStatsResponse::GetMetadata() const
                    {
                        ::google::protobuf::internal::AssignDescriptors(&::assign_descriptors_table_command_2eproto);
                        return ::file_level_metadata_command_2eproto[kIndexInFileMessages];
                    }


                    // ===================================================================

                    void SysStatsRequest::InitAsDefaultInstance()
                    {
                    }
                    class SysStatsRequest::HasBitSetters
                    {
                        public:
                    };

#if !defined(_MSC_VER) || _MSC_VER >= 1900
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

                    SysStatsRequest::SysStatsRequest()
                        : ::google::protobuf::Message(), _internal_metadata_(nullptr)
                    {
                        SharedCtor();
                        // @@protoc_insertion_point(constructor:v2ray.core.app.stats.command.SysStatsRequest)
                    }
                    SysStatsRequest::SysStatsRequest(const SysStatsRequest &from)
                        : ::google::protobuf::Message(),
                          _internal_metadata_(nullptr)
                    {
                        _internal_metadata_.MergeFrom(from._internal_metadata_);
                        // @@protoc_insertion_point(copy_constructor:v2ray.core.app.stats.command.SysStatsRequest)
                    }

                    void SysStatsRequest::SharedCtor()
                    {
                    }

                    SysStatsRequest::~SysStatsRequest()
                    {
                        // @@protoc_insertion_point(destructor:v2ray.core.app.stats.command.SysStatsRequest)
                        SharedDtor();
                    }

                    void SysStatsRequest::SharedDtor()
                    {
                    }

                    void SysStatsRequest::SetCachedSize(int size) const
                    {
                        _cached_size_.Set(size);
                    }
                    const SysStatsRequest &SysStatsRequest::default_instance()
                    {
                        ::google::protobuf::internal::InitSCC(&::scc_info_SysStatsRequest_command_2eproto.base);
                        return *internal_default_instance();
                    }


                    void SysStatsRequest::Clear()
                    {
                        // @@protoc_insertion_point(message_clear_start:v2ray.core.app.stats.command.SysStatsRequest)
                        ::google::protobuf::uint32 cached_has_bits = 0;
                        // Prevent compiler warnings about cached_has_bits being unused
                        (void) cached_has_bits;
                        _internal_metadata_.Clear();
                    }

#if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
                    const char *SysStatsRequest::_InternalParse(const char *begin, const char *end, void *object,
                            ::google::protobuf::internal::ParseContext *ctx)
                    {
                        auto msg = static_cast<SysStatsRequest *>(object);
                        ::google::protobuf::int32 size;
                        (void)size;
                        int depth;
                        (void)depth;
                        ::google::protobuf::uint32 tag;
                        ::google::protobuf::internal::ParseFunc parser_till_end;
                        (void)parser_till_end;
                        auto ptr = begin;

                        while (ptr < end) {
                            ptr = ::google::protobuf::io::Parse32(ptr, &tag);
                            GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);

                            switch (tag >> 3) {
                                default: {
                                    if ((tag & 7) == 4 || tag == 0) {
                                        ctx->EndGroup(tag);
                                        return ptr;
                                    }

                                    auto res = UnknownFieldParse(tag, {_InternalParse, msg},
                                                                 ptr, end, msg->_internal_metadata_.mutable_unknown_fields(), ctx);
                                    ptr = res.first;
                                    GOOGLE_PROTOBUF_PARSER_ASSERT(ptr != nullptr);

                                    if (res.second) return ptr;
                                }
                            }  // switch
                        }  // while

                        return ptr;
                    }
#else  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
                    bool SysStatsRequest::MergePartialFromCodedStream(
                        ::google::protobuf::io::CodedInputStream *input)
                    {
#define DO_(EXPRESSION) if (!PROTOBUF_PREDICT_TRUE(EXPRESSION)) goto failure
                        ::google::protobuf::uint32 tag;

                        // @@protoc_insertion_point(parse_start:v2ray.core.app.stats.command.SysStatsRequest)
                        for (;;) {
                            ::std::pair<::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
                            tag = p.first;

                            if (!p.second) goto handle_unusual;

handle_unusual:

                            if (tag == 0) {
                                goto success;
                            }

                            DO_(::google::protobuf::internal::WireFormat::SkipField(
                                    input, tag, _internal_metadata_.mutable_unknown_fields()));
                        }

success:
                        // @@protoc_insertion_point(parse_success:v2ray.core.app.stats.command.SysStatsRequest)
                        return true;
failure:
                        // @@protoc_insertion_point(parse_failure:v2ray.core.app.stats.command.SysStatsRequest)
                        return false;
#undef DO_
                    }
#endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER

                    void SysStatsRequest::SerializeWithCachedSizes(
                        ::google::protobuf::io::CodedOutputStream *output) const
                    {
                        // @@protoc_insertion_point(serialize_start:v2ray.core.app.stats.command.SysStatsRequest)
                        ::google::protobuf::uint32 cached_has_bits = 0;
                        (void) cached_has_bits;

                        if (_internal_metadata_.have_unknown_fields()) {
                            ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
                                _internal_metadata_.unknown_fields(), output);
                        }

                        // @@protoc_insertion_point(serialize_end:v2ray.core.app.stats.command.SysStatsRequest)
                    }

                    ::google::protobuf::uint8 *SysStatsRequest::InternalSerializeWithCachedSizesToArray(
                        ::google::protobuf::uint8 *target) const
                    {
                        // @@protoc_insertion_point(serialize_to_array_start:v2ray.core.app.stats.command.SysStatsRequest)
                        ::google::protobuf::uint32 cached_has_bits = 0;
                        (void) cached_has_bits;

                        if (_internal_metadata_.have_unknown_fields()) {
                            target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
                                         _internal_metadata_.unknown_fields(), target);
                        }

                        // @@protoc_insertion_point(serialize_to_array_end:v2ray.core.app.stats.command.SysStatsRequest)
                        return target;
                    }

                    size_t SysStatsRequest::ByteSizeLong() const
                    {
                        // @@protoc_insertion_point(message_byte_size_start:v2ray.core.app.stats.command.SysStatsRequest)
                        size_t total_size = 0;

                        if (_internal_metadata_.have_unknown_fields()) {
                            total_size +=
                                ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
                                    _internal_metadata_.unknown_fields());
                        }

                        ::google::protobuf::uint32 cached_has_bits = 0;
                        // Prevent compiler warnings about cached_has_bits being unused
                        (void) cached_has_bits;
                        int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
                        SetCachedSize(cached_size);
                        return total_size;
                    }

                    void SysStatsRequest::MergeFrom(const ::google::protobuf::Message &from)
                    {
                        // @@protoc_insertion_point(generalized_merge_from_start:v2ray.core.app.stats.command.SysStatsRequest)
                        GOOGLE_DCHECK_NE(&from, this);
                        const SysStatsRequest *source =
                            ::google::protobuf::DynamicCastToGenerated<SysStatsRequest>(
                                &from);

                        if (source == nullptr) {
                            // @@protoc_insertion_point(generalized_merge_from_cast_fail:v2ray.core.app.stats.command.SysStatsRequest)
                            ::google::protobuf::internal::ReflectionOps::Merge(from, this);
                        } else {
                            // @@protoc_insertion_point(generalized_merge_from_cast_success:v2ray.core.app.stats.command.SysStatsRequest)
                            MergeFrom(*source);
                        }
                    }

                    void SysStatsRequest::MergeFrom(const SysStatsRequest &from)
                    {
                        // @@protoc_insertion_point(class_specific_merge_from_start:v2ray.core.app.stats.command.SysStatsRequest)
                        GOOGLE_DCHECK_NE(&from, this);
                        _internal_metadata_.MergeFrom(from._internal_metadata_);
                        ::google::protobuf::uint32 cached_has_bits = 0;
                        (void) cached_has_bits;
                    }

                    void SysStatsRequest::CopyFrom(const ::google::protobuf::Message &from)
                    {
                        // @@protoc_insertion_point(generalized_copy_from_start:v2ray.core.app.stats.command.SysStatsRequest)
                        if (&from == this) return;

                        Clear();
                        MergeFrom(from);
                    }

                    void SysStatsRequest::CopyFrom(const SysStatsRequest &from)
                    {
                        // @@protoc_insertion_point(class_specific_copy_from_start:v2ray.core.app.stats.command.SysStatsRequest)
                        if (&from == this) return;

                        Clear();
                        MergeFrom(from);
                    }

                    bool SysStatsRequest::IsInitialized() const
                    {
                        return true;
                    }

                    void SysStatsRequest::Swap(SysStatsRequest *other)
                    {
                        if (other == this) return;

                        InternalSwap(other);
                    }
                    void SysStatsRequest::InternalSwap(SysStatsRequest *other)
                    {
                        using std::swap;
                        _internal_metadata_.Swap(&other->_internal_metadata_);
                    }

                    ::google::protobuf::Metadata SysStatsRequest::GetMetadata() const
                    {
                        ::google::protobuf::internal::AssignDescriptors(&::assign_descriptors_table_command_2eproto);
                        return ::file_level_metadata_command_2eproto[kIndexInFileMessages];
                    }


                    // ===================================================================

                    void SysStatsResponse::InitAsDefaultInstance()
                    {
                    }
                    class SysStatsResponse::HasBitSetters
                    {
                        public:
                    };

#if !defined(_MSC_VER) || _MSC_VER >= 1900
                    const int SysStatsResponse::kNumGoroutineFieldNumber;
                    const int SysStatsResponse::kNumGCFieldNumber;
                    const int SysStatsResponse::kAllocFieldNumber;
                    const int SysStatsResponse::kTotalAllocFieldNumber;
                    const int SysStatsResponse::kSysFieldNumber;
                    const int SysStatsResponse::kMallocsFieldNumber;
                    const int SysStatsResponse::kFreesFieldNumber;
                    const int SysStatsResponse::kLiveObjectsFieldNumber;
                    const int SysStatsResponse::kPauseTotalNsFieldNumber;
                    const int SysStatsResponse::kUptimeFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

                    SysStatsResponse::SysStatsResponse()
                        : ::google::protobuf::Message(), _internal_metadata_(nullptr)
                    {
                        SharedCtor();
                        // @@protoc_insertion_point(constructor:v2ray.core.app.stats.command.SysStatsResponse)
                    }
                    SysStatsResponse::SysStatsResponse(const SysStatsResponse &from)
                        : ::google::protobuf::Message(),
                          _internal_metadata_(nullptr)
                    {
                        _internal_metadata_.MergeFrom(from._internal_metadata_);
                        ::memcpy(&numgoroutine_, &from.numgoroutine_,
                                 static_cast<size_t>(reinterpret_cast<char *>(&uptime_) -
                                                     reinterpret_cast<char *>(&numgoroutine_)) + sizeof(uptime_));
                        // @@protoc_insertion_point(copy_constructor:v2ray.core.app.stats.command.SysStatsResponse)
                    }

                    void SysStatsResponse::SharedCtor()
                    {
                        ::memset(&numgoroutine_, 0, static_cast<size_t>(
                                     reinterpret_cast<char *>(&uptime_) -
                                     reinterpret_cast<char *>(&numgoroutine_)) + sizeof(uptime_));
                    }

                    SysStatsResponse::~SysStatsResponse()
                    {
                        // @@protoc_insertion_point(destructor:v2ray.core.app.stats.command.SysStatsResponse)
                        SharedDtor();
                    }

                    void SysStatsResponse::SharedDtor()
                    {
                    }

                    void SysStatsResponse::SetCachedSize(int size) const
                    {
                        _cached_size_.Set(size);
                    }
                    const SysStatsResponse &SysStatsResponse::default_instance()
                    {
                        ::google::protobuf::internal::InitSCC(&::scc_info_SysStatsResponse_command_2eproto.base);
                        return *internal_default_instance();
                    }


                    void SysStatsResponse::Clear()
                    {
                        // @@protoc_insertion_point(message_clear_start:v2ray.core.app.stats.command.SysStatsResponse)
                        ::google::protobuf::uint32 cached_has_bits = 0;
                        // Prevent compiler warnings about cached_has_bits being unused
                        (void) cached_has_bits;
                        ::memset(&numgoroutine_, 0, static_cast<size_t>(
                                     reinterpret_cast<char *>(&uptime_) -
                                     reinterpret_cast<char *>(&numgoroutine_)) + sizeof(uptime_));
                        _internal_metadata_.Clear();
                    }

#if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
                    const char *SysStatsResponse::_InternalParse(const char *begin, const char *end, void *object,
                            ::google::protobuf::internal::ParseContext *ctx)
                    {
                        auto msg = static_cast<SysStatsResponse *>(object);
                        ::google::protobuf::int32 size;
                        (void)size;
                        int depth;
                        (void)depth;
                        ::google::protobuf::uint32 tag;
                        ::google::protobuf::internal::ParseFunc parser_till_end;
                        (void)parser_till_end;
                        auto ptr = begin;

                        while (ptr < end) {
                            ptr = ::google::protobuf::io::Parse32(ptr, &tag);
                            GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);

                            switch (tag >> 3) {
                                // uint32 NumGoroutine = 1;
                                case 1: {
                                    if (static_cast<::google::protobuf::uint8>(tag) != 8) goto handle_unusual;

                                    msg->set_numgoroutine(::google::protobuf::internal::ReadVarint(&ptr));
                                    GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
                                    break;
                                }

                                // uint32 NumGC = 2;
                                case 2: {
                                    if (static_cast<::google::protobuf::uint8>(tag) != 16) goto handle_unusual;

                                    msg->set_numgc(::google::protobuf::internal::ReadVarint(&ptr));
                                    GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
                                    break;
                                }

                                // uint64 Alloc = 3;
                                case 3: {
                                    if (static_cast<::google::protobuf::uint8>(tag) != 24) goto handle_unusual;

                                    msg->set_alloc(::google::protobuf::internal::ReadVarint(&ptr));
                                    GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
                                    break;
                                }

                                // uint64 TotalAlloc = 4;
                                case 4: {
                                    if (static_cast<::google::protobuf::uint8>(tag) != 32) goto handle_unusual;

                                    msg->set_totalalloc(::google::protobuf::internal::ReadVarint(&ptr));
                                    GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
                                    break;
                                }

                                // uint64 Sys = 5;
                                case 5: {
                                    if (static_cast<::google::protobuf::uint8>(tag) != 40) goto handle_unusual;

                                    msg->set_sys(::google::protobuf::internal::ReadVarint(&ptr));
                                    GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
                                    break;
                                }

                                // uint64 Mallocs = 6;
                                case 6: {
                                    if (static_cast<::google::protobuf::uint8>(tag) != 48) goto handle_unusual;

                                    msg->set_mallocs(::google::protobuf::internal::ReadVarint(&ptr));
                                    GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
                                    break;
                                }

                                // uint64 Frees = 7;
                                case 7: {
                                    if (static_cast<::google::protobuf::uint8>(tag) != 56) goto handle_unusual;

                                    msg->set_frees(::google::protobuf::internal::ReadVarint(&ptr));
                                    GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
                                    break;
                                }

                                // uint64 LiveObjects = 8;
                                case 8: {
                                    if (static_cast<::google::protobuf::uint8>(tag) != 64) goto handle_unusual;

                                    msg->set_liveobjects(::google::protobuf::internal::ReadVarint(&ptr));
                                    GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
                                    break;
                                }

                                // uint64 PauseTotalNs = 9;
                                case 9: {
                                    if (static_cast<::google::protobuf::uint8>(tag) != 72) goto handle_unusual;

                                    msg->set_pausetotalns(::google::protobuf::internal::ReadVarint(&ptr));
                                    GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
                                    break;
                                }

                                // uint32 Uptime = 10;
                                case 10: {
                                    if (static_cast<::google::protobuf::uint8>(tag) != 80) goto handle_unusual;

                                    msg->set_uptime(::google::protobuf::internal::ReadVarint(&ptr));
                                    GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
                                    break;
                                }

                                default: {
handle_unusual:

                                    if ((tag & 7) == 4 || tag == 0) {
                                        ctx->EndGroup(tag);
                                        return ptr;
                                    }

                                    auto res = UnknownFieldParse(tag, {_InternalParse, msg},
                                                                 ptr, end, msg->_internal_metadata_.mutable_unknown_fields(), ctx);
                                    ptr = res.first;
                                    GOOGLE_PROTOBUF_PARSER_ASSERT(ptr != nullptr);

                                    if (res.second) return ptr;
                                }
                            }  // switch
                        }  // while

                        return ptr;
                    }
#else  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
                    bool SysStatsResponse::MergePartialFromCodedStream(
                        ::google::protobuf::io::CodedInputStream *input)
                    {
#define DO_(EXPRESSION) if (!PROTOBUF_PREDICT_TRUE(EXPRESSION)) goto failure
                        ::google::protobuf::uint32 tag;

                        // @@protoc_insertion_point(parse_start:v2ray.core.app.stats.command.SysStatsResponse)
                        for (;;) {
                            ::std::pair<::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
                            tag = p.first;

                            if (!p.second) goto handle_unusual;

                            switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
                                // uint32 NumGoroutine = 1;
                                case 1: {
                                    if (static_cast< ::google::protobuf::uint8>(tag) == (8 & 0xFF)) {
                                        DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive <
                                             ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32 > (
                                                 input, &numgoroutine_)));
                                    } else {
                                        goto handle_unusual;
                                    }

                                    break;
                                }

                                // uint32 NumGC = 2;
                                case 2: {
                                    if (static_cast< ::google::protobuf::uint8>(tag) == (16 & 0xFF)) {
                                        DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive <
                                             ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32 > (
                                                 input, &numgc_)));
                                    } else {
                                        goto handle_unusual;
                                    }

                                    break;
                                }

                                // uint64 Alloc = 3;
                                case 3: {
                                    if (static_cast< ::google::protobuf::uint8>(tag) == (24 & 0xFF)) {
                                        DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive <
                                             ::google::protobuf::uint64, ::google::protobuf::internal::WireFormatLite::TYPE_UINT64 > (
                                                 input, &alloc_)));
                                    } else {
                                        goto handle_unusual;
                                    }

                                    break;
                                }

                                // uint64 TotalAlloc = 4;
                                case 4: {
                                    if (static_cast< ::google::protobuf::uint8>(tag) == (32 & 0xFF)) {
                                        DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive <
                                             ::google::protobuf::uint64, ::google::protobuf::internal::WireFormatLite::TYPE_UINT64 > (
                                                 input, &totalalloc_)));
                                    } else {
                                        goto handle_unusual;
                                    }

                                    break;
                                }

                                // uint64 Sys = 5;
                                case 5: {
                                    if (static_cast< ::google::protobuf::uint8>(tag) == (40 & 0xFF)) {
                                        DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive <
                                             ::google::protobuf::uint64, ::google::protobuf::internal::WireFormatLite::TYPE_UINT64 > (
                                                 input, &sys_)));
                                    } else {
                                        goto handle_unusual;
                                    }

                                    break;
                                }

                                // uint64 Mallocs = 6;
                                case 6: {
                                    if (static_cast< ::google::protobuf::uint8>(tag) == (48 & 0xFF)) {
                                        DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive <
                                             ::google::protobuf::uint64, ::google::protobuf::internal::WireFormatLite::TYPE_UINT64 > (
                                                 input, &mallocs_)));
                                    } else {
                                        goto handle_unusual;
                                    }

                                    break;
                                }

                                // uint64 Frees = 7;
                                case 7: {
                                    if (static_cast< ::google::protobuf::uint8>(tag) == (56 & 0xFF)) {
                                        DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive <
                                             ::google::protobuf::uint64, ::google::protobuf::internal::WireFormatLite::TYPE_UINT64 > (
                                                 input, &frees_)));
                                    } else {
                                        goto handle_unusual;
                                    }

                                    break;
                                }

                                // uint64 LiveObjects = 8;
                                case 8: {
                                    if (static_cast< ::google::protobuf::uint8>(tag) == (64 & 0xFF)) {
                                        DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive <
                                             ::google::protobuf::uint64, ::google::protobuf::internal::WireFormatLite::TYPE_UINT64 > (
                                                 input, &liveobjects_)));
                                    } else {
                                        goto handle_unusual;
                                    }

                                    break;
                                }

                                // uint64 PauseTotalNs = 9;
                                case 9: {
                                    if (static_cast< ::google::protobuf::uint8>(tag) == (72 & 0xFF)) {
                                        DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive <
                                             ::google::protobuf::uint64, ::google::protobuf::internal::WireFormatLite::TYPE_UINT64 > (
                                                 input, &pausetotalns_)));
                                    } else {
                                        goto handle_unusual;
                                    }

                                    break;
                                }

                                // uint32 Uptime = 10;
                                case 10: {
                                    if (static_cast< ::google::protobuf::uint8>(tag) == (80 & 0xFF)) {
                                        DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive <
                                             ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32 > (
                                                 input, &uptime_)));
                                    } else {
                                        goto handle_unusual;
                                    }

                                    break;
                                }

                                default: {
handle_unusual:

                                    if (tag == 0) {
                                        goto success;
                                    }

                                    DO_(::google::protobuf::internal::WireFormat::SkipField(
                                            input, tag, _internal_metadata_.mutable_unknown_fields()));
                                    break;
                                }
                            }
                        }

success:
                        // @@protoc_insertion_point(parse_success:v2ray.core.app.stats.command.SysStatsResponse)
                        return true;
failure:
                        // @@protoc_insertion_point(parse_failure:v2ray.core.app.stats.command.SysStatsResponse)
                        return false;
#undef DO_
                    }
#endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER

                    void SysStatsResponse::SerializeWithCachedSizes(
                        ::google::protobuf::io::CodedOutputStream *output) const
                    {
                        // @@protoc_insertion_point(serialize_start:v2ray.core.app.stats.command.SysStatsResponse)
                        ::google::protobuf::uint32 cached_has_bits = 0;
                        (void) cached_has_bits;

                        // uint32 NumGoroutine = 1;
                        if (this->numgoroutine() != 0) {
                            ::google::protobuf::internal::WireFormatLite::WriteUInt32(1, this->numgoroutine(), output);
                        }

                        // uint32 NumGC = 2;
                        if (this->numgc() != 0) {
                            ::google::protobuf::internal::WireFormatLite::WriteUInt32(2, this->numgc(), output);
                        }

                        // uint64 Alloc = 3;
                        if (this->alloc() != 0) {
                            ::google::protobuf::internal::WireFormatLite::WriteUInt64(3, this->alloc(), output);
                        }

                        // uint64 TotalAlloc = 4;
                        if (this->totalalloc() != 0) {
                            ::google::protobuf::internal::WireFormatLite::WriteUInt64(4, this->totalalloc(), output);
                        }

                        // uint64 Sys = 5;
                        if (this->sys() != 0) {
                            ::google::protobuf::internal::WireFormatLite::WriteUInt64(5, this->sys(), output);
                        }

                        // uint64 Mallocs = 6;
                        if (this->mallocs() != 0) {
                            ::google::protobuf::internal::WireFormatLite::WriteUInt64(6, this->mallocs(), output);
                        }

                        // uint64 Frees = 7;
                        if (this->frees() != 0) {
                            ::google::protobuf::internal::WireFormatLite::WriteUInt64(7, this->frees(), output);
                        }

                        // uint64 LiveObjects = 8;
                        if (this->liveobjects() != 0) {
                            ::google::protobuf::internal::WireFormatLite::WriteUInt64(8, this->liveobjects(), output);
                        }

                        // uint64 PauseTotalNs = 9;
                        if (this->pausetotalns() != 0) {
                            ::google::protobuf::internal::WireFormatLite::WriteUInt64(9, this->pausetotalns(), output);
                        }

                        // uint32 Uptime = 10;
                        if (this->uptime() != 0) {
                            ::google::protobuf::internal::WireFormatLite::WriteUInt32(10, this->uptime(), output);
                        }

                        if (_internal_metadata_.have_unknown_fields()) {
                            ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
                                _internal_metadata_.unknown_fields(), output);
                        }

                        // @@protoc_insertion_point(serialize_end:v2ray.core.app.stats.command.SysStatsResponse)
                    }

                    ::google::protobuf::uint8 *SysStatsResponse::InternalSerializeWithCachedSizesToArray(
                        ::google::protobuf::uint8 *target) const
                    {
                        // @@protoc_insertion_point(serialize_to_array_start:v2ray.core.app.stats.command.SysStatsResponse)
                        ::google::protobuf::uint32 cached_has_bits = 0;
                        (void) cached_has_bits;

                        // uint32 NumGoroutine = 1;
                        if (this->numgoroutine() != 0) {
                            target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(1, this->numgoroutine(), target);
                        }

                        // uint32 NumGC = 2;
                        if (this->numgc() != 0) {
                            target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(2, this->numgc(), target);
                        }

                        // uint64 Alloc = 3;
                        if (this->alloc() != 0) {
                            target = ::google::protobuf::internal::WireFormatLite::WriteUInt64ToArray(3, this->alloc(), target);
                        }

                        // uint64 TotalAlloc = 4;
                        if (this->totalalloc() != 0) {
                            target = ::google::protobuf::internal::WireFormatLite::WriteUInt64ToArray(4, this->totalalloc(), target);
                        }

                        // uint64 Sys = 5;
                        if (this->sys() != 0) {
                            target = ::google::protobuf::internal::WireFormatLite::WriteUInt64ToArray(5, this->sys(), target);
                        }

                        // uint64 Mallocs = 6;
                        if (this->mallocs() != 0) {
                            target = ::google::protobuf::internal::WireFormatLite::WriteUInt64ToArray(6, this->mallocs(), target);
                        }

                        // uint64 Frees = 7;
                        if (this->frees() != 0) {
                            target = ::google::protobuf::internal::WireFormatLite::WriteUInt64ToArray(7, this->frees(), target);
                        }

                        // uint64 LiveObjects = 8;
                        if (this->liveobjects() != 0) {
                            target = ::google::protobuf::internal::WireFormatLite::WriteUInt64ToArray(8, this->liveobjects(), target);
                        }

                        // uint64 PauseTotalNs = 9;
                        if (this->pausetotalns() != 0) {
                            target = ::google::protobuf::internal::WireFormatLite::WriteUInt64ToArray(9, this->pausetotalns(), target);
                        }

                        // uint32 Uptime = 10;
                        if (this->uptime() != 0) {
                            target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(10, this->uptime(), target);
                        }

                        if (_internal_metadata_.have_unknown_fields()) {
                            target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
                                         _internal_metadata_.unknown_fields(), target);
                        }

                        // @@protoc_insertion_point(serialize_to_array_end:v2ray.core.app.stats.command.SysStatsResponse)
                        return target;
                    }

                    size_t SysStatsResponse::ByteSizeLong() const
                    {
                        // @@protoc_insertion_point(message_byte_size_start:v2ray.core.app.stats.command.SysStatsResponse)
                        size_t total_size = 0;

                        if (_internal_metadata_.have_unknown_fields()) {
                            total_size +=
                                ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
                                    _internal_metadata_.unknown_fields());
                        }

                        ::google::protobuf::uint32 cached_has_bits = 0;
                        // Prevent compiler warnings about cached_has_bits being unused
                        (void) cached_has_bits;

                        // uint32 NumGoroutine = 1;
                        if (this->numgoroutine() != 0) {
                            total_size += 1 +
                                          ::google::protobuf::internal::WireFormatLite::UInt32Size(
                                              this->numgoroutine());
                        }

                        // uint32 NumGC = 2;
                        if (this->numgc() != 0) {
                            total_size += 1 +
                                          ::google::protobuf::internal::WireFormatLite::UInt32Size(
                                              this->numgc());
                        }

                        // uint64 Alloc = 3;
                        if (this->alloc() != 0) {
                            total_size += 1 +
                                          ::google::protobuf::internal::WireFormatLite::UInt64Size(
                                              this->alloc());
                        }

                        // uint64 TotalAlloc = 4;
                        if (this->totalalloc() != 0) {
                            total_size += 1 +
                                          ::google::protobuf::internal::WireFormatLite::UInt64Size(
                                              this->totalalloc());
                        }

                        // uint64 Sys = 5;
                        if (this->sys() != 0) {
                            total_size += 1 +
                                          ::google::protobuf::internal::WireFormatLite::UInt64Size(
                                              this->sys());
                        }

                        // uint64 Mallocs = 6;
                        if (this->mallocs() != 0) {
                            total_size += 1 +
                                          ::google::protobuf::internal::WireFormatLite::UInt64Size(
                                              this->mallocs());
                        }

                        // uint64 Frees = 7;
                        if (this->frees() != 0) {
                            total_size += 1 +
                                          ::google::protobuf::internal::WireFormatLite::UInt64Size(
                                              this->frees());
                        }

                        // uint64 LiveObjects = 8;
                        if (this->liveobjects() != 0) {
                            total_size += 1 +
                                          ::google::protobuf::internal::WireFormatLite::UInt64Size(
                                              this->liveobjects());
                        }

                        // uint64 PauseTotalNs = 9;
                        if (this->pausetotalns() != 0) {
                            total_size += 1 +
                                          ::google::protobuf::internal::WireFormatLite::UInt64Size(
                                              this->pausetotalns());
                        }

                        // uint32 Uptime = 10;
                        if (this->uptime() != 0) {
                            total_size += 1 +
                                          ::google::protobuf::internal::WireFormatLite::UInt32Size(
                                              this->uptime());
                        }

                        int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
                        SetCachedSize(cached_size);
                        return total_size;
                    }

                    void SysStatsResponse::MergeFrom(const ::google::protobuf::Message &from)
                    {
                        // @@protoc_insertion_point(generalized_merge_from_start:v2ray.core.app.stats.command.SysStatsResponse)
                        GOOGLE_DCHECK_NE(&from, this);
                        const SysStatsResponse *source =
                            ::google::protobuf::DynamicCastToGenerated<SysStatsResponse>(
                                &from);

                        if (source == nullptr) {
                            // @@protoc_insertion_point(generalized_merge_from_cast_fail:v2ray.core.app.stats.command.SysStatsResponse)
                            ::google::protobuf::internal::ReflectionOps::Merge(from, this);
                        } else {
                            // @@protoc_insertion_point(generalized_merge_from_cast_success:v2ray.core.app.stats.command.SysStatsResponse)
                            MergeFrom(*source);
                        }
                    }

                    void SysStatsResponse::MergeFrom(const SysStatsResponse &from)
                    {
                        // @@protoc_insertion_point(class_specific_merge_from_start:v2ray.core.app.stats.command.SysStatsResponse)
                        GOOGLE_DCHECK_NE(&from, this);
                        _internal_metadata_.MergeFrom(from._internal_metadata_);
                        ::google::protobuf::uint32 cached_has_bits = 0;
                        (void) cached_has_bits;

                        if (from.numgoroutine() != 0) {
                            set_numgoroutine(from.numgoroutine());
                        }

                        if (from.numgc() != 0) {
                            set_numgc(from.numgc());
                        }

                        if (from.alloc() != 0) {
                            set_alloc(from.alloc());
                        }

                        if (from.totalalloc() != 0) {
                            set_totalalloc(from.totalalloc());
                        }

                        if (from.sys() != 0) {
                            set_sys(from.sys());
                        }

                        if (from.mallocs() != 0) {
                            set_mallocs(from.mallocs());
                        }

                        if (from.frees() != 0) {
                            set_frees(from.frees());
                        }

                        if (from.liveobjects() != 0) {
                            set_liveobjects(from.liveobjects());
                        }

                        if (from.pausetotalns() != 0) {
                            set_pausetotalns(from.pausetotalns());
                        }

                        if (from.uptime() != 0) {
                            set_uptime(from.uptime());
                        }
                    }

                    void SysStatsResponse::CopyFrom(const ::google::protobuf::Message &from)
                    {
                        // @@protoc_insertion_point(generalized_copy_from_start:v2ray.core.app.stats.command.SysStatsResponse)
                        if (&from == this) return;

                        Clear();
                        MergeFrom(from);
                    }

                    void SysStatsResponse::CopyFrom(const SysStatsResponse &from)
                    {
                        // @@protoc_insertion_point(class_specific_copy_from_start:v2ray.core.app.stats.command.SysStatsResponse)
                        if (&from == this) return;

                        Clear();
                        MergeFrom(from);
                    }

                    bool SysStatsResponse::IsInitialized() const
                    {
                        return true;
                    }

                    void SysStatsResponse::Swap(SysStatsResponse *other)
                    {
                        if (other == this) return;

                        InternalSwap(other);
                    }
                    void SysStatsResponse::InternalSwap(SysStatsResponse *other)
                    {
                        using std::swap;
                        _internal_metadata_.Swap(&other->_internal_metadata_);
                        swap(numgoroutine_, other->numgoroutine_);
                        swap(numgc_, other->numgc_);
                        swap(alloc_, other->alloc_);
                        swap(totalalloc_, other->totalalloc_);
                        swap(sys_, other->sys_);
                        swap(mallocs_, other->mallocs_);
                        swap(frees_, other->frees_);
                        swap(liveobjects_, other->liveobjects_);
                        swap(pausetotalns_, other->pausetotalns_);
                        swap(uptime_, other->uptime_);
                    }

                    ::google::protobuf::Metadata SysStatsResponse::GetMetadata() const
                    {
                        ::google::protobuf::internal::AssignDescriptors(&::assign_descriptors_table_command_2eproto);
                        return ::file_level_metadata_command_2eproto[kIndexInFileMessages];
                    }


                    // ===================================================================

                    void Config::InitAsDefaultInstance()
                    {
                    }
                    class Config::HasBitSetters
                    {
                        public:
                    };

#if !defined(_MSC_VER) || _MSC_VER >= 1900
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

                    Config::Config()
                        : ::google::protobuf::Message(), _internal_metadata_(nullptr)
                    {
                        SharedCtor();
                        // @@protoc_insertion_point(constructor:v2ray.core.app.stats.command.Config)
                    }
                    Config::Config(const Config &from)
                        : ::google::protobuf::Message(),
                          _internal_metadata_(nullptr)
                    {
                        _internal_metadata_.MergeFrom(from._internal_metadata_);
                        // @@protoc_insertion_point(copy_constructor:v2ray.core.app.stats.command.Config)
                    }

                    void Config::SharedCtor()
                    {
                    }

                    Config::~Config()
                    {
                        // @@protoc_insertion_point(destructor:v2ray.core.app.stats.command.Config)
                        SharedDtor();
                    }

                    void Config::SharedDtor()
                    {
                    }

                    void Config::SetCachedSize(int size) const
                    {
                        _cached_size_.Set(size);
                    }
                    const Config &Config::default_instance()
                    {
                        ::google::protobuf::internal::InitSCC(&::scc_info_Config_command_2eproto.base);
                        return *internal_default_instance();
                    }


                    void Config::Clear()
                    {
                        // @@protoc_insertion_point(message_clear_start:v2ray.core.app.stats.command.Config)
                        ::google::protobuf::uint32 cached_has_bits = 0;
                        // Prevent compiler warnings about cached_has_bits being unused
                        (void) cached_has_bits;
                        _internal_metadata_.Clear();
                    }

#if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
                    const char *Config::_InternalParse(const char *begin, const char *end, void *object,
                                                       ::google::protobuf::internal::ParseContext *ctx)
                    {
                        auto msg = static_cast<Config *>(object);
                        ::google::protobuf::int32 size;
                        (void)size;
                        int depth;
                        (void)depth;
                        ::google::protobuf::uint32 tag;
                        ::google::protobuf::internal::ParseFunc parser_till_end;
                        (void)parser_till_end;
                        auto ptr = begin;

                        while (ptr < end) {
                            ptr = ::google::protobuf::io::Parse32(ptr, &tag);
                            GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);

                            switch (tag >> 3) {
                                default: {
                                    if ((tag & 7) == 4 || tag == 0) {
                                        ctx->EndGroup(tag);
                                        return ptr;
                                    }

                                    auto res = UnknownFieldParse(tag, {_InternalParse, msg},
                                                                 ptr, end, msg->_internal_metadata_.mutable_unknown_fields(), ctx);
                                    ptr = res.first;
                                    GOOGLE_PROTOBUF_PARSER_ASSERT(ptr != nullptr);

                                    if (res.second) return ptr;
                                }
                            }  // switch
                        }  // while

                        return ptr;
                    }
#else  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
                    bool Config::MergePartialFromCodedStream(
                        ::google::protobuf::io::CodedInputStream *input)
                    {
#define DO_(EXPRESSION) if (!PROTOBUF_PREDICT_TRUE(EXPRESSION)) goto failure
                        ::google::protobuf::uint32 tag;

                        // @@protoc_insertion_point(parse_start:v2ray.core.app.stats.command.Config)
                        for (;;) {
                            ::std::pair<::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
                            tag = p.first;

                            if (!p.second) goto handle_unusual;

handle_unusual:

                            if (tag == 0) {
                                goto success;
                            }

                            DO_(::google::protobuf::internal::WireFormat::SkipField(
                                    input, tag, _internal_metadata_.mutable_unknown_fields()));
                        }

success:
                        // @@protoc_insertion_point(parse_success:v2ray.core.app.stats.command.Config)
                        return true;
failure:
                        // @@protoc_insertion_point(parse_failure:v2ray.core.app.stats.command.Config)
                        return false;
#undef DO_
                    }
#endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER

                    void Config::SerializeWithCachedSizes(
                        ::google::protobuf::io::CodedOutputStream *output) const
                    {
                        // @@protoc_insertion_point(serialize_start:v2ray.core.app.stats.command.Config)
                        ::google::protobuf::uint32 cached_has_bits = 0;
                        (void) cached_has_bits;

                        if (_internal_metadata_.have_unknown_fields()) {
                            ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
                                _internal_metadata_.unknown_fields(), output);
                        }

                        // @@protoc_insertion_point(serialize_end:v2ray.core.app.stats.command.Config)
                    }

                    ::google::protobuf::uint8 *Config::InternalSerializeWithCachedSizesToArray(
                        ::google::protobuf::uint8 *target) const
                    {
                        // @@protoc_insertion_point(serialize_to_array_start:v2ray.core.app.stats.command.Config)
                        ::google::protobuf::uint32 cached_has_bits = 0;
                        (void) cached_has_bits;

                        if (_internal_metadata_.have_unknown_fields()) {
                            target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
                                         _internal_metadata_.unknown_fields(), target);
                        }

                        // @@protoc_insertion_point(serialize_to_array_end:v2ray.core.app.stats.command.Config)
                        return target;
                    }

                    size_t Config::ByteSizeLong() const
                    {
                        // @@protoc_insertion_point(message_byte_size_start:v2ray.core.app.stats.command.Config)
                        size_t total_size = 0;

                        if (_internal_metadata_.have_unknown_fields()) {
                            total_size +=
                                ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
                                    _internal_metadata_.unknown_fields());
                        }

                        ::google::protobuf::uint32 cached_has_bits = 0;
                        // Prevent compiler warnings about cached_has_bits being unused
                        (void) cached_has_bits;
                        int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
                        SetCachedSize(cached_size);
                        return total_size;
                    }

                    void Config::MergeFrom(const ::google::protobuf::Message &from)
                    {
                        // @@protoc_insertion_point(generalized_merge_from_start:v2ray.core.app.stats.command.Config)
                        GOOGLE_DCHECK_NE(&from, this);
                        const Config *source =
                            ::google::protobuf::DynamicCastToGenerated<Config>(
                                &from);

                        if (source == nullptr) {
                            // @@protoc_insertion_point(generalized_merge_from_cast_fail:v2ray.core.app.stats.command.Config)
                            ::google::protobuf::internal::ReflectionOps::Merge(from, this);
                        } else {
                            // @@protoc_insertion_point(generalized_merge_from_cast_success:v2ray.core.app.stats.command.Config)
                            MergeFrom(*source);
                        }
                    }

                    void Config::MergeFrom(const Config &from)
                    {
                        // @@protoc_insertion_point(class_specific_merge_from_start:v2ray.core.app.stats.command.Config)
                        GOOGLE_DCHECK_NE(&from, this);
                        _internal_metadata_.MergeFrom(from._internal_metadata_);
                        ::google::protobuf::uint32 cached_has_bits = 0;
                        (void) cached_has_bits;
                    }

                    void Config::CopyFrom(const ::google::protobuf::Message &from)
                    {
                        // @@protoc_insertion_point(generalized_copy_from_start:v2ray.core.app.stats.command.Config)
                        if (&from == this) return;

                        Clear();
                        MergeFrom(from);
                    }

                    void Config::CopyFrom(const Config &from)
                    {
                        // @@protoc_insertion_point(class_specific_copy_from_start:v2ray.core.app.stats.command.Config)
                        if (&from == this) return;

                        Clear();
                        MergeFrom(from);
                    }

                    bool Config::IsInitialized() const
                    {
                        return true;
                    }

                    void Config::Swap(Config *other)
                    {
                        if (other == this) return;

                        InternalSwap(other);
                    }
                    void Config::InternalSwap(Config *other)
                    {
                        using std::swap;
                        _internal_metadata_.Swap(&other->_internal_metadata_);
                    }

                    ::google::protobuf::Metadata Config::GetMetadata() const
                    {
                        ::google::protobuf::internal::AssignDescriptors(&::assign_descriptors_table_command_2eproto);
                        return ::file_level_metadata_command_2eproto[kIndexInFileMessages];
                    }


                    // @@protoc_insertion_point(namespace_scope)
                }  // namespace command
            }  // namespace stats
        }  // namespace app
    }  // namespace core
}  // namespace v2ray
namespace google
{
    namespace protobuf
    {
        template<> PROTOBUF_NOINLINE ::v2ray::core::app::stats::command::GetStatsRequest *Arena::CreateMaybeMessage< ::v2ray::core::app::stats::command::GetStatsRequest >(Arena *arena)
        {
            return Arena::CreateInternal< ::v2ray::core::app::stats::command::GetStatsRequest >(arena);
        }
        template<> PROTOBUF_NOINLINE ::v2ray::core::app::stats::command::Stat *Arena::CreateMaybeMessage< ::v2ray::core::app::stats::command::Stat >(Arena *arena)
        {
            return Arena::CreateInternal< ::v2ray::core::app::stats::command::Stat >(arena);
        }
        template<> PROTOBUF_NOINLINE ::v2ray::core::app::stats::command::GetStatsResponse *Arena::CreateMaybeMessage< ::v2ray::core::app::stats::command::GetStatsResponse >(Arena *arena)
        {
            return Arena::CreateInternal< ::v2ray::core::app::stats::command::GetStatsResponse >(arena);
        }
        template<> PROTOBUF_NOINLINE ::v2ray::core::app::stats::command::QueryStatsRequest *Arena::CreateMaybeMessage< ::v2ray::core::app::stats::command::QueryStatsRequest >(Arena *arena)
        {
            return Arena::CreateInternal< ::v2ray::core::app::stats::command::QueryStatsRequest >(arena);
        }
        template<> PROTOBUF_NOINLINE ::v2ray::core::app::stats::command::QueryStatsResponse *Arena::CreateMaybeMessage< ::v2ray::core::app::stats::command::QueryStatsResponse >(Arena *arena)
        {
            return Arena::CreateInternal< ::v2ray::core::app::stats::command::QueryStatsResponse >(arena);
        }
        template<> PROTOBUF_NOINLINE ::v2ray::core::app::stats::command::SysStatsRequest *Arena::CreateMaybeMessage< ::v2ray::core::app::stats::command::SysStatsRequest >(Arena *arena)
        {
            return Arena::CreateInternal< ::v2ray::core::app::stats::command::SysStatsRequest >(arena);
        }
        template<> PROTOBUF_NOINLINE ::v2ray::core::app::stats::command::SysStatsResponse *Arena::CreateMaybeMessage< ::v2ray::core::app::stats::command::SysStatsResponse >(Arena *arena)
        {
            return Arena::CreateInternal< ::v2ray::core::app::stats::command::SysStatsResponse >(arena);
        }
        template<> PROTOBUF_NOINLINE ::v2ray::core::app::stats::command::Config *Arena::CreateMaybeMessage< ::v2ray::core::app::stats::command::Config >(Arena *arena)
        {
            return Arena::CreateInternal< ::v2ray::core::app::stats::command::Config >(arena);
        }
    }  // namespace protobuf
}  // namespace google

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
