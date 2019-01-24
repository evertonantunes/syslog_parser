#pragma once

//#define BOOST_SPIRIT_DEBUG

#include <boost/spirit/include/qi_lexeme.hpp>
#include <boost/spirit/include/qi_match.hpp>
#include <boost/spirit/include/qi.hpp>

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
