#include <CLI/CLI.hpp>

#include <iostream>
#include <optional>

#include "platform.hpp"

TAGLIB_HEADERS_BEGIN
  #include <taglib/tag.h>
  #include <taglib/fileref.h>
  #include <taglib/tpropertymap.h>
TAGLIB_HEADERS_END

#include "arguments.hpp"
#include "help.hpp"

// SetF is void(TagLib::Tag::* setter)(T), for some T.
// ConvF is T(*convert)(std::string) for SetF's T.
template<typename SetF, typename ValueF, typename ConvF>
bool process_field(TagLib::Tag& tag, SetF setter, const std::pair<bool, ValueF>& info, ConvF convert)
{
    auto& [valid, data] = info;
    if (valid)
    {
        (tag.*setter)(convert(data));
        return true;
    }
    return false;
}

template<typename T>
void print_field(const std::string& field_name, const T& field_value)
{
    std::cout << field_name << ": " << field_value << std::endl;
}

bool process_file(arguments&& args)
{
    platform::string file_name = args.file_name();
    TagLib::FileRef file(file_name.c_str());
    
    if (file.isNull())
        return false;
    
    TagLib::Tag& tag = *file.tag();

    auto [artist_valid, artist] = args.artist();
    auto [title_valid, title] = args.title();
    auto [album_valid, album] = args.album();
    auto [year_valid, year] = args.year();
    auto [track_valid, track] = args.track();
    auto [genre_valid, genre] = args.genre();
    auto [comment_valid, comment] = args.comment();

    auto utf8string = [](const std::string& str) -> TagLib::String
    {
        if (str.empty())
            return TagLib::String();
        return TagLib::String(str, TagLib::String::Type::UTF8);
    };

    auto itoi = [](const int& i)
    {
        return i;
    };

    bool processed = false;
    processed |= process_field(tag, &TagLib::Tag::setArtist, args.artist(), utf8string);
    processed |= process_field(tag, &TagLib::Tag::setTitle, args.title(), utf8string);
    processed |= process_field(tag, &TagLib::Tag::setAlbum, args.album(), utf8string);
    processed |= process_field(tag, &TagLib::Tag::setYear, args.year(), itoi);
    processed |= process_field(tag, &TagLib::Tag::setTrack, args.track(), itoi);
    processed |= process_field(tag, &TagLib::Tag::setGenre, args.genre(), utf8string);
    processed |= process_field(tag, &TagLib::Tag::setComment, args.comment(), utf8string);

    if (processed)
    {
        file.save();
        return true;
    }

    if (!file.isNull() && file.tag())
    {
        std::cout << "Information for file " << file_name << std::endl;
        print_field(" Artist", tag.artist().to8Bit(true));
        print_field("  Title", tag.title().to8Bit(true));
        print_field("  Album", tag.album().to8Bit(true));
        print_field("   Year", tag.year());
        print_field("  Track", tag.track());
        print_field("  Genre", tag.genre().to8Bit(true));
        print_field("Comment", tag.comment().to8Bit(true));

        TagLib::PropertyMap tags = file.file()->properties();
        
        unsigned int longest = 0;
        for(TagLib::PropertyMap::ConstIterator i = tags.begin(); i != tags.end(); ++i)
        {
            if (i->first.size() > longest)
            {
                longest = i->first.size();
            }
        }
        
        std::cout << "-- TAG (properties) --" << std::endl;
        for(TagLib::PropertyMap::ConstIterator i = tags.begin(); i != tags.end(); ++i)
            for(TagLib::StringList::ConstIterator j = i->second.begin(); j != i->second.end(); ++j)
                std::cout << std::left << std::setw(longest) << i->first << " - " << '"' << j->to8Bit(true) << '"' << std::endl;
    }

    if (!file.isNull() && file.audioProperties())
    {
        TagLib::AudioProperties *properties = file.audioProperties();
        
        int seconds = properties->length() % 60;
        int minutes = (properties->length() - seconds) / 60;
        
        std::cout << "-- AUDIO --" << std::endl;
        std::cout << "bitrate     - " << properties->bitrate() << std::endl;
        std::cout << "sample rate - " << properties->sampleRate() << std::endl;
        std::cout << "channels    - " << properties->channels() << std::endl;
        std::cout << "length      - " << minutes << ":" << std::setfill('0') << std::setw(2) << seconds << std::endl;
    }

    return true;
}

constexpr static const int RETURN_OK = 0;
constexpr static const int RETURN_ERROR = 1;

int MAIN(int argc, platform::char_t* argv[])
{
    CLI::App app("id3-tag-cli");
    app.footer("If no argument is specified, information of given file is retrieved.\n"
               "If the option is not specified, the value is unchanged.\n"
               "If the argument is empty string (\"\") (for [STR]) or 0 (for [INT]) the value is cleared.\n"
               "\n"
               "Written by Zereges <https://github.com/Zereges/id3-tag-cli>");
    
    arguments args;
    try
    {
        arguments::parse_args(app, argc, argv, args);
    }
    catch (const CLI::Error& ex)
    {
        std::cerr << CLI::FailureMessage::help(&app, ex);
        return RETURN_ERROR;
    }
    
    CLI11_PARSE(app, argc, argv);
    
    if (app.get_option("--print")->as<bool>())
    {
        std::cout << app.config_to_str(true, true);
        return RETURN_OK;
    }

    if (!process_file(std::move(args)))
    {
        std::cerr << "Couldn't process given file" << std::endl;
        std::cerr << app.help();
        return RETURN_ERROR;
    }
}
