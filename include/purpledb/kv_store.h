#include <cstdint>
#include <cstddef>
#include <fstream>
#include <string>
#include <unordered_map>

class KvStore {
public:
    explicit KvStore(const std::string& path);
    void put(const std::string& key, const std::string& value);
    bool get(const std::string& key, std::string& out_value);
    void flush();
    std::size_t size() const;
    bool empty() const;

private:
    struct Record {
        std::string key;
        std::string value;
    };

    std::string path_;
    std::fstream file_;
    std::unordered_map<std::string, std::uint64_t> index_;

    static constexpr const char* kHeaderMagic = "PURPDB1";
    static constexpr std::uint32_t kMagic = 0x31424450u;
    static constexpr std::size_t kHeaderSize = 8;
    void open_or_create();
    void write_header(std::ostream& out);
    void check_header();
    static void write_u32_le(std::ostream& out, std::uint32_t value);
    static bool read_u32_le(std::istream& in, std::uint32_t& value);
    static std::uint32_t checksum(const std::string& key, const std::string& value);
    void write_record(const std::string& key, const std::string& value);
    bool read_record_at(std::uint64_t offset, Record& out_record);
    void rebuild_index();
};