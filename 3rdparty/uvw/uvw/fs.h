#ifndef UVW_FS_INCLUDE_H
#define UVW_FS_INCLUDE_H


#include <utility>
#include <memory>
#include <string>
#include <chrono>
#include <uv.h>
#include "request.hpp"
#include "util.h"
#include "loop.h"


namespace uvw {


namespace details {


enum class UVFsType: std::underlying_type_t<uv_fs_type> {
    UNKNOWN = UV_FS_UNKNOWN,
    CUSTOM = UV_FS_CUSTOM,
    OPEN = UV_FS_OPEN,
    CLOSE = UV_FS_CLOSE,
    READ = UV_FS_READ,
    WRITE = UV_FS_WRITE,
    SENDFILE = UV_FS_SENDFILE,
    STAT = UV_FS_STAT,
    LSTAT = UV_FS_LSTAT,
    FSTAT = UV_FS_FSTAT,
    FTRUNCATE = UV_FS_FTRUNCATE,
    UTIME = UV_FS_UTIME,
    FUTIME = UV_FS_FUTIME,
    ACCESS = UV_FS_ACCESS,
    CHMOD = UV_FS_CHMOD,
    FCHMOD = UV_FS_FCHMOD,
    FSYNC = UV_FS_FSYNC,
    FDATASYNC = UV_FS_FDATASYNC,
    UNLINK = UV_FS_UNLINK,
    RMDIR = UV_FS_RMDIR,
    MKDIR = UV_FS_MKDIR,
    MKDTEMP = UV_FS_MKDTEMP,
    RENAME = UV_FS_RENAME,
    SCANDIR = UV_FS_SCANDIR,
    LINK = UV_FS_LINK,
    SYMLINK = UV_FS_SYMLINK,
    READLINK = UV_FS_READLINK,
    CHOWN = UV_FS_CHOWN,
    FCHOWN = UV_FS_FCHOWN,
    REALPATH = UV_FS_REALPATH,
    COPYFILE = UV_FS_COPYFILE,
    LCHOWN = UV_FS_LCHOWN,
    OPENDIR = UV_FS_OPENDIR,
    READDIR = UV_FS_READDIR,
    CLOSEDIR = UV_FS_CLOSEDIR,
    STATFS = UV_FS_STATFS,
    MKSTEMP = UV_FS_MKSTEMP,
    LUTIME = UV_FS_LUTIME
};


enum class UVDirentTypeT: std::underlying_type_t<uv_dirent_type_t> {
    UNKNOWN = UV_DIRENT_UNKNOWN,
    FILE = UV_DIRENT_FILE,
    DIR = UV_DIRENT_DIR,
    LINK = UV_DIRENT_LINK,
    FIFO = UV_DIRENT_FIFO,
    SOCKET = UV_DIRENT_SOCKET,
    CHAR = UV_DIRENT_CHAR,
    BLOCK = UV_DIRENT_BLOCK
};


enum class UVFileOpenFlags: int {
    APPEND = UV_FS_O_APPEND,
    CREAT = UV_FS_O_CREAT,
    DIRECT = UV_FS_O_DIRECT,
    DIRECTORY = UV_FS_O_DIRECTORY,
    DSYNC = UV_FS_O_DSYNC,
    EXCL = UV_FS_O_EXCL,
    EXLOCK = UV_FS_O_EXLOCK,
    FILEMAP = UV_FS_O_FILEMAP,
    NOATIME = UV_FS_O_NOATIME,
    NOCTTY = UV_FS_O_NOCTTY,
    NOFOLLOW = UV_FS_O_NOFOLLOW,
    NONBLOCK = UV_FS_O_NONBLOCK,
    RANDOM = UV_FS_O_RANDOM,
    RDONLY = UV_FS_O_RDONLY,
    RDWR = UV_FS_O_RDWR,
    SEQUENTIAL = UV_FS_O_SEQUENTIAL,
    SHORT_LIVED = UV_FS_O_SHORT_LIVED,
    SYMLINK = UV_FS_O_SYMLINK,
    SYNC = UV_FS_O_SYNC,
    TEMPORARY = UV_FS_O_TEMPORARY,
    TRUNC = UV_FS_O_TRUNC,
    WRONLY = UV_FS_O_WRONLY
};


enum class UVCopyFileFlags: int {
    EXCL = UV_FS_COPYFILE_EXCL,
    FICLONE = UV_FS_COPYFILE_FICLONE,
    FICLONE_FORCE = UV_FS_COPYFILE_FICLONE_FORCE
};


enum class UVSymLinkFlags: int {
    DIR = UV_FS_SYMLINK_DIR,
    JUNCTION = UV_FS_SYMLINK_JUNCTION
};


}


/**
 * @brief Default FsEvent event.
 *
 * Available types are:
 *
 * * `FsRequest::Type::UNKNOWN`
 * * `FsRequest::Type::CUSTOM`
 * * `FsRequest::Type::OPEN`
 * * `FsRequest::Type::CLOSE`
 * * `FsRequest::Type::READ`
 * * `FsRequest::Type::WRITE`
 * * `FsRequest::Type::SENDFILE`
 * * `FsRequest::Type::STAT`
 * * `FsRequest::Type::LSTAT`
 * * `FsRequest::Type::FSTAT`
 * * `FsRequest::Type::FTRUNCATE`
 * * `FsRequest::Type::UTIME`
 * * `FsRequest::Type::FUTIME`
 * * `FsRequest::Type::ACCESS`
 * * `FsRequest::Type::CHMOD`
 * * `FsRequest::Type::FCHMOD`
 * * `FsRequest::Type::FSYNC`
 * * `FsRequest::Type::FDATASYNC`
 * * `FsRequest::Type::UNLINK`
 * * `FsRequest::Type::RMDIR`
 * * `FsRequest::Type::MKDIR`
 * * `FsRequest::Type::MKDTEMP`
 * * `FsRequest::Type::RENAME`
 * * `FsRequest::Type::SCANDIR`
 * * `FsRequest::Type::LINK`
 * * `FsRequest::Type::SYMLINK`
 * * `FsRequest::Type::READLINK`
 * * `FsRequest::Type::CHOWN`
 * * `FsRequest::Type::FCHOWN`
 * * `FsRequest::Type::REALPATH`
 * * `FsRequest::Type::COPYFILE`
 * * `FsRequest::Type::LCHOWN`
 * * `FsRequest::Type::OPENDIR`
 * * `FsRequest::Type::READDIR`
 * * `FsRequest::Type::CLOSEDIR`
 * * `FsRequest::Type::STATFS`
 * * `FsRequest::Type::MKSTEMP`
 * * `FsRequest::Type::LUTIME`
 *
 * It will be emitted by FsReq and/or FileReq according with their
 * functionalities.
 *
 * See the official
 * [documentation](http://docs.libuv.org/en/v1.x/fs.html#c.uv_fs_type)
 * for further details.
 */
template<details::UVFsType e>
struct FsEvent {
    FsEvent(const char *pathname) noexcept: path{pathname} {}

