#include <alsongpp/alsong.h>
#include <alsongpp/boost/split.hpp>

#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <date/tz.h>
#include <httplib.h>
#include <mbedcrypto/rsa.hpp>
#include <mbedtls/bignum.h>
#include <simdjson.h>

#include <algorithm>
#include <regex>

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

        simdjson::ondemand::parser parser;
        simdjson::padded_string json = result;
        auto json_result = parser.iterate(json);
        auto arr = json_result.get_array();

        std::vector<SearchResult> searchResults;
        for (auto element : arr) {
            SearchResult search_result{
                    .lyric_id = element["lyric_id"].get_int64(),
                    .playtime = element["playtime"].get_int64(),
                    .artist = to_string(element["artist"]),
                    .album = to_string(element["album"]),
                    .title = to_string(element["title"]),
                    .register_date = to_string(element["register_date"])};
            searchResults.emplace_back(search_result);
        }
        return searchResults;
    }

    std::map<int64_t, std::vector<std::string>> parse_lyric(std::string_view lyric) {
        std::map<int64_t, std::vector<std::string>> result;
        std::vector<std::string> lyric_vec;
        boost::split(lyric_vec, lyric, "<br>");
        static std::regex reg(R"(^\[(\d+):(\d\d).(\d\d)\](.*)$)");
        for (const auto& element : lyric_vec) {
            std::smatch match;

            if (!std::regex_match(element, match, reg)) {
                return result;
            }
            auto timestamp = 10 * std::stoi(match[1]) * 60 * 100 + std::stoi(match[2]) * 100 + std::stoi(match[3]);
            result[timestamp].emplace_back(match[4]);
        }
        return result;
    }

    LyricInfo Alsong::get_lyric_by_id(std::string_view lyric_id) {
        httplib::Client http_client(alsong_base_url);
        http_client.enable_server_certificate_verification(false);

        httplib::MultipartFormDataItems form_data_items;

        httplib::MultipartFormData lyric_id_form{
                .name = "info_id",
        };
        lyric_id_form.content = lyric_id;
        form_data_items.emplace_back(lyric_id_form);

        httplib::MultipartFormData enc_data_form{
                .name = "encData",
                .content = enc_key};
        form_data_items.emplace_back(enc_data_form);
        auto result = http_client.Post("/v1/info", form_data_items)->body;

        simdjson::ondemand::parser parser;
        simdjson::padded_string json = result;
        auto json_result = parser.iterate(json);

        LyricInfo lyric_info{
                .register_url = to_string(json_result["register_url"]),
                .lyrics = parse_lyric(to_string(json_result["lyric"])),
                .album_name = to_string(json_result["album_name"]),
                .info_id = json_result["info_id"].get_int64(),
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

        simdjson::ondemand::parser parser;
        simdjson::padded_string json = result;
        auto json_result = parser.iterate(json);

        LyricLookup lyric_info{
                .register_url = to_string(json_result["register_url"]),
                .lyrics = parse_lyric(to_string(json_result["lyric"])),
                .album_name = to_string(json_result["album_name"]),
                .info_id = json_result["info_id"].get_int64(),
                .count_good = json_result["count_good"].get_int64(),
                .count_bad = json_result["count_bad"].get_int64(),
                .status_id = json_result["status_id"].get_int64(),
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

        simdjson::ondemand::parser parser;
        simdjson::padded_string json = result;
        auto json_result = parser.iterate(json);
        auto arr = json_result.get_array();

        LyricMurekaId lyric_info;
        decltype(lyric_info.lyric) lyrics;
        for (auto element : arr) {
            lyrics.emplace_back(parse_lyric(to_string(element["lyric"])));
        }
        lyric_info.lyric = lyrics;
        return lyric_info;
    }

    std::string set_enc_key() {
        mbedcrypto::rsa rsa;
        rsa.import_public_key(mbedcrypto::buffer_t(R"xx(-----BEGIN RSA PUBLIC KEY-----
MIGJAoGBAN+8Hz9MEOF+ARLXLniRbaUG7dV9oG6sauTwDdMBBnF4BXuqm6lO9uZl
v7Kc7lZ95AgSScC+N2+YETg85tErrXRKLxL8FhicPW7AQSIrRZVBhBZfN9mNGI7V
rRWP+LUATo5xf3FPyWKrfrAtWEgZYNTWLwnAtkLkluxwPsocZTdLAgMBAAE=
-----END RSA PUBLIC KEY-----)xx"));
        auto enc_key = rsa.encrypt(date::format(
                "ALSONG_ANDROID_%4Y%2m%2d_%2H%2M%2S",
                date::make_zoned(
                        date::locate_zone("Etc/UTC"),
                        std::chrono::system_clock::now())));
        std::transform(enc_key.begin(), enc_key.end(), enc_key.begin(), ::toupper);
        return enc_key;
    }

    void Alsong::refresh_enc_key() {
        this->enc_key = set_enc_key();
    }
    Alsong::Alsong() {
        this->enc_key = set_enc_key();
    }
}// namespace alsongpp
