#pragma once

#include <boost/fusion/tuple.hpp>
#include <boost/phoenix.hpp>
#include <catch2/catch.hpp>
#include <boost/fusion/adapted/struct.hpp>
#include <boost/utility/string_view.hpp>
#include <chrono>

namespace event
{
    struct TimeStamp
    {
        std::uint16_t year;
        std::uint8_t month;
        std::uint8_t day;
        std::uint8_t hour;
        std::uint8_t minutes;
        std::uint8_t second;
        char padding_0;
        std::uint16_t time_zone;

        bool operator ==( const TimeStamp &other ) const
        {
            return     this->year == other.year
                    && this->month == other.month
                    && this->day == other.day
                    && this->hour == other.hour
                    && this->minutes == other.minutes
                    && this->second == other.second
                    && this->time_zone == other.time_zone;
        }

        std::chrono::system_clock::time_point to_chrono() const
        {
            std::tm tm;

            tm.tm_year = year;
            tm.tm_mon = month;
            tm.tm_mday = day;
            tm.tm_hour = hour;
            tm.tm_min = minutes;
            tm.tm_sec = second;

            const std::time_t time = std::mktime(&tm);

            return std::chrono::system_clock::from_time_t(time);
        }
    };

    struct Param
    {
        std::string key;
        std::string value;
    };

    struct Id
    {
        std::string  sd_id;
        std::uint32_t iana_id;
    };

    struct StructuredData
    {
        typedef Id id_t;
        typedef Param param_t;
        typedef std::vector<Param> params_t;

        id_t id;
        params_t params;
    };

    struct Message
    {
        using string_t = std::string;
        using time_stamp_t = TimeStamp;
        using param_t = Param;
        using structured_data_t = StructuredData;
        using structured_data_list_t = std::vector<structured_data_t>;

        std::uint8_t priority;
        std::uint8_t version;

        time_stamp_t timestamp;
        char padding_0[4];
        structured_data_list_t structured_data;

        string_t host_name;
        string_t app_name;
        string_t proc_id;
        string_t msg_id;
        string_t message;

        std::uint16_t get_facility( ) const
        {
            return priority / 8;
        }

        std::uint16_t get_severity( ) const
        {
            return priority % 8;
        }
    };

    inline std::ostream& operator<<( std::ostream &out, const Param &value )
    {
        return out << "{key: " << value.key << " value: " << value.value << "}";
    }

    inline std::ostream& operator<<( std::ostream &out, const std::vector<Param> &value )
    {
        out << "{";
        for ( const auto &item: value )
        {
            out << item;
        }
        return out << "}";
    }

    inline std::ostream& operator<<( std::ostream &out, const StructuredData &value )
    {
        return out  << " sd-id: " << value.id.sd_id
                    << " IANA: " << value.id.iana_id
                    << " params: [" << value.params << "]";
    }

    inline std::ostream& operator<<( std::ostream &out, const std::vector<StructuredData> &data )
    {
        out << "{";
        for ( const auto &item: data )
        {
            out << item;
        }
        return out << "}";
    }

    inline std::ostream& operator<<( std::ostream &out, const TimeStamp &value )
    {
        return out << std::setw(4) << std::setfill('0') << value.year
                   << '-'
                   << std::setw(2) << std::setfill('0') << static_cast<int>(value.month)
                   << '-'
                   << std::setw(2) << std::setfill('0') << static_cast<int>(value.day)
                   << 'T'
                   << std::setw(2) << std::setfill('0') << static_cast<int>(value.hour)
                   << ':'
                   << std::setw(2) << std::setfill('0') << static_cast<int>(value.minutes)
                   << ':'
                   << std::setw(2) << std::setfill('0') << static_cast<int>(value.second)
                   << '.'
                   << std::setw(3) << std::setfill('0') << value.time_zone
                   << 'Z';
    }

    inline std::ostream& operator<<( std::ostream &out, const Message &value )
    {
        return out  << "priority: " << static_cast<int>(value.priority) << std::endl
                    << "version: " << static_cast<int>(value.version) << std::endl
                    << "timestamp: " << value.timestamp << std::endl
                    << "host_name: " << value.host_name << std::endl
                    << "app_name: " << value.app_name << std::endl
                    << "proc_id: " << value.proc_id << std::endl
                    << "msg_id:" << value.msg_id << std::endl
                    << "structured_data: [" << value.structured_data << "]" << std::endl
                    << "message: " << value.message << std::endl;
    }

    template<typename T>
    inline std::string to_string( const T &message )
    {
        std::stringstream stream;
        stream << message;
        return stream.str();
    }
}


BOOST_FUSION_ADAPT_STRUCT(  event::TimeStamp
                            , year
                            , month
                            , day
                            , hour
                            , minutes
                            , second
                            , time_zone
                            )

BOOST_FUSION_ADAPT_STRUCT(  event::Param
                            , key
                            , value
                            )

BOOST_FUSION_ADAPT_STRUCT(  event::Id
                            , sd_id
                            , iana_id
                            )

BOOST_FUSION_ADAPT_STRUCT(  event::StructuredData
                            , id
                            , params
                            )

BOOST_FUSION_ADAPT_STRUCT(  event::Message
                            , priority
                            , version
                            , timestamp
                            , host_name
                            , app_name
                            , proc_id
                            , msg_id
                            , structured_data
                            , message
                            )