    const char * path; /*!< The path affecting the request. */
};


/**
 * @brief FsEvent event specialization for `FsRequest::Type::READ`.
 *
 * It will be emitted by FsReq and/or FileReq according with their
 * functionalities.
 */
template<>
struct FsEvent<details::UVFsType::READ> {
    FsEvent(const char *pathname, std::unique_ptr<const char[]> buf, std::size_t sz) noexcept
        : path{pathname}, data{std::move(buf)}, size{sz}
    {}

    const char * path; /*!< The path affecting the request. */
    std::unique_ptr<const char[]> data; /*!< A bunch of data read from the given path. */
    std::size_t size; /*!< The amount of data read from the given path. */
};


/**
 * @brief FsEvent event specialization for `FsRequest::Type::WRITE`.
 *
 * It will be emitted by FsReq and/or FileReq according with their
 * functionalities.
 */
template<>
struct FsEvent<details::UVFsType::WRITE> {
    FsEvent(const char *pathname, std::size_t sz) noexcept
        : path{pathname}, size{sz}
    {}

    const char * path; /*!< The path affecting the request. */
    std::size_t size; /*!< The amount of data written to the given path. */
};


/**
 * @brief FsEvent event specialization for `FsRequest::Type::SENDFILE`.
 *
 * It will be emitted by FsReq and/or FileReq according with their
 * functionalities.
 */
template<>
struct FsEvent<details::UVFsType::SENDFILE> {
    FsEvent(const char *pathname, std::size_t sz) noexcept
        : path{pathname}, size{sz}
    {}

    const char * path; /*!< The path affecting the request. */
    std::size_t size; /*!< The amount of data transferred. */
};


/**
 * @brief FsEvent event specialization for `FsRequest::Type::STAT`.
 *
 * It will be emitted by FsReq and/or FileReq according with their
 * functionalities.
 */
template<>
struct FsEvent<details::UVFsType::STAT> {
    FsEvent(const char *pathname, Stat curr) noexcept
        : path{pathname}, stat{std::move(curr)}
    {}

    const char * path; /*!< The path affecting the request. */
    Stat stat; /*!< An initialized instance of Stat. */
};


/**
 * @brief FsEvent event specialization for `FsRequest::Type::FSTAT`.
 *
 * It will be emitted by FsReq and/or FileReq according with their
 * functionalities.
 */
template<>
struct FsEvent<details::UVFsType::FSTAT> {
    FsEvent(const char *pathname, Stat curr) noexcept
        : path{pathname}, stat{std::move(curr)}
    {}

    const char * path; /*!< The path affecting the request. */
    Stat stat; /*!< An initialized instance of Stat. */
};


/**
 * @brief FsEvent event specialization for `FsRequest::Type::LSTAT`.
 *
 * It will be emitted by FsReq and/or FileReq according with their
 * functionalities.
 */
template<>
struct FsEvent<details::UVFsType::LSTAT> {
    FsEvent(const char *pathname, Stat curr) noexcept
        : path{pathname}, stat{std::move(curr)}
    {}

    const char * path; /*!< The path affecting the request. */
    Stat stat; /*!< An initialized instance of Stat. */
};


/**
 * @brief FsEvent event specialization for `FsRequest::Type::STATFS`.
 *
 * It will be emitted by FsReq and/or FileReq according with their
 * functionalities.
 */
template<>
struct FsEvent<details::UVFsType::STATFS> {
    FsEvent(const char *pathname, Statfs curr) noexcept
        : path{pathname}, statfs{std::move(curr)}
    {}

    const char * path; /*!< The path affecting the request. */
    Statfs statfs; /*!< An initialized instance of Statfs. */
};


/**
 * @brief FsEvent event specialization for `FsRequest::Type::MKSTEMP`.
 *
 * It will be emitted by FsReq and/or FileReq according with their
 * functionalities.
 */
template<>
struct FsEvent<details::UVFsType::MKSTEMP> {
    FsEvent(const char *pathname, std::size_t desc) noexcept
        : path{pathname}, descriptor{desc}
    {}

    const char * path; /*!< The created file path. */
    std::size_t descriptor; /*!< The file descriptor as an integer. */
};


/**
 * @brief FsEvent event specialization for `FsRequest::Type::SCANDIR`.
 *
 * It will be emitted by FsReq and/or FileReq according with their
 * functionalities.
 */
template<>
struct FsEvent<details::UVFsType::SCANDIR> {
    FsEvent(const char *pathname, std::size_t sz) noexcept
        : path{pathname}, size{sz}
    {}

    const char * path; /*!< The path affecting the request. */
    std::size_t size; /*!< The number of directory entries selected. */
};


/**
 * @brief FsEvent event specialization for `FsRequest::Type::READLINK`.
 *
 * It will be emitted by FsReq and/or FileReq according with their
 * functionalities.
 */
template<>
struct FsEvent<details::UVFsType::READLINK> {
    explicit FsEvent(const char *pathname, const char *buf, std::size_t sz) noexcept
        : path{pathname}, data{buf}, size{sz}
    {}

