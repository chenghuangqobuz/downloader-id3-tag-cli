#pragma once
#include <string>
#include <optional>
#include <exception>

#include <getopt.h>

#include "help.hpp"

class arguments
{
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
        
        app.add_option("--albumArtist,--performer", args.m_albumArtist, "")->group("Tags");
        app.add_option("--composer", args.m_composer, "")->group("Tags");
        app.add_option("--copyright", args.m_copyright, "")->group("Tags");
        app.add_option("--discnumber", args.m_discnumber, "")->group("Tags");
        app.add_option("--disctotal", args.m_disctotal, "")->group("Tags");
        app.add_option("--encodedby", args.m_encodedby, "")->group("Tags");
        app.add_option("--isrc", args.m_isrc, "")->group("Tags");
        app.add_option("--license,--owner", args.m_license, "")->group("Tags");
        app.add_option("-p,--picture", args.m_picture, "Sets Picture contained in file.")->group("Tags")
          ->check(CLI::ExistingFile);
        app.add_option("--trackId", args.m_trackId, "")->group("Tags");
        app.add_option("--tracknumber", args.m_tracknumber, "")->group("Tags");
        app.add_option("--tracktotal", args.m_tracktotal, "")->group("Tags");
        app.add_option("--work,--subtitle", args.m_work, "")->group("Tags");

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
    
    std::pair<bool, std::string> work() const
    {
        return { m_work.has_value(), m_work.value_or("") };
    }

    std::pair<bool, std::string> albumArtist() const
    {
        return { m_albumArtist.has_value(), m_albumArtist.value_or("") };
    }

    std::pair<bool, std::string> composer() const
    {
        return { m_composer.has_value(), m_composer.value_or("") };
    }

    std::pair<bool, std::string> copyright() const
    {
        return { m_copyright.has_value(), m_copyright.value_or("") };
    }

    std::pair<bool, std::string> license() const
    {
        return { m_license.has_value(), m_license.value_or("") };
    }

    std::pair<bool, std::string> encodedby() const
    {
        return { m_encodedby.has_value(), m_encodedby.value_or("") };
    }

    std::pair<bool, std::string> picture() const
    {
        return { m_picture.has_value(), m_picture.value_or("") };
    }

    std::pair<bool, std::string> isrc() const
    {
        return { m_isrc.has_value(), m_isrc.value_or("") };
    }

    std::pair<bool, int> tracknumber() const
    {
        return { m_tracknumber.has_value(), m_tracknumber.value_or(0) };
    }

    std::pair<bool, int> tracktotal() const
    {
        return { m_tracktotal.has_value(), m_tracktotal.value_or(0) };
    }

    std::pair<bool, int> discnumber() const
    {
        return { m_discnumber.has_value(), m_discnumber.value_or(0) };
    }

    std::pair<bool, int> disctotal() const
    {
        return { m_disctotal.has_value(), m_disctotal.value_or(0) };
    }

    std::pair<bool, int> trackId() const
    {
        return { m_trackId.has_value(), m_trackId.value_or(0) };
    }

private:
    std::string m_file_name;
    std::optional<std::string> m_work;
    std::optional<std::string> m_albumArtist;
    std::optional<std::string> m_artist;
    std::optional<std::string> m_title;
    std::optional<std::string> m_album;
    std::optional<int> m_year;
    std::optional<int> m_track;
    std::optional<std::string> m_genre;
    std::optional<std::string> m_comment;
    std::optional<std::string> m_composer;
    std::optional<std::string> m_copyright;
    std::optional<std::string> m_license;
    std::optional<std::string> m_encodedby;
    std::optional<std::string> m_isrc;
    std::optional<std::string> m_picture;

    std::optional<int> m_tracknumber;
    std::optional<int> m_tracktotal;
    std::optional<int> m_discnumber;
    std::optional<int> m_disctotal;
    std::optional<int> m_trackId;
};
