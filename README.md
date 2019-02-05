[![Build Status](https://travis-ci.org/evertonantunes/syslog_parser.svg?branch=master)](https://travis-ci.org/evertonantunes/syslog_parser)

    conan install . --build=missing
    cmake CMakeLists.txt
    cmake --build . --targe all

 ```cpp
    #include <boost/utility/string_view.hpp>
    #include "grammar.hpp"
    #include "message.hpp"

    ...


    using message_t = event::Message;
    grammar::syslog::definitions<boost::string_view::const_iterator, message_t> gramar;
    
    const boost::string_view data = R"(<165>1 2003-10-11T22:14:15.003Z - MyApp - - - Message)";

    auto f = begin(data);
    auto l = end(data);

    message_t event;
    const auto result = boost::spirit::qi::parse(f, l, gramar, event);
    // result == true
    // event.message == "Message"
```
