#include <cassert>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "btree.h"

class KvStore {
public:
    KvStore(const std::string &path) : path_(path) {
        open_or_create();
        rebuild_index();
    }

    void put(const std::string &key, const std::string &value) {
        file_.clear();
        file_.seekp(0, std::ios::end);
        std::uint64_t offset = file_.tellp();
        write_record(key, value);
        index_[key] = offset;
    }

    bool get(const std::string &key, std::string &out_value) {
        auto it = index_.find(key);
        if (it == index_.end()) {
            return false;
        }

        Record record;
        if (!read_record_at(it->second, record)) {
            return false;
        }

        out_value = record.value;
        return true;
    }

    void flush() {
        file_.flush();
    }

private:
    struct Record {
        std::string key;
        std::string value;
    };

    std::string path_;
    std::fstream file_;
    std::unordered_map<std::string, std::uint64_t> index_;

    static constexpr const char *kHeaderMagic = "PURPDB1";
    static constexpr std::uint32_t kMagic = 0x31424450u; // "PDB1"
    static constexpr std::size_t kHeaderSize = 8;

    void open_or_create() {
        file_.open(path_, std::ios::binary | std::ios::in | std::ios::out);
        if (!file_.is_open()) {
            file_.clear();
            std::ofstream created_file(path_, std::ios::binary | std::ios::out | std::ios::trunc);
            if (!created_file.is_open()) {
                throw std::runtime_error("Could not create file");
            }

            write_header(created_file);
            created_file.close();

            file_.clear();
            file_.open(path_, std::ios::binary | std::ios::in | std::ios::out);

            if (!file_.is_open()) {
                throw std::runtime_error("Could not open the created file");
            }
        }
        check_header();
    }


    void write_header(std::ostream &out) {
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

    static void write_u32_le(std::ostream &out, std::uint32_t value) {
        char bytes[4];
        bytes[0] = value & 0xFFu;
        bytes[1] = (value >> 8) & 0xFFu;
        bytes[2] = (value >> 16) & 0xFFu;
        bytes[3] = (value >> 24) & 0xFFu;
        out.write(bytes, 4);
    }

    static bool read_u32_le(std::istream &in, std::uint32_t &value) {
        unsigned char bytes[4];
        in.read(reinterpret_cast<char *>(bytes), 4);
        if (!in) {
            return false;
        }

        value =
                static_cast<std::uint32_t>(bytes[0]) |
                (static_cast<std::uint32_t>(bytes[1]) << 8) |
                (static_cast<std::uint32_t>(bytes[2]) << 16) |
                (static_cast<std::uint32_t>(bytes[3]) << 24);

        return true;
    }

    static std::uint32_t checksum(const std::string &key, const std::string &value) {
        //fnv_1a
        std::uint32_t hash = 2166136261u;
        for (char c: key) {
            hash ^= static_cast<unsigned char>(c);
            hash *= 16777619u;
        }
        for (char c: value) {
            hash ^= static_cast<unsigned char>(c);
            hash *= 16777619u;
        }
        return hash;
    }

    void write_record(const std::string &key, const std::string &value) {
        if (key.size() > std::numeric_limits<std::uint32_t>::max() ||
            value.size() > std::numeric_limits<std::uint32_t>::max()) {
            throw std::runtime_error("Key/value is too large");
        }

        std::uint32_t key_size = static_cast<std::uint32_t>(key.size());
        std::uint32_t value_size = static_cast<std::uint32_t>(value.size());
        std::uint32_t record_checksum = checksum(key, value);

        write_u32_le(file_, kMagic);
        write_u32_le(file_, key_size);
        write_u32_le(file_, value_size);
        write_u32_le(file_, record_checksum);

        if (!key.empty()) {
            file_.write(key.data(), static_cast<std::streamsize>(key.size()));
        }
        if (!value.empty()) {
            file_.write(value.data(), static_cast<std::streamsize>(value.size()));
        }
        if (!file_) {
            throw std::runtime_error("Cant write record");
        }
    }

    bool read_record_at(std::uint64_t offset, Record &out_record) {
        file_.clear();
        file_.seekg(offset, std::ios::beg);

        std::uint32_t magic = 0;
        std::uint32_t key_size = 0;
        std::uint32_t value_size = 0;
        std::uint32_t stored_checksum = 0;

        if (!read_u32_le(file_, magic)) {
            return false;
        }
        if (magic != kMagic) {
            return false;
        }
        if (!read_u32_le(file_, key_size)) {
            return false;
        }
        if (!read_u32_le(file_, value_size)) {
            return false;
        }
        if (!read_u32_le(file_, stored_checksum)) {
            return false;
        }

        std::string key(key_size, '\0');
        std::string value(value_size, '\0');

        if (key_size > 0) {
            file_.read(&key[0], key_size);
            if (!file_) {
                return false;
            }
        }
        if (value_size > 0) {
            file_.read(&value[0], value_size);
            if (!file_) {
                return false;
            }
        }
        if (checksum(key, value) != stored_checksum) {
            return false;
        }
        out_record.key = key;
        out_record.value = value;
        return true;
    }

    void rebuild_index() {
        index_.clear();

        file_.clear();
        file_.seekg(kHeaderSize, std::ios::beg);

        while (true) {
            std::uint64_t offset = static_cast<std::uint64_t>(file_.tellg());

            Record record;
            if (!read_record_at(offset, record)) {
                break;
            }
            index_[record.key] = offset;

            std::uint64_t next_offset =
                    offset +
                    4 + // magic
                    4 + // key_size
                    4 + // value_size
                    4 + // checksum
                    record.key.size() +
                    record.value.size();

            file_.clear();
            file_.seekg(next_offset, std::ios::beg);
        }
        file_.clear();
    }
};


int main() {
    // const std::string path = "/home/alexandervinarsky/Projects/PurpleDBMS/PurpleDBMS/demo.pdb";
    // std::remove(path.c_str());
    // {
    //     KvStore db(path);
    //     db.put("test1", "symbol:1");
    //     db.put("test67", "symbol:67");
    //     db.flush();
    // }
    //
    // {
    //     KvStore db(path);
    //
    //     std::string main_symbol;
    //     std::string print_symbol;
    //     std::string missing_symbol;
    //
    //     if (db.get("test1", main_symbol)) {
    //         std::cout << "test1: " << main_symbol << "\n";
    //     }
    //     if (db.get("test67", print_symbol)) {
    //         std::cout << "test67: " << print_symbol << "\n";
    //     }
    // }
    //
    // return 0;
    BTree tree(2);
    tree.insert(10);
    assert(tree.contains(10));
    assert(!tree.contains(67));

    tree.insert(67);
    tree.insert(5);

    assert(tree.contains(10));
    assert(tree.contains(67));
    assert(tree.contains(5));
    assert(!tree.contains(100));

    tree.print();

    return 0;
}
