/**
A tracer which outputs data in format capable to external tools analyzation

// TODO: redo all the messages with SybXML

*/
#ifndef SYB_XML2_JOURNAL_H
#define SYB_XML2_JOURNAL_H

#include "XmlWriter.h"

#define NJOURNAL

#ifdef NJOURNAL

#define JOURNAL1(type) {}

#define JOURNAL2(type,name) {}

#define JOURNAL(type,name,params) {}

#define JOURNAL_SCOPE(type,name,params) {}

#else

#define JOURNAL1(type)                       \
  syb::Journal::newMessage<type>("");        \
  syb::Journal::endMessage()

#define JOURNAL2(type, name)                 \
  syb::Journal::newMessage<type>(name);      \
  syb::Journal::endMessage()

#define JOURNAL(type, name, params)          \
  syb::Journal::newMessage<type>(name);      \
  params;                                    \
  syb::Journal::endMessage()

#define JOURNAL_SCOPE(type, name, params)    \
  JOURNAL(name + ".Enter",params);           \
  syb::JournalScope journalScope_(name)

#endif

namespace qx 
{
    namespace syb 
    {

      struct JournalOpts {
        static std::ostream& out();
      };

      template<typename T>
      struct JournalScope {
        inline explicit JournalScope(std::string const name):name_(name){}
        inline ~JournalScope() {
          JOURNAL2(T,name_ + ".Exit");
        }
      private:
        std::string name_;
      };


      struct Journal {


        static std::string getPostdate();

        template<typename T>
        inline static void newMessage(std::string const& name) {
          JournalOpts::out() << "<Event Name=\"" << TraceName<T>::value();
          if (!name.empty())
            JournalOpts::out() << "." << name;
          JournalOpts::out() << "\" At=\"" << getPostdate() 
            << "\" Source=\"server\">" << std::endl;
        }

        inline static void endMessage() {
          JournalOpts::out() << "</Event>" << std::endl;
        }


        static Journal& instance();
      };

      template<typename T>
      inline Journal const& logvar(std::string const& name, T const& v) {
          JournalOpts::out() 
            << "<Parameter Name=\"" << name << "\">" << toXml(v) << "</Parameter>";
          return Journal::instance();
      }

      template<typename T>
      inline Journal const& logvar(T const& v) {
          JournalOpts::out() 
            << "<Parameter Name=\"" << TypeName<T>::value() << "\">" 
            << toXml(v) << "</Parameter>" << std::endl;
          return Journal::instance();
      }

      inline Journal const& operator<<(Journal const&,Journal const& ) {}
    }
}


#endif
