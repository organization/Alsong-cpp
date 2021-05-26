#include <alsongpp/alsong.h>

#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <openssl/bn.h>
#include <openssl/rsa.h>

#include <date/tz.h>
#include <httplib.h>
#include <nlohmann/json.hpp>

#include <algorithm>
#include <regex>

#define ALKEY "dfbc1f3f4c10e17e0112d72e78916da506edd57da06eac6ae4f00dd301067178"\
              "057baa9ba94ef6e665bfb29cee567de4081249c0be376f9811383ce6d12bad74"\
              "4a2f12fc16189c3d6ec041222b45954184165f37d98d188ed5ad158ff8b5004e"\
              "8e717f714fc962ab7eb02d58481960d4d62f09c0b642e496ec703eca1c65374b"

namespace alsongpp {
    constexpr auto alsong_base_url = "https://lyric.altools.com";
    std::vector<SearchResult> Alsong::get_resemble_lyric_list(
            std::string_view artist,
            std::string_view title,
            int32_t playtime,
            int32_t page) {
        httplib::Client http_client(alsong_base_url);
        http_client.enable_server_certificate_verification(false);

        httplib::MultipartFormDataItems form_data_items;

        httplib::MultipartFormData title_form;
        title_form.name = "title";
        title_form.content = title;
        form_data_items.emplace_back(title_form);

        httplib::MultipartFormData artist_form;
        artist_form.name = "artist";
        artist_form.content = artist;
        form_data_items.emplace_back(artist_form);

        if (playtime != 0) {
            httplib::MultipartFormData playtime_form{
                    .name = "playtime",
                    .content = std::to_string(playtime)};
            form_data_items.emplace_back(playtime_form);
        }

        httplib::MultipartFormData page_form{
                .name = "page",
                .content = std::to_string(page)};
        form_data_items.emplace_back(page_form);

        httplib::MultipartFormData enc_data_form{
                .name = "encData",
                .content = enc_key};
        form_data_items.emplace_back(enc_data_form);
        auto result = http_client.Post("/v1/search", form_data_items)->body;

        nlohmann::json json = nlohmann::json::parse(result);

        std::vector<SearchResult> searchResults;
        for (auto element : json) {
            SearchResult search_result{
                    .lyric_id = element["lyric_id"].get<int64_t>(),
                    .playtime = element["playtime"].get<int64_t>(),
                    .artist = to_string(element["artist"]),
                    .album = to_string(element["album"]),
                    .title = to_string(element["title"]),
                    .register_date = to_string(element["register_date"])};
            searchResults.emplace_back(search_result);
        }
        return searchResults;
    }

    std::map<int64_t, std::vector<std::string>> parse_lyric(std::string lyric) {
        std::map<int64_t, std::vector<std::string>> result;
        size_t pos;
        std::vector<std::string> lyric_vec;
        std::string del = "<br>";
        while ((pos = lyric.find(del)) != std::string::npos) {
            lyric_vec.emplace_back(lyric.substr(0, pos));
            lyric.erase(0, pos + del.length());
        }
        static std::regex reg(R"(^(.)?\[(\d+):(\d\d).(\d\d)\](.*)$)");
        for (const auto& element : lyric_vec) {
            std::smatch match;

            if (!std::regex_match(element, match, reg)) {
                return result;
            }
            auto timestamp = 10 * std::stoi(match[2]) * 60 * 100 + std::stoi(match[3]) * 100 + std::stoi(match[4]);
            result[timestamp].emplace_back(match[5]);
        }
        return result;
    }