    const char * path; /*!< The path affecting the request. */
    const char * data; /*!< A bunch of data read from the given path. */
    std::size_t size; /*!< The amount of data read from the given path. */
};


/**
 * @brief FsEvent event specialization for `FsRequest::Type::READDIR`.
 *
 * It will be emitted by FsReq and/or FileReq according with their
 * functionalities.
 */
template<>
struct FsEvent<details::UVFsType::READDIR> {
    using EntryType = details::UVDirentTypeT;

    FsEvent(const char *name, EntryType type, bool eos) noexcept
        : name{name}, type{type}, eos{eos}
    {}

    const char * name; /*!< The name of the last entry. */
    EntryType type; /*!< The entry type. */
    bool eos; /*!< True if there a no more entries to read. */
};


/**
 * @brief Base class for FsReq and/or FileReq.
 *
 * Not directly instantiable, should not be used by the users of the library.
 */
template<typename T>
class FsRequest: public Request<T, uv_fs_t> {
protected:
    template<details::UVFsType e>
    static void fsGenericCallback(uv_fs_t *req) {
        auto ptr = Request<T, uv_fs_t>::reserve(req);
        if(req->result < 0) { ptr->publish(ErrorEvent{req->result}); }
        else { ptr->publish(FsEvent<e>{req->path}); }
    }

    template<details::UVFsType e>
    static void fsResultCallback(uv_fs_t *req) {
        auto ptr = Request<T, uv_fs_t>::reserve(req);
        if(req->result < 0) { ptr->publish(ErrorEvent{req->result}); }
        else { ptr->publish(FsEvent<e>{req->path, static_cast<std::size_t>(req->result)}); }
    }

    template<details::UVFsType e>
    static void fsStatCallback(uv_fs_t *req) {
        auto ptr = Request<T, uv_fs_t>::reserve(req);
        if(req->result < 0) { ptr->publish(ErrorEvent{req->result}); }
        else { ptr->publish(FsEvent<e>{req->path, req->statbuf}); }
    }

    static void fsStatfsCallback(uv_fs_t *req) {
        auto ptr = Request<T, uv_fs_t>::reserve(req);
        if(req->result < 0) { ptr->publish(ErrorEvent{req->result}); }
        else { ptr->publish(FsEvent<Type::STATFS>{req->path, *static_cast<Statfs *>(req->ptr)}); }
    }

    template<typename... Args>
    void cleanupAndInvoke(Args&&... args) {
        uv_fs_req_cleanup(this->get());
        this->invoke(std::forward<Args>(args)...);
    }

    template<typename F, typename... Args>
    void cleanupAndInvokeSync(F &&f, Args&&... args) {
        uv_fs_req_cleanup(this->get());
        std::forward<F>(f)(std::forward<Args>(args)..., nullptr);
    }

public:
    using Time = std::chrono::duration<double>;
    using Type = details::UVFsType;
    using EntryType = details::UVDirentTypeT;

    using Request<T, uv_fs_t>::Request;
};


/**
 * @brief The FileReq request.
 *
 * Cross-platform sync and async filesystem operations.<br/>
 * All file operations are run on the threadpool.
 *
 * To create a `FileReq` through a `Loop`, no arguments are required.
 *
 * See the official
 * [documentation](http://docs.libuv.org/en/v1.x/fs.html)
 * for further details.
 */
class FileReq final: public FsRequest<FileReq> {
    static constexpr uv_file BAD_FD = -1;

    static void fsOpenCallback(uv_fs_t *req);
    static void fsCloseCallback(uv_fs_t *req);
    static void fsReadCallback(uv_fs_t *req);

public:
    using FileOpen = details::UVFileOpenFlags;

    using FsRequest::FsRequest;

    ~FileReq() noexcept;

    /**
     * @brief Async [close](http://linux.die.net/man/2/close).
     *
     * Emit a `FsEvent<FileReq::Type::CLOSE>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     */
    void close();

    /**
     * @brief Sync [close](http://linux.die.net/man/2/close).
     * @return True in case of success, false otherwise.
     */
    bool closeSync();

    /**
     * @brief Async [open](http://linux.die.net/man/2/open).
     *
     * Emit a `FsEvent<FileReq::Type::OPEN>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     *
     * Available flags are:
     *
     * * `FileReq::FileOpen::APPEND`
     * * `FileReq::FileOpen::CREAT`
     * * `FileReq::FileOpen::DIRECT`
     * * `FileReq::FileOpen::DIRECTORY`
     * * `FileReq::FileOpen::DSYNC`
     * * `FileReq::FileOpen::EXCL`
     * * `FileReq::FileOpen::EXLOCK`
     * * `FileReq::FileOpen::FILEMAP`
     * * `FileReq::FileOpen::NOATIME`
     * * `FileReq::FileOpen::NOCTTY`
     * * `FileReq::FileOpen::NOFOLLOW`
     * * `FileReq::FileOpen::NONBLOCK`
     * * `FileReq::FileOpen::RANDOM`
     * * `FileReq::FileOpen::RDONLY`
     * * `FileReq::FileOpen::RDWR`
     * * `FileReq::FileOpen::SEQUENTIAL`
     * * `FileReq::FileOpen::SHORT_LIVED`
     * * `FileReq::FileOpen::SYMLINK`
     * * `FileReq::FileOpen::SYNC`
     * * `FileReq::FileOpen::TEMPORARY`
     * * `FileReq::FileOpen::TRUNC`
     * * `FileReq::FileOpen::WRONLY`
     *
     * See the official
     * [documentation](http://docs.libuv.org/en/v1.x/fs.html#file-open-constants)
     * for further details.
     *
     * @param path A valid path name for a file.
     * @param flags Flags made out of underlying constants.
     * @param mode Mode, as described in the official documentation.
     */
    void open(std::string path, Flags<FileOpen> flags, int mode);

