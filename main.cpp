#define CATCH_CONFIG_MAIN

//#define BOOST_SPIRIT_DEBUG

#include <iostream>

#include <boost/spirit/include/qi_lexeme.hpp>
#include <boost/spirit/include/qi_match.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/tuple.hpp>
#include <boost/phoenix.hpp>
#include <catch2/catch.hpp>
#include <boost/fusion/adapted/struct.hpp>
#include <boost/utility/string_view.hpp>

#include <chrono>

namespace grammar
{
    namespace syslog
    {
        template<typename Iterator, typename T>
        class definitions : public boost::spirit::qi::grammar<Iterator, T()>
        {
            typedef T message_t;

            typedef typename message_t::string_t string_t;
            typedef typename message_t::time_stamp_t time_stamp_t;
            typedef typename message_t::structured_data_t structured_data_t;
            typedef typename message_t::structured_data_list_t structured_data_list_t;
            typedef typename structured_data_t::param_t param_t;
            typedef typename structured_data_t::params_t param_list_t;
            typedef typename structured_data_t::id_t id_t;

            /*
             *  separar o código em arquivos distintos
             *  criar dubprojeto para testes unitários
             *  criar subporjeto para benchmark
             *  avaliar a possiblilidade de fazer uma análize da quantidade de testes unitários possíveis e determinar quais são viaveis de serem feitos
            */

            using skipper_t = boost::spirit::qi::space_type;

            boost::spirit::qi::rule<Iterator, message_t()> m_start;
            boost::spirit::qi::rule<Iterator, std::uint16_t()> m_priority;
            boost::spirit::qi::rule<Iterator, std::uint16_t()> m_version;

            boost::spirit::qi::rule<Iterator, std::uint16_t()> m_yyyy;
            boost::spirit::qi::rule<Iterator, std::uint8_t()> m_mon;
            boost::spirit::qi::rule<Iterator, std::uint8_t()> m_dd;
            boost::spirit::qi::rule<Iterator, std::uint8_t()> m_hh;
            boost::spirit::qi::rule<Iterator, std::uint8_t()> m_mm;
            boost::spirit::qi::rule<Iterator, std::uint8_t()> m_ss;
            boost::spirit::qi::rule<Iterator, std::uint16_t()> m_fff;
            boost::spirit::qi::rule<Iterator, time_stamp_t()> m_timestamp;

            boost::spirit::qi::rule<Iterator, string_t()> m_host_name;
            boost::spirit::qi::rule<Iterator, string_t()> m_app_name;
            boost::spirit::qi::rule<Iterator, string_t()> m_proc_id;
            boost::spirit::qi::rule<Iterator, string_t()> m_msg_id;

            boost::spirit::qi::rule<Iterator, string_t()> m_param_key;
            boost::spirit::qi::rule<Iterator, string_t()> m_param_value;
            boost::spirit::qi::rule<Iterator, param_t()> m_param;
            boost::spirit::qi::rule<Iterator, param_list_t()> m_param_list;

            boost::spirit::qi::rule<Iterator, id_t()> m_structured_data_id;
            boost::spirit::qi::rule<Iterator, structured_data_t()> m_structured_data;

            boost::spirit::qi::rule<Iterator, structured_data_list_t()> m_structured_data_list;

            boost::spirit::qi::rule<Iterator, string_t()> m_msg;
        public:
            definitions()
                : definitions::base_type(m_start)
            {
                using namespace boost::spirit::qi;

                using field_2_t = boost::spirit::qi::uint_parser<std::uint8_t, 10, 2, 2>;
                using field_3_t = boost::spirit::qi::uint_parser<std::uint16_t, 10, 3, 3>;
                using field_4_t = boost::spirit::qi::uint_parser<std::uint16_t, 10, 4, 4>;

                m_priority = int_;
                m_version = int_;

                m_yyyy = field_4_t();
                m_mon  = field_2_t();
                m_dd   = field_2_t();
                m_hh   = field_2_t();
                m_mm   = field_2_t();
                m_ss   = field_2_t();
                m_fff  = field_3_t();
                m_timestamp = m_yyyy >> '-' >> m_mon >> '-' >> m_dd >> 'T' >> m_hh >> ':' >> m_mm >> ':' >> m_ss >> '.' >> m_fff >> 'Z';

                m_host_name = omit[ (char_('-')) ] | +~char_(' ');
                m_app_name = omit[ (char_('-')) ] | +~char_(' ');
                m_proc_id = omit[ (char_('-')) ] | +~char_(' ');
                m_msg_id = omit[ (char_('-')) ] | +~char_(' ');

                m_param_key = +(char_ - '=') >> '=';
                m_param_value = '\"' >> +(char_ - '\"') >> '\"';
                m_param = m_param_key >> m_param_value;
                m_param_list = m_param % ' ';

                m_structured_data_id  = +(char_ - '@') >> '@' >> int_ >> ' ';
                m_structured_data = '[' >> m_structured_data_id >> m_param_list >> ']';
                m_structured_data_list = +( m_structured_data ) | omit[ (char_('-') >> ' ') ];

                m_msg = +char_;

                m_start =  skip(space)[    '<'                          // begin prin
                                        >> m_priority                   // priority
                                        >> '>'                          // end pri
                                        >> m_version                    // version
                                        >> m_timestamp                  // timestamp
                                        >> m_host_name                  // host name
                                        >> m_app_name                   // app name
                                        >> m_proc_id                    // proc id
                                        >> m_msg_id                     // msg id
                                        >> m_structured_data_list       // sd
                                        >> m_msg                        // message
                                     ];

                BOOST_SPIRIT_DEBUG_NODES((m_start)
                                         (m_priority)
                                         (m_version)
                                         (m_yyyy)
                                         (m_mon)
                                         (m_dd)
                                         (m_hh)
                                         (m_mm)
                                         (m_ss)
                                         (m_fff)
                                         (m_timestamp)
                                         (m_host_name)
                                         (m_app_name)
                                         (m_proc_id)
                                         (m_msg_id)
                                         (m_param_key)
                                         (m_param_value)
                                         (m_param)
                                         (m_param_list)
                                         (m_structured_data_id)
                                         (m_structured_data)
                                         (m_structured_data_list)
                                         (m_msg))
            }
        };
    }
}

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


