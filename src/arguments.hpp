#pragma once
#include <string>
#include <optional>
#include <exception>

#include <getopt.h>

#include "help.hpp"

struct arguments_parse_exception final : std::runtime_error
{
    arguments_parse_exception()
        : runtime_error("")
    {
    }

    explicit arguments_parse_exception(const std::string& msg)
        : runtime_error(msg)
    {
    }

    explicit arguments_parse_exception(const char* const msg)
        : runtime_error(msg)
    {
    }
};

class arguments
{
    static bool is_valid_int(const std::string& str)
    {
        try
        {
            return std::stoi(optarg) >= 0;
        }
        catch (const std::invalid_argument&)
        {
            return false;
        }
    }

public:
    static void parse_args(CLI::App& app, int argc, char** argv, arguments& args)
    {
        app.set_config("--config");

        app.add_flag("--print", "Print configuration and exit")->configurable(false);
        std::function< std::string()> vfunc = std::bind(print_version_info);
        app.set_version_flag("--version", vfunc);

        app.add_option("-a,--artist", args.m_artist,   "Sets the Artist/Performer.")->group("Tags");
        app.add_option("-t,--title", args.m_title,     "Sets the Title/Song name/Content.")->group("Tags");
        app.add_option("-A,--album", args.m_album,     "Sets the Album/Movie/Show title.")->group("Tags");
        app.add_option("-y,--year", args.m_year,       "Sets the Year.")->group("Tags");
        app.add_option("-T,--track", args.m_track,     "Sets the Track number/Position.")->group("Tags");
        app.add_option("-g,--genre", args.m_genre,     "Sets the Genre.")->group("Tags");
        app.add_option("-c,--comment", args.m_comment, "Sets the Description/Comment.")->group("Tags");

        //app.add_option("-p,--picture", args.picture_filename, "Sets Picture contained in file.")->group("Tags")
        //  ->check(CLI::ExistingFile);

        app.add_option("file", args.m_file_name,       "File to process")
          ->check(CLI::ExistingFile);

        return args;
    }

    const std::string& file_name() const
    {
        return m_file_name;
    }

    std::pair<bool, std::string> artist() const
    {
        return { m_artist.has_value(), m_artist.value_or("") };
    }

    std::pair<bool, std::string> title() const
    {
        return { m_title.has_value(), m_title.value_or("") };
    }

    std::pair<bool, std::string> album() const
    {
        return { m_album.has_value(), m_album.value_or("") };
    }

    std::pair<bool, int> year() const
    {
        return { m_year.has_value(), m_year.value_or(0) };
    }

    std::pair<bool, int> track() const
    {
        return { m_track.has_value(), m_track.value_or(0) };
    }

    std::pair<bool, std::string> genre() const
    {
        return { m_genre.has_value(), m_genre.value_or("") };
    }

    std::pair<bool, std::string> comment() const
    {
        return { m_comment.has_value(), m_comment.value_or("") };
    }

private:
    std::string m_file_name;
    std::optional<std::string> m_artist;
    std::optional<std::string> m_title;
    std::optional<std::string> m_album;
    std::optional<int> m_year;
    std::optional<int> m_track;
    std::optional<std::string> m_genre;
    std::optional<std::string> m_comment;
};