    /**
     * @brief Sync [open](http://linux.die.net/man/2/open).
     *
     * Available flags are:
     *
     * * `FileReq::FileOpen::APPEND`
     * * `FileReq::FileOpen::CREAT`
     * * `FileReq::FileOpen::DIRECT`
     * * `FileReq::FileOpen::DIRECTORY`
     * * `FileReq::FileOpen::DSYNC`
     * * `FileReq::FileOpen::EXCL`
     * * `FileReq::FileOpen::EXLOCK`
     * * `FileReq::FileOpen::FILEMAP`
     * * `FileReq::FileOpen::NOATIME`
     * * `FileReq::FileOpen::NOCTTY`
     * * `FileReq::FileOpen::NOFOLLOW`
     * * `FileReq::FileOpen::NONBLOCK`
     * * `FileReq::FileOpen::RANDOM`
     * * `FileReq::FileOpen::RDONLY`
     * * `FileReq::FileOpen::RDWR`
     * * `FileReq::FileOpen::SEQUENTIAL`
     * * `FileReq::FileOpen::SHORT_LIVED`
     * * `FileReq::FileOpen::SYMLINK`
     * * `FileReq::FileOpen::SYNC`
     * * `FileReq::FileOpen::TEMPORARY`
     * * `FileReq::FileOpen::TRUNC`
     * * `FileReq::FileOpen::WRONLY`
     *
     * See the official
     * [documentation](http://docs.libuv.org/en/v1.x/fs.html#file-open-constants)
     * for further details.
     *
     * @param path A valid path name for a file.
     * @param flags Flags made out of underlying constants.
     * @param mode Mode, as described in the official documentation.
     * @return True in case of success, false otherwise.
     */
    bool openSync(std::string path, Flags<FileOpen> flags, int mode);

    /**
     * @brief Async [read](http://linux.die.net/man/2/preadv).
     *
     * Emit a `FsEvent<FileReq::Type::READ>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     *
     * @param offset Offset, as described in the official documentation.
     * @param len Length, as described in the official documentation.
     */
    void read(int64_t offset, unsigned int len);

    /**
     * @brief Sync [read](http://linux.die.net/man/2/preadv).
     *
     * @param offset Offset, as described in the official documentation.
     * @param len Length, as described in the official documentation.
     *
     * @return A `std::pair` composed as it follows:
     * * A boolean value that is true in case of success, false otherwise.
     * * A `std::pair` composed as it follows:
     *   * A bunch of data read from the given path.
     *   * The amount of data read from the given path.
     */
    std::pair<bool, std::pair<std::unique_ptr<const char[]>, std::size_t>> readSync(int64_t offset, unsigned int len);

    /**
     * @brief Async [write](http://linux.die.net/man/2/pwritev).
     *
     * The request takes the ownership of the data and it is in charge of delete
     * them.
     *
     * Emit a `FsEvent<FileReq::Type::WRITE>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     *
     * @param buf The data to be written.
     * @param len The lenght of the submitted data.
     * @param offset Offset, as described in the official documentation.
     */
    void write(std::unique_ptr<char[]> buf, unsigned int len, int64_t offset);

    /**
     * @brief Async [write](http://linux.die.net/man/2/pwritev).
     *
     * The request doesn't take the ownership of the data. Be sure that their
     * lifetime overcome the one of the request.
     *
     * Emit a `FsEvent<FileReq::Type::WRITE>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     *
     * @param buf The data to be written.
     * @param len The lenght of the submitted data.
     * @param offset Offset, as described in the official documentation.
     */
    void write(char *buf, unsigned int len, int64_t offset);

    /**
     * @brief Sync [write](http://linux.die.net/man/2/pwritev).
     *
     * @param buf The data to be written.
     * @param len The lenght of the submitted data.
     * @param offset Offset, as described in the official documentation.
     *
     * @return A `std::pair` composed as it follows:
     * * A boolean value that is true in case of success, false otherwise.
     * * The amount of data written to the given path.
     */
    std::pair<bool, std::size_t> writeSync(std::unique_ptr<char[]> buf, unsigned int len, int64_t offset);

    /**
     * @brief Async [fstat](http://linux.die.net/man/2/fstat).
     *
     * Emit a `FsEvent<FileReq::Type::FSTAT>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     */
    void stat();

    /**
     * @brief Sync [fstat](http://linux.die.net/man/2/fstat).
     *
     * @return A `std::pair` composed as it follows:
     * * A boolean value that is true in case of success, false otherwise.
     * * An initialized instance of Stat.
     */
    std::pair<bool, Stat> statSync();

    /**
     * @brief Async [fsync](http://linux.die.net/man/2/fsync).
     *
     * Emit a `FsEvent<FileReq::Type::FSYNC>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     */
    void sync();

    /**
     * @brief Sync [fsync](http://linux.die.net/man/2/fsync).
     * @return True in case of success, false otherwise.
     */
    bool syncSync();

    /**
     * @brief Async [fdatasync](http://linux.die.net/man/2/fdatasync).
     *
     * Emit a `FsEvent<FileReq::Type::FDATASYNC>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     */
    void datasync();

    /**
     * @brief Sync [fdatasync](http://linux.die.net/man/2/fdatasync).
     * @return True in case of success, false otherwise.
     */
    bool datasyncSync();

    /**
     * @brief Async [ftruncate](http://linux.die.net/man/2/ftruncate).
     *
     * Emit a `FsEvent<FileReq::Type::FTRUNCATE>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     *
     * @param offset Offset, as described in the official documentation.
     */
    void truncate(int64_t offset);

    /**
     * @brief Sync [ftruncate](http://linux.die.net/man/2/ftruncate).
     * @param offset Offset, as described in the official documentation.
     * @return True in case of success, false otherwise.
     */
    bool truncateSync(int64_t offset);

    /**
     * @brief Async [sendfile](http://linux.die.net/man/2/sendfile).
     *
     * Emit a `FsEvent<FileReq::Type::SENDFILE>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     *
     * @param out A valid instance of FileHandle.
     * @param offset Offset, as described in the official documentation.
     * @param length Length, as described in the official documentation.
     */
    void sendfile(FileHandle out, int64_t offset, std::size_t length);

    /**
     * @brief Sync [sendfile](http://linux.die.net/man/2/sendfile).
     *
     * @param out A valid instance of FileHandle.
     * @param offset Offset, as described in the official documentation.
     * @param length Length, as described in the official documentation.
     *
     * @return A `std::pair` composed as it follows:
     * * A boolean value that is true in case of success, false otherwise.
     * * The amount of data transferred.
     */
    std::pair<bool, std::size_t> sendfileSync(FileHandle out, int64_t offset, std::size_t length);