using message_t = event::Message;

TEST_CASE( "minimal message", "valid" )
{
    const boost::string_view data = R"(<165>1 2003-10-11T22:14:15.003Z - - - - - Message)";

    grammar::syslog::definitions<boost::string_view::const_iterator, message_t> gramar;

    auto f = begin(data);
    auto l = end(data);

    message_t event;

    const auto result = boost::spirit::qi::parse(f, l, gramar, event);

    INFO(event::to_string(event));

    REQUIRE( result );
    REQUIRE( f == l );
    REQUIRE( static_cast<int>(event.priority) == 165 );
    REQUIRE( static_cast<int>(event.version) == 1 );
    REQUIRE( event.get_facility() == 20 );
    REQUIRE( event.get_severity() == 5 );
    REQUIRE( event.timestamp == message_t::time_stamp_t{2003, 10, 11, 22, 14, 15, 0, 003} );
    REQUIRE( event.host_name.empty() );
    REQUIRE( event.app_name.empty() );
    REQUIRE( event.proc_id.empty() );
    REQUIRE( event.msg_id.empty() );
    REQUIRE( event.structured_data.size() == 0 );
    REQUIRE( event.message == "Message" );
}

TEST_CASE( "minimal message containing hostname", "valid" )
{
    const boost::string_view data = R"(<165>1 2003-10-11T22:14:15.003Z test.host.com - - - - Message)";

    grammar::syslog::definitions<boost::string_view::const_iterator, message_t> gramar;

    auto f = begin(data);
    auto l = end(data);

    message_t event;

    const auto result = boost::spirit::qi::parse(f, l, gramar, event);

    INFO(event::to_string(event));

    REQUIRE( result );
    REQUIRE( f == l );
    REQUIRE( static_cast<int>(event.priority) == 165 );
    REQUIRE( static_cast<int>(event.version) == 1 );
    REQUIRE( event.get_facility() == 20 );
    REQUIRE( event.get_severity() == 5 );
    REQUIRE( event.timestamp == message_t::time_stamp_t{2003, 10, 11, 22, 14, 15, 0, 003} );
    REQUIRE( event.host_name == "test.host.com" );
    REQUIRE( event.app_name.empty() );
    REQUIRE( event.proc_id.empty() );
    REQUIRE( event.msg_id.empty() );
    REQUIRE( event.structured_data.size() == 0 );
    REQUIRE( event.message == "Message" );
}

TEST_CASE( "minimal message containing app name", "valid" )
{
    const boost::string_view data = R"(<165>1 2003-10-11T22:14:15.003Z - MyApp - - - Message)";

    grammar::syslog::definitions<boost::string_view::const_iterator, message_t> gramar;

    auto f = begin(data);
    auto l = end(data);

    message_t event;

    const auto result = boost::spirit::qi::parse(f, l, gramar, event);

    INFO(event::to_string(event));

    REQUIRE( result );
    REQUIRE( f == l );
    REQUIRE( static_cast<int>(event.priority) == 165 );
    REQUIRE( static_cast<int>(event.version) == 1 );
    REQUIRE( event.get_facility() == 20 );
    REQUIRE( event.get_severity() == 5 );
    REQUIRE( event.timestamp == message_t::time_stamp_t{2003, 10, 11, 22, 14, 15, 0, 003} );
    REQUIRE( event.host_name.empty() );
    REQUIRE( event.app_name == "MyApp" );
    REQUIRE( event.proc_id.empty() );
    REQUIRE( event.msg_id.empty() );
    REQUIRE( event.structured_data.size() == 0 );
    REQUIRE( event.message == "Message" );
}

