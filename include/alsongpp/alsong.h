#ifndef ALSONG_CPP_LIBRARY_H
#define ALSONG_CPP_LIBRARY_H

#include <string>
#include <string_view>
#include <map>
#include <vector>
#include <cstdint>

namespace alsongpp {
    struct SearchResult {
        int64_t lyric_id;
        int64_t playtime;
        std::string artist;
        std::string album;
        std::string title;
        std::string register_date;
    };
    struct LyricInfo {
        std::string register_url;
        std::map<int64_t, std::vector<std::string>> lyrics;
        std::string album_name;
        int64_t info_id;
        std::string register_first_email;
        std::string register_comment;
        std::string register_name;
        std::string register_first_name;
        std::string register_first_url;
        std::string register_phone;
        std::string artist_name;
        std::string register_first_phone;
        std::string title_name;
        std::string register_first_comment;
        std::string register_email;
    };

    struct LyricLookup {
        std::string register_url;
        std::map<int64_t, std::vector<std::string>> lyrics;
        std::string album_name;
        int64_t info_id;
        int64_t count_good;
        int64_t count_bad;
        int64_t status_id;
        std::string register_first_email;
        std::string register_comment;
        std::string register_name;
        std::string register_first_name;
        std::string register_first_url;
        std::string regist_date;
        std::string register_phone;
        std::string register_first_phone;
        std::string artist;
        std::string title;
        std::string register_first_comment;
        std::string register_email;
    };

    struct LyricMurekaId {
        std::vector<std::map<int64_t, std::vector<std::string>>> lyric;
    };

    class Alsong {
        Alsong();
        void refresh_enc_key();
        std::vector<SearchResult> get_resemble_lyric_list(std::string_view artist, std::string_view title, int32_t playtime = 0, int32_t page = 1);
        LyricInfo get_lyric_by_id(std::string_view lyric_id);
        LyricLookup get_lyric_by_hash(std::string_view md5);
        LyricMurekaId get_lyric_by_mureka_id(int64_t mureka_id);
    private:
        std::string enc_key;
    };
}; // namespace alsongpp


#endif//ALSONG_CPP_LIBRARY_H
