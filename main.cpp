#include <cstdint>
#include <fstream>
#include <string>
#include <unordered_map>

class KvStore {

public:
    KvStore (const std::string& path) : path_(path) {
        open_or_create();
    }

    void put(const std::string& key, const std::string& value) {
        file_.clear();
        file_.seekp(0, std::ios::end);
        std::uint64_t offset = static_cast<std::uint64_t>(file_.tellp());


    }

    bool get(const std::string& key, std::string& out_value) {
        auto it = index_.find(key);
        if (it == index_.end()) {return false;}
        Record record;

        out_value = record.value;
        return true;
    }




private:
    struct Record {
        std::string key;
        std::string value;
    };

    std::string path_;
    std::fstream file_;
    std::unordered_map<std::string, std::uint64_t> index_;

    static constexpr const char* kHeaderMagic = "PURPDB1";
    static constexpr std::size_t kHeaderSize = 8;

    void open_or_create() {
        file_.open(path_, std::ios::in | std::ios::binary | std::ios::out);
        if (!file_.is_open()) {
            std::ofstream created_file(path_, std::ios::in | std::ios::binary | std::ios::out);
            if (!created_file.is_open()) {
                throw std::runtime_error("Could not create file");
            }

            write_header(created_file);
            created_file.close();

            file_.open(path_, std::ios::out | std::ios::binary);
            if (!created_file.is_open()) {
                throw std::runtime_error("Could not open the created file");
            }
        }
        check_header();
    }


    void write_header(std::ostream& out) {
        char header[kHeaderSize] = {};

        for (std::size_t i = 0; kHeaderMagic[i] != '\0'; ++i) {
            header[i] = kHeaderMagic[i];
        }

        out.write(header, kHeaderSize);
    }

    void check_header() {
        file_.clear();
        file_.seekg(0, std::ios::beg);

        char header[kHeaderSize] = {};
        file_.read(header, kHeaderSize);

        std::string actual(header, header + 7);
        std::string expected(kHeaderMagic);

        if (actual != expected) {
            throw std::runtime_error("Expected head" + expected + " got" + actual);
        }
    }
};

int main() {
    return 0;
}