    LyricInfo Alsong::get_lyric_by_id(int64_t lyric_id) {
        httplib::Client http_client(alsong_base_url);
        http_client.enable_server_certificate_verification(false);

        httplib::MultipartFormDataItems form_data_items;

        httplib::MultipartFormData lyric_id_form{
                .name = "info_id",
        };
        lyric_id_form.content = std::to_string(lyric_id);
        form_data_items.emplace_back(lyric_id_form);

        httplib::MultipartFormData enc_data_form{
                .name = "encData",
                .content = enc_key};
        form_data_items.emplace_back(enc_data_form);
        auto result = http_client.Post("/v1/info", form_data_items)->body;

        nlohmann::json json_result = nlohmann::json::parse(result);

        LyricInfo lyric_info{
                .register_url = to_string(json_result["register_url"]),
                .lyrics = parse_lyric(to_string(json_result["lyric"])),
                .album_name = to_string(json_result["album_name"]),
                .info_id = json_result["info_id"].get<int64_t>(),
                .register_first_email = to_string(json_result["register_first_email"]),
                .register_comment = to_string(json_result["register_comment"]),
                .register_name = to_string(json_result["register_name"]),
                .register_first_name = to_string(json_result["register_first_name"]),
                .register_first_url = to_string(json_result["register_first_url"]),
                .register_phone = to_string(json_result["register_phone"]),
                .register_first_phone = to_string(json_result["register_first_phone"]),
                .title_name = to_string(json_result["title_name"]),
                .register_first_comment = to_string(json_result["register_first_comment"]),
                .register_email = to_string(json_result["register_email"]),
        };

        return lyric_info;
    }
    LyricLookup Alsong::get_lyric_by_hash(std::string_view md5) {
        httplib::Client http_client(alsong_base_url);
        http_client.enable_server_certificate_verification(false);

        httplib::MultipartFormDataItems form_data_items;

        httplib::MultipartFormData md5_form{
                .name = "md5",
        };
        md5_form.content = md5;
        form_data_items.emplace_back(md5_form);

        httplib::MultipartFormData enc_data_form{
                .name = "encData",
                .content = enc_key};
        form_data_items.emplace_back(enc_data_form);
        auto result = http_client.Post("/v1/lookup", form_data_items)->body;

        nlohmann::json json_result = nlohmann::json::parse(result);

        LyricLookup lyric_info{
                .register_url = to_string(json_result["register_url"]),
                .lyrics = parse_lyric(to_string(json_result["lyric"])),
                .album_name = to_string(json_result["album_name"]),
                .info_id = json_result["info_id"].get<int64_t>(),
                .count_good = json_result["count_good"].get<int64_t>(),
                .count_bad = json_result["count_bad"].get<int64_t>(),
                .status_id = json_result["status_id"].get<int64_t>(),
                .register_first_email = to_string(json_result["register_first_email"]),
                .register_comment = to_string(json_result["register_comment"]),
                .register_name = to_string(json_result["register_name"]),
                .register_first_name = to_string(json_result["register_first_name"]),
                .register_first_url = to_string(json_result["register_first_url"]),
                .regist_date = to_string(json_result["regist_date"]),
                .register_phone = to_string(json_result["register_phone"]),
                .register_first_phone = to_string(json_result["register_first_phone"]),
                .artist = to_string(json_result["artist"]),
                .title = to_string(json_result["title"]),
                .register_first_comment = to_string(json_result["register_first_comment"]),
                .register_email = to_string(json_result["register_email"]),
        };

        return lyric_info;
    }
    LyricMurekaId Alsong::get_lyric_by_mureka_id(int64_t mureka_id) {
        httplib::Client http_client(alsong_base_url);
        http_client.enable_server_certificate_verification(false);

        httplib::MultipartFormDataItems form_data_items;

        httplib::MultipartFormData mureka_id_form{
                .name = "murekaid",
                .content = std::to_string(mureka_id)};
        form_data_items.emplace_back(mureka_id_form);
        auto result = http_client.Post("/v1/lookupListByMurekaId", form_data_items)->body;

        nlohmann::json json_result = nlohmann::json::parse(result);

        LyricMurekaId lyric_info;
        decltype(lyric_info.lyric) lyrics;
        for (auto element : json_result) {
            lyrics.emplace_back(parse_lyric(to_string(element["lyric"])));
        }
        lyric_info.lyric = lyrics;
        return lyric_info;
    }

    int constexpr length(const char* str) {
        return *str ? 1 + length(str + 1) : 0;
    }

    std::string set_enc_key() {
        std::string enc_key;
        auto rsa = RSA_new();
        if (rsa) {
            BIGNUM* bnN = nullptr;
            BIGNUM* bnE = nullptr;
            BN_hex2bn(&bnN, ALKEY);
            BN_hex2bn(&bnE, "010001");
            RSA_set0_key(rsa, bnN, bnE, nullptr);
            auto data = date::format(
                    "ALSONG_ANDROID_%Y%m%d_%H%M%OS",
                    std::chrono::system_clock::now()
            );

            auto len = RSA_size(rsa);
            auto encrypted = new unsigned char[len];

            RSA_public_encrypt(
                    static_cast<int>(data.length()),
                    reinterpret_cast<const unsigned char *>(data.c_str()),
                    encrypted,
                    rsa,
                    RSA_PKCS1_PADDING
            );
            enc_key = std::string(reinterpret_cast<const char *>(encrypted), len);
            std::transform(enc_key.begin(), enc_key.end(), enc_key.begin(), ::toupper);
            delete[] encrypted;
            RSA_free(rsa);
        }
        return enc_key;
    }

    void Alsong::refresh_enc_key() {
        this->enc_key = set_enc_key();
    }
    Alsong::Alsong() {
        this->enc_key = set_enc_key();
    }
}// namespace alsongpp