    /**
     * @brief Async [fchmod](http://linux.die.net/man/2/fchmod).
     *
     * Emit a `FsEvent<FileReq::Type::FCHMOD>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     *
     * @param mode Mode, as described in the official documentation.
     */
    void chmod(int mode);

    /**
     * @brief Sync [fchmod](http://linux.die.net/man/2/fchmod).
     * @param mode Mode, as described in the official documentation.
     * @return True in case of success, false otherwise.
     */
    bool chmodSync(int mode);

    /**
     * @brief Async [futime](http://linux.die.net/man/3/futimes).
     *
     * Emit a `FsEvent<FileReq::Type::FUTIME>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     *
     * @param atime `std::chrono::duration<double>`, having the same meaning as
     * described in the official documentation.
     * @param mtime `std::chrono::duration<double>`, having the same meaning as
     * described in the official documentation.
     */
    void futime(Time atime, Time mtime);

    /**
     * @brief Sync [futime](http://linux.die.net/man/3/futimes).
     * @param atime `std::chrono::duration<double>`, having the same meaning as
     * described in the official documentation.
     * @param mtime `std::chrono::duration<double>`, having the same meaning as
     * described in the official documentation.
     * @return True in case of success, false otherwise.
     */
    bool futimeSync(Time atime, Time mtime);

    /**
     * @brief Async [fchown](http://linux.die.net/man/2/fchown).
     *
     * Emit a `FsEvent<FileReq::Type::FCHOWN>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     *
     * @param uid UID, as described in the official documentation.
     * @param gid GID, as described in the official documentation.
     */
    void chown(Uid uid, Gid gid);

    /**
     * @brief Sync [fchown](http://linux.die.net/man/2/fchown).
     * @param uid UID, as described in the official documentation.
     * @param gid GID, as described in the official documentation.
     * @return True in case of success, false otherwise.
     */
    bool chownSync(Uid uid, Gid gid);

    /**
     * @brief Cast operator to FileHandle.
     *
     * Cast operator to an internal representation of the underlying file
     * handle.
     *
     * @return A valid instance of FileHandle (the descriptor can be invalid).
     */
    operator FileHandle() const noexcept;

private:
    std::unique_ptr<char[]> current{nullptr};
    uv_buf_t buffer{};
    uv_file file{BAD_FD};
};


/**
 * @brief The FsReq request.
 *
 * Cross-platform sync and async filesystem operations.<br/>
 * All file operations are run on the threadpool.
 *
 * To create a `FsReq` through a `Loop`, no arguments are required.
 *
 * See the official
 * [documentation](http://docs.libuv.org/en/v1.x/fs.html)
 * for further details.
 */
class FsReq final: public FsRequest<FsReq> {
    static void fsReadlinkCallback(uv_fs_t *req);
    static void fsReaddirCallback(uv_fs_t *req);

public:
    using CopyFile = details::UVCopyFileFlags;
    using SymLink = details::UVSymLinkFlags;

    using FsRequest::FsRequest;

    ~FsReq() noexcept;

    /**
     * @brief Async [unlink](http://linux.die.net/man/2/unlink).
     *
     * Emit a `FsEvent<FsReq::Type::UNLINK>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     *
     * @param path Path, as described in the official documentation.
     */
    void unlink(std::string path);

    /**
     * @brief Sync [unlink](http://linux.die.net/man/2/unlink).
     * @param path Path, as described in the official documentation.
     * @return True in case of success, false otherwise.
     */
    bool unlinkSync(std::string path);

    /**
     * @brief Async [mkdir](http://linux.die.net/man/2/mkdir).
     *
     * Emit a `FsEvent<FsReq::Type::MKDIR>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     *
     * @param path Path, as described in the official documentation.
     * @param mode Mode, as described in the official documentation.
     */
    void mkdir(std::string path, int mode);

    /**
     * @brief Sync [mkdir](http://linux.die.net/man/2/mkdir).
     * @param path Path, as described in the official documentation.
     * @param mode Mode, as described in the official documentation.
     * @return True in case of success, false otherwise.
     */
    bool mkdirSync(std::string path, int mode);

    /**
     * @brief Async [mktemp](http://linux.die.net/man/3/mkdtemp).
     *
     * Emit a `FsEvent<FsReq::Type::MKDTEMP>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     *
     * @param tpl Template, as described in the official documentation.
     */
    void mkdtemp(std::string tpl);

    /**
     * @brief Sync [mktemp](http://linux.die.net/man/3/mkdtemp).
     *
     * @param tpl Template, as described in the official documentation.
     *
     * @return A `std::pair` composed as it follows:
     * * A boolean value that is true in case of success, false otherwise.
     * * The actual path of the newly created directory.
     */
    std::pair<bool, const char *> mkdtempSync(std::string tpl);

    /**
     * @brief Async [mkstemp](https://linux.die.net/man/3/mkstemp).
     *
     * Emit a `FsEvent<FsReq::Type::MKSTEMP>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     *
     * @param tpl Template, as described in the official documentation.
     */
    void mkstemp(std::string tpl);

    /**
     * @brief Sync [mkstemp](https://linux.die.net/man/3/mkstemp).
     *
     * Returns a composed value where:
     *
     * * The first parameter indicates the created file path.
     * * The second parameter is the file descriptor as an integer.
     *
     * See the official
     * [documentation](http://docs.libuv.org/en/v1.x/fs.html#c.uv_fs_mkstemp)
     * for further details.
     *
     * @param tpl Template, as described in the official documentation.
     *
     * @return A pair where:

     * * The first parameter is a boolean value that is true in case of success,
     * false otherwise.
     * * The second parameter is a composed value (see above).
     */
    std::pair<bool, std::pair<std::string, std::size_t>> mkstempSync(std::string tpl);

