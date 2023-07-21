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
template<typename SetF, typename ConvF>
bool process_field(TagLib::Tag& tag, SetF setter, const std::pair<bool, std::string>& info, ConvF convert)
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
    platform::string file_name = platform::convert::to_platform(args.file_name());
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

    // ReSharper disable once IdentifierTypo
    auto stoi = [](const std::string& str)
    {
        return std::stoi(str); // overload
    };

    bool processed = false;
    processed |= process_field(tag, &TagLib::Tag::setArtist, args.artist(), utf8string);
    processed |= process_field(tag, &TagLib::Tag::setTitle, args.title(), utf8string);
    processed |= process_field(tag, &TagLib::Tag::setAlbum, args.album(), utf8string);
    processed |= process_field(tag, &TagLib::Tag::setYear, args.year(), stoi);
    processed |= process_field(tag, &TagLib::Tag::setTrack, args.track(), stoi);
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
    std::string exe_name = platform::convert::from_platform(argv[0]);

    std::optional<arguments> args;
    try
    {
        auto arg_vec = platform::convert::convert_args(argc, argv);
        args = arguments::parse_args(argc, arg_vec);
    }
    catch (const arguments_parse_exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        print_usage(std::cerr, exe_name);
        return RETURN_ERROR;
    }

    if (args->is_help())
    {
        print_help(exe_name);
        return RETURN_OK;
    }
    if (args->is_version())
    {
        print_version_info();
        return RETURN_OK;
    }

    if (!process_file(std::move(args.value())))
    {
        std::cerr << "Couldn't process given file" << std::endl;
        print_usage(std::cerr, exe_name);
        return RETURN_ERROR;
    }
}