TEST_CASE( "minimal message containing process id", "valid" )
{
    const boost::string_view data = R"(<165>1 2003-10-11T22:14:15.003Z - - a2a - - Message)";

    grammar::syslog::definitions<boost::string_view::const_iterator, message_t> gramar;

    auto f = begin(data);
    auto l = end(data);

    message_t event;

    const auto result = boost::spirit::qi::parse(f, l, gramar, event);

    INFO(event::to_string(event));

    REQUIRE( result );
    REQUIRE( f == l );
    REQUIRE( static_cast<int>(event.priority) == 165 );
    REQUIRE( static_cast<int>(event.version) == 1 );
    REQUIRE( event.get_facility() == 20 );
    REQUIRE( event.get_severity() == 5 );
    REQUIRE( event.timestamp == message_t::time_stamp_t{2003, 10, 11, 22, 14, 15, 0, 003} );
    REQUIRE( event.host_name.empty() );
    REQUIRE( event.app_name.empty() );
    REQUIRE( event.proc_id == "a2a" );
    REQUIRE( event.msg_id.empty() );
    REQUIRE( event.structured_data.size() == 0 );
    REQUIRE( event.message == "Message" );
}

TEST_CASE( "minimal message containing message id", "valid" )
{
    const boost::string_view data = R"(<165>1 2003-10-11T22:14:15.003Z - - - acme:2 - Message)";

    grammar::syslog::definitions<boost::string_view::const_iterator, message_t> gramar;

    auto f = begin(data);
    auto l = end(data);

    message_t event;

    const auto result = boost::spirit::qi::parse(f, l, gramar, event);

    INFO(event::to_string(event));

    REQUIRE( result );
    REQUIRE( f == l );
    REQUIRE( static_cast<int>(event.priority) == 165 );
    REQUIRE( static_cast<int>(event.version) == 1 );
    REQUIRE( event.get_facility() == 20 );
    REQUIRE( event.get_severity() == 5 );
    REQUIRE( event.timestamp == message_t::time_stamp_t{2003, 10, 11, 22, 14, 15, 0, 003} );
    REQUIRE( event.host_name.empty() );
    REQUIRE( event.app_name.empty() );
    REQUIRE( event.proc_id.empty() );
    REQUIRE( event.msg_id == "acme:2" );
    REQUIRE( event.structured_data.size() == 0 );
    REQUIRE( event.message == "Message" );
}


TEST_CASE( "minimal message containing structured data", "" )
{
    const boost::string_view data = R"(<165>1 2003-10-11T22:14:15.003Z - - - - [a@1 index="1"] BOMAn application event log entry)";

    grammar::syslog::definitions<boost::string_view::const_iterator, message_t> gramar;

    auto f = begin(data);
    auto l = end(data);

    message_t event;

    const auto result = boost::spirit::qi::parse(f, l, gramar, event);

    INFO(event::to_string(event));

    REQUIRE( result );
    REQUIRE( f == l );
    REQUIRE( static_cast<int>(event.priority) == 165 );
    REQUIRE( static_cast<int>(event.version) == 1 );
    REQUIRE( event.get_facility() == 20 );
    REQUIRE( event.get_severity() == 5 );
    REQUIRE( event.timestamp == message_t::time_stamp_t{2003, 10, 11, 22, 14, 15, 0, 003} );
    REQUIRE( event.host_name.empty() );
    REQUIRE( event.app_name.empty() );
    REQUIRE( event.proc_id.empty() );
    REQUIRE( event.msg_id.empty() );
    REQUIRE( event.structured_data.size() == 1 );
    REQUIRE( event.message == "BOMAn application event log entry" );


}

TEST_CASE( "D", "" )
{
    const boost::string_view data = R"(<165>1 2003-10-11T22:14:15.003Z mymachine.example.com evntslog - ID47 [exampleSDID@32473 iut="3" eventSource="Application" eventID="1011"][exampleSDID@1223 iut="4" eventSource="Application" eventID="1011"] BOMAn application event log entry)";


    grammar::syslog::definitions<boost::string_view::const_iterator, message_t> gramar;

    const auto start = std::chrono::system_clock::now();
    const auto count = 100000;
    for (int x = 0; x < count; x++ )
    {
        auto f = begin(data);
        auto l = end(data);
        message_t event;
        boost::spirit::qi::parse(f, l, gramar, event);
        REQUIRE( event.structured_data.size() == 2 );
    }
    const auto finish = std::chrono::system_clock::now();

    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( finish - start );

    std::cout << "duration: " << duration.count() << "ms loop: " << (std::chrono::duration_cast<std::chrono::microseconds>(duration)/count).count() << "us" << std::endl;

}