    /**
     * @brief Async [lutime](http://linux.die.net/man/3/lutimes).
     *
     * Emit a `FsEvent<FsReq::Type::UTIME>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     *
     * @param path Path, as described in the official documentation.
     * @param atime `std::chrono::duration<double>`, having the same meaning as
     * described in the official documentation.
     * @param mtime `std::chrono::duration<double>`, having the same meaning as
     * described in the official documentation.
     */
    void lutime(std::string path, Time atime, Time mtime);

    /**
     * @brief Sync [lutime](http://linux.die.net/man/3/lutimes).
     * @param path Path, as described in the official documentation.
     * @param atime `std::chrono::duration<double>`, having the same meaning as
     * described in the official documentation.
     * @param mtime `std::chrono::duration<double>`, having the same meaning as
     * described in the official documentation.
     * @return True in case of success, false otherwise.
     */
    bool lutimeSync(std::string path, Time atime, Time mtime);

    /**
     * @brief Async [rmdir](http://linux.die.net/man/2/rmdir).
     *
     * Emit a `FsEvent<FsReq::Type::RMDIR>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     *
     * @param path Path, as described in the official documentation.
     */
    void rmdir(std::string path);

    /**
     * @brief Sync [rmdir](http://linux.die.net/man/2/rmdir).
     * @param path Path, as described in the official documentation.
     * @return True in case of success, false otherwise.
     */
    bool rmdirSync(std::string path);

    /**
     * @brief Async [scandir](http://linux.die.net/man/3/scandir).
     *
     * Emit a `FsEvent<FsReq::Type::SCANDIR>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     *
     * @param path Path, as described in the official documentation.
     * @param flags Flags, as described in the official documentation.
     */
    void scandir(std::string path, int flags);

    /**
     * @brief Sync [scandir](http://linux.die.net/man/3/scandir).
     *
     * @param path Path, as described in the official documentation.
     * @param flags Flags, as described in the official documentation.
     *
     * @return A `std::pair` composed as it follows:
     * * A boolean value that is true in case of success, false otherwise.
     * * The number of directory entries selected.
     */
    std::pair<bool, std::size_t> scandirSync(std::string path, int flags);

    /**
     * @brief Gets entries populated with the next directory entry data.
     *
     * Returns a composed value where:
     *
     * * The first parameter indicates the entry type (see below).
     * * The second parameter is a string that contains the actual value.
     *
     * Available entry types are:
     *
     * * `FsReq::EntryType::UNKNOWN`
     * * `FsReq::EntryType::FILE`
     * * `FsReq::EntryType::DIR`
     * * `FsReq::EntryType::LINK`
     * * `FsReq::EntryType::FIFO`
     * * `FsReq::EntryType::SOCKET`
     * * `FsReq::EntryType::CHAR`
     * * `FsReq::EntryType::BLOCK`
     *
     * See the official
     * [documentation](http://docs.libuv.org/en/v1.x/fs.html#c.uv_dirent_t)
     * for further details.
     *
     * @return A pair where:
     *
     * * The first parameter is a boolean value that indicates if the current
     * entry is still valid.
     * * The second parameter is a composed value (see above).
     */
    std::pair<bool, std::pair<EntryType, const char *>> scandirNext();

    /**
     * @brief Async [stat](http://linux.die.net/man/2/stat).
     *
     * Emit a `FsEvent<FsReq::Type::STAT>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     *
     * @param path Path, as described in the official documentation.
     */
    void stat(std::string path);

    /**
     * @brief Sync [stat](http://linux.die.net/man/2/stat).
     *
     * @param path Path, as described in the official documentation.
     *
     * @return A `std::pair` composed as it follows:
     * * A boolean value that is true in case of success, false otherwise.
     * * An initialized instance of Stat.
     */
    std::pair<bool, Stat> statSync(std::string path);

    /**
     * @brief Async [lstat](http://linux.die.net/man/2/lstat).
     *
     * Emit a `FsEvent<FsReq::Type::LSTAT>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     *
     * @param path Path, as described in the official documentation.
     */
    void lstat(std::string path);

    /**
     * @brief Sync [lstat](http://linux.die.net/man/2/lstat).
     *
     * @param path Path, as described in the official documentation.
     *
     * @return A `std::pair` composed as it follows:
     * * A boolean value that is true in case of success, false otherwise.
     * * An initialized instance of Stat.
     */
    std::pair<bool, Stat> lstatSync(std::string path);

    /**
     * @brief Async [statfs](http://linux.die.net/man/2/statfs).
     *
     * Emit a `FsEvent<FsReq::Type::STATFS>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     *
     * Any fields in the resulting object that are not supported by the
     * underlying operating system are set to zero.
     *
     * @param path Path, as described in the official documentation.
     */
    void statfs(std::string path);

    /**
     * @brief Sync [statfs](http://linux.die.net/man/2/statfs).
     *
     * Any fields in the resulting object that are not supported by the
     * underlying operating system are set to zero.
     *
     * @param path Path, as described in the official documentation.
     *
     * @return A `std::pair` composed as it follows:
     * * A boolean value that is true in case of success, false otherwise.
     * * An initialized instance of Statfs.
     */
    std::pair<bool, Statfs> statfsSync(std::string path);

    /**
     * @brief Async [rename](http://linux.die.net/man/2/rename).
     *
     * Emit a `FsEvent<FsReq::Type::RENAME>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     *
     * @param old Old path, as described in the official documentation.
     * @param path New path, as described in the official documentation.
     */
    void rename(std::string old, std::string path);

    /**
     * @brief Sync [rename](http://linux.die.net/man/2/rename).
     * @param old Old path, as described in the official documentation.
     * @param path New path, as described in the official documentation.
     * @return True in case of success, false otherwise.
     */
    bool renameSync(std::string old, std::string path);

