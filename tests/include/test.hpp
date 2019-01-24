#pragma once

#include <catch2/catch.hpp>
#include <boost/fusion/adapted/struct.hpp>
#include <boost/utility/string_view.hpp>

#include "../../include/grammar.hpp"
#include "../../include/message.hpp"

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

//TEST_CASE( "D", "" )
//{
//    const boost::string_view data = R"(<165>1 2003-10-11T22:14:15.003Z mymachine.example.com evntslog - ID47 [exampleSDID@32473 iut="3" eventSource="Application" eventID="1011"][exampleSDID@1223 iut="4" eventSource="Application" eventID="1011"] BOMAn application event log entry)";


//    grammar::syslog::definitions<boost::string_view::const_iterator, message_t> gramar;

//    const auto start = std::chrono::system_clock::now();
//    const auto count = 100000;
//    for (int x = 0; x < count; x++ )
//    {
//        auto f = begin(data);
//        auto l = end(data);
//        message_t event;
//        boost::spirit::qi::parse(f, l, gramar, event);
//        REQUIRE( event.structured_data.size() == 2 );
//    }
//    const auto finish = std::chrono::system_clock::now();

//    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( finish - start );

//    std::cout << "duration: " << duration.count() << "ms loop: " << (std::chrono::duration_cast<std::chrono::microseconds>(duration)/count).count() << "us" << std::endl;

//}


