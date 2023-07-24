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

void checkForRejectedProperties(const TagLib::PropertyMap &tags)
{
    if(tags.size() > 0)
    {
        unsigned int longest = 0;
        for(TagLib::PropertyMap::ConstIterator i = tags.begin(); i != tags.end(); ++i) {
            if(i->first.size() > longest) {
                longest = i->first.size();
            }
        }
        std::cout << "-- rejected TAGs (properties) --" << std::endl;
        for(TagLib::PropertyMap::ConstIterator i = tags.begin(); i != tags.end(); ++i)
            for(TagLib::StringList::ConstIterator j = i->second.begin(); j != i->second.end(); ++j)
                std::cout << std::left << std::setw(longest) << i->first << " - " << '"' << *j << '"' << std::endl;
    }
}

// Additional tags from https://docs.mp3tag.de/mapping/
// https://taglib.org/api/classTagLib_1_1PropertyMap.html
template<typename ValueF, typename ConvF>
bool process_field(TagLib::File* file, TagLib::String propertyName, const std::pair<bool, ValueF>& info, ConvF convert)
{
    auto& [valid, data] = info;
    if (valid)
    {
        TagLib::PropertyMap map = file->properties();
        TagLib::String value = convert(data);
        map.replace(propertyName, value);
        checkForRejectedProperties(file->setProperties(map));
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
    
    TagLib::File* f = file.file();
    if (f == nullptr)
        return false;
    TagLib::Tag& tag = *file.tag();

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
    
    auto itos = [](const int& i) -> std::string
    {
        return std::to_string(i);
    };

    bool processed = false;
    processed |= process_field(tag, &TagLib::Tag::setArtist, args.artist(), utf8string);
    processed |= process_field(tag, &TagLib::Tag::setTitle, args.title(), utf8string);
    processed |= process_field(tag, &TagLib::Tag::setAlbum, args.album(), utf8string);
    processed |= process_field(tag, &TagLib::Tag::setYear, args.year(), itoi);
    processed |= process_field(tag, &TagLib::Tag::setTrack, args.track(), itoi);
    processed |= process_field(tag, &TagLib::Tag::setGenre, args.genre(), utf8string);
    processed |= process_field(tag, &TagLib::Tag::setComment, args.comment(), utf8string);

    processed |= process_field(f, "SUBTITLE", args.work(), utf8string);
    processed |= process_field(f, "ALBUMARTIST", args.albumArtist(), utf8string);
    processed |= process_field(f, "DISCNUMBER", args.discnumber(), itos);
    processed |= process_field(f, "DISCTOTAL", args.disctotal(), itos);
    processed |= process_field(f, "TRACKNUMBER", args.tracknumber(), itos);
    processed |= process_field(f, "TRACKTOTAL", args.tracktotal(), itos);
    processed |= process_field(f, "COMPOSER", args.composer(), utf8string);
    processed |= process_field(f, "COPYRIGHT", args.copyright(), utf8string);
    processed |= process_field(f, "OWNER", args.license(), utf8string);
    processed |= process_field(f, "ENCODEDBY", args.encodedby(), utf8string);
    processed |= process_field(f, "ISRC", args.isrc(), utf8string);

    //processed |= process_field(f, TagField::PICTURE, args.picture(), utf8string);
    //processed |= process_field(f, TagField::TRACKID, args.trackId(), itoi);

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

        TagLib::PropertyMap tags = f->properties();
        
        unsigned int longest = 0;
        for(TagLib::PropertyMap::ConstIterator i = tags.begin(); i != tags.end(); ++i)
            if (i->first.size() > longest)
                longest = i->first.size();
        
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