    /**
     * @brief Copies a file asynchronously from a path to a new one.
     *
     * Emit a `FsEvent<FsReq::Type::UV_FS_COPYFILE>` event when
     * completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     *
     * Available flags are:
     *
     * * `FsReq::CopyFile::EXCL`: it fails if the destination path
     * already exists (the default behavior is to overwrite the destination if
     * it exists).
     * * `FsReq::CopyFile::FICLONE`: If present, it will attempt to create a
     * copy-on-write reflink. If the underlying platform does not support
     * copy-on-write, then a fallback copy mechanism is used.
     * * `FsReq::CopyFile::FICLONE_FORCE`: If present, it will attempt to create
     * a copy-on-write reflink. If the underlying platform does not support
     * copy-on-write, then an error is returned.
     *
     * @warning
     * If the destination path is created, but an error occurs while copying the
     * data, then the destination path is removed. There is a brief window of
     * time between closing and removing the file where another process could
     * access the file.
     *
     * @param old Old path, as described in the official documentation.
     * @param path New path, as described in the official documentation.
     * @param flags Optional additional flags.
     */
    void copyfile(std::string old, std::string path, Flags<CopyFile> flags = Flags<CopyFile>{});

    /**
     * @brief Copies a file synchronously from a path to a new one.
     *
     * Available flags are:
     *
     * * `FsReq::CopyFile::EXCL`: it fails if the destination path
     * already exists (the default behavior is to overwrite the destination if
     * it exists).
     *
     * If the destination path is created, but an error occurs while copying the
     * data, then the destination path is removed. There is a brief window of
     * time between closing and removing the file where another process could
     * access the file.
     *
     * @param old Old path, as described in the official documentation.
     * @param path New path, as described in the official documentation.
     * @param flags Optional additional flags.
     * @return True in case of success, false otherwise.
     */
    bool copyfileSync(std::string old, std::string path, Flags<CopyFile> flags = Flags<CopyFile>{});

    /**
     * @brief Async [access](http://linux.die.net/man/2/access).
     *
     * Emit a `FsEvent<FsReq::Type::ACCESS>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     *
     * @param path Path, as described in the official documentation.
     * @param mode Mode, as described in the official documentation.
     */
    void access(std::string path, int mode);

    /**
     * @brief Sync [access](http://linux.die.net/man/2/access).
     * @param path Path, as described in the official documentation.
     * @param mode Mode, as described in the official documentation.
     * @return True in case of success, false otherwise.
     */
    bool accessSync(std::string path, int mode);

    /**
     * @brief Async [chmod](http://linux.die.net/man/2/chmod).
     *
     * Emit a `FsEvent<FsReq::Type::CHMOD>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     *
     * @param path Path, as described in the official documentation.
     * @param mode Mode, as described in the official documentation.
     */
    void chmod(std::string path, int mode);

    /**
     * @brief Sync [chmod](http://linux.die.net/man/2/chmod).
     * @param path Path, as described in the official documentation.
     * @param mode Mode, as described in the official documentation.
     * @return True in case of success, false otherwise.
     */
    bool chmodSync(std::string path, int mode);

    /**
     * @brief Async [utime](http://linux.die.net/man/2/utime).
     *
     * Emit a `FsEvent<FsReq::Type::UTIME>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     *
     * @param path Path, as described in the official documentation.
     * @param atime `std::chrono::duration<double>`, having the same meaning as
     * described in the official documentation.
     * @param mtime `std::chrono::duration<double>`, having the same meaning as
     * described in the official documentation.
     */
    void utime(std::string path, Time atime, Time mtime);

    /**
     * @brief Sync [utime](http://linux.die.net/man/2/utime).
     * @param path Path, as described in the official documentation.
     * @param atime `std::chrono::duration<double>`, having the same meaning as
     * described in the official documentation.
     * @param mtime `std::chrono::duration<double>`, having the same meaning as
     * described in the official documentation.
     * @return True in case of success, false otherwise.
     */
    bool utimeSync(std::string path, Time atime, Time mtime);

    /**
     * @brief Async [link](http://linux.die.net/man/2/link).
     *
     * Emit a `FsEvent<FsReq::Type::LINK>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     *
     * @param old Old path, as described in the official documentation.
     * @param path New path, as described in the official documentation.
     */
    void link(std::string old, std::string path);

    /**
     * @brief Sync [link](http://linux.die.net/man/2/link).
     * @param old Old path, as described in the official documentation.
     * @param path New path, as described in the official documentation.
     * @return True in case of success, false otherwise.
     */
    bool linkSync(std::string old, std::string path);

    /**
     * @brief Async [symlink](http://linux.die.net/man/2/symlink).
     *
     * Emit a `FsEvent<FsReq::Type::SYMLINK>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     *
     * Available flags are:
     *
     * * `FsReq::SymLink::DIR`: it indicates that the old path points to a
     * directory.
     * * `FsReq::SymLink::JUNCTION`: it requests that the symlink is created
     * using junction points.
     *
     * @param old Old path, as described in the official documentation.
     * @param path New path, as described in the official documentation.
     * @param flags Optional additional flags.
     */
    void symlink(std::string old, std::string path, Flags<SymLink> flags = Flags<SymLink>{});

    /**
     * @brief Sync [symlink](http://linux.die.net/man/2/symlink).
     *
     * Available flags are:
     *
     * * `FsReq::SymLink::DIR`: it indicates that the old path points to a
     * directory.
     * * `FsReq::SymLink::JUNCTION`: it requests that the symlink is created
     * using junction points.
     *
     * @param old Old path, as described in the official documentation.
     * @param path New path, as described in the official documentation.
     * @param flags Flags, as described in the official documentation.
     * @return True in case of success, false otherwise.
     */
    bool symlinkSync(std::string old, std::string path, Flags<SymLink> flags = Flags<SymLink>{});

    /**
     * @brief Async [readlink](http://linux.die.net/man/2/readlink).
     *
     * Emit a `FsEvent<FsReq::Type::READLINK>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     *
     * @param path Path, as described in the official documentation.
     */
    void readlink(std::string path);

    /**
     * @brief Sync [readlink](http://linux.die.net/man/2/readlink).
     *
     * @param path Path, as described in the official documentation.
     *
     * @return A `std::pair` composed as it follows:
     * * A boolean value that is true in case of success, false otherwise.
     * * A `std::pair` composed as it follows:
     *   * A bunch of data read from the given path.
     *   * The amount of data read from the given path.
     */
    std::pair<bool, std::pair<const char *, std::size_t>> readlinkSync(std::string path);

    /**
     * @brief Async [realpath](http://linux.die.net/man/3/realpath).
     *
     * Emit a `FsEvent<FsReq::Type::REALPATH>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     *
     * @param path Path, as described in the official documentation.
     */
    void realpath(std::string path);

    /**
     * @brief Sync [realpath](http://linux.die.net/man/3/realpath).
     *
     * @param path Path, as described in the official documentation.
     *
     * @return A `std::pair` composed as it follows:
     * * A boolean value that is true in case of success, false otherwise.
     * * The canonicalized absolute pathname.
     */
    std::pair<bool, const char *> realpathSync(std::string path);

    /**
     * @brief Async [chown](http://linux.die.net/man/2/chown).
     *
     * Emit a `FsEvent<FsReq::Type::CHOWN>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     *
     * @param path Path, as described in the official documentation.
     * @param uid UID, as described in the official documentation.
     * @param gid GID, as described in the official documentation.
     */
    void chown(std::string path, Uid uid, Gid gid);

    /**
     * @brief Sync [chown](http://linux.die.net/man/2/chown).
     * @param path Path, as described in the official documentation.
     * @param uid UID, as described in the official documentation.
     * @param gid GID, as described in the official documentation.
     * @return True in case of success, false otherwise.
     */
    bool chownSync(std::string path, Uid uid, Gid gid);

    /**
     * @brief Async [lchown](https://linux.die.net/man/2/lchown).
     *
     * Emit a `FsEvent<FsReq::Type::LCHOWN>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     *
     * @param path Path, as described in the official documentation.
     * @param uid UID, as described in the official documentation.
     * @param gid GID, as described in the official documentation.
     */
    void lchown(std::string path, Uid uid, Gid gid);

    /**
     * @brief Sync [lchown](https://linux.die.net/man/2/lchown).
     * @param path Path, as described in the official documentation.
     * @param uid UID, as described in the official documentation.
     * @param gid GID, as described in the official documentation.
     * @return True in case of success, false otherwise.
     */
    bool lchownSync(std::string path, Uid uid, Gid gid);

    /**
     * @brief Opens a path asynchronously as a directory stream.
     *
     * Emit a `FsEvent<FsReq::Type::OPENDIR>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     *
     * The contents of the directory can be iterated over by means of the
     * `readdir` od `readdirSync` member functions. The memory allocated by this
     * function must be freed by calling `closedir` or `closedirSync`.
     *
     * @param path The path to open as a directory stream.
     */
    void opendir(std::string path);

    /**
     * @brief Opens a path synchronously as a directory stream.
     *
     * The contents of the directory can be iterated over by means of the
     * `readdir` od `readdirSync` member functions. The memory allocated by this
     * function must be freed by calling `closedir` or `closedirSync`.
     *
     * @param path The path to open as a directory stream.
     * @return True in case of success, false otherwise.
     */
    bool opendirSync(std::string path);

    /**
     * @brief Closes asynchronously a directory stream.
     *
     * Emit a `FsEvent<FsReq::Type::CLOSEDIR>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     *
     * It frees also the memory allocated internally when a path has been opened
     * as a directory stream.
     */
    void closedir();

    /**
     * @brief Closes synchronously a directory stream.
     *
     * It frees also the memory allocated internally when a path has been opened
     * as a directory stream.
     *
     * @return True in case of success, false otherwise.
     */
    bool closedirSync();

    /**
     * @brief Iterates asynchronously over a directory stream one entry at a
     * time.
     *
     * Emit a `FsEvent<FsReq::Type::READDIR>` event when completed.<br/>
     * Emit an ErrorEvent event in case of errors.
     *
     * This function isn't thread safe. Moreover, it doesn't return the `.` and
     * `..` entries.
     */
    void readdir();

    /**
     * @brief Iterates synchronously over a directory stream one entry at a
     * time.
     *
     * Returns a composed value where:
     *
     * * The first parameter indicates the entry type (see below).
     * * The second parameter is a string that contains the actual value.
     *
     * Available entry types are:
     *
     * * `FsReq::EntryType::UNKNOWN`
     * * `FsReq::EntryType::FILE`
     * * `FsReq::EntryType::DIR`
     * * `FsReq::EntryType::LINK`
     * * `FsReq::EntryType::FIFO`
     * * `FsReq::EntryType::SOCKET`
     * * `FsReq::EntryType::CHAR`
     * * `FsReq::EntryType::BLOCK`
     *
     * See the official
     * [documentation](http://docs.libuv.org/en/v1.x/fs.html#c.uv_dirent_t)
     * for further details.
     *
     * This function isn't thread safe. Moreover, it doesn't return the `.` and
     * `..` entries.
     *
     * @return A pair where:
     *
     * * The first parameter is a boolean value that indicates if the current
     * entry is still valid.
     * * The second parameter is a composed value (see above).
     */
    std::pair<bool, std::pair<EntryType, const char *>> readdirSync();

private:
    uv_dirent_t dirents[1];
};


/*! @brief Helper functions. */
struct FsHelper {
    /**
     * @brief Gets the OS dependent handle.
     *
     * For a file descriptor in the C runtime, get the OS-dependent handle. On
     * UNIX, returns the file descriptor as-is. On Windows, this calls a system
     * function.<br/>
     * Note that the return value is still owned by the C runtime, any attempts
     * to close it or to use it after closing the file descriptor may lead to
     * malfunction.
     */
    static OSFileDescriptor handle(FileHandle file) noexcept;

    /**
     * @brief Gets the file descriptor.
     *
     * For a OS-dependent handle, get the file descriptor in the C runtime. On
     * UNIX, returns the file descriptor as-is. On Windows, this calls a system
     * function.<br/>
     * Note that the return value is still owned by the C runtime, any attempts
     * to close it or to use it after closing the handle may lead to
     * malfunction.
     */
    static FileHandle open(OSFileDescriptor descriptor) noexcept;
};


}


#ifndef UVW_AS_LIB
#include "fs.cpp"
#endif

#endif // UVW_FS_INCLUDE_H